#include "Server.hpp"

int main()
{
    Server  server;
    int     result;
    
    result = server.start();
    if (result == 1)
        exit(1);
    server.waitForRequest();
}