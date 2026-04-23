#include "utils.hpp"
#include <limits.h>
#include <unistd.h>
#include <stdexcept>

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
