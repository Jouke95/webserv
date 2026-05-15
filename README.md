*This project has been created as part of the 42 curriculum by theyn, tridley and jspannin.*

# webserv
 
## Description
 
webserv is a fully functional HTTP/1.1 web server written in C++17, built as part of the Codam (42 Network) curriculum. The goal is to deeply understand how web servers work at the protocol level — from TCP socket handling and non-blocking I/O multiplexing to HTTP request parsing and response generation.
 
The server is modelled after Nginx's architecture and configuration format. It supports multiple virtual servers on different ports, location-based routing with longest-prefix matching, static file serving, file uploads, CGI execution, and a clean event loop driven by `poll()`.
 
**Supported features:**
- GET, POST, and DELETE methods
- Static website serving with configurable root directories
- File uploads to configurable upload directories
- Directory listing (autoindex)
- Default index files per location
- HTTP redirects (301)
- CGI execution for `.py` (Python) and `.php` (PHP) scripts
- Multiple server blocks listening on different ports
- Per-route method restrictions
- Configurable client body size limit
- Custom error pages with built-in defaults
- Non-blocking I/O using a single `poll()` loop for all socket and pipe operations
## Instructions
 
### Requirements
 
- A C++17-compatible compiler (`c++`)
- `python3` in `PATH` for Python CGI
- `php-cgi` in `PATH` for PHP CGI
### Compilation
 
```bash
make
```
 
This produces the `./webserv` binary. To clean up:
 
```bash
make clean   # removes object files
make fclean  # removes object files and binary
make re      # fclean + full rebuild
```
 
### Running the server
 
```bash
./webserv [configuration file]
```
 
If no configuration file is provided, the server falls back to a default path.
 
**Example:**
```bash
./webserv config/default.conf
```
 
### Configuration file format
 
The configuration file follows an Nginx-inspired syntax with `server {}` and `location {}` blocks.
 
```nginx
server {
	host localhost;
	port 8080;
	max_body_size 1048576;
 
	error_page 404 /errors/404.html;
	error_page 500 /errors/500.html;
 
	location / {
		root /var/www/html;
		index index.html;
		methods GET;
		autoindex off;
	}
 
	location /upload {
		methods GET POST DELETE;
		upload_store /var/www/uploads;
	}
 
	location /cgi-bin {
		methods GET POST;
		cgi_extension .py /usr/bin/python3;
		cgi_extension .php /usr/bin/php-cgi;
		root /var/www/cgi-bin;
	}
 
	location /old {
		return 301 /new;
	}
}
```

### Testing

Test with a browser by navigating to `http://localhost:<port>/`.

Test raw HTTP with netcat:
```bash
printf "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc localhost 8080
```

Test file upload with curl:
```bash
curl -X POST http://localhost:8080/upload \
	 -F "file=@/path/to/file.txt"
```
 
Test CGI:
```bash
curl http://localhost:8080/cgi-bin/hello.py
```
 
## Resources
 
### HTTP & Networking
 
- [RFC 7230 — HTTP/1.1: Message Syntax and Routing](https://datatracker.ietf.org/doc/html/rfc7230)
- [RFC 7231 — HTTP/1.1: Semantics and Content](https://datatracker.ietf.org/doc/html/rfc7231)
- [MDN Web Docs — HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) — Essential reference for sockets, `bind`, `accept`, `poll`, etc.
- [The Linux `poll()` man page](https://man7.org/linux/man-pages/man2/poll.2.html)
- [Nginx documentation — Server block configuration](https://nginx.org/en/docs/http/ngx_http_core_module.html)

### CGI

- [RFC 3875 — The Common Gateway Interface (CGI/1.1)](https://datatracker.ietf.org/doc/html/rfc3875)
- [CGI environment variables reference (MDN)](https://developer.mozilla.org/en-US/docs/Web/HTTP/MIME_types)

### Common Time GEO
- [wordly.org](https://wordly.org)


### How AI was used
 
AI (Claude by Anthropic) was used throughout this project exclusively as a **Socratic learning tool** — asking guiding questions rather than providing direct solutions. Specifically:
 
- **Concept explanation**: Understanding the `poll()` event loop, non-blocking I/O, socket lifecycle (bind → listen → accept), and HTTP request/response structure.
- **Debugging guidance**: When unexpected behaviour occurred (e.g. wrong server config matched, POST requests rejected incorrectly), AI helped reason through the root cause by asking targeted questions about the logic, rather than providing fixes directly.
- **Architecture discussion**: Reasoning through where responsibilities belong (e.g. request validation vs. parsing vs. routing), and why certain design decisions matter for correctness and maintainability.
No AI-generated code was used directly. All implementation decisions were made and understood by the team.
