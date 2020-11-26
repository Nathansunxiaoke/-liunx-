#include"head.h"



int server_login_fun(regin *usr,struct msg_handler *msg)
{
	char order[10];
	flight *f_head = (flight *)msg->f_head;
	login *l_head = (login *)msg->l_head;
	int connfd = (int)msg->connfd;
	// 接收登录成功后用户发送过来的指令
	while(1)
	{
		bzero(order,sizeof(order));
		if(recv(msg->connfd,order,sizeof(order),0) >= 0)
		{	
			switch (atoi(order))
			{
				case 1: 
					// 显示航班列表
					show_all_flight_data(f_head,connfd);
                	break;
				case 2:
					// 查询航班
					srv_fly_date_by(f_head,connfd);
					break;
				case 3:
					// 快速查询
					srv_qf_num(f_head,connfd);
					break;
				case 4:
					// 订购航班
					buy_flight_fun(usr,msg);
					break;
				case 5:
					back_ticket(usr,msg);
					break;
				case 6:
					change_ticket(usr,msg);
					break;
				case 7:
					print_usr_menoy(usr,connfd);
					break;
				case 8:
					recharge(usr,connfd);
					break;
				case 9:
					break;
				case 10:
					check_real_id(usr,msg);
					break;
				case 11:
					// 返回上一级
					return 0;
				case 12:
					// 推出登录
					usr_quit_login(usr,l_head);
					return 0;
			}
		}
	}
}

int check_login_status(struct msg_handler *msg,regin *usr,char *l_name_buf)
{
	char u_lpasswd_buf[20];
	login * l_head = (login *)msg->l_head;
	login *check = l_head->next;
	client *c_head = (client *)msg->c_head;
	client *p = c_head;

	// 寻找已连接上套接字对应的节点
	list_for_each_entry(p,&(c_head->list),list)
	{
		if(p->connfd == msg->connfd)
		{
			// 找到客户端节点，将用户名添加到客户端链上
			strncpy(p->client_name,l_name_buf,sizeof(l_name_buf));
			break;
		}
	}

	// 检查是否在登陆链上
	for(;check != l_head;check = check->next)                 
	{
		if(strcmp(check->l_name,l_name_buf) == 0)  
		{
			// 通知登录的用户不需要登录密码、需要一个接口不让函数返回
			send(msg->connfd,"notpasswd",strlen("notpasswd"),0);  
			server_login_fun(usr,msg); 
			return 0;       
		}
	}

	// 没有登录过，提示输入密码,不断接受用户发来,如果有超时属性，将15秒向下执行
	while(1)
	{
		bzero(u_lpasswd_buf,sizeof(u_lpasswd_buf));
		if(recv(msg->connfd,u_lpasswd_buf,sizeof(u_lpasswd_buf),0) >= 0)
		{
			// 15秒内用户有登录密码发送过来,判断密码是否正确
			if(strcmp(usr->std.passwd,u_lpasswd_buf) == 0)    
			{
				// 密码正确
				login *usr_login = NULL;                       
				usr_login = (login *)malloc(sizeof(login));        
				if(usr_login == NULL)
					perror("malloc usr_login");
				
				// 将用户的姓名加载到登陆链上   
				strncpy(usr_login->l_name,usr->std.name,sizeof(usr->std.name));     
				
				// 指针域赋值
				usr_login->next = l_head;
				login *p = l_head->prev;

				p->next = usr_login;
				usr_login->prev = p;

				// 需要一个接口不让函数返回
				server_login_fun(usr,msg);
				return 0;
			}
			else
			{
				send(msg->connfd,"error passwd",strlen("error passwd"),0);
				return 0;
			}
			
		}
	}
}

int usr_login_fun(struct msg_handler *msg)
{
	char rcv_buf_lname[100];
	char snd_buf[100];
	regin *r_head =(regin *) msg->r_head;
	regin *check_reg = r_head -> next;
	// 接受用户发送过来的用户名，进行判断
	while(1)
	{
		bzero(rcv_buf_lname,sizeof(rcv_buf_lname));
    	if(recv(msg->connfd,rcv_buf_lname,sizeof(rcv_buf_lname),0) >= 0)
		{
			for(;check_reg != r_head;check_reg = check_reg->next)
			{
				if(strcmp(check_reg->std.name,rcv_buf_lname) == 0)  
				{
					// 客户发生的登录有效
					check_login_status(msg,check_reg,rcv_buf_lname);
					return 0;
				}
			}
			// 向客户端发送没有注册消息
			bzero(snd_buf,sizeof(snd_buf));
			strncpy(snd_buf,"您还没有注册过，请先去注册一个新帐号吧",sizeof("您还没有注册过，请先去注册一个新帐号吧"));
			send(msg->connfd,snd_buf,sizeof(snd_buf),0);
			return -1;			
		}
	}
}

