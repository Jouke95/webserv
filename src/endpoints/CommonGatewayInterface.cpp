#include "endpoints/CommonGatewayInterface.hpp"

#include <sched.h>

#include "endpoints/AEndpoint.hpp"
#include "endpoints/TextEndpoint.hpp"

CommonGatewayInterface::CommonGatewayInterface(HttpRequest request, HttpResponse response) :
_request(request), _response(response)
{
	
}

CommonGatewayInterface::~CommonGatewayInterface()
{
}

CommonGatewayInterface::CommonGatewayInterface(const CommonGatewayInterface& other)
{
	*this = other;
}

CommonGatewayInterface CommonGatewayInterface::operator=(const CommonGatewayInterface& other)
{
	if (this != &other)
	{
		this->_request = other._request;
	}
	return (*this);
}

HttpResponse CommonGatewayInterface::Redirect()
{
	if (_request.getPath() == "/text")
	{
		TextEndpoint endpoint(_request, _response);
		return endpoint.handle();
	}
			
	return _response;
}

