#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <fstream>
#include <map>
#include <string>
#include <vector>

struct LocationConfig {
	std::string path;
	std::vector<std::string> methods;
	std::string root;
	std::string index;
	bool autoindex;
	std::string uploadStore;
	std::vector<std::string> cgiExtensions;
	std::vector<std::string> cgiPaths;
	int redirectCode;
	std::string redirectPage;

	LocationConfig() : autoindex(false), redirectCode(0) {}
};

struct ServerConfig {
	std::string host;
	std::string serverName;
	int port;
	size_t maxBodySize;
	std::map<int, std::string> errorPages;
	std::vector<LocationConfig> locations;

	ServerConfig() : port(0), maxBodySize(0) {}
};

class Config {
	private:
		std::vector<ServerConfig> _servers;

		void parseServerBlock(std::ifstream& file, ServerConfig& server);
		void parseLocationBlock(std::ifstream& file, LocationConfig& location);

	public:
		Config(const std::string& path);
		Config(const Config& other);
		Config& operator=(const Config& other);
		~Config();

		const std::vector<ServerConfig>& getServers() const;
};

#endif