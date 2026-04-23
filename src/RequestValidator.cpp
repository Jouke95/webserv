#include "RequestValidator.hpp"

RequestValidator::RequestValidator(const HttpRequest& request, const ServerConfig& server, const LocationConfig& location)
	: _request(request),
	  _server(server),
	  _location(location),
	  _isValid(true),
	  _errorCode(0)
{
	if (!versionCheck())
		return;
	if (!hostHeaderCheck())
		return;
	if (!isValidMethod())
		return;
	if (!contentLengthcheck())
		return;
}

bool RequestValidator::isValid() const {
	return _isValid;
}

int RequestValidator::errorCode() const {
	return _errorCode;
}

