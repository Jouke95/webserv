#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include "HttpRequest.hpp"

class RequestParser {
	private:
		HttpRequest _httpRequest;
		std::string _requestString;

		void parse();

	public:
		RequestParser() = delete;
		RequestParser(const std::string& request);
		~RequestParser();
		RequestParser(const RequestParser& other) = delete;
		RequestParser& operator=(const RequestParser& other) = delete;

		// Methods

		HttpRequest getRequest() const;
};

#endif
