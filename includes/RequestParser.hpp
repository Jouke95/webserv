#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "HttpRequest.hpp"

#include <sstream>
#include <string>

class RequestParser {
	private:
		HttpRequest _httpRequest;
		std::string _requestString;
		int _listeningPort;

		void parse();
		void parseRequestLine(std::istringstream& stream);
		void parseHeaders(std::istringstream& stream);
		void parseBody(std::istringstream& stream);
		bool parseHeaderLine(const std::string& line, std::string& key, std::string& value);
		void parseHostLine(const std::string& value);

	public:
		RequestParser() = delete;
		RequestParser(const std::string& request, int port);
		~RequestParser();
		RequestParser(const RequestParser& other) = delete;
		RequestParser& operator=(const RequestParser& other) = delete;

		const HttpRequest& getRequest() const;

		void printRequest() const;
};

#endif
