#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Config.hpp"

#include <ctime>
#include <netinet/in.h>
#include <poll.h>
#include <string>
#include <vector>
#include <map>
#include <csignal>

extern volatile sig_atomic_t g_running;
class CGI;
class HttpRequest;
class HttpResponse;
class RequestParser;

class Server {
	private:
		static constexpr size_t CONTENT_LENGTH_PREFIX	= 16;
		static constexpr size_t HEADER_END_LEN			= 4;
		static constexpr size_t BUFFER_SIZE				= 10000;
		static constexpr long TIMEOUT					= 30;

		struct Connection {
			struct pollfd pfd;
			struct pollfd cgiReadPfd;
			struct pollfd cgiWritePfd;
			CGI* cgi;
			Client client;
			time_t timestamp;
			bool isServer;
			int listeningPort;
			std::string sessionId;
			std::string visitCount;
		};

		std::vector<Connection> _connections;
		Config _config;
		std::map<std::string, std::map<std::string, std::string>> _sessions;
		int _numberOfConnections = 0;

		// Methods
		void		pollConnections();
		sockaddr_in	createAddress(const ServerConfig& server);
		void		handleConnection(size_t &i);
		bool		isTimedOut(Connection& conn);
		bool		isReadable(Connection& conn);
		bool		isWritable(Connection& conn);
		void		removeConnection(size_t &i);
		void		addConnection(int serverFD, int port);
		int			acceptClient(int serverFD);
		Connection	createConnection(int fd, bool isServer, int port);
		bool		handleRequest(Connection& conn);
		bool		sendResponse(Connection& conn);
		bool		isCompleteRequest(Connection& conn);
		bool		readFromClient(Connection& conn);
		const ServerConfig&		findServer(int listeningPort);
		const LocationConfig&	findLocation(const ServerConfig& server, const std::string& path);
		void		buildResponse(Connection& conn, const HttpResponse& response);
		bool		isCGI(const LocationConfig& location, const std::string& path);
		void		startCGIRequest(Connection& conn, const HttpRequest& req, const LocationConfig& location, const ServerConfig& server);
		std::string	getExtension(const std::string& path);
		void 		startCGI(Connection& conn, const HttpRequest& request, const LocationConfig& location, const ServerConfig& server);
		bool		isCGIClient(Connection& conn);
		bool		handleCGI(Connection& conn);
		bool		validateRequest(Connection& conn, const HttpRequest& req, const ServerConfig& server, const LocationConfig& location);
		void		initCookieSession(Connection& conn, const RequestParser& parser);
		bool		isKnownSession(std::string cookieHeader);
		std::string	generateSessionId();
		void		countVisits(const std::string& sessionId);


	public:
		Server() = delete;
		Server(const Config& config);
		~Server();
		Server(const Server& other) = delete;
		Server& operator=(const Server& other) = delete;

		// Methods
		void	start();
		void	run();
};

#endif
