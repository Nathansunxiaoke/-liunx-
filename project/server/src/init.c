#include"head.h"


client *init_client_list(client *c_head)
{
    // 为客户端的头节点申请空间
    c_head = (client *)malloc(sizeof(client));
    if(c_head == NULL)
    {
        perror("malloc c_head error\n");
        exit(0);
    }

    // 为客户端的指针域赋值
    INIT_LIST_HEAD(&(c_head->list));

    return c_head;
}

regin *init_regin_list(regin *r_head)                                                         //初始化注册头结点
{
	r_head = (regin *)malloc(sizeof(regin));              //位头节点申请空间
	if(r_head == NULL)
		perror("malloc r_head");

	//双向循环链表初始化头节点，前驱节点和后驱节点都指向头
	r_head->next = r_head;
	r_head->prev = r_head;

	return r_head;	
}

login *init_login_list(login *l_head)                                                          //初始化登陆的头结点
{
	l_head = (login *)malloc(sizeof(login));
	if(l_head == NULL)
		perror("malloc l_head");

	l_head->prev = l_head;
	l_head->next = l_head;

	return l_head;
}

flight *init_flight_list(flight *f_head)                                                       //初始化飞机链的头结点
{
	f_head = (flight *)malloc(sizeof(flight));
	if(f_head == NULL)
	{
		perror("malloc f_head");
		exit(0);
	}
	//指针域赋值
	f_head->prev = f_head;
	f_head->next = f_head;

	return f_head;
}

real_id *init_read_id_list(real_id *i_head)                                                   //初始化真实身份数据链
{
	i_head = (real_id *)malloc(sizeof(real_id));
	if(i_head == NULL)
	{
		perror("malloc f_head");
		exit(0);
	}
	//指针域赋值
	i_head->prev = i_head;
	i_head->next = i_head;

	return i_head;
}

buy_ticket *init_buy_ticket_list(regin *old_usr,buy_ticket *b_head)
{
	b_head = (buy_ticket *)malloc(sizeof(buy_ticket));
	if(b_head == NULL)
	{
		perror("malloc error");
		exit(0);
	}

	//双向循环链表初始化头节点，前驱节点和后驱节点都指向头
	b_head->prev = &(old_usr->b_ticket_list);               //指向初始化后的头节点
	b_head->next = &(old_usr->b_ticket_list);

	return b_head;	
}

del_flight *init_delflight_list(del_flight *d_head)
{
	// 为删除的航班申请空间
	d_head = (del_flight *)malloc(sizeof(del_flight));
	if(d_head == NULL)
	{
		perror("malloc d_head error\n");
        exit(0);
	}

	// 双向循环链表初始化头节点，前驱节点和后驱节点都指向头
	// 为客户端的指针域赋值
    INIT_LIST_HEAD(&(d_head->list));


	return d_head;	
}

// ==================================init===============

void read_usr_buy_info(regin *r_head,buy_ticket *b_head,char *file_name)
{
	
	FILE *fp = NULL;
	char taken[] = ",";
	char *buff = NULL;
	int n;
	char usr_buy_buff[100] = {0};

	fp = fopen(file_name,"r");
	if(fp ==  NULL)
		perror("fopen");
	n = fread(usr_buy_buff,sizeof(usr_buy_buff),1,fp);
	if(n < 0)
		perror("fread");

	//为数据域赋值

	buff = strtok(usr_buy_buff,taken);
	regin *p = r_head->next;
	for(;p != r_head;p = p->next)
	{
		if(strcmp(p->std.name,buff) == 0)
		{
			buy_ticket *new =NULL;
			new = (buy_ticket *)malloc(sizeof(buy_ticket));
			strcpy(new->b_info.buy_usr,buff);//购买者
			buff = strtok(NULL,taken);
			strcpy(new->b_info.take_usr,buff);//乘车者
			buff = strtok(NULL,taken);
			strcpy(new->b_info.f_number,buff);//乘坐的航班号
			buff = strtok(NULL,taken);
			new->b_info.buy_price = atoi(buff);//买票的钱
			buff = strtok(NULL,taken);
			new->b_info.ins_flag = atoi(buff);//是否购买保险

			//指针域
			for(p = r_head->next;p != r_head;p = p->next)
			{
				//乘车的人的信息
				if(strcmp(p->std.real_name,new->b_info.take_usr) == 0)
				{
					new->next = &(p->b_ticket_list);
					
					//买票的头节点
					buy_ticket *u = (&(p->b_ticket_list))->prev;
					u->next = new;
					new->prev = u;
					(&(p->b_ticket_list))->prev = new;
				}
			}
		}
	}
}

