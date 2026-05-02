#include "ResponseBuilder.hpp"

ResponseBuilder::ResponseBuilder(const HttpResponse& response) : _response(response) {}

ResponseBuilder::~ResponseBuilder() {}

std::string ResponseBuilder::build() {
	std::string body = _response.getBody();
	std::string response;

	response += _response.getVersion() + " "
			  + std::to_string(_response.getStatusCode()) + " "
			  + _response.getStatusMessage() + "\r\n";

	if (!_response.getContentType().empty())
		response += "Content-Type: " + _response.getContentType() + "\r\n";

	response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
	response += "Connection: close\r\n";

	const std::map<std::string, std::string>& headers = _response.getHeaders();
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		if (it->first == "Content-Length" || it->first == "Content-Type")
			continue;
		response += it->first + ": " + it->second + "\r\n";
	}

	response += "\r\n";
	response += body;

	return response;
}
