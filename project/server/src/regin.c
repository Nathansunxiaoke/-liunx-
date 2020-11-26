#include"head.h"




int regin_fun(regin *r_head,char *r_usr_info)
{
    char *buf = NULL;
    char taken[] = ",";

    regin *new_usr = NULL;
	new_usr = (regin *)malloc(sizeof(regin));
	if(new_usr == NULL)
		perror("malloc new_usr");

    // 将数据进行存入用户链表
	buf = strtok(r_usr_info,taken);
	strcpy(new_usr->std.name,buf);     //姓名

    buf = strtok(NULL,taken);
	strcpy(new_usr->std.passwd,buf);     // 密码

    buf = strtok(NULL,taken);
	strcpy(new_usr->std.tel,buf);        // 电话号码

    buf = strtok(NULL,taken);
	strcpy(new_usr->std.Sec_quation,buf);        // 密保问题

    buf = strtok(NULL,taken);
	strcpy(new_usr->std.Sec_answer,buf);        // 密保问题答案

    new_usr->std.real_name_flag = 0;             // 没有实名认证
	new_usr->std.memony = 0;                     // 没有钱
	new_usr->std.Vip = 0;                        // 不是VIP
	strcpy(new_usr->std.real_name,"NULL");       // 没有名字
	new_usr->std.type = 0;                       // 默认位成人

	new_usr->b_ticket_list = *(init_buy_ticket_list(new_usr,&new_usr->b_ticket_list));                    //初始化用户的购票链

	// 数据域赋值结束     
	new_usr->next = r_head;            // 新用户尾插下一个节点指向头               
	
	regin *p = r_head->prev;

	p->next = new_usr;                 // 最后一个节点指向新节点
	new_usr->prev = p;                 // 新节点前驱指向最后一个节点
	r_head->prev = new_usr;

	//数据域结束

	save_usr_data_file(new_usr);       // 保存用户数据到文件


    return 0;
}


int check_usr_name(regin *r_head,int connfd)
{
    char snd_buf[100];
    char rcv_buf[200];
    // 不断接收客户读发送的消息
	while(1)
	{
		bzero(rcv_buf,sizeof(rcv_buf));
    	if(recv(connfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
		{
			regin *check = r_head->next;
			for(check;check != r_head;check = check->next)
			{
       			// 需要注册的用户名已经被注册
				if(strcmp(check->std.name,rcv_buf) == 0)
				{
					bzero(snd_buf,sizeof(snd_buf));
					strncpy(snd_buf,"该用户已经被注册，请重新注册",sizeof("该用户已经被注册，请重新注册\n"));
					send(connfd,snd_buf,sizeof(snd_buf),0);
					return -1;
				}
			}
			break;
		}	

	}
	while (1)
	{
		bzero(rcv_buf,sizeof(rcv_buf));
    	if(recv(connfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
		{
			// 完成用户注册
			regin_fun(r_head,rcv_buf);

			// 向用户发生注册成功
			send(connfd,"恭喜你，注册成功",sizeof("恭喜你，注册成功"),0);
			return 0;
		}
	}

}

void client_quit(client *c_head,int connfd)
{
	// 静态初始化互斥锁
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
	// 如果执行的退出操作
	close(connfd);

	// 删除客户端的节点信息
	del_client_node(c_head,connfd,m);

	pthread_exit(NULL);
}
