#include "master_server.h"
#include <unistd.h>
#include <iostream>
#include "worker_server.h"

void MasterServer::readEvent(struct ev::io &watcher, int revent) {
    char buf[1024];
    int len = read(watcher.fd, buf, 1024);


    if(len == 0) {

        close(watcher.fd);
        delete &watcher;

    } else if(len < 0) {
        std::cerr<<"error in reading from fd "<<watcher.fd<<std::endl;
        return;
    } else if (len > 0) {

        write(watcher.fd, buf, len);

    }


}

void MasterServer::accesEvent(struct ev::io &watcher, int revent) {
    int fd = accept(_sock, NULL, NULL);

    if(fd < 0) {
        std::cerr<<"invalid descriptor"<<std::endl;
        return;
    }


    char buf[1];
    buf[0] = 0;

    sendFd(worker_sockets[_rr_cnt], buf, sizeof(buf), fd);
    _rr_cnt = (_rr_cnt + 1) % _numCpu;

    close(fd);



//    if(size > 0) {
//        ev::io *event = new ev::io;
//        event->set<MasterServer, &MasterServer::readEvent>(this);
//        event->set(fd, ev::READ);
//        event->start();
//    }

}

ssize_t MasterServer::sendFd(int sock, char *buf, int buflen, int fd)
{
    struct iovec iov;
    struct cmsghdr *cmsg;
    struct msghdr msg;

    union {
        struct cmsghdr cmsghdr;
        char control[CMSG_SPACE(sizeof(int))];
    } cmsgu;


    iov.iov_base = buf;
    iov.iov_len = buflen;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    if(fd) {
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);
        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;


        (*(int*) CMSG_DATA(cmsg)) = fd;
        std::cerr<<"passing fd "<<fd<<" from master"<<std::endl;
    }

    ssize_t size = sendmsg(sock, &msg, 0);

    if(size < 0)
        std::cerr<<"fd "<<fd<<" not passing from master"<<std::endl;


    return size;



}



void MasterServer::initSock(std::string hostStr, int port) {
    if((_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        std::cerr<<"error socket init"<<std::endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in sin;
    std::memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    int flag = 1;
    setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

    if (inet_pton(AF_INET, hostStr.c_str(), &(sin.sin_addr.s_addr)) != 1) {
        std::cerr<<"invalid adress\n"<<std::endl;
        exit(EXIT_FAILURE);
    }


    if(bind(_sock, (struct sockaddr*)&sin, sizeof(sin)) == -1) {
        std::cerr<<"invalid adress binding"<<std::endl;
        exit(EXIT_FAILURE);
    }


    listen(_sock, SOMAXCONN);


}

const int master_usock_fd = 0;
const int worker_usock_fd = 1;

void MasterServer::initWorkers()
{
    int sv[2];
    for(int i = 0; i < _numCpu; ++i) {
        socketpair(AF_UNIX, SOCK_STREAM, 0, sv);


        if(fork() == 0) {
            WorkerServer w(sv[master_usock_fd], _rootDir);
            std::cerr<<"on worker: unix socket to master is "<<sv[master_usock_fd]<<std::endl;


            close(sv[worker_usock_fd]);

            for(auto j = worker_sockets.begin(); j != worker_sockets.end(); ++j) {
                close(*j);
                std::cerr<<"on worker removing vector of sockets "<<*j<<std::endl;

            }

            w.run();
        }

        close(sv[master_usock_fd]);
        worker_sockets.push_back(sv[worker_usock_fd]);
    }

}

MasterServer::MasterServer(std::string hostStr, int port, std::string dirStr) {
    daemon( 1, 0);
    initSock(hostStr, port);

    if(dirStr.empty()) {
        dirStr = "/";
    } else  if (dirStr.back() != '/') {
        dirStr.push_back('/');
    }
    _rootDir = dirStr;

    if((_numCpu = sysconf(_SC_NPROCESSORS_ONLN)) == -1 ) {
        _numCpu = 2;
        errno = 0;
    }

    initWorkers();
    _rr_cnt = 0;

}

void MasterServer::run() {

    loop = ev_default_loop(0);

    ev::io event(loop);
    event.set<MasterServer, &MasterServer::accesEvent>(this);
    event.set(_sock, ev::READ);
    event.start();


    while(true)
        ev_loop(loop, 0);


}
