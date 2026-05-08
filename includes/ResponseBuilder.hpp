#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include "HttpResponse.hpp"

#include <string>

class ResponseBuilder {
	private:
		HttpResponse _response;
		std::string _sessionId;

	public:
		ResponseBuilder(const HttpResponse& response, std::string sessionId);
		~ResponseBuilder();

		std::string build();
};

#endif
