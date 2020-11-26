#ifndef _HEAD_H_
#define _HEAD_H_

#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <syslog.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include  <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "kernel_list.h"





// ===========================================server=========================================
// ==================================数据链表===========================
typedef struct client_list
{
    int connfd;            // 用户链接上的套接字
    char client_name[10];  // 链接上服务器的用户名

    // 用户套接字指针域
    struct list_head list;
}client;

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
    char real_name[10];       //真实姓名 
    int type;                 //旅客类型 
};

struct real_info                 //用户真实信息
{
	char real_name[10];           //真实姓名          
	char real_idnum[20];          //真实身份证号码
	unsigned int  real_type;      //身份类型 0：成人 1：儿童 2：军人、残疾人
};

struct flight_data                 //飞机基本信息
{
	char number[10];        //航班号
	char staddress[10];     //起点站
	char arraddress[15];    //终点站
	char date[15];          //班期
	char type[2];           //机型
	char stime[10];         //起飞时间
	unsigned int  price;    //票价
	int poll;	            //余票
};

struct buy_ticket_info           //买票的相关信息
{
	char buy_usr[10];               //买票的人
	char take_usr[10];              //乘车的人
	char f_number[10];              //航班号
	int buy_price;                  //购买的价格
	int ins_flag;                   //是否需要买了保险
};

typedef struct buy_ticket_list        //买票链
{
	struct buy_ticket_info b_info; //买票的信息

	struct buy_ticket_list *prev;   
	struct buy_ticket_list *next;
}buy_ticket;

typedef struct Regin_usr           //注册链
{
	struct usr_data std;          //注册用户的信息
	buy_ticket b_ticket_list;     //用户购票的相关信息

	struct Regin_usr *prev;
	struct Regin_usr *next;        
}regin;

typedef struct login_list          //登陆链
{
	char l_name[10];              //登陆的用户信息

	struct login_list *prev;
	struct login_list *next;
}login;

typedef struct Real_type          //实名认证链
{
	struct real_info r_info;      //身份数据域

	struct Real_type *prev;        //前驱指针
	struct Real_type *next;        //后继指针
}real_id;

typedef struct flight_list         //机票链
{
	struct flight_data info;        //机票信息

	struct flight_list *prev;       //前驱指针
	struct flight_list *next;      //后继指针
}flight;

typedef struct del_flight_list
{
	struct flight_data d_flight;    //删除的航班的号

	struct list_head list;   //删除航班的指针域
}del_flight;

struct msg_handler
{
	client *c_head;      // 链接上的客户头节点
	regin *r_head;       // 注册头节点
	login *l_head;       // 登录头节点
	flight *f_head;      // 航班头节点
	real_id *i_head;     // 实名认证头节点
	del_flight *d_head;  //删除航班的头节点
	int connfd;          // 链接上的客户端的套接字 
};



// ==================================init list ===========



void *msg_handler(void *arg);

client *init_client_list(client *c_head);
regin *init_regin_list(regin *r_head);
login *init_login_list(login *l_head);
flight *init_flight_list(flight *f_head);
real_id *init_read_id_list(real_id *i_head);
del_flight *init_delflight_list(del_flight *d_head);
buy_ticket *init_buy_ticket_list(regin *old_usr,buy_ticket *b_head);
void read_usr_buy_info(regin *r_head,buy_ticket *b_head,char *file_name);
int read_usr_info(regin *r_head,char *file_name);
int init_old_usr_data(regin *r_head);
int init_buy_ticket_date(regin *r_head,buy_ticket *b_head);
void save_usr_buy_flight_data(buy_ticket *new_usr);
void save_flight_date(flight *p);
int read_flight_data_buff(flight *f_head,char *file_name);
int init_flight_data(flight *f_head);
int read_delfly_data_buff(del_flight *d_head,char *file_name);    //文件名;
int init_delflight_date(del_flight *d_head);
int read_id_info(real_id *i_head,char *id_info);
int init_read_id_data(real_id *i_head);

int add_client_tail(client *c_head,int connfd,pthread_mutex_t m);
int del_client_node(client *c_head,int connfd,pthread_mutex_t m);
int save_usr_data_file(regin *usr);
int regin_fun(regin *r_head,char *r_usr_info);
int check_usr_name(regin *r_head,int connfd);
void client_quit(client *c_head,int connfd);
int srv_fly_date_by(flight *f_head,int connfd);
int price_fly_date(flight *f_head,int connfd);
int type_fly_date(flight *f_head,int connfd);
int date_fly_date(flight *f_head,int connfd);
int at_fly_date(flight *f_head,int connfd);
int srv_qf_num(flight *f_head,int connfd);
void snd_flight_usr(int connfd,flight *p);
int usr_quit_login(regin *usr,login *l_head);
int show_all_flight_data(flight *f_head,int connfd);
int change_ticket(regin *usr,struct msg_handler *msg);
void rm_old_ticket(char *take_name,char *old_number);
int add_old_flight(flight *f_head,char *add_number);
int sub_flight_poll(flight *f_head,char *ticket_number);
int check_monoy(regin *usr,flight *f_head,buy_ticket *b,int connfd,int monoy);
int check_monoy_change(regin *usr,int pay);
int check_number_poll(flight *f_head,char *buy_number,int connfd);
int back_ticket(regin *usr,struct msg_handler *msg);
void add_flight_poll(flight *f_head,buy_ticket *p);
int back_monoy(regin *r_head,buy_ticket *p);
int show_all_per_order(regin *usr,flight *f_head,int connfd);
int print_usr_menoy(regin *usr,int connfd);
int recharge(regin *usr,int connfd);
int check_real_id(regin *usr,struct msg_handler *msg);
int buy_flight_fun(regin *usr,struct msg_handler *msg);
int add_buy_ticket_tail(regin *usr,struct msg_handler *msg,flight *p);
int flight_delay_ins(regin *usr,int connfd);
int check_usr_start_date(buy_ticket *b_head,flight *p,flight *f_head,int connfd);
int admin_login(struct msg_handler *msg);
int admin_fun_meun(struct msg_handler *msg);
int show_del_flight_date(struct msg_handler *msg);
int del_flight_info(struct msg_handler *msg);
int check_del_flight(del_flight *d_head,flight *f_head,char *d_number,int connfd);
int add_flight_data(flight *f_head,int connfd);
int append_flight_data(flight *f_head,char *flight_data_buff);
int usr_login_fun(struct msg_handler *msg);
int check_login_status(struct msg_handler *msg,regin *usr,char *l_name_buf);
int server_login_fun(regin *usr,struct msg_handler *msg);


#endif












