#include "RequestHandler.hpp"
#include <fstream>
#include <iostream>

RequestHandler::RequestHandler(const HttpRequest& request, const LocationConfig& location) : _request(request), _location(location) {
	handle();
}

RequestHandler::~RequestHandler() {}

void RequestHandler::handle() {
	bool methodAllowed = methodCheck();
	if (!methodAllowed)
		return ;

	bool redirect = redirectCheck();
	if (redirect)
		return ;

	std::string method = _request.getMethod();
	if (method == "GET")
		handleGet();
	else if (method == "POST")
		handlePost();
	else if (method == "DELETE")
		handleDelete();
	else if (method == "PUT")
		handlePut();

}

bool RequestHandler::methodCheck() {
	for (int i = 0; i < _location.methods.size(); i++) {
		if (_request.getMethod() == _location.methods[i]) {
			return true;
		}
	}
	giveErrorResponse(405);
	return false;
}

bool RequestHandler::redirectCheck() {
	if (_location.redirect_code != 0 && _location.redirect_page != "") {
		_response.setHeader("Location", _location.redirect_page);
		_response.setStatusCode(301);
		_response.setStatusMessage("Moved Permanently");
		return true;
	}
	return false;
}

std::string getMimeType(const std::string& extension){
	std::map<std::string, std::string> mimeTypes;
	mimeTypes["html"] = "text/html";
	mimeTypes["txt"]  = "text/plain";
	mimeTypes["jpg"]  = "image/jpeg";
	mimeTypes["png"]  = "image/png";
	mimeTypes["css"]  = "text/css";
	mimeTypes["js"]  = "application/javascript";

	std::map<std::string, std::string>::iterator it = mimeTypes.find(extension);
	if (it != mimeTypes.end())
		return it->second;
	return "application/octet-stream";
}

void RequestHandler::handleGet(){
	std::string path = _location.root + _request.getPath();
	std::ifstream file(path);
	if (!file.is_open()) {
		_response.setStatusCode(404);
		return ;
	}
	std::string body((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	_response.setBody(body);
	_response.setContentLength(body.size());
	_response.setStatusCode(200);
	_response.setVersion(_request.getVersion());
	size_t typePos = path.rfind('.');
	_response.setContentType(getMimeType(path.substr(typePos + 1)));
}

void RequestHandler::handlePost(){

}

void RequestHandler::handleDelete(){

}

void RequestHandler::handlePut(){

}

int RequestHandler::giveErrorResponse(int code){

}