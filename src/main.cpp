#include "Server.hpp"
#include "Config.hpp"
#include <iostream>

int main(int ac, char **av)
{
	try
	{
		if (ac != 2)
			throw std::runtime_error("Usage: ./webserv [config file]");
		Config config(av[1]);
		Server  server;

		server.start();
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Fatal error: " << e.what() << "\n";
		return 1;
	}
	return 0;
}
