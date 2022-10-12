#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_drv_conf.h"
#include "lv_conf.h"
#include "lvgl/examples/lv_examples.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <signal.h>
#define WALLPAPER_FIFO_PIPE "../Desktop/wallpaper_pipe"
#define ICON_DIR "../Desktop/pictures/"
#define DISP_BUF_SIZE (480 * 1024)

typedef struct prosess
{
    char photo_name[64]; //图标显示名字

} PRO_T;

#define DATATYPE PRO_T

#include "list.h"
listnode *head;
int photo_number = 0;
void File_Data_Init(listnode *head)
{
    datatype data_user;
    DIR *dp = opendir(ICON_DIR);
    struct dirent *ptr = NULL;
    while (((ptr = readdir(dp))) != NULL)
    {
        if (strcmp(".", ptr->d_name) == 0 || strcmp("..", ptr->d_name) == 0)
        {
            continue;
        }
        //保存数据到结构体中
        sprintf(data_user.photo_name, "S:/Desktop/pictures/%s", ptr->d_name);
        printf("正在添加照片:%s\n", data_user.photo_name);
        photo_number++;
        list_head_tail_insert(head, data_user, 1); //尾插数据
        memset(&data_user, 0, sizeof(data_user));  //清空数据,循环使用
    }
    closedir(dp);
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

int cur_pic = 0;

void btnnext_cb(lv_event_t *e)
{
    printf("下一张\n");
    listnode *p = head->next;
    lv_obj_t *img = (lv_obj_t *)e->user_data;
    cur_pic++;
    if (cur_pic > photo_number)
    {
        cur_pic = 0;
    }
    for (int i = 0; i < cur_pic; i++)
    {
        p = p->next;
    }
    lv_img_set_src(img, p->data.photo_name);
}
void btnprev_cb(lv_event_t *e)
{
    printf("上一张\n");
    listnode *p = head->next;
    lv_obj_t *img = (lv_obj_t *)e->user_data;
    cur_pic--;
    if (cur_pic <= 0)
    {
        cur_pic = photo_number;
    }
    for (int i = 0; i < cur_pic; i++)
    {
        p = p->prev;
    }
    lv_img_set_src(img, p->data.photo_name);
}

//删除照片，记录当前照片路径，并显示下一张照片，通过unlink函数删除照片文件,free该节点
void btn_del_cb(lv_event_t *e)
{
    printf("删除照片\n");
    listnode *p = head->next;
    lv_obj_t *img = (lv_obj_t *)e->user_data;
    char data_del[32] = {0};
    printf("%s\n", data_del);
    cur_pic++;
    for (int i = 0; i < cur_pic; i++)
    {
        p = p->next;
    }
    listnode *p_del = p->prev;
    stpcpy(data_del, strstr(p_del->data.photo_name, "/Desktop"));
    photo_number--;
    lv_img_set_src(img, p->data.photo_name);
    lv_obj_set_align(img, LV_ALIGN_CENTER);
    unlink(data_del);
    lv_obj_t *mbox = lv_msgbox_create(NULL, "Del Success", " ", NULL, true);
    lv_obj_center(mbox);
    printf("删除成功\n");
}

//设置壁纸 通过管道发送当前照片的路径给桌面,然后桌面显示
void btn_wallpaper_cb(lv_event_t *e)
{
    if (access(WALLPAPER_FIFO_PIPE, F_OK) != 0)
    {
        umask(0000);
        mkfifo(WALLPAPER_FIFO_PIPE, 0777);
    }
    int pipe_fd = open(WALLPAPER_FIFO_PIPE, O_RDWR | O_TRUNC);
    char data_wallpaper[32] = {0};
    listnode *p = head->next;
    for (int i = 0; i < cur_pic; i++)
    {
        p = p->next;
    }
    // stpcpy(data_wallpaper, strstr(p->data.photo_name, "/Desktop"));
    // printf("wallpaper: %s\n", data_wallpaper);
    write(pipe_fd, p->data.photo_name, strlen(p->data.photo_name) + 1);
    kill(getppid(), SIGRTMIN + 6);
    usleep(1000);
    close(pipe_fd);
}

void pic_brow(listnode *head)
{
    static lv_style_t btn_style; //返回按键样式
    lv_style_init(&btn_style);
    lv_style_set_radius(&btn_style, 50);
    lv_style_set_bg_opa(&btn_style, LV_OPA_20);
    lv_style_set_shadow_color(&btn_style, lv_color_hex(0x002F54EB));

    listnode *p = head->next;
    lv_obj_t *img = lv_img_create(lv_scr_act());
    lv_obj_set_size(img, 800, 480);
    lv_img_set_src(img, p->data.photo_name);
    lv_obj_center(img);

    lv_obj_t *btnnext = lv_btn_create(lv_layer_top());
    lv_obj_set_size(btnnext, 60, 60);
    lv_obj_add_style(btnnext, &btn_style, 0);
    lv_obj_align(btnnext, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_add_event_cb(btnnext, btnnext_cb, LV_EVENT_PRESSED, img);
    lv_obj_t *next_label = lv_label_create(btnnext);
    lv_label_set_text(next_label, LV_SYMBOL_RIGHT);
    lv_obj_center(next_label);

    lv_obj_t *btnprev = lv_btn_create(lv_layer_top());
    lv_obj_set_size(btnprev, 60, 60);
    lv_obj_add_style(btnprev, &btn_style, 0);
    lv_obj_align(btnprev, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_add_event_cb(btnprev, btnprev_cb, LV_EVENT_PRESSED, img);
    lv_obj_t *prev_label = lv_label_create(btnprev);
    lv_label_set_text(prev_label, LV_SYMBOL_LEFT);
    lv_obj_center(prev_label);

    lv_obj_t *btn_del = lv_btn_create(lv_layer_top());
    lv_obj_align(btn_del, LV_ALIGN_TOP_LEFT, 0, 0);                   //靠右居中对齐
    lv_obj_set_size(btn_del, 60, 60);                                 //设置按钮对象大小
    lv_obj_set_style_radius(btn_del, 100, LV_STATE_ANY);              //设置对象的边角
    lv_obj_add_event_cb(btn_del, btn_del_cb, LV_EVENT_RELEASED, img); //给按钮添加松开后触发的回调函数
    lv_obj_add_style(btn_del, &btn_style, 0);
    lv_obj_t *del_label = lv_label_create(btn_del);
    lv_label_set_text(del_label, LV_SYMBOL_TRASH);
    lv_obj_center(del_label);

    lv_obj_t *btn_wallpaper = lv_btn_create(lv_layer_top());
    lv_obj_align_to(btn_wallpaper, btn_del, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);       //靠右居中对齐
    lv_obj_set_size(btn_wallpaper, 60, 60);                                       //设置按钮对象大小
    lv_obj_set_style_radius(btn_wallpaper, 100, LV_STATE_ANY);                    //设置对象的边角
    lv_obj_add_event_cb(btn_wallpaper, btn_wallpaper_cb, LV_EVENT_RELEASED, img); //给按钮添加松开后触发的回调函数
    lv_obj_add_style(btn_wallpaper, &btn_style, 0);
    lv_obj_t *wallpaper_label = lv_label_create(btn_wallpaper);
    lv_label_set_text(wallpaper_label, LV_SYMBOL_IMAGE);
    lv_obj_center(wallpaper_label);
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
    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);

    //返回/最小化
    //——————————————————————————————————————————————————————————————————————
    static lv_style_t return_btn_style; //返回按键样式
    lv_style_init(&return_btn_style);
    lv_style_set_radius(&return_btn_style, 30);
    lv_style_set_bg_opa(&return_btn_style, LV_OPA_20);
    lv_style_set_shadow_color(&return_btn_style, lv_color_hex(0x002F54EB));

    lv_obj_t *obj_exit = lv_obj_create(lv_layer_top());
    lv_obj_set_size(obj_exit, 90, 40);
    lv_obj_align(obj_exit, LV_ALIGN_TOP_RIGHT, -5, 0);
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

    head = list_init();
    File_Data_Init(head);
    pic_brow(head);

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
