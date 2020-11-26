#include"head.h"






int  show_all_flight_data(int sockfd)
{
    // 向服务器发送查询航班列表信息
    send(sockfd,"1",sizeof("1"),0);

    // 打印服务器送过来的信息
    char rcv_buf[300];
    printf("=======================================================================\n");
	printf("航班号\t出发地\t目的地\t  班期\t   机型   起飞时间\t票价\t余票\n");
    while (1)
    {   bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            printf("%s",rcv_buf);
        }
        else
        {
            sleep(1);
            break;
        }  
    }
}

int quick_find(int sockfd)
{
    char snd_f_num[20] = {0};
    char rcv_buf[200];

    // 向服务器发送查询航班列表信息
    send(sockfd,"3",sizeof("3"),0);

    // 提示用户输入需要查询的航班号
    printf("请输入需要查询的航班号:");
    scanf("%s",snd_f_num);
    send(sockfd,snd_f_num,strlen(snd_f_num),0);

    // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"no fliaght number date") == 0)
            {
                printf("%s\n",rcv_buf);
                return 0;
            }
            // 查找到该航班信息
            else
            {
                printf("=======================================================================\n");
                printf("航班号\t出发地\t目的地\t  班期\t   机型   起飞时间\t票价\t余票\n");
                printf("%s",rcv_buf);
            }
        } 
        else
        {
            sleep(1);
            break;
        }          
    }
    // 询问用户是否购买
    int buy;
    while (1)
    {
        printf("是否需要购买机票\n:1 -> 购买，0 -> 不购买，其他按键退出航班查询（例如:1):");
        scanf("%d",&buy);
        switch (buy)
        {
        case 0:
            printf("not buy\n");
            return 0;
        case 1:
            printf("buy\n");
            return 0;
        default:
            return 0;
        }
    }         
}

int f_flyda_by_con(int sockfd)
{
    char snd_buf[10];
    char rcv_buf[300];

    // 向服务器发送数据
    scanf("%s",snd_buf);
    send(sockfd,snd_buf,strlen(snd_buf),0);

    // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"no fliaght number date") == 0)
            {
                printf("%s\n",rcv_buf);
                return 0;
            }
            // 查找到该航班信息
            else
            {
                printf("%s",rcv_buf);
            }
        } 
        else
        {
            sleep(1);
            break;
        }          
    }
    // 询问用户是否购买
    int buy;
    while (1)
    {
        printf("是否需要购买机票\n:1 -> 购买，0 -> 不购买，其他按键退出航班查询（例如:1):");
        scanf("%d",&buy);
        switch (buy)
        {
        case 0:
            printf("not buy\n");
            return 0;
        case 1:
            printf("buy\n");
            return 0;
        default:
            return 0;
        }
    }         
}

int buy_ticket(int sockfd)
{
    char rcv_buf[300];
    // 向服务器发送查询航班列表信息
    send(sockfd,"4",sizeof("4"),0);

    // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"no realname") == 0)
            {
                printf("对不起，您还没有进行实名认证，不能购票!\n");
                return -1;
            }
            // 显示所有航班，开始购票
            else
            {
                printf("%s",rcv_buf);
            }
        } 
        else
        {
            sleep(1);
            break;
        }          
    } 
    
    // 询问用户购买那张机票
    char buy_num[10] = {0};
    printf("请选择需要购买的航班号:");
    scanf("%s",buy_num);
    send(sockfd,buy_num,strlen(buy_num),0);

    // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"no flydate") == 0)
            {
                printf("没有该航班的相关信息，请重新输入\n");
                return -3;
            }
            if(strcmp(rcv_buf,"no poll") == 0)
            {
                printf("该航班已售空\n");
                return -4;
            }

        } 
        else
        {
            sleep(1);
            break;
        }          
    } 

    // 询问乘坐的人是谁
    char take_name[10] = {0};
    printf("请输入乘车人:");
    scanf("%s",take_name);
    send(sockfd,take_name,strlen(take_name),0);

     // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"no realname") == 0)
            {
                printf("对不起，乘坐的用户不存在，或者没有实名认证\n");
                return -2;
            }
            if(strcmp(rcv_buf,"no monoy") == 0)
            {
                printf("余额不足，请充值\n");
                return -2;
            }
            if(strcmp(rcv_buf,"already exit ticket") == 0)
            {
                printf("已经有买过该票了\n");
                return -2;
            }
            if(strcmp(rcv_buf,"already day exit") == 0)
            {
                printf("已经有同一天的航班了\n");
                return -2;
            }
            if(strcmp(rcv_buf,"buy ins") == 0)
            {
                break;
            }

        } 
        else
        {
            sleep(1);
            break;
        }          
    } 
    
    // 提示用户是否购买保险
    char buy_ins[10];
    printf("是否需要花费100购买飞机延误保险:1 -> 购买，0 -> 不购买:");
    scanf("%s",buy_ins);
    send(sockfd,buy_ins,sizeof(buy_ins),0);

    // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"successful") == 0)
            {
                printf("购买成功!\n");
                return -1;
            }
        } 
        else
        {
            sleep(1);
            break;
        }          
    } 
    return 0;
    
}

