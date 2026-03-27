#include "Config.hpp"
#include <fstream>
#include <iostream>

Config::Config(const std::string& path){
	std::string content;
	std::ifstream confFile(path);

	if (!confFile.is_open())
		throw

	while (std::getline(confFile, content)){
		std::cout << content;
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
