#ifndef __DRAWING_H
#define __DRAWING_H
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

typedef enum
{
    LV_100ASK_SKETCHPAD_TOOLBAR_OPT_ALL = 0,
    LV_100ASK_SKETCHPAD_TOOLBAR_OPT_CW,
    LV_100ASK_SKETCHPAD_TOOLBAR_OPT_WIDTH,
    LV_100ASK_SKETCHPAD_TOOLBAR_OPT_LAST,
    LV_100ASK_SKETCHPAD_TOOLBAR_OPT_CLEAR
} lv_100ask_sketchpad_toolbar_t;

/*Data of canvas*/
typedef struct
{
    lv_img_t img;
    lv_img_dsc_t dsc;
    lv_draw_line_dsc_t line_rect_dsc;
} lv_100ask_sketchpad_t;

lv_obj_t *lv_100ask_sketchpad_create(lv_obj_t *parent);
#endif