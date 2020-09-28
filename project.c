#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<stdbool.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>
#include<linux/input.h>
#include<error.h>


//====================LCD图片
char static bmp_24_0[] = "./image/meun/welcome.bmp";                   //欢迎界面
char static bmp_24_1[] = "./image/meun/mian_meun.bmp";                 //系统主界面
char static bmp_24_2[] = "./image/meun/usr_login_meun.bmp";            //登陆界面
char static bmp_24_3[] = "./image/meun/regin_meun.bmp";                //注册界面
char static bmp_24_4[] = "./image/meun/usr_main_meun.bmp";         //用户个人主界面


struct usr_data                    //用户数据
{  
    char name[10];            //姓名 
    char passwd[20];          //密码
    char tel[15];             //电话号码
    char Sec_quation[50];     //密保
    char Sec_answer[20];	  //密保问题答案
    int real_name_flag;       //是否实名认证           
    int memony;               //余额
    int Vip;                  //是否是VIP    
};

struct flight_data                 //飞机基本信息
{
	char number[10];        //航班号
	char staddress[10];     //起点站
	char arraddress[15];    //终点站
	char date[15];          //班期
	char type[1];              //机型
	char stime[10];         //起飞时间
	unsigned int  price;    //票价
	int poll;	            //余票
};

struct real_info                 //用户真实信息
{
	char real_name[10];           //真实姓名          
	char real_idnum[20];          //真实身份证号码
	unsigned int  real_type;      //身份类型 0：成人 1：儿童 2：军人、残疾人
}；

typedef struct Real_type          //实名认证链
{
	struct real_info r_info;      //身份数据域

	struct Real_type *prev;        //前驱指针
	struct Real_type *next;        //后继指针
}real_id;

typedef struct Regin_usr           //注册链
{
	struct usr_data std;          //注册用户的信息
	                              //用户购票的相关信息

	struct Regin_usr *prev;
	struct Regin_usr *next;        
}regin;

typedef struct login_list          //登陆链
{
	struct usr_data l_std;         //登陆的用户信息
	                               //购票飞机票的数据

	struct login_list *prev;
	struct login_list *next;
}login;

typedef struct flight_list         //机票链
{
	struct flight_data info;        //机票信息

	struct flight_list *prev;       //前驱指针
	struct flight_list *next;      //后继指针
}flight;


//====================================bmp===============================================//
int show_all_lcd_bmp(char *bmp_path)                      //全屏显示一张24位bmp格式图片
{
    FILE *fp;                        //打开bmp为文件指针
    int fd;                         //打开LCD屏幕
    int n;                          //读取到的字节块
    char *bmp = NULL;              //内存映射


    char read_buff[480*800*3] = {0};        //读取、校正】、
    char write_buff[480*800*4] = {0};       
    fp = fopen(bmp_path,"r");
    if(fp == NULL)
    {
        perror("fopen");
        return 0;
    }

    fd = open("/dev/fb0",O_RDWR);
    if(fd == -1)
    {
        perror("open dev fb0");
        return 0;
    }

    fseek(fp,54,SEEK_SET);
    n = fread(read_buff,sizeof(read_buff),1,fp);
    if(n != 1)
    {
        perror("fread");
        return 0;
    }
    //调整ｂｍｐ格式的大小与分辨率一直
    fseek(fp,54,SEEK_SET);
    for (int i = 0,j = 0; i < 480*800*4;i+=4,j+=3)
    {
        write_buff[i] = read_buff[j];
        write_buff[i + 1] = read_buff[j+1];
        write_buff[i + 2] = read_buff[j+2];
        write_buff[i + 3] = 0;
    }
    
    //内存映射,必须按照图片的摆放规律进行映射
    bmp = (char *)mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    for(int i = 0;i < 480;i++)
    {
        for(int j = 0;j < 800 * 4;j++)
            memcpy(bmp + 800 * 4 * i + j,&write_buff[(479 - i) * 800*4 + j],1);
    }

    munmap(bmp,sizeof(bmp));
    fclose(fp);
    close(fd);
    return 0;
}