int append_flight_data(flight *f_head,char *flight_data_buff)
{
	char taken[] = ",";
	char *buff =  NULL;

	// 申请新的结点
	flight *new = NULL;
	new = (flight *)malloc(sizeof(flight));
	if(new == NULL)
	{
		perror("malloc error");
		exit(0);
	}
	//为数值域赋值
	printf("%s\n",flight_data_buff);
	buff = strtok(flight_data_buff,taken);
	strcpy(new->info.number,buff);       //航班号

	buff =  strtok(NULL,taken);
	strcpy(new->info.staddress,buff);       //出发地

	buff =  strtok(NULL,taken);
	strcpy(new->info.arraddress,buff);       //目的地

	buff =  strtok(NULL,taken);
	strcpy(new->info.date,buff);       //班期

	buff =  strtok(NULL,taken);
	strcpy(new->info.type,buff);       //机型

	buff =  strtok(NULL,taken);
	strcpy(new->info.stime,buff);       //出发时间

	buff =  strtok(NULL,taken);
	new->info.price = atoi(buff);             //价格

	buff =  strtok(NULL,taken);
	new->info.poll =  atoi(buff);             //余票

	new->next = f_head;
	//指针域赋值
	flight *p = f_head->prev;

	p->next = new;
	new->prev =  p;
	f_head->prev = new;

	save_flight_date(new);

	return 0;
}

int add_flight_data(flight *f_head,int connfd)
{
	struct timeval t;
    t.tv_sec = 25;
    t.tv_usec = 0;

	char rcv_buf[300];

	// 设置connfd有超时属性  25s录入超时
    setsockopt(connfd,SOL_SOCKET,SO_RCVTIMEO,&t,sizeof(t));

	while (1)
	{
		bzero(rcv_buf,sizeof(rcv_buf));
		if(recv(connfd,rcv_buf,sizeof(rcv_buf),0) >=0)
		{
			printf("%s\n",rcv_buf);
			// 开始录入
			append_flight_data(f_head,rcv_buf);
			send(connfd,"successful",strlen("successful"),0);
			return 0;
		}
		else
		{
			// 25s内没有收到数据
			break;
		}
		
	}
	// 没有数据到达，录入超时
	send(connfd,"timeout",strlen("timeout"),0);
	return -1;
}
 

int check_del_flight(del_flight *d_head,flight *f_head,char *d_number,int connfd)
{
	char del_file[200] = {0};
	flight *q = f_head;
	flight *p = f_head->next;
	for(q,p;p != f_head;q = p,p = p->next)
	{
		printf("ddd%s\n",d_number);
		if(strcmp(p->info.number,d_number) == 0)
		{
			// 将航班的信息节点给删除
			del_flight *new = malloc(sizeof(del_flight));

			new->d_flight = (p->info);

			list_add_tail(&(new->list),&(d_head->list));

			// 删除文件信息
			sprintf(del_file,"mv ./data/flight_data/%s.txt ./data/del_flight_data",p->info.number);
			system(del_file);

			// 删除节点信息
			q->next = p->next;
			p->next->prev = q;
			// 4.释放该节点
			free(p);
			return 0;
		}
	}
	printf("ddd\n");
	return -2;
}

int del_flight_info(struct msg_handler *msg)
{
	int connfd = (int )msg->connfd;
	flight *f_head = (flight *)msg->f_head;
	del_flight *d_head = (del_flight *)msg->d_head;

	struct timeval t;
    t.tv_sec = 10;
    t.tv_usec = 0;

	char rcv_buf[100];
	int ret;

	// 设置connfd有超时属性  10s删除超时
    setsockopt(connfd,SOL_SOCKET,SO_RCVTIMEO,&t,sizeof(t));

	while (1)
	{
		bzero(rcv_buf,sizeof(rcv_buf));
		if(recv(connfd,rcv_buf,sizeof(rcv_buf),0) >=0)
		{
			printf("%s\n",rcv_buf);
			// 开始删除
			// 判断的是否有这个航班
		 	if((ret = check_del_flight(d_head,f_head,rcv_buf,connfd))< 0)
			 {
				 send(connfd,"nomessage",strlen("nomessage"),0);
				 return -2;
			 }
			send(connfd,"successful",strlen("successful"),0);
			return 0;
		}
		else
		{
			// 10内没有接受到需要删除的航班号
			break;
		}
	}
	// 删除超时
	send(connfd,"dtimeout",strlen("dtimeout"),0);
	return -1;
}

