#include "RequestParser.hpp"
#include <iostream>
#include <sstream>

static void stripCR(std::string& line) {
	if (!line.empty() && line.back() == '\r')
		line.pop_back();
}

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

		if (key == "Host")
			parseHostLine(value);
		else if (key == "Content-Type")
			_httpRequest.setContentType(value);
		else if (key == "Content-Length")
			_httpRequest.setContentLength(std::stoi(value));
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
		port = stoi(line.substr(hostColon + 1));
	}

	if (host == "localhost")
		host = "127.0.0.1";

	_httpRequest.setHost(host);
	_httpRequest.setPort(port);
}

const HttpRequest& RequestParser::getRequest() const {
	return _httpRequest;
}
