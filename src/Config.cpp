#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <unistd.h>

Config::Config(const std::string& path) {
	std::string content;
	std::ifstream file(path);

	if (!file.is_open())
		throw std::runtime_error("Could not open config file: " + path);

	while (std::getline(file, content)) {
		if (content.find("server {") != std::string::npos) {
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

const std::vector<ServerConfig>& Config::GetServer() const {
	return this->_servers;
}

static void stripSemicolon(std::string& value) {
	if (!value.empty() && value.back() == ';')
		value.pop_back();
}

void Config::parseServerBlock(std::ifstream& file, ServerConfig& server) {
	std::string line, key, value, path;
	while (std::getline(file, line)) {
		if (line.find("}") != std::string::npos)
			return ;
		std::istringstream iss(line);
		iss >> key;
		if (key == "host") {
			iss >> value;
			stripSemicolon(value);
			server.host = value;
		}
		else if (key == "port") {
			iss >> value;
			stripSemicolon(value);
			server.port = stoi(value);
		}
		else if (key == "max_body_size") {
			iss >> value;
			stripSemicolon(value);
			server.max_body_size = std::stoi(value);
		}
		else if (key == "error_page") {
			iss >> value >> path;
			stripSemicolon(path);
			server.error_page[std::stoi(value)] = path;
		}
		else if (key == "location") {
			LocationConfig location;
			iss >> location.path;
			parseLocationBlock(file, location);
			server.locations.push_back(location);
		}
	}
}

std::string makeAbsolute(const std::string& path) {
	if (path[0] == '/')
		return path; // al absoluut, niets te doen

	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	return std::string(cwd) + "/" + path;
}

void Config::parseLocationBlock(std::ifstream& file, LocationConfig& location) {
	std::string line, key, path, method, ext, code;
	while (std::getline(file, line)) {
		if (line.find("}") != std::string::npos)
			return ;
		std::istringstream iss(line);
		iss >> key;
		if (key == "methods") {
			while (iss >> method) {
				stripSemicolon(method);
				location.methods.push_back(method);
			}
		}
		else if (key == "root") {
			iss >> path;
			stripSemicolon(path);
			location.root = makeAbsolute(path);
		}
		else if (key == "index") {
			iss >> path;
			stripSemicolon(path);
			location.index = path;
		}
		else if (key == "autoindex") {
			iss >> path;
			stripSemicolon(path);
			location.autoindex = (path == "on");
		}
		else if (key == "upload_store") {
			iss >> path;
			stripSemicolon(path);
			location.upload_store = makeAbsolute(path);
		}
		else if (key == "cgi") {
			iss >> ext >> path;
			stripSemicolon(path);
			location.cgi_ext = ext;
			location.cgi_path = makeAbsolute(path);
		}
		else if (key == "redirect") {
			iss >> code >> path;
			stripSemicolon(path);
			location.redirect_code = stoi(code);
			location.redirect_page = path;
		}
	}
}