int read_usr_info(regin *r_head,char *file_name)                                               //读取已注册用户的信息
{
	FILE *fp = NULL;
	char taken[] = ",";
	char *buff = NULL;
	int n;
	char usr_data_buff[100] = {0};

	regin *old_usr = NULL;                       //为老用户添加结点
	old_usr = (regin *)malloc(sizeof(regin));
	if(old_usr == NULL)
		perror("malloc old_usr");

	fp = fopen(file_name,"r");
	if(fp ==  NULL)
		perror("fopen");
	n = fread(usr_data_buff,sizeof(usr_data_buff),1,fp);
	if(n < 0)
		perror("fread");

	//为数据域赋值

	buff = strtok(usr_data_buff,taken);
	strcpy(old_usr->std.name,buff);     //姓名

	buff = strtok(NULL,taken);
	strcpy(old_usr->std.passwd,buff);  //密码

	buff = strtok(NULL,taken);
	strcpy(old_usr->std.tel,buff);  //电话号码

	buff = strtok(NULL,taken);
	strcpy(old_usr->std.Sec_quation,buff);  //密保问题

	buff = strtok(NULL,taken);
	strcpy(old_usr->std.Sec_answer,buff);  //密保答案

	buff = strtok(NULL,taken);
	old_usr->std.real_name_flag = atoi(buff);       //是否实名认证

	buff = strtok(NULL,taken);
	old_usr->std.memony = atoi(buff);     //余额

	buff = strtok(NULL,taken);
	old_usr->std.Vip = atoi(buff);     //是否是会员

	buff = strtok(NULL,taken);
	strcpy(old_usr->std.real_name,buff);  //真实姓名

	buff = strtok(NULL,taken);
	old_usr->std.type = atoi(buff);     //用户旅客类型

	old_usr->b_ticket_list = *(init_buy_ticket_list(old_usr,&old_usr->b_ticket_list));                    //初始化用户的购票链

	old_usr->next = r_head;

	regin *p = r_head->prev;
	//指针域赋值
	p->next = old_usr;
	old_usr->prev = p;
	r_head->prev = old_usr;
	return 0;
}

int init_old_usr_data(regin *r_head)                                                           //初始化注册过的用户
{
	int ret;
	DIR *dp = NULL;
	dp = opendir("./data/usr_data");                      //带开到用户储用户信息的地方
	if(dp ==  NULL)
		perror("opendir");
	ret = chdir("./data/usr_data");                              //切换到储存用户的目录下
	if(ret == -1)
		perror("chdir");
	struct dirent *usr_data_buff = NULL;
	while(1)
	{
		usr_data_buff = readdir(dp);
		if(usr_data_buff == NULL) //没有文件
			break;
		if(usr_data_buff->d_name[0] == '.')  //是隐藏文件继续读
		{
			continue;
		}
		read_usr_info(r_head,usr_data_buff->d_name);
	}
	ret = chdir("./../..");
	if(ret == -1)
		perror("chdir");
	closedir(dp);

	return 0;
}

int init_buy_ticket_date(regin *r_head,buy_ticket *b_head)
{
	int ret;
	DIR *dp = NULL;
	dp = opendir("./data/buy_ticket_data");
	if(dp == NULL)
	{
		perror("opendir error\n");
	}
	ret = chdir("./data/buy_ticket_data");
	struct dirent *usr_buy_buff = NULL;
	while(1)
	{
		usr_buy_buff = readdir(dp);
		if(usr_buy_buff == NULL) //没有文件
			break;
		if(usr_buy_buff->d_name[0] == '.')  //是隐藏文件继续读
		{
			continue;
		}
		read_usr_buy_info(r_head,b_head,usr_buy_buff->d_name);
	}
	ret = chdir("./../..");
	if(ret == -1)
		perror("chdir");
	closedir(dp);

	return 0;

}

