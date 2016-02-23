#include "worker_server.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <algorithm>

const int READ_BUFSIZE = 1024;

const int OK_STATUS = 200;
const int NOT_FOUND_STATUS = 404;

void WorkerServer::readClientData(ev::io &watcher, int revents)
{




//    std::string debugStr = "HTTP/1.0 200 OK\r\n ContentType: text/html\r\n ContentLength: 14\r\n\r\n Hello, World!\n";
//    send(watcher.fd, debugStr.c_str(), debugStr.size(), MSG_NOSIGNAL);
////    _data.erase(watcher.fd);
//    shutdown(watcher.fd, SHUT_RDWR);
//    close(watcher.fd);
//    delete &watcher;
//    return;



    char buf[READ_BUFSIZE];
    int len = recv(watcher.fd, buf, READ_BUFSIZE, MSG_NOSIGNAL);
    std::string endSign = "\r\n\r\n";

    if(len == 0) {
        _data.erase(watcher.fd);
        shutdown(watcher.fd, SHUT_RDWR);
        close(watcher.fd);
        delete &watcher;
        return;
    } else if(len < 0) {

        std::cerr<<"error reading from fd "<<watcher.fd<<std::endl;
        return;

    } else {

        std::string &refString = _data[watcher.fd].append(buf, len);

        //need optimization
        if(std::search(refString.begin(), refString.end(), endSign.begin(), endSign.end()) != refString.end()) {
                parser.parse(refString);

                if (parser.getCommand().compare("get") == 0) {


//                    std::cerr<<"recieved get request"<<std::endl;

                    std::string ret;
                    ret.append("HTTP/1.0 ");
                    int status;

                    const std::string &tmpPath = parser.getDirectory();
                    std::string filePath;

                    auto haveParam = std::find(tmpPath.begin(), tmpPath.end(), '?');
                    if(!tmpPath.empty()) {
                        if(tmpPath[0] == '/')
                            filePath = std::string(tmpPath.begin() + 1, haveParam);
                        else
                            filePath = std::string(tmpPath.begin(), haveParam);
                    }


                    std::cerr<<filePath.c_str()<<std::endl;

                    struct stat st;
                    int statcode = lstat((_rootDir + filePath).c_str(), &st);


                    if(statcode == -1) {
                        errno = 0;
//                        std::cerr<<"File "<<(_rootDir + filePath).c_str()<<" not found"<<std::endl;
                        status = NOT_FOUND_STATUS;
                        ret.append(std::to_string(status));
                        ret.append(" NOT FOUND\r\n");
                        ret.append("ContentType: text/html\r\n");
                        ret.append("\r\n");


                    } else if(S_ISREG(st.st_mode)) {


                        int ffd = open((_rootDir + filePath).c_str(), O_RDONLY, 0440);
//                        std::cerr<<"File "<<(_rootDir + filePath).c_str()<<" is OK"<<std::endl;
                        status = OK_STATUS;
                        ret.append(std::to_string(status));
                        ret.append(" OK\r\n");
                        ret.append("ContentType: text/html\r\n");

                        off_t fileSize =  st.st_size;
                        std::vector<char> respString(fileSize);

                        int len = 0;
                        off_t seek = 0;
                        while(len = read(ffd, &(respString[0]) + seek, fileSize - seek )) {
                            if(len > 0)
                                seek+= len;
                        }

                        close(ffd);


                        ret.append("Content-Length: ");
                        ret.append(std::to_string(fileSize));
                        ret.append("\r\n\r\n");
                        ret.append(respString.begin(), respString.end());

                    } else if(S_ISDIR(st.st_mode)) {


//                        std::cerr<<"File "<<(_rootDir + filePath).c_str()<<" is OK"<<std::endl;
                        status = OK_STATUS;
                        ret.append(std::to_string(status));
                        ret.append(" OK\r\n");
                        ret.append("ContentType: text/html\r\n");


                    }

//                    std::cerr<<"result string is: "<<ret.c_str()<<std::endl;

                    off_t send_seek = 0;
                    int cur_sended = 0;
//                    std::cerr<<"send_seek = "<<send_seek<<" "<<"cur sended = "<<cur_sended<<std::endl;
                    while((cur_sended = send(watcher.fd, ret.c_str() + send_seek, ret.size() - send_seek, MSG_NOSIGNAL)) != ret.size() - send_seek) {
                        if(cur_sended > 0)
                            send_seek += cur_sended;

                    }



//                    std::cerr<<"Writing OK"<<std::endl;

                    parser.clear();
                    watcher.stop();
                    shutdown(watcher.fd, SHUT_RDWR);
                    close(watcher.fd);
                    _data.erase(watcher.fd);
//                    watcher.~io();
                    delete &watcher;
                    return;

                }
        }



    }


}

WorkerServer::WorkerServer(int fd, std::string &rootDir)
{
    _sock = fd;
    _rootDir = rootDir;
}

void WorkerServer::run()
{
    _loop = ev_default_loop(0);
    ev::io event(_loop);
    event.set<WorkerServer, &WorkerServer::getFd>(this);
    event.set(_sock, ev::READ);
    event.start();
    while(true) {
        ev_loop(_loop, 0);
    }



}

void WorkerServer::getFd(ev::io &watcher, int revents)
{

//    std::cerr<<"get fd from master"<<std::endl;

    char buf[1];
    buf[0] = 0;
    int fd = -1;
    int size = recvFd(watcher.fd, buf, sizeof(buf), &fd);

//    std::cerr<<"geting fd is "<<fd<<std::endl;

//    std::cerr<<"size is "<<size<<std::endl;

    if(size > 0 && fd > 0) {
        ev::io *event = new ev::io(_loop);
        event->set<WorkerServer, &WorkerServer::readClientData> (this);
        event->set(fd, ev::READ);
        event->start();
    }

}

size_t WorkerServer::recvFd(int sock, char *buf, int buflen, int *fd)
{

    struct iovec iov;
    struct msghdr msg;
    struct cmsghdr* cmsg;

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


    msg.msg_control = cmsgu.control;
    msg.msg_controllen = sizeof(cmsgu.control);

    int size = recvmsg(sock, &msg, 0);

    cmsg = CMSG_FIRSTHDR(&msg);

    if(cmsg->cmsg_len == CMSG_LEN(sizeof(int))) {
        if(cmsg->cmsg_level != SOL_SOCKET) {
            std::cerr<<"error recieving cmsg_level"<<std::endl;
            return -1;
        }

        if(cmsg->cmsg_type != SCM_RIGHTS) {
            std::cerr<<"error recieving cmsg_type"<<std::endl;
            return -1;
        }


        *fd = (*(int*) CMSG_DATA(cmsg));
        std::cerr<<"recieved fd "<<*fd<<"to Worker"<<std::endl;


    }

    return size;



}