int check_real(int sockfd)
{
    char rcv_buf[100];
    char snd_buf[100];

    // 向服务器发送查询航班列表信息
    send(sockfd,"10",sizeof("10"),0);

    // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"successful") == 0)
            {
                printf("您已经完成实名认证了\n");
                return 0;
            }
        } 
        else
        {
            sleep(1);
            break;
        }          
    }

    // 输入真实姓名和身份证号码
    
    char real_name_buf[10];
	char real_ID_buf[20];

    printf("请输入您的真实姓名:");
	scanf("%s",real_name_buf);
	printf("请输入您的身份证号码:");
	scanf("%s",real_ID_buf);

    // 发送数据到服务端口
    sprintf(snd_buf,"%s,%s,",real_name_buf,real_ID_buf);
    send(sockfd,snd_buf,strlen(snd_buf),0);

    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"successful") == 0)
            {
                printf("实名认证成功\n");
                return 0;
            }
        }
        else
        {
            usleep(100000);
            break;
        }
        
    }
    printf("认证失败，姓名和身份证不匹配\n");	
	return -1;
}

int check_menoy(int sockfd)
{
    char rcv_buf[100];
    char snd_buf[100];

    // 向服务器发送查询航班列表信息
    send(sockfd,"8",sizeof("8"),0);

    // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            printf("%s",rcv_buf);
        }
        else
        {
            sleep(1);
            break;
        }          
    }

    // 进行充值
    int rc_money;
	printf("请输入充值的金额:");
	scanf("%d",&rc_money);
    sprintf(snd_buf,"%d\n",rc_money);
    send(sockfd,snd_buf,sizeof(snd_buf),0);

    // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"充值成功!") == 0)
            {
                printf("%s\n",rcv_buf);
                return 0;
            }
        } 
        else
        {
            sleep(1);
            break;
        }          
    }
    return 0;
}

int usr_menony(int sockfd)
{
    // 向服务器发送查询余额
    send(sockfd,"7",sizeof("7"),0);


    // 接收服务器发生的消息
    char rcv_buf[100] = {0};
    while(1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            printf("%s\n",rcv_buf);
            return 0;
        }
        else
        {
            break;
        }
    }
    return 0;
}

int c_back_ticket(int sockfd)
{
    char rcv_buf[100];
    // 向服务器发送退票指令
    send(sockfd,"5",sizeof("5"),0);

    // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"no fly") == 0)
            {
                printf("没有订单!\n");
                return 0;
            }
            // 查找到订单了
            else
            {
                printf("%s",rcv_buf);
            }
        } 
        else
        {
            sleep(1);
            break;
        }          
    }

    // 选择需要退的票
     // 询问用户购买那张机票
    char buy_num[10] = {0};
    printf("请选择需要退票的航班号:");
    scanf("%s",buy_num);
    send(sockfd,buy_num,strlen(buy_num),0);

     // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"num error") == 0)
            {
                printf("输入航班号错误!\n");
                return 0;
            }
        } 
        else
        {
            sleep(1);
            break;
        }          
    }
    printf("退票成功!\n");
    return 0;

}

