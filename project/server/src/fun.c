#include"head.h"




int add_client_tail(client *c_head,int connfd,pthread_mutex_t m)
{

    // 为链接上的客户端创建j节点
    client *new = NULL;
    new = (client *)malloc(sizeof(client));
    if(new == NULL)
    {
        perror("malloc new error\n");
        exit(0);
    }

    // 修改公用资源先上锁
    pthread_mutex_lock(&m);

    // 为链接上的客户端数值域赋值
    new->connfd = connfd;
    strncpy(new->client_name,"usrname",sizeof("usrname"));

    // 通过尾插方式指针域赋值
    list_add_tail(&(new->list),&(c_head->list));

    // 修改后解锁
    pthread_mutex_unlock(&m);


    return 0;
}

int del_client_node(client *c_head,int connfd,pthread_mutex_t m)
{

    client *p = NULL;
	client *q = NULL;

    // 修改公用资源先上锁
    pthread_mutex_lock(&m);
	list_for_each_entry_safe(p,q,&(c_head->list),list)
	{
        // 找到该节点了
		if(p->connfd == connfd)  
		{
            //断开该节点
			printf("%s已退出\n",p->client_name);
			list_del(&(p->list)); 
			free(p);

            // 修改后解锁
            pthread_mutex_unlock(&m);
			return 0;
		}
	}
	return 0;
}

int save_usr_data_file(regin *usr)
{
	int n = 0;
	FILE *fp = NULL;
	char file_name[30] = {0};
	char file_info_buff[200] = {0};

    // 保存用户注册信息到文件
	sprintf(file_name,"./data/usr_data/%s.txt",usr->std.name);
	fp = fopen(file_name,"w");
	if(fp == NULL)
		perror("fopen");
	sprintf(file_info_buff,"%s,%s,%s,%s,%s,%d,%d,%d,%s,%d",	usr->std.name,
														    usr->std.passwd,
														    usr->std.tel,
														    usr->std.Sec_quation,
														    usr->std.Sec_answer,
														    usr->std.real_name_flag,
														    usr->std.memony,
														    usr->std.Vip,
														    usr->std.real_name,
														    usr->std.type);
	n = fwrite(file_info_buff,strlen(file_info_buff),1,fp);
	if(n < 1)
		perror("fwrite");

	fclose(fp);
	return 0;
}


int check_usr_start_date(buy_ticket *b_head,flight *p,flight *f_head,int connfd)
{
	buy_ticket *q = b_head->next;
	flight *d = f_head->next;
	for(;q != b_head;q = q->next)
	{
		//购买有这个航班了
		if(strcmp(p->info.number,q->b_info.f_number) == 0)
		{
			send(connfd,"already exit ticket",strlen("already exit ticket"),0);
			return -1;
		}
		for(d;d != f_head;d = d->next)
		{
			if(strcmp(p->info.number,d->info.number) == 0)
			{
				//是否有当天票
				if(strcmp(p->info.number,d->info.number) == 0)
				send(connfd,"already day exit",strlen("already day exit"),0);
				return -2;
			}
		}
	}
	return 0;
}

int flight_delay_ins(regin *usr,int connfd)
{
	char rcv_ins[10]={0};
	while(1)
	{
		bzero(rcv_ins,sizeof(rcv_ins));
		if(recv(connfd,rcv_ins,sizeof(rcv_ins),0) >= 0)
		{
			printf("%s\n",rcv_ins);
			if(strcmp(rcv_ins,"1") == 0)
			{
				return 1;
			}
			if(strcmp(rcv_ins,"0") == 0)
			{
				return 0;
			}
		}
	}
}

