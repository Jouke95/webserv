#include <HttpResponse.hpp>

HttpResponse::HttpResponse() : _statusCode(0), _contentLength(0)
{
}

HttpResponse::~HttpResponse()
{
}

HttpResponse::HttpResponse(const HttpResponse& other)
{
	*this = other;
}

HttpResponse& HttpResponse::operator=(const HttpResponse& other)
{
	if (this != &other)
	{
		_statusCode = other._statusCode;
		_body = other._body;
		_connection = other._connection;
		_contentLength = other._contentLength;
		_statusMessage = other._statusMessage;
		_contentType = other._contentType;
	}
	return (*this);
}


void HttpResponse::setStatusCode(int statusCode)
{
	_statusCode = statusCode;
}

void HttpResponse::setStatusMessage(const std::string& statusMessage)
{
	_statusMessage = statusMessage;
}

void HttpResponse::setConnection(const std::string& connection)
{
	_connection = connection;
}

void HttpResponse::setContentType(const std::string& contentType)
{
	_contentType = contentType;
}

void HttpResponse::setBody(const std::string& body)
{
	_body = body;
}

void HttpResponse::setVersion(const std::string& version)
{
	_version = version;
}


void HttpResponse::setContentLength(int contentLength)
{
	_contentLength = contentLength;
}

int HttpResponse::getStatusCode() const
{
	return _statusCode;
}

std::string HttpResponse::getStatusMessage()
{
	return _statusMessage;
}

std::string HttpResponse::getConnection()
{
	return _connection;
}

std::string HttpResponse::getContentType()
{
	return _contentType;
}

std::string HttpResponse::getBody()
{
	return _body;
}

std::string HttpResponse::getVersion()
{
	return _version;
}


int HttpResponse::getContentLength() const
{
	return _contentLength;
}