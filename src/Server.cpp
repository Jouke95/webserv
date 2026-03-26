#include <vector>
#include <poll.h>
#include <cstdio>
#include <iostream>
#include <Server.hpp>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <Parser.hpp>

#include "endpoints/CommonGatewayInterface.hpp"

Server::Server() : _serverFileDescriptor(-1) {}

Server::~Server()
= default;

int Server::start()
{
	_serverFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

	if (_serverFileDescriptor < 0) {
		perror("socket");
		return 1;
	}
	
	int opt = 1;
	setsockopt(_serverFileDescriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	sockaddr_in addr{};
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port        = htons(PORT);
	
	if (bind(_serverFileDescriptor, (sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("bind");
		return 1;
	}

	if (listen(_serverFileDescriptor, 10) < 0) {
		perror("listen");
		return 1;
	}
	std::cout << "Server up, listening at 127.0.0.1:" << PORT << std::endl;
	return 0;
}

int Server::run()
{
	Connection serverConn;
	serverConn.pfd.fd = _serverFileDescriptor;
	serverConn.pfd.events = POLLIN;
	serverConn.pfd.revents = 0;
	serverConn.isServer = true;

	_connections.push_back(serverConn);

	while (true) {

		myPoll();

		for (size_t i = 0; i < _connections.size(); i++) {
			if (_connections[i].pfd.revents & POLLIN) {
				if (_connections[i].isServer)
					addConnection(_connections);
				else {
					if (handleRequest(_connections[i]) == false) {
						_connections.erase(_connections.begin() + i);
						i--;
					}
				}
			}
			else if (_connections[i].pfd.revents & POLLOUT) {
				sendResponse(_connections[i]);
				_connections.erase(_connections.begin() + i);
				i--;
			}
		}
	}
}

bool Server::handleRequest(Connection &conn) {
	char buf[BUFFER_SIZE] = {};
	int bytesRead = read(conn.pfd.fd, buf, BUFFER_SIZE - 1);
	std::cout << "Dit is de request:\n\n";
	std::cout << buf << std::endl;
	if (bytesRead <= 0) {
		close(conn.pfd.fd);
		return false;
	}
	Parser parser(buf);
	conn.client._response = parser.buildResponseString();
	std::cout << "dit is de response: \n\n\n";
	std::cout << conn.client._response << std::endl;
	conn.pfd.events = POLLIN | POLLOUT;
	return true;
}

void Server::sendResponse(Connection &conn) {
	write(conn.pfd.fd, conn.client._response.c_str(), conn.client._response.size());
	close(conn.pfd.fd);
}

void Server::myPoll() {
	std::vector<pollfd> fds;

	for (size_t i = 0; i < _connections.size(); i++)
		fds.push_back(_connections[i].pfd);

	poll(fds.data(), fds.size(), -1);

	for (size_t i = 0; i < _connections.size(); i++)
		_connections[i].pfd.revents = fds[i].revents;
}

void Server::addConnection(std::vector<Connection>& connections) {
	int _clientFileDescriptor = accept(_serverFileDescriptor, nullptr, nullptr);
	std::cout << "Connection made.\n\n";

	Connection newClient;

	newClient.pfd.fd = _clientFileDescriptor;
	newClient.pfd.events = POLLIN;
	newClient.pfd.revents = 0;
	newClient.client._response = "";
	newClient.isServer = false;

	connections.push_back(newClient);
}








// int poll(struct pollfd *fds, nfds_t nfds, int timeout);

// struct pollfd {
// 	int   fd;			// welke file descriptor?
// 	short events;		// wat wil IK monitoren?
// 	short revents;		// wat meldt het OS terug?
// };

// fds[0].fd		= listening_fd;		// de fd van socket()
// fds[0].events	= POLLIN;			// ik wil weten als er iets te lezen is
// fds[0].revents	= 0;				// dit vult het OS in, jij zet het op 0
