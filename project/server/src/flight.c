#include"head.h"


int show_all_flight_data(flight *f_head,int connfd)                                                             //显示所有机票信息
{
	flight *p = f_head->next;
	char snd_buf[300];

	for(;p != f_head;p = p ->next)
	{
		bzero(snd_buf,sizeof(snd_buf));
		sprintf(snd_buf," %s\t %s\t %s\t%s   %s\t  %s  \t%d\t%d\n",p->info.number,
											p->info.staddress,
											p->info.arraddress,
											p->info.date,
											p->info.type,
											p->info.stime,
											p->info.price,
											p->info.poll);
		// 查询到一趟航班就发送给用户
		send(connfd,snd_buf,strlen(snd_buf),0);
	}
	return	0;
}

int usr_quit_login(regin *usr,login *l_head)
{
	login *q = l_head;
	login *p = l_head->next;
	for(q,p;p != l_head;q = p,p = p->next)
	{
		if(strcmp(usr->std.name,p->l_name) == 0)         //找到节点
		{
			q->next = p->next;
			p->next->prev = q;
			free(p);
			return 0;
		}
	}
	return -1;
}

void snd_flight_usr(int connfd,flight *p)
{
	char snd_fto_usr[300];

	// 将产寻到的用户信息发送给客户端
	bzero(snd_fto_usr,sizeof(snd_fto_usr));
	sprintf(snd_fto_usr," %s\t %s\t %s\t%s   %s\t  %s  \t%d\t%d\n",p->info.number,
																p->info.staddress,
																p->info.arraddress,
																p->info.date,
																p->info.type,
																p->info.stime,
																p->info.price,
																p->info.poll);
	// 查询到一趟航班就发送给用户
	send(connfd,snd_fto_usr,strlen(snd_fto_usr),0);
}

int srv_qf_num(flight *f_head,int connfd)
{
	char rcv_ufq_num[100];
	int i = 0;
	flight *p = f_head->next;

	// 接受客户端快速查询的航班号
	while (1)
	{
		bzero(rcv_ufq_num,sizeof(rcv_ufq_num));
		if(recv(connfd,rcv_ufq_num,sizeof(rcv_ufq_num),0) >= 0)
		{
			char snd_buf[300];
			for(p = f_head->next;p != f_head;p = p ->next)
			{
				// 快速查询接口
				if(strcmp(p->info.number,rcv_ufq_num) == 0)
				{
					// 有该航班的相关信息，可以进行发送
					snd_flight_usr(connfd,p);
					i++;
				}
			}
			break;
		}
	}

	// 判断是否查询到航班'
	if(p == f_head && strcmp(p->info.number,rcv_ufq_num) != 0 && i ==0)
	{
		send(connfd,"no fliaght number date",strlen("no fliaght number date"),0);
		return -1;
	}
	return 0;
}

int at_fly_date(flight *f_head,int connfd)
{
	char rcv_ufq_num[100];
	int i = 0;
	flight *p = f_head->next;

	// 接受客户端快速查询的航班号
	while (1)
	{
		bzero(rcv_ufq_num,sizeof(rcv_ufq_num));
		if(recv(connfd,rcv_ufq_num,sizeof(rcv_ufq_num),0) >= 0)
		{
			char snd_buf[300];
			sprintf(snd_buf,"%s","=======================================================================\n");
			send(connfd,snd_buf,sizeof(snd_buf),0);
            sprintf(snd_buf,"%s","航班号\t出发地\t目的地\t  班期\t   机型   起飞时间\t票价\t余票\n");
			send(connfd,snd_buf,sizeof(snd_buf),0);
			for(p = f_head->next;p != f_head;p = p ->next)
			{
				// 快速查询接口
				if(strcmp(p->info.arraddress,rcv_ufq_num) == 0)
				{
					// 有该航班的相关信息，可以进行发送
					snd_flight_usr(connfd,p);
					i++;
				}
			}
			break;
		}
	}

	// 判断是否查询到航班'
	if(p == f_head && strcmp(p->info.arraddress,rcv_ufq_num) != 0 && i == 0)
	{
		send(connfd,"no fliaght number date",strlen("no fliaght number date"),0);
		return -1;
	}
	return 0;
}

int date_fly_date(flight *f_head,int connfd)
{
	char rcv_ufq_num[100];
	int i = 0;
	flight *p = f_head->next;

	// 接受客户端快速查询的航班号
	while (1)
	{
		bzero(rcv_ufq_num,sizeof(rcv_ufq_num));
		if(recv(connfd,rcv_ufq_num,sizeof(rcv_ufq_num),0) >= 0)
		{
			char snd_buf[300];
			sprintf(snd_buf,"%s","=======================================================================\n");
			send(connfd,snd_buf,sizeof(snd_buf),0);
            sprintf(snd_buf,"%s","航班号\t出发地\t目的地\t  班期\t   机型   起飞时间\t票价\t余票\n");
			send(connfd,snd_buf,sizeof(snd_buf),0);
			for(p = f_head->next;p != f_head;p = p ->next)
			{
				// 快速查询接口
				if(strcmp(p->info.date,rcv_ufq_num) == 0)
				{
					// 有该航班的相关信息，可以进行发送
					snd_flight_usr(connfd,p);
					i++;
				}
			}
			break;
		}
	}

	// 判断是否查询到航班'
	if(p == f_head && strcmp(p->info.date,rcv_ufq_num) != 0 && i ==0)
	{
		send(connfd,"no fliaght number date",strlen("no fliaght number date"),0);
		return -1;
	}
	return 0;
}

int type_fly_date(flight *f_head,int connfd)
{
	char rcv_ufq_num[100];
	int i = 0;
	flight *p = f_head->next;

	// 接受客户端快速查询的航班号
	while (1)
	{
		bzero(rcv_ufq_num,sizeof(rcv_ufq_num));
		if(recv(connfd,rcv_ufq_num,sizeof(rcv_ufq_num),0) >= 0)
		{
			char snd_buf[300];
			sprintf(snd_buf,"%s","=======================================================================\n");
			send(connfd,snd_buf,sizeof(snd_buf),0);
            sprintf(snd_buf,"%s","航班号\t出发地\t目的地\t  班期\t   机型   起飞时间\t票价\t余票\n");
			send(connfd,snd_buf,sizeof(snd_buf),0);
			for(p = f_head->next;p != f_head;p = p ->next)
			{
				// 快速查询接口
				if(strcmp(p->info.type,rcv_ufq_num) == 0)
				{
					// 有该航班的相关信息，可以进行发送
					snd_flight_usr(connfd,p);
					i++;
				}
			}
			break;
		}
	}

	// 判断是否查询到航班'
	if(p == f_head && strcmp(p->info.type,rcv_ufq_num) != 0 && i ==0)
	{
		send(connfd,"no fliaght number date",strlen("no fliaght number date"),0);
		return -1;
	}
	return 0;
}
int price_fly_date(flight *f_head,int connfd)
{
	char rcv_ufq_num[100];
	int i = 0;
	flight *p = f_head->next;

	// 接受客户端快速查询的航班号
	while (1)
	{
		bzero(rcv_ufq_num,sizeof(rcv_ufq_num));
		if(recv(connfd,rcv_ufq_num,sizeof(rcv_ufq_num),0) >= 0)
		{
			char snd_buf[300];
			sprintf(snd_buf,"%s","=======================================================================\n");
			send(connfd,snd_buf,sizeof(snd_buf),0);
            sprintf(snd_buf,"%s","航班号\t出发地\t目的地\t  班期\t   机型   起飞时间\t票价\t余票\n");
			send(connfd,snd_buf,sizeof(snd_buf),0);
			for(p = f_head->next;p != f_head;p = p ->next)
			{
				// 快速查询接口
				if(p->info.price < atoi(rcv_ufq_num))
				{
					// 有该航班的相关信息，可以进行发送
					snd_flight_usr(connfd,p);
					i++;
				}
			}
			break;
		}
	}

	// 判断是否查询到航班'
	if(p == f_head && i ==0)
	{
		send(connfd,"no fliaght number date",strlen("no fliaght number date"),0);
		return -1;
	}
	return 0;
}

int srv_fly_date_by(flight *f_head,int connfd)
{
	char order[10];
	while(1)
	{
		bzero(order,sizeof(order));
		if(recv(connfd,order,sizeof(order),0) >= 0)
		{	
			printf("%s",order);
			switch (atoi(order))
			{
				case 1:
					// 目的的查询
					at_fly_date(f_head,connfd); 
                	break;
				case 2:
					// 班期的查询
					date_fly_date(f_head,connfd); 
					break;
				case 3:
					// 机型的查询
					type_fly_date(f_head,connfd); 
					break;
				case 4:
					// 价格的查询
					price_fly_date(f_head,connfd); 
					break;
				case 5:
					return 0;
			}
		}
	}
}
