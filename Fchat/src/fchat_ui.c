#include "fchat.ui.h"
#include "my_dict.h"

//输入法输入键盘、文本框、拼音键盘定义
static lv_obj_t *kb;
static lv_obj_t *text;
static lv_obj_t *pinyin_kb;

//回调函数
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

void up_dis_event_cb(lv_event_t *e)
{
    //strcpy(label_mid, lv_textarea_get_text(text));
}


void Fchat_ui_init(listnode*head)
{
    //主面板样式
    static lv_style_t main_style;
    lv_style_init(&main_style);
    lv_style_set_radius(&main_style,0);
    lv_style_set_bg_color(&main_style, lv_color_hex(0x007ED8E4));
    lv_style_set_bg_opa(&main_style, LV_OPA_70);

    //显示IP面板样式
    static lv_style_t show_ip_style;
     lv_style_set_radius(&show_ip_style, 25);
    lv_style_init(&show_ip_style);
    lv_style_set_radius(&show_ip_style,0);
    lv_style_set_bg_color(&show_ip_style, lv_color_hex(0x00ffffff));
    //lv_style_set_bg_opa(&show_ip_style, LV_OPA_70);
    
    //中文字体样式
    static lv_ft_info_t info;
    info.name = "/font/GenSenRounded-R.ttc";
    info.weight = 20;
    info.style = 0;
    info.mem = NULL;

    if (!lv_ft_font_init(&info))
    {
        LV_LOG_ERROR("Failed to initialize font");
    }
    static lv_style_t fnot_style;
    lv_style_init(&fnot_style);
    lv_style_set_text_font(&fnot_style, info.font);
    lv_style_set_text_align(&fnot_style, LV_ALIGN_CENTER);
    lv_style_set_text_color(&fnot_style, lv_color_hex(0x00000000));

    //链表样式
    static lv_style_t list_style; 
    lv_style_init(&list_style);
    lv_style_reset(&list_style); // 重置样式
    lv_style_set_radius(&list_style, 5); // 设置样式圆角弧度
    lv_style_set_width(&list_style, 150); // 设置样式宽度
    lv_style_set_height(&list_style, LV_SIZE_CONTENT); // 设置样式高度
    lv_style_set_pad_ver(&list_style, 20); // 设置垂直方向的填充，即padding top和padding bottom分别填充20
    lv_style_set_pad_left(&list_style, 5); // 设置左边填充
    lv_style_set_x(&list_style, lv_pct(30)); // 设置水平坐标，lv_pct转换百分比到坐标值，即X坐标为屏幕宽度的20%
    lv_style_set_y(&list_style, 100); // 设置垂直坐标
    lv_style_set_text_font(&list_style,info.font); //设置字体样式


    //主面板
    lv_obj_t*main_obj=lv_obj_create(lv_scr_act());
    lv_obj_add_style(main_obj,&main_style,0);
    lv_obj_set_size(main_obj,800,480);
    lv_obj_clear_flag(main_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(main_obj);

    //显示IP面板
    lv_obj_t* show_ip_obj=lv_obj_create(main_obj);
    lv_obj_set_size(show_ip_obj,330,390);
    lv_obj_add_style(show_ip_obj,&show_ip_style,0);
    lv_obj_align(show_ip_obj,LV_ALIGN_LEFT_MID,5,0);

    //用户列表文字显示
    lv_obj_t*show_ip_label=lv_label_create(main_obj);
    lv_obj_align_to(show_ip_label,show_ip_obj,LV_ALIGN_OUT_LEFT_MID, 5, 0);
    lv_label_set_text_fmt(show_ip_label,info.font,0);
    lv_label_set_text(show_ip_label,"用户列表");

     //显示通信信息
    lv_obj_t* show_message_obj=lv_obj_create(main_obj);
    lv_obj_set_size(show_message_obj,330,390);
    lv_obj_add_style(show_message_obj,&show_ip_style,0);
    lv_obj_align_to(show_message_obj,show_ip_obj,LV_ALIGN_OUT_RIGHT_MID,5,0);

    //信息文字显示
    lv_obj_t*show_message_label=lv_label_create(main_obj);
    lv_obj_align_to(show_message_label,show_message_obj,LV_ALIGN_OUT_LEFT_MID, 5, 0);
    lv_label_set_text_fmt(show_message_label,&info.font,0);
    lv_label_set_text(show_message_label,"信息");

    //通信录列表
    lv_obj_t* user_list = lv_list_create(show_ip_label);
    lv_obj_add_style(user_list,&list_style,0);
    lv_obj_set_size(user_list,330,390);
    lv_obj_center(user_list);

    listnode*p=head->next;
    while(p!=head)
    {
        p->data.text_obj=lv_list_add_text()
    }


}


//聊天页面
void chat_page_ui(listnode *p)
{
    /* //主面板样式
    lv_style_t main_style;
    lv_style_init(&main_style);
    lv_style_set_radius(&main_style,0);
    lv_style_set_bg_color(&main_style, lv_color_hex(0x007ED8E4));
    lv_style_set_bg_opa(&main_style, LV_OPA_20); */

    //聊天显示主面板
    lv_obj_t*main_obj=lv_obj_create(lv_scr_act());
    lv_obj_add_style(main_obj,&main_style,0);
    lv_obj_set_size(main_obj,800,480);
    lv_obj_clear_flag(main_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(main_obj);

    //显示IP面板
    lv_obj_t* show_ip_obj=lv_obj_create(main_obj);
    lv_obj_set_size(show_ip_obj,330,390);
    lv_obj_add_style(show_ip_obj,&show_ip_style,0);
    lv_obj_align(show_ip_obj,LV_ALIGN_LEFT_MID,5,0);

    //用户列表文字显示
    lv_obj_t*show_ip_label=lv_label_create(main_obj);
    lv_obj_align_to(show_ip_label,show_ip_obj,LV_ALIGN_OUT_LEFT_MID, 5, 0);
    lv_label_set_text_fmt(show_ip_label,info.font,0);
    lv_label_set_text(show_ip_label,p->data.ip);

     //键盘和文本框
    pinyin_kb = lv_ime_pinyin_create(lv_layer_top());
    kb = lv_keyboard_create(lv_layer_top());
    text = lv_textarea_create(main_obj);
    //添加中文输入

    lv_obj_move_foreground(pinyin_kb);
    lv_obj_align_to(pinyin_kb, kb, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_obj_set_style_text_font(pinyin_kb, info.font, 0); //添加自己的字库
    lv_ime_pinyin_set_dict(pinyin_kb, my_dict);          //添加字库
    lv_ime_pinyin_set_keyboard(pinyin_kb, kb);

    lv_obj_set_size(text, 360, 60);
    lv_obj_align(text, LV_ALIGN_LEFT_MID, 0, -50);
    lv_obj_set_style_text_font(text, info.font, 0);
    lv_textarea_add_text(text, " ");                //默认显示内容
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);        //开始隐藏键盘
    lv_obj_add_flag(pinyin_kb, LV_OBJ_FLAG_HIDDEN); //开始隐藏键盘

    lv_obj_add_event_cb(text, ta_event_cb1, LV_EVENT_ALL, p);

    //发送信息
    lv_obj_t *up_dis_btn = lv_btn_create(add_obj);
    lv_obj_set_size(up_dis_btn, 40, 40);
    lv_obj_add_style(up_dis_btn, &add_return_style, 0);
    lv_obj_align_to(up_dis_btn, text, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t *img_up_dis = lv_label_create(up_dis_btn);
    lv_label_set_text(img_up_dis, LV_SYMBOL_UPLOAD);
    lv_obj_center(img_up_dis);
    lv_obj_add_event_cb(up_dis_btn, up_dis_event_cb, LV_EVENT_SHORT_CLICKED, head); 
}