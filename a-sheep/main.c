#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_drv_conf.h"
#include "lv_conf.h"
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
#include <sys/types.h> //open
#include <sys/stat.h>  //open
#include <fcntl.h>     //open

#define TEST printf("正在执行%s函数第%d行代码\n", __FILE__, __LINE__)

pthread_mutex_t lvgl_mutex;

#define PIPE_FLFO "/tmp/pipe_flfo"
typedef struct brand_node
{
    char brand_icon[32]; //牌的图标
    int brand_flag;      //牌是否选中
    // int brand_layer;     //牌所在的层
    lv_obj_t *obj;
} brand_data;

#define DATATYPE brand_data

#include "list.h"

#define DISP_BUF_SIZE (480 * 1024)
#define RAND(min, max) (rand() % ((int)(max + 1) - (int)(min)) + (int)(min)) //随机
int brand_sum = 54;
int layer_num;
listnode *head_bar;
lv_obj_t *brand_show;
//宏定义牌图标的路径
#define plane "S:/Desktop/A-sheep/icon/plane.png"
#define rocket "S:/Desktop/A-sheep/icon/rocket.png"
#define play "S:/Desktop/A-sheep/icon/play.png"
#define watermelon "S:/Desktop/A-sheep/icon/watermelon.png"
#define drinks "S:/Desktop/A-sheep/icon/drinks.png"
#define windmill "S:/Desktop/A-sheep/icon/windmill.png"
#define iec "S:/Desktop/A-sheep/icon/iec.png"

void rang_game_init(listnode *head)
{
    char icon_bin[7][64] = {plane, rocket, play, watermelon, drinks, windmill, iec};
    brand_data data;
    TEST;
    srand((unsigned)time(NULL));
    // layer_num = RAND(1, 5);
    // int brang_num = 0;
    // brand_sum = 0;
    for (int i = 0; i < brand_sum; i++)
    {
        /* brang_num = RAND(7, 12);
        brand_sum += brang_num;
        for (int j = 0; j < brang_num; j++)
        { */
        // data.brand_layer = i;
        memset(&data, 0, sizeof(data));
        data.brand_flag = 0;
        strcpy(data.brand_icon, icon_bin[RAND(0, 6)]);
        list_head_tail_insert(head, data, 1);
        /*  } */
    }
    //判断牌的数量是否为3的倍数
    /*  if (brand_sum % 3 != 0)
     {
         for (int i = 0; i < 3; i++)
         {
             brand_sum++;
             data.brand_layer = layer_num - 1;
             data.brand_flag = 0;
             strcpy(data.brand_icon, icon_bin[RAND(0, 6)]);
             list_head_tail_insert(head, data, 1);
             if (brand_sum % 3 == 0)
                 break;
         }
     } */
}

bool isname(datatype arg, datatype data)
{
    if (strcmp(arg.brand_icon, data.brand_icon) == 0)
        return true;
    else
        return false;
}

void *game_rule(void *arg)
{
    int flag_num = 0;
    listnode *find_node;
    while (1)
    {
        listnode *p = head_bar->next;
        while (p != head_bar)
        {
            if ((find_node = list_findnode(head_bar, isname, p->data)) != NULL)
            {
                flag_num++;
                if (flag_num >= 3)
                {
                    while (p != head_bar)
                    {
                        if (strcmp(p->data.brand_icon, find_node->data.brand_icon) == 0)
                        {
                            lv_obj_del(p->data.obj);
                            __NodeDel(p->next, p->prev);
                            free(p);
                        }
                        p = p->next;
                    }
                }
            }
            p = p->next;
        }
    }
}

