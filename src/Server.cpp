#include "Server.hpp"

#include "RequestParser.hpp"
#include "ResponseBuilder.hpp"
#include "RequestHandler.hpp"
#include "RequestValidator.hpp"
#include "utils.hpp"
#include "CGI.hpp"

#include <cerrno>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(const Config& config) : _config(config) {}

Server::~Server() {}

void Server::start()
{
	if (_config.getServers().empty())
		throw std::runtime_error("no servers configured");

	for (size_t i = 0; i < _config.getServers().size(); i++) {

		const ServerConfig& server = _config.getServers()[i];

		int serverFD = socket(AF_INET, SOCK_STREAM, 0);
		if (serverFD < 0)
			throw std::runtime_error("socket() failed");
		fcntl(serverFD, F_SETFL, O_NONBLOCK);
		int opt = 1;
		setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		sockaddr_in addr = createAddress(server);

		if (bind(serverFD, (sockaddr*)&addr, sizeof(addr)) < 0) {
			std::cerr << "Warning: bind() failed on " 
					  << server.host << ":" << server.port 
					  << " - skipping" << std::endl;
			close(serverFD);
			continue;
		}

		if (listen(serverFD, 10) < 0)
			throw std::runtime_error("listen() failed");

		std::cout << "Server up, listening at " << server.host << ":" << server.port << std::endl;

		_connections.push_back(createConnection(serverFD, true, server.port));
	}

	if (_connections.empty())
		throw std::runtime_error("no servers could bind, exiting");
}

sockaddr_in Server::createAddress(const ServerConfig& server) {
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(server.port);

	return addr;
}

void Server::run()
{
	while (true) {
		pollConnections();
		for (size_t i = 0; i < _connections.size(); i++)
			handleConnection(i);
	}
}

void Server::handleConnection(size_t &i)
{
	Connection &conn = _connections[i];

	if (isTimedOut(conn)) {
		removeConnection(i);
		return;
	}

	if (isCGIClient(conn))
		if (!handleCGI(conn))
			return;

	if (isReadable(conn)) {
		if (conn.isServer)
			addConnection(conn.pfd.fd, conn.listeningPort);
		else if (!handleRequest(conn))
			removeConnection(i);
	}
	else if (isWritable(conn)) {
		bool done = sendResponse(conn);
		if (done)
			removeConnection(i);
	}
}

bool Server::isCGIClient(Connection& conn) {
	return conn.cgi != nullptr;
}

bool Server::handleCGI(Connection& conn) {
	if (conn.cgiWritePfd.revents & POLLOUT)
		conn.cgi->writeToCGI();
	if (conn.cgiReadPfd.revents & (POLLIN | POLLHUP))
		conn.cgi->readFromCGI();
	if (!conn.cgi->isDone())
		return false;
	else {
		HttpResponse response = conn.cgi->getResponse();
		if (conn.cgi->hasError()) {
			int errorCode = response.getStatusCode();
			const ServerConfig& server = findServer(conn.listeningPort);
			RequestHandler errorHandler(server.errorPages, errorCode);
			buildResponse(conn, errorHandler.getResponse());
		}
		else {
			buildResponse(conn, response);
		}
		delete conn.cgi;
		conn.cgi = nullptr;
		conn.cgiReadPfd.fd = -1;
		conn.cgiWritePfd.fd = -1;
	}
	return true;
}

bool Server::isTimedOut(Connection& conn) {
	return (!conn.isServer && time(NULL) - conn.timestamp > TIMEOUT);
}

bool Server::isReadable(Connection& conn) {
	return conn.pfd.revents & POLLIN;
}

bool Server::isWritable(Connection& conn) {
	return conn.pfd.revents & POLLOUT;
}

void Server::removeConnection(size_t &i) {
	close(_connections[i].pfd.fd);
	if (_connections[i].cgiReadPfd.fd != -1)
		close(_connections[i].cgiReadPfd.fd);
	if (_connections[i].cgiWritePfd.fd != -1)
		close(_connections[i].cgiWritePfd.fd);
	if (_connections[i].cgi != nullptr)
		delete _connections[i].cgi;
	_connections.erase(_connections.begin() + i);
	i--;
}

void Server::addConnection(int serverFD, int port) {
	int clientFD = acceptClient(serverFD);
	if (clientFD < 0)
		return;
	_connections.push_back(createConnection(clientFD, false, port));
}

int Server::acceptClient(int serverFD) {
	int clientFD = accept(serverFD, nullptr, nullptr);
	if (clientFD < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return -1;
		throw std::runtime_error("accept() failed");
	}
	fcntl(clientFD, F_SETFL, O_NONBLOCK);
	_numberOfConnections++;
	std::cout << "New connection (#" << _numberOfConnections << ")\n";
	return clientFD;
}

Server::Connection Server::createConnection(int fd, bool isServer, int listeningPort) {
	Connection conn;

	conn.pfd.fd = fd;
	conn.pfd.events = POLLIN;
	conn.pfd.revents = 0;

	conn.cgiReadPfd.fd = -1;
	conn.cgiReadPfd.events = 0;
	conn.cgiReadPfd.revents = 0;

	conn.cgiWritePfd.fd = -1;
	conn.cgiWritePfd.events = 0;
	conn.cgiWritePfd.revents = 0;

	conn.cgi = nullptr;
	conn.timestamp = time(NULL);
	conn.isServer = isServer;
	conn.listeningPort = listeningPort;

	return conn;
}

