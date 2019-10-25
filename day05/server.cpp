#include<sys/types.h>
#include<sys/epoll.h>
#include<sys/errno.h>
#include<sys/socket.h>
#include<iostream>
#include<vector>
#include<arpa/inet.h>
#include<fcntl.h>
#include<poll.h>
#include<unistd.h>

#define port 8776

int main(){
     int listen_fd=socket(AF_INET,SOCK_STREAM,0);
     if(listen_fd<0){
         std::cout<<"创建套接字失败"<<std::endl;
         close(listen_fd);
         return -1;
     }
     int oldFlag=fcntl(listen_fd,F_GETFL,0);
     int newFlag=oldFlag|O_NONBLOCK;
     if(fcntl(listen_fd,F_SETFD,newFlag)==-1){
         std::cout<<"设置非阻塞时出错"<<std::endl;
     }
     struct sockaddr_in sockaddr;
     sockaddr.sin_family=AF_INET;
     sockaddr.sin_addr.s_addr=htonl(INADDR_ANY);
     sockaddr.sin_port=htons(port);
     if(bind(listen_fd,(struct sockaddr*)&sockaddr,sizeof(sockaddr))==-1){
         std::cout<<"绑定地址端口失败"<<std::endl;
         close(listen_fd);
         return -1;
     }
     if(listen(listen_fd,1024)<0){
         std::cout<<"监听套接字失败"<<std::endl;
         close(listen_fd);

     }
     int epoll_fd=epoll_create(1);
     epoll_event epoll_event_listen;
     epoll_event_listen.events=EPOLLIN;
     epoll_event_listen.events |= EPOLLET;
     epoll_event_listen.data.fd=listen_fd;
     if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,listen_fd,&epoll_event_listen)<0){
         std::cout<<"设置epoll错误"<<std::endl;
         close(listen_fd);
         return -1;
     }
     int num;
     epoll_event events[1024];
     while(true){
        num=epoll_wait(epoll_fd,events,1024,1000);
        if(num<0){
            if(errno==EINTR){
                continue;
            }
            return -1;
        }
        else if(num==0){
            continue;
        }
        else {
            for(int i=0;i<num;i++){
                if(events[i].events&EPOLLIN){
                    //处理新连接的套接字.
                    if(events[i].data.fd==listen_fd){
                        int client_fd=accept(listen_fd,NULL,NULL);
                        if(client_fd!=-1){
                            //已经建立新连接
                            int oldFlag=fcntl(client_fd,F_GETFL,0);
                            int newFlag=oldFlag|O_NONBLOCK;
                            if(fcntl(client_fd,F_SETFD,newFlag)==-1){
                                std::cout<<client_fd<<"设置非阻塞套接字失败"<<std::endl;
                                close(client_fd);
                            }
                            else {
                                epoll_event client_epoll_event;
                                client_epoll_event.data.fd=client_fd;
                                client_epoll_event.events=EPOLLIN|EPOLLOUT|EPOLLET;
                                if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_fd,&client_epoll_event)==-1){
                                    std::cout<<client_fd<<"注册epoll失败"<<std::endl;
                                    close(client_fd);
                                }
                                else{
                                    std::cout<<client_fd<<"已被注册"<<std::endl;
                                }
                            }
                        }
                    }
                    else{
                        //有套接字的数据读
                        //这里为了测试lT模式,一次读一个.
                        char ch;
                        int read_ret=recv(events[i].data.fd,&ch,1,0);
                        if(read_ret==0){
                            if(epoll_ctl(epoll_fd,EPOLL_CTL_DEL,events[i].data.fd,NULL)!=-1){
                                std::cout<<events[i].data.fd<<"断开连接"<<std::endl;
                                close(events[i].data.fd);
                            }
                        }
                        else if(read_ret<=0){
                            if(errno!=EINTR&&errno!=EWOULDBLOCK){
                                if(epoll_ctl(epoll_fd,EPOLL_CTL_DEL,events[i].data.fd,NULL)!=-1){
                                std::cout<<events[i].data.fd<<"断开连接"<<std::endl;
                                close(events[i].data.fd);
                                }
                            }

                        }
                        else{
                            std::cout<<"收到数据"<<ch<<std::endl;
                        }
                    }
                }
                // else if(events[i].events=EPOLLOUT){

                // }
            }

        }
     }
}