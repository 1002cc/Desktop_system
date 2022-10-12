#include "app_data.h"
#include "desktop_ui.h"
#include "show_time.h"
#include "my_dict.h"
#define TEST printf("正在执行%s文件第%d行代码\n", __FILE__, __LINE__)

//输入法输入键盘、文本框、拼音键盘定义
lv_obj_t *kb;
lv_obj_t *text;
lv_obj_t *pinyin_kb;
lv_obj_t *main_img;
#define WALLPAPER_FIFO_PIPE "../Desktop/wallpaper_pipe" //壁纸管道
static char wallpaper_bg_bin[100] = {"S:/Desktop/icon/Desktop.bg.png"};

//删除程序回调函数
/*
    判断程序是否在运行，如果正在运行则发送信号kill该程序进程
    删除对应的图标和程序
*/
static void Del_event_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_current_target(e);
    listnode *p = (listnode *)e->user_data;
    char photo_src[256] = {0};
    char process_bin[64] = {0};
    if (strcmp(lv_msgbox_get_active_btn_text(obj), "Del") == 0)
    {
        //如果程序正在运行，则放松信号杀死进程，并删除节点数据和app面板
        if (p->data.program_flag == APP_Backstage)
        {
            lv_obj_del(p->data.bar_obj_icon);
            kill(p->data.program_pid, SIGKILL);
        }
        sprintf(photo_src, "/Desktop/icon/%s", p->data.icon_name);
        sprintf(process_bin, "/Desktop/app/%s", p->data.process_name);
        lv_obj_del(p->data.obj_icon);
        lv_obj_del(p->data.obj_label);
        memset(&p->data, 0, sizeof(datatype));
        __NodeDel(p->prev, p->next);
        free(p);
        unlink(photo_src);
        unlink(process_bin);
        printf("删除");
    }
    else if (strcmp(lv_msgbox_get_active_btn_text(obj), "Close") == 0)
    {
        printf("关闭\n");
        lv_msgbox_close(obj);
    }
    lv_msgbox_close(lv_obj_get_parent(obj));
}

//切换背景
void walpeper_cp(int num)
{
    char data_wallpaper[64] = {0};
    int pipe_fd = open(WALLPAPER_FIFO_PIPE, O_RDWR);
    if (pipe_fd == -1)
    {
        printf("open出错了\n");
        return;
    }
    memset(&data_wallpaper, 0, sizeof(data_wallpaper));
    read(pipe_fd, data_wallpaper, 32);
    printf("text:%s\n", data_wallpaper);
    close(pipe_fd);
    strcpy(wallpaper_bg_bin, data_wallpaper);
}

int falg_wallpaper = 0;
void wallpaper_event_cb(lv_event_t *e)
{
    falg_wallpaper++;
    if (falg_wallpaper == 1)
    {
        strcpy(wallpaper_bg_bin, "S:/Desktop/pictures/7.jpg");
    }
    else if (falg_wallpaper == 2)
    {
        strcpy(wallpaper_bg_bin, "S:/Desktop/pictures/8.jpg");
    }
    else if (falg_wallpaper == 3)
    {
        strcpy(wallpaper_bg_bin, "S:/Desktop/pictures/10.jpg");
        falg_wallpaper = 0;
    }
    Desktop_Init((listnode *)lv_event_get_user_data(e));
}

void APP_FUNCTION(lv_event_t *e)
{
    listnode *p = (listnode *)e->user_data;
    lv_event_code_t code = lv_event_get_code(e);
    if (LV_EVENT_SHORT_CLICKED == code) //事件单击，运行程序
    {

        lv_obj_add_flag(lv_scr_act(), LV_OBJ_FLAG_HIDDEN); //隐藏桌面
        //如果程序没有运行则建立新的进程运行程序，并调整app运行状态为运行
        fork_newpid(p);
    }
    else if (code == LV_EVENT_LONG_PRESSED) //长按删除程序
    {
        static const char *btns[] = {"Del", "Close", ""};
        lv_obj_t *mbox_del = lv_msgbox_create(NULL, "Delete app", "Please select", btns, true);
        lv_obj_center(mbox_del);
        lv_obj_add_event_cb(mbox_del, Del_event_cb, LV_EVENT_VALUE_CHANGED, p);
    }
}

