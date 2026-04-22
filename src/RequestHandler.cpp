#include "RequestHandler.hpp"
#include "gzip/Gzip.hpp"
#include <algorithm>
#include <cctype>
#include <climits>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>

static std::string lowerCopy(const std::string& value) {
	std::string lower = value;
	std::transform(lower.begin(), lower.end(), lower.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return lower;
}

static std::string trimHeaderValue(const std::string& value) {
	size_t start = value.find_first_not_of(" \t");
	if (start == std::string::npos)
		return "";
	size_t end = value.find_last_not_of(" \t");
	return value.substr(start, end - start + 1);
}

static bool startsWith(const std::string& value, const std::string& prefix) {
	return value.size() >= prefix.size() && value.compare(0, prefix.size(), prefix) == 0;
}

static bool hasHeaderToken(const std::string& value, const std::string& token) {
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

static bool isCompressibleContentType(const std::string& contentType) {
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

RequestHandler::RequestHandler(const HttpRequest& request,
							   const std::map<int, std::string>& errorPages,
							   const LocationConfig& location)
	: _request(request),
	  _errorPages(errorPages),
	  _location(location)
{
	handle();
}

RequestHandler::~RequestHandler() {}

HttpResponse RequestHandler::getResponse() const {
	return _response;
}

void RequestHandler::handle() {
	_response.setVersion("HTTP/1.1");
	if (_request.getVersion() != "HTTP/1.1") {
		errorPage(505); // HTTP Version Not Supported
		return;
	}

	if (redirectCheck())
		return ;
	if (!isValidMethod())
		return ;

	std::string method = _request.getMethod();

	if (method == "GET")
		handleGet();
	else if (method == "POST")
		handlePost();
	else if (method == "DELETE")
		handleDelete();

	applyGzip();
}

bool RequestHandler::redirectCheck() {
	if (_location.redirectCode != 0 && !_location.redirectPage.empty()) {
		_response.setHeader("Location", _location.redirectPage);
		_response.setStatusCode(_location.redirectCode);
		return true;
	}
	return false;
}

static bool vectorContains(const std::vector<std::string>& vec, const std::string& value) {
	for (size_t i = 0; i < vec.size(); i++) {
		if (vec[i] == value)
			return true;
	}
	return false;
}

bool RequestHandler::isValidMethod() {
	std::string method = _request.getMethod();

	if (!vectorContains(_knownMethods, method)) {
		_response.setStatusCode(400);
		return false;
	}
	if (!vectorContains(_implementedMethods, method)) {
		_response.setStatusCode(501);
		return false;
	}
	if (!vectorContains(_location.methods, method)) {
		_response.setStatusCode(405);
		return false;
	}
	return true;
}

std::string RequestHandler::makePath(const std::string& root) const {
	std::string requestPath = _request.getPath();
	std::string path = requestPath.substr(_location.path.size());
	if (path.empty() || path[0] != '/')
		path = "/" + path;
	return root + path;
}

void RequestHandler::handleGet() {
	std::string path = makePath(_location.root);

	struct stat info;
	if (stat(path.c_str(), &info) == -1) {
		errorPage(404);
		return;
	}

	if (S_ISDIR(info.st_mode))
		handleDirectory(path);
	else if (S_ISREG(info.st_mode))
		serveFile(path);
}

void RequestHandler::handlePost(){
	if (_location.uploadStore.empty()) {
		errorPage(500);
		return;
	}
	std::string path = makePath(_location.uploadStore);
	if (access(path.c_str(), F_OK) == 0) {
		errorPage(409);
		return;
	}
	std::ofstream file(path);
	if (!file.is_open()){
		errorPage(500);
		return;
	}
	file << _request.getBody();
	file.close();
	_response.setStatusCode(201);
	_response.setHeader("Location", _request.getPath());
}

void RequestHandler::handleDelete(){
	std::string path = makePath(_location.uploadStore);
	if (!std::filesystem::exists(path)){
		errorPage(404);
		return;
	}
	try {
		std::filesystem::remove(path);
		_response.setStatusCode(204);
	} catch (const std::filesystem::filesystem_error& e) {
		if (e.code() == std::errc::permission_denied)
			errorPage(403);
		else
			errorPage(500);
	}
}

void RequestHandler::handleDirectory(std::string path) {
	if (!_location.index.empty()) {
		if (path.back() != '/')
			path += "/";
		path += _location.index;
		serveFile(path);
	}
	else if (_location.autoindex) {
		// implement: serve directory listing
		// serveDirList();
		std::cout << "Dit is even een test om te checken of hij hier idd uit komt" << std::endl;
	}
	else
		errorPage(403);
}

void RequestHandler::serveFile(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		struct stat info;
		if (stat(path.c_str(), &info) == -1)
			errorPage(404);
		else
			errorPage(403);
		return;
	}

	std::string body((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	_response.setBody(body);
	_response.setContentLength(body.size());
	_response.setStatusCode(200);
	_response.setContentType(getContentType(path));
}

bool RequestHandler::shouldGzipResponse() const {
	if (_request.getMethod() != "GET")
		return false;
	if (!_request.hasHeaderToken("Accept-Encoding", "gzip"))
		return false;
	if (_response.getStatusCode() != 200)
		return false;
	if (_response.getBody().empty())
		return false;
	if (!isCompressibleContentType(_response.getContentType()))
		return false;

	const std::map<std::string, std::string>& headers = _response.getHeaders();
	if (headers.find("Content-Encoding") != headers.end())
		return false;
	return true;
}

void RequestHandler::applyGzip() {
	if (!shouldGzipResponse())
		return;

	std::string body = _response.getBody();
	try {
		std::vector<uint8_t> compressed = Gzip::compress(
			reinterpret_cast<const uint8_t*>(body.data()), body.size());
		if (compressed.size() > INT_MAX)
			return;

		std::string compressedBody(
			reinterpret_cast<const char*>(compressed.data()), compressed.size());
		_response.setBody(compressedBody);
		_response.setContentLength(static_cast<int>(compressedBody.size()));
		_response.setHeader("Content-Encoding", "gzip");

		const std::map<std::string, std::string>& headers = _response.getHeaders();
		std::map<std::string, std::string>::const_iterator vary = headers.find("Vary");
		if (vary == headers.end())
			_response.setHeader("Vary", "Accept-Encoding");
		else if (!hasHeaderToken(vary->second, "Accept-Encoding"))
			_response.setHeader("Vary", vary->second + ", Accept-Encoding");
	}
	catch (const std::exception&) {
		return;
	}
}

std::string RequestHandler::getContentType(const std::string& path) {
	size_t lastSlash = path.rfind('/');
	size_t lastDot = path.rfind('.');
	if (lastDot == std::string::npos || lastDot < lastSlash)
		return "application/octet-stream";

	std::string extension = path.substr(lastDot + 1);
	std::map<std::string, std::string>::iterator it = _mimeTypes.find(extension);
	if (it != _mimeTypes.end())
		return it->second;
	return "application/octet-stream";
}

void RequestHandler::errorPage(int errorCode) {
	_response.setStatusCode(errorCode);
	
	std::map<int, std::string>::iterator it = _errorPages.find(errorCode);
	if (it == _errorPages.end()) {
		setFallbackError();
		return;
	}
	std::string path = it->second;

	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		setFallbackError();
		return;
	}

	std::string body((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	_response.setBody(body);
	_response.setContentType(getContentType(path));
	_response.setContentLength(body.size());
}

void RequestHandler::setFallbackError() {
	_response.setBody("<html><body><h1>Error</h1></body></html>");
	_response.setContentType("text/html");
	_response.setContentLength(_response.getBody().size());
}

std::map<std::string, std::string> RequestHandler::_mimeTypes = {
	{"html", "text/html"},
	{"txt",  "text/plain"},
	{"jpg",  "image/jpeg"},
	{"png",  "image/png"},
	{"css",  "text/css"},
	{"js",   "application/javascript"}
};

std::vector<std::string> RequestHandler::_knownMethods = {
	"GET", "HEAD", "OPTIONS", "TRACE",
	"PUT", "DELETE", "POST", "PATCH", "CONNECT"
};

std::vector<std::string> RequestHandler::_implementedMethods = {
	"GET", "POST", "DELETE"
};
