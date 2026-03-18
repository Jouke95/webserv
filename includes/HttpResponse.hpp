#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>

class HttpResponse {
private:
	int         _statusCode;
	std::string _statusMessage;
	std::string _connection;
	std::string _contentType;
	std::string _body;
	int         _contentLength;

public:
	HttpResponse();
	~HttpResponse();
	HttpResponse(const HttpResponse& other) = delete;
	const HttpResponse& operator=(const HttpResponse& other) = delete;

	void		setStatusCode();
	void		setStatusMessage();
	void		setConnection();
	void		setContentType();
	void		setBody();
	void		setContentLength();

	int			getStatusCode();
	std::string getStatusMessage();
	std::string getConnection();
	std::string getContentType();
	std::string getBody();
	int			getContentLength();
};

#endif