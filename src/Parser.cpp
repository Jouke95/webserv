#include "Parser.hpp"
#include "endpoints/CommonGatewayInterface.hpp"

#include <iostream>
#include <sstream>


Parser::Parser(const std::string& request)
{
	_requestString = request;

	buildRequest();
	HttpRequest httpRequest = getRequest();
	HttpResponse httpResponse = getResponse();

	std::cout << "Request Method: " << httpRequest.getMethod() << std::endl;
	CommonGatewayInterface CGI(httpRequest, httpResponse);

	httpResponse = CGI.Redirect();
		
	std::cout << "Response: " << httpResponse.getStatusMessage() << std::endl;
	setResponse(httpResponse);
}

Parser::~Parser()
= default;

std::string Parser::buildResponseString() const
{
	std::ostringstream oss;
	if (_request.getMethod() == "OPTIONS")
	{
		oss << "HTTP/1.1 200 OK\r\n"
			<< "Access-Control-Allow-Origin: *\r\n"
			<< "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
			<< "Access-Control-Allow-Headers: Content-Type\r\n"
			<< "Content-Length: 0\r\n"
			<< "Connection: close\r\n\r\n";
		return oss.str();
	}
	
	oss << _request.getVersion() << " " << _response.getStatusCode() << " " << _response.getStatusMessage() << "\r\n"
		<< "Server: webserv/1.0\r\n"
		<< "Access-Control-Allow-Origin: *\r\n"
		<< "Content-Type: " << _response.getContentType() << "\r\n"
		<< "Content-Length: " << _response.getContentLength() << "\r\n"
		<< "Connection: close\r\n\r\n"
		<< _response.getBody();

	std::cout << "Response String: " << oss.str() << std::endl;
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

void Parser::setResponse(HttpResponse response)
{
	_response = response;
	_response.setContentLength(response.getBody().length());
}

