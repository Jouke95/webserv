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
	
	void		setMethod(const std::string& method);
	void		setHost(const std::string& host);
	void		setPath(const std::string& path);
	void		setQueryString(const std::string& query);
	void		setBody(const std::string& body);
	void		setContentType(const std::string& contentType);
	void		setUserAgent(const std::string& userAgent);
	void		setConnection(const std::string& connection);
	void		setContentLength(int contentLength);

	std::string getMethod();
	std::string getHost();
	std::string getPath();
	std::string getQueryString();
	std::string getBody();
	std::string getContentType();
	std::string getUserAgent();
	std::string getConnection();
	int			getContentLength() const;
};

#endif