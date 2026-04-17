#include "RequestHandler.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

RequestHandler::RequestHandler(const HttpRequest& request, const LocationConfig& location) : _request(request), _location(location) {
	handle();
}

RequestHandler::~RequestHandler() {}

void RequestHandler::handle() {
	bool redirect = redirectCheck();
	if (redirect)
		return ;

	bool methodAllowed = methodCheck();
	if (!methodAllowed)
		return ;

	std::string method = _request.getMethod();
	if (method == "GET")
		handleGet();
	else if (method == "POST")
		handlePost();
	else if (method == "DELETE")
		handleDelete();
}

bool RequestHandler::methodCheck() {
	for (size_t i = 0; i < _location.methods.size(); i++) {
		if (_request.getMethod() == _location.methods[i]) {
			return true;
		}
	}
	_response.setStatusCode(405);
	return false;
}

bool RequestHandler::redirectCheck() {
	if (_location.redirect_code != 0 && _location.redirect_page != "") {
		_response.setHeader("Location", _location.redirect_page);
		_response.setStatusCode(_location.redirect_code);
		return true;
	}
	return false;
}

std::string RequestHandler::makePath(const std::string& base) const {
	return (base + _request.getPath().substr(_location.path.size()));
}

void RequestHandler::handleGet(){
	std::string path = _location.root + _request.getPath();
	std::cout << "GET path: " << path << std::endl;

	if (path.back() == '/'){
		if (!_location.index.empty())
			path = path + _location.index;
		else {
			_response.setStatusCode(403);
			return;
		}
	}
	std::ifstream file(path);
	if (!file.is_open()) {
		_response.setStatusCode(404);
		return;
	}
	std::string body((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	_response.setBody(body);
	_response.setContentLength(body.size());
	_response.setStatusCode(200);
	_response.setVersion(_request.getVersion());
	size_t lastSlash = path.rfind('/');
	size_t lastDot = path.rfind('.');
	if (lastDot == std::string::npos || lastDot < lastSlash){
		_response.setContentType("application/octet-stream");
		return;
	}
	std::string extension = path.substr(lastDot + 1);
	std::map<std::string, std::string>::iterator it = _mimeTypes.find(extension);
	if (it != _mimeTypes.end())
		_response.setContentType(it->second);
	else
		_response.setContentType("application/octet-stream");
	return;
}

void RequestHandler::handlePost(){
	if (_location.upload_store.empty()) {
		_response.setStatusCode(500);
		return;
	}
	std::string path = makePath(_location.upload_store);
	std::cout << "POST path: " << path << std::endl;

	std::ofstream file(path);
	if (!file.is_open()){
		_response.setStatusCode(500);
		return;
	}
	file << _request.getBody();
	_response.setStatusCode(201);
	_response.setVersion(_request.getVersion());
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
	_response.setVersion(_request.getVersion());
	return;
}

std::map<std::string, std::string> RequestHandler::_mimeTypes = RequestHandler::initMimeTypes();

std::map<std::string, std::string> RequestHandler::initMimeTypes() {
	std::map<std::string, std::string> mimeTypes;
	mimeTypes["html"] = "text/html";
	mimeTypes["txt"]  = "text/plain";
	mimeTypes["jpg"]  = "image/jpeg";
	mimeTypes["png"]  = "image/png";
	mimeTypes["css"]  = "text/css";
	mimeTypes["js"]  = "application/javascript";

	return mimeTypes;
}

HttpResponse RequestHandler::getResponse() const {
	return _response;
}