int Set_Pos_bmpsize(const char *bmp_path,int set_pos_x,int set_pos_y,int bmp_x,int bmp_y)     //在任意位置显示任意大小24位bmp图片
{
    FILE *fp = NULL;
    int lcd,n,k=0;
    int x_max = (set_pos_x + bmp_x) * 4;
    int y_max = (set_pos_y + bmp_y);
    char *bmpmem = NULL;
    char read_bmp_buff[bmp_x * bmp_y *3];
    char check_bmp_buff[bmp_x * bmp_y * 4];
    char show_buf[bmp_x * bmp_y *4];
    //1.if set postion more than screem retrn -1;
    if((set_pos_x + bmp_x) > 800 || (set_pos_y + bmp_y) > 480)
    {
        printf("Bmp too large\n");
        return -1;
    }

    fp = fopen(bmp_path,"r");              //open bmp 
    if(fp == NULL)
        printf("open bmp error\n");
    lcd = open("/dev/fb0",O_RDWR); 
    if(lcd == -1)
        printf("open dev fb0 error\n");

    fseek(fp,54,SEEK_SET);                   //seek RGB head

    n = fread(read_bmp_buff,bmp_x * bmp_y,3,fp);
    if(n != 3)
        printf("fread bmp error\n");
    for(int i = 0,j = 0;i < bmp_x * bmp_y * 3;j +=4,i +=3)              //check bmp 
    {
        check_bmp_buff[j] = read_bmp_buff[i];
        check_bmp_buff[j + 1] = read_bmp_buff[i + 1];
        check_bmp_buff[j + 2] = read_bmp_buff[i + 2];
        check_bmp_buff[j + 3] = 0;
    }
    for(int y=0;y<bmp_y;y++)
    {
        for(int x=0;x<bmp_x*4;x++)
        {
            show_buf[bmp_x*4*y+x] = check_bmp_buff[bmp_x*4*(bmp_y-1-y)+x];
        }
    }
    bmpmem = (char*)mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd,0);
    if(bmpmem == NULL)
        printf("mmap error!\n");
    
    //9.将show_buf的内容刷到内存上
    for(int y = set_pos_y;y < y_max;y++)
    {
        for(int x=set_pos_x*4;x<x_max;x++)
        {
            memcpy(bmpmem+800*4*y+x,&show_buf[k],1);
            k++;
        }
    }
    munmap(bmpmem,800*480*4);
    fclose(fp);
    close(lcd);
}
//========================================================
void wel_interface()                                                                          //欢迎界面
{
	show_all_lcd_bmp(bmp_24_0);
	sleep(3);
}
//=====================================init===============================
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

flight *init_flight_list(flight *f_head)
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

int save_usr_data_file(regin *new_usr)                                                          //保存用户注册的数据到文件
{
	int n = 0;
	FILE *fp = NULL;
	char file_name[30] = {0};
	char file_info_buff[100] = {0};

	sprintf(file_name,"./data/usr_data/%s.txt",new_usr->std.name);
	fp = fopen(file_name,"w");
	if(fp == NULL)
		perror("fopen");
	sprintf(file_info_buff,"%s,%s,%s,%s,%s,%d,%d,%d,",	new_usr->std.name,
														new_usr->std.passwd,
														new_usr->std.tel,
														new_usr->std.Sec_quation,
														new_usr->std.Sec_answer,
														new_usr->std.real_name_flag,
														new_usr->std.memony,
														new_usr->std.Vip);
	n = fwrite(file_info_buff,strlen(file_info_buff),1,fp);
	if(n < 1)
		perror("fwrite");
	fclose(fp);

	return 0;
}

