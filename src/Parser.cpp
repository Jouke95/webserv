#include "Parser.hpp"

#include <sstream>


Parser::Parser(const std::string& request)
{
	_requestString = request;
	buildPath();
}

Parser::~Parser()
= default;

void Parser::buildPath()
{
	size_t start = _request.find(' ') + 1;
	size_t end   = _request.find(' ', start);
	if (start == std::string::npos || end == std::string::npos)
		_path = "/";
	_path = _request.substr(start, end - start);
}

std::string Parser::buildResponse() const
{
	std::string body =
		"<!DOCTYPE html><html><body>"
		"<h1>Hello from C++! joe</h1>"
		"<p>Path: " + _path + "</p>"
		"</body></html>";

	std::ostringstream oss;
	oss << "HTTP/1.1 200 OK\r\n"
		<< "Content-Type: text/html\r\n"
		<< "Content-Length: " << body.size() << "\r\n"
		<< "Connection: close\r\n\r\n"
		<< body;
	return oss.str();
}

HttpRequest Parser::getRequest()
{
	return _request;
}

HttpResponse Parser::getResponse()
{
	return _response;
}

void Parser::buildRequest()
{
	std::istringstream stream(_requestString);
	std::string line;

	// First line
	if (std::getline(stream, line))
	{
		std::istringstream requestLine(line);
		std::string method, path, version;

		requestLine >> method >> path >> version;

		_request.setMethod(method);
		
		size_t queryPos = path.find('?');
		if (queryPos != std::string::npos)
		{
			_request.setQueryString(path.substr(queryPos + 1));
			_request.setPath(path.substr(0, queryPos));
		}
		else
			_request.setPath(path);
		_request.setVersion(version);
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
		if      (key == "Host")           _request.setHost(value);
		else if (key == "Content-Type")   _request.setContentType(value);
		else if (key == "Content-Length") _request.setContentLength(std::stoi(value));
		else if (key == "User-Agent")     _request.setUserAgent(value);
		else if (key == "Connection")     _request.setConnection(value);
	}

	std::string body;
	std::getline(stream, body, '\0');
	if (!body.empty() && body.back() == '\r')
		body.pop_back();
	_request.setBody(body);
}
