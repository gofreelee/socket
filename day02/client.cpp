#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<iostream>
#include<unistd.h>
using namespace std;
#define ip "127.0.0.1"
#define port 8773
void send_all(int client_socket,void* buf,int length){
    int size=length;
      while(size>0){
          int send_length=send(client_socket,buf,length,0);
          cout<<"向缓冲区写入了一次数据"<<endl;
          size-=send_length;
          buf=buf+send_length;
      }
      return;
}
int main(){
    int client_socket=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family=AF_INET;
    server_sockaddr.sin_addr.s_addr=inet_addr(ip);
    server_sockaddr.sin_port=htons(port);
    if(connect(client_socket,(struct sockaddr*)&server_sockaddr,sizeof(server_sockaddr))==-1){
        cout<<"三次握手错误"<<endl;
        return -1;
    }
    char content[102400];
    for(int i=0;i<102400;i++){
        content[i]=0;
    }
    cout<<"开始发送数据"<<endl;
    send_all(client_socket,content,102400);
    cout<<"客户端数据发送完毕"<<endl;
    close(client_socket);
}