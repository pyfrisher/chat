#ifndef SERVER_H
#define SERVER_H

#include "global.h"

class server{
    private:
        int server_port;
        int server_sockfd;
        string server_ip;
        static vector<bool> sock_arr;//改为了静态成员变量，且类型变为vector<bool>
        static unordered_map<string, int> name_sock_map;//名字和套接字描述符
        static pthread_mutex_t name_sock_mutx;//互斥锁，锁住需要修改name_sock_map的临界区
    public:
        server(int port,string ip);
        ~server();
        void run();
        static void RecvMsg(int conn);
        static void HandleRequest(int conn, string str, tuple<bool, string, string, int> &info);
};
#endif