int add_buy_ticket_tail(regin *usr,struct msg_handler *msg,flight *p)
{
	// 静态初始化互斥锁
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
	int connfd = (int )msg->connfd;
	regin *r_head = (regin *)msg->r_head;
	flight *f_head = (flight *)msg->f_head; 
	char rcv_take[100]={0};

	// 1.有票判断是否是会员、会员打9折、是儿童半价、军人8折
	// 2.判断旅客类型如果是军人儿童，无论是否是会员都是半价和８折
	int b_money;
	buy_ticket * new = NULL;
	new = (buy_ticket *)malloc(sizeof(buy_ticket));
	if(new == NULL)
	{
		perror("malloc new error");
		exit(0);
	}

	//4.判断是否有这个人
	
	int ret_ins;
	int check;

	while (1)
	{
		bzero(rcv_take,sizeof(rcv_take));
		if(recv(connfd,rcv_take,sizeof(rcv_take),0) >= 0)
		{
			regin *u = r_head->next;
			for(u;u != r_head;u = u->next)
			{
				// 5.有真实名字的都实名了 //是否有同一天的飞机，
				if(strcmp(u->std.real_name,rcv_take) == 0 )
				{
					// 3.判断该用户是否有同一天的机票
					if(check = check_usr_start_date(&(u->b_ticket_list),p,f_head,connfd) < 0)
					{
						return 0;
					}
					pthread_mutex_lock(&m);
					if(u->std.type == 1)      
					{
						//3.是儿童,半价
						b_money = p->info.price / 2;
					}
					if(u->std.type == 2)
					{
						//4.是军人8折
						b_money = p->info.price *0.8;
					}
					if(u->std.type == 0 && usr->std.Vip == 0)
					{
						//5.是成人购买者不是会员
						b_money = p->info.price;
					}
					if(u->std.type == 0 && usr->std.Vip == 1)
					{
						//6.是成人购买者是会员
						b_money = p->info.price*0.9;
					}
					//7.判断用户是否钱够
					if(usr->std.memony > b_money)
					{
						// 有足够的钱，扣钱
						usr->std.memony -= b_money;

						// 是否需要买保险
						ret_ins = flight_delay_ins(usr,connfd);
						if(ret_ins == 1)
						{
							//8.为乘车着买了保险，实际花钱加100元，购买者扣100元
							new->b_info.ins_flag = 1;
							new->b_info.buy_price = b_money + 100;
							usr->std.memony -= 100;

						}
						if(ret_ins == 0)
						{
							new->b_info.ins_flag = 0;
							new->b_info.buy_price = b_money;
						}
						// 购买票的人，乘坐这趟航班的人，乘坐的航班号,购买的实际价格、是否买了保险
						strcpy(new->b_info.buy_usr,usr->std.name);
						strcpy(new->b_info.take_usr,u->std.real_name);
						strcpy(new->b_info.f_number,p->info.number);
						//指针域赋值
						new->next = &(u->b_ticket_list); //可以修改便于检查

						buy_ticket *q = (&(u->b_ticket_list))->prev;
						q->next = new;
						new->prev =  q;
						(&(u->b_ticket_list))->prev = new;
						p->info.poll--;

						save_flight_date(p);
						save_usr_data_file(usr);
						save_usr_buy_flight_data(new);
		
						//购买票成功
						send(connfd,"successful",strlen("successful"),0);
						pthread_mutex_unlock(&m);

						return 0;
						}
					else
					{
					send(connfd,"no monoy",strlen("no monoy"),0);
					return -1;
					}
				}
			}
			send(connfd,"no realname",strlen("no realname"),0);	
			return 0;
		}
	}
}

int buy_flight_fun(regin *usr,struct msg_handler *msg)
{
	char snd_buf[300];
	char buy_buf[20];
	flight *f_head = (flight *)msg->f_head;
	flight *p = f_head->next;
	int connfd = (int)msg->connfd;
	// 判断用户是否实名认证了
	if(usr->std.real_name_flag == 1)
	{
		// 发送所有航班头给客户端
		sprintf(snd_buf,"%s","=======================================================================\n");
		send(connfd,snd_buf,sizeof(snd_buf),0);
		sprintf(snd_buf,"%s","航班号\t出发地\t目的地\t  班期\t   机型   起飞时间\t票价\t余票\n");
		send(connfd,snd_buf,sizeof(snd_buf),0);

		// 发送所有航班信息给客户端
		show_all_flight_data(f_head,connfd);
		
		// 接受客户端发送过来的航班号
		while (1)
		{
			bzero(buy_buf,sizeof(buy_buf));
			if(recv(connfd,buy_buf,sizeof(buy_buf),0)>=0)
			{
				printf("%s",buy_buf);
				for(;p != f_head;p = p ->next)
				{
					//判断是否有这趟航班
					if(strcmp(p->info.number,buy_buf) == 0)
					{
						//判断是否还有余票
						if(p->info.poll > 0)
						{
							add_buy_ticket_tail(usr,msg,p);
							return 0;
						}
						else
						{
							// 向用户发送没有实名的信息
							send(connfd,"no poll",strlen("no poll"),0);
							return -2;
						}	
					}
				}
				send(connfd,"no flydate",strlen("no flydate"),0);
				return 0;
			}
		}
	}
	else
	{
		// 向用户发送没有实名的信息
		send(connfd,"no realname",strlen("no realname"),0);
		return 0;
	}
	
	
}

