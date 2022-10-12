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
#include "my_dict.h"
#include <regex.h> //正则表达式 regcomp()、regexec()、regfree()和regerror()。
#include "cJSON.h"
#define FIFO_PIPE "../Desktop/weather_pipe"
#define DISP_BUF_SIZE (480 * 1024)
#define TEST printf("正在执行%s文件第%d行代码\n", __FILE__, __LINE__)
#define IP_ADDR "116.62.81.138"
#define Port_Number 80

static lv_style_t font_style;
static lv_obj_t *main_show_obj;
static lv_obj_t *text_search;
static lv_obj_t *kb;

char temp[2048];
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
    lv_obj_del(lv_obj_get_child(lv_layer_top(), 0));
    lv_obj_del(lv_obj_get_child(lv_layer_top(), 1));
    for (int i = 0; i < lv_obj_get_child_cnt(lv_scr_act()); i++)
    {
        lv_obj_del(lv_obj_get_child(lv_scr_act(), i));
    }
    system("kill -9 mplayer");
    printf("退出了\n");
    exit(0);
}

void pro_max_dis(int num)
{

    lv_obj_clear_flag(lv_obj_get_child(lv_layer_top(), 0), LV_OBJ_FLAG_HIDDEN); //两个按键
    lv_obj_clear_flag(lv_obj_get_child(lv_layer_top(), 1), LV_OBJ_FLAG_HIDDEN);
    for (int i = 0; i < lv_obj_get_child_cnt(lv_scr_act()); i++)
    {
        lv_obj_clear_flag(lv_obj_get_child(lv_scr_act(), i), LV_OBJ_FLAG_HIDDEN);
    }
}

void pro_min_dis(int num)
{
    lv_obj_add_flag(lv_obj_get_child(lv_layer_top(), 0), LV_OBJ_FLAG_HIDDEN); //两个按键
    lv_obj_add_flag(lv_obj_get_child(lv_layer_top(), 1), LV_OBJ_FLAG_HIDDEN);
    for (int i = 0; i < lv_obj_get_child_cnt(lv_scr_act()); i++)
    {
        lv_obj_add_flag(lv_obj_get_child(lv_scr_act(), i), LV_OBJ_FLAG_HIDDEN);
    }
}
//————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void send_weather(char *weather_text)
{
    if (access(FIFO_PIPE, F_OK) != 0)
    {
        umask(0000);
        mkfifo(FIFO_PIPE, 0777);
    }
    int pipe_fd = open(FIFO_PIPE, O_RDWR | O_TRUNC);
    write(pipe_fd, weather_text, strlen(weather_text) + 1);
    printf("%s\n", weather_text);
    kill(getppid(), SIGRTMIN + 5);
    usleep(1000);
    close(pipe_fd);
}

