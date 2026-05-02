#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include "HttpResponse.hpp"

#include <string>

class ResponseBuilder {
	private:
		HttpResponse _response;

	public:
		ResponseBuilder(const HttpResponse& response);
		~ResponseBuilder();

		std::string build();
};

#endif
