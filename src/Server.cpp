#include <fcntl.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include "Server.hpp"
#include "Parser.hpp"

Server::Server(const Config& config) : _config(config) {}

Server::~Server() {}

void Server::start()
{
	for (size_t i = 0; i < _config.GetServer().size(); i++) {

		const ServerConfig& server = _config.GetServer()[i];

		int serverFD = socket(AF_INET, SOCK_STREAM, 0);
		if (serverFD < 0)
			throw std::runtime_error("socket() failed");
		fcntl(serverFD, F_SETFL, O_NONBLOCK);
		int opt = 1;
		setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		sockaddr_in addr = createAddress(server);

		if (bind(serverFD, (sockaddr*)&addr, sizeof(addr)) < 0)
			throw std::runtime_error("bind() failed");

		if (listen(serverFD, 10) < 0)
			throw std::runtime_error("listen() failed");

		std::cout << "Server up, listening at " << server.host << ":" << server.port << std::endl;

		_connections.push_back(createConnection(serverFD, true));
	}
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
		myPoll();
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
	if (isReadable(conn)) {
		if (conn.isServer)
			addConnection(conn.pfd.fd);
		else if (!handleRequest(conn))
			removeConnection(i);
	}
	else if (isWritable(conn)) {
		sendResponse(conn);
		removeConnection(i);
	}
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
	_connections.erase(_connections.begin() + i);
	i--;
}

void Server::addConnection(int serverFD) {
	int clientFD = acceptClient(serverFD);
	if (clientFD < 0)
		return;
	_connections.push_back(createConnection(clientFD, false));
}

int Server::acceptClient(int serverFD) {
	int clientFD = accept(serverFD, nullptr, nullptr);
	if (clientFD < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return -1;
		throw std::runtime_error("accept() failed");
	}
	std::cout << "Connection made.\n\n";
	fcntl(clientFD, F_SETFL, O_NONBLOCK);
	return clientFD;
}

Server::Connection Server::createConnection(int fd, bool isServer) {
	Connection conn;
	conn.pfd.fd = fd;
	conn.pfd.events = POLLIN;
	conn.pfd.revents = 0;
	conn.timestamp = time(NULL);
	conn.isServer = isServer;
	return conn;
}

bool Server::handleRequest(Connection& conn) {
	if (!readFromClient(conn))
		return false;
	if (!isCompleteRequest(conn))
		return true;
	buildResponse(conn);
	return true;
}

bool Server::readFromClient(Connection& conn) {
	char buffer[BUFFER_SIZE] = {};
	int bytesRead = read(conn.pfd.fd, buffer, BUFFER_SIZE - 1);
	if (bytesRead <= 0)
		return false;

	conn.client._request += buffer;
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
		size_t contentLength = stoi(request.substr(pos));
		if (request.size() - (headerEnd + HEADER_END_LEN) < contentLength)
			return false;
	}
	return true;
}

void Server::buildResponse(Connection& conn) {
	Parser parser(conn.client._request);
	conn.client._response = parser.buildResponseString();
	conn.pfd.events = POLLIN | POLLOUT;
}

void Server::sendResponse(Connection &conn) {
	const std::string& response = conn.client._response;
	write(conn.pfd.fd, response.c_str(), response.size());
	close(conn.pfd.fd);
}

void Server::myPoll() {
	std::vector<pollfd> fds;

	for (size_t i = 0; i < _connections.size(); i++)
		fds.push_back(_connections[i].pfd);

	poll(fds.data(), fds.size(), -1);

	for (size_t i = 0; i < _connections.size(); i++)
		_connections[i].pfd.revents = fds[i].revents;
}
