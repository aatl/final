#include <iostream>
#include <vector>
#include <string>
#include "master_server.h"
#include "cmd_parse.h"

int main(int argc, char* argv[])
{
//    std::string hostStr  = "127.0.0.1";
//    int port = 8080;
//    std::string dirStr = "testDir";

    comandline_parse(argc, argv);

    MasterServer server(globalArgs.ipAddr, globalArgs.port, globalArgs.directory);

    server.run();


    return 0;
}

