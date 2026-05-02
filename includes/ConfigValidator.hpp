#ifndef CONFIGVALIDATOR_HPP
#define CONFIGVALIDATOR_HPP

#include "Config.hpp"

#include <map>
#include <string>
#include <vector>

class ConfigValidator {
	private:
		const Config& _config;
		std::vector<std::string> _errors;

		// Server validators
		void validateServer(const ServerConfig& server);
		void validateHost(const std::string& host);
		void validatePort(int port);
		void validateMaxBodySize(size_t maxBodySize);
		void validateErrorPages(const std::map<int, std::string>& errorPages);

		// Location validators
		void validateLocation(const LocationConfig& location);
		void validatePath(const std::string& path);
		void validateMethods(const std::vector<std::string>& methods);
		void validateRoot(const std::string& root);
		void validateIndex(const std::string& index);
		void validateAutoIndex(const LocationConfig& location);
		void validateUploadStore(const std::string& uploadStore);
		void validateCGI(const LocationConfig& location);
		void validateRedirect(const LocationConfig& location);

	public:
		ConfigValidator(const Config& config);
};

#endif