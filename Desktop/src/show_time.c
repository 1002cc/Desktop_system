#include "show_time.h"
// IP地址 端口号
#define IP_ADDR "14.215.172.243"
#define Port_Number 80
//打印
#define TEST printf("正在执行%s函数第%d行代码\n", __FILE__, __LINE__)
//中间文件显示
#define FIFO_PIPE "../Desktop/weather_pipe" //天气管道
static lv_obj_t *mid_label;                 //文字显示
static char label_mid[100] = {"我就是我"};  //显示
extern lv_obj_t *text;
// extern pthread_mutex_t lvgl_mutex;
//两个获取当前时间的方法
//  1:网络获取，通过访问API获取网络时间，并通过正则表达式提取时间
//  2:获取开发板localtion时间,但开发板时间是不正确的，需要修改 date -s 当前时间
int show_fb()
{
    //申请一个套接字文件
    int client_fd;
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        perror("socket");
        return 0;
    }
    struct sockaddr_in server_info;
    server_info.sin_family = AF_INET;
    server_info.sin_port = htons(Port_Number); // 1023~65535
    server_info.sin_addr.s_addr = inet_addr(IP_ADDR);
    if (connect(client_fd, &server_info, sizeof(server_info)) != 0)
    {
        perror("bind");
        return 0;
    }
    printf("连接成功\n");
    for (int i = 0; i < 20; i++)
    {
        int ret = get_time_updata(client_fd);
        if (ret == 1)
        {
            return 1;
        }
    }
    printf("获取网络时间失败，系统继续运行\n");
    return 0;
}

//获取网络时间，并通过js和正则表达式提取信息处理,并通过命令修改时间
int get_time_updata(int client_fd)
{
    //正则表达式
    int status, i;
    int cflags = REG_EXTENDED;
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    regex_t reg;

    char *http = "GET /getSysTime.do HTTP/1.1\r\nHost:quan.suning.com\r\n\r\n";
    char get_day[64] = {0};
    char get_duf[1024] = {0};
    write(client_fd, http, strlen(http));
    read(client_fd, get_duf, sizeof(get_duf));
    // lv_obj_t *label = timer->user_data;
    const char *pattern = "([0-9]{3}[1-9]|[0-9]{2}[1-9][0-9]{1}|[0-9]{1}[1-9][0-9]{2}|[1-9][0-9]{3})-(((0[13578]|1[02])-(0[1-9]|[12][0-9]|3[01]))|((0[469]|11)-(0[1-9]|[12][0-9]|30))|(02-(0[1-9]|[1][0-9]|2[0-8])))\\s([0-9]|0[0-9]|1[0-9]|2[0-3]):[0-5][0-9]"; //只显示文字
    regcomp(&reg, pattern, cflags);                                                                                                                                                                                                                              //编译正则模式
    status = regexec(&reg, get_duf, nmatch, pmatch, 0);                                                                                                                                                                                                          //执行正则表达式和缓存的比较
    if (status == REG_NOMATCH)
        printf("No match\n");
    else if (0 == status)
    {
        if (pmatch[i].rm_so != -1 && pmatch[i].rm_eo != -1)
        {
            int len = pmatch[i].rm_eo - pmatch[i].rm_so;
            strncpy(get_day, get_duf + pmatch[i].rm_so, len);
            get_day[len] = '\0';
            // sprintf(stderr, "%s\n", get_day);
        }
        regfree(&reg);
        char cmd[20] = {0};
        printf("%s\n", get_day);
        sprintf(cmd, "date -s \"%s\"", get_day);
        system(cmd);
        return 1;
    }
    return 0;
    // lv_label_set_text_fmt(label, "  %s", get_day);
}

//获取lacaltiem时间,但开发板时间不对，开机时获取网络时间并改写到开发板
void lv_timer_update_time(lv_timer_t *timer)
{
    lv_obj_t *label = timer->user_data;
    // 获取系统时间
    time_t rawtime = 0;
    struct tm *info = NULL;
    time(&rawtime);
    info = localtime(&rawtime);
    lv_label_set_text_fmt(label, "  %02d:%02d  %02d-%02d-%02d", info->tm_hour, info->tm_min, (info->tm_year + 1900), (info->tm_mon + 1), info->tm_mday);
}

void up_dis_event_cb(lv_event_t *e)
{
    strcpy(label_mid, lv_textarea_get_text(text));
    Desktop_Init(lv_event_get_user_data(e));
}

