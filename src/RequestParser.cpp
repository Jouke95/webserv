#include "RequestParser.hpp"
#include <iostream>
#include <sstream>

RequestParser::RequestParser(const std::string& request) : _requestString(request) {
	parse();
}

RequestParser::~RequestParser() {}

void RequestParser::parse() {
	std::istringstream stream(_requestString);
	std::string line;

	// First line
	if (std::getline(stream, line))
	{
		std::istringstream requestLine(line);
		std::string method, path, version;

		requestLine >> method >> path >> version;

		_httpRequest.setMethod(method);
		_httpRequest.setPath(path);
		_httpRequest.setVersion(version);
	}

	// Headers
	while (std::getline(stream, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		if (line.empty())
			break;

		size_t colonPos = line.find(':');
		if (colonPos == std::string::npos)
			continue;

		std::string key   = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 2);
		if (key == "Host") {
			size_t hostColon = value.find(':');
			std::string host = value.substr(0, hostColon);
			int port = stoi(value.substr(hostColon + 1));
			if (host == "localhost")
				host = "127.0.0.1";
			_httpRequest.setHost(host);
			_httpRequest.setPort(port);
		}
		else if (key == "Content-Type")		_httpRequest.setContentType(value);
		else if (key == "Content-Length")	_httpRequest.setContentLength(std::stoi(value));
		else if (key == "User-Agent")		_httpRequest.setUserAgent(value);
		else if (key == "Connection")		_httpRequest.setConnection(value);
	}

	// Body
	size_t headerEnd = _requestString.find("\r\n\r\n");
	if (headerEnd != std::string::npos)
	{
		std::string body = _requestString.substr(headerEnd + 4);
		_httpRequest.setBody(body);
	}
}

HttpRequest RequestParser::getRequest() const {
	return _httpRequest;
}
