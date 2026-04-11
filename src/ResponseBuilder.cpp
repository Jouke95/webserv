#include "ResponseBuilder.hpp"
#include <fstream>
#include <iostream>

ResponseBuilder::ResponseBuilder(HttpRequest request, LocationConfig location) : _request(request), _location(location) {
	std::string method = _request.getMethod();

	if (method == "GET") {
		handleGet();
	}

	else if (method == "POST") {
		handlePost();
	}

	else if (method == "DELETE") {
		handleDelete();
	}
}

ResponseBuilder::~ResponseBuilder() {}

void ResponseBuilder::handleGet() {
	std::string requestje = (_location.root + _request.getPath());
	std::cout << "requestje: "<< requestje << std::endl;
	std::ifstream file(requestje);
	if (!file.is_open()) {
		_response.setStatusCode(404);
		std::cout << "statuscode na requestje: " << _response.getStatusCode() << std::endl;
		return;
	}
	std::string body((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	std::cout << "boddie: " << body << std::endl;
	_response.setStatusCode(200);
	_response.setBody(body);
}

void ResponseBuilder::handlePost() {

}

void ResponseBuilder::handleDelete() {

}

std::string ResponseBuilder::build() {
	std::string i = "hello";
	return i;
}