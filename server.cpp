#include "server.h"
vector<bool> server::sock_arr(1000,false);

server::server(int port, string ip)
:server_port(port),server_ip(ip)
{}

server::~server()
{
    for(int i = 0; i < sock_arr.size(); i++ )
    {
        if(sock_arr[i])
        {
            close(i);
        }
    }
    close(server_sockfd);
}

void server::run()
{
    //定义sockfd
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;//TCP/IP协议族
    server_sockaddr.sin_port = htons(server_port);
    server_sockaddr.sin_addr.s_addr = inet_addr(server_ip.c_str());

    //bind,成功返回0,出错返回-1
    if(bind(server_sockfd, (struct sockaddr *)&server_sockaddr,
    sizeof(server_sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }

    //listen,成功返回0,出错返回-1
    if(listen(server_sockfd,20) == -1)
    {
        perror("listen");
        exit(1);
    }

    //客户端套接字
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);

    //不断取出新连接并创建子线程为其服务
    while(1)
    {
        int conn = accept(server_sockfd, (struct sockaddr*)&client_addr,&length);
        if(conn<0)
        {
            perror("connect");
            exit(1);
        }
        cout << "文件描述符为"<<conn<<"的客户端成功连接\n";
        sock_arr.push_back(conn);
        //创建线程
        thread t(server::RecvMsg, conn);
        t.detach();//置为分离状态，不能用join,join会导致主线程阻塞
    }
}

//子线程工作的静态函数
//注意，前面不能加static,否则会编译错误
void server::RecvMsg(int conn)
{
    //接收缓冲区
    char buffer[1000];
    //不断接收数据
    while(1)
    {
        memset(buffer, 0, sizeof(buffer));
        int len = recv(conn, buffer, sizeof(buffer), 0);
        //客户端发送exit或者异常结束时，退出
        if(strcmp(buffer, "exit")==0 || len <= 0)
        {
            close(conn);
            sock_arr[conn] = false;
            break;
        }
        cout<<"收到套接字描述符为"<<conn<<"发来的信息: "<<buffer<<endl;
        //回复客户端
        string ans = "收到";
        int ret = send(conn, ans.c_str(),ans.length(), 0);
        //服务器收到exit或者异常关闭套接字描述符
        if(ret <= 0)
        {
            close(conn);
            sock_arr[conn] = false;
            break;
        }
    }
}



