/*
     全局样式定义
*/
static lv_style_t bar_choose;
static lv_style_t brand_style;
static lv_style_t brand_show_style;
static lv_style_t frist_btn_style;
static lv_style_t return_btn_style;
static lv_style_t style;
static lv_style_t bar_clear_style;
void style_init()
{
    lv_style_init(&bar_choose);
    lv_style_set_radius(&bar_choose, 5);
    lv_style_set_bg_color(&bar_choose, lv_color_hex(0x006481FA));
    lv_style_set_outline_width(&bar_choose, 2);                        //外边框厚度
    lv_style_set_outline_color(&bar_choose, lv_color_hex(0x00ECF3EB)); //外边框颜色
    lv_style_set_outline_pad(&bar_choose, 0);                          //外边框距离
    lv_style_set_border_color(&bar_choose, lv_color_hex(0x000000FF));
    lv_style_set_border_width(&bar_choose, 5);
    lv_style_set_border_opa(&bar_choose, LV_OPA_20);

    lv_style_init(&brand_style);
    lv_style_set_radius(&brand_style, 5);
    lv_style_set_bg_color(&brand_style, lv_color_hex(0x002F54EB));
    lv_style_set_border_color(&brand_style, lv_color_hex(0x002F54EB));
    lv_style_set_border_width(&brand_style, 2);
    lv_style_set_border_opa(&brand_style, LV_OPA_20);

    lv_style_init(&brand_show_style);
    lv_style_set_bg_opa(&brand_show_style, LV_OPA_TRANSP);

    lv_style_init(&frist_btn_style);
    lv_style_set_radius(&frist_btn_style, 30);
    lv_style_set_bg_color(&frist_btn_style, lv_color_hex(0x00BC9BE0));

    lv_style_init(&return_btn_style);
    lv_style_set_radius(&return_btn_style, 30);
    lv_style_set_bg_color(&return_btn_style, lv_color_hex(0x002F54EB));

    static lv_ft_info_t info;
    info.name = "/font/GenSenRounded-R.ttc";
    info.weight = 15;
    info.style = 0;
    info.mem = NULL;

    if (!lv_ft_font_init(&info))
    {
        LV_LOG_ERROR("Failed to initialize font");
    }
    lv_style_init(&style);
    lv_style_set_text_font(&style, info.font);
    lv_style_set_text_align(&style, LV_ALIGN_CENTER);
    lv_style_set_text_color(&style, lv_color_hex(0x00ffffff)); //字体颜色

    lv_style_init(&bar_clear_style); //添加—清理图标样式
    lv_style_set_radius(&bar_clear_style, 30);
    lv_style_set_bg_color(&bar_clear_style, lv_color_hex(0x009BC8F1));
    lv_style_set_bg_opa(&bar_clear_style, LV_OPA_70);
    lv_style_set_border_width(&bar_clear_style, 0);
    lv_style_set_border_color(&bar_clear_style, lv_color_hex(0x00ffffff));
    lv_style_set_border_opa(&bar_clear_style, LV_OPA_70);
}

void brand_show_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);       //获得事件最初瞄准的对象。即使事件是冒泡的，也是一样的。
    lv_obj_t *cont = lv_event_get_current_target(e); //获取事件的当前目标。它是被调用事件处理程序的对象。

    listnode *p = (listnode *)e->user_data;
    printf("%s\n", p->data.brand_icon);
    p->data.brand_flag = 1;
    list_head_tail_insert(head_bar, p->data, 1);
    __NodeDel(p->next, p->prev);
    free(p);
    brand_sum--;
    if (target == cont)
        return;
    //判断最初瞄准对象和当前获取的对象是否一致
    /*  lv_obj_t *brand_btn = lv_btn_create(game_bar_obj); //创建对象
     brand_btn = target ;*/

    lv_obj_del(target);
    listnode *p_bar = head_bar->next;
    printf("%s\n", p_bar->data.brand_icon);
    while (p_bar != head_bar)
    {
        if (p_bar->data.brand_flag == 1)
        {
            p_bar->data.obj = lv_btn_create(lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 1), 1)); //创建对象
            lv_obj_set_size(p_bar->data.obj, 40, 50);                                                //设置对象尺寸
            lv_obj_add_style(p_bar->data.obj, &brand_style, 0);                                      //设置样式
            lv_obj_add_flag(p_bar->data.obj, LV_OBJ_FLAG_EVENT_BUBBLE);                              //添加对象标志位 对象标志事件气泡

            lv_obj_t *brand_img = lv_img_create(p_bar->data.obj); //创建Label
            lv_img_set_src(brand_img, p_bar->data.brand_icon);    //设置牌的图片
            lv_obj_center(brand_img);                             //居中对象
        }
        p_bar = p_bar->next;
    }
}
/*
   层数：随机
   每层的牌数随机
   一共54张牌
   */
