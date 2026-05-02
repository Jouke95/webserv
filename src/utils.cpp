#include "utils.hpp"

#include <cctype>
#include <limits.h>
#include <stdexcept>
#include <unistd.h>

int strToInt(const std::string& str) {
	for (size_t i = 0; i < str.size(); i++) {
		if (!isdigit(str[i]))
			return -1;
	}
	try {
		return std::stoi(str);
	} catch (std::exception& e) {
		return -1;
	}
}

size_t strToSizeT(const std::string& str) {
	for (size_t i = 0; i < str.size(); i++) {
		if (!isdigit(str[i]))
			return std::string::npos;
	}
	try {
		return std::stoul(str);
	} catch (std::exception& e) {
		return std::string::npos;
	}
}

void stripSemicolon(std::string& value) {
	if (!value.empty() && value.back() == ';')
		value.pop_back();
}

std::string makeAbsolute(const std::string& path) {
	if (path.empty() || path[0] == '/')
		return path;

	char cwd[PATH_MAX];
	if (!getcwd(cwd, sizeof(cwd)))
		throw std::runtime_error("getcwd failed");
	return std::string(cwd) + "/" + path;
}

void stripCR(std::string& line) {
	if (!line.empty() && line.back() == '\r')
		line.pop_back();
}

bool vectorContains(const std::vector<std::string>& vec, const std::string& value) {
	for (size_t i = 0; i < vec.size(); i++) {
		if (vec[i] == value)
			return true;
	}
	return false;
}

std::string lowerCopy(const std::string& value) {
	std::string lower = value;
	std::transform(lower.begin(), lower.end(), lower.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return lower;
}

std::string trimHeaderValue(const std::string& value) {
	size_t start = value.find_first_not_of(" \t");
	if (start == std::string::npos)
		return "";
	size_t end = value.find_last_not_of(" \t");
	return value.substr(start, end - start + 1);
}

bool startsWith(const std::string& value, const std::string& prefix) {
	return value.size() >= prefix.size() && value.compare(0, prefix.size(), prefix) == 0;
}

bool hasHeaderToken(const std::string& value, const std::string& token) {
	std::string wanted = lowerCopy(token);
	size_t pos = 0;

	while (pos < value.size()) {
		size_t comma = value.find(',', pos);
		std::string part = value.substr(pos, comma - pos);
		size_t semicolon = part.find(';');
		std::string name = lowerCopy(trimHeaderValue(part.substr(0, semicolon)));
		if (name == wanted)
			return true;
		if (comma == std::string::npos)
			break;
		pos = comma + 1;
	}
	return false;
}

bool isCompressibleContentType(const std::string& contentType) {
	std::string type = lowerCopy(contentType);
	size_t semicolon = type.find(';');
	if (semicolon != std::string::npos)
		type = type.substr(0, semicolon);
	type = trimHeaderValue(type);

	return startsWith(type, "text/")
		|| type == "application/javascript"
		|| type == "application/json"
		|| type == "application/xml";
}

std::string normalizedContentEncoding(const std::string& value) {
	return lowerCopy(trimHeaderValue(value));
}
