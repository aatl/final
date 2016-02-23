#ifndef HTTP_PARSER_H_
#define HTTP_PARSER_H_
#include <string>
#include <sstream>
#include <map>

#include <algorithm>

class HttpParser {
    std::string command;
    std::string directory;
    std::stringstream ss;

    std::map<std::string, std::string> headers;
//    std::string command;
//    std::string Host;
//    std::string ContentType;

public:
    HttpParser();

    /* {
    headers["host"] = "0";
    headers["contenttype"] = "0";

    }*/

    void parse(const std::string &str);

    std::string getCommand() const;
    std::string getDirectory() const;
    void clear();
};



#endif
