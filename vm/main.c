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
#define DISP_BUF_SIZE (480 * 1024)
#define Page_text_Prompt " Click to select the product and hold to reduce the product"
#define TEST printf("正在执行%s函数第%d行代码\n", __FILE__, __LINE__)

pthread_mutex_t lvgl_mutex;
void vendin_machine_show(lv_event_t *e);
void first_muen(void);

//=====================链表定义========================
typedef struct node_data
{
    char photo[256];
    char name[24];
    unsigned int money;
    unsigned int count;
    lv_obj_t *obj_list;
    lv_obj_t *obj_label;
} NODE_DATA;

typedef struct node
{
    struct node *p_prev;
    NODE_DATA data;
    struct node *p_next;
} NODE_T, *P_NODE_T, **PP_NODE_T;

typedef struct head_node
{
    struct node *p_frist_node;
    struct node *p_last_node;
    int node_number;
} HEAD_T, *P_HEAD_T, **PP_HEAD_NODE_T;

void list_init(PP_NODE_T pp_new, P_NODE_T data1)
{
    (*pp_new) = (P_NODE_T)malloc(sizeof(NODE_T));
    if (data1 == NULL)
    {
    }
    else
    {
        (*pp_new)->data.money = data1->data.money;
        strcpy((*pp_new)->data.name, data1->data.name);
        strcpy((*pp_new)->data.photo, data1->data.photo);
        (*pp_new)->data.count = data1->data.count;
    }
    (*pp_new)->p_next = (*pp_new);
    (*pp_new)->p_prev = (*pp_new);
}

void list_add_tail(P_HEAD_T head, P_NODE_T data)
{
    P_NODE_T p_new = NULL;
    list_init(&p_new, data);

    if (head->p_frist_node == NULL)
    {
        head->p_frist_node = p_new;
        head->p_last_node = p_new;
        head->node_number++;
    }
    else
    {
        head->p_last_node->p_next = p_new;
        p_new->p_prev = head->p_last_node;

        head->p_last_node = p_new;

        p_new->p_next = head->p_frist_node->p_next;
        head->p_last_node = p_new;

        head->node_number++;
    }
}

void list_look(P_HEAD_T head)
{
    P_NODE_T p = head->p_frist_node;
    for (int i = 0; i < head->node_number; i++, p = p->p_next)
    {
        printf("照片名:%s\n", p->data.photo);
        printf("商品价钱:%d\n", p->data.money);
        printf("商品名字:%s\n", p->data.name);
        printf("商品数量:%d\n", p->data.count);
    }
}
//=================================================、
//======================全局定义====================
bool flag = false;
lv_obj_t *text;
lv_obj_t *kb;
static lv_obj_t *main_obj;
static lv_obj_t *main_shopping;

//=================================================、

//======================样式定义====================
//
static lv_style_t style_cont;
static lv_style_t style_obj;
static lv_style_t return_btn_style;
void init_style()
{
    lv_style_init(&style_obj);
    lv_style_set_radius(&style_obj, 25);
    lv_style_set_shadow_width(&style_obj, 4);
    lv_style_set_shadow_color(&style_obj, lv_color_hex(0x00F5F2F0));
    lv_style_set_shadow_ofs_x(&style_obj, 1);
    lv_style_set_shadow_ofs_y(&style_obj, 5);

    lv_style_init(&style_cont);
    lv_style_set_radius(&style_cont, 20);
    lv_style_set_shadow_width(&style_cont, 4);
    lv_style_set_shadow_color(&style_cont, lv_color_hex(0x008C8C8C));
    lv_style_set_shadow_ofs_x(&style_cont, 5);
    lv_style_set_shadow_ofs_y(&style_cont, 5);

    lv_style_init(&return_btn_style);
    lv_style_set_radius(&return_btn_style, 30);
    lv_style_set_bg_opa(&return_btn_style, LV_OPA_20);
    lv_style_set_shadow_color(&return_btn_style, lv_color_hex(0x002F54EB));
}

//=================================================

