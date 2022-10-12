#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>

#define MUSIC_FIFO_PIPE "/Desktop/music_pipe"
#define DISP_BUF_SIZE (480 * 1024)
#define WIN_HEADER_HEIGHT (60)
#define WIN_BTN_WIDTH (50)
/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *cur_path_lable; //当前路径
static lv_obj_t *show_file_list; //展示列表
static char *file_browser_dir[256] = {0};

static lv_obj_t *win; //主窗口
static void show_dir(char *path);

static lv_style_t win_sytle;        // win风格样式
static lv_style_t win_btn;          // win中btn的风格样式
static lv_style_t win_cont_style;   // win展示区域风格样式
static lv_style_t list_menu_style;  // 左侧菜单的风格样式
static lv_style_t list_menu_btn;    // 左侧菜单按钮的风格样式
static lv_style_t file_list_style;  // 右侧文件浏览区域的样式风格
static lv_style_t return_btn_style; //返回按键样式
static lv_style_t font_style;       //中文字体样式
static lv_ft_info_t info_font;
void style_init(void)
{

    lv_style_init(&win_sytle);
    lv_style_set_radius(&win_sytle, 0);
    lv_style_set_text_font(&win_sytle, &lv_font_montserrat_20);
    lv_style_set_bg_color(&win_sytle, lv_color_hex(0xff813f));

    lv_style_init(&win_btn);
    lv_style_set_border_opa(&win_btn, 0);
    lv_style_set_outline_opa(&win_btn, 0);
    lv_style_set_radius(&win_btn, 12);
    lv_style_set_text_font(&win_btn, &lv_font_montserrat_20);

    lv_style_init(&win_cont_style);
    lv_style_set_pad_column(&win_cont_style, 0);
    lv_style_set_pad_row(&win_cont_style, 0);
    lv_style_set_flex_flow(&win_cont_style, LV_FLEX_FLOW_ROW);
    lv_style_set_pad_all(&win_cont_style, 0);
    lv_style_set_layout(&win_cont_style, LV_LAYOUT_FLEX);

    lv_style_init(&list_menu_style);
    lv_style_set_bg_color(&list_menu_style, lv_color_hex(0x00FFFFFF));
    lv_style_set_radius(&list_menu_style, 0);
    lv_style_set_pad_row(&list_menu_style, 5);
    lv_style_set_pad_all(&list_menu_style, 0);
    lv_style_set_text_font(&list_menu_style, &lv_font_montserrat_20);

    lv_style_init(&list_menu_btn);
    lv_style_set_bg_opa(&list_menu_btn, 0);
    lv_style_set_border_opa(&list_menu_btn, 0);
    lv_style_set_text_color(&list_menu_btn, lv_color_hex(0x00000000));

    lv_style_init(&file_list_style);
    lv_style_set_pad_all(&file_list_style, 0);
    lv_style_set_radius(&file_list_style, 0);
    lv_style_set_text_font(&file_list_style, &lv_font_montserrat_20);

    lv_style_init(&return_btn_style);
    lv_style_set_radius(&return_btn_style, 30);
    lv_style_set_bg_opa(&return_btn_style, LV_OPA_20);
    lv_style_set_shadow_color(&return_btn_style, lv_color_hex(0x002F54EB));

    info_font.name = "/font/GenSenRounded-R.ttc";
    info_font.weight = 20;
    info_font.style = 0;
    info_font.mem = NULL;

    if (!lv_ft_font_init(&info_font))
    {
        LV_LOG_ERROR("Failed to initialize font");
    }

    lv_style_init(&font_style);
    lv_style_set_text_font(&font_style, info_font.font);
    lv_style_set_text_align(&font_style, LV_ALIGN_CENTER);
    lv_style_set_text_color(&font_style, lv_color_hex(0x00ffffff));
}

static void trim_string(char *str) //去除最后的换行符
{
    int len = strlen(str);

    //去掉最后的换行符
    if (str[len - 1] == '\n')
    {
        len--;        //字符串长度减一
        str[len] = 0; //给字符串最后一个字符赋0，即结束符
    }
}

