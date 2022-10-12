#ifndef __FCHAT_APP_DATA_H
#define __FCHAT_APP_DATA_H

#include "lvgl/examples/lv_examples.h"
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <arpa/inet.h> //tcp网络信息结构体  struct sockaddr_in


#define IP_ADDR "192.168.61.51"
#define Port_Number 2022

typedef struct user_data
{
    unsigned int *user_ip;

};

void Fchat_Init();

#endif
