#ifndef SERVER_H
#define SERVER_H

#include "global.h"

class server{
    private:
        int server_port;
        int server_sockfd;
        string server_ip;
        static vector<bool> sock_arr;//改为了静态成员变量，且类型变为vector<bool>
    public:
        server(int port,string ip);
        ~server();
        void run();
        static void RecvMsg(int conn);
};
#endif