int c_change_ticket(int sockfd)
{
    char rcv_buf[100];
    // 向服务器发送退票指令
    send(sockfd,"6",strlen("6"),0);

    // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"no fly") == 0)
            {
                printf("没有订单!\n");
                return 0;
            }
            // 查找到订单了
            else
            {
                printf("%s",rcv_buf);
            }
        } 
        else
        {
            sleep(1);
            break;
        }          
    }

    // 选择需要改签的票
    char buy_num[10] = {0};
    printf("请选择需要改签的航班号:");
    scanf("%s",buy_num);
    send(sockfd,buy_num,strlen(buy_num),0);

    // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"num error") == 0)
            {
                printf("输入航班号错误\n");
                return -2;
            }
            if(strcmp(rcv_buf,"no poll") == 0)
            {
                printf("需要修改的航班号没有票了，请选择新的航班\n");
                return -3;
            }
             if(strcmp(rcv_buf,"already exit ticket") == 0)
            {
                printf("已经有买过该票了\n");
                return -2;
            }
            if(strcmp(rcv_buf,"already day exit") == 0)
            {
                printf("已经有同一天的航班了\n");
                return -2;
            }
            else
            {
                    printf("%s\n",rcv_buf);
                    break;
            }
            
            
        } 
        else
        {
            sleep(1);
            break;
        }          
    }

    // 选择需要改签的新的票

    printf("请选择需要改签为的航班号:");
    scanf("%s",buy_num);
    send(sockfd,buy_num,strlen(buy_num),0);

    // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {

            if(strcmp(rcv_buf,"no poll") == 0)
            {
                printf("需要修改的航班号没有票了，请选择新的航班\n");
                return -3;
            }
             if(strcmp(rcv_buf,"num error") == 0)
            {
                printf("输入航班号错误\n");
                return -2;
            }
            if(strcmp(rcv_buf,"successful") == 0)
            {
                printf("改签成功!\n");
                return 0;
            }
            else
            {
                printf("%s\n",rcv_buf);
                break;
            }
        } 
        else
        {
            sleep(1);
            break;
        }          
    }

    // 提示用户是否进行支付操作
    char pay[5] = {0}; 
    printf("是否进行支付：");
    scanf("%s",pay);
    send(sockfd,pay,strlen(pay),0);

     // 等待接受服务器消息
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"no pay") == 0)
            {
                printf("改签失败，取消支付\n");
                return -1;
            }
            if(strcmp(rcv_buf,"successful") == 0)
            {
                printf("改签成功!\n");
                return 0;
            }
        } 
        else
        {
            sleep(1);
            break;
        }          
    }
    return 0;
}

int login_usr_fun(int sockfd)
{
    char rcv_buf[100];
    // 向服务发送用户登录指令
    send(sockfd,"2\n",sizeof("2\n"),0);

    // 提示用户输入登录名
    char snd_ln_buf[10] = {0};
    printf("请输入登录名(例如：sun):");
    scanf("%s",snd_ln_buf);

    // 向服务器发送登录名
    send(sockfd,snd_ln_buf,strlen(snd_ln_buf),0);

    // 检查登录名是否注册过,是否需要密码登录
    bzero(rcv_buf,sizeof(rcv_buf));
    if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
    {
        printf("aaaaaa\n");
        if(strncmp(rcv_buf,"您还没有注册过，请先去注册一个新帐号吧",sizeof("您还没有注册过，请先去注册一个新帐号吧")) == 0 )
        {
            printf("%s\n",rcv_buf);
            return -1;
        }
        if(strncmp(rcv_buf,"notpasswd",sizeof("notpasswd")) == 0)
        {
            // 在登陆链上,免密码登陆,进入界面
			per_main_meun(sockfd);
            return 0;
        }
    }

    // 提示输入密码进行登录
    char passwd[20] = {0};
    printf("请输入密码:");
    scanf("%s",passwd);

    // 将密码发送到服务进行验证是否正确
    send(sockfd,passwd,strlen(passwd),0);

    bzero(rcv_buf,sizeof(rcv_buf));
    if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
    {
        printf("%s\n",rcv_buf);
        return -1;
		
    }
    // 通过输入密码进入登录界面
	per_main_meun(sockfd);
    return 0;
		
}

int  regin_usr_fun(int sockfd)
{
    char snd_buf[200];
    char rcv_buf[300];
    r_info snd_info;
    bzero(&snd_info,sizeof(snd_info));
    bzero(snd_buf,sizeof(snd_buf));

    // 向服务器发送注册指令
    send(sockfd,"1",strlen("1"),0);

  
    // 执行注册操作
    printf("请输入需要注册的用户名(例如：sun):");
    scanf("%s",snd_info.name);
    send(sockfd,snd_info.name,strlen(snd_info.name),0);
    
    // 检查用户名是否别注册
    bzero(rcv_buf,sizeof(rcv_buf));
    if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
    {
        if(strncmp(rcv_buf,"该用户已经被注册，请重新注册",sizeof("该用户已经被注册，请重新注册")) == 0)
        {
            printf("%s\n",rcv_buf);
                return -1;
        }
    }

    // 继续执行注册操作
    printf("请输入密码(例如：123456):");
    scanf("%s",snd_info.passwd);
        
    printf("请输入电话号码(例如：123456):");
    scanf("%s",snd_info.tel);

    printf("请输入密保问题(例如：我儿子叫什么):");
    scanf("%s",snd_info.Sec_quation);

    printf("请输入密保问题答案(例如：sun):");
    scanf("%s",snd_info.Sec_answer);

    // 将需要发送的数据发送过去
    sprintf(snd_buf,"%s,%s,%s,%s,%s,",  snd_info.name,
                                        snd_info.passwd,
                                        snd_info.tel,
                                        snd_info.Sec_quation,
                                        snd_info.Sec_answer);
    send(sockfd,snd_buf,strlen(snd_buf),0);
        
    // 判断是否成功

    bzero(rcv_buf,sizeof(rcv_buf));
    if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
    {
        printf("%s\n",rcv_buf);
        return 0;
    }    
}

