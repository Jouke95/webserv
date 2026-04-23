#ifndef REQUESTVALIDATOR_HPP
#define REQUESTVALIDATOR_HPP

#include "Config.hpp"
#include "HttpRequest.hpp"

class RequestValidator {
	private:
		bool _isValid;
		int _errorCode;

		const HttpRequest& _request;
		const ServerConfig& _server;
		const LocationConfig& _location;

	public:
		RequestValidator(const HttpRequest& request, const ServerConfig& server, const LocationConfig& location);
		bool isValid() const;
		int errorCode() const;


};

#endif
