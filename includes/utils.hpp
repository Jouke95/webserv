#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <string>
#include <vector>

int strToInt(const std::string& str);
size_t strToSizeT(const std::string& str);
void stripCR(std::string& line);
std::string makeAbsolute(const std::string& path);
void stripSemicolon(std::string& value);
bool vectorContains(const std::vector<std::string>& vec, const std::string& value);
std::string lowerCopy(const std::string& value);
std::string trimHeaderValue(const std::string& value);
bool startsWith(const std::string& value, const std::string& prefix);
bool hasHeaderToken(const std::string& value, const std::string& token);
bool isCompressibleContentType(const std::string& contentType);
std::string normalizedContentEncoding(const std::string& value);

#endif
