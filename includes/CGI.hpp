#ifndef CGI_HPP
#define CGI_HPP

#include "HttpRequest.hpp"
#include "Config.hpp"

class CGI {
	private:

	public:
		CGI(struct pollfd& cgiReadPfd, struct pollfd& cgiWritePfd, const HttpRequest& request, const LocationConfig& location);
		void write();
		void read();
		bool isDone();
		bool hasError();
		int getErrorCode();

		HttpResponse getResponse();
};

#endif
