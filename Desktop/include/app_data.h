#ifndef __APP_DATA_H
#define __APP_DATA_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "lvgl/lvgl.h"

typedef struct prosess
{
    char icon_name[64];    //图标显示名字
    char process_name[32]; //程序名

    lv_obj_t *obj_icon;     //软件图标面板
    lv_obj_t *obj_label;    //软件名面板
    lv_obj_t *bar_obj_icon; //状态栏软件面板
    int program_flag;       //记录程序是否运行 2为后台运行 1为正在运行 0为未运行 默认为0
    pid_t program_pid;      //记录运行进程pid

} PRO_T;

#define DATATYPE PRO_T

#include "list.h"

#define APP_Backstage 2 //后台运行
#define APP_RUN 1
#define APP_CLOSE 0

void File_Data_Init(listnode *head);
void fork_newpid(listnode *p);
#endif