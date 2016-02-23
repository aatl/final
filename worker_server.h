#ifndef WORKER_SERVER_H_
#define WORKER_SERVER_H_
#include <ev++.h>
#include "http_parser.h"
#include <map>


class WorkerServer {
    struct ev_loop *_loop;
    std::string _rootDir;
    HttpParser parser;
    int _sock;
    std::map<int, std::string> _data;

public:
    WorkerServer(int fd, std::string &rootDir);
    void run();

private:
    void getFd(ev::io &watcher, int revents);
    size_t recvFd(int _sock, char *buf, int buflen, int *fd);
    
    void readClientData(ev::io &watcher, int revents);
};

#endif
