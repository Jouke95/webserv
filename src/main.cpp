#include "Server.hpp"

int main(int ac, char **av)
{
	try
	{
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
