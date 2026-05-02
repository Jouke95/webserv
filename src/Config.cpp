#include "Config.hpp"
#include "utils.hpp"

#include <climits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

Config::Config(const std::string& path) {
	std::string line;
	std::ifstream file(path);

	if (!file.is_open())
		throw std::runtime_error("Could not open config file: " + path);

	while (std::getline(file, line)) {
		if (line.find("server {") != std::string::npos) {
			ServerConfig server;
			parseServerBlock(file, server);
			_servers.push_back(server);
		}
	}
}

Config::Config(const Config& other) {
	*this = other;
}

Config& Config::operator=(const Config& other) {
	if (this != &other)
		_servers = other._servers;
	return *this;
}

Config::~Config() {}

void Config::parseServerBlock(std::ifstream& file, ServerConfig& server) {
	std::string line;
	while (std::getline(file, line)) {
		if (line.find("}") != std::string::npos)
			return ;
		std::istringstream iss(line);
		std::string key;
		iss >> key;
		if (key == "host") {
			std::string host;
			iss >> host;
			stripSemicolon(host);
			server.host = host;
		}
		else if (key == "port") {
			std::string port;
			iss >> port;
			stripSemicolon(port);
			server.port = strToInt(port);
		}
		else if (key == "max_body_size") {
			std::string size;
			iss >> size;
			stripSemicolon(size);
			server.maxBodySize = strToSizeT(size);
		}
		else if (key == "error_page") {
			std::string code, page;
			iss >> code >> page;
			stripSemicolon(page);
			server.errorPages[strToInt(code)] = page;
		}
		else if (key == "location") {
			LocationConfig location;
			iss >> location.path;
			parseLocationBlock(file, location);
			server.locations.push_back(location);
		}
	}
}

void Config::parseLocationBlock(std::ifstream& file, LocationConfig& location) {
	std::string line, key;
	while (std::getline(file, line)) {
		if (line.find("}") != std::string::npos)
			return ;
		std::istringstream iss(line);
		iss >> key;
		if (key == "methods") {
			std::string method;
			while (iss >> method) {
				stripSemicolon(method);
				location.methods.push_back(method);
			}
		}
		else if (key == "root") {
			std::string path;
			iss >> path;
			stripSemicolon(path);
			location.root = makeAbsolute(path);
		}
		else if (key == "index") {
			std::string index;
			iss >> index;
			stripSemicolon(index);
			location.index = index;
		}
		else if (key == "autoindex") {
			std::string value;
			iss >> value;
			stripSemicolon(value);
			location.autoindex = (value == "on");
		}
		else if (key == "upload_store") {
			std::string path;
			iss >> path;
			stripSemicolon(path);
			location.uploadStore = makeAbsolute(path);
		}
		else if (key == "cgi") {
			std::string extension, cgiPath;
			iss >> extension >> cgiPath;
			stripSemicolon(cgiPath);
			location.cgiExtensions.push_back(extension);
			location.cgiPaths.push_back(makeAbsolute(cgiPath));
		}
		else if (key == "redirect") {
			std::string code, page;
			iss >> code >> page;
			stripSemicolon(page);
			location.redirectCode = strToInt(code);
			location.redirectPage = page;
		}
	}
}

const std::vector<ServerConfig>& Config::getServers() const {
	return _servers;
}
