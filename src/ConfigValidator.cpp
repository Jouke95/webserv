#include "ConfigValidator.hpp"

#include <stdexcept>
#include <unistd.h>

ConfigValidator::ConfigValidator(const Config& config) : _config(config) {
	for (size_t i = 0; i < _config.getServers().size(); i++){
		validateServer(_config.getServers()[i]);
	}
	if (!_errors.empty()){
		std::string message = "\nErrors in config file: \n";
		for (size_t i = 0; i < _errors.size(); i++)
			message += "[error " + std::to_string(i) + "] - " + _errors[i] + "\n";
		throw std::runtime_error(message);
	}

}

void ConfigValidator::validateServer(const ServerConfig& server) {
	validateHost(server.host);
	validatePort(server.port);
	validateMaxBodySize(server.maxBodySize);
	validateErrorPages(server.errorPages);

	for (size_t i = 0; i < server.locations.size(); i++){
		validateLocation(server.locations[i]);
	}
}

void ConfigValidator::validateLocation(const LocationConfig& location) {
	validatePath(location.path);
	validateMethods(location.methods);
	validateRoot(location.root);
	validateIndex(location.index);
	validateAutoIndex(location);
	validateUploadStore(location.uploadStore);
	validateCGI(location);
	validateRedirect(location);
}

//Server Checks
void ConfigValidator::validateHost(const std::string& host) {
	if (host.empty())
		_errors.push_back("Host cannot be empty");
}

void ConfigValidator::validatePort(int port) {
	if (port < 1 || port > 65535)
		_errors.push_back("Invalid port number: must be between 1 and 65535");
}

void ConfigValidator::validateMaxBodySize(size_t MaxBodySize) {
	if (MaxBodySize == 0 || MaxBodySize > 10000000)
		_errors.push_back("Invalid max_body_size: must be between 1 and 10000000");
}

void ConfigValidator::validateErrorPages(const std::map<int, std::string>& ErrorPages) {
	for (std::map<int, std::string>::const_iterator it = ErrorPages.begin(); it != ErrorPages.end(); it++){
		if (it->first < 400 || it->first > 599)
			_errors.push_back("Invalid Error Code: " + std::to_string(it->first));
		if (access(it->second.c_str(), F_OK) == -1)
			_errors.push_back("Error page not accessible: " + it->second);
	}
}

//Location Checks
void ConfigValidator::validatePath(const std::string& path) {
	if (path.empty() || path[0] != '/')
		_errors.push_back("Location path must start with '/': " + path);
}

void ConfigValidator::validateMethods(const std::vector<std::string>& methods) {
	for (size_t i = 0; i < methods.size(); i++){
		if (methods[i] != "GET" && methods[i] != "DELETE" && methods[i] != "POST"){
			_errors.push_back("Invalid method: " + methods[i]);
		}
	}
}

void ConfigValidator::validateRoot(const std::string& root) {
	if (!root.empty() && access(root.c_str(), F_OK) == -1)
		_errors.push_back("Root path does not exist: " + root);
}

void ConfigValidator::validateIndex(const std::string& index) {
	if (!index.empty() && index[0] == '/')
		_errors.push_back("Index must be a filename, not a path: " + index);
}

void ConfigValidator::validateAutoIndex(const LocationConfig& location) {
	if (location.autoindex && !location.index.empty())
		_errors.push_back("Autoindex and index cannot both be set");
	if (location.autoindex && location.redirectCode)
		_errors.push_back("Autoindex and redirectcode cannot both be set");
	if (location.autoindex && location.root.empty())
		_errors.push_back("Root must have a value if autoindex is set");
}

void ConfigValidator::validateUploadStore(const std::string& uploadstore) {
	if (!uploadstore.empty() && access(uploadstore.c_str(), W_OK | F_OK) == -1)
		_errors.push_back("Upload store path does not exist or is not writable: " + uploadstore);
}

void ConfigValidator::validateCGI(const LocationConfig& location) {
	if (location.cgiExtensions.size() != location.cgiPaths.size())
		_errors.push_back("The amount of CGI extensions and paths are not the same");
	for (size_t i = 0; i < location.cgiExtensions.size(); i++){
		if (location.cgiExtensions[i].empty() || location.cgiExtensions[i][0] != '.')
			_errors.push_back("CGI extension must start with a '.': " + location.cgiExtensions[i]);
	}
	for (size_t i = 0; i < location.cgiPaths.size(); i++){
		if (access(location.cgiPaths[i].c_str(), F_OK | X_OK) == -1)
			_errors.push_back("CGI path doesn't exist or not executable: " + location.cgiPaths[i]);
	}
}

void ConfigValidator::validateRedirect(const LocationConfig& location) {
	if (location.redirectCode && location.redirectPage.empty())
		_errors.push_back("Redirect Page is Empty");
	if (!location.redirectPage.empty() && (location.redirectCode < 300 || location.redirectCode > 399))
		_errors.push_back("Redirect Code is nog between 300 and 399: " + std::to_string(location.redirectCode));
}