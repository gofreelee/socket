#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<iostream>
#include<string.h>
/*
socket 的客户端使用.先创建一个套接字.然后该套接字要去连接服务器的端口
socket(),
htons()
iner_addr()
connect()
send()
recv()
close()
 */
#define ip "127.0.0.1"
#define port 9111
int main(){
   int client_socket=socket(AF_INET,SOCK_STREAM,0);//这个第三个参数是用来?
   if(client_socket==-1){
       std::cout<<"创建套接字失败";
   }
   struct sockaddr_in addr;
   addr.sin_family=AF_INET;
   addr.sin_addr.s_addr=inet_addr(ip);
   addr.sin_port=htons(port);
   if(connect(client_socket,(struct sockaddr*)&addr,sizeof(addr))==-1){
   std::cout<<"向服务器连接端口失败"<<std::endl;
   return -1;
   }
   char* content="hello world";
   int send_info=send(client_socket,(void*)content,strlen(content),0);
   if(send_info!=strlen(content)){
       std::cout<<"发送数据有误"<<std::endl;
       return -1;
   }
   std::cout<<"成功发送数据"<<std::endl;
   char recvbuff[32]={0};
   int recv_info=recv(client_socket,recvbuff,strlen(recvbuff),0);
   if(recv_info==strlen(recvbuff)){
       std::cout<<"成功接受到服务端发回的数据";
   }
   else{
       std::cout<<"未成功接受服务端发回数据";
       return -1;
   }
   return 0;
}