void *seed_news(void *arg)
{
    int client_fd = (int)arg;
    char get_duf[4000] = {0};
    char http[64] = {0};
    char reg_data[32] = {0};
    char reg_date[32] = {0};
    char reg_local[32] = {0};
    char reg_text[32] = {0};
    char temp[32] = {0};
    char msg[64] = {0};
    stpcpy(msg, lv_textarea_get_text(text_search));
    if (strlen(msg) == 0)
        pthread_exit(NULL);
    printf("%s\n", msg);
    // https://api.seniverse.com/v3/weather/now.json?key=Srhi2-y2LtemJAmOB&location=beijing&language=zh-Hans&unit=c
    sprintf(http, "GET /v3/weather/now.json?key=Srhi2-y2LtemJAmOB&location=%s&language=zh-Hans&unit=c HTTP/1.1\r\nHost:api.seniverse.com\r\n\r\n", msg);
    printf("%s\n", http);
    write(client_fd, http, strlen(http));
    read(client_fd, get_duf, sizeof(get_duf));
    memset(get_duf, 0, sizeof(get_duf));
    read(client_fd, get_duf, sizeof(get_duf));
    printf("%s\n", get_duf);
    if (strstr(get_duf, "中国"))
    {
        cJSON *json;
        json = cJSON_Parse(get_duf);
        cJSON *arrayItem = cJSON_GetObjectItem(json, "results");
        cJSON *object = cJSON_GetArrayItem(arrayItem, 0);
        cJSON *item = cJSON_GetObjectItem(object, "location");
        cJSON *item_now = cJSON_GetObjectItem(object, "now");
        cJSON *item1 = cJSON_GetObjectItem(item, "name");
        strcpy(reg_local, item1->valuestring);
        item1 = cJSON_GetObjectItem(item_now, "text");
        strcpy(reg_text, item1->valuestring);
        item1 = cJSON_GetObjectItem(item_now, "temperature");
        strcpy(temp, item1->valuestring);
        send_weather(reg_text);
        //正则获取时间
        int status, i;
        int cflags = REG_EXTENDED;
        regmatch_t pmatch[1];
        const size_t nmatch = 1;
        regex_t reg;
        //处理时间
        const char *pattern_date = "([0-9]{3}[1-9]|[0-9]{2}[1-9][0-9]{1}|[0-9]{1}[1-9][0-9]{2}|[1-9][0-9]{3})-(((0[13578]|1[02])-(0[1-9]|[12][0-9]|3[01]))|((0[469]|11)-(0[1-9]|[12][0-9]|30))|(02-(0[1-9]|[1][0-9]|2[0-8])))"; //只显示文字
        regcomp(&reg, pattern_date, cflags);                                                                                                                                                                                    //编译正则模式
        status = regexec(&reg, get_duf, nmatch, pmatch, 0);                                                                                                                                                                     //执行正则表达式和缓存的比较
        if (status == REG_NOMATCH)
            printf("No match\n");
        else if (0 == status)
        {
            if (pmatch[i].rm_so != -1 && pmatch[i].rm_eo != -1)
            {
                int len = pmatch[i].rm_eo - pmatch[i].rm_so;
                strncpy(reg_date, get_duf + pmatch[i].rm_so, len);
                reg_date[len] = '\0';
                printf("%s\n", reg_date);
            }
        }
        sprintf(reg_data, " %s   %s   温度:%s°C   天气:%s", reg_date, reg_local, temp, reg_text);
        regfree(&reg);
        printf("处理完成\n");
        lv_label_set_text(main_show_obj, reg_data);
        strcpy(temp, reg_data);
        printf("%s\n", temp);
        printf("%s\n", reg_data);
        pthread_exit(NULL);
    }
    else
    {
        printf("获取失败\n");
        lv_label_set_text(main_show_obj, "获取失败");
        pthread_exit(NULL);
    }
}

void show_weather(lv_event_t *e)
{
    TEST;
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
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
        strcpy(tmp_show, "连接失败");
        lv_label_set_text(main_show_obj, tmp_show);
        return;
    }
    printf("连接成功\n");
    TEST;
    pthread_t thread;
    pthread_create(&thread, NULL, seed_news, (void *)client_fd);
    pthread_join(thread, NULL);
    TEST;
    // lv_label_set_text(main_show_obj, temp);
}