int c_add_flight_date(int sockfd)
{
    char number[10] = {0};        //航班号
	char staddress[10]= {0};     //起点站
	char arraddress[15]= {0};    //终点站
	char date[15]= {0};          //班期
	char type[2]= {0};           //机型
	char stime[10]= {0};         //起飞时间
	unsigned int  price;    //票价
	int poll;	            //余票
    // 向服务器发送录入指令
    send(sockfd,"2",sizeof("2"),0);


    printf("请输入航班号:");
    scanf("%s",number);

    printf("请输入航班的起点站:");
    scanf("%s",staddress);

    printf("请输入航班的终点站:");
    scanf("%s",arraddress);

    printf("请输入班期:");
    scanf("%s",date);

    printf("请输入机型：");
    scanf("%s",type);

    printf("请输入航班的起飞时间:");
    scanf("%s",stime);

    printf("请输入票价:");
    scanf("%d",&price);

    printf("请输入票数:");
    scanf("%d",&poll);

    char snd_flight_data[300];
    bzero(snd_flight_data,sizeof(snd_flight_data));

    sprintf(snd_flight_data,"%s,%s,%s,%s,%s,%s,%d,%d,",number,
													staddress,
													arraddress,
													date,
													type,
													stime,
													price,
													poll);
    // 发送航班数据
    send(sockfd,snd_flight_data,strlen(snd_flight_data),0);

    // 接收服务器是否发送失败
    char rcv_buf[100];

    while (1)
    {   
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
        {
            if(strcmp(rcv_buf,"successful")== 0)
            {
                printf("录入航班成功!\n");
                return 0;
            }
            if(strcmp(rcv_buf,"timeout") == 0)
            {
                printf("录入操作超时!\n");
                return -1;
            }
        }
        else
        {
            break;
        }
    }

    return -1;
}

int  del_flight_date(int sockfd)
{
    // 向服务器发送删除航班的指令
    send(sockfd,"3",sizeof("3"),0);

    char del_num[10]={0};
    printf("请输入删除的航班:");
    scanf("%s",del_num);
    send(sockfd,del_num,strlen(del_num),0);

    // 等待接受服务其的消息
    char rcv_buf[100];
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >=0)
        {
            // 接收到服务器发送的消息
            if(strcmp(rcv_buf,"dtimeout") == 0)
            {
                printf("删除操作超时!\n");
                return -1;
            }
            if(strcmp(rcv_buf,"nomessage")== 0)
            {
                printf("需要删除的航班不存在！\n");
                return -2;
            }
            if(strcmp(rcv_buf,"successful")== 0)
            {
                printf("删除航班成功\n");
                return 0;
            }
        }
        else
        {
            usleep(100000);
            break;
        }
    } 
    return 0;
}

int show_del_fly(int sockfd)
{
    // 向服务器发送删除航班的指令
    send(sockfd,"5",sizeof("5"),0);

    // 等待接受服务其的消息
    char rcv_buf[100];
    while (1)
    {
        bzero(rcv_buf,sizeof(rcv_buf));
        if(recv(sockfd,rcv_buf,sizeof(rcv_buf),0) >=0)
        {
            // 接收到服务器发送的消息
            if(strcmp(rcv_buf,"dtimeout") == 0)
            {
                printf("删除操作超时!\n");
                return -1;
            }
            if(strcmp(rcv_buf,"nomessage")== 0)
            {
                printf("需要删除的航班不存在！\n");
                return -2;
            }
            else
            {
                 printf("%s",rcv_buf);
            }
            
        }
        else
        {
            break;
        }
    } 
    return 0;
}






