#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <string>
#include <map>

struct LocationConfig {
	std::string					path;
	std::vector<std::string>	methods;
	std::string					root;
	std::string					index;
	bool						autoindex;
	std::string					upload_store;
	std::string					cgi_ext;
	std::string					cgi_path;
	int							redirect_code;
	std::string					redirect_page;

	LocationConfig() : autoindex(false), redirect_code(0){}
};

struct ServerConfig {
	std::string					host;
	int							port;
	int							max_body_size;
	std::map<int, std::string>	error_page;
	std::vector<LocationConfig>	location;

	ServerConfig() : port(0), max_body_size(0) {}
};

class Config {
private:
	std::vector<ServerConfig>	_servers;

	void parseServerBlock(std::ifstream& file, ServerConfig& server);
	void parseLocationBlock(std::ifstream& file, LocationConfig& location);

public:
	Config(const std::string& path);
	Config(const Config& other);
	Config& operator=(const Config& other);
	~Config();

	const std::vector<ServerConfig>& GetServer() const;
};

#endif