//状态栏显示 时间 状态 文字
void lcd_top_widgets(lv_obj_t *parent)
{
    static lv_style_t obj_layout_style; // 容器的样式

    lv_style_init(&obj_layout_style);
    lv_style_set_pad_all(&obj_layout_style, 0);
    lv_style_set_bg_opa(&obj_layout_style, 20);
    lv_style_set_text_font(&obj_layout_style, &lv_font_montserrat_16);
    lv_style_set_border_opa(&obj_layout_style, LV_OPA_0);
    lv_style_set_radius(&obj_layout_style, 0);
    lv_style_set_text_color(&obj_layout_style, lv_color_hex(0xffffff));

    /* Layout Init */
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_set_size(panel, 800, 30);
    lv_obj_add_style(panel, &obj_layout_style, 0);
    lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, -20);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

    /* 右上角小图标 */
    lv_obj_t *panel_icon = lv_obj_create(panel);
    lv_obj_set_size(panel_icon, 200, 25);
    lv_obj_set_layout(panel_icon, LV_LAYOUT_FLEX);
    lv_obj_set_style_base_dir(panel_icon, LV_BASE_DIR_RTL, 0);
    lv_obj_set_flex_flow(panel_icon, LV_FLEX_FLOW_ROW);
    lv_obj_align(panel_icon, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_style(panel_icon, &obj_layout_style, 0);

    lv_obj_t *label = lv_label_create(panel_icon);
    lv_label_set_text(label, " ");
    lv_obj_t *label_bat = lv_label_create(panel_icon);
    lv_label_set_text(label_bat, LV_SYMBOL_BATTERY_EMPTY);

    lv_obj_t *label_batchar = lv_label_create(label_bat);
    lv_label_set_text(label_batchar, LV_SYMBOL_CHARGE);
    lv_obj_center(label_batchar);

    lv_obj_t *label_wifi = lv_label_create(panel_icon);
    lv_label_set_text(label_wifi, LV_SYMBOL_WIFI);

    lv_obj_t *label_usb = lv_label_create(panel_icon);
    lv_label_set_text(label_usb, LV_SYMBOL_USB);

    lv_obj_t *label_bluet = lv_label_create(panel_icon);
    lv_label_set_text(label_bluet, LV_SYMBOL_BLUETOOTH);

    lv_obj_t *label_time = lv_label_create(panel);
    lv_label_set_text(label_time, " ");
    lv_obj_align(label_time, LV_ALIGN_LEFT_MID, 0, 0);

    static lv_ft_info_t info;
    info.name = "/font/GenSenRounded-R.ttc";
    info.weight = 15;
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
    lv_style_set_text_color(&style, lv_color_hex(0x00ffffff));

    mid_label = lv_label_create(panel);
    lv_obj_center(mid_label);
    lv_obj_add_style(mid_label, &style, 0);
    lv_label_set_text(mid_label, label_mid);

    // lv_obj_add_flag(mid_label, LV_OBJ_FLAG_CLICKABLE);
    // lv_obj_add_event_cb(mid_label, text_mid_cb, LV_EVENT_SHORT_CLICKED, NULL);

    lv_timer_t *timer = lv_timer_create(lv_timer_update_time, 1000, label_time);
}

//将数据存到链表中
void get_weather(listnode_weather *head_weeather)
{
    //将天气图标信息数据存入链表
    datatype_weather weather = {
        .name_bin = "weather_sun",
        .text = "晴"};
    list_head_tail_insert(head_weeather, weather, 1);
    datatype_weather weather1 = {
        .name_bin = "weather_rain",
        .text = "雨"};
    list_head_tail_insert(head_weeather, weather1, 1);
    datatype_weather weather2 = {
        .name_bin = "weather_cloudy",
        .text = "多云"};
    list_head_tail_insert(head_weeather, weather2, 1);
    datatype_weather weather3 = {
        .name_bin = "weather_overcast",
        .text = "阴"};
    list_head_tail_insert(head_weeather, weather3, 1);
}

//判断数据是否存在链表
bool istext(datatype_weather arg, datatype_weather data)
{
    if (strcmp(arg.text, data.text) == 0)
        return true;
    else
        return false;
}

//打印链表数据
void *print(datatype_weather weather)
{
    printf("%s\n", weather.name_bin);
    printf("%s\n", weather.text);
}

