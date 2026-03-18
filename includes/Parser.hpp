#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>

class Parser
{
private:

	void buildPath();
	std::string _path;
	std::string _response;
	std::string _request;
	

public:
	Parser() = delete;
	Parser(const std::string& request);
	~Parser();
	Parser(const Parser& other) = delete;
	const Parser& operator=(const Parser& other) = delete;

	// Methods
	std::string createResponse() const;

	struct P
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