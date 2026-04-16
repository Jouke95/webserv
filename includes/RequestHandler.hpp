#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Config.hpp"

class RequestHandler {
	private:
		HttpResponse _response;
		HttpRequest _request;
		LocationConfig _location;

	public:
		RequestHandler(const HttpRequest& request, const LocationConfig& location);
		~RequestHandler();

		void handle();
		void handleGet();
		void handlePost();
		void handleDelete();
		void handlePut();

		HttpResponse& getResponse();
		bool methodCheck();
		bool redirectCheck();
		std::string makePath() const;

		int giveErrorResponse(int code);
		static std::map<std::string, std::string> _mimeTypes;
		static std::map<std::string, std::string> initMimeTypes();
};

#endif
