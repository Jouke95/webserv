#include "Config.hpp"
#include "ConfigValidator.hpp"
#include "Server.hpp"

#include <iostream>
#include <stdexcept>
#include <csignal>

volatile sig_atomic_t g_running = 1;

void signalHandler(int sig){
	(void)sig;
	g_running = 0;
}

int main(int ac, char **av) {
	std::string configFile;
	if (ac == 1)
		configFile = "default.conf";
	else if (ac == 2)
		configFile = av[1];
	else {
		std::cerr << "Usage: ./webserv [config file]\n";
		return 1;
	}

	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	try {
		Config config(configFile);
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