void display_brand_icon(listnode *head)
{
    /*  for (int i = 0; i < layer_num; i++)
     { */
    listnode *p = head->next;
    while (p != head)
    {
        /* if (p->data.brand_layer == i)
        { */
        lv_obj_t *brand_btn = lv_btn_create(brand_show);      //创建对象
        lv_obj_set_size(brand_btn, 40, 50);                   //设置对象尺寸
        lv_obj_add_style(brand_btn, &brand_style, 0);         //设置样式
        lv_obj_add_flag(brand_btn, LV_OBJ_FLAG_EVENT_BUBBLE); //添加对象标志位 对象标志事件气泡

        lv_obj_t *brand_img = lv_img_create(brand_btn); //创建Label
        lv_img_set_src(brand_img, p->data.brand_icon);  //设置牌的图片
        lv_obj_center(brand_img);                       //居中对象
                                                        /*   } */
        p = p->next;
    }
    /*   } */
    lv_obj_add_event_cb(brand_show, brand_show_cb, LV_EVENT_CLICKED, p); //添加事件回调函数
}

void again_refresh_cb(lv_event_t *e)
{
    listnode *head = (listnode *)e->user_data;
    rang_game_init(head);
    display_brand_icon(head);
    lv_task_handler();
}

/*
牌的大小：40*50
显示区： 8*7
         牌的显示 400*400 选择栏 50*400
        * * * * * * * *     -
        * * * * * * * *     -
        * * * * * * * *     -
        * * * * * * * *     -
        * * * * * * * *     -
        * * * * * * * *     -
        * * * * * * * *     -

选择栏容量：7张牌  40*7 牌与牌的间距：5

*/
void game_start_cb(lv_event_t *e)
{

    listnode *head = (listnode *)e->user_data;
    lv_obj_add_flag((lv_obj_t *)e->current_target, LV_OBJ_FLAG_HIDDEN);
    lv_obj_t *game_start_obj = lv_obj_create(lv_scr_act());
    lv_obj_clear_flag(game_start_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(game_start_obj, 800, 480);
    lv_obj_center(game_start_obj);

    lv_obj_t *game_start_img = lv_img_create(game_start_obj);
    lv_obj_clear_flag(game_start_img, LV_OBJ_FLAG_SCROLLABLE);
    lv_img_set_src(game_start_img, "S:/Desktop/A-sheep/game_start.png");
    lv_obj_center(game_start_img);

    //选择栏
    lv_obj_t *game_bar_obj = lv_obj_create(game_start_obj);
    lv_obj_set_size(game_bar_obj, 80, 400);
    lv_obj_add_style(game_bar_obj, &bar_choose, 0);
    lv_obj_align(game_bar_obj, LV_ALIGN_RIGHT_MID, -30, 0);

    lv_obj_set_flex_flow(game_bar_obj, LV_FLEX_FLOW_ROW_WRAP); //让图标等比例排列

    //显示牌
    brand_show = lv_obj_create(game_start_obj);
    lv_obj_set_size(brand_show, 400, 400);
    lv_obj_align(brand_show, LV_ALIGN_CENTER, -30, 0);
    lv_obj_add_style(brand_show, &brand_show_style, 0);
    lv_obj_clear_flag(brand_show, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(brand_show, LV_FLEX_FLOW_ROW_WRAP); //让图标等比例排列
    lv_obj_set_style_pad_row(brand_show, 5, 0);
    lv_obj_set_style_pad_column(brand_show, 5, 0);

    //重新刷新
    lv_obj_t *again_refresh_btn = lv_obj_create(game_start_obj);
    lv_obj_set_size(again_refresh_btn, 60, 60);
    lv_obj_add_style(again_refresh_btn, &bar_clear_style, 0);
    lv_obj_align(again_refresh_btn, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_add_event_cb(again_refresh_btn, again_refresh_cb, LV_EVENT_SHORT_CLICKED, head);

    lv_obj_t *img_refresh = lv_label_create(again_refresh_btn);
    lv_label_set_text(img_refresh, LV_SYMBOL_REFRESH);
    lv_obj_center(img_refresh);

    //绘制牌
    display_brand_icon(head);

    pthread_t thread_one;
    pthread_create(&thread_one, NULL, game_rule, NULL);
}

/*************
*   羊了个羊
每个数据对应一张牌
通过LVGL绘制页面

**************/
void game_frist(listnode *head)
{
    lv_obj_t *game_obj = lv_obj_create(lv_scr_act());
    lv_obj_clear_flag(game_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(game_obj, 800, 480);
    lv_obj_center(game_obj);

    lv_obj_t *game_img = lv_img_create(game_obj);
    lv_img_set_src(game_img, "S:/Desktop/A-sheep/game_frist.png");
    lv_obj_center(game_img);

    lv_obj_t *game_start_btn = lv_btn_create(game_obj);

    lv_obj_set_size(game_start_btn, 170, 60);
    lv_obj_align(game_start_btn, LV_ALIGN_CENTER, 0, 30);
    lv_obj_add_style(game_start_btn, &frist_btn_style, 0);

    lv_obj_t *game_start_label = lv_label_create(game_start_btn);
    lv_obj_add_style(game_start_label, &style, 0);
    lv_label_set_text(game_start_label, "开始游戏");
    lv_obj_center(game_start_label);

    lv_obj_add_event_cb(game_start_btn, game_start_cb, LV_EVENT_SHORT_CLICKED, head);
}

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
    for (int i = 0; i < lv_obj_get_child_cnt(lv_layer_top()); i++)
    {
        lv_obj_del(lv_obj_get_child(lv_layer_top(), i));
    }
    for (int i = 0; i < lv_obj_get_child_cnt(lv_scr_act()); i++)
    {
        lv_obj_del(lv_obj_get_child(lv_scr_act(), i));
    }
    printf("退出了\n");
    exit(0);
}

void pro_max_dis(int num)
{
    for (int i = 0; i < lv_obj_get_child_cnt(lv_layer_top()); i++)
    {
        lv_obj_clear_flag(lv_obj_get_child(lv_layer_top(), i), LV_OBJ_FLAG_HIDDEN);
    }
    for (int i = 0; i < lv_obj_get_child_cnt(lv_scr_act()); i++)
    {
        lv_obj_clear_flag(lv_obj_get_child(lv_scr_act(), i), LV_OBJ_FLAG_HIDDEN);
    }
}

void pro_min_dis(int num)
{

    for (int i = 0; i < lv_obj_get_child_cnt(lv_layer_top()); i++)
    {
        lv_obj_add_flag(lv_obj_get_child(lv_layer_top(), i), LV_OBJ_FLAG_HIDDEN);
    }
    for (int i = 0; i < lv_obj_get_child_cnt(lv_scr_act()); i++)
    {
        lv_obj_add_flag(lv_obj_get_child(lv_scr_act(), i), LV_OBJ_FLAG_HIDDEN);
    }
}

//————————————————————————————————————————————————————————————————————————————————————————————————————————————————

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

    listnode *head = list_init();
    head_bar = list_init();
    rang_game_init(head);
    style_init();
    game_frist(head);
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
