#include <cstdio>
#include <iostream>
#include <Server.hpp>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <Parser.hpp>

#include "endpoints/CommonGatewayInterface.hpp"

Server::Server() : _serverFileDescriptor(-1), _clientFileDescriptor(-1)
{
	
}

Server::~Server()
= default;

int Server::start()
{
	_serverFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

	if (_serverFileDescriptor < 0)
	{
		perror("socket");
		return 1;
	}
	
	int opt = 1;
	setsockopt(_serverFileDescriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	sockaddr_in addr{};
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port        = htons(PORT);
	
	if (bind(_serverFileDescriptor, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		return 1;
	}
	if (listen(_serverFileDescriptor, 10) < 0)
	{
		perror("listen");
		return 1;
	}
	std::cout << "Server up, listening at 127.0.0.1:" << PORT << std::endl;
	return 0;
}

int Server::waitForRequest()
{
	while (true)
	{
		_clientFileDescriptor = accept(_serverFileDescriptor, nullptr, nullptr);
		if (_clientFileDescriptor < 0)
		{
			perror("accept");
			continue;
		}
		std::cout << "Connection made.\n\n";
		char buf[BUFFER_SIZE] = {};
		read(this->_clientFileDescriptor, buf, BUFFER_SIZE - 1);

		std::string request = buf;
		
		std::cout << request << std::endl;
		
		Parser parser(request);

		parser.buildRequest();
		HttpRequest httpRequest = parser.getRequest();
		HttpResponse httpResponse = parser.getResponse();
		
		CommonGatewayInterface CGI(httpRequest, httpResponse);
		
		std::string response = parser.buildResponse(httpResponse);

		write(this->_clientFileDescriptor, response.c_str(), response.size());
		close(this->_clientFileDescriptor);
		std::cout << "Connection closed.\n";
	}
}