int check_real_id(regin *usr,struct msg_handler *msg)
{
	real_id *i_head = (real_id *)msg->i_head;
	int connfd = msg->connfd;

	real_id *p =i_head->next;
	p = i_head ->next;
	char rcv_buf[100];
	char taken [] = ",";
	char *buff = NULL;
	char real_name_buf[10];
	char real_ID_buf[20];
	printf("%d\n",usr->std.real_name_flag);
	// 判断用户是否实名
	if(usr->std.real_name_flag == 0)
	{
		while(1)
		{
			// 用户还没有实名
			bzero(rcv_buf,sizeof(rcv_buf));
			if(recv(connfd,rcv_buf,sizeof(rcv_buf),0) >= 0)
			{
				buff = strtok(rcv_buf,taken);
				strncpy(real_name_buf,buff,sizeof(buff));

				buff = strtok(NULL,taken);
				strncpy(real_ID_buf,buff,sizeof(buff));
				for(p;p != i_head;p = p->next)
				{

					if((strcmp(real_name_buf,p->r_info.real_name) == 0) && (strcmp(real_ID_buf,p->r_info.real_idnum) == 0))         //验证姓名和身份证是否一致
					{
						// 将实名认证标志位置1
						usr->std.real_name_flag = 1;
						// 将旅客类型添加到用户的旅客类型上
						usr->std.type = p->r_info.real_type;
						// 将真实名字添加到用户数据上
						strcpy(usr->std.real_name,p->r_info.real_name);
						send(connfd,"successful",strlen("successful"),0);
						save_usr_data_file(usr);
						return 0;
					}
				}
				// 没有相关信息
				break;
			}
		}
		
	}
	else
	{
		// 用户已经完成实名
		send(connfd,"successful",strlen("successful"),0);
		return 0;
	}
	return 0;
}

int recharge(regin *usr,int connfd)                //充值
{
	char snd_buf[200] = {0};
	char rcv_buf[100] = {0};
	//提示，充值一次性充值3000升级为砖石会员
	bzero(snd_buf,sizeof(snd_buf));
	sprintf(snd_buf,"%s","尊敬的用户，一次性充值3000元，将成为砖石会员，买票将享受8折优化哦");
	send(connfd,snd_buf,sizeof(snd_buf),0);
	while(1)
	{
		bzero(rcv_buf,sizeof(rcv_buf));
		if(recv(connfd,rcv_buf,sizeof(rcv_buf),0)>=0)
		{
			if(atoi(rcv_buf) >= 3000)
			{
				usr->std.Vip = 1;
				usr->std.memony += atoi(rcv_buf);
				save_usr_data_file(usr);
			}	
			else
			{
				usr->std.memony += atoi(rcv_buf);
				save_usr_data_file(usr);
				
			}
			send(connfd,"充值成功!",strlen("充值成功!"),0);
			return 0;
		}
		
	}	
}

int print_usr_menoy(regin *usr,int connfd)                                      //产看余额
{
	// 向用户发送账户余额
	char snd_buf[100] = {0};
	sprintf(snd_buf,"您的账户余额为：%d",usr->std.memony);
	send(connfd,snd_buf,sizeof(snd_buf),0);
	return 0;
}

int  show_all_per_order(regin *usr,flight *f_head,int connfd)
{
	char snd_buf[300] = {0};
	buy_ticket *b_head =(buy_ticket *)&(usr->b_ticket_list);
	buy_ticket *p = b_head->next;
	int i = 0;
	// 发送所有航班头给客户端
	sprintf(snd_buf,"%s","============================================================================\n");
	send(connfd,snd_buf,sizeof(snd_buf),0);
	sprintf(snd_buf,"%s","航班号\t出发地\t目的地\t  班期\t   机型   起飞时间\t票价\t航班保险   已支付\n");
	send(connfd,snd_buf,sizeof(snd_buf),0);
	
	for(;p != b_head;p = p ->next)
	{
		flight *q = f_head->next;
		for(q;q != f_head;q = q -> next)
		{
			bzero(snd_buf,sizeof(snd_buf));
			if(strcmp(p->b_info.f_number,q->info.number) == 0)
			{
				sprintf(snd_buf," %s\t %s\t %s\t%s   %s\t  %s  \t%d\t  %d\t   %d\n",q->info.number,
											q->info.staddress,
											q->info.arraddress,
											q->info.date,
											q->info.type,
											q->info.stime,
											q->info.price,
											p->b_info.ins_flag,
											p->b_info.buy_price);
				send(connfd,snd_buf,strlen(snd_buf),0);
				i++;

			}
		}
	}
	if(p->next == &(usr->b_ticket_list) && i == 0)
	{
		send(connfd,"no fly",strlen("no fly"),0);
		return -1;
	}
	return 0;
}

