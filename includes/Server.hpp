#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <poll.h>
#include <vector>
#include "Client.hpp"

#define PORT 8080
#define BUFFER_SIZE 10000

class Server {
	private:
		int	_serverFileDescriptor;

		struct Connection {
			struct pollfd pfd;
			Client client;
			bool isServer;
		};

		std::vector<Connection> _connections;

		// Methods
		void myPoll();
		void addConnection();
		bool handleRequest(Connection &conn);
		void sendResponse(Connection &conn);

	public:
		Server();
		~Server();
		Server(const Server& other) = delete;
		const Server& operator=(const Server& other) = delete;

		// Methods
		int start();
		int run();
};

#endif