/*
从文件中读取图片名称
并分割商品名称和金额
存到链表中
 */
void scan_files(P_HEAD_T head)
{
    char data[256] = {0};
    P_NODE_T p = NULL;
    list_init(&p, NULL);
    char *p2 = NULL;
    char *p3 = NULL;
    char money_data[10] = {0};
    unsigned int money;
    DIR *dp = opendir("../photo2/");
    struct dirent *ptr = NULL;
    while (((ptr = readdir(dp))) != NULL)
    {
        if (strcmp(".", ptr->d_name) == 0 || strcmp("..", ptr->d_name) == 0)
        {
            continue;
        }
        sprintf(data, "%s", ptr->d_name);
        strcpy(p->data.photo, data);
        printf("%s\n", p->data.photo);
        p2 = strtok(data, "-");
        p3 = strtok(NULL, "-");
        strcpy(p->data.name, p2);
        printf("%s\n", p->data.name);

        strcpy(money_data, p3);
        sscanf(money_data, "%d", &money);
        p->data.money = money;
        printf("%d\n", p->data.money);

        p->data.count = 0;
        list_add_tail(head, p);
        memset(data, '0', sizeof(data));
        p2 = NULL;
        p3 = NULL;
        memset(p, '0', sizeof(NODE_T));
    }
}

//======================函数声明====================

void status_bar(P_HEAD_T head);    //商品展示页面
void shopping_cart(P_HEAD_T head); //购物车

//=================================================

//======================回调函数====================
//商品购买数量加减
//单击为添加 长按为减少
static void count_add(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    P_NODE_T p = (P_NODE_T)e->user_data;
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        p->data.count++;
        printf("%d\n", p->data.count);
        lv_label_set_text_fmt(p->data.obj_label, "add: %d", p->data.count);
        lv_obj_set_style_bg_color(p->data.obj_list, lv_color_hex(0x0011a8fd), 0);
        printf("%d\n", p->data.count);
    }
    else if (code == LV_EVENT_LONG_PRESSED_REPEAT)
    {
        p->data.count--;
        if (p->data.count == 0)
        {
            p->data.count = 0;
            lv_obj_set_style_bg_color(p->data.obj_list, lv_color_hex(0x00bbbbbb), 0);
        }
        lv_label_set_text_fmt(p->data.obj_label, "add: %d", p->data.count);
        printf("%d\n", p->data.count);
    }
}
//================= =============页面切换========================
static void shopping_list(lv_event_t *e)
{
    P_HEAD_T head = (P_HEAD_T)e->user_data;
    lv_obj_add_flag(main_obj, LV_OBJ_FLAG_HIDDEN);
    // lv_obj_clear_flag(main_shopping, LV_OBJ_FLAG_HIDDEN);
    shopping_cart(head);
}

static void shopping_cart_cb(lv_event_t *e)
{
    // P_HEAD_T head = (P_HEAD_T)e->user_data;
    lv_obj_add_flag(main_shopping, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(main_obj, LV_OBJ_FLAG_HIDDEN);
    // status_bar(head);
}

static void first_cb(lv_event_t *e)
{
    lv_obj_del(main_obj);
    first_muen();
}

static void event_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_current_target(e);
    P_HEAD_T commodity_head = (P_HEAD_T)e->user_data;
    P_NODE_T p1 = commodity_head->p_frist_node;
    if (strcmp(lv_msgbox_get_active_btn_text(obj), "支付") == 0)
    {
        for (int i = 0; i < commodity_head->node_number; ++i, p1 = p1->p_next)
        {
            printf("正在支付\n");
            if (p1->data.count != 0)
            {
                p1->data.count = 0;
            }
        }
        lv_obj_add_flag(main_shopping, LV_OBJ_FLAG_HIDDEN);
        flag = true;
        printf("支付成功\n");
        lv_msgbox_close(obj);
        lv_obj_t *mbox = lv_msgbox_create(NULL, "支付成功", " ", NULL, true);
        lv_obj_set_style_text_font(mbox, &lv_font_simsun_16_cjk, 0);
        lv_obj_center(mbox);
        list_look(commodity_head);
    }
    else if (strcmp(lv_msgbox_get_active_btn_text(obj), "Close") == 0)
    {
        printf("关闭\n");
        lv_msgbox_close(obj);
    }
}