bool Server::handleRequest(Connection& conn) {
	if (!readFromClient(conn))
		return false;
	if (!isCompleteRequest(conn))
		return true;

	RequestParser parser(conn.client._request, conn.listeningPort);
	// parser.printRequest();
	
	const ServerConfig& server = findServer(conn.listeningPort);
	const LocationConfig& location = findLocation(server, parser.getRequest().getPath());

	RequestValidator requestValidator(parser.getRequest(), server, location);

	if (!requestValidator.isValid()) {
		RequestHandler errorHandler(server.errorPages, requestValidator.errorCode());
		buildResponse(conn, errorHandler.getResponse());
		return true;
	}

	if (isCGI(location, parser.getRequest().getPath())) {
		startCGI(conn, parser.getRequest(), location, server);
		if (conn.cgi->hasError()) {
			int errorCode = conn.cgi->getErrorCode();
			delete conn.cgi;
			conn.cgi = nullptr;
			RequestHandler errorHandler(server.errorPages, errorCode);
			buildResponse(conn, errorHandler.getResponse());
		}
		return true;
	}

	RequestHandler handler(parser.getRequest(), server.errorPages, location, server.maxBodySize);
	buildResponse(conn, handler.getResponse());
	return true;
}

bool Server::isCGI(const LocationConfig& location, const std::string& path) {
	if (location.cgiExtensions.empty()) {
		return false;
	}
	std::string extension = getExtension(path);
	for (size_t i = 0; i < location.cgiExtensions.size(); i++) {
		if (location.cgiExtensions[i] == extension)
			return true;
	}
	return false;
}

std::string Server::getExtension(const std::string& path) {
	size_t dot = path.rfind('.');
	size_t slash = path.rfind('/');

	if (dot == std::string::npos || (slash != std::string::npos && dot < slash))
		return "";
	return path.substr(dot);
}

void Server::startCGI(Connection& conn, const HttpRequest& request, const LocationConfig& location, const ServerConfig& server) {
	conn.cgi = new CGI(conn.cgiReadPfd, conn.cgiWritePfd, request, location, server);
	conn.pfd.events = 0;
}

bool Server::readFromClient(Connection& conn) {
	char buffer[BUFFER_SIZE] = {};
	int bytesRead = read(conn.pfd.fd, buffer, BUFFER_SIZE - 1);
	if (bytesRead <= 0)
		return false;

	conn.client._request.append(buffer, bytesRead);
	return true;
}

bool Server::isCompleteRequest(Connection& conn) {
	std::string& request = conn.client._request;

	size_t headerEnd = request.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return false;

	size_t pos = request.find("Content-Length");
	if (pos != std::string::npos) {
		pos += CONTENT_LENGTH_PREFIX;
		size_t lineEnd = request.find("\r\n", pos);
		std::string lengthStr = request.substr(pos, lineEnd - pos);
		int contentLength = strToInt(lengthStr);
		if (contentLength < 0)
			return true;	// invalid Content-Length, let the handler deal with it

		if (request.size() - (headerEnd + HEADER_END_LEN) < (size_t)contentLength)
			return false;
	}
	return true;
}

const ServerConfig& Server::findServer(int port) {
	for (size_t i = 0; i < _config.getServers().size(); i++) {
		if (port == _config.getServers()[i].port)
			return _config.getServers()[i];
	}
	throw std::runtime_error("No server config found for port " + std::to_string(port));
}

const LocationConfig& Server::findLocation(const ServerConfig& server, const std::string& requestPath) {
	size_t	bestMatchLen = 0;
	int		bestMatchIdx = 0;

	for (size_t i = 0; i < server.locations.size(); i++) {
		const std::string& locPath = server.locations[i].path;

		bool prefixMatch = requestPath.substr(0, locPath.size()) == locPath;
		bool exactOrSlash = (locPath.size() == requestPath.size() || requestPath[locPath.size()] == '/' || locPath.back() == '/');

		if (prefixMatch && exactOrSlash) {
			if (locPath.size() > bestMatchLen) {
				bestMatchLen = locPath.size();
				bestMatchIdx = i;
			}
		}
	}

	return server.locations[bestMatchIdx];
}

void Server::buildResponse(Connection& conn, const HttpResponse& response) {
	ResponseBuilder builder(response);
	conn.client._response = builder.build();

	std::cout << "Response: \n" << conn.client._response << std::endl;

	conn.pfd.events = POLLOUT;
}

bool Server::sendResponse(Connection &conn) {
	const std::string& response = conn.client._response;
	ssize_t& bytesSent = conn.client.bytesSent;

	ssize_t bytes = write(conn.pfd.fd, response.c_str() + bytesSent, response.size() - bytesSent);
	if (bytes == -1)
		return true;

	bytesSent += bytes;

	return bytesSent == (ssize_t)response.size();	// true = done
}

void Server::pollConnections() {
	// poll() requires a flat array, so we copy the pfds in and back out
	std::vector<pollfd> fds;

	for (size_t i = 0; i < _connections.size(); i++) {
		fds.push_back(_connections[i].pfd);
		fds.push_back(_connections[i].cgiReadPfd);
		fds.push_back(_connections[i].cgiWritePfd);
	}

	poll(fds.data(), fds.size(), 1000);

	for (size_t i = 0; i < _connections.size(); i++) {
		_connections[i].pfd.revents = fds[i*3].revents;
		_connections[i].cgiReadPfd.revents = fds[i*3+1].revents;
		_connections[i].cgiWritePfd.revents = fds[i*3+2].revents;
	}
}
