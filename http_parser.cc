#include "http_parser.h"

HttpParser::HttpParser() {
    headers["host"] = "0";
    headers["contenttype"] = "0";

}

void HttpParser::parse(const std::string &str) {
    ss.str(str);

    std::string tmp;

    ss>>command;
    ss>>directory;
    ss>>tmp;

    std::transform(command.begin(), command.end(), command.begin(), ::tolower);


    while(ss && !ss.eof()) {

        ss>>tmp;
    }

    ss.clear();

}


std::string HttpParser::getDirectory() const
{
    return directory;
}

void HttpParser::clear()
{
    command = "";
    directory = "";
}


std::string HttpParser::getCommand() const
{
    return command;
}
