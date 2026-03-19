
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

std::string HttpRequest::getMethod()
{
	return _method;
}

std::string HttpRequest::getHost()
{
	return _host;
}

std::string HttpRequest::getPath()
{
	return _path;
}

std::string HttpRequest::getQueryString()
{
	return _queryString;
}

std::string HttpRequest::getBody()
{
	return _body;
}

std::string HttpRequest::getContentType()
{
	return _contentType;
}

std::string HttpRequest::getUserAgent()
{
	return _userAgent;
}

std::string HttpRequest::getConnection()
{
	return _connection;
}

int HttpRequest::getContentLength() const
{
	return _contentLength;
}