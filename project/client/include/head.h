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

// ===========================================client=========================================
typedef struct snd_regin_info
{
    char name[10];            // 姓名
    char passwd[20];          // 密码
    char tel[15];             // 电话号码
    char Sec_quation[50];     // 密保
    char Sec_answer[20];	  // 密保问题答案

}r_info;


int quick_find(int sockfd);
int  show_all_flight_data(int sockfd);
void welcome_meun();
int show_flyda_bycon(int sockfd);
int f_flyda_by_con(int sockfd);
int buy_ticket(int sockfd);
int c_back_ticket(int sockfd);
int usr_menony(int sockfd);
int check_real(int sockfd);
int check_menoy(int sockfd);
int c_change_ticket(int sockfd);
int per_main_meun(int sockfd);
int  regin_usr_fun(int sockfd);
int login_usr_fun(int sockfd);
int c_add_flight_date(int sockfd);
int  del_flight_date(int sockfd);
int show_del_fly(int sockfd);
int admin_login(int sockfd);


#endif












