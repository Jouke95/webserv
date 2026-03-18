#include <HttpRequest.hpp>

HttpRequest::HttpRequest()
{
	_method = "";
	_host = "";
	_path = "";
	_queryString = "";
	_body = "";
	_contentType = "";
	_userAgent = "";
	_connection = "";
	_contentLength = 0;
}

HttpRequest::~HttpRequest()
{
	
}

