#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>

class Server
{
private:
	int			_serverFileDescriptor;
	int			_clientFileDescriptor;
	
public:
	Server();
	~Server();
	Server(const Server& other) = delete;
	const Server& operator=(const Server& other) = delete;

	// Methods
	int start();
	int waitForRequest();
	
};

#define PORT 8080
#define BUFFER_SIZE 10000

#endif