static void strip_ext(char *fname) // 去掉最后的后缀名
{
    char *end = fname + strlen(fname);

    while (end >= fname && *end != '/')
    {
        --end;
    }

    if (end > fname)
    {
        *end = '\0';
    }
    else if (end == fname)
    {
        *(end + 1) = '\0';
    }
}

static void win_event_handler(lv_event_t *e) //窗口按钮事件
{
    lv_obj_t *obj = lv_event_get_target(e);
    LV_LOG_USER("Button %d clicked", (int)lv_obj_get_index(obj));
}

static void click_menu_item_event_handler(lv_event_t *e) //菜单按钮事件
{
    lv_event_code_t code = lv_event_get_code(e);
    char *buff = lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED)
    {
        char *file_name[256];
        if ((strcmp(buff, "..") == 0) && (strcmp(buff, "/") != 0))
        {
            strip_ext(file_browser_dir);
            lv_snprintf(file_name, sizeof(file_name), "%s", file_browser_dir);
            show_dir(file_name);
        }
        else
        {
            show_dir(buff);
        }
    }
}

static void click_file_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        struct stat stat_buf;
        char *file_name[256];

        memset(file_name, 0, sizeof(file_name));

        if ((strcmp(lv_list_get_btn_text(show_file_list, obj), "..") == 0) && (strcmp(file_browser_dir, "/") != 0))
        {
            strip_ext(file_browser_dir);
            lv_snprintf(file_name, sizeof(file_name), "%s", file_browser_dir);
        }
        else
        {
            if (strcmp(file_browser_dir, "/") == 0)
            {
                lv_snprintf(file_name, sizeof(file_name), "%s%s", file_browser_dir,
                            lv_list_get_btn_text(show_file_list, obj));
            }
            else
            {
                lv_snprintf(file_name, sizeof(file_name), "%s/%s", file_browser_dir,
                            lv_list_get_btn_text(show_file_list, obj));
            }
        }

        if (stat(file_name, &stat_buf) == -1)
        {
            LV_LOG_USER("stat error");
        }

        if (S_ISDIR(stat_buf.st_mode))
        {
            show_dir(file_name);
        }
        else
        {
            lv_obj_t *mbox = lv_msgbox_create(NULL, "无法打开", " ", NULL, true);
            lv_obj_set_style_text_font(mbox, info_font.font, 0);
            lv_obj_center(mbox);
        }
    }
}

static void shell_opt_handle(const char *cmd, char *result) //命令处理
{
    FILE *fp;
    char buffer[128] = {0};
    fp = popen(cmd, "r");
    while (fgets(buffer, 128, fp) != NULL)
    {
        strcat(result, buffer);
    }
    if (pclose(fp) == -1)
    {
        LV_LOG_USER("close failed!");
        return NULL;
    }
}

static void show_img_exit_event(lv_event_t *e) //文件处理--展示后关闭按钮事件
{
    lv_obj_t *obj = lv_event_get_user_data(e);

    lv_obj_del(obj);
}

//安装软件
char app_shell_name[256] = {0};

static void fork_execl()
{
    pid_t app_pid = fork();

    if (app_pid == -1)
    {
        perror("fork");
    }
    else if (app_pid == 0) //子进程
    {
        execl("../Desktop/app/music", "music", NULL);
    }
    else if (app_pid > 0) //父进程
    {
        kill(app_pid, SIGRTMIN + 7);
    }
}

char *file_name[256];
static void Del_event_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_current_target(e);
    printf("%s\n", file_name);
    if (strcmp(lv_msgbox_get_active_btn_text(obj), "Del") == 0)
    {
        unlink(file_name);
        printf("删除");
    }
    else if (strcmp(lv_msgbox_get_active_btn_text(obj), "Close") == 0)
    {
        printf("关闭\n");
        lv_msgbox_close(obj);
    }
    lv_msgbox_close(lv_obj_get_parent(obj));
}

static void add_event_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_current_target(e);

    printf("%s\n", file_name);
    if (strcmp(lv_msgbox_get_active_btn_text(obj), "安装") == 0)
    {
        lv_msgbox_close(obj);
        system(file_name);
        lv_obj_t *mbox = lv_msgbox_create(NULL, "安装成功", " ", NULL, true);
        // lv_obj_set_style_text_font(mbox, &lv_font_simsun_16_cjk, 0);
        lv_obj_set_style_text_font(mbox, info_font.font, 0);
        lv_obj_center(mbox);
    }
    else if (strcmp(lv_msgbox_get_active_btn_text(obj), "关闭") == 0)
    {
        printf("关闭\n");
        lv_msgbox_close(obj);
    }
}

