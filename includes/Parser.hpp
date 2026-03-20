#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class Parser
{
private:
	HttpRequest _request;
	HttpResponse _response;

	std::string _path;
	std::string _responseString;
	std::string _requestString;
	

public:
	Parser() = delete;
	Parser(const std::string& request);
	~Parser();
	Parser(const Parser& other) = delete;
	const Parser& operator=(const Parser& other) = delete;

	// Methods
	std::string buildResponseString() const;
	HttpRequest getRequest();
	HttpResponse getResponse();
	void buildRequest();

	void setResponse(HttpResponse response);
};

// Parsestruct:
// responseStatusCode
// responseStatusMessage
// responseContent-Type
// responseContent-Length
// responseConnection
// responseBody

// requestMethod
// requestHost
// requestUserAgent
// path = _path
// requestBody
// requestContentLength
// requestContentType
// requestQueryString
// requestConnection
// requestEncoding

#endif