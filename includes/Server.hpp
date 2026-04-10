#ifndef SERVER_HPP
#define SERVER_HPP

#include <ctime>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <vector>
#include "Client.hpp"
#include "Config.hpp"
#include "HttpRequest.hpp"

class Server {
	private:
		static constexpr size_t CONTENT_LENGTH_PREFIX	= 16;
		static constexpr size_t HEADER_END_LEN			= 4;
		static constexpr size_t BUFFER_SIZE				= 10000;
		static constexpr long TIMEOUT					= 30;

		struct Connection {
			struct pollfd pfd;
			Client client;
			time_t timestamp;
			bool isServer;
		};

		std::vector<Connection> _connections;
		Config _config;

		// Methods
		void		myPoll();
		sockaddr_in	createAddress(const ServerConfig& server);
		void		handleConnection(size_t &i);
		bool		isTimedOut(Connection& conn);
		bool		isReadable(Connection& conn);
		bool		isWritable(Connection& conn);
		void		removeConnection(size_t &i);
		void		addConnection(int serverFD);
		int			acceptClient(int serverFD);
		Connection	createConnection(int fd, bool isServer);
		bool		handleRequest(Connection& conn);
		bool		sendResponse(Connection& conn);
		bool		isCompleteRequest(Connection& conn);
		bool		readFromClient(Connection& conn);
		const LocationConfig& findLocation(const HttpRequest& request);

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
