#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <map>
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
		std::map<std::string, std::string> _headers;

	public:
		HttpRequest();
		~HttpRequest();
		HttpRequest(const HttpRequest& other);
		HttpRequest& operator=(const HttpRequest& other);

		void		setMethod(const std::string& method);
		void		setPath(const std::string& path);
		void		setVersion(const std::string& version);
		void		setHost(const std::string& host);
		void		setPort(const int port);
		void		setContentType(const std::string& contentType);
		void		setContentLength(int contentLength);
		void		setUserAgent(const std::string& userAgent);
		void		setConnection(const std::string& connection);
		void		setBody(const std::string& body);
		void		setHeader(const std::string& key, const std::string& value);

		const std::string&	getMethod() const;
		const std::string&	getPath() const;
		const std::string&	getHost() const;
		int					getPort() const;
		const std::string&	getContentType() const;
		const std::string&	getVersion() const;
		int					getContentLength() const;
		const std::string&	getUserAgent() const;
		const std::string&	getConnection() const;
		const std::string&	getBody() const;
		std::string			getHeader(const std::string& key) const;
		bool				hasHeaderToken(const std::string& key, const std::string& token) const;
};

#endif