int  back_monoy(regin *r_head,buy_ticket *p)
{
	char file_name[100] = {0};
	regin *u = r_head->next;
	for(u;u != r_head;u = u->next)
	{
		//找到购买的用户
		if(strcmp(p->b_info.buy_usr,u->std.name) == 0)
		{
			if(u->std.Vip == 1)
			{
				//2会员不需要手续费
				u->std.memony += p->b_info.buy_price;
			}
			if(u->std.Vip == 0)
			{
				//2.不是会员收购买票时的10%的手续费
				u->std.memony += (p->b_info.buy_price * 0.9);
			}
		}
	}
	//3.删除文件信息
	sprintf(file_name,"rm ./data/buy_ticket_data/%s_%s.txt",p->b_info.take_usr,p->b_info.f_number);
	system(file_name);
	return 0;
}

void add_flight_poll(flight *f_head,buy_ticket *p)
{
	// 静态初始化互斥锁
	pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
	char file_name[100] = {0};
	flight *f = f_head->next;
	for(;f != f_head;f = f->next)
	{
		if(strcmp(f->info.number,p->b_info.f_number) == 0)
		{	
			pthread_mutex_lock(&m);
			//1.找到需要增加余票的航班
			f->info.poll++;
			pthread_mutex_unlock(&m);
			//2.航班数据保存
			save_flight_date(f);
		}
	}
	//3.删除文件信息
	sprintf(file_name,"rm ./data/buy_ticket_data/%s_%s.txt",p->b_info.take_usr,p->b_info.f_number);
	system(file_name);
}

int back_ticket(regin *usr,struct msg_handler *msg)
{
	int ret = 0;
	flight *f_head = (flight *)msg->f_head;
	regin *r_head = (regin *)msg->r_head;
	int connfd = (int )msg->connfd;

	buy_ticket *b_head =(buy_ticket *) &(usr->b_ticket_list);
	// 1.显示用户的订单
	if(ret = show_all_per_order(usr,f_head,connfd) < 0)
	{
		// 没有订单需要退票
		return 0;
	}

	// 接受用户退的航班号
	char ticket_number[10];
	buy_ticket *q = b_head;
	buy_ticket *p =  b_head->next;
	while(1)
	{
		bzero(ticket_number,sizeof(ticket_number));
		if(recv(connfd,ticket_number,sizeof(ticket_number),0) >= 0)
		{
			for(q,p;p != b_head;q = p,p = p->next)
			{
				//2.找到需要退票的航班
				if(strcmp(p->b_info.f_number,ticket_number) == 0)
				{
					//3.退钱
					back_monoy(r_head,p);
					//5.该航班的余票加一
					add_flight_poll(f_head,p);
					//4.释放该节点
					q->next = p->next;
					p->next->prev = q;
					free(p);

					return 0;
				}
			}
			break;
		}
	}
	
	send(connfd,"num error",strlen("num error"),0);
	printf("no\n");
	return -1;
}

int check_number_poll(flight *f_head,char *buy_number,int connfd)
{
	flight *p = f_head->next;
	for(p;p != f_head;p = p->next)
	{
		if(strcmp(p->info.number,buy_number) == 0)
		{
			if(p->info.poll > 0)
			{
				return p->info.price;
			}
			else
			{
				send(connfd,"no poll",strlen("no poll"),0);
				return -2;
			}
			
		}
	}
	send(connfd,"num error",strlen("num error"),0);
	return -1;
}

int check_monoy_change(regin *usr,int pay)
{
	if(usr->std.memony > pay)
	{
		return 0;
	}
	else
	{
		return -1;
	}
	
}

