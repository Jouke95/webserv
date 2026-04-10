#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>

class HttpRequest {
	private:
		std::string	_method;
		std::string	_path;
		std::string	_version;
		std::string	_host;
		int			_port;
		std::string	_contentType;
		int			_contentLength;
		std::string	_userAgent;
		std::string	_connection;
		std::string	_body;

	public:
		HttpRequest();
		~HttpRequest();
		HttpRequest(const HttpRequest& other);
		HttpRequest& operator=(const HttpRequest& other);

		void		setMethod(const std::string& method);
		void		setPath(const std::string& path);
		void		setVersion(std::string& version);
		void		setHost(const std::string& host);
		void		setPort(const int port);
		void		setContentType(const std::string& contentType);
		void		setContentLength(int contentLength);
		void		setUserAgent(const std::string& userAgent);
		void		setConnection(const std::string& connection);
		void		setBody(const std::string& body);

		std::string	getMethod() const;
		std::string	getPath() const;
		std::string	getHost() const;
		int			getPort() const;
		std::string	getContentType() const;
		std::string	getVersion() const;
		int			getContentLength() const;
		std::string	getUserAgent() const;
		std::string	getConnection() const;
		std::string	getBody() const;
};

#endif
