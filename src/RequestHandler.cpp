#include "RequestHandler.hpp"

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