int show_del_flight_date(struct msg_handler *msg)
{
	del_flight *d_head =  (del_flight *)msg->d_head;
	flight *f_head = (flight *)msg->f_head;
	int connfd = msg->connfd;
	int i = 0;
	del_flight *p = NULL;

	char snd_buf[300];
	sprintf(snd_buf,"%s","=======================================================================\n");
	send(connfd,snd_buf,sizeof(snd_buf),0);
    sprintf(snd_buf,"%s","航班号\t出发地\t目的地\t  班期\t   机型   起飞时间\t票价\t余票\n");
	send(connfd,snd_buf,sizeof(snd_buf),0);
	list_for_each_entry(p,&(d_head->list),list)
	{
		// 将产寻到的用户信息发送给客户端
		bzero(snd_buf,sizeof(snd_buf));
		sprintf(snd_buf," %s\t %s\t %s\t%s   %s\t  %s  \t%d\t%d\n",p->d_flight.number,
																p->d_flight.staddress,
																p->d_flight.arraddress,
																p->d_flight.date,
																p->d_flight.type,
																p->d_flight.stime,
																p->d_flight.price,
																p->d_flight.poll);
		// 查询到一趟航班就发送给用户
		send(connfd,snd_buf,strlen(snd_buf),0);
		i++;
	}

	// 判断是否查询到航班'
	if(p == d_head && i == 0)
	{
		send(connfd,"nomessage",strlen("nomessage"),0);
		return -1;
	}

	return 0;
	
}


int admin_fun_meun(struct msg_handler *msg)
{
	char order[10] = {0};
	int connfd = (int )msg->connfd;
	flight *f_head = (flight *)msg->f_head;

	// 不断接收客户端发送过来的指令
	while(1)
	{
		bzero(order,sizeof(order));
		if(recv(connfd,order,sizeof(order),0) >= 0)
		{
			switch (atoi(order))
			{
				case 1:
					// 显示航班列表
					show_all_flight_data(f_head,connfd);
					break;
				case 2:
					// 录入新航班
					add_flight_data(f_head,connfd);
					break;
				case 3:
					del_flight_info(msg);
					break;
				case 4:
					
					break;
				case 5:
					show_del_flight_date(msg);
					break;
				case 6:
					// 推出登录
					printf("hhh\n");
					return 0;
			}
		}
	}
	
}

int admin_login(struct msg_handler *msg)
{
	int connfd = (int )msg->connfd;
	struct timeval t;
    t.tv_sec = 15;
    t.tv_usec = 0;

	// 设置connfd有超时属性
    setsockopt(connfd,SOL_SOCKET,SO_RCVTIMEO,&t,sizeof(t));

	char rcv_buf[300] = {0};

    char admin_name[20] = {0};
    char admin_passwd[30] = {0};
	char taken[] = ",";
	char *buff = NULL;

	// 接收客户端发送的用户名和密码
	while (1)
	{
		bzero(rcv_buf,sizeof(rcv_buf));
		if(recv(connfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
		{
			printf("%s\n",rcv_buf);
			// 接受到用户名与密码了
			buff = strtok(rcv_buf,taken);
			strcpy(admin_name,buff); 

			buff =  strtok(NULL,taken);
			strcpy(admin_passwd,buff);
			// 判断师傅是管理员
			if(strcmp(admin_name,"admin")== 0 && strcmp(admin_passwd,"123456")==0)
			{
				// 是管理员进入下一个界面
				send(connfd,"successful",strlen("successful"),0);
				admin_fun_meun(msg);
				return 0;
			}
			else
			{
				// 不是管理员，name或admin错误
				send(connfd,"error",strlen("error"),0);
				return -1;
			}		
		}
		else
		{
			// 5秒内没有接受到管理员信息，推出
			break;
		}
		
	}
	printf("admin login timeout\n");
	send(connfd,"login timeout",strlen("login timeout"),0);
	return -2;
}

void *msg_handler(void *arg)
{
	// 将子线程设置为分离属性
	pthread_detach(pthread_self());

	int ret;
	int a;
	char order[10];
	struct msg_handler msg = *(struct msg_handler *)arg;
	// 不断接收客户端发送过来的指令
	while(1)
	{
		bzero(order,sizeof(order));
		if(recv(msg.connfd,order,sizeof(order),0) >= 0)
		{
			a = atoi(order);
			switch (a)
			{
				case 1:
					check_usr_name(msg.r_head,msg.connfd); 
					break;
				case 2:
					usr_login_fun(&msg);
					break;
				case 3:
					admin_login(&msg);
					break;
				case 4:
					client_quit(msg.c_head,msg.connfd);
					break;
			}
		}
	}
	
}
