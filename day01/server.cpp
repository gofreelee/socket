#include<sys/types.h>
#include<sys/socket.h>
#include<iostream>
#include<arpa/inet.h>
#include<iostream>
#include<string.h>
#include<unistd.h>
#define ip "127.0.0.1"
#define port 9111
int main(){
    int listen_sock=socket(AF_INET,SOCK_STREAM,0);//监听socket
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(ip);
    addr.sin_port=htons(port);
    //bind 绑定
    if(bind(listen_sock,(struct sockaddr*)&addr,sizeof(addr))==-1){
        std::cout<<"绑定端口失败"<<std::endl;
        return -1;
    }
    if(listen(listen_sock,SOMAXCONN)!=0){
        std::cout<<"监听端头失败"<<std::endl;
        return -1;
    }
    while(true){
        //要accept 一个
        struct sockaddr_in client_addr;
        socklen_t client_addr_len_t=sizeof(client_addr);
        int client_sock=accept(listen_sock,(struct sockaddr*)&client_addr
                         ,&client_addr_len_t);
        char *message="i have got the message";
        char buffer[32]={};
        int rev_num=0;
        if((rev_num=recv(client_sock,(void*)buffer,32,0))>0){
            for(int i=0;i<rev_num;i++){
                std::cout<<buffer[i];
            }
            std::cout<<std::endl;
            int send_num=send(client_sock,(void*)message,strlen(message),0);
            if(send_num!=strlen(message)){
                std::cout<<"发送错误"<<std::endl;
            }
        }
        else {
                std::cout<<"接受错误"<<std::endl;
        }
        close(client_sock);
    }
    close(listen_sock);    
}