int regin_new_usr(regin *r_head)                                                               //注册                     
{
	char regin_name_buff[10] = {0};
	char regin_passwd[20] = {0};
	//提示输入用户名、密码、电话号码、密保问题、密保答案
	printf("请输入用户名:");
	scanf("%s",regin_name_buff);
	regin *check = NULL;
	for(check = r_head->next;check != r_head;check = check->next)
	{
		if(strcmp(check->std.name,regin_name_buff) == 0)
		{
			printf("该用户名已经注册\n");
			return 0;
		}
	}

	regin *new_usr = NULL;
	new_usr = (regin *)malloc(sizeof(regin));
	if(new_usr == NULL)
		perror("malloc new_usr");
	strcpy(new_usr->std.name,regin_name_buff);       //用户名

	printf("请输入密码：");
	scanf("%s",new_usr->std.passwd);

	printf("请输入电话号码:");
	scanf("%s",new_usr->std.tel);

	printf("请输入密保问题:");
	scanf("%s",new_usr->std.Sec_quation);

	printf("请输入密保答案:");
	scanf("%s",new_usr->std.Sec_answer);

	new_usr->std.real_name_flag = 0;             //没有实名认证
	new_usr->std.memony = 0;                     //没有钱
	new_usr->std.Vip = 0;                     //不是VIP

	//数据域赋值结束     
	new_usr->next = r_head;            //新用户尾插下一个节点指向头               
	
	regin *p = r_head->prev;

	p->next = new_usr;                 //最后一个节点指向新节点
	new_usr->prev = p;                 //新节点前驱指向最后一个节点
	r_head->prev = new_usr;

	//数据域结束
	save_usr_data_file(new_usr);       //保存用户数据到文件

	return 0;
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
	strcpy(old_usr->std.Sec_quation,buff);  //密保问题

	buff = strtok(NULL,taken);
	strcpy(old_usr->std.Sec_answer,buff);  //密保答案

	buff = strtok(NULL,taken);
	old_usr->std.real_name_flag = atoi(buff);       //是否实名认证

	buff = strtok(NULL,taken);
	old_usr->std.memony = atoi(buff);     //余额

	buff = strtok(NULL,taken);
	old_usr->std.Vip = atoi(buff);     //是否是会员

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
//================================================用户界面
int deleta_login_status(regin *usr,login *l_head)                                          //退出登陆直接清楚登陆节点
{
	login *q = l_head;
	login *p = l_head->next;
	for(q,p;p != l_head;q = p,p = p->next)
	{
		if(strcmp(usr->std.name,p->l_std.name) == 0)         //找到节点
		{
			q->next = p->next;
			p->next->prev = q;
			free(p);
			return 0;
		}
	}
	return -1;
}

int show_all_flight_data(flight *f_head)                                                             //显示所有机票信息
{
	flight *p = f_head->next;
	printf("==========================================================================\n");
	printf("航班号\t出发地\t目的地\t班期\t机型\t起飞时间\t票价\t余票\n");
	for(;p != f_head;p = p ->next)
	{
		printf("%s\t%s\t%s\t%s\t%s\t%s\t%d\t%d\n",p->info.number,
											p->info.staddress,
											p->info.arraddress,
											p->info.date,
											p->info.type,
											p->info.stime,
											p->info.price,
											p->info.poll);
	}
	printf("==========================================================================\n");
	return	0;
}
/*
int targer_find_flight_data(flight * f_head)                                                         //通过目的地查询航班
{
	char adress_buff[10]= {0};
	scanf("%s",adress_buff);
	printf("==========================================================================\n");
	printf("航班号\t出发地\t目的地\t班期\t机型\t起飞时间\t票价\t余票\n");
	floght *p = f_head->next;
	for(;p != f_head,p = p->next)
	{
		if(strcmp(p->arraddress,adress_buff) == 0)
		{
			printf("%s\t%s\t%s\t%s\t%s\t%d\t%d\n",p->info.number,
											p->info.staddress,
											p->info.arraddress,
											p->info.date,
											p->info.type,
											p->info.stime,
											p->info.price,
											p->info.poll);
		}
	}
	printf("==========================================================================\n");
	return	0;
}
*/

int buy_flight_fun(regin *usr,flight *f_head,int fd)               //购买机票
{
	char buy_buff[20];
	show_all_flight_data(f_head);
	printf("Place inter buy number:\n");
	scanf("%s",buy_buff);
	flight *p = f_head->next;
	for(;p != f_head;p = p->next);
	{
		if(strcmp(p->info.number,buy_buff) == 0)
		{
			//开始购票
		}
	}
	printf("NO find number messtion\n");
}

int buy_flight_meun(regin *usr,flight *f_head,int fd)
{
	//显示全部机票信息、条件查询：目的地、班期、价格、起飞时间，购买
	int x,y;
	struct input_event buff;
    while(1)
    {
        show_all_lcd_bmp(bmp_24_4);    //显示用户个人主界面的图片
        bzero(&buff,sizeof(buff));
		read(fd,&buff,sizeof(buff));
		if(buff.type == EV_ABS && buff.code == ABS_X)
		{
			x = buff.value;
		}
        if(buff.type == EV_ABS && buff.code == ABS_Y)
        {
            y = buff.value;
        }
        if(buff.type == EV_KEY && buff.code == BTN_TOUCH && buff.value == 0) 
        {
        	buy_flight_fun(usr,f_head,fd);
        }
	}
}

int per_main_meun(regin *usr,login *l_head,flight *f_head,int fd)                                          //个人主界面
{
	int x,y;
	struct input_event buff;
    while(1)
    {
        show_all_lcd_bmp(bmp_24_4);    //显示用户个人主界面的图片
        bzero(&buff,sizeof(buff));
		read(fd,&buff,sizeof(buff));
		if(buff.type == EV_ABS && buff.code == ABS_X)
		{
			x = buff.value;
		}
        if(buff.type == EV_ABS && buff.code == ABS_Y)
        {
            y = buff.value;
        }
        if(buff.type == EV_KEY && buff.code == BTN_TOUCH && buff.value == 0) //购票接口、个人用户、退出...
        {
        	if(x < 512 && y < 300)
        	{
        		buy_flight_meun(usr,f_head,fd);         //机票数据查询购买
        	}
        	if(x > 512 && y < 300)
        	{
        		printf("person info\n");
        	}
        	if(x < 512 && y > 300)
        	{
        		printf("retrun last option\n");
        		break;
        	}
        	if(x > 512 && y > 300)
        	{
        		printf("logout\n");
        		deleta_login_status(usr,l_head);
        		break;
        	}
        }
    }
}
//================================================登陆
int check_login_status(regin *usr,login *l_head,char *login_name_buff,flight *f_head,int fd)               //检查用户的登陆状态
{
	char login_passwd_buff[20];
	login *check = l_head->next;
	for(;check != l_head;check = check->next)                 //检查是否在登陆链上
	{
		if(strcmp(check->l_std.name,login_name_buff) == 0)      //在登陆链上,免密码登陆
		{
			per_main_meun(usr,l_head,f_head,fd);           //进入用户界面
			return 0;
		}
	}
	printf("请输入密码：");
	scanf("%s",login_passwd_buff);
	if(strcmp(usr->std.passwd,login_passwd_buff) == 0)    
	{
		//密码正确
		login *usr_login = NULL;                       
		usr_login = (login *)malloc(sizeof(login));        
		if(usr_login == NULL)
			perror("malloc usr_login");

		(usr_login->l_std) = (usr->std);     //将用户的数据加载到登陆链上    
		usr_login->next = l_head;
		login *p = l_head->prev;

		p->next = usr_login;
		usr_login->prev = p;
		l_head->prev = usr_login;              //完成链表
		per_main_meun(usr,l_head,f_head,fd);       //进入的个人界面
		return 0;
	}
	else
	{
		printf("密码错误\n");
		return 0;
	}
}

int usr_login_fun(regin *r_head,login *l_head,flight *f_head,int fd)                                          //检查用户的注册状态
{
	char login_name_buff[10];
	printf("请输入用户名:");
	scanf("%s",login_name_buff);
	regin *check_reg = r_head->next;                      //检查是否注册
	for(;check_reg != r_head;check_reg = check_reg->next)
	{
		if(strcmp(check_reg->std.name,login_name_buff) == 0)  //注册过
		{
			check_login_status(check_reg,l_head,login_name_buff,f_head,fd);   
		}
		else
		{
			printf("您还没有注册过\n");
			return -1;
		}
	}
	return 0;
}

int usr_regin_meun(regin *r_head,int fd)                                                                    //用户注册界面
{
	int x,y;
	struct input_event buff;
    while(1)
    {
        show_all_lcd_bmp(bmp_24_3);    //显示用户注册界面
        bzero(&buff,sizeof(buff));
		read(fd,&buff,sizeof(buff));
		if(buff.type == EV_ABS && buff.code == ABS_X)
		{
			x = buff.value;
		}
        if(buff.type == EV_ABS && buff.code == ABS_Y)
        {
            y = buff.value;
        }
        if(buff.type == EV_KEY && buff.code == BTN_TOUCH && buff.value == 0) //用户名、密码、确认密码、电话号码
        {
        	if(y < 300)
        	{
        		regin_new_usr(r_head);          //注册
        	}
        	if( y > 300)
        	{
        		break;                         //返回上一级，取消注册
        	}
        }
    }
    return 0;
}

int usr_login_main_meun(regin *r_head,login *l_head,flight *f_head,int fd)                                   //用户登陆界面
{	
    int x,y;
    struct input_event buff;
    while(1)
    {
        show_all_lcd_bmp(bmp_24_2);    //显示用户登陆系统主界面的图片
        bzero(&buff,sizeof(buff));
		read(fd,&buff,sizeof(buff));
		if(buff.type == EV_ABS && buff.code == ABS_X)
		{
			x = buff.value;
		}
        if(buff.type == EV_ABS && buff.code == ABS_Y)
        {
            y = buff.value;
        }
        if(buff.type == EV_KEY && buff.code == BTN_TOUCH && buff.value == 0)  //需要做登陆、注册、找回密码、返回上一级接口
        {
        	if(y <120)
        	{
        		usr_regin_meun(r_head,fd);
        	}
        	if(y > 120 && y < 240)
        	{
        		usr_login_fun(r_head,l_head,f_head,fd);             //可以字符可以先加入缓冲区直接验证
        	}
        	if(y > 240 && y < 360)
        	{
        		printf("找回密码\n");
        	}
        	if(y > 360)
        	{
        		break;                 //返回上一级
        	}

        }
    }
    return 0;
}

int system_main_meun(regin *r_head,login *l_head,flight *f_head)                                             //购票系统的主界面
{
	int fd;
    struct input_event buff;
    fd = open("/dev/input/event0",O_RDONLY);                  //打开触摸屏
    if(fd == -1)
    	perror("open fd dev input event0");
    int x,y;
    while(1)
    {
        show_all_lcd_bmp(bmp_24_1);    //显示系统主界面的图片
        bzero(&buff,sizeof(buff));
		read(fd,&buff,sizeof(buff));
		if(buff.type == EV_ABS && buff.code == ABS_X)
		{
			x =  buff.value;
		}
        if(buff.type == EV_ABS && buff.code == ABS_Y)
        {
            y = buff.value;
        }
        if(buff.type == EV_KEY && buff.code == BTN_TOUCH && buff.value == 0)
        {
            if(y < 300)                                   
            {
            	usr_login_main_meun(r_head,l_head,f_head,fd);     //用户登陆界面
            }
            if( y >300 && y < 500)
            {
                printf("admin login main meun\n");
            }
            if(y > 500)
            {
            	break;
            }
        }
    }
    close(fd);
    return 0;
}

int main(int argc, char const *argv[])
{
	//1.进入欢迎界面
	wel_interface();

	regin *r_head = NULL;
	r_head = init_regin_list(r_head);      //初始化注册头结点

	login *l_head = NULL;
	l_head = init_login_list(l_head);      //初始化登陆


	flight *f_head = NULL;
	f_head = init_flight_list(f_head);     //初始化飞机数据


	//2.初始化老用户、飞机票数据
	init_old_usr_data(r_head);
	init_flight_data(f_head);

	//2.进入主界面
	system_main_meun(r_head,l_head,f_head);


	return 0;
}