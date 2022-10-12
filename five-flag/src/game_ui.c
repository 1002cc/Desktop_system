#include "game_ui.h"
#include "game_data.h"

//游戏界面UI
//为了方便其他设备使用,不使用本地照片资源
/*
游戏有五个界面：  1：首菜单
                2：单机游戏UI界面
                3：联机显示UI界面
                4：联机游戏显示UI界面
                5：win界面
*/
void first_game_display()
{
    //样式定义
    //________________________________________________________________________________________________________
    static lv_ft_info_t info;
    info.name = "/font/GenSenRounded-R.ttc";
    info.weight = 25;
    info.style = 0;
    info.mem = NULL;

    if (!lv_ft_font_init(&info))
    {
        LV_LOG_ERROR("Failed to initialize font");
    }

    lv_style_init(&style);
    lv_style_set_text_font(&style, info.font);
    lv_style_set_text_align(&style, LV_ALIGN_CENTER);
    lv_style_set_text_color(&style, lv_color_hex(0x00000000));

    static lv_ft_info_t info_tipe;
    info_tipe.name = "/font/GenSenRounded-R.ttc";
    info_tipe.weight = 30;
    info_tipe.style = 0;
    info_tipe.mem = NULL;

    if (!lv_ft_font_init(&info_tipe))
    {
        LV_LOG_ERROR("Failed to initialize font");
    }
    static lv_style_t style_tipe;

    lv_style_init(&style_tipe);
    lv_style_set_text_font(&style_tipe, info_tipe.font);
    lv_style_set_text_align(&style_tipe, LV_ALIGN_CENTER);
    lv_style_set_text_color(&style_tipe, lv_color_hex(0x00000000));

    static lv_style_t title_style;
    lv_style_init(&title_style);
    lv_style_set_radius(&title_style, 30);
    lv_style_set_bg_opa(&title_style, LV_OPA_100);
    lv_style_set_shadow_color(&title_style, lv_color_hex(0x006991c7));

    static lv_style_t func_btn_style;
    lv_style_init(&func_btn_style);
    lv_style_set_radius(&func_btn_style, 30);
    lv_style_set_bg_opa(&func_btn_style, LV_OPA_100);
    lv_style_set_shadow_color(&func_btn_style, lv_color_hex(0x00a3bded));

    //________________________________________________________________________________________________________

    lv_obj_t *main_game = lv_obj_create(lv_scr_act());
    lv_obj_set_size(main_game, 800, 480);
    lv_obj_set_style_bg_color(main_game, lv_color_hex(0x00e4efe9));
    lv_obj_clear_flag(main_game, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(main_game);

    lv_obj_t *text = lv_textarea_create(main_game);
    lv_obj_set_size(text, 300, 80);
    lv_obj_set_style_radius(text, 20, 0);
    lv_obj_align(text, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t *tips_label = lv_label_create(text);
    lv_obj_add_style(tips_label, &style_tipe, 0);
    lv_obj_center(tips_label);
    lv_label_set_text(tips_label, "五子棋");

    lv_obj_t *obj_start = lv_btn_create(main_game);
    lv_obj_set_size(obj_start, 170, 65);
    lv_obj_set_pos(obj_start, 590, 120);
    lv_obj_add_style(obj_start, &func_btn_style, 0);
    lv_obj_clear_flag(obj_start, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label_start = lv_label_create(obj_start);
    lv_obj_add_style(label_start, &style, 0);
    lv_label_set_text(label_start, "开始游戏");
    lv_obj_center(label_start);

    lv_obj_t *obj_online = lv_btn_create(main_game);
    lv_obj_set_size(obj_online, 170, 65);
    lv_obj_align_to(obj_online, obj_start, LV_ALIGN_OUT_BOTTOM_MID, 0, 45);
    lv_obj_add_style(obj_online, &func_btn_style, 0);
    lv_obj_clear_flag(obj_online, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label_online = lv_label_create(obj_online);
    lv_obj_add_style(label_online, &style, 0);
    lv_label_set_text(label_online, "联机");
    lv_obj_center(label_online);

    lv_obj_t *obj_set = lv_btn_create(main_game);
    lv_obj_set_size(obj_set, 170, 65);
    lv_obj_align_to(obj_set, obj_online, LV_ALIGN_OUT_BOTTOM_MID, 0, 45);
    lv_obj_add_style(obj_set, &func_btn_style, 0);
    lv_obj_clear_flag(obj_set, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label_set = lv_label_create(obj_set);
    lv_obj_add_style(label_set, &style, 0);
    lv_label_set_text(label_set, "设置");
    lv_obj_center(label_set);

    //按键回调
    lv_obj_add_event_cb(obj_start, start_game, LV_EVENT_SHORT_CLICKED, main_game);
    lv_obj_add_event_cb(obj_online, start_online, LV_EVENT_SHORT_CLICKED, main_game);
    lv_obj_add_event_cb(obj_set, start_set, LV_EVENT_SHORT_CLICKED, main_game);
}

static void start_game(lv_obj_t *obj)
{
    static lv_ft_info_t info;
    info.name = "/font/GenSenRounded-R.ttc";
    info.weight = 20;
    info.style = 0;
    info.mem = NULL;

    if (!lv_ft_font_init(&info))
    {
        LV_LOG_ERROR("Failed to initialize font");
    }

    lv_style_init(&style);
    lv_style_set_text_font(&style, info.font);
    lv_style_set_text_align(&style, LV_ALIGN_CENTER);
    lv_style_set_text_color(&style, lv_color_hex(0x00000000));

    static lv_style_t func_btn_style;
    lv_style_init(&func_btn_style);
    lv_style_set_radius(&func_btn_style, 30);
    lv_style_set_bg_opa(&func_btn_style, LV_OPA_100);
    lv_style_set_shadow_color(&func_btn_style, lv_color_hex(0x00a3bded));

    lv_obj_t *game_one_obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(game_one_obj, 800, 480);
    lv_obj_clear_flag(game_one_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(game_one_obj);
    lv_obj_set_style_bg_color(game_one_obj, lv_color_hex(0x00ffffff), 0);

    lv_obj_t *new_game = lv_btn_create(game_one_obj);
    lv_obj_set_size(new_game, 100, 50);
    lv_obj_align(new_game, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_t *new_label = lv_label_create(new_game);

    lv_obj_add_style(new_label, &func_btn_style, 0);
    lv_label_set_text(new_label, "新游戏");
    lv_obj_center(new_label);

    pthread_t thread;
    pthread_create(&thread, NULL, Dis_pan, NULL);
    pthread_t thread_xy;
    pthread_create(&thread_xy, NULL, game_xy, (void *)game_one_obj);

    pthread_join(thread, NULL);
    pthread_join(thread_xy, NULL);
}

void *game_xy(void *arg)
{
    lv_obj_t *obj = (lv_obj_t *)arg;
    while (1)
    {
        game_control(lv_obj_get_x(obj), lv_obj_get_y(obj));
        int r = game_over();
        if (r == 1)
        {
            printf("游戏结束\n");
            break;
        }
    }
}

void start_online(lv_event_t *e)
{
    lv_obj_t *game_online_obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(game_online_obj, 800, 480);
    lv_obj_clear_flag(game_online_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(game_online_obj);
    lv_obj_set_style_bg_color(game_online_obj, lv_color_hex(0x00ffffff), 0);
}
