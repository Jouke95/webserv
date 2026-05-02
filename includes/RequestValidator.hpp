#ifndef REQUESTVALIDATOR_HPP
#define REQUESTVALIDATOR_HPP

#include "Config.hpp"

#include <string>
#include <vector>

class HttpRequest;

class RequestValidator {
	private:
		const HttpRequest& _request;
		const ServerConfig& _server;
		const LocationConfig& _location;
		bool _isValid;
		int _errorCode;
	
		bool isValidVersion();
		bool isValidHostHeader();
		bool isValidPath();
		bool isValidMethod();
		bool isValidContentLength();
		bool isValidContentType();

		static std::vector<std::string> _knownMethods;
		static std::vector<std::string> _implementedMethods;

	public:
		RequestValidator(const HttpRequest& request, const ServerConfig& server, const LocationConfig& location);
		bool isValid() const;
		int errorCode() const;
};

#endif
