#include "RequestHandler.hpp"

#include "gzip/Gzip.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

RequestHandler::RequestHandler(const HttpRequest& request,
							   const std::map<int, std::string>& errorPages,
							   const LocationConfig& location,
							   size_t maxBodySize)
	: _request(request),
	  _errorPages(errorPages),
	  _location(location),
	  _maxBodySize(maxBodySize)
{
	handle();
}

RequestHandler::RequestHandler(const std::map<int, std::string>& errorPages, int errorCode) 
	: _errorPages(errorPages)
{
	errorPage(errorCode);
}

RequestHandler::~RequestHandler() {}

HttpResponse RequestHandler::getResponse() const {
	return _response;
}

void RequestHandler::handle() {
	if (redirectCheck())
		return;

	if (!decodeRequestBody())
		return;

	std::string method = _request.getMethod();

	if (method == "GET")
		handleGet();
	else if (method == "POST")
		handlePost();
	else if (method == "DELETE")
		handleDelete();

	applyGzip();
}

bool RequestHandler::redirectCheck() {
	if (_location.redirectCode != 0 && !_location.redirectPage.empty()) {
		_response.setHeader("Location", _location.redirectPage);
		_response.setStatusCode(_location.redirectCode);
		return true;
	}
	return false;
}

bool RequestHandler::decodeRequestBody() {
	if (_request.getBody().empty())
		return true;

	std::string encoding = normalizedContentEncoding(_request.getHeader("Content-Encoding"));
	if (encoding.empty() || encoding == "identity")
		return true;
	if (encoding != "gzip") {
		errorPage(415);
		return false;
	}

	try {
		std::string body = _request.getBody();
		std::vector<uint8_t> decoded = Gzip::decompress(
			reinterpret_cast<const uint8_t*>(body.data()), body.size());
		if ((_maxBodySize > 0 && decoded.size() > _maxBodySize) || decoded.size() > INT_MAX) {
			errorPage(413);
			return false;
		}
		_request.setBody(std::string(
			reinterpret_cast<const char*>(decoded.data()), decoded.size()));
		_request.setContentLength(static_cast<int>(_request.getBody().size()));
		return true;
	}
	catch (const std::exception&) {
		errorPage(400);
		return false;
	}
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
		if (errno == EACCES)
			errorPage(403);
		else
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
	if (access(path.c_str(), F_OK) == 0) {
		errorPage(409);
		return;
	}
	std::ofstream file(path, std::ios::binary);
	if (!file.is_open()){
		errorPage(500);
		return;
	}
	file.write(_request.getBody().data(), _request.getBody().size());
	file.close();
	_response.setStatusCode(201);
	_response.setHeader("Location", _request.getPath());
}

void RequestHandler::handleDelete(){
	std::string path = makePath(_location.uploadStore);
	if (!std::filesystem::exists(path)){
		errorPage(404);
		return;
	}
	try {
		std::filesystem::remove(path);
		_response.setStatusCode(204);
	} catch (const std::filesystem::filesystem_error& e) {
		if (e.code() == std::errc::permission_denied)
			errorPage(403);
		else
			errorPage(500);
	}
}

void RequestHandler::handleDirectory(std::string path) {
	if (!_location.index.empty()) {
		if (path.back() != '/')
			path += "/";
		path += _location.index;
		serveFile(path);
	}
	else if (_location.autoindex)
		serveDirList(path);
	else
		errorPage(403);
}

void RequestHandler::serveDirList(std::string path) {
	DIR *directory = opendir(path.c_str());
	if (directory == NULL) {
		errorPage(500);
		return;
	}

	std::vector<std::string> dirs;
	std::vector<std::string> files;
	readDirectory(directory, dirs, files);
	closedir(directory);

	std::string requestPath = _request.getPath();
	if (requestPath.back() != '/')
		requestPath += "/";

	_response.setBody(buildDirListHtml(requestPath, dirs, files));
	_response.setStatusCode(200);
	_response.setContentType("text/html");
}

void RequestHandler::readDirectory(DIR* dir, std::vector<std::string>& dirs, std::vector<std::string>& files) {
	dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;
		if (name == "." || name == "..")
			continue;
		if (entry->d_type == DT_DIR)
			dirs.push_back(name + "/");
		else
			files.push_back(name);
	}
}

// std::string RequestHandler::buildDirListHtml(const std::string& requestPath, const std::vector<std::string>& dirs, const std::vector<std::string>& files) {
// 	std::string html = "<html><body><h1>Index of " + requestPath + "</h1><hr><ul>";
// 	for (size_t i = 0; i < dirs.size(); i++)
// 		html += "<li><a href=\"" + requestPath + dirs[i] + "\">" + dirs[i] + "</a></li>";
// 	for (size_t i = 0; i < files.size(); i++)
// 		html += "<li><a href=\"" + requestPath + files[i] + "\">" + files[i] + "</a></li>";
// 	html += "</ul><hr></body></html>";
// 	return html;
// }

