#ifndef CGI_HPP
#define CGI_HPP

#include "Config.hpp"
#include "HttpRequest.hpp"

#include <poll.h>

class HttpResponse;

class CGI {
	private:
		int _readFd1;
		int _writeFd1;
		int _readFd2;
		int _writeFd2;
		pid_t _pid;
		LocationConfig _location;
		HttpRequest _request;
		ServerConfig _server;
		std::string _output;
		ssize_t _bytesWritten;
		bool _isDone;
		bool _hasError;
		int _errorCode;
		std::string _scriptPath;

	public:
		CGI(struct pollfd& cgiReadPfd, struct pollfd& cgiWritePfd, const HttpRequest& request, const LocationConfig& location, const ServerConfig& server);
		~CGI();

		void initPipes();
		void setupChildProcess();
		void setupParent(pollfd& cgiReadPfd, pollfd& cgiWritePfd);

		void childProcess(const std::string& interpreter, const std::string& path, char **env);
		void writeToCGI();
		void readFromCGI();
		bool isDone();
		bool hasError();
		int getErrorCode();
		std::string getExtension(const std::string& path);
		std::string getInterpreter();
		void validateScript();
		std::string makePath(const std::string& root);
		char** setCGIEnv();
		void freeCGIEnv(char** env);
		void forkError();

		HttpResponse getResponse();
};

#endif