static void CHECK_OUT_CB(lv_event_t *e)
{
    P_HEAD_T commodity_head = (P_HEAD_T)e->user_data;
    P_NODE_T p1 = commodity_head->p_frist_node;
    int sum = 0;
    for (int i = 0; i < commodity_head->node_number; i++, p1 = p1->p_next)
    {
        sum += (p1->data.count * p1->data.money);
    }
    static const char *btns[] = {"支付", "Close", ""};
    char sum_data[100] = {0};
    sprintf(sum_data, "$%d", sum);
    lv_obj_t *mbox1 = lv_msgbox_create(NULL, "共计", sum_data, btns, true);
    lv_obj_set_style_text_font(mbox1, &lv_font_simsun_16_cjk, 0);
    lv_obj_add_event_cb(mbox1, event_cb, LV_EVENT_VALUE_CHANGED, commodity_head);
    lv_obj_center(mbox1);
}

void return_func(lv_event_t *e)
{
    lv_obj_del(((lv_obj_t *)e->user_data));
}

static void list_event_cb(lv_event_t *e)
{
    char photo_src[256] = {0};
    char money_display[256] = {0};
    P_HEAD_T head = (P_HEAD_T)e->user_data;
    P_NODE_T p = head->p_frist_node;
    for (int i = 0; i < head->node_number; i++, p = p->p_next)
    {
        if (strcmp(lv_textarea_get_text(text), p->data.name) == 0)
        {
            lv_obj_t *obj_result = lv_obj_create(lv_obj_get_child(lv_scr_act(), 1));
            lv_obj_set_size(obj_result, 400, 300);
            lv_obj_center(obj_result);

            lv_obj_t *img1 = lv_img_create(obj_result);
            sprintf(photo_src, "S:/photo2/%s", p->data.photo);
            lv_img_set_src(img1, photo_src);
            lv_obj_align(img1, LV_ALIGN_LEFT_MID, 10, 0);

            lv_obj_t *label1 = lv_label_create(obj_result);
            lv_label_set_text(label1, p->data.name);
            lv_obj_align(label1, LV_ALIGN_CENTER, 0, -20);

            lv_obj_t *label = lv_label_create(obj_result);
            sprintf(money_display, "$%d", p->data.money);
            lv_label_set_text(label, money_display);
            lv_obj_align(label, LV_ALIGN_CENTER, 0, 20);

            lv_obj_t *btn_add = lv_btn_create(obj_result);
            lv_obj_set_size(btn_add, 100, 50);
            lv_obj_align(btn_add, LV_ALIGN_RIGHT_MID, -20, 50);
            lv_obj_add_event_cb(btn_add, count_add, LV_EVENT_SHORT_CLICKED, p);

            lv_obj_t *add_label = lv_label_create(btn_add);
            lv_label_set_text(add_label, LV_SYMBOL_PLUS);
            lv_obj_center(add_label);

            lv_obj_t *btn_result = lv_btn_create(obj_result);
            lv_obj_set_size(btn_result, 100, 50);
            lv_obj_align(btn_result, LV_ALIGN_RIGHT_MID, -20, -50);

            lv_obj_t *return_label = lv_label_create(btn_result);
            lv_label_set_text(return_label, LV_SYMBOL_NEW_LINE);
            lv_obj_center(return_label);
            lv_obj_add_event_cb(btn_result, return_func, LV_EVENT_SHORT_CLICKED, obj_result);
        }
    }
}

