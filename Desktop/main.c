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
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "app_data.h"
#include "desktop_ui.h"
#include "show_time.h"
#define DISP_BUF_SIZE (480 * 1024)

listnode *head;
pthread_mutex_t lvgl_mutex;
#define TEST printf("正在执行%s函数第%d行代码\n", __FILE__, __LINE__)

//查询pid
bool ispid(datatype arg, datatype data)
{
    if (arg.program_pid == data.program_pid)
        return true;
    else
        return false;
}

//绑定函数处理
// 思路：
//      退出：通过wait返回值可以得到退出进程的pid,系统通过比对链表数据里的pid确定是哪一个程序,从而改变程序的状态,并显示桌面
//      后台运行：通过sigaction函数绑定自定义信号,软件按下最小化会发送一个信号和一个保存有数据的结果体到系统中，系统可以获取该进程pid
void sig_stop(int num) //信号处理--进程退出
{
    pid_t exit_chile_pid = wait(NULL); //获取退出进程的pid
    if (exit_chile_pid == -1)
        return;
    printf("exit_chile_pid:%d\n", exit_chile_pid);
    datatype exit_data = {.program_pid = exit_chile_pid};
    listnode *p = list_findnode(head, ispid, exit_data); //判断pid是哪个程序
    if (p == NULL)
        return;
    p->data.program_flag = APP_CLOSE;                    //将程序运行状态调整为后台运行
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_HIDDEN); //恢复桌面显示
    if ((strcmp(p->data.process_name, "file")) == 0 || (strcmp(p->data.process_name, "weather")) == 0)
    {
        File_Data_Init(head);
        Desktop_Init(head);
    }
}

void sig_mini(int num, siginfo_t *arg_one, void *arg_two) //信号处理----进程最小化
{
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_HIDDEN); //恢复桌面显示
    pid_t mini_chile_pid = arg_one->si_pid;
    printf("mini_chile_pid:%d\n", mini_chile_pid);
    datatype exit_data = {.program_pid = mini_chile_pid};
    listnode *p = list_findnode(head, ispid, exit_data); //判断pid是哪个程序
    if (p == NULL)
        return;
    p->data.program_flag = APP_Backstage;
    File_Data_Init(head);
    Desktop_Init(head);
    kill(mini_chile_pid, SIGRTMIN + 1);
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

    //信号处理
    //+++++++++++++++++++++++++++++++++++++++++++++++++++
    signal(SIGCHLD, sig_stop);           //子进程退出
    signal(SIGRTMIN + 5, run_shell_cmd); //显示桌面天气
    signal(SIGRTMIN + 6, walpeper_cp);   //设置壁纸

    struct sigaction act;
    sigemptyset(&act.sa_mask); //清空信号集
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sig_mini;
    sigaction(SIGRTMIN, &act, NULL); //绑定自定义信号

    //初始化开发板时间，校对时间
    //获取网络时间然后通过命令设置开发板localtion时间
    show_fb();

    //+++++++++++++++++++++++++++++++++++++++++++++++++++
    //定义一条链表
    head = list_init();
    //写入数据到链表里
    File_Data_Init(head);
    //设置样式
    Desktop_Init(head);
    //桌面绘制

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
