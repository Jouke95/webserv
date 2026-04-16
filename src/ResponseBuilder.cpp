#include "ResponseBuilder.hpp"
#include <fstream>
#include <iostream>

ResponseBuilder::ResponseBuilder(const HttpResponse& response) : _response(response) {

}

ResponseBuilder::~ResponseBuilder() {}

std::string ResponseBuilder::build() {
	std::string i = "hello";
	return i;
}