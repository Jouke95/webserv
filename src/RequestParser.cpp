#include "RequestParser.hpp"
#include "utils.hpp"
#include <iostream>
#include <sstream>

RequestParser::RequestParser(const std::string& request) : _requestString(request) {
	parse();
}

RequestParser::~RequestParser() {}

void RequestParser::parse() {
	std::istringstream stream(_requestString);

	parseRequestLine(stream);
	parseHeaders(stream);
	parseBody(stream);
}

void RequestParser::parseRequestLine(std::istringstream& stream) {
	std::string line;

	if (std::getline(stream, line)) {
		stripCR(line);
		std::istringstream requestLine(line);
		std::string method, path, version;

		requestLine >> method >> path >> version;

		_httpRequest.setMethod(method);
		_httpRequest.setPath(path);
		_httpRequest.setVersion(version);
	}
}

void RequestParser::parseHeaders(std::istringstream& stream) {
	std::string line;

	while (std::getline(stream, line)) {
		stripCR(line);
		if (line.empty())
			break;

		std::string key, value;
		if (!parseHeaderLine(line, key, value))
			continue;

		_httpRequest.setHeader(key, value);
		if (key == "Host")
			parseHostLine(value);
		else if (key == "Content-Type")
			_httpRequest.setContentType(value);
		else if (key == "Content-Length")
			_httpRequest.setContentLength(strToInt(value));
		else if (key == "User-Agent")
			_httpRequest.setUserAgent(value);
		else if (key == "Connection")
			_httpRequest.setConnection(value);
	}
}

void RequestParser::parseBody(std::istringstream& stream) {
	std::string body(std::istreambuf_iterator<char>(stream), {});
	_httpRequest.setBody(body);
}

bool RequestParser::parseHeaderLine(const std::string& line, std::string& key, std::string& value) {
	size_t colonPos = line.find(':');
	if (colonPos == std::string::npos)
		return false;

	key = line.substr(0, colonPos);
	value = line.substr(colonPos + 1);
	size_t start = value.find_first_not_of(" \t");
	if (start == std::string::npos)
		return false;
	value = value.substr(start);

	return true;
}

void RequestParser::parseHostLine(const std::string& line) {
	std::string host;
	int port;

	size_t hostColon = line.find(':');
	if (hostColon == std::string::npos) {
		host = line;
		port = 80;
	}
	else {
		host = line.substr(0, hostColon);
		port = strToInt(line.substr(hostColon + 1));
	}

	if (host == "localhost")
		host = "127.0.0.1";

	_httpRequest.setHost(host);
	_httpRequest.setPort(port);
}

const HttpRequest& RequestParser::getRequest() const {
	return _httpRequest;
}

void RequestParser::printRequest() const {
	const HttpRequest& req = _httpRequest;

	std::cout << "=== HTTP Request ===\n";
	std::cout << "Method:         " << req.getMethod() << "\n";
	std::cout << "Path:           " << req.getPath() << "\n";
	std::cout << "Version:        " << req.getVersion() << "\n";
	std::cout << "Host:           " << req.getHost() << "\n";
	std::cout << "Port:           " << req.getPort() << "\n";
	std::cout << "Content-Type:   " << req.getContentType() << "\n";
	std::cout << "Content-Length: " << req.getContentLength() << "\n";
	std::cout << "User-Agent:     " << req.getUserAgent() << "\n";
	std::cout << "Connection:     " << req.getConnection() << "\n";

	std::cout << "--- All Headers ---\n";
	const std::map<std::string, std::string>& headers = req.getHeaders();
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		std::cout << "  " << it->first << ": " << it->second << "\n";

	std::cout << "--- Body ---\n";
	const std::string& body = req.getBody();
	if (body.empty())
		std::cout << "  (empty)\n";
	else
		std::cout << body << "\n";

	std::cout << "====================\n";
}
