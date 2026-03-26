#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <string>
#include <vector>

struct LocationConfig {
	std::string					path;			// "/"
	std::vector<std::string>	methods;		// ["GET", "POST"]
	std::string					root;			// "var/www/html"
	std::string					index;			//"index.html"
	bool						autoindex;		// true/false
	std::string					uploadStore;	// "/var/www/uploads"
};

#endif