static void ta_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *ta = lv_event_get_target(e);
    lv_obj_t *kb = lv_event_get_user_data(e);

    if (code == LV_EVENT_FOCUSED)
    {
        if (lv_indev_get_type(lv_indev_get_act()) != LV_INDEV_TYPE_KEYPAD)
        {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
        }
    }
    else if (code == LV_EVENT_CANCEL)
    {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ta);
    }
    else if (code == LV_EVENT_READY)
    {
        // lv_keyboard_set_textarea(kb, ta);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

/*
=============================================
首界面

=============================================
*/
void first_interface(void)
{

    static lv_style_t main_obj_style;
    lv_style_init(&main_obj_style);
    lv_style_set_bg_color(&main_obj_style, lv_color_hex(0x00c2e9fb));
    lv_style_set_radius(&main_obj_style, 0);
    lv_style_set_border_width(&main_obj_style, 0);
    lv_style_set_border_opa(&main_obj_style, LV_OPA_0);

    static lv_ft_info_t info;
    info.name = "/font/GenSenRounded-R.ttc";
    info.weight = 20;
    info.style = 0;
    info.mem = NULL;

    if (!lv_ft_font_init(&info))
    {
        LV_LOG_ERROR("Failed to initialize font");
    }

    lv_style_init(&font_style);
    lv_style_set_text_font(&font_style, info.font);
    lv_style_set_text_align(&font_style, LV_ALIGN_CENTER);
    lv_style_set_text_color(&font_style, lv_color_hex(0x00ffffff));

    static lv_style_t main_show_text_style;
    lv_style_init(&main_show_text_style);
    lv_style_set_bg_color(&main_show_text_style, lv_color_hex(0x00a1c4fd));
    lv_style_set_radius(&main_show_text_style, 20);

    lv_obj_t *main_obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(main_obj, 800, 480);
    lv_obj_add_style(main_obj, &main_obj_style, 0);
    lv_obj_center(main_obj);

    lv_obj_t *main_show_text = lv_obj_create(main_obj);
    lv_obj_set_size(main_show_text, 760, 350);
    lv_obj_add_style(main_show_text, &main_show_text_style, 0);
    lv_obj_align(main_show_text, LV_ALIGN_BOTTOM_MID, 0, 0);

    main_show_obj = lv_label_create(main_show_text);
    lv_obj_set_style_text_font(main_show_obj, info.font, 0);
    lv_label_set_text(main_show_obj, " ");

    kb = lv_keyboard_create(main_obj);

    lv_obj_t *pinyin_kb = lv_ime_pinyin_create(main_obj);
    lv_obj_align_to(pinyin_kb, kb, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_obj_set_style_text_font(pinyin_kb, info.font, 0); //添加自己的字库
    lv_ime_pinyin_set_dict(pinyin_kb, my_dict);          //添加字库

    text_search = lv_textarea_create(main_obj); //搜索框
    lv_textarea_set_one_line(text_search, true);
    lv_textarea_add_text(text_search, "广州");
    lv_obj_set_style_text_font(text_search, info.font, 0); //可接收中文
    lv_obj_set_size(text_search, 600, 60);
    lv_obj_align(text_search, LV_ALIGN_TOP_MID, 0, 25);

    lv_ime_pinyin_set_keyboard(pinyin_kb, kb);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(text_search, ta_event_cb, LV_EVENT_ALL, kb);

    lv_obj_t *search_btn = lv_btn_create(main_obj);
    lv_obj_set_size(search_btn, 50, 50);
    lv_obj_set_style_radius(search_btn, 100, 0);

    lv_obj_t *search_label = lv_label_create(search_btn);
    lv_label_set_text(search_label, LV_SYMBOL_GPS);
    lv_obj_center(search_label);
    lv_obj_add_event_cb(search_btn, show_weather, LV_EVENT_SHORT_CLICKED, NULL);
    lv_obj_align_to(search_btn, text_search, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
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
    static lv_style_t btn_style;

    lv_style_init(&btn_style);
    lv_style_set_radius(&btn_style, 30);
    lv_style_set_bg_opa(&btn_style, LV_OPA_20);
    lv_style_set_shadow_color(&btn_style, lv_color_hex(0x002F54EB));

    lv_obj_t *obj_exit = lv_obj_create(lv_layer_top());
    lv_obj_set_size(obj_exit, 90, 40);
    lv_obj_align(obj_exit, LV_ALIGN_TOP_RIGHT, -5, 5);
    lv_obj_clear_flag(obj_exit, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(obj_exit, &btn_style, 0);

    lv_obj_t *return_exit = lv_label_create(obj_exit);
    lv_label_set_text(return_exit, LV_SYMBOL_NEW_LINE " Exit");
    lv_obj_center(return_exit);
    lv_obj_add_event_cb(obj_exit, exit_game_cb, LV_EVENT_SHORT_CLICKED, NULL);

    lv_obj_t *obj_mini = lv_obj_create(lv_layer_top());
    lv_obj_set_size(obj_mini, 90, 40);
    lv_obj_align_to(obj_mini, obj_exit, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_clear_flag(obj_mini, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(obj_mini, &btn_style, 0);

    lv_obj_t *return_label = lv_label_create(obj_mini);
    lv_label_set_text(return_label, LV_SYMBOL_MINUS " mini");
    lv_obj_center(return_label);
    lv_obj_add_event_cb(obj_mini, return_game_cb, LV_EVENT_SHORT_CLICKED, NULL);
    //——————————————————————————————————————————————————————————————————————

    signal(SIGRTMIN, pro_max_dis);     //最大化
    signal(SIGRTMIN + 1, pro_min_dis); //最小化

    first_interface();

    pthread_mutex_t lvgl_mutex;
    pthread_mutex_init(&lvgl_mutex, NULL);
    /*事物处理及告知lvgl节拍数*/
    while (1)
    {
        pthread_mutex_lock(&lvgl_mutex);
        lv_timer_handler(); //事务处理
        pthread_mutex_unlock(&lvgl_mutex);
        lv_tick_inc(5); //节拍累计
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
