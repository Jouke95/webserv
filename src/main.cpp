#include "Server.hpp"

int main(int ac, char **av)
{
    Server  server;
    int     result;

    result = server.start();
    if (result != 0)
        exit(result);
    server.run();
}