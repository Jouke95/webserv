#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <sys/types.h>

class Client {
	private:

	public:
		Client();
		~Client();

		std::string _response;
		std::string _request;
		ssize_t bytesSent;
};

#endif
