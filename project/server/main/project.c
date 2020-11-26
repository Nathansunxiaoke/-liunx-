/*
    function:多用户购票系统
    date:2020.10.12
    author:
*/


#include"head.h"




int main(int argc, char const *argv[])  // ./server 5000
{

    // 静态初始化互斥锁
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    // 初始化链接上的客户端的头节点
    client *c_head = NULL;
    c_head = init_client_list(c_head);

    // 初始化注册头节点
    regin *r_head = NULL;
	r_head = init_regin_list(r_head);

    // 初始化登录头节点
    login *l_head = NULL;
	l_head = init_login_list(l_head);

    // 初始化机票头节点
    flight *f_head = NULL;
	f_head = init_flight_list(f_head);

    // 初始化实名认头节点
    real_id *i_head = NULL;
	i_head = init_read_id_list(i_head);

	// 初始化删除航班的头节点
	del_flight *d_head = NULL;
	d_head = init_delflight_list(d_head);

    // 初始化老用户、飞机票、实名认证数据、购买票的数据
    init_old_usr_data(r_head);      // 用户
	init_buy_ticket_date(r_head,&(r_head->b_ticket_list));//购买的飞机
	init_flight_data(f_head);      // 机票
	init_read_id_data(i_head);     // 真实身份
	init_delflight_date(d_head);   // 删除的航班
 
    // 创建一个TCP套接字
    int sockfd;
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd == -1)
    {
        perror("socket error\n");
        exit(0);
    }

    // 绑定IP地址、端口号
    struct sockaddr_in srvaddr;
    socklen_t len = sizeof(srvaddr);
    bzero(&srvaddr,len);

    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(atoi(argv[1]));
    srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd,(struct sockaddr *)&srvaddr,len);

    // 设置监听套接字
    listen(sockfd,5);

    // 设置非阻塞等待链接,给sockfd添加非阻塞属性
    int state;
    state = fcntl(sockfd,F_GETFL);
    state |= O_NONBLOCK;
    fcntl(sockfd,F_SETFL,state);

    int connfd;
    int ret;
    struct sockaddr_in cliaddr;
    struct timeval t;
    t.tv_sec = 5;
    t.tv_usec = 0;
    client *p = NULL;
    char buf[100];
	pthread_t tid;

    while(1)
    {
        // 非阻塞等待客户端的链接
        connfd = accept(sockfd,(struct sockaddr *)&cliaddr,&len);

        // 如果有人链接进来.
        if(connfd > 0)
        {
            // 设置connfd为非阻塞的状态
            //state = fcntl(connfd,F_GETFL);
            //state |= O_NONBLOCK;
           // fcntl(connfd,F_SETFL,state);

            // 设置connfd有超时属性
            ret = setsockopt(connfd,SOL_SOCKET,SO_RCVTIMEO,&t,sizeof(t));

            // 将链接上的connfd存储到链接用户链表中
            ret = add_client_tail(c_head,connfd,m);

			// 创建一个子线程，进行消息处理
			struct msg_handler msg;
			msg.c_head = c_head;
			msg.r_head = r_head;
			msg.f_head = f_head;
			msg.l_head = l_head;
			msg.i_head = i_head;
			msg.d_head = d_head;
			msg.connfd = connfd;
			pthread_create(&tid,NULL,msg_handler,(void *)&msg);

            // 成功链接上服务器
            if(ret == 0)
            {
                printf("new connetction:%s\n",inet_ntoa(cliaddr.sin_addr));;
            }
            else
            {
                printf("add client error\n");
                close(connfd);
            }
                        
        }
    }

    // 关闭服务器套接字
    close(sockfd);

    return 0;
}
