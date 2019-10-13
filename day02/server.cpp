#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<iostream>
#include<errno.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
using namespace std;

#define ip "127.0.0.1"
#define port 8773

//读操作
void read_data(int conn,void* buff,int length){
    int i=1;
     while(true){
        cout<<"服务端读数据第"<<i<<"次"<<endl;
        int n=recv(conn,buff,length,0);
        if(n==0){
            return;
        }
        cout<<strerror(errno);
        cout<<n;
        i++;
     }
    return ;
}
int main(){
    int listen_socket=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serveraddr;
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr=inet_addr(ip);
    serveraddr.sin_port=htons(8773);
    if(bind(listen_socket,(struct sockaddr*)&serveraddr,sizeof(serveraddr))==-1){
        cout<<"绑定端口错误"<<endl;
    }
    if(listen(listen_socket,1024)==-1){
        cout<<"监听端口错误"<<endl;
    }
    char *buff=(char*)malloc(1024);
    while (true)
    {
        int conn_socket=accept(listen_socket,(struct sockaddr*)NULL,NULL);
        cout<<conn_socket;
        read_data(conn_socket,buff,1024);
        close(conn_socket);        
    }
    close(listen_socket);
}