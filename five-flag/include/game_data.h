#ifndef __GAME_DATA_H
#define __GAME_DATA_H

#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_drv_conf.h"
#include "lv_conf.h"
#include "lvgl/src/font/lv_font.h"
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

#define TEST printf("正在执行%s函数第%d行代码\n", __FILE__, __LINE__)
static unsigned int game_buf[15][15] = {0};

void game_control(int x, int y);
void clear();
void Dis_zi(int x, int y, int flag);
int game_over();
void Display(int color, int x, int y);
void *Dis_pan(void *arg);
#endif