#include <HttpRequest.hpp>

#include <algorithm>
#include <cctype>

static std::string toLower(const std::string& value) {
	std::string lower = value;
	std::transform(lower.begin(), lower.end(), lower.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return lower;
}

static std::string trim(const std::string& value) {
	size_t start = value.find_first_not_of(" \t");
	if (start == std::string::npos)
		return "";
	size_t end = value.find_last_not_of(" \t");
	return value.substr(start, end - start + 1);
}

static bool tokenIsAllowed(const std::string& params) {
	size_t pos = 0;
	while (pos < params.size()) {
		size_t semicolon = params.find(';', pos);
		std::string param = params.substr(pos, semicolon - pos);
		size_t equals = param.find('=');
		if (equals != std::string::npos) {
			std::string name = toLower(trim(param.substr(0, equals)));
			std::string value = trim(param.substr(equals + 1));
			if (name == "q" && (value == "0" || value == "0.0" || value == "0.00" || value == "0.000"))
				return false;
		}
		if (semicolon == std::string::npos)
			break;
		pos = semicolon + 1;
	}
	return true;
}

HttpRequest::HttpRequest() : _port(-1), _contentLength(0), _parseError(false) {}

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
		_queryString = other._queryString;
		_userAgent = other._userAgent;
		_connection = other._connection;
		_body = other._body;
		_headers = other._headers;
		_parseError = other._parseError;
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

void HttpRequest::setQueryString(const std::string& queryString) {
	_queryString = queryString;
}

void HttpRequest::setVersion(const std::string& version) {
	_version = version;
}

void HttpRequest::setHeader(const std::string& key, const std::string& value) {
	_headers[toLower(key)] = value;
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

const std::string& HttpRequest::getQueryString() const {
	return _queryString;
}

std::string HttpRequest::getHeader(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator it = _headers.find(toLower(key));
	if (it == _headers.end())
		return "";
	return it->second;
}

bool HttpRequest::hasHeaderToken(const std::string& key, const std::string& token) const {
	std::string value = getHeader(key);
	std::string wanted = toLower(token);
	size_t pos = 0;

	while (pos < value.size()) {
		size_t comma = value.find(',', pos);
		std::string part = value.substr(pos, comma - pos);
		size_t semicolon = part.find(';');
		std::string name = toLower(trim(part.substr(0, semicolon)));

		if (name == wanted) {
			std::string params;
			if (semicolon != std::string::npos)
				params = part.substr(semicolon + 1);
			return tokenIsAllowed(params);
		}
		if (comma == std::string::npos)
			break;
		pos = comma + 1;
	}
	return false;
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const {
	return _headers;
}