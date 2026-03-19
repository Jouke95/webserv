#include "Parser.hpp"

#include <sstream>


Parser::Parser(const std::string& request)
{
	_request = request;
	buildPath();
	
}

Parser::~Parser()
{
	
}

void Parser::buildPath()
{
	size_t start = _request.find(' ') + 1;
	size_t end   = _request.find(' ', start);
	if (start == std::string::npos || end == std::string::npos)
		_path = "/";
	_path = _request.substr(start, end - start);
}

std::string Parser::createResponse() const
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
