#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <map>
#include <string>

class HttpResponse {
	private:
		int			_statusCode;
		std::string	_statusMessage;
		std::string	_version;
		std::string	_contentType;
		int			_contentLength;
		std::string	_body;
		std::map<std::string, std::string> _headers;

		static std::map<int, std::string> _statusMessages;

	public:
		HttpResponse();
		~HttpResponse();
		HttpResponse(const HttpResponse& other);
		HttpResponse& operator=(const HttpResponse& other);

		void		setStatusCode(int statusCode);
		void		setStatusMessage(const std::string& statusMessage);
		void		setContentType(const std::string& contentType);
		void		setBody(const std::string& body);
		void		setVersion(const std::string &version);
		void		setHeader(const std::string& key, const std::string& value);

		int			getStatusCode() const;
		std::string	getStatusMessage() const;
		std::string	getContentType() const;
		std::string	getBody() const;
		std::string	getVersion() const;
		const std::map<std::string, std::string>& getHeaders() const;

		static std::map<int, std::string> initStatusMessages();

};

#endif