#ifndef WEBSERV_AENDPOINT_HPP
#define WEBSERV_AENDPOINT_HPP

#include "../HttpRequest.hpp"
#include "../HttpResponse.hpp"

class AEndpoint
{
protected:
	HttpRequest  _request;
	HttpResponse _response;

public:
	AEndpoint(HttpRequest& request, HttpResponse& response) : _request(request), _response(response) {}
	virtual ~AEndpoint() = default;

	virtual HttpResponse handle() = 0;
};

#endif