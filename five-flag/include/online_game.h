#ifndef __ONLINE_GAME_H
#define __ONLINE_GAME_H

#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_drv_conf.h"
#include "lv_conf.h"
#include "lvgl/src/font/lv_font.h"
#include "lvgl/src/widgets/tabview/lv_tabview.h"
#include "lvgl/examples/lv_examples.h"
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
#include <sys/types.h>   //open
#include <sys/stat.h>    //open
#include <fcntl.h>       //open
#include <linux/input.h> //struct input_event
#include <arpa/inet.h>
#include <sys/socket.h>

typedef struct xy_data
{
    int sock; //对方IP
    int type; //当前用户状态  1：白下 2：黑下
    int x;
    int y;
    int win_go;        //是否有人赢 1为赢
    char message[256]; //消息内容
} XY_DATA_T;

void user_online(XY_DATA_T xy);

#endif