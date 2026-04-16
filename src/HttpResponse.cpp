#include <HttpResponse.hpp>

HttpResponse::HttpResponse() : _statusCode(0), _contentLength(0) {}

HttpResponse::~HttpResponse() {}

HttpResponse::HttpResponse(const HttpResponse& other) {
	*this = other;
}

HttpResponse& HttpResponse::operator=(const HttpResponse& other)
{
	if (this != &other) {
		_statusCode = other._statusCode;
		_body = other._body;
		_connection = other._connection;
		_contentLength = other._contentLength;
		_statusMessage = other._statusMessage;
		_contentType = other._contentType;
	}
	return *this;
}

void HttpResponse::setStatusCode(int statusCode) {
	std::map<int, std::string>::iterator it = _statusMessages.find(statusCode);
	if (it == _statusMessages.end()) {
		setStatusMessage("Unknown Status");
		return;
	}

	setStatusMessage(it->second);
}

void HttpResponse::setStatusMessage(const std::string& statusMessage) {
	_statusMessage = statusMessage;
}

void HttpResponse::setConnection(const std::string& connection) {
	_connection = connection;
}

void HttpResponse::setContentType(const std::string& contentType) {
	_contentType = contentType;
}

void HttpResponse::setBody(const std::string& body) {
	_body = body;
}

void HttpResponse::setVersion(const std::string& version) {
	_version = version;
}


void HttpResponse::setContentLength(int contentLength) {
	_contentLength = contentLength;
}

int HttpResponse::getStatusCode() const {
	return _statusCode;
}

std::string HttpResponse::getStatusMessage() const {
	return _statusMessage;
}

std::string HttpResponse::getConnection() const {
	return _connection;
}

std::string HttpResponse::getContentType() const {
	return _contentType;
}

std::string HttpResponse::getBody() const {
	return _body;
}

std::string HttpResponse::getVersion() const {
	return _version;
}

int HttpResponse::getContentLength() const {
	return _contentLength;
}

const std::map<std::string, std::string>& HttpResponse::getHeaders() const {
	return _headers;
}

std::map<int, std::string> HttpResponse::_statusMessages = HttpResponse::initStatusMessages();

std::map<int, std::string> HttpResponse::initStatusMessages() {
	std::map<int, std::string> m;

	m[200] = "OK";
	m[201] = "Created";
	m[202] = "Accepted";
	m[204] = "No Content";
	m[301] = "Moved Permanently";
	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[405] = "Method Not Allowed";
	m[500] = "Internal Server Error";
	m[502] = "Bad Gateway";
	m[504] = "Gateway Timeout";

	return m;
}
