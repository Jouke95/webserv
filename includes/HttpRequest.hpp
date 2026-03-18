#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>

class HttpRequest
{
private:
	std::string _method;
	std::string _host;
	std::string _path;
	std::string _queryString;
	std::string _body;
	std::string _contentType;
	std::string _userAgent;
	std::string _connection;
	int         _contentLength;

public:
	HttpRequest();
	~HttpRequest();
	HttpRequest(const HttpRequest& other) = delete;
	const HttpRequest& operator=(const HttpRequest& other) = delete;
	
	void		setMethod();
	void		setHost();
	void		setPath();
	void		setQueryString();
	void		setBody();
	void		setContentType();
	void		setUserAgent();
	void		setConnection();
	void		setContentLength();

	std::string getMethod();
	std::string getHost();
	std::string getPath();
	std::string getQueryString();
	std::string getBody();
	std::string getContentType();
	std::string getUserAgent();
	std::string getConnection();
	int			getContentLength();
};

#endif