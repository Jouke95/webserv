#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include "HttpRequest.hpp"
#include "Config.hpp"
#include "HttpResponse.hpp"

class ResponseBuilder {
	private:
		HttpRequest _request;
		LocationConfig _location;
		HttpResponse _response;

	public:
	ResponseBuilder(HttpRequest request, LocationConfig location);
	~ResponseBuilder();

	void handleGet();
	void handlePost();
	void handleDelete();

	// std::string& build();
};

#endif