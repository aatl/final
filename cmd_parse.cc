#include "cmd_parse.h"
#include <unistd.h>
#include <cstdlib>
#include <iostream>

struct globalArgs_t globalArgs {
    "127.0.0.1",
    12345,
    ""
};

const char *optString = "h:p:d:";

//::directory = "";
//struct globalArgs_t globalArgs::ipAddr = "127.0.0.1";
//struct globalArgs_t globalArgs::port = "8080";


void comandline_parse(int argc, char *argv[]) {


    int opt = getopt(argc, argv, optString);
    while( opt != -1) {
        switch (opt) {
        case 'h':
            globalArgs.ipAddr = optarg;
            break;
        case 'p':
            globalArgs.port = atoi(optarg);
            break;
        case 'd':
            globalArgs.directory = optarg;
            break;
        default:

            std::cerr<<"invalid arguments"<<std::endl;
            exit(EXIT_FAILURE);
        }



        opt = getopt(argc, argv, optString);
    }

}
