#pragma once

#include"err.hpp"

#include<iostream>
#include<string>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<cstdlib>
#include<cstring>
#include<unistd.h>
#include<functional>


namespace ns_server
{
    static const int backlog = 32;
    using func_t = std::function<std::string(const std::string &)>;

    class TcpServer
    {
    public:
        TcpServer(func_t func,const uint16_t port)
            :port_(port)
            ,func_(func)
        {}

        void InitServer()
        {
            // 1.创建 socket
            listensock_=socket(AF_INET,SOCK_STREAM,0);
            if(listensock_<0)
            {
                cerr<<"create socket error" << strerror(errno)<<endl;
                exit(SOCKET_ERR);
            }
            cout<<"create socket success:"<<listensock_<<endl;

            //2. bind
            struct sockaddr_in local;
            memset(&local,0,sizeof(local));
            local.sin_family=AF_INET;
            local.sin_port=htons(port_);
            local.sin_addr.s_addr=htonl(INADDR_ANY);

            if(bind(listensock_,(struct sockaddr*)&local,sizeof(local)) < 0)
            {
                cerr<<"bind error "<<strerror(errno)<<endl;
                exit(BIND_ERR);
            }
            cout<<"bind success"<<endl;

            // 3. lsiten
            if( listen(listensock_,backlog) < 0)
            {
                cerr<<"listen error "<<strerror(errno)<<endl;
                exit(LISTEN_ERR);
            }
            cout<<"listen success"<<endl;

        }

        void start()
        {
            quit_=false;
            while(!quit_)
            {
                // 4. accept
                struct sockaddr_in client;
                socklen_t len=sizeof(client);
                int sock=accept(listensock_,(struct sockaddr*)&client,&len);
                if(sock < 0)
                {
                    cerr<<"accept error "<<strerror(errno)<<endl;
                    exit(ACCEPT_ERR);
                }

                string clientip=inet_ntoa(client.sin_addr);
                uint16_t clientport=ntohs(client.sin_port);

                // 5. 开始业务处理
                cout<<"获取新连接成功："<<sock<<"from"<<listensock_<<\
                  ","<<clientip<<" - "<<clientport<<endl;

                service(sock,clientip,clientport);
            }

        }

        void service(int sock, string ip, uint16_t port)
        {
            string name=ip+"-"+std::to_string(port);
            char buffer[1024];
            while(true)
            {
                ssize_t n=read(sock,buffer,sizeof(buffer)-1);
                if(n > 0)
                {
                    buffer[n]=0;
                    string response=func_(buffer);
                    cout<< name <<":" <<response<<endl;

                    write(sock,response.c_str(),response.size());
                }
                else if(n == 0) // 对方关闭连接
                {
                    close(sock);
                    cout<<name<<" quit,me too"<<endl;
                    break;
                }
                else if(n < 0)
                {
                    close(sock);
                    cerr<<"read error"<<strerror(errno)<<endl;
                    break;
                }
            }
        }

    private:
        uint16_t port_;
        int listensock_;
        bool quit_;
        func_t func_;
    };
}