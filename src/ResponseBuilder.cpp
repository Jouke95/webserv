#include "ResponseBuilder.hpp"
#include <fstream>
#include <iostream>

ResponseBuilder::ResponseBuilder(const HttpResponse& response) : _response(response) {

}

ResponseBuilder::~ResponseBuilder() {}

std::string ResponseBuilder::build() {
	std::string response;

    // Status line
    response += _response.getVersion() + " "
              + std::to_string(_response.getStatusCode()) + " "
              + _response.getStatusMessage() + "\r\n";

    // Vaste headers
    if (!_response.getContentType().empty())
        response += "Content-Type: " + _response.getContentType() + "\r\n";

    response += "Content-Length: " + std::to_string(_response.getContentLength()) + "\r\n";

    if (!_response.getConnection().empty())
        response += "Connection: " + _response.getConnection() + "\r\n";

    // Extra headers uit de map
    const std::map<std::string, std::string>& headers = _response.getHeaders();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
        response += it->first + ": " + it->second + "\r\n";

    // Lege regel + body
    response += "\r\n";
    response += _response.getBody();

    return response;
}