#include "CGI.hpp"
#include "HttpResponse.hpp"
#include "utils.hpp"

#include <csignal>
#include <cstring>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>

CGI::CGI(pollfd& cgiReadPfd, pollfd& cgiWritePfd, const HttpRequest& request, const LocationConfig& location, const ServerConfig& server)
	: _readFd1(-1),
	  _writeFd1(-1),
	  _readFd2(-1),
	  _writeFd2(-1),
	  _pid(-1),
	  _location(location),
	  _request(request),
	  _server(server),
	  _bytesWritten(0),
	  _isDone(false),
	  _hasError(false),
	  _errorCode(0)
{
	initPipes();
	if (_hasError)
		return;

	_scriptPath = makePath(_location.root);
	validateScript();
	if (_hasError)
		return;

	setupChildProcess();
	if (_hasError)
		return;

	setupParent(cgiReadPfd, cgiWritePfd);
}

CGI::~CGI() {
	if (_readFd1 != -1) close(_readFd1);
	if (_writeFd1 != -1) close(_writeFd1);
	if (_readFd2 != -1) close(_readFd2);
	if (_writeFd2 != -1) close(_writeFd2);
	if (_pid != -1) {
		kill(_pid, SIGKILL);
		waitpid(_pid, nullptr, WNOHANG);
	}
}

void CGI::initPipes() {
	int pipe1[2];
	int pipe2[2];

	if (pipe(pipe1) == -1) {
		_hasError = true;
		_errorCode = 500;
		return;
	}

	if (pipe(pipe2) == -1) {
		close(pipe1[0]);
		close(pipe1[1]);
		_hasError = true;
		_errorCode = 500;
		return;
	}

	_readFd1 = pipe1[0];
	_writeFd1 = pipe1[1];
	_readFd2 = pipe2[0];
	_writeFd2 = pipe2[1];
}

void CGI::setupChildProcess() {
	std::string interpreter = getInterpreter();
	if (_hasError)
		return;

	char** env = setCGIEnv();

	_pid = fork();
	if (_pid == -1) {
		forkError();
		return;
	}

	if (_pid == 0)
		childProcess(interpreter, _scriptPath, env);

	freeCGIEnv(env);
	close(_readFd1);
	_readFd1 = -1;
	close(_writeFd2);
	_writeFd2 = -1;
}

void CGI::setupParent(pollfd& cgiReadPfd, pollfd& cgiWritePfd) {
	cgiReadPfd.fd = _readFd2;
	cgiReadPfd.events = POLLIN;

	if (!_request.getBody().empty()) {
		cgiWritePfd.fd = _writeFd1;
		cgiWritePfd.events = POLLOUT;
	} else {
		close(_writeFd1);
		_writeFd1 = -1;
	}
}

void CGI::childProcess(const std::string& interpreter, const std::string& path, char **env) {
	close(_writeFd1);
	close(_readFd2);
	if (dup2(_readFd1, STDIN_FILENO) == -1 || dup2(_writeFd2, STDOUT_FILENO) == -1) {
		freeCGIEnv(env);
		close(_readFd1);
		close(_writeFd2);
		exit(1);
	}
	close(_readFd1);
	close(_writeFd2);

	char* argv[] = { 
		const_cast<char*>(interpreter.c_str()), 
		const_cast<char*>(path.c_str()), 
		nullptr 
	};
	chdir(path.substr(0, path.rfind('/')).c_str());
	execve(interpreter.c_str(), argv, env);
	freeCGIEnv(env);
	exit(1);
}

std::string CGI::getInterpreter() {
	std::string extension = getExtension(_request.getPath());
	std::string interpreter;
	for (size_t i = 0; i < _location.cgiExtensions.size(); i++) {
		if (_location.cgiExtensions[i] == extension) {
			interpreter = _location.cgiPaths[i];
			break;
		}
	}
	if (interpreter.empty()) {
		_hasError = true;
		_errorCode = 403;
		return "";
	}
	return interpreter;
}

void CGI::validateScript() {
	if (access(_scriptPath.c_str(), F_OK) == -1) {
		_hasError = true;
		_errorCode = 404;
		return;
	}
	if (access(_scriptPath.c_str(), R_OK) == -1) {
		_hasError = true;
		_errorCode = 403;
		return;
	}
}

std::string CGI::makePath(const std::string& root) {
	std::string requestPath = _request.getPath();
	std::string path = requestPath.substr(_location.path.size());
	if (path.empty() || path[0] != '/')
		path = "/" + path;
	return root + path;
}

std::string CGI::getExtension(const std::string& path) {
	size_t dot = path.rfind('.');
	size_t slash = path.rfind('/');

	if (dot == std::string::npos || (slash != std::string::npos && dot < slash))
		return "";
	return path.substr(dot);
}

bool CGI::isDone() {
	return _isDone;
}

bool CGI::hasError() {
	return _hasError;
}

int CGI::getErrorCode() {
	return _errorCode;
}

void CGI::writeToCGI() {
	const std::string& body = _request.getBody();
	ssize_t bytes = write(_writeFd1, body.c_str() + _bytesWritten, body.size() - _bytesWritten);
	if (bytes <= 0) {
		_hasError = true;
		_errorCode = 500;
		_isDone = true;
		return;
	}
	_bytesWritten += bytes;
	if (_bytesWritten >= (ssize_t)body.size()) {
		close(_writeFd1);
		_writeFd1 = -1;
	}
}

void CGI::readFromCGI() {
	char buffer[4096];
	int bytesRead = read(_readFd2, buffer, sizeof(buffer) - 1);
	if (bytesRead == -1) {
		_hasError = true;
		_errorCode = 500;
		_isDone = true;
		return;
	}
	if (bytesRead == 0) {
		close(_readFd2);
		_readFd2 = -1;
		_isDone = true;
		return;
	}
	_output.append(buffer, bytesRead);
}

HttpResponse CGI::getResponse() {
	HttpResponse response;

	size_t delimLen;
	size_t headerEnd = _output.find("\r\n\r\n");
	if (headerEnd != std::string::npos) {
		delimLen = 4;
	} else {
		headerEnd = _output.find("\n\n");
		delimLen = 2;
	}

	if (headerEnd == std::string::npos) {
		response.setStatusCode(500);
		_hasError = true;
		return response;
	}

	std::string headerSection = _output.substr(0, headerEnd);
	std::string body = _output.substr(headerEnd + delimLen);

	// headers parsing
	std::istringstream stream(headerSection);
	std::string line;
	int statusCode = 200;
	while (std::getline(stream, line)) {
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		size_t colon = line.find(':');
		if (colon == std::string::npos)
			continue;
		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 2);
		if (key == "Status")
			statusCode = std::stoi(value);
		else if (key == "Content-Type")
			response.setContentType(value);
		else if (key != "Content-Length")
			response.setHeader(key, value);
	}

	response.setStatusCode(statusCode);
	response.setBody(body);
	return response;
}

char** CGI::setCGIEnv() {
	std::vector<std::string> env;

	std::string scriptPath = _request.getPath().substr(_location.path.size());
	if (scriptPath.empty() || scriptPath[0] != '/')
		scriptPath = "/" + scriptPath;

	env.push_back("SCRIPT_NAME=" + scriptPath);
	env.push_back("SCRIPT_FILENAME=" + makePath(_location.root));
	env.push_back("REQUEST_METHOD=" + _request.getMethod());
	env.push_back("PATH_INFO=" + _request.getPath());
	env.push_back("QUERY_STRING=" + _request.getQueryString());
	env.push_back("SERVER_NAME=" + _server.host);
	env.push_back("SERVER_PORT=" + std::to_string(_server.port));
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("HTTP_HOST=" + _request.getHost());
	env.push_back("REDIRECT_STATUS=200");

	if (_request.getMethod() == "POST") {
		env.push_back("CONTENT_TYPE=" + _request.getContentType());
		env.push_back("CONTENT_LENGTH=" + std::to_string(_request.getContentLength()));
	}

	const std::map<std::string, std::string>& headers = _request.getHeaders();
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++) {
		std::string lower = lowerCopy(it->first);
		if (lower == "content-type" || lower == "content-length")
			continue;
		std::string key = "HTTP_";
		for (size_t i = 0; i < it->first.size(); i++) {
			if (it->first[i] == '-')
				key += '_';
			else
				key += std::toupper(it->first[i]);
		}
		env.push_back(key + "=" + it->second);
	}

	char** envp = new char*[env.size() + 1];
	for (size_t i = 0; i < env.size(); i++) {
		envp[i] = new char[env[i].size() + 1];
		std::strcpy(envp[i], env[i].c_str());
	}
	envp[env.size()] = nullptr;

	return envp;
}

void CGI::freeCGIEnv(char** env) {
	for (size_t i = 0; env[i] != nullptr; i++)
		delete[] env[i];
	delete[] env;
}

void CGI::forkError() {
	close(_readFd1);
	close(_writeFd1);
	close(_readFd2);
	close(_writeFd2);
	_hasError = true;
	_errorCode = 500;
}
