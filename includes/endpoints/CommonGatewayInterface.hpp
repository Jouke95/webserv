#ifndef WEBSERV_COMMONGATEWAYINTERFACE_HPP
#define WEBSERV_COMMONGATEWAYINTERFACE_HPP

#include "../HttpRequest.hpp"
#include "../HttpResponse.hpp"

class CommonGatewayInterface {
	private:
		HttpRequest		_request;
		HttpResponse	_response;
	public:
		CommonGatewayInterface() = delete;
		CommonGatewayInterface(HttpRequest request, HttpResponse response);
		~CommonGatewayInterface();
		CommonGatewayInterface(const CommonGatewayInterface& other);
		CommonGatewayInterface operator=(const CommonGatewayInterface& other);

	HttpResponse Redirect();
};

#endif
