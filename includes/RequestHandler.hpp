#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Config.hpp"

class RequestHandler {
	private:
		HttpResponse _response;
		HttpRequest _request;
		std::map<int, std::string> _errorPages;
		LocationConfig _location;

		static std::map<std::string, std::string> _mimeTypes;
		static std::vector<std::string> _knownMethods;
		static std::vector<std::string> _implementedMethods;

		void handle();
		void handleGet();
		void handlePost();
		void handleDelete();

		bool isValidMethod();
		bool redirectCheck();
		std::string makePath(const std::string& base) const;
		void errorPage(int errorCode);
		void setFallbackError();
		void serveFile(const std::string& path);
		void handleDirectory(std::string path);
		std::string getContentType(const std::string& path);
		bool shouldGzipResponse() const;
		void applyGzip();

	public:
		RequestHandler(const HttpRequest& request,
					   const std::map<int, std::string>& errorPages,
					   const LocationConfig& location);
		~RequestHandler();

		HttpResponse getResponse() const;
};

#endif
