#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include "HttpRequest.hpp"
#include "Config.hpp"
#include "HttpResponse.hpp"

class ResponseBuilder {
	private:
		HttpResponse _response;

	public:
		ResponseBuilder(const HttpResponse& response);
		~ResponseBuilder();

		std::string build();
};

#endif
