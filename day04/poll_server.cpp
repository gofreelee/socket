#include<sys/types.h>
#include<sys/socket.h>
#include<poll.h>
#include<iostream>
#include<arpa/inet.h>
#include<vector>
#include<unistd.h>
#include<errno.h>
#define port 8773
int main(){
    int listen_socket=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in sockaddr;
    sockaddr.sin_family=AF_INET;
    sockaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    sockaddr.sin_port=htons(port);

    //绑定
    if(bind(listen_socket,(struct sockaddr*)&sockaddr,sizeof(sockaddr))!=0){
        std::cout<<"bind套接字失败"<<std::endl;
        close(listen_socket);
        return -1;
    }
    //监听套接字.
    if(listen(listen_socket,1024)!=0){
        std::cout<<"listen套接字失败"<<std::endl;
        close(listen_socket);
        return -1;
    }
    std::vector<pollfd>  pollfd_vec;
    pollfd listen_pollfd;
    listen_pollfd.fd=listen_socket;
    listen_pollfd.events=POLLIN;
    listen_pollfd.revents=0;
    pollfd_vec.push_back(listen_pollfd);
    //之后可以进入循环了
    while(true){
        int poll_ret=poll(&pollfd_vec[0],pollfd_vec.size(),1000);
        if(poll_ret==-1){
             if(errno==EINTR){
                 continue;
             }
             break;
        }
        else if(poll_ret==0){
            continue;
        }
        else {
            if(pollfd_vec[0].revents==POLLIN){
                //监听套接字来事件了,说明有connect了.
                int accept_socket=accept(listen_socket,NULL,NULL);//不关心客户端的地址.
                if(accept_socket!=-1){
                    //套接字有效
                    pollfd client_pollfd;
                    client_pollfd.fd=accept_socket;
                    client_pollfd.events=POLLIN;
                    client_pollfd.revents=0;
                    std::cout<<"新连接一个套接字"<<std::endl;
                    pollfd_vec.push_back(client_pollfd);
                }
            }
            for(int i=0;i<pollfd_vec.size();i++){
                if(pollfd_vec[i].fd!=listen_socket){
                    //有客户端来消息了.
                    char buff[64]={0};
                    if(recv(pollfd_vec[i].fd,buff,64,0)==-1){
                        if(errno==EINTR){
                            continue;
                        }
                        //不是进程被挂起就是套接字出问题.
                        close(pollfd_vec[i].fd);
                        pollfd_vec[i].fd=-1;
                        break;
                    }
                    else {
                        std::cout<<buff;
                    }
                }
            }

        }
        for(std::vector<pollfd>::iterator iter=pollfd_vec.begin();
        iter!=pollfd_vec.end();iter++){
            if(iter->fd==-1)
            pollfd_vec.erase(iter);
        }
    }
    close(listen_socket);
}