int check_monoy(regin *usr,flight *f_head,buy_ticket *b,int connfd,int monoy)
{
	// 判断退钱还是支付
	int ret;
	char snd_buf[100] = {0};
	char rcv_buf[100];
	flight *p = f_head->next;
	printf("mai %d\n",monoy);
	for(p;p != f_head;p = p->next)
	{
		if(strcmp(p->info.number,b->b_info.f_number) == 0)
		{
			// 新票价格大于旧票
			printf("gai%d\n",p->info.price);
			if(p->info.price < monoy)
			{
				// 改签的航班费用大于购买的航班，提示需要额外支付
				sprintf(snd_buf,"改签需要额外支付%d元，是否进行改签，当前账户余额为￥%d  1--> 是 0 --> 否!\n",
								monoy - p->info.price,usr->std.memony);
				send(connfd,snd_buf,strlen(snd_buf),0);
				while(1)
				{
					bzero(rcv_buf,sizeof(rcv_buf));
					if(recv(connfd,rcv_buf,sizeof(rcv_buf),0) >=0)
					{
						if(strcmp(rcv_buf,"1") == 0)
						{
							// 进行扣钱,判断是否钱够
							if(ret = check_monoy_change(usr,p->info.price - monoy) < 0)
							{	
								return -2;
							}
							else
							{
								if(usr->std.Vip == 1)
								{
									// 是会员则不扣除手续费
									usr->std.memony -= (monoy - p->info.price);
									b->b_info.buy_price  += (monoy - p->info.price);
										// 扣钱
									return 0;
								}
								else
								{
									// 不是会员扣除元票价的10%手续费
									usr->std.memony -= (monoy - (p->info.price) * 0.9);
									b->b_info.buy_price  += (monoy - p->info.price);
								}
								
								
							}return 0;
						}
						if(strcmp(rcv_buf,"0") == 0)
						{
							// 取消改签操作
							return -1;
						}
					}
				}
			}
			else
			{
				// 改签的航班费用小于购买的航班，退钱
				if(usr->std.Vip == 1)
				{
					usr->std.memony += (p->info.price - monoy);
					b->b_info.buy_price  -= (p->info.price - monoy);
					return 0;
				}
				else
				{
					usr->std.memony += ((p->info.price)*0.9 - monoy);
					b->b_info.buy_price  -= ((p->info.price)*0.9 - monoy);
					return 0;
				}
				
			}
			
		}
	}
	return 0;
}

int sub_flight_poll(flight *f_head,char *ticket_number)
{
	flight * p = f_head->next;

	for(p;p != f_head;p = p->next)
	{
		if(strcmp(p->info.number,ticket_number) == 0)
		{	
			p->info.poll--;
			return 0;
		}
	}
	return -1;
}

int add_old_flight(flight *f_head,char *add_number)
{
	flight *p = f_head->next;
	for(p;p != f_head;p = p->next)
	{
		if(strcmp(p->info.number,add_number) == 0)
		{
			p->info.poll++;
		}
	}
	return 0;
}

void rm_old_ticket(char *take_name,char *old_number)
{
	char rm_ticket[100] = {0};

	sprintf(rm_ticket,"rm ./data/buy_ticket_data/%s_%s.txt",take_name,old_number);
	system(rm_ticket);

}
int change_ticket(regin *usr,struct msg_handler *msg)
{
	int ret;
	flight *f_head = (flight *)msg->f_head;
	regin *r_head = (regin *)msg->r_head;
	int connfd = (int )msg->connfd;

	buy_ticket *b_head =(buy_ticket *) &(usr->b_ticket_list);
	// 1.显示用户的订单
	if(ret = show_all_per_order(usr,f_head,connfd) < 0)
	{
		// 没有订单需要退票
		return 0;
	}

	char ticket_number[10];
	buy_ticket *p =  b_head->next;

	// 接受客户端发送的需要改签的航班号
	while(1)
	{
		bzero(ticket_number,sizeof(ticket_number));
		if(recv(connfd,ticket_number,sizeof(ticket_number),0) >= 0)
		{
			for(p;p != b_head;p = p->next)
			{
				// 2.找到需要改签的航班
				if(strcmp(p->b_info.f_number,ticket_number) == 0)
				{
					// 接收新的航班号
					while(1)
					{
						bzero(ticket_number,sizeof(ticket_number));
						if(recv(connfd,ticket_number,sizeof(ticket_number),0) >= 0)
						{
							printf("%s\n",ticket_number);
							// 判断需要改签的航班号是否有票
							ret = check_number_poll(f_head,ticket_number,connfd);
							if(ret < 0)
							{
								return -1;
							}
							// 3.判断该用户是否有同一天的机票

							// 判断需要支付退钱
							if(ret = check_monoy(usr,f_head,p,connfd,ret) == -1)
							{
								// 取消改签费用
								send(connfd,"no pay",strlen("no pay"),0);
								return -2;
							}

							// 5.原来航班的余票加一
							add_old_flight(f_head,p->b_info.f_number);
							
							rm_old_ticket(p->b_info.take_usr,p->b_info.f_number);

							// 5.改签的航班的余票加减一
							sub_flight_poll(f_head,ticket_number);

							// 4.将票信息给为改签名字
							strncpy(p->b_info.f_number,ticket_number,sizeof(ticket_number));
							save_usr_buy_flight_data(p);
									
							//购买票成功
							send(connfd,"successful",strlen("successful"),0);

							return 0;
								
						}
					}
				}
			}
			// 没有找到该航班
			break;
		}
	}
	send(connfd,"num error",strlen("num error"),0);
	printf("no\n");
	return 0;
}

