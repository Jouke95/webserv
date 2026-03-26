#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <map>
#include <vector>
#include "LocationConfig.hpp"

struct ServerConfig {
	std::string					host;        // "127.0.0.1"
	int							port;        // 8080
	size_t						maxBodySize; // 1000000
	std::map<int, std::string>	errorPages;  // {404: "/errors/404.html"}
	std::vector<LocationConfig>	locations;
};

#endif