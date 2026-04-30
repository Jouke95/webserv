#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Config.hpp"
#include <dirent.h>

class RequestHandler {
	private:
		HttpResponse _response;
		HttpRequest _request;
		std::map<int, std::string> _errorPages;
		LocationConfig _location;
		size_t _maxBodySize;

		static std::map<std::string, std::string> _mimeTypes;

		void handle();
		void handleGet();
		void handlePost();
		void handleDelete();

		bool redirectCheck();
		bool decodeRequestBody();
		std::string makePath(const std::string& base) const;
		void errorPage(int errorCode);
		void setFallbackError();
		void serveFile(const std::string& path);
		void handleDirectory(std::string path);
		void serveDirList(std::string path);
		void readDirectory(DIR* dir, std::vector<std::string>& dirs, std::vector<std::string>& files);
		std::string buildDirListHtml(const std::string& requestPath, const std::vector<std::string>& dirs, const std::vector<std::string>& files);
		std::string getContentType(const std::string& path);
		bool shouldGzipResponse() const;
		void applyGzip();
		bool isCGI(std::string path);
		void handleCGI();
		void executeCGI(std::string interpreter);
		void setCGIEnv(const HttpRequest& req, const LocationConfig& location, const std::string& scriptPath);

	public:
		RequestHandler(const HttpRequest& request,
					   const std::map<int, std::string>& errorPages,
					   const LocationConfig& location,
					   size_t maxBodySize = 0);
		RequestHandler(const std::map<int, std::string>& errorPages, int errorCode);
		~RequestHandler();

		HttpResponse getResponse() const;
};

#endif