//搜索
static void ta_event_cb1(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_FOCUSED)
    {
        lv_keyboard_set_textarea(kb, text);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    if (code == LV_EVENT_DEFOCUSED)
    {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

//=================================================

//------------------------------------------商品选择页面(start)---------------------------------------------------------------------------
void status_bar(P_HEAD_T head)
{
    TEST;
    main_obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(main_obj, 800, 480);
    lv_obj_center(main_obj);
    lv_obj_add_flag(main_obj, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_clear_flag(main_obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *obj_bar = lv_obj_create(main_obj);
    lv_obj_set_size(obj_bar, 500, 50);
    lv_obj_align_to(obj_bar, main_obj, LV_ALIGN_TOP_MID, 0, -20);
    lv_obj_clear_flag(obj_bar, LV_OBJ_FLAG_SCROLLABLE);

    static lv_ft_info_t info;
    info.name = "/font/GenSenRounded-R.ttc";
    info.weight = 20;
    info.style = 0;
    info.mem = NULL;

    if (!lv_ft_font_init(&info))
    {
        LV_LOG_ERROR("Failed to initialize font");
    }
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, info.font);
    lv_style_set_text_align(&style, LV_ALIGN_CENTER);
    lv_style_set_text_color(&style, lv_color_hex(0x00000000));

    lv_obj_t *mid_label = lv_label_create(obj_bar);
    lv_obj_set_size(mid_label, 140, 20);
    lv_obj_center(mid_label);
    lv_obj_add_style(mid_label, &style, 0);
    lv_label_set_text(mid_label, " 商品展示");

    char photo_src[256] = {0};
    char money_display[256] = {0};

    if (flag == true)
    {
        P_NODE_T p1 = head->p_frist_node;
        for (int i = 0; i < head->node_number; ++i, p1 = p1->p_next)
        {
            if (p1->data.count != 0)
            {
                p1->data.count = 0;
            }
        }
        flag = false;
    }

    P_NODE_T p = head->p_frist_node;

    lv_obj_t *cont = lv_obj_create(main_obj);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_pos(cont, 0, 40);
    lv_obj_set_size(cont, 760, 400);
    lv_obj_add_style(cont, &style_cont, LV_PART_MAIN);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLL_CHAIN_VER);

    lv_obj_set_style_bg_color(cont, lv_color_hex(0x00ff5a41), 0);

    for (uint8_t i = 0; i < head->node_number; i++, p = p->p_next)
    {
        lv_obj_t *obj = lv_obj_create(cont);
        lv_obj_set_size(obj, 110, 360);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x00FAC9A6), 0);
        lv_obj_align_to(obj, cont, LV_ALIGN_LEFT_MID, 10, 340);
        lv_obj_add_style(obj, &style_obj, LV_PART_MAIN);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *img1 = lv_img_create(obj);
        sprintf(photo_src, "S:/photo2/%s", p->data.photo);
        lv_img_set_src(img1, photo_src);
        lv_obj_center(img1); //居中显示

        lv_obj_t *label1 = lv_label_create(obj);
        lv_label_set_text(label1, p->data.name);
        lv_obj_align_to(obj, label1, LV_ALIGN_OUT_TOP_MID, 0, 0);

        lv_obj_t *label = lv_label_create(obj);
        sprintf(money_display, "$%d", p->data.money);
        lv_label_set_text(label, money_display);
        lv_obj_align_to(label, obj, LV_ALIGN_BOTTOM_MID, 0, -30);

        lv_obj_t *btn_add = lv_btn_create(obj);
        lv_obj_t *label_btn_add = lv_label_create(btn_add);

        p->data.obj_list = btn_add;
        p->data.obj_label = label_btn_add;

        lv_obj_set_size(btn_add, 70, 30);
        lv_obj_align_to(btn_add, obj, LV_ALIGN_OUT_BOTTOM_MID, 0, -45);
        lv_label_set_text(label_btn_add, LV_SYMBOL_PLUS " add");
        lv_obj_set_style_bg_color(btn_add, lv_color_hex(0x00bbbbbb), 0);
        lv_obj_add_event_cb(btn_add, count_add, LV_EVENT_ALL, p);
        lv_obj_center(label_btn_add);
    }

    lv_obj_t *btn = lv_btn_create(main_obj);
    lv_obj_set_size(btn, 135, 40);
    lv_obj_align_to(btn, obj_bar, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_add_event_cb(btn, shopping_list, LV_EVENT_SHORT_CLICKED, head);

    lv_obj_t *img = lv_img_create(btn);
    lv_img_set_src(img, LV_SYMBOL_DRIVE " shopping_cart");

    lv_obj_center(img);

    lv_obj_t *btn1 = lv_btn_create(main_obj);
    lv_obj_set_size(btn1, 135, 40);
    lv_obj_align_to(btn1, obj_bar, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_add_event_cb(btn1, first_cb, LV_EVENT_SHORT_CLICKED, head);

    lv_obj_t *img1 = lv_img_create(btn1);
    lv_img_set_src(img1, LV_SYMBOL_NEW_LINE " 返回");
    lv_obj_set_style_text_font(img1, &lv_font_simsun_16_cjk, 0);

    lv_obj_center(img1);

    //搜索功能
    kb = lv_keyboard_create(main_obj);

    text = lv_textarea_create(obj_bar);
    lv_obj_clear_flag(text, LV_OBJ_FLAG_SCROLLABLE);
    lv_keyboard_set_textarea(kb, text);
    lv_keyboard_set_popovers(kb, true);
    lv_obj_set_size(text, 150, 40);
    lv_obj_align(text, LV_ALIGN_RIGHT_MID, -35, 0);
    lv_obj_add_event_cb(text, ta_event_cb1, LV_EVENT_ALL, head);
    lv_textarea_set_placeholder_text(text, "Please");
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *label_ad = lv_label_create(obj_bar);
    lv_obj_align(label_ad, LV_ALIGN_LEFT_MID, -5, 10);
    lv_obj_set_size(label_ad, 150, 40);
    lv_label_set_text(label_ad, Page_text_Prompt);
    lv_label_set_long_mode(label_ad, LV_LABEL_LONG_SCROLL_CIRCULAR);

    lv_obj_t *btn_search = lv_obj_create(obj_bar);
    lv_obj_clear_flag(btn_search, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(btn_search, 35, 35);
    lv_obj_align_to(btn_search, text, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_radius(btn_search, 30, 0);

    lv_obj_t *img_search = lv_label_create(btn_search);
    lv_label_set_text(img_search, LV_SYMBOL_GPS);
    lv_obj_center(img_search);

    lv_obj_add_event_cb(btn_search, list_event_cb, LV_EVENT_SHORT_CLICKED, head);
}
//------------------------------------------商品选择页面(end)---------------------------------------------------------------------------

//------------------------------------------购物车页面(start)---------------------------------------------------------------------------
void shopping_cart(P_HEAD_T head)
{
    main_shopping = lv_obj_create(lv_scr_act());
    lv_obj_set_size(main_shopping, 800, 480);
    lv_obj_center(main_shopping);

    list_look(head);
    lv_obj_t *table = lv_table_create(main_shopping);
    lv_table_set_row_cnt(table, 1);
    lv_table_set_col_cnt(table, 3);
    lv_obj_align(table, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_height(table, 400);
    lv_obj_set_width(table, 400);

    P_HEAD_T commodity_head = (P_HEAD_T)malloc(sizeof(HEAD_T));
    commodity_head->p_frist_node = NULL;
    commodity_head->p_last_node = NULL;
    commodity_head->node_number = 0;

    P_NODE_T p = head->p_frist_node;
    for (int i = 0; i < head->node_number; i++, p = p->p_next)
    {
        if (p->data.count != 0)
        {
            printf("%d\n", p->data.count);
            list_add_tail(commodity_head, p);
        }
    }
    list_look(commodity_head);
    static lv_ft_info_t info;
    info.name = "/font/GenSenRounded-R.ttc";
    info.weight = 18;
    info.style = 0;
    info.mem = NULL;

    if (!lv_ft_font_init(&info))
    {
        LV_LOG_ERROR("Failed to initialize font");
    }

    P_NODE_T p1 = commodity_head->p_frist_node;

    lv_table_set_cell_value(table, 0, 0, "商品");
    lv_table_set_cell_value(table, 0, 1, "数量");
    lv_table_set_cell_value(table, 0, 2, "单价");
    lv_obj_set_style_text_font(table, info.font, 0);
    char money[10] = {0};
    char count[10] = {0};
    for (int i = 0; i < commodity_head->node_number; i++, p1 = p1->p_next)
    {
        sprintf(count, "%d", p1->data.count);
        sprintf(money, "%d", p1->data.money);
        lv_table_set_cell_value(table, i + 1, 0, p1->data.name);
        lv_table_set_cell_value(table, i + 1, 1, count);
        lv_table_set_cell_value(table, i + 1, 2, money);
        memset(money, '0', 10);
        memset(count, '0', 10);
    }
    lv_obj_t *btn = lv_btn_create(main_shopping);
    lv_obj_set_size(btn, 135, 40);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 15, -15);
    lv_obj_add_event_cb(btn, shopping_cart_cb, LV_EVENT_SHORT_CLICKED, head);

    lv_obj_t *img = lv_img_create(btn);
    lv_img_set_src(img, LV_SYMBOL_LIST " shopping");
    // lv_obj_set_style_text_font(img, &lv_font_simsun_16_cjk, 0);
    lv_obj_center(img);

    lv_obj_t *btn1 = lv_btn_create(main_shopping);
    lv_obj_set_size(btn1, 135, 40);
    lv_obj_align_to(btn1, btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
    lv_obj_add_event_cb(btn1, CHECK_OUT_CB, LV_EVENT_SHORT_CLICKED, commodity_head);

    lv_obj_t *img1 = lv_img_create(btn1);
    lv_img_set_src(img1, LV_SYMBOL_EJECT " 支付");
    lv_obj_set_style_text_font(img1, &lv_font_simsun_16_cjk, 0);
    lv_obj_center(img1);
}
//------------------------------------------购物车页面(end)---------------------------------------------------------------------------

/*
=============================================
商品选择页面
调用函数：scan_files() 将数据写入链表
         status_bar() 商品选择页面
定义头结点：head
=============================================
*/
void vendin_machine_show(lv_event_t *e)
{
    P_HEAD_T head = malloc(sizeof(HEAD_T));
    head->p_frist_node = NULL;
    head->p_last_node = NULL;
    head->node_number = 0;

    scan_files(head); // 将数据写入链表

    status_bar(head); // 商品选择页面
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
    lv_obj_del(lv_obj_get_child(lv_layer_top(), 0));
    lv_obj_del(lv_obj_get_child(lv_layer_top(), 1));
    for (int i = 0; i < lv_obj_get_child_cnt(lv_scr_act()); i++)
    {
        lv_obj_del(lv_obj_get_child(lv_scr_act(), i));
    }
    printf("退出了\n");
    exit(0);
}

void pro_max_dis(int num)
{
    TEST;
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

/*

=============================================
首界面，作为屏保使用
点击btn 进入商品选择页面
回调函数：vendin_machine_show
图片信息：S:/photo/muen.png
=============================================
*/
void first_muen(void)
{
    lv_obj_t *img = lv_img_create(lv_scr_act());
    lv_obj_t *btn = lv_btn_create(img);
    lv_obj_t *label = lv_label_create(btn);

    lv_img_set_src(img, "S:/photo/muen.png");
    lv_obj_center(img);

    lv_obj_add_event_cb(btn, vendin_machine_show, LV_EVENT_CLICKED, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x00fe5a42), 0);
    lv_obj_set_size(btn, 135, 50);
    lv_obj_set_pos(btn, 335, 300);

    lv_label_set_text(label, "place_order");
    lv_obj_center(label);
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
    lv_obj_t *obj_exit = lv_obj_create(lv_layer_top());
    lv_obj_set_size(obj_exit, 90, 40);
    lv_obj_align(obj_exit, LV_ALIGN_TOP_RIGHT, -5, 40);
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

    first_muen();

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
