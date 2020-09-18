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


struct usr_data               //用户数据
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

typedef struct Regin_usr          //注册链
{
	struct usr_data std;          //注册用户的信息

	struct Regin_usr *prev;
	struct Regin_usr *next;        
}regin;

typedef struct login_list          //登陆链
{
	struct usr_data l_std;         //登陆的用户信息

	struct login_list *prev;
	struct login_list *next;
}login;

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

void wel_interface()                                      //欢迎界面
{
	show_all_lcd_bmp("./image/meun/welcome.bmp");
	sleep(3);
}

regin *init_regin_list(regin *r_head)                           //初始化注册头结点
{
	r_head = (regin *)malloc(sizeof(regin));              //位头节点申请空间
	if(r_head == NULL)
		perror("malloc r_head");

	//双向循环链表初始化头节点，前驱节点和后驱节点都指向头
	r_head->next = r_head;
	r_head->prev = r_head;

	return r_head;	
}

login *init_login_list(login *l_head)                          //初始化登陆的头结点
{
	l_head = (login *)malloc(sizeof(login));
	if(l_head == NULL)
		perror("malloc l_head");

	l_head->prev = l_head;
	l_head->next = l_head;

	return l_head;
}

int save_usr_data_file(regin *new_usr)                                     //保存用户注册的数据到文件
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

int regin_new_usr(regin *r_head)                                   //注册                     
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

int read_usr_info(regin *r_head,char *file_name)                  //读取已注册用户的信息
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

int init_old_usr_data(regin *r_head)                          //初始化注册过的用户
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
//================================================
int check_login_status(regin *check_reg,login *l_head,char *login_name_buff,int fd)               //检查用户的登陆状态
{
	printf("aaaa\n");
	char login_passwd_buff[20];
	login *check = l_head->next;
	for(;check != l_head;check = check->next)                 //检查是否在登陆链上
	{
		if(strcmp(l_head->l_std.name,login_name_buff) == 0)      //在登陆链上
		{
			//per_main_meun(r_head,l_head,fd);           //进入用户界面
			printf("yonghujiemian\n");
			return 0;
		}
	}
	printf("请输入密码：");
	scanf("%s",login_passwd_buff);
	if(strcmp(check_reg->std.passwd,login_passwd_buff) == 0)
	{
		//密码正确
		login *usr_login = NULL;                       
		usr_login = (login *)malloc(sizeof(login));        
		if(usr_login == NULL)
			perror("malloc usr_login");

		(usr_login->l_std) = (check_reg->std);     //将用户的数据加载到登陆链上    
		usr_login->next = l_head;
		login *p = l_head->prev;

		p->next = usr_login;
		usr_login->prev = p;
		l_head->prev = usr_login;              //完成链表
		//per_main_meun(r_head,l_head,fd);       //进入的个人界面
		printf("bbbbbbbbbs\n");
		return 0;
	}
	else
	{
		printf("密码错误\n");
		return 0;
	}
}

int usr_login_fun(regin *r_head,login *l_head,int fd)               //检查用户的注册状态
{
	char login_name_buff[10];
	printf("请输入用户名:");
	scanf("%s",login_name_buff);
	regin *check_reg = r_head->next;                      //检查是否注册
	for(;check_reg != r_head;check_reg = check_reg->next)
	{
		if(strcmp(check_reg->std.name,login_name_buff) == 0)  //注册过
		{
			check_login_status(check_reg,l_head,login_name_buff,fd);   
		}
		else
		{
			printf("您还没有注册过\n");
			return -1;
		}
	}
	return 0;
}

int usr_login_main_meun(regin *r_head,login *l_head,int fd)
{	
	struct input_event buff;
    int x,y;
    while(1)
    {
        show_all_lcd_bmp("./image/meun/test.bmp");    //显示用户登陆系统主界面的图片
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
        	if(x < 512)
        	{
				usr_login_fun(r_head,l_head,fd);         //检查登陆状态
        	}
        	if(x > 512)
        	{
        		break;
        	}
        }
    }
	

}

int system_main_meun(regin *r_head,login *l_head)                                //购票系统的主界面
{
	int fd;
    struct input_event buff;
    fd = open("/dev/input/event0",O_RDONLY);                  //打开触摸屏
    if(fd == -1)
    	perror("open fd dev input event0");
    int x,y;
    while(1)
    {
        show_all_lcd_bmp("./image/meun/mian_meun.bmp");    //显示系统主界面的图片
        bzero(&buff,sizeof(buff));
		read(fd,&buff,sizeof(buff));
        if(buff.type == EV_ABS && buff.code == ABS_Y)
        {
            y = buff.value;
        }
        if(buff.type == EV_KEY && buff.code == BTN_TOUCH && buff.value == 0)
        {
            if(y < 150)                                    //注册
            {
                regin_new_usr(r_head);
            }
            if(y > 150 && y < 300)
            {
                usr_login_main_meun(r_head,l_head,fd);     //用户登陆界面
            }
            if(y > 300 && y < 450)
            {
                break;
            }
            if(y > 450)
            {
                return 0;
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


	//2.初始化老用户
	init_old_usr_data(r_head);

	//2.进入主界面
	system_main_meun(r_head,l_head);


	return 0;
}