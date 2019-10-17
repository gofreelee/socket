#include<sys/select.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<iostream>
#include<vector>
#include<unistd.h>
using namespace std;
#define ip "127.0.0.1"
#define port 8776

int main(){
    int listen_socket=socket(AF_INET,SOCK_STREAM,0);
    if(listen_socket==-1){
        cout<<"创建监听套接字有误"<<endl;
        return -1;
    }
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len=sizeof(client_addr);
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(listen_socket,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1){
        cout<<"绑定套接字失败"<<endl;
        return -1;
    }
    //开始监听
    int listen_type=listen(listen_socket,1024);//监听状态
    if(listen_type==-1){
        cout<<"监听失败"<<endl;
        return -1;
    }
    int maxfd=listen_socket;
    vector<int>client_set;
    fd_set readset;
    int ret;
    while(true){
        FD_SET(listen_socket,&readset);
        //设置客户端读事件
        for(int i=0;i<client_set.size();i++){
            FD_SET(client_set[i],&readset);
        }
        struct timeval timer;
        timer.tv_usec=1;
        timer.tv_usec=0;
        ret=select(maxfd+1,&readset,nullptr,nullptr,&timer);
        if(ret==-1){
            cout<<"select有误"<<endl;
            break;
        }
        else if(ret==0){
            cout<<"超时"<<endl;
        }
        else if(ret==1){
            if(FD_ISSET(listen_socket,&readset)){
               //有新的客户端与服务器三次握手,把这个客户端放到服务器中去.
               int client_fd=accept(listen_socket,(struct sockaddr*)&client_addr,&client_addr_len);
               if(client_fd!=-1){
                   client_set.push_back(client_fd);
                   if(client_fd>maxfd)maxfd=client_fd;
                   cout<<"新连接一个客户端"<<endl;
               }
               else {
                   cout<<"三次握手错误"<<endl;
               }
            }
            else {
                //有客户端的数据来了;
                char rcvbuffer[64]={0};
                for(int i=0;i<client_set.size();i++){
                    if(client_set[i]!=-1&&FD_ISSET(client_set[i],&readset)){
                        //收到消息了
                        int length=recv(client_set[i],rcvbuffer,64,0);
                        if(length<0&&errno!=EINTR){
                            close(client_set[i]);
                            client_set[i]=-1;
                            cout<<"关闭一个错误连接"<<endl;
                            continue;
                        }
                        //
                        cout<<rcvbuffer<<endl;
                    }
                }
            }

        }
    }
    for(int i=0;i<client_set.size();i++){
        if(client_set[i]!=-1){
            close(client_set[i]);
        }
    }
    close(listen_socket); 
    return 0;
}