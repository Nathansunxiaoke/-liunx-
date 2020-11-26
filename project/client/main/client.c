/*
    function:购票的客户端
    date:2020.10.12
    author:
*/
#include"head.h"


int main(int argc, char const *argv[])
{
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
    srvaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET,argv[1],&srvaddr.sin_addr);

	// connect调用之前： sockfd  -> 待连接套接字
	int ret = connect(sockfd,(struct sockaddr *)&srvaddr,len);
	if(ret == 0)
	{
		printf("connect success!\n");
	}

    // 向服务器发送指令
	char buf[100];
    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = 100000;

    setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&t,sizeof(t));

    // 欢迎界面
    welcome_meun();
    int  order;

    while(1)
    {

        // 系统的主界面
        printf("======================flight system=========================\n");
        printf("                        1.注册                               \n");
        printf("                        2.用户登录                          \n");
        printf("                        3.管理员登录                               \n");
        printf("                        4.退出                               \n");
        printf("============================================================\n");
        printf("请输入您操作的指令(例如:1):");
        scanf("%d",&order);

        // 判断需要发送的指令
        switch (order)
        {
            case 1: 
                regin_usr_fun(sockfd);
                break;
            case 2:
                login_usr_fun(sockfd);
                break;
            case 3:
                admin_login(sockfd);
                break;
            case 4:
                send(sockfd,"4",strlen("4"),0);
                return 0;
            default:
                printf("输入的指令有误,请重新输入\n");
                break;
        }
    }
    return 0;
}
