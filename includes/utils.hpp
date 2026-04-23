#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

int strToInt(const std::string& str);
size_t strToSizeT(const std::string& str);
void stripCR(std::string& line);
std::string makeAbsolute(const std::string& path);
void stripSemicolon(std::string& value);
bool vectorContains(const std::vector<std::string>& vec, const std::string& value);

#endif