static void Click_callback_cb(lv_event_t *e)
{
    char *file_falg = (char *)e->user_data;
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    memset(file_name, 0, sizeof(file_name));
    lv_snprintf(file_name, sizeof(file_name), "%s/%s", file_browser_dir, lv_list_get_btn_text(show_file_list, obj));
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        if (strcmp(file_falg, "pictures") == 0)
        {
            memset(file_name, 0, sizeof(file_name));
            lv_snprintf(file_name, sizeof(file_name), "S:%s/%s", file_browser_dir, lv_list_get_btn_text(show_file_list, obj));
            lv_obj_t *show_img_con = lv_obj_create(lv_scr_act());
            lv_obj_set_size(show_img_con, 600, 360);
            lv_obj_center(show_img_con);
            lv_obj_clear_flag(show_img_con, LV_OBJ_FLAG_SCROLLABLE);

            lv_obj_t *img = lv_img_create(show_img_con);
            lv_img_set_src(img, file_name);
            lv_obj_center(img);
            lv_obj_set_size(img, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_size_mode(img, LV_IMG_SIZE_MODE_REAL);

            lv_obj_t *show_img_exit = lv_btn_create(show_img_con);
            lv_obj_align(show_img_exit, LV_ALIGN_TOP_RIGHT, 0, 0);
            lv_obj_set_size(show_img_exit, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_add_event_cb(show_img_exit, show_img_exit_event, LV_EVENT_CLICKED, show_img_con);
            lv_obj_t *show_img_exit_lable = lv_label_create(show_img_exit);
            lv_label_set_text(show_img_exit_lable, "Exit");
            lv_obj_center(show_img_exit_lable);
        }
        else if (strcmp(file_falg, "music") == 0)
        {
            if (access(MUSIC_FIFO_PIPE, F_OK) != 0)
            {
                umask(0000);
                mkfifo(MUSIC_FIFO_PIPE, 0777);
            }
            int pipe_fd = open(MUSIC_FIFO_PIPE, O_RDWR | O_TRUNC);
            write(pipe_fd, file_name, strlen(file_name) + 1);
            fork_execl();
        }
        else if (strcmp(file_falg, "text") == 0)
        {
            lv_obj_t *show_txt_con = lv_obj_create(lv_scr_act());
            lv_obj_set_size(show_txt_con, 600, 360);
            lv_obj_center(show_txt_con);

            lv_obj_t *file_txt = lv_label_create(show_txt_con);
            lv_label_set_text(file_txt, file_name);
            lv_obj_set_style_text_color(file_txt, lv_color_hex(0x00000000), 0);
            lv_obj_set_align(file_txt, LV_ALIGN_TOP_MID);

            lv_obj_t *txt = lv_label_create(show_txt_con);
            lv_label_set_long_mode(txt, LV_LABEL_LONG_WRAP);

            lv_obj_set_width(txt, 540);
            lv_obj_set_style_border_color(txt, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_obj_set_style_border_width(txt, 5, 0);
            lv_obj_set_style_pad_all(txt, 10, 0);
            lv_label_set_text_fmt(txt, "\n");
            lv_obj_set_style_text_align(txt, LV_TEXT_ALIGN_LEFT, NULL);
            lv_obj_align(txt, LV_ALIGN_TOP_MID, 0, 60);

            FILE *fp;
            char buff[128];
            bzero(buff, sizeof(buff));
            fp = fopen(file_name, "r");
            if (fp == NULL)
            {
                LV_LOG_USER("fopen err1");
                return;
            }
            while (fgets(buff, sizeof(buff), fp) != NULL)
            {
                lv_label_ins_text(txt, LV_LABEL_POS_LAST, buff);
                bzero(buff, sizeof(buff));
            }

            lv_obj_t *show_txt_exit = lv_btn_create(show_txt_con);
            lv_obj_align(show_txt_exit, LV_ALIGN_TOP_RIGHT, 0, 0);
            lv_obj_set_size(show_txt_exit, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_add_event_cb(show_txt_exit, show_img_exit_event, LV_EVENT_CLICKED, show_txt_con);
            lv_obj_t *show_txt_exit_lable = lv_label_create(show_txt_exit);
            lv_label_set_text(show_txt_exit_lable, "Exit");
            lv_obj_center(show_txt_exit_lable);
        }
        else if (strcmp(file_falg, "dpk.sh") == 0)
        {
            static const char *btns[] = {"安装", "关闭", ""};
            lv_obj_t *mbox = lv_msgbox_create(NULL, "是否确认安装该软件", " ", btns, true);
            lv_obj_set_style_text_font(mbox, info_font.font, 0);
            // lv_obj_set_style_text_font(mbox, &lv_font_simsun_16_cjk, 0);
            lv_obj_add_event_cb(mbox, add_event_cb, LV_EVENT_VALUE_CHANGED, file_name);
            lv_obj_center(mbox);
        }
    }
    else if (code == LV_EVENT_LONG_PRESSED)
    {
        // lv_snprintf(file_name_bin, sizeof(file_name_bin), "%s/%s", file_browser_dir, lv_list_get_btn_text(show_file_list, obj));
        static const char *btns[] = {"Del", "Close", ""};
        lv_obj_t *mbox_del = lv_msgbox_create(NULL, "Delete", "Please select", btns, true);
        lv_obj_center(mbox_del);
        lv_obj_add_event_cb(mbox_del, Del_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    }
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

    lv_obj_move_foreground(lv_obj_get_child(lv_scr_act(), 0));
}

void pro_min_dis(int num)
{
    lv_obj_add_flag(lv_obj_get_child(lv_layer_top(), 0), LV_OBJ_FLAG_HIDDEN); //两个按键
    lv_obj_add_flag(lv_obj_get_child(lv_layer_top(), 1), LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lv_obj_get_child(lv_scr_act(), 0), LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_background(lv_obj_get_child(lv_scr_act(), 0));
}
//————————————————————————————————————————————————————————————————————————————————————————————————————————————————

//展示文件夹
static void show_dir(char *path)
{
    lv_obj_t *btn;
    struct stat stat_buf;
    struct dirent **namelist;
    char str_file_path_name[256];
    int32_t file_n;

    file_n = scandir(path, &namelist, 0, alphasort);
    if (file_n < 0)
    {
        LV_LOG_USER("Not found!");
        return;
    }
    else
    {
        lv_obj_clean(show_file_list);
    }

    for (uint16_t i = 0; i < file_n; i++)
    {
        memset(str_file_path_name, 0, sizeof(str_file_path_name));
        lv_snprintf(str_file_path_name, sizeof(str_file_path_name), "%s/%s", path, namelist[i]->d_name);

        if ((stat(str_file_path_name, &stat_buf) == -1))
        {
            LV_LOG_USER("stat error");
            continue;
        }

        if ((strcmp((namelist[i]->d_name + (strlen(namelist[i]->d_name) - 4)), ".png") == 0) ||
            (strcmp((namelist[i]->d_name + (strlen(namelist[i]->d_name) - 4)), ".PNG") == 0) ||
            (strcmp((namelist[i]->d_name + (strlen(namelist[i]->d_name) - 4)), ".jpg") == 0) ||
            (strcmp((namelist[i]->d_name + (strlen(namelist[i]->d_name) - 4)), ".JPG") == 0) ||
            (strcmp((namelist[i]->d_name + (strlen(namelist[i]->d_name) - 4)), ".bmp") == 0) ||
            (strcmp((namelist[i]->d_name + (strlen(namelist[i]->d_name) - 4)), ".BMP") == 0) ||
            (strcmp((namelist[i]->d_name + (strlen(namelist[i]->d_name) - 4)), ".gif") == 0) ||
            (strcmp((namelist[i]->d_name + (strlen(namelist[i]->d_name) - 4)), ".GIF") == 0))
        {
            btn = lv_list_add_btn(show_file_list, NULL, NULL);
            lv_obj_t *img = lv_img_create(btn);
            lv_img_set_src(img, LV_SYMBOL_IMAGE);
            lv_obj_set_style_text_color(img, lv_color_hex(0x626B2FF), 0);
            lv_obj_t *label = lv_label_create(btn);
            lv_label_set_text(label, namelist[i]->d_name);

            lv_obj_add_event_cb(btn, Click_callback_cb, LV_EVENT_ALL, "pictures");
        }
        else if ((strcmp((namelist[i]->d_name + (strlen(namelist[i]->d_name) - 4)), ".txt") == 0) ||
                 (strcmp((namelist[i]->d_name + (strlen(namelist[i]->d_name) - 2)), ".c") == 0))
        {
            btn = lv_list_add_btn(show_file_list, NULL, NULL);
            lv_obj_t *img = lv_img_create(btn);
            lv_img_set_src(img, LV_SYMBOL_FILE);
            lv_obj_set_style_text_color(img, lv_color_hex(0xFF99CC), 0);
            lv_obj_t *label = lv_label_create(btn);
            lv_label_set_text(label, namelist[i]->d_name);
            lv_obj_add_event_cb(btn, Click_callback_cb, LV_EVENT_ALL, "text");
        }
        else if ((strcmp((namelist[i]->d_name + (strlen(namelist[i]->d_name) - 4)), ".mp3") == 0) ||
                 (strcmp((namelist[i]->d_name + (strlen(namelist[i]->d_name) - 4)), ".MP3") == 0))
        {
            btn = lv_list_add_btn(show_file_list, NULL, NULL);
            lv_obj_t *img = lv_img_create(btn);
            lv_img_set_src(img, LV_SYMBOL_AUDIO);
            lv_obj_set_style_text_color(img, lv_color_hex(0xCCFF99), 0);
            lv_obj_t *label = lv_label_create(btn);
            lv_label_set_text(label, namelist[i]->d_name);
            lv_obj_add_event_cb(btn, Click_callback_cb, LV_EVENT_ALL, "music");
        }
        else if ((strcmp(namelist[i]->d_name, "dpk.sh") == 0))
        {
            btn = lv_list_add_btn(show_file_list, NULL, NULL);
            lv_obj_t *img = lv_img_create(btn);
            lv_img_set_src(img, LV_SYMBOL_FILE);
            lv_obj_set_style_text_color(img, lv_color_hex(0xFF99CC), 0);
            lv_obj_t *label = lv_label_create(btn);
            lv_label_set_text(label, namelist[i]->d_name);
            lv_obj_add_event_cb(btn, Click_callback_cb, LV_EVENT_ALL, "dpk.sh");
        }
        else if (S_ISDIR(stat_buf.st_mode))
        {
            btn = lv_list_add_btn(show_file_list, NULL, NULL);
            lv_obj_t *img = lv_img_create(btn);
            lv_img_set_src(img, LV_SYMBOL_DIRECTORY);
            lv_obj_set_style_text_color(img, lv_color_hex(0xFFFF66), 0);
            lv_obj_t *label = lv_label_create(btn);
            lv_label_set_text(label, namelist[i]->d_name);
            lv_obj_add_event_cb(btn, click_file_event_handler, LV_EVENT_CLICKED, NULL);
        }
        else
        {
            btn = lv_list_add_btn(show_file_list, LV_SYMBOL_FILE, namelist[i]->d_name);
            lv_obj_add_event_cb(btn, click_file_event_handler, LV_EVENT_CLICKED, NULL);
        }
    }
    free(namelist);

    memset(file_browser_dir, 0, sizeof(file_browser_dir));
    strcpy(file_browser_dir, path);

    // 更新路径信息
    lv_label_set_text_fmt(cur_path_lable, LV_SYMBOL_EYE_OPEN " %s", path);
}

void file_init(void) //初始化
{
    static lv_ft_info_t info; //中文字体样式
    info.name = "/font/GenSenRounded-R.ttc";
    info.weight = 20;
    info.style = FT_FONT_STYLE_NORMAL;
    info.mem = NULL;
    if (!lv_ft_font_init(&info))
    {
        LV_LOG_ERROR("create failed.");
    }
    style_init();
    //窗口
    win = lv_win_create(lv_scr_act(), WIN_HEADER_HEIGHT);
    lv_obj_add_style(win, &win_sytle, 0);

    // 顶部按钮
    lv_obj_t *btn;
    btn = lv_win_add_btn(win, LV_SYMBOL_LEFT, WIN_BTN_WIDTH);
    lv_obj_add_style(btn, &return_btn_style, 0);
    lv_obj_add_event_cb(btn, click_menu_item_event_handler, LV_EVENT_CLICKED, "..");

    // 当前路径
    cur_path_lable = lv_win_add_title(win, "Path://");

    lv_label_set_text(cur_path_lable, "Path://");

    //标题
    lv_obj_t *win_titie = lv_win_add_title(win, "系统文件管理");
    lv_obj_set_style_text_color(win_titie, lv_color_hex(0x00000000), 0);
    lv_obj_set_style_text_font(win_titie, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_font(win_titie, info.font, 0);
    lv_obj_align(win_titie, LV_ALIGN_CENTER, 0, 0);

    // 展示区域
    lv_obj_t *win_cont = lv_win_get_content(win); /*Content can be aded here*/
    lv_obj_add_style(win_cont, &win_cont_style, 0);

    // 左侧菜单
    lv_obj_t *list_menu = lv_list_create(win_cont);
    lv_obj_set_size(list_menu, 200, LV_PCT(100));
    lv_obj_add_style(list_menu, &list_menu_style, 0);

    //菜单1-->DEVICE
    lv_obj_set_style_bg_color(lv_list_add_text(list_menu, "DEVICE"), lv_palette_main(LV_PALETTE_ORANGE), 0);

    btn = lv_list_add_btn(list_menu, NULL, LV_SYMBOL_DRIVE " File System");
    lv_obj_add_style(btn, &list_menu_btn, 0);
    lv_obj_add_event_cb(btn, click_menu_item_event_handler, LV_EVENT_CLICKED, "/");

    btn = lv_list_add_btn(list_menu, NULL, LV_SYMBOL_LIST " Desktop");
    lv_obj_add_style(btn, &list_menu_btn, 0);
    lv_obj_add_event_cb(btn, click_menu_item_event_handler, LV_EVENT_CLICKED, "/Desktop");

    //菜单1-->places
    lv_obj_set_style_bg_color(lv_list_add_text(list_menu, "ACCESS"), lv_palette_main(LV_PALETTE_LIME), 0);
    btn = lv_list_add_btn(list_menu, NULL, LV_SYMBOL_HOME " HOME");
    lv_obj_add_style(btn, &list_menu_btn, 0);
    lv_obj_add_event_cb(btn, click_menu_item_event_handler, LV_EVENT_CLICKED, "/root");
    btn = lv_list_add_btn(list_menu, NULL, LV_SYMBOL_AUDIO " Music");
    lv_obj_add_style(btn, &list_menu_btn, 0);
    lv_obj_add_event_cb(btn, click_menu_item_event_handler, LV_EVENT_CLICKED, "/Desktop/music");
    btn = lv_list_add_btn(list_menu, NULL, LV_SYMBOL_IMAGE " Pictures");
    lv_obj_add_style(btn, &list_menu_btn, 0);
    lv_obj_add_event_cb(btn, click_menu_item_event_handler, LV_EVENT_CLICKED, "/Desktop/pictures");
    btn = lv_list_add_btn(list_menu, NULL, LV_SYMBOL_TRASH " Recovery");
    lv_obj_add_style(btn, &list_menu_btn, 0);
    lv_obj_add_event_cb(btn, click_menu_item_event_handler, LV_EVENT_CLICKED, "/root");

    /* 展示区域 */
    show_file_list = lv_list_create(win_cont);
    lv_obj_set_size(show_file_list, 600, LV_PCT(100));
    lv_obj_add_style(show_file_list, &file_list_style, 0);
    lv_list_add_btn(show_file_list, LV_SYMBOL_GPS, "File Maneger");

    show_dir("/root");
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
    file_init();

    signal(SIGRTMIN, pro_max_dis);     //最大化
    signal(SIGRTMIN + 1, pro_min_dis); //最小化

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
