#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include "HttpResponse.hpp"

#include <string>

class ResponseBuilder {
	private:
		HttpResponse _response;
		std::string _sessionId;
		std::string _visitCount;

	public:
		ResponseBuilder(const HttpResponse& response, std::string sessionId, std::string visitCount);
		~ResponseBuilder();

		std::string build();
};

#endif