//安装软件用到的回调函数
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void list_event_cb(lv_event_t *e)
{
    //../Desktop_system/five-flag/dpk.sh
    //通过shell脚本
    //获取shell脚本路径，用system运行shell脚本，将软件图标和程序拷贝到Desktop文件icon和app文件内
    //调用scan_files函数，重新扫描文件，刷新桌面软件
    char shell_bin[64] = {0};
    memset(shell_bin, 0, sizeof(shell_bin));
    strcpy(shell_bin, lv_textarea_get_text(text));
    if (strlen(shell_bin) != 0)
    {
        int status = system(shell_bin);
        printf("%s:%d\n", shell_bin, status);
        if (-1 != status && 0 == WEXITSTATUS(status))
        {
            printf("重新加载\n");
            File_Data_Init((listnode *)e->user_data);
            lv_obj_del(lv_obj_get_child(lv_layer_top(), 0));
            lv_obj_del(lv_obj_get_child(lv_scr_act(), 0));
            Desktop_Init((listnode *)e->user_data);
        }
        else
        {
            printf("添加失败,路径\n");
        }
    }
}

//键盘是否显示
static void ta_event_cb1(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_FOCUSED)
    {
        if (lv_indev_get_type(lv_indev_get_act()) != LV_INDEV_TYPE_KEYPAD)
        {
            lv_keyboard_set_textarea(kb, text);
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
            // lv_obj_clear_flag(pinyin_kb, LV_OBJ_FLAG_HIDDEN);
        }
    }
    else if (code == LV_EVENT_CANCEL)
    {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        // lv_obj_add_flag(pinyin_kb, LV_OBJ_FLAG_HIDDEN);
        lv_indev_reset(NULL, text);
    }
    else if (code == LV_EVENT_READY)
    {
        lv_keyboard_set_textarea(kb, text);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

static void clear_event_cb()
{
    for (int i = 0; i < lv_obj_get_child_cnt(lv_layer_top()); i++)
    {
        lv_obj_del(lv_obj_get_child(lv_layer_top(), i));
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//添加软件回调函数
/*
思路：
    键盘输入软件名称
    单击添加按钮将数据添加到链表中
    键盘输入shell路径，单击添加键运行shell
    shell：将软件名和图标copy到程序运行软件目录下 app、icon
    并重新运行File_Data_Init函数，将数据添加到链表中，然后刷新屏幕
*/
static void Add_Program_Shell_CB(lv_event_t *e)
{
    static lv_ft_info_t info;
    info.name = "/font/GenSenRounded-R.ttc";
    info.weight = 16;
    info.style = 0;
    info.mem = NULL;

    if (!lv_ft_font_init(&info))
    {
        LV_LOG_ERROR("Failed to initialize font");
    }

    static lv_ft_info_t info_tile;
    info_tile.name = "/font/GenSenRounded-R.ttc";
    info_tile.weight = 20;
    info_tile.style = 0;
    info_tile.mem = NULL;

    if (!lv_ft_font_init(&info_tile))
    {
        LV_LOG_ERROR("Failed to initialize font");
    }

    static lv_style_t add_return_style;
    lv_style_init(&add_return_style); //添加-退出图标样式
    lv_style_set_radius(&add_return_style, 30);
    lv_style_set_bg_color(&add_return_style, lv_color_hex(0x008ec5fc));
    lv_style_set_bg_opa(&add_return_style, LV_OPA_70);
    lv_style_set_border_width(&add_return_style, 0);
    lv_style_set_border_color(&add_return_style, lv_color_hex(0x00ffffff));
    lv_style_set_border_opa(&add_return_style, LV_OPA_70);

    pinyin_kb = lv_ime_pinyin_create(lv_layer_top());
    listnode *head = (listnode *)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *add_obj = lv_obj_create(lv_layer_top()); //创建一个lv_layer_top图层的显示面板
    lv_obj_set_size(add_obj, 410, 125);
    lv_obj_clear_flag(add_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(add_obj, LV_ALIGN_CENTER, 0, -90);
    lv_obj_move_background(add_obj);

    //标题
    lv_obj_t *tile = lv_label_create(add_obj);
    lv_obj_set_size(tile, 100, 60);
    lv_obj_align(tile, LV_ALIGN_TOP_LEFT, 5, -5);
    lv_obj_set_style_text_font(tile, info_tile.font, 0);
    lv_obj_set_style_text_color(tile, lv_color_hex(0x008ec5fc), 0);
    lv_label_set_text(tile, "功能按键");

    //进度条控制
    lcd_brightness_init(add_obj);

    //键盘和文本框
    kb = lv_keyboard_create(lv_layer_top());
    text = lv_textarea_create(add_obj);
    //添加中文输入

    lv_obj_move_foreground(pinyin_kb);
    // lv_obj_set_style_bg_color(pinyin_kb, lv_color_hex(0x00ffffff), 0);
    lv_obj_align_to(pinyin_kb, kb, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_obj_set_style_text_font(pinyin_kb, info.font, 0); //添加自己的字库
    lv_ime_pinyin_set_dict(pinyin_kb, my_dict);          //添加字库
    lv_ime_pinyin_set_keyboard(pinyin_kb, kb);

    lv_obj_set_size(text, 300, 45);
    lv_obj_align(text, LV_ALIGN_TOP_MID, -45, 30);
    lv_textarea_set_one_line(text, true);
    // lv_keyboard_set_textarea(kb, text);
    // lv_keyboard_set_popovers(kb, true);
    lv_obj_set_style_text_font(text, info.font, 0);
    // lv_obj_set_style_text_color(text, lv_color_hex(0x008ec5fc), 0);
    lv_textarea_add_text(text, " ");                //默认显示内容
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);        //开始隐藏键盘
    lv_obj_add_flag(pinyin_kb, LV_OBJ_FLAG_HIDDEN); //开始隐藏键盘

    //添加按键

    //应用添加按键
    lv_obj_t *add_app_btn = lv_btn_create(add_obj);
    lv_obj_set_size(add_app_btn, 40, 40);
    lv_obj_add_style(add_app_btn, &add_return_style, 0);
    lv_obj_align_to(add_app_btn, text, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t *img_add = lv_label_create(add_app_btn);
    lv_label_set_text(img_add, LV_SYMBOL_PLUS);
    lv_obj_center(img_add);

    lv_obj_t *up_dis_btn = lv_btn_create(add_obj);
    lv_obj_set_size(up_dis_btn, 40, 40);
    lv_obj_add_style(up_dis_btn, &add_return_style, 0);
    lv_obj_align_to(up_dis_btn, add_app_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t *img_up_dis = lv_label_create(up_dis_btn);
    lv_label_set_text(img_up_dis, LV_SYMBOL_UPLOAD);
    lv_obj_center(img_up_dis);

    lv_obj_t *modify_wallpaper = lv_btn_create(add_obj);
    lv_obj_set_size(modify_wallpaper, 40, 40);
    lv_obj_add_style(modify_wallpaper, &add_return_style, 0);
    lv_obj_align_to(modify_wallpaper, up_dis_btn, LV_ALIGN_OUT_TOP_MID, 0, -5);

    lv_obj_t *wallpaper_label = lv_label_create(modify_wallpaper);
    lv_label_set_text(wallpaper_label, LV_SYMBOL_IMAGE);
    lv_obj_center(wallpaper_label);

    //增加事件
    lv_obj_add_event_cb(text, ta_event_cb1, LV_EVENT_ALL, head);
    lv_obj_add_event_cb(add_app_btn, list_event_cb, LV_EVENT_SHORT_CLICKED, head);
    lv_obj_add_event_cb(up_dis_btn, up_dis_event_cb, LV_EVENT_SHORT_CLICKED, head);
    lv_obj_add_event_cb(modify_wallpaper, wallpaper_event_cb, LV_EVENT_SHORT_CLICKED, head);
}

//控制拓展页面显示关闭
bool add_clean_flag = false;
static void Add_OBJ_CB(lv_event_t *e)
{
    add_clean_flag = !add_clean_flag;
    if (add_clean_flag)
        Add_Program_Shell_CB(e);
    else
        clear_event_cb();
}

/*
思路：
    遍历链表,查询链表软件状态是否为APP_Backstage
*/
//查找链表是否有该数据(用于链表查询调用)
static void back_stage_management_cb(lv_event_t *e)
{
    listnode *head = (listnode *)e->user_data;
    listnode *p = head->next;
    while (p != head)
    {
        if (p->data.program_flag == APP_Backstage)
        {
            kill(p->data.program_pid, SIGKILL);
            p->data.program_flag = 0;
            lv_obj_del(p->data.bar_obj_icon);
        }
        p = p->next;
    }
    Desktop_Init(head);
}

/*
        页面绘制
建立一个主面板 800*480
设置壁纸 S:/Desktop/desktop.png
建立一个软件显示区域
第二个面板 800*450   显示app的
绘制一个父对象为主面板的状态栏------->显示后台运行软件的
绘制顶部显示栏----->显示当前时间 一些状态图标
绘制安装软件和清理后台软件的按键

*/
void Desktop_Init(listnode *head)
{
    //样式
    //================================================================
    static lv_style_t win_style; //设置样式
    static lv_style_t bar_style;
    static lv_style_t icon_obj_style;
    static lv_style_t bar_clear_style;

    lv_style_init(&win_style); //透明界面
    lv_style_set_radius(&win_style, 0);
    lv_style_set_bg_opa(&win_style, LV_OPA_0);
    lv_style_set_border_width(&win_style, 0);
    lv_style_set_border_opa(&win_style, LV_OPA_0);

    lv_style_init(&bar_style); //状态栏样式
    lv_style_set_radius(&bar_style, 20);
    lv_style_set_bg_color(&bar_style, lv_color_hex(0x00D9E4EE));
    lv_style_set_bg_opa(&bar_style, LV_OPA_50);

    lv_style_init(&icon_obj_style); //图标样式
    lv_style_set_radius(&icon_obj_style, 20);
    lv_style_set_bg_color(&icon_obj_style, lv_color_hex(0x00FFFFFF));

    lv_style_init(&bar_clear_style); //添加—清理图标样式
    lv_style_set_radius(&bar_clear_style, 30);
    lv_style_set_bg_color(&bar_clear_style, lv_color_hex(0x009BC8F1));
    lv_style_set_bg_opa(&bar_clear_style, LV_OPA_70);
    lv_style_set_border_width(&bar_clear_style, 0);
    lv_style_set_border_color(&bar_clear_style, lv_color_hex(0x00ffffff));
    lv_style_set_border_opa(&bar_clear_style, LV_OPA_70);

    //================================================================

    //主页显示------>显示壁纸
    //------------------------------------------------------------------------
    lv_obj_t *main_obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(main_obj, 800, 480);
    lv_obj_center(main_obj);                             //居中显示
    lv_obj_clear_flag(main_obj, LV_OBJ_FLAG_SCROLLABLE); //不让图标跟随划定
    lv_obj_add_style(main_obj, &win_style, 0);

    main_img = lv_img_create(main_obj);
    lv_img_set_src(main_img, wallpaper_bg_bin);
    lv_obj_center(main_img);
    //------------------------------------------------------------------------

    //显示图标面板----->图标显示
    //------------------------------------------------------------------------
    lv_obj_t *win_obj = lv_obj_create(main_obj);
    lv_obj_add_style(win_obj, &win_style, 0);
    lv_obj_set_size(win_obj, 780, 450);
    lv_obj_center(win_obj);
    lv_obj_clear_flag(win_obj, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_flex_flow(win_obj, LV_FLEX_FLOW_ROW_WRAP); //让图标等比例排列
    lv_obj_set_style_pad_row(win_obj, 35, LV_PART_MAIN);
    lv_obj_set_style_pad_column(win_obj, 25, LV_PART_MAIN);

    //------------------------------------------------------------------------

    //后台状态显示栏----->屏幕功能按键
    //------------------------------------------------------------------------
    lv_obj_t *win_bar_app = lv_obj_create(main_obj);
    lv_obj_set_size(win_bar_app, 640, 75);
    lv_obj_add_style(win_bar_app, &bar_style, 0);
    lv_obj_set_flex_flow(win_bar_app, LV_FLEX_FLOW_ROW); //让图标等比例排列
    lv_obj_align(win_bar_app, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_clear_flag(win_bar_app, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_top(win_bar_app, 5, LV_PART_MAIN);

    //添加软件
    lv_obj_t *add_app_btn = lv_obj_create(main_obj);
    lv_obj_set_size(add_app_btn, 60, 60);
    lv_obj_add_style(add_app_btn, &bar_clear_style, 0);
    lv_obj_align_to(add_app_btn, win_bar_app, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t *img_add = lv_label_create(add_app_btn);
    lv_label_set_text(img_add, LV_SYMBOL_PLUS);
    lv_obj_center(img_add);

    //清除键
    lv_obj_t *clear_pif_btn = lv_obj_create(main_obj);
    lv_obj_set_size(clear_pif_btn, 60, 60);
    lv_obj_add_style(clear_pif_btn, &bar_clear_style, 0);
    lv_obj_align_to(clear_pif_btn, add_app_btn, LV_ALIGN_OUT_LEFT_MID, -10, 0);

    lv_obj_t *img_clear = lv_label_create(clear_pif_btn);
    lv_label_set_text(img_clear, LV_SYMBOL_CLOSE);
    lv_obj_center(img_clear);
    lv_obj_add_event_cb(add_app_btn, Add_OBJ_CB, LV_EVENT_SHORT_CLICKED, head);
    lv_obj_add_event_cb(clear_pif_btn, back_stage_management_cb, LV_EVENT_SHORT_CLICKED, head);
    //------------------------------------------------------------------------

    //顶部状态栏
    //------------------------------------------------------------------------
    lcd_top_widgets(main_obj);
    //------------------------------------------------------------------------

    char photo_src[64] = {0}; //拼接图标路径
    listnode *p = head->next;
    while (p != head)
    {
        memset(photo_src, 0, sizeof(photo_src));

        p->data.obj_icon = lv_obj_create(win_obj);
        lv_obj_set_size(p->data.obj_icon, 70, 70);
        lv_obj_add_style(p->data.obj_icon, &icon_obj_style, 0);
        lv_obj_clear_flag(p->data.obj_icon, LV_OBJ_FLAG_SCROLLABLE); //让图标图像不动

        lv_obj_t *img_icon = lv_img_create(p->data.obj_icon);
        sprintf(photo_src, "S:/Desktop/icon/%s", p->data.icon_name);
        lv_img_set_src(img_icon, photo_src);
        lv_obj_center(img_icon); //居中显示

        p->data.obj_label = lv_label_create(main_img);
        lv_obj_set_style_text_color(p->data.obj_label, lv_color_hex(0x00FFFFFF), 0);
        lv_label_set_text(p->data.obj_label, p->data.process_name);
        lv_obj_align_to(p->data.obj_label, p->data.obj_icon, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
        lv_obj_add_event_cb(p->data.obj_icon, APP_FUNCTION, LV_EVENT_ALL, p); //增加回调函数，运行相应的程序

        //判断软件是否处于后台运行状态
        //如果处于APP_Backstage状态
        //则显示在状态栏中
        if (p->data.program_flag == APP_Backstage)
        {
            memset(photo_src, 0, sizeof(photo_src));
            p->data.bar_obj_icon = lv_obj_create(win_bar_app);
            lv_obj_set_size(p->data.bar_obj_icon, 60, 60);
            lv_obj_clear_flag(p->data.bar_obj_icon, LV_OBJ_FLAG_SCROLLABLE);          //让图标图像不动
            lv_obj_add_event_cb(p->data.bar_obj_icon, APP_FUNCTION, LV_EVENT_ALL, p); //增加回调函数，运行相应的程序
            lv_obj_t *img_icon = lv_img_create(p->data.bar_obj_icon);
            sprintf(photo_src, "S:/Desktop/icon/%s", p->data.icon_name);
            lv_img_set_src(img_icon, photo_src);
            lv_obj_center(img_icon); //居中显示
        }
        p = p->next;
    }
    lv_obj_move_background(main_img); //将壁纸移动到背景后面 可以不加
}