//获取weather发来的天气,并切换图标显示，达到显示天气的效果
void run_shell_cmd(int num)
{
    listnode_weather *head_weather = list_init();
    get_weather(head_weather);
    // list_print(head_weather, print);
    datatype_weather weather;
    //打开管道,获取天气传输的天气信息
    //并和链表数据对比
    char wearther_cmd[64] = {0};
    int pipe_fd = open(FIFO_PIPE, O_RDWR);
    if (pipe_fd == -1)
    {
        printf("open出错了\n");
        return;
    }
    memset(&weather, 0, sizeof(weather));
    read(pipe_fd, weather.text, 32);
    printf("text:%s\n", weather.text);
    close(pipe_fd);
    printf("text:%s\n", weather.text);
    listnode_weather *p = list_findnode(head_weather, istext, weather);
    if (p == NULL)
        return;
    sprintf(wearther_cmd, "../Desktop/weather_icon/%s.sh", p->data.name_bin);
    system(wearther_cmd);
}

/* static void backlight_control(int32_t brightness)
{
    char ch_led_state[3];
    char *led_brightness = "/sys/class/backlight/backlight/brightness";

    int p_led_control = NULL;

    if ((p_led_control = open(led_brightness, "r+")) != NULL)
    {
        // The specified LED's brightness file is open for reading and writing.
        // Convert led_state to a char, write the result to the brightness file,
        // and close the file.

        lv_snprintf(ch_led_state, sizeof(ch_led_state), "%d", brightness);
        write(p_led_control, ch_led_state, sizeof(char));
        close(p_led_control);
    }
}

//读取屏幕亮度信息
static char get_backlight_brightenss(void)
{
    char ch_led_state[3];
    char *led_brightness = "/sys/class/backlight/../../devices/platform/pwm-backlight/backlight/pwm-backlight/brightness";

    int p_led_control = 0;

    if ((p_led_control = open(led_brightness, O_RDONLY)) != -1)
    {
        size_t ret = read(p_led_control, ch_led_state, 5);
        if (ret == 0)
        {
            exit(EXIT_FAILURE);
        }
        close(p_led_control);
    }
    printf("%s\n", ch_led_state);
    return ch_led_state;
}
 */

static void slider_event_cb(lv_event_t *e)
{
}

void lcd_brightness_init(lv_obj_t *obj)
{
    static const lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, 0};
    static lv_style_transition_dsc_t transition_dsc;
    lv_style_transition_dsc_init(&transition_dsc, props, lv_anim_path_linear, 300, 0, NULL);

    static lv_style_t style_main;
    static lv_style_t style_indicator;
    static lv_style_t style_knob;
    static lv_style_t style_pressed_color;
    lv_style_init(&style_main);
    lv_style_set_bg_opa(&style_main, LV_OPA_COVER);
    lv_style_set_bg_color(&style_main, lv_color_hex(0x00c2e9fb)); //背景条
    lv_style_set_radius(&style_main, LV_RADIUS_CIRCLE);
    lv_style_set_pad_ver(&style_main, -2);

    lv_style_init(&style_indicator);
    lv_style_set_bg_opa(&style_indicator, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indicator, lv_color_hex(0x00a1c4fd)); //进度条
    lv_style_set_radius(&style_indicator, LV_RADIUS_CIRCLE);
    lv_style_set_transition(&style_indicator, &transition_dsc);

    lv_style_init(&style_knob);
    lv_style_set_bg_opa(&style_knob, LV_OPA_COVER);
    lv_style_set_bg_color(&style_knob, lv_color_hex(0x00a1c4fd));
    lv_style_set_border_color(&style_knob, lv_color_hex(0x00e0c3fc)); //圆
    lv_style_set_border_width(&style_knob, 2);
    lv_style_set_radius(&style_knob, LV_RADIUS_CIRCLE);
    lv_style_set_pad_all(&style_knob, 6); /*Makes the knob larger*/
    lv_style_set_transition(&style_knob, &transition_dsc);

    lv_style_init(&style_pressed_color);
    lv_style_set_bg_color(&style_pressed_color, lv_color_hex(0x00e0c3fc));

    lv_obj_t *slider = lv_slider_create(obj);
    lv_obj_remove_style_all(slider);
    lv_obj_set_width(slider, 180);
    lv_bar_set_range(slider, 0, 10);
    lv_slider_set_value(slider, (100 - '0'), LV_ANIM_ON);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_add_style(slider, &style_main, LV_PART_MAIN);
    lv_obj_add_style(slider, &style_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(slider, &style_pressed_color, LV_PART_INDICATOR | LV_STATE_PRESSED);
    lv_obj_add_style(slider, &style_knob, LV_PART_KNOB);
    lv_obj_add_style(slider, &style_pressed_color, LV_PART_KNOB | LV_STATE_PRESSED);

    lv_obj_align(slider, LV_ALIGN_TOP_MID, 40, 0);
}