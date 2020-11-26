#include"head.h"



void welcome_meun()
{
    printf("============================================================\n");
    printf("\n              welcome to flight system                    \n");
    printf("\n==========================================================\n");
    sleep(3);
}

int per_main_meun(int sockfd)
{
    // 检查是否是VIP用户是否实名认证了
    char rcv_buf[100];
    int  order;
    while(1)
    {
        // 系统的主界面
        printf("======================flight system=============================\n");
        printf("                1.航班列表           7.查询余额                    \n");
        printf("                2.查询航班           8.充值                       \n");
        printf("                3.快速查询           9.个人信息                    \n");
        printf("                4.订购航班          10.实名认证                    \n");
        printf("                5.航班退票          11.返回上一级                  \n");
        printf("                6.航班改签          12.退出系统                    \n");
        printf("=================================================================\n");
        printf("请输入您操作的指令(例如:1):");
        scanf("%d",&order);

        // 判断需要发送的指令
        switch (order)
        {
            case 1:
                // 航班列表
                show_all_flight_data(sockfd);
                break;
            case 2:
                // 查询航班
                show_flyda_bycon(sockfd);
                break;
            case 3:
                // 快速查询
                quick_find(sockfd);
                break;
            case 4:
                buy_ticket(sockfd);
                break;
            case 5:
                c_back_ticket(sockfd);
                break;
            case 6:
                c_change_ticket(sockfd);
                break;
            case 7:
                usr_menony(sockfd);
                break;
            case 8:
                check_menoy(sockfd);
                break;
            case 9:
                break;
            case 10:
                check_real(sockfd);
                break;
            case 11:
                // 返回上一级
                send(sockfd,"11",sizeof("11"),0);
                return 0;
            case 12:
                // 退出登录
                send(sockfd,"12",sizeof("12"),0);
                return 0;
            default:
                printf("输入的指令有误,请重新输入\n");
                break;
        }
    }
}

int c_admin_fun(int sockfd)
{
    int  order;

    while(1)
    {

        // 系统的主界面
        printf("======================flight system=========================\n");
        printf("                        1.航班列表                            \n");
        printf("                        2.录入航班                            \n");
        printf("                        3.删除航班                            \n");
        printf("                        4.保险生效                            \n");
        printf("                        5.查看删除航班                        \n");
        printf("                        6.退出登录                            \n");
        printf("=============================================================\n");
        printf("请输入您操作的指令(例如:1):");
        scanf("%d",&order);
        switch (order)
        {
            case 1:
                show_all_flight_data(sockfd);
                break;
            case 2:
                c_add_flight_date(sockfd);
                break;
            case 3:
                del_flight_date(sockfd);
                break;
            case 4: 
                break;
            case 5:
                show_del_fly(sockfd);
                break;
             case 6:
                send(sockfd,"6",strlen("6"),0);
                return 0;
            default:
                printf("输入指令错误");
                break;
        }
    }

}

int admin_login(int sockfd)
{
    char snd_buf[100];
    char rcv_buf[200];

    // 向服务器发送管理员登录指令
    send(sockfd,"3",strlen("3"),0);

    char admin_name[20] = {0};
    char admin_passwd[30] = {0};
    printf("请输入管理员用户名:");
    scanf("%s",admin_name);
    printf("请输入管理员密码:");
    scanf("%s",admin_passwd);

    // 想服务器发送需要登录的用户名
    bzero(snd_buf,sizeof(snd_buf));
    sprintf(snd_buf,"%s,%s,",admin_name,admin_passwd);
    send(sockfd,snd_buf,strlen(snd_buf),0);

    // 等待服务器送的结果
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"login timeout") == 0)
            {
                printf("登录超时！\n");
                return -1;
            }
            if(strcmp(rcv_buf,"error") == 0)
            {
                printf("您输入的用户名或密码错误\n");
                return -2;
            }
            if(strcmp(rcv_buf,"successful") == 0)
            {
                // 进入管理员界面
                printf("登录成功!\n");
                c_admin_fun(sockfd);
                return 0;
            }

        }
        else
        {
            break;
        }      
    }
    return -1;
}

int show_flyda_bycon(int sockfd)
{
    // 向服务器发送查询航班列表信息
    send(sockfd,"2",sizeof("2"),0);

    int  order;
    while(1)
    {
        // 系统的主界面
        printf("======================flight system=============================\n");
        printf("                        1.目的地                                 \n");
        printf("                        2.班期                                   \n");
        printf("                        3.机型                                   \n");
        printf("                        4.价格                                   \n");
        printf("                        5.返回上一级                              \n");
        printf("=================================================================\n");
        printf("请输入您操作的指令(例如:1):");
        scanf("%d",&order);

        switch (order)
        {
            case 1:
                send(sockfd,"1",strlen("1"),0);
                // 提示用户输入需要查询的航班号
                printf("请输入需要查询目的地:");
                f_flyda_by_con(sockfd);
                break;
            case 2:
                send(sockfd,"2",strlen("2"),0);
                // 提示用户输入需要查询的班期
                printf("请输入需要查询的班期:");
                f_flyda_by_con(sockfd);
                break;
            case 3:
                send(sockfd,"3",strlen("3"),0);
                // 提示用户输入需要查询的机型
                printf("请输入需要查询的机型:");
                f_flyda_by_con(sockfd);
                break;
            case 4:
                 send(sockfd,"4",strlen("4"),0);
                // 提示用户输入需要查询的价格
                printf("请输入需要查询价格:");
                f_flyda_by_con(sockfd);
                break;
            case 5:
                // 返回上一级
                send(sockfd,"5",strlen("5"),0);
                return 0;
            default:
                printf("输入指令错误，请重新输入\n");
                break;
        }
    }
}
