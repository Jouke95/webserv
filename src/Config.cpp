#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>

Config::Config(const std::string& path){
	std::string content;
	std::ifstream file(path);

	if (!file.is_open())
		throw std::runtime_error("Could not open config file: " + path);

	while (std::getline(file, content)){
		if (content.find("server {") != std::string::npos) {
			ServerConfig server;
			parseServerBlock(file, server);
			_servers.push_back(server);
		}
	}
}

Config::Config(const Config& other){
	*this = other;
}

Config& Config::operator=(const Config& other){
	if (this != &other)
		_servers = other._servers;
	return *this;
}

Config::~Config(){

}

const std::vector<ServerConfig>& Config::GetServer() const{
	return this->_servers;
}

static void stripSemicolon(std::string& value) {
	if (!value.empty() && value.back() == ';')
		value.pop_back();
}

void Config::parseServerBlock(std::ifstream& file, ServerConfig& server){
	std::string line, key, value, path;
	while (std::getline(file, line)) {
		if (line.find("}") != std::string::npos)
			return ;
		std::istringstream iss(line);
		iss >> key;
		if (key == "host"){
			iss >> value;
			stripSemicolon(value);
			server.host = value;
		}
		else if (key == "port"){
			iss >> value;
			stripSemicolon(value);
			server.port = stoi(value);
		}
		else if (key == "max_body_size"){
			iss >> value;
			stripSemicolon(value);
			server.max_body_size = std::stoi(value);
		}
		else if (key == "error_page"){
			iss >> value >> path;
			stripSemicolon(path);
			server.error_page[std::stoi(value)] = path;
		}
		else if (key == "location"){
			LocationConfig location;
			iss >> location.path;
			parseLocationBlock(file, location);
			server.location.push_back(location);
		}
	}
}

void Config::parseLocationBlock(std::ifstream& file, LocationConfig& location){
	std::string line, key, path, method, ext, code;
	while (std::getline(file, line)) {
		if (line.find("}") != std::string::npos)
			return ;
		std::istringstream iss(line);
		iss >> key;
		if (key == "methods"){
			while (iss >> method) {
				stripSemicolon(method);
				location.methods.push_back(method);
			}
		}
		else if (key == "root"){
			iss >> path;
			stripSemicolon(path);
			location.root = path;
		}
		else if (key == "index"){
			iss >> path;
			stripSemicolon(path);
			location.index = path;
		}
		else if (key == "autoindex"){
			iss >> path;
			stripSemicolon(path);
			location.autoindex = (path == "on");
		}
		else if (key == "upload_store"){
			iss >> path;
			stripSemicolon(path);
			location.upload_store = path;
		}
		else if (key == "cgi"){
			iss >> ext >> path;
			stripSemicolon(path);
			location.cgi_ext = ext;
			location.cgi_path = path;
		}
		else if (key == "redirect"){
			iss >> code >> path;
			stripSemicolon(path);
			location.redirect_code = stoi(code);
			location.redirect_page = path;
		}
	}
}



// void Config::parseServerBlock(std::ifstream& file, ServerConfig& server){
// 	std::string line, key, value, path;
// 	while (std::getline(file, line)) {
// 		if (line.find("}") != std::string::npos)
// 			return ;
// 		else if (line.find("host") != std::string::npos){
// 			std::istringstream iss(line);
// 			iss >> key >> value;
// 			if (!value.empty() && value.back() == ';')
// 				value.pop_back();
// 			server.host = value;
// 		}
// 		else if (line.find("port") != std::string::npos){
// 			std::istringstream iss(line);
// 			iss >> key >> value;
// 			if (!value.empty() && value.back() == ';')
// 				value.pop_back();
// 			server.port = std::stoi(value);
// 		}
// 		else if (line.find("max_body_size") != std::string::npos){
// 			std::istringstream iss(line);
// 			iss >> key >> value;
// 			if (!value.empty() && value.back() == ';')
// 				value.pop_back();
// 			server.max_body_size = std::stoi(value);
// 		}
// 		else if (line.find("error_page") != std::string::npos){
// 			std::istringstream iss(line);
// 			iss >> key >> value >> path;
// 			if (!path.empty() && path.back() == ';')
// 				path.pop_back();
// 			server.error_page[std::stoi(value)] = path;
// 		}
// 		else if (line.find("location") != std::string::npos){
// 			std::istringstream iss(line);
// 			LocationConfig location;
// 			iss >> key >> location.path;
// 			parseLocationBlock(file, location);
// 			server.location.push_back(location);
// 		}
// 	}
// }

// void Config::parseLocationBlock(std::ifstream& file, LocationConfig& location){
// 	std::string line, key, path, method, ext, code;
// 	while (std::getline(file, line)) {
// 		if (line.find("}") != std::string::npos)
// 			return ;
// 		else if (line.find("methods") != std::string::npos){
// 			std::istringstream iss(line);
// 			iss >> key;
// 			while (iss >> method) {
// 				if (!method.empty() && method.back() == ';')
// 					method.pop_back();
// 				location.methods.push_back(method);
// 			}
// 		}
// 		else if (line.find("root") != std::string::npos){
// 			std::istringstream iss(line);
// 			iss >> key >> path;
// 			if (!path.empty() && path.back() == ';')
// 				path.pop_back();
// 			location.root = path;
// 		}
// 		else if (line.find("index") != std::string::npos){
// 			std::istringstream iss(line);
// 			iss >> key >> path;
// 			if (!path.empty() && path.back() == ';')
// 				path.pop_back();
// 			location.index = path;
// 		}
// 		else if (line.find("autoindex") != std::string::npos){
// 			std::istringstream iss(line);
// 			iss >> key >> path;
// 			if (!path.empty() && path.back() == ';')
// 				path.pop_back();
// 			location.autoindex = (path == "on");
// 		}
// 		else if (line.find("upload_store") != std::string::npos){
// 			std::istringstream iss(line);
// 			iss >> key >> path;
// 			if (!path.empty() && path.back() == ';')
// 				path.pop_back();
// 			location.upload_store = path;
// 		}
// 		else if (line.find("cgi") != std::string::npos){
// 			std::istringstream iss(line);
// 			iss >> key >> ext >> path;
// 			if (!path.empty() && path.back() == ';')
// 				path.pop_back();
// 			location.cgi_ext = ext;
// 			location.cgi_path = path;
// 		}
// 		else if (line.find("redirect") != std::string::npos){
// 			std::istringstream iss(line);
// 			iss >> key >> code >> path;
// 			if (!path.empty() && path.back() == ';')
// 				path.pop_back();
// 			location.redirect_code = stoi(code);
// 			location.redirect_page = path;
// 		}
// 	}
// }