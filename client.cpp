#include "client.h"

client::client(int port,string ip)
:server_port(port),server_ip(ip){}
client::~client(){
    close(sock);
}

void client::run()
{
    //定义sockfd
    sock = socket(AF_INET, SOCK_STREAM,0);

    //定义sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(server_port);
    servaddr.sin_addr.s_addr = inet_addr(server_ip.c_str());

    //连接服务器，成功返回0，错误返回-1
    if(connect(sock, (struct sockaddr *)&servaddr,
    sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }
    cout << "连接服务器成功\n";

    HandleClient(sock);
/*
    //创建发送线程和接收线程
    thread send_t(SendMsg, sock), recv_t(RecvMsg,sock);
    send_t.join();
    cout << "发送线程已结束\n";
    
    recv_t.join();
    cout << "接收线程已结束\n";
*/
    return;
}
void client::HandleClient(int conn)
{
    int choice;
    string name,pass,pass1;
    bool if_login=false;//记录是否登录成功
    string login_name;//记录成功登录的用户名

    cout<<" ------------------\n";
    cout<<"|                  |\n";
    cout<<"| 请输入你要的选项:|\n";
    cout<<"|    0:退出        |\n";
    cout<<"|    1:登录        |\n";
    cout<<"|    2:注册        |\n";
    cout<<"|                  |\n";
    cout<<" ------------------ \n\n";

    //开始处理各种事物
    while(1)
    {
        if(if_login)
        {
            break;
        }
        cin >> choice;
        if(choice == 0)
        {
            break;
        }
        //注册
        else if(choice == 2)
        {
            cout<< "请输入用户名：";
            cin >> name;
            while(1)
            {
                cout << "密码：";
                cin >> pass;
                cout << "确认密码：";
                cin >> pass1;
                if(pass == pass1)
                {
                    break;
                }else
                {
                    cout << "两次密码不一致!\n\n";
                }
            }
            name = "name:"+name;
            pass = "pass:"+pass;
            string str = name + pass;
            send(conn, str.c_str(), str.length(), 0);
            cout << "注册成功!\n";
            cout << "\n继续输入你要的选项:";
        }
        //登录
        else if(choice == 1 && !if_login)
        {
            while(1)
            {
                cout << "用户名: ";
                cin >> name;
                cout << "密码：";
                cin >> pass;
                //格式化
                string str = "login" + name;
                str += "pass:";
                str += pass;
                send(sock, str.c_str(), str.length(), 0);//发送登录信息
                char buffer[1000];
                memset(buffer, 0, sizeof(buffer));
                recv(sock, buffer, sizeof(buffer), 0);
                string recv_str(buffer);
                //登录成功
                if(recv_str.substr(0,2) == "ok")
                {
                    if_login = true;
                    login_name = name;
                    cout << "登录成功\n\n";
                    break;
                }
                //登录失败
                else
                {
                    cout << "密码或用户名错误！\n\n";
                }
            }
        }
    }
    
    //登录成功
    while(if_login&&1)
    {
        if(if_login)
        {
            system("clear");//清空终端
            cout << "           欢迎回来,"<<login_name << endl;
            cout<<" -------------------------------------------\n";
            cout<<"|                                           |\n";
            cout<<"|          请选择你要的选项：               |\n";
            cout<<"|              0:退出                       |\n";
            cout<<"|              1:发起单独聊天               |\n";
            cout<<"|              2:发起群聊                   |\n";
            cout<<"|                                           |\n";
            cout<<" ------------------------------------------- \n\n";
        }
        cin >> choice;
        if(choice == 0)
        {
            break;
        }
        //私聊
        if(choice == 1)
        {
            cout << "请输入对方的用户名:";
            string target_name, content;
            cin >> target_name;
            string sendstr("target:"+target_name+"from:"+login_name);
            send(sock, sendstr.c_str(),sendstr.length(),0);
            cout << "请输入你想说的话(输入exit退出)：\n";
            thread t1(client::SendMsg, conn);
            thread t2(client::RecvMsg, conn);
            t1.join();
            t2.join();
        }
    }
    close(sock);
}  
void client::SendMsg(int conn)
{  
    char sendbuf[100];
    while(1)
    {
        string str;
        cin >> str;
        str = "content:"+str;
        int ret = send(conn, str.c_str(), str.length(),0);
        //输入exit或者对端关闭时结束
        if(str == "content:exit" || ret <= 0)
        {
            break;
        }
    }
}

void client::RecvMsg(int conn)
{
    char buffer[1000];
    while(1)
    {
        memset(buffer, 0, sizeof(buffer));
        int len = recv(conn, buffer, sizeof(buffer), 0);
        if(len <= 0)
        {
            break;
        }
        cout<<buffer<<endl;
    }
}
