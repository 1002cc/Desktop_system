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
#include <regex.h> //正则表达式 regcomp()、regexec()、regfree()和regerror()。
#define DISP_BUF_SIZE (480 * 1024)
#define TEST printf("正在执行%s文件第%d行代码\n", __FILE__, __LINE__)

#include "Drawing.h"

//信号退出最小化处理
//————————————————————————————————————————————————————————————————————————————————————————————————————————————————
void return_game_cb(lv_event_t *e)
{
    printf("最小化\n");
    union sigval mysend;
    sigqueue(getppid(), SIGRTMIN, mysend);
}

void exit_game_cb(lv_event_t *e)
{
    printf("退出了\n");
    lv_obj_del(lv_obj_get_child(lv_layer_top(), 0));
    lv_obj_del(lv_obj_get_child(lv_layer_top(), 1));
    lv_obj_del(lv_obj_get_child(lv_scr_act(), 0));
    usleep(10000);
    exit(0);
}

void pro_max_dis(int num)
{

    lv_obj_clear_flag(lv_obj_get_child(lv_scr_act(), 0), LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(lv_obj_get_child(lv_layer_top(), 0), LV_OBJ_FLAG_HIDDEN); //两个按键
    lv_obj_clear_flag(lv_obj_get_child(lv_layer_top(), 1), LV_OBJ_FLAG_HIDDEN);
}

void pro_min_dis(int num)
{
    lv_obj_add_flag(lv_obj_get_child(lv_layer_top(), 0), LV_OBJ_FLAG_HIDDEN); //两个按键
    lv_obj_add_flag(lv_obj_get_child(lv_layer_top(), 1), LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lv_obj_get_child(lv_scr_act(), 0), LV_OBJ_FLAG_HIDDEN);
}
//————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void clear_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_user_data(e);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x00ffffff), 0);
}

int main(void)
{
    /*lvgl初始化*/
    lv_init();

    /*输出设备初始化及注册*/
    fbdev_init();
    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];
    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);
    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = fbdev_flush;
    disp_drv.hor_res = 800;
    disp_drv.ver_res = 480;
    lv_disp_drv_register(&disp_drv);

    //输入设备初始化及注册
    evdev_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;
    /*This function will be called periodically (by the library) to get the mouse position and state*/
    indev_drv_1.read_cb = evdev_read;
    lv_indev_drv_register(&indev_drv_1);

    //返回/最小化
    //——————————————————————————————————————————————————————————————————————
    static lv_style_t return_btn_style;

    lv_style_init(&return_btn_style);
    lv_style_set_radius(&return_btn_style, 30);
    lv_style_set_bg_opa(&return_btn_style, LV_OPA_20);
    lv_style_set_shadow_color(&return_btn_style, lv_color_hex(0x002F54EB));

    lv_obj_t *obj_exit = lv_obj_create(lv_layer_top());
    lv_obj_set_size(obj_exit, 90, 40);
    lv_obj_align(obj_exit, LV_ALIGN_TOP_RIGHT, -5, 5);
    lv_obj_clear_flag(obj_exit, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(obj_exit, &return_btn_style, 0);

    lv_obj_t *return_exit = lv_label_create(obj_exit);
    lv_label_set_text(return_exit, LV_SYMBOL_NEW_LINE " Exit");
    lv_obj_center(return_exit);
    lv_obj_add_event_cb(obj_exit, exit_game_cb, LV_EVENT_SHORT_CLICKED, NULL);

    lv_obj_t *obj_mini = lv_obj_create(lv_layer_top());
    lv_obj_set_size(obj_mini, 90, 40);
    lv_obj_align_to(obj_mini, obj_exit, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_clear_flag(obj_mini, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(obj_mini, &return_btn_style, 0);

    lv_obj_t *return_label = lv_label_create(obj_mini);
    lv_label_set_text(return_label, LV_SYMBOL_MINUS " mini");
    lv_obj_center(return_label);
    lv_obj_add_event_cb(obj_mini, return_game_cb, LV_EVENT_SHORT_CLICKED, NULL);
    //——————————————————————————————————————————————————————————————————————

    signal(SIGRTMIN, pro_max_dis);     //最大化
    signal(SIGRTMIN + 1, pro_min_dis); //最小化

    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(800, 480)];
    lv_obj_t *sketchpad = lv_100ask_sketchpad_create(lv_scr_act());

    lv_canvas_set_buffer(sketchpad, cbuf, 800, 480, LV_IMG_CF_TRUE_COLOR);
    lv_obj_center(sketchpad);
    lv_canvas_fill_bg(sketchpad, lv_palette_lighten(LV_PALETTE_GREY, 3), LV_OPA_COVER);

    lv_obj_t *clear = lv_label_create(sketchpad);
    lv_obj_set_size(clear, 30, 30);
    lv_label_set_text(clear, LV_SYMBOL_TRASH);
    lv_obj_align(clear, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_add_flag(clear, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(clear, clear_cb, LV_EVENT_SHORT_CLICKED, sketchpad);

    /*事物处理及告知lvgl节拍数*/
    while (1)
    {
        lv_timer_handler(); //事务处理
        lv_tick_inc(5);     //节拍累计
        usleep(5000);
    }

    return 0;
}

/*用户节拍获取*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if (start_ms == 0)
    {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
