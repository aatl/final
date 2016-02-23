#ifndef CMD_PARSE_H_
#define CMD_PARSE_H_
#include <string>

struct globalArgs_t {
    std::string ipAddr;
    int port;
    std::string directory;

} ;


extern globalArgs_t globalArgs;
extern const char *optString;

void comandline_parse(int argc, char* argv[]);


#endif
