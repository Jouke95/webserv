#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 4096

std::string buildResponse(const std::string& path) {
    std::string body =
        "<!DOCTYPE html><html><body>"
        "<h1>Hello from C++! joe</h1>"
        "<p>Path: " + path + "</p>"
        "</body></html>";

    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\r\n"
        << "Content-Type: text/html\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "Connection: close\r\n\r\n"
        << body;
    return oss.str();
}

std::string parsePath(const std::string& request) {
    size_t start = request.find(' ') + 1;
    size_t end   = request.find(' ', start);
    if (start == std::string::npos || end == std::string::npos)
        return "/";
    return request.substr(start, end - start);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(PORT);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); return 1; }
    if (listen(server_fd, 10) < 0) { perror("listen"); return 1; }

    std::cout << "Server luistert op http://localhost:" << PORT << "\n";

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) { perror("accept"); continue; }

        char buf[BUFFER_SIZE] = {};
        read(client_fd, buf, BUFFER_SIZE - 1);

        std::string request(buf);
        std::string path     = parsePath(request);
        std::string response = buildResponse(path);

        write(client_fd, response.c_str(), response.size());
        close(client_fd);

        std::cout << "GET " << path << "\n";
    }

    close(server_fd);
}