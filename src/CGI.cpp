#include "CGI.hpp"

CGI::CGI(struct pollfd& cgiReadPfd, struct pollfd& cgiWritePfd, const HttpRequest& request, const LocationConfig& location) {

}

// void write() {}
// void read(std::string& response) {}
// bool isDone() {}

// void handleCGI()
// {

// 	std::string extension = getExtension(_request.getPath());
// 	std::string interpreter;
// 	for (size_t i = 0; i < _location.cgiExtensions.size(); i++) {
// 		if (_location.cgiExtensions[i] == extension) {
// 			interpreter = _location.cgiPaths[i];
// 			break;
// 		}
// 	}
// 	if (interpreter.empty()) {
// 		errorPage(403);
// 		return;
// 	}

// 	executeCGI(interpreter);

// 	// Set ENV Vars

// 	// Fork + Pipe

// 	// in child:
// 	// read van stdin
// 	// Execv(CGI Path, path)

// 	// in parent:
// 	// Read pipe
// 	// 200 OK HTTP1.1
// 	// output van het script parsen
// }

// void executeCGI(std::string interpreter) {

// 	setCGIEnv(_request, _location, _request.getPath());


// 	execve(interpreter, _request.getPath(), env)

// }