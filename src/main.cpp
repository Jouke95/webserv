#include "Config.hpp"
#include "ConfigValidator.hpp"
#include "Server.hpp"

#include <iostream>
#include <stdexcept>

int main(int ac, char **av) {
	if (ac != 2) {
		std::cerr << "Usage: ./webserv [config file]\n";
		return 1;
	}

	try {
		Config config(av[1]);
		ConfigValidator configVal(config);
		Server server(config);

		server.start();
		server.run();
	} catch (const std::exception& e) {
		std::cerr << "Fatal error: " << e.what() << "\n";
		return 1;
	}
	return 0;
}
