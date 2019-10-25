非阻塞IO :
1.一个是非阻塞读写 send() recv()
非阻塞情况下,read()/recv() 如果没有数据可读,立刻返回,返回值是-1 错误码是EWORDBLOCK
有数据可读也可能是不到自己期望的size,需要自己进行判断.
为0则收到了fin包.

非阻塞情况下,write()/send() 如果没有发送缓冲区,则返回-1

非阻塞 connect() accept()


epoll()

poll()有 struct pollfd
epoll()有  epollfd

需要使用epoll_create(int size)
epoll_create1()

epoll_create()-->epoll_ctl()

epollfd  opt fd epoll_event
epoll{
    uint32_t events;
    epoll_data_t data;
}
epoll_wait()可用于检测事件
四个参数:epoll_fd epoll_event
maxevents
timeout
-----------------------
把一个套接字设置为非阻塞的
int oldFlag=fcntl(fd,F_GETFL,0)
int newFlag=oldSocketFlag|O_NONBLOCK

fcntl(fd,F_SETFL,newSocketFlag)--出错时返回值为-1
