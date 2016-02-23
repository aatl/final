#ifndef MASTER_SERVER_CC
#define MASTER_SERVER_CC

#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ev++.h>
#include <vector>





class MasterServer {
    int _sock;
    int _numCpu;
    int _rr_cnt;
    std::string _rootDir;
    struct ev_loop *loop;
    std::vector<int> worker_sockets;

    void readEvent(struct ev::io &watcher, int revent);
    void accesEvent(struct ev::io &watcher, int revent);
    ssize_t sendFd(int sock, char *buf, int buflen, int fd);

    void initSock(std::string hostStr, int port);
    void initWorkers();

public:
    MasterServer(std::string hostStr, int port, std::string dirStr);
    void run();
};


#endif //MASTER_SERVER_CC
