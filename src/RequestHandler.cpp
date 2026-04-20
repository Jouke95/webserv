#include "RequestHandler.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>

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
	std::cout << "POST path: " << path << std::endl;

	std::ofstream file(path);
	if (!file.is_open()){
		errorPage(500);
		return;
	}
	file << _request.getBody();
	_response.setStatusCode(201);
}

void RequestHandler::handleDelete(){
	std::string path = _location.root + _request.getPath();
	if (!std::filesystem::exists(path)){
		_response.setStatusCode(404);
		return;
	}
	try {
		std::filesystem::remove(path);
		_response.setStatusCode(204);
	} catch (const std::filesystem::filesystem_error& e) {
		if (e.code() == std::errc::permission_denied)
			_response.setStatusCode(403);
		else
			_response.setStatusCode(500);
	}
	return;
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
	std::ifstream file(path);
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

	std::ifstream file(path);
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