std::string RequestHandler::buildDirListHtml(const std::string& requestPath, const std::vector<std::string>& dirs, const std::vector<std::string>& files) {
	std::string rows;

	for (size_t i = 0; i < dirs.size(); i++) {
		rows += "<a class='entry dir' href='" + requestPath + dirs[i] + "'>"
			"<span class='icon'>📁</span>"
			"<span class='name'>" + dirs[i] + "</span>"
			"<span class='type'>dir</span>"
			"</a>";
	}
	for (size_t i = 0; i < files.size(); i++) {
		rows += "<a class='entry file' href='" + requestPath + files[i] + "'>"
			"<span class='icon'>📄</span>"
			"<span class='name'>" + files[i] + "</span>"
			"<span class='type'>file</span>"
			"</a>";
	}

	return "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
		"<title>Index of " + requestPath + "</title>"
		"<style>"
		"@import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@300;400;600&display=swap');"
		":root{--bg:#0a0a0f;--surface:#111118;--border:#1e1e2e;--accent:#00ff9d;--text:#e0e0f0;--muted:#555570;--mono:'JetBrains Mono',monospace;}"
		"*{margin:0;padding:0;box-sizing:border-box;}"
		"body{background:var(--bg);color:var(--text);font-family:var(--mono);min-height:100vh;padding:48px 24px;}"
		"body::before{content:'';position:fixed;inset:0;"
		"background-image:linear-gradient(rgba(0,255,157,0.03) 1px,transparent 1px),linear-gradient(90deg,rgba(0,255,157,0.03) 1px,transparent 1px);"
		"background-size:40px 40px;pointer-events:none;}"
		".wrap{max-width:700px;margin:0 auto;position:relative;z-index:1;}"
		".back{font-size:11px;color:var(--muted);text-decoration:none;display:block;margin-bottom:32px;}"
		".back:hover{color:var(--accent);}"
		".badge{font-size:11px;color:var(--accent);letter-spacing:3px;text-transform:uppercase;margin-bottom:12px;display:flex;align-items:center;gap:8px;}"
		".badge::before{content:'';width:6px;height:6px;background:var(--accent);border-radius:50%;box-shadow:0 0 8px var(--accent);}"
		"h1{font-size:22px;font-weight:600;margin-bottom:32px;color:var(--text);}"
		"h1 span{color:var(--accent);}"
		".entry{display:flex;align-items:center;gap:16px;padding:12px 16px;"
		"border-bottom:1px solid var(--border);text-decoration:none;color:var(--text);"
		"transition:background 0.15s,padding-left 0.15s;}"
		".entry:first-child{border-top:1px solid var(--border);}"
		".entry:hover{background:var(--surface);padding-left:20px;}"
		".icon{font-size:16px;width:20px;}"
		".name{flex:1;font-size:13px;}"
		".entry.dir .name{color:var(--accent);}"
		".type{font-size:10px;color:var(--muted);letter-spacing:2px;text-transform:uppercase;}"
		".empty{font-size:13px;color:var(--muted);padding:24px 0;}"
		"</style></head><body>"
		"<div class='wrap'>"
		"<a class='back' href='/'>← back to index</a>"
		"<div class='badge'>autoindex</div>"
		"<h1>index of <span>" + requestPath + "</span></h1>"
		+ (rows.empty() ? "<div class='empty'>empty directory</div>" : rows) +
		"</div></body></html>";
}

void RequestHandler::serveFile(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
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
	_response.setStatusCode(200);
	_response.setContentType(getContentType(path));
}

bool RequestHandler::shouldGzipResponse() const {
	if (_request.getMethod() != "GET")
		return false;
	if (!_request.hasHeaderToken("Accept-Encoding", "gzip"))
		return false;
	if (_response.getStatusCode() != 200)
		return false;
	if (_response.getBody().empty())
		return false;
	if (!isCompressibleContentType(_response.getContentType()))
		return false;

	const std::map<std::string, std::string>& headers = _response.getHeaders();
	if (headers.find("Content-Encoding") != headers.end())
		return false;
	return true;
}

void RequestHandler::applyGzip() {
	if (!shouldGzipResponse())
		return;

	std::string body = _response.getBody();
	try {
		std::vector<uint8_t> compressed = Gzip::compress(
			reinterpret_cast<const uint8_t*>(body.data()), body.size());
		if (compressed.size() > INT_MAX)
			return;

		std::string compressedBody(
			reinterpret_cast<const char*>(compressed.data()), compressed.size());
		_response.setBody(compressedBody);
		_response.setHeader("Content-Encoding", "gzip");

		const std::map<std::string, std::string>& headers = _response.getHeaders();
		std::map<std::string, std::string>::const_iterator vary = headers.find("Vary");
		if (vary == headers.end())
			_response.setHeader("Vary", "Accept-Encoding");
		else if (!hasHeaderToken(vary->second, "Accept-Encoding"))
			_response.setHeader("Vary", vary->second + ", Accept-Encoding");
	}
	catch (const std::exception&) {
		return;
	}
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

	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		setFallbackError();
		return;
	}

	std::string body((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	_response.setBody(body);
	_response.setContentType(getContentType(path));
}

void RequestHandler::setFallbackError() {
	_response.setBody("<html><body><h1>Error</h1></body></html>");
	_response.setContentType("text/html");
}

std::map<std::string, std::string> RequestHandler::_mimeTypes = {
	{"html", "text/html"},
	{"txt",  "text/plain"},
	{"jpg",  "image/jpeg"},
	{"png",  "image/png"},
	{"css",  "text/css"},
	{"js",   "application/javascript"}
};
