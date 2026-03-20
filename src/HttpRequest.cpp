
#include <HttpRequest.hpp>

HttpRequest::HttpRequest() : _contentLength(0)
{
}

HttpRequest::~HttpRequest()
= default;

HttpRequest::HttpRequest(const HttpRequest& other)
{
	*this = other;
}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	if (this != &other)
	{
		_body = other._body;
		_connection = other._connection;
		_contentLength = other._contentLength;
		_contentType = other._contentType;
		_host = other._host;
		_method = other._method;
		_path = other._path;
		_queryString = other._queryString;
		_userAgent = other._userAgent;
	}
	return (*this);
}


void HttpRequest::setMethod(const std::string& method)
{
	_method = method;
}

void HttpRequest::setHost(const std::string& host)
{
	_host = host;
}

void HttpRequest::setPath(const std::string& path)
{
	_path = path;
}

void HttpRequest::setQueryString(const std::string& query)
{
	_queryString = query;
}

void HttpRequest::setBody(const std::string& body)
{
	_body = body;
}

void HttpRequest::setContentType(const std::string& contentType)
{
	_contentType = contentType;
}

void HttpRequest::setUserAgent(const std::string& userAgent)
{
	_userAgent = userAgent;
}

void HttpRequest::setConnection(const std::string& connection)
{
	_connection = connection;
}

void HttpRequest::setContentLength(int contentLength)
{
	_contentLength = contentLength;
}

void HttpRequest::setVersion(std::string& version)
{
	_version = version;
}


std::string HttpRequest::getMethod() const
{
	return _method;
}

std::string HttpRequest::getHost() const
{
	return _host;
}

std::string HttpRequest::getPath() const
{
	return _path;
}

std::string HttpRequest::getQueryString() const
{
	return _queryString;
}

std::string HttpRequest::getBody() const
{
	return _body;
}

std::string HttpRequest::getContentType()  const
{
	return _contentType;
}

std::string HttpRequest::getUserAgent() const
{
	return _userAgent;
}

std::string HttpRequest::getConnection() const
{
	return _connection;
}

std::string HttpRequest::getVersion() const
{
	return _version;
}


int HttpRequest::getContentLength() const
{
	return _contentLength;
}