void save_usr_buy_flight_data(buy_ticket *new_usr)
{
	int n = 0;
	FILE *fp = NULL;
	char file_name[50] = {0};
	char file_info_buf[100] = {0};

	sprintf(file_name,"./data/buy_ticket_data/%s_%s.txt",new_usr->b_info.take_usr,new_usr->b_info.f_number);
	fp = fopen(file_name,"w");
	if(fp == NULL)
		perror("fopen");
	sprintf(file_info_buf,"%s,%s,%s,%d,%d,",new_usr->b_info.buy_usr,
											new_usr->b_info.take_usr,
											new_usr->b_info.f_number,
											new_usr->b_info.buy_price,
											new_usr->b_info.ins_flag);
	n = fwrite(file_info_buf,strlen(file_info_buf),1,fp);
	if(n < 1)
		perror("fwrite");
	fclose(fp);
}

void save_flight_date(flight *p)
{
	int n = 0;
	FILE *fp = NULL;
	char file_name[50] = {0};
	char file_info_buf[100] = {0};

	sprintf(file_name,"./data/flight_data/%s.txt",p->info.number);
	fp = fopen(file_name,"w");
	if(fp == NULL)
		perror("fopen");
	sprintf(file_info_buf,"%s,%s,%s,%s,%s,%s,%d,%d",p->info.number,
													p->info.staddress,
													p->info.arraddress,
													p->info.date,
													p->info.type,
													p->info.stime,
													p->info.price,
													p->info.poll);
	n = fwrite(file_info_buf,strlen(file_info_buf),1,fp);
	if(n < 1)
		perror("fwrite");
	fclose(fp);
}

int read_flight_data_buff(flight *f_head,char *file_name)                    
{
	FILE *fp;
	int n;
	char taken[] = ",";
	char *buff;
	char flight_data_buff[100] = {0};      //飞机的数据
	fp = fopen(file_name,"r");
	if(fp == NULL)
	{
		perror("fopen flight_data");
		exit(0);
	}
	n = fread(flight_data_buff,sizeof(flight_data_buff),1,fp);
	if(n < 0)
	{
		perror("fread flight_data");
		exit(0);
	}
	flight *flight_data = NULL;
	flight_data = (flight *)malloc(sizeof(flight));
	if(flight_data == NULL)
	{
		perror("malloc flight_data");
		exit(0);
	}
	//为数值域赋值
	buff = strtok(flight_data_buff,taken);
	strcpy(flight_data->info.number,buff);       //航班号

	buff =  strtok(NULL,taken);
	strcpy(flight_data->info.staddress,buff);       //出发地

	buff =  strtok(NULL,taken);
	strcpy(flight_data->info.arraddress,buff);       //目的地

	buff =  strtok(NULL,taken);
	strcpy(flight_data->info.date,buff);       //班期

	buff =  strtok(NULL,taken);
	strcpy(flight_data->info.type,buff);       //机型

	buff =  strtok(NULL,taken);
	strcpy(flight_data->info.stime,buff);       //出发时间

	buff =  strtok(NULL,taken);
	flight_data->info.price = atoi(buff);             //价格

	buff =  strtok(NULL,taken);
	flight_data->info.poll =  atoi(buff);             //余票

	flight_data->next = f_head;
	//指针域赋值
	flight *p = f_head->prev;

	p->next = flight_data;
	flight_data->prev =  p;
	f_head->prev = flight_data;

	return 0;
}

int init_flight_data(flight *f_head)                                                            //初始化飞机数据
{
	int ret;
	DIR *dp;
	dp = opendir("./data/flight_data");
	if(dp == NULL)
	{
		perror("opendir flight_data");
		exit(0);
	}
	ret = chdir("./data/flight_data");
	if(ret == -1)
	{
		perror("chdir flight_data");
		exit(0);
	}
	struct dirent *flight_data_buff = NULL;
	while(1)
	{
		flight_data_buff = readdir(dp);
		if(flight_data_buff == NULL)              //没有文件
			break;
		if(flight_data_buff->d_name[0] == '.')    //隐藏文件
		{
			continue;
		}
		read_flight_data_buff(f_head,flight_data_buff->d_name);     //文件名
	}
	ret = chdir("./../.."); //切换路径回到当前目录
	if(ret == -1)
		perror("chdir");
	closedir(dp);
	return 0;
}

int read_delfly_data_buff(del_flight *d_head,char *file_name)    //文件名
{
	FILE *fp;
	int n;
	char taken[] = ",";
	char *buff;
	char del_flight_info[100] = {0};      //飞机的数据
	fp = fopen(file_name,"r");
	if(fp == NULL)
	{
		perror("fopen flight_data");
		exit(0);
	}
	n = fread(del_flight_info,sizeof(del_flight_info),1,fp);
	if(n < 0)
	{
		perror("fread flight_data");
		exit(0);
	}

	del_flight *new = NULL;
	new = (del_flight *)malloc(sizeof(del_flight));
	if(new ==  NULL)
	{
		perror("malloc errror\n");
		exit(0);
	}
	//为数值域赋值
	buff = strtok(del_flight_info,taken);
	strcpy(new->d_flight.number,buff);       //航班号

	buff =  strtok(NULL,taken);
	strcpy(new->d_flight.staddress,buff);       //出发地

	buff =  strtok(NULL,taken);
	strcpy(new->d_flight.arraddress,buff);       //目的地

	buff =  strtok(NULL,taken);
	strcpy(new->d_flight.date,buff);       //班期

	buff =  strtok(NULL,taken);
	strcpy(new->d_flight.type,buff);       //机型

	buff =  strtok(NULL,taken);
	strcpy(new->d_flight.stime,buff);       //出发时间

	buff =  strtok(NULL,taken);
	new->d_flight.price = atoi(buff);             //价格

	buff =  strtok(NULL,taken);
	new->d_flight.poll =  atoi(buff);             //余票

	// 指针域
	
	list_add_tail(&(new->list),&(d_head->list));
	return 0;
}

int init_delflight_date(del_flight *d_head)
{
	int ret;
	DIR *dp;
	dp = opendir("./data/del_flight_data");
	if(dp == NULL)
	{
		perror("opendir del_flight_data");
		exit(0);
	}
	ret = chdir("./data/del_flight_data");
	if(ret == -1)
	{
		perror("chdir del_flight_data error");
		exit(0);
	}
	struct dirent *del_fly_file = NULL;
	while(1)
	{
		del_fly_file = readdir(dp);
		if(del_fly_file == NULL)              //没有文件
			break;
		if(del_fly_file->d_name[0] == '.')    //隐藏文件
		{
			continue;
		}
		read_delfly_data_buff(d_head,del_fly_file->d_name);     //文件名
	}
	ret = chdir("./../.."); //切换路径回到当前目录
	if(ret == -1)
		perror("chdir");
	closedir(dp);
	return 0;
}

int read_id_info(real_id *i_head,char *id_info)
{
	FILE *fp;
	int n;
	char taken[] = ",";
	char *buff;
	char read_id_info_buf[100] = {0};      //真实信息
	fp = fopen(id_info,"r");
	if(fp == NULL)
	{
		perror("fopen real id");
		exit(0);
	}
	n = fread(read_id_info_buf,sizeof(read_id_info_buf),1,fp);
	if(n < 0)
	{
		perror("fread real id");
		exit(0);
	}

	real_id *new = NULL;
	new = (real_id *)malloc(sizeof(real_id));
	if(new == NULL)
	{
		perror("malloc error");
		exit(0);
	}
	//为数值域赋值
	//为数值域赋值
	buff = strtok(read_id_info_buf,taken);
	strcpy(new->r_info.real_name,buff);       //真实姓名

	buff =  strtok(NULL,taken);
	strcpy(new->r_info.real_idnum,buff);       //真实省份号码

	buff =  strtok(NULL,taken);
	new->r_info.real_type = atoi(buff);     //旅客类型

	//指针域赋值
	new->next = i_head;
	real_id *p = i_head->prev;

	p->next = new;
	new->prev =  p;
	i_head->prev = new;

	return 0;
}

int init_read_id_data(real_id *i_head)                                                         //初始化实名认证数据链
{
	int ret;
	DIR *dp = NULL;
	dp = opendir("./data/real_id");                      //打开到用户储用户信息的地方
	if(dp ==  NULL)
		perror("opendir");
	ret = chdir("./data/real_id");                              //切换到储存用户的目录下
	if(ret == -1)
		perror("chdir");
	struct dirent *usr_data_buff = NULL;
	while(1)
	{
		usr_data_buff = readdir(dp);
		if(usr_data_buff == NULL) //没有文件
			break;
		if(usr_data_buff->d_name[0] == '.')  //是隐藏文件继续读
		{
			continue;
		}
		read_id_info(i_head,usr_data_buff->d_name);
	}
	ret = chdir("./../..");
	if(ret == -1)
		perror("chdir");
	closedir(dp);

	return 0;
}