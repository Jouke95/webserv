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
};

#endif