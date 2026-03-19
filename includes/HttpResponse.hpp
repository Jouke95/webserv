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
	HttpResponse(const HttpResponse& other);
	HttpResponse& operator=(const HttpResponse& other);

	void		setStatusCode(int statusCode);
	void		setStatusMessage(const std::string& statusMessage);
	void		setConnection(const std::string& connection);
	void		setContentType(const std::string& contentType);
	void		setBody(const std::string& body);
	void		setContentLength(int contentLength);

	int			getStatusCode() const;
	std::string getStatusMessage();
	std::string getConnection();
	std::string getContentType();
	std::string getBody();
	int			getContentLength() const;
};

#endif