#include "RequestValidator.hpp"
#include "HttpRequest.hpp"
#include "utils.hpp"

#include <iostream>

RequestValidator::RequestValidator(const HttpRequest& request, const ServerConfig& server, const LocationConfig& location)
	: _request(request),
	  _server(server),
	  _location(location),
	  _isValid(true),
	  _errorCode(0)
{
	if (!isValidVersion())
		return;
	if (!isValidHostHeader())
		return;
	if (!isValidPath())
		return;
	if (!isValidMethod())
		return;
	if (_request.getMethod() == "POST") {
		if (!isValidContentLength())
			return;
		if (!isValidContentType())
			return;
	}
}

bool RequestValidator::isValid() const {
	return _isValid;
}

int RequestValidator::errorCode() const {
	return _errorCode;
}

bool RequestValidator::isValidVersion() {
	if (_request.getVersion() != "HTTP/1.1") {
		_errorCode = 505; // HTTP Version Not Supported
		_isValid = false;
		return false;
	}
	return true;
}

bool RequestValidator::isValidHostHeader() {
	if (_request.getHost().empty()) {
		_errorCode = 400;
		_isValid = false;
		return false;
	}
	if (_request.getPort() < 0 || _request.getPort() > 65535) {
		_errorCode = 400;
		_isValid = false;
		return false;
	}
	return true;
}

bool RequestValidator::isValidPath() {
	const std::string& path = _request.getPath();
	if (path.empty() || path[0] != '/') {
		_errorCode = 400;
		_isValid = false;
		return false;
	}
	return true;
}

bool RequestValidator::isValidMethod() {
	std::string method = _request.getMethod();

	if (!vectorContains(_knownMethods, method)) {
		_errorCode = 400;
		_isValid = false;
		return false;
	}
	if (!vectorContains(_implementedMethods, method)) {
		_errorCode = 501;
		_isValid = false;
		return false;
	}
	if (!vectorContains(_location.methods, method)) {
		_errorCode = 405;
		_isValid = false;
		return false;
	}
	return true;
}

bool RequestValidator::isValidContentLength() {
	int contentLength = _request.getContentLength();
	bool hasBody = !_request.getBody().empty();

	if (contentLength < 0 && hasBody) {
		_errorCode = 400;
		_isValid = false;
		return false;
	}

	if (contentLength >= 0 && (size_t)contentLength > _server.maxBodySize) {
		_errorCode = 413;
		_isValid = false;
		return false;
	}
	return true;
}

bool RequestValidator::isValidContentType() {
	if (_request.getBody().empty())
		return true;
	if (_request.getContentType().empty()) {
		_errorCode = 400;
		_isValid = false;
		return false;
	}
	return true;
}

std::vector<std::string> RequestValidator::_knownMethods = {
	"GET", "HEAD", "OPTIONS", "TRACE",
	"PUT", "DELETE", "POST", "PATCH", "CONNECT"
};

std::vector<std::string> RequestValidator::_implementedMethods = {
	"GET", "POST", "DELETE"
};
