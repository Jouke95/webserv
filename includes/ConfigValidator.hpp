#ifndef CONFIGVALIDATOR_HPP
#define CONFIGVALIDATOR_HPP

#include "Config.hpp"
#include <string>
#include <vector>
#include <map>


class ConfigValidator {
private:
//Server validators
	void validateServer(const ServerConfig& server);
	void validateHost(const std::string& host);
	void validatePort(int port);
	void validateMaxBodySize(size_t MaxBodySize);
	void validateErrorPages(const std::map<int, std::string>& ErrorPages);

//Location validators
	void validateLocation(const LocationConfig& location);
	void validatePath(const std::string& path);
	void validateMethods(const std::vector<std::string>& methods);
	void validateRoot(const std::string& root);
	void validateIndex(const std::string& index);
	void validateAutoIndex(const LocationConfig& location);
	void validateUploadStore(const std::string& uploadstore);
	void validateCGI(const LocationConfig& location);
	void validateRedirect(const LocationConfig& location);



	const Config&	_config;
	std::vector<std::string> _errors;

	public:
	ConfigValidator(const Config& config);
};

#endif