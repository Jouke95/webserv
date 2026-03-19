#include "Server.hpp"

int main()
{
    Server  server;
    int     result;
    
    result = server.start();
    if (result != 0)
        exit(result);
    server.waitForRequest();
}