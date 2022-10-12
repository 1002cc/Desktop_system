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
#define IP_ADDR "36.27.212.84"
#define Port_Number 80

static lv_style_t font_style;
static lv_obj_t *main_show_obj;
static lv_obj_t *text_search;

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
    lv_obj_del(lv_obj_get_child(lv_scr_act(), 0));
    lv_obj_del(lv_obj_get_child(lv_scr_act(), 1));
    usleep(10000);
    exit(0);
}

void pro_max_dis(int num)
{
    if (lv_obj_has_flag(lv_scr_act(), LV_OBJ_FLAG_HIDDEN) == true)
    {
        lv_obj_clear_flag(lv_obj_get_child(lv_scr_act(), 0), LV_OBJ_FLAG_HIDDEN); //两个按键
        lv_obj_clear_flag(lv_obj_get_child(lv_scr_act(), 1), LV_OBJ_FLAG_HIDDEN);
    }
    lv_obj_move_foreground(lv_obj_get_child(lv_scr_act(), 0));
    lv_obj_move_foreground(lv_obj_get_child(lv_scr_act(), 1));
}

void pro_min_dis(int num)
{
    if (lv_obj_has_flag(lv_scr_act(), LV_OBJ_FLAG_HIDDEN) == false)
    {
        lv_obj_add_flag(lv_obj_get_child(lv_scr_act(), 0), LV_OBJ_FLAG_HIDDEN); //两个按键
        lv_obj_add_flag(lv_obj_get_child(lv_scr_act(), 1), LV_OBJ_FLAG_HIDDEN);
    }
}
//————————————————————————————————————————————————————————————————————————————————————————————————————————————————

int get_code(char *head)
{
    //解析数据报
    char *p = head;
    while (*p != ' ')
    {
        p++;
    }

    //状态码
    char s_code[5] = {0};
    strncpy(s_code, p, 4);
    printf("%s\n", s_code);

    //转换为整形数据
    int code = atoi(s_code);

    return code;
}

char *get_video(char *head)
{

    char *p = strstr(head, "Location");

    static char http[4096] = {0};
    bzero(http, 4096);

    //指向链接的首元素
    while (*p != '/')
    {
        p++;
    }
    int i = 0;
    //获取链接
    while (*p != '\r' && *(p + 1) != '\n')
    {
        http[i] = *p;
        i++;
        p++;
    }

    http[i] = '\0';

    return http;
}

void *show_video(void *data)
{
    //申请一个套接字文件
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        perror("socket");
        return;
    }
    struct sockaddr_in server_info;
    server_info.sin_family = AF_INET;
    server_info.sin_port = htons(Port_Number); // 1023~65535
    server_info.sin_addr.s_addr = inet_addr(IP_ADDR);
    char tmp_show[64];
    if (connect(client_fd, &server_info, sizeof(server_info)) != 0)
    {
        perror("bind");
        return;
    }
    printf("连接成功\n");

    char *http = "GET /api-girl/index.php?wpon=302 HTTP/1.1\r\nHost:tucdn.wpon.cn\r\n\r\n";
    write(client_fd, http, strlen(http));

    char head[4096] = {0};
    read(client_fd, head, 4096);
    close(client_fd);

    int code = get_code(head);
    if (code == 302)
    {
        //提取视频链接
        char *http = get_video(head);

        printf("http=%s\n", http);

        //拼接播放命令
        char cmd[4096] = {0};
        sprintf(cmd, "../mplayer -loop 1 -vf rotate=1 -aspect 4:3 http:%s", http);

        printf("cmd=%s\n", cmd);
        //开始播放
        system(cmd);
    }
}

void video_next(lv_event_t *e)
{
    pthread_t id = (pthread_t *)e->user_data;
    pthread_detach(id);
    system("killall -9 mplayer");
    pthread_cancel(id);
    sleep(1);
    pthread_create(&id, NULL, show_video, NULL);
}

/*
=============================================
首界面

=============================================
*/
void first_interface(void)
{

    static lv_ft_info_t info;
    info.name = "/font/GenSenRounded-R.ttc";
    info.weight = 12;
    info.style = 0;
    info.mem = NULL;

    if (!lv_ft_font_init(&info))
    {
        LV_LOG_ERROR("Failed to initialize font");
    }
    pthread_t thread;
    lv_style_init(&font_style);
    lv_style_set_text_font(&font_style, info.font);
    lv_style_set_text_align(&font_style, LV_ALIGN_CENTER);
    lv_style_set_text_color(&font_style, lv_color_hex(0x00ffffff));

    /* lv_obj_t *main_display = lv_obj_create(lv_scr_act());
    lv_obj_set_size(main_display, 800, 480);
    lv_obj_center(main_display); */

    lv_obj_t *btn = lv_obj_create(lv_layer_top());
    lv_obj_set_size(btn, 100, 50);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -5, -50);

    lv_obj_t *label = lv_label_create(btn);
    lv_obj_add_style(label, &font_style, 0);
    lv_label_set_text(label, LV_SYMBOL_NEXT " 下一个");
    lv_obj_center(label);
    ((void *(*)(void *))show_video)(NULL);

    lv_obj_t *btn_stop = lv_obj_create(lv_layer_top());
    lv_obj_set_size(btn_stop, 100, 50);
    lv_obj_clear_flag(btn_stop, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align_to(btn_stop, btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    lv_obj_t *label_stop = lv_obj_create(btn_stop);
    lv_obj_add_style(label_stop, &font_style, 0);
    lv_label_set_text(label_stop, LV_SYMBOL_STOP " 暂停");
    lv_obj_center(label_stop);
    ((void *(*)(void *))show_video)(NULL);

    lv_obj_add_event_cb(btn, video_next, LV_EVENT_SHORT_CLICKED, thread);
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

    signal(SIGRTMIN + 1, pro_max_dis); //显示
    signal(SIGRTMIN, pro_min_dis);     //最小化

    first_interface();

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
