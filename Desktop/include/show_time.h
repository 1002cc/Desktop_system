#ifndef SHOW_TIME_H
#define SHOW_TIME_H

#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_drv_conf.h"
#include "lv_conf.h"
#include "lvgl/src/font/lv_symbol_def.h"
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
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "lvgl/src/font/lv_font.h"
#include <regex.h> //正则表达式 regcomp()、regexec()、regfree()和regerror()

typedef struct weather
{
    char name_bin[32];
    char text[12];
} WEATHER_T;

#define DATATYPE_W WEATHER_T
#include "list_weather.h"

void up_dis_event_cb(lv_event_t *e);
int show_fb();
int get_time_updata(int client_fd);
void lcd_top_widgets(lv_obj_t *parent);
void run_shell_cmd();
void lcd_brightness_init(lv_obj_t *obj);
// void wallpaper_event_cb(lv_event_t *e);
#endif
