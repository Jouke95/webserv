#ifndef WEBSERV_TEXTENDPOINT_HPP
#define WEBSERV_TEXTENDPOINT_HPP

#include "AEndpoint.hpp"

class TextEndpoint : public AEndpoint {
	public:
		TextEndpoint(HttpRequest& request, HttpResponse& response) : AEndpoint(request, response) {}

		HttpResponse handle() override;

	private:
		int handleGet();
		int handlePost();
		int handleDelete();
};

#endif
