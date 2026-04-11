#include <HttpRequest.hpp>

HttpRequest::HttpRequest() : _contentLength(0) {}

HttpRequest::~HttpRequest()
= default;

HttpRequest::HttpRequest(const HttpRequest& other) {
	*this = other;
}

HttpRequest& HttpRequest::operator=(const HttpRequest& other) {
	if (this != &other) {
		_method = other._method;
		_path = other._path;
		_version = other._version;
		_host = other._host;
		_port = other._port;
		_contentType = other._contentType;
		_contentLength = other._contentLength;
		_userAgent = other._userAgent;
		_connection = other._connection;
		_body = other._body;
	}
	return (*this);
}

void HttpRequest::setMethod(const std::string& method) {
	_method = method;
}

void HttpRequest::setHost(const std::string& host) {
	_host = host;
}

void HttpRequest::setPort(const int port) {
	_port = port;
}

void HttpRequest::setPath(const std::string& path) {
	_path = path;
}

void HttpRequest::setBody(const std::string& body) {
	_body = body;
}

void HttpRequest::setContentType(const std::string& contentType) {
	_contentType = contentType;
}

void HttpRequest::setUserAgent(const std::string& userAgent) {
	_userAgent = userAgent;
}

void HttpRequest::setConnection(const std::string& connection) {
	_connection = connection;
}

void HttpRequest::setContentLength(int contentLength) {
	_contentLength = contentLength;
}

void HttpRequest::setVersion(const std::string& version) {
	_version = version;
}


const std::string& HttpRequest::getMethod() const {
	return _method;
}

const std::string& HttpRequest::getHost() const {
	return _host;
}

int HttpRequest::getPort() const {
	return _port;
}

const std::string& HttpRequest::getPath() const {
	return _path;
}

const std::string& HttpRequest::getBody() const {
	return _body;
}

const std::string& HttpRequest::getContentType()  const {
	return _contentType;
}

const std::string& HttpRequest::getUserAgent() const {
	return _userAgent;
}

const std::string& HttpRequest::getConnection() const {
	return _connection;
}

const std::string& HttpRequest::getVersion() const {
	return _version;
}

int HttpRequest::getContentLength() const {
	return _contentLength;
}
