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
	std::string _version;
	int         _contentLength;

public:
	HttpRequest();
	~HttpRequest();
	HttpRequest(const HttpRequest& other);
	HttpRequest& operator=(const HttpRequest& other);
	
	void		setMethod(const std::string& method);
	void		setHost(const std::string& host);
	void		setPath(const std::string& path);
	void		setQueryString(const std::string& query);
	void		setBody(const std::string& body);
	void		setContentType(const std::string& contentType);
	void		setUserAgent(const std::string& userAgent);
	void		setConnection(const std::string& connection);
	void		setContentLength(int contentLength);
	void		setVersion(std::string& version);

	std::string getMethod() const;
	std::string getHost() const;
	std::string getPath() const;
	std::string getQueryString() const;
	std::string getBody() const;
	std::string getContentType() const;
	std::string getUserAgent() const;
	std::string getConnection() const;
	std::string getVersion() const;
	int			getContentLength() const;
};

#endif