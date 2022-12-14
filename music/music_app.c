#include "music_app.h"
#define FIFO_PIPE "/pipe"
#define MUSIC_FIFO_PIPE "/Desktop/music_pipe" //音乐管道
#define TEST printf("正在执行%s函数第%d行代码\n", __FILE__, __LINE__)
extern pthread_mutex_t mutex_lv; // lvgl线程锁

#define MUSIC_FIFO "../Desktop/music"       //音乐路径
#define PICTURES_FIFO "../Desktop/pictures" //图片路径
#define IYRIC_FIFO "../Desktop/lyric"       //歌词路径

static char music_path[100][1024];     //音乐路径
static char pic_path[100][1024];       //图片路径
static char words_path[100][1024];     //歌词路径
static char words_buf[5 * 1024] = {0}; //放歌词的数组

static FILE *fp_mplayer = NULL; //播放器传回的管道，接收播放进度
static FILE *fp_words = NULL;   //歌词文件
static int fd_mplayer = 0;      //定义管道文件描述符,发送指令给mplayer
static int music_num = 0;       //音乐数量
static int music_index = -1;    //当前音乐的下标值，用于寻找上下首

static int percent_pos;   //当前播放百分比
static float time_length; //歌曲长度
static float time_pos;    //当前播放进度

static bool list_flag = 1;  //开关列表
static bool words_flag = 1; //歌词列表
static bool play_flag = 0;  //播放音乐开关
static bool start = 0;      //启动，线程读mplayer返回信息

static lv_style_t font_style;   //中文
static lv_obj_t *play_mode;     //下拉列表对象，播放模式
static lv_obj_t *speed_obj;     //下拉列表对象，播放速度
static lv_obj_t *music_list;    //列表对象
static lv_obj_t *pic_obj;       //图片对象
static lv_obj_t *volume_slider; //音量滑条对象
static lv_obj_t *play_slider;   //播放进度条对象

static lv_obj_t *title_label;       //图片信息标签
static lv_obj_t *words_label;       //歌词标签
static lv_obj_t *volume_label;      //音量标签
static lv_obj_t *time_length_label; //时长标签
static lv_obj_t *time_pos_label;    //当前时间标签
static lv_obj_t *words_list;        //歌词标签

pthread_cond_t cond; //条件变量，用于暂停读取mplayer
pthread_cond_t cond1;
pthread_mutex_t mutex;
pthread_mutex_t mutex1;

pthread_t tid_read;  //读mplayer的线程id
pthread_t tid_write; //写mplayer的线程id

//扫描指定文件夹内文件名,用strstr判断文件名是否是.mp3格式的
//通过拼接得到音乐的路径,并记录歌曲数量
void get_music_path()
{
    DIR *dirp = opendir(MUSIC_FIFO);
    struct dirent *msg;
    while ((msg = readdir(dirp)) != NULL)
    {
        if (strcmp(".", msg->d_name) == 0 || strcmp("..", msg->d_name) == 0)
            continue;
        if (strstr(msg->d_name, ".mp3"))
        {
            sprintf(music_path[music_num], "%s/%s", MUSIC_FIFO, msg->d_name);
            char name[100] = {0};
            strcpy(name, strtok(msg->d_name, "."));
            sprintf(pic_path[music_num], "S:%s/%s.png", PICTURES_FIFO, name);
            sprintf(words_path[music_num], "%s/%s.lrc", IYRIC_FIFO, name);

            printf("pic_path:%s\n", pic_path[music_num]);
            printf("words_path:%s\n", words_path[music_num]);
            music_num++;
            puts(music_path[music_num - 1]);
        }
    }
    printf("检索歌单完成 共%d\n", music_num);
}

//初始化字体风格
void init_font_style()
{
    static lv_ft_info_t info;
    info.name = "/font/GenSenRounded-R.ttc"; //字体位置
    info.weight = 18;                        //大小
    info.style = 0;
    info.mem = NULL;
    if (!lv_ft_font_init(&info))
    {
        LV_LOG_ERROR("create failed.");
    }
    /*给字体设置样式*/
    lv_style_init(&font_style);
    lv_style_set_text_font(&font_style, info.font);
    lv_style_set_text_align(&font_style, LV_ALIGN_CENTER);
}

//初始化图片的父对象，即图片都在这个对象中显示
void init_pic_parent_obj()
{
    //在屏幕中创建一个对象
    lv_obj_t *img = lv_obj_create(lv_scr_act());
    //取消滚动
    lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(img, 0, 0);
    lv_obj_set_size(img, 200, 200);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, -60);
    // lv_obj_set_style_pad_all
    //图片对象
    pic_obj = lv_img_create(img);
    lv_obj_center(pic_obj);
}
//初始化歌曲信息的父对象
void init_title_obj()
{
    lv_obj_t *title = lv_obj_create(lv_scr_act());
    lv_obj_clear_flag(title, LV_OBJ_FLAG_SCROLLABLE); //禁用滚动
    lv_obj_set_size(title, 200, 50);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);
    //标题标签
    title_label = lv_label_create(title);
    //添加字体
    lv_obj_add_style(title_label, &font_style, 0);
    lv_obj_center(title_label);
    lv_label_set_text(title_label, "音乐");
    //歌词标签
    words_label = lv_label_create(lv_scr_act());
    lv_obj_add_style(words_label, &font_style, 0);

    lv_obj_align(words_label, LV_ALIGN_CENTER, 0, 70);
    lv_label_set_text(words_label, "歌词");
}

//信号任务
void signal_10_task(int arg)
{
    if (arg == 10)
    {
        printf("收到信号 %d 线程任务(读取mplayer)暂停\n", arg);
        pthread_kill(tid_write, 12);      //停止写
        pthread_cond_wait(&cond, &mutex); //停止读

        pthread_cond_signal(&cond1); //唤醒获取时间
        puts("继续工作,读mplayer");
        return;
    }
}
void signal_12_task(int arg)
{
    // printf("收到信号 %d\n",arg);
    printf("收到信号 %d 线程任务(get_time_pos)暂停\n", arg);
    pthread_cond_wait(&cond1, &mutex1);
    puts("继续工作,写mplayer");
    return;
}

static char words_line[1024] = {0};
void *find_words_task(void *arg) //找歌词
{
    char *p = strstr(words_buf, (char *)arg); //寻找这个时间有没有歌词
    if (p != NULL)                            //和歌词相同时间
    {
        char buf[1024] = {0};
        sscanf(p, "%s", buf);

        char *q = strrchr(buf, ']');
        sscanf(++q, "%s", buf);
        if (strcmp(buf, words_line)) //不相同
        {
            strcpy(words_line, buf);
            puts(words_line);
            // pthread_mutex_lock(&mutex_lv); //上锁
            lv_label_set_text(words_label, words_line);
            // pthread_mutex_unlock(&mutex_lv);
        }
    }
}
//线程任务 读播放器返回内容
static char show_time_buf[100] = {0};
void *read_mplayer_task(void *arg)
{
    signal(10, signal_10_task); //收到10就暂停
    char line[1024] = {0};      //返回信息
    char *p;
    while (1)
    {
        memset(line, 0, 1024);
        fgets(line, 1024, fp_mplayer); //读取进程返回的内容
        // printf("RETURN %s",line);
        if (strstr(line, "ANS_TIME_POSITION")) //当前播放秒数
        {
            p = strrchr(line, '=');
            p++;
            sscanf(p, "%f", &time_pos);
            // printf("歌曲播放时间 %.2f\n",time_pos);
            char tmp[100] = {0};
            int tmp_time = time_pos;
            sprintf(tmp, "%02d:%02d", tmp_time / 60, tmp_time % 60);
            if (strcmp(show_time_buf, tmp)) //不相同
            {
                strcpy(show_time_buf, tmp);
                // lv_label_set_text(time_pos_label,show_time_buf); //设置标签
                pthread_mutex_lock(&mutex_lv); //上锁
                lv_label_set_text(time_pos_label, show_time_buf);
                pthread_mutex_unlock(&mutex_lv);
            }
            puts(tmp);
            pthread_t tid;
            pthread_create(&tid, NULL, find_words_task, tmp);
            pthread_detach(tid);
        }
        if (strstr(line, "ANS_PERCENT_POSITION")) //播放百分比
        {
            p = strrchr(line, '='); // p++;
            int percent = 0;
            sscanf(++p, "%d", &percent);
            if (percent != percent_pos)
            {
                percent_pos = percent;
                // lv_slider_set_value(play_slider,percent_pos,LV_ANIM_OFF);//设置进度条值
                pthread_mutex_lock(&mutex_lv); //上锁
                lv_slider_set_value(play_slider, percent_pos, LV_ANIM_OFF);
                pthread_mutex_unlock(&mutex_lv);
            }
            if (percent_pos >= 99) //播完了
            {
                lv_label_set_text(time_pos_label, "0:00");
                //播放模式
                int mod = lv_dropdown_get_selected(play_mode);
                switch (mod)
                {
                case 0:
                {
                    pthread_kill(tid_read, 10); //停止读写mplayer
                    break;
                }
                case 1: //循环播放
                    break;
                case 2: //列表循环
                {
                    if (++music_index >= music_num)
                        music_index = 0;
                    play_one_music();
                    break;
                }
                case 3: //随机播放
                {
                    srand((unsigned)time(NULL));
                    music_index = (music_index + rand()) % music_num;
                    play_one_music();
                    break;
                }
                default:
                    break;
                }
            }
        }
        if (strstr(line, "ANS_LENGTH")) //歌曲长度
        {
            p = strrchr(line, '=');
            p++;
            sscanf(p, "%f", &time_length);
            printf("歌曲长度 %.2f\n", time_length);
            char time_buf[100] = {0};
            int length = time_length;
            sprintf(time_buf, "%02d:%02d", length / 60, length % 60);
            puts(time_buf);
            pthread_mutex_lock(&mutex_lv);                  //上锁
            lv_label_set_text(time_length_label, time_buf); //设置标签
            pthread_mutex_unlock(&mutex_lv);
        }
    }
}
//线程任务 发送命令给mplayer
void *write_mplayer_task(void *arg)
{
    //注册一个信号
    signal(12, signal_12_task); //收到12就暂停
    char cmd1[1024] = "get_time_pos\n";
    char cmd[1024] = "get_percent_pos\n";
    while (1)
    {
        write(fd_mplayer, cmd1, strlen(cmd1)); //发送获取时间命令
        usleep(400 * 1000);
        write(fd_mplayer, cmd, strlen(cmd)); //发送获取百分比
        usleep(400 * 1000);
        // sleep(1);
    }
}
//线程任务 创建子进程播放音乐
void *play_music_task(void *arg)
{
    // printf("---- %ld线程任务------------------\n",pthread_self());
    //拼接命令
    char cmd[1024] = "../mplayer -slave -quiet -input file=/pipe";
    sprintf(cmd, "%s %s", cmd, music_path[music_index]);
    printf("%s\n", cmd);
    // puts(cmd);
    //播放音乐
    fp_mplayer = popen(cmd, "r");
    if (fp_mplayer == NULL)
        perror("popen fail:");
    puts("----线程任务(启动播放音乐)完成 -----------------------\n");

    strcpy(cmd, "get_time_length\n"); //获取长度
    write(fd_mplayer, cmd, strlen(cmd));
    strcpy(cmd, "get_time_pos\n"); //发送获取时间命令
    write(fd_mplayer, cmd, strlen(cmd));
    pthread_exit(NULL); //线程结束
}

//播放一首音乐
void play_one_music()
{
    if (play_flag != 0) //有进程在使用播放器
        system("killall -9 mplayer");
    play_flag = 1;

    // printf("music_index %d\n", music_index);
    //开启线程播放这首歌曲
    pthread_t tid;
    pthread_create(&tid, NULL, play_music_task, NULL);
    pthread_detach(tid); //分离属性，自动回收

    if (!start)
    {                 //第一次才开启
        usleep(1000); //等待启动播放器
        //开启一个线程读取mplayer返回的内容
        // pthread_t tid_read;
        pthread_create(&tid_read, NULL, read_mplayer_task, NULL);
        pthread_detach(tid_read); //分离属性，自动回收
        //开启一个线程发送获取时间命令
        pthread_create(&tid_write, NULL, write_mplayer_task, NULL);
        pthread_detach(tid_write); //分离属性，自动回收
        printf("tid_read %ld  tid_write %ld\n", tid_read, tid_write);
        start = 1;
    }
    pthread_cond_signal(&cond); //唤醒线程读取mplayer返回的内容

    //***********************************************************//
    //显示音乐信息
    char tmp[100] = {0};
    char *p = strrchr(music_path[music_index], '/');
    strcpy(tmp, ++p);
    lv_label_set_text(title_label, strtok(tmp, "."));
    lv_label_set_text(words_label, strtok(tmp, "."));
    //显示音乐图片
    lv_obj_set_style_radius(pic_obj, 100, 0);
    lv_img_set_src(pic_obj, pic_path[music_index]);
    //打开歌词文件
    FILE *fp = fopen(words_path[music_index], "r");
    memset(words_buf, 0, sizeof(words_buf));
    if (fp != NULL)
        fread(words_buf, 5 * 1024, 1, fp);
    fclose(fp);
    //设置播放属性
    int ret = lv_dropdown_get_selected(speed_obj);
    //音量
    int volume = (int)lv_slider_get_value(volume_slider);
    char cmd[1024] = {0};
    sprintf(cmd, "volume %d 1\n", volume);
    write(fd_mplayer, cmd, strlen(cmd));

    //修改播放速度
    switch (ret)
    {
    case 0:
    {
        char cmd[1024] = {"speed_set 1\n"};
        write(fd_mplayer, cmd, strlen(cmd));
        break;
    }
    case 1:
    {
        char cmd[1024] = {"speed_set 1.25\n"};
        write(fd_mplayer, cmd, strlen(cmd));
        break;
    }
    case 2:
    {
        char cmd[1024] = {"speed_set 1.5\n"};
        write(fd_mplayer, cmd, strlen(cmd));
        break;
    }
    case 3:
    {
        char cmd[1024] = {"speed_set 2\n"};
        write(fd_mplayer, cmd, strlen(cmd));
        break;
    }
    default:
        break;
    }
    //播放模式
    int mod = lv_dropdown_get_selected(play_mode);
    switch (mod)
    {
    case 0:
    {
        char cmd[1024] = {"loop -1\n"};
        write(fd_mplayer, cmd, strlen(cmd));
        break;
    }
    case 1:
    {
        char cmd[1024] = {"loop -1\n"};
        write(fd_mplayer, cmd, strlen(cmd));
        strcpy(cmd, "loop 1\n");
        write(fd_mplayer, cmd, strlen(cmd));
        break;
    }
    case 2: //列表循环
    {
        break;
    }
    case 3: //随机播放
    {
        break;
    }
    default:
        break;
    }
}

//列表处理函数,播放列表中的音乐
static lv_event_cb_t event_handler_music_list(lv_event_t *e)
{
    //获取事件码
    lv_event_code_t code = lv_event_get_code(e);
    //获取事件对象,这里是按钮
    lv_obj_t *obj = lv_event_get_target(e);
    //如果点击按钮
    if (code == LV_EVENT_CLICKED)
    {
        //打印按钮中的文本
        // LV_LOG_USER("Clicked: %s", lv_list_get_btn_text(music_list, obj));
        printf("Clicked  %s\n", lv_list_get_btn_text(music_list, obj));

        //当前播放中的音乐下标更新为点击列表的下标
        music_index = lv_event_get_user_data(e);
        play_one_music();
    }
}
//显示歌单列表
static void show_list()
{
    /*创建列表*/
    music_list = lv_list_create(lv_scr_act());
    //添加风格
    lv_obj_add_style(music_list, &font_style, 0);
    lv_obj_set_size(music_list, 180, 300);
    //列表位置
    // lv_obj_center(music_list);
    lv_obj_align(music_list, LV_ALIGN_LEFT_MID, 20, -80);

    //添加文本
    lv_list_add_text(music_list, "音乐列表");
    /*在列表中添加按钮*/
    lv_obj_t *btn;
    for (int i = 0; i < music_num; i++)
    {
        char tmp[100] = {0};
        char *p = music_path[i];
        p = strrchr(p, '/');
        strcpy(tmp, ++p); //裁剪到只剩音乐名字  music.mp3
        //参数：列表对象，图标宏，按钮名
        btn = lv_list_add_btn(music_list, NULL, strtok(tmp, "."));
        //列表按钮风格
        lv_obj_add_style(music_list, &font_style, 0);
        //触发事件，把下标传递
        lv_obj_add_event_cb(btn, event_handler_music_list, LV_EVENT_CLICKED, i);
    }
}

//按钮事件
static void btn_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    char *msg = lv_event_get_user_data(e);
    if (strcmp(msg, "pause") == 0)
    {
        if (!start)
            return;
        pthread_kill(tid_read, 10); //暂停读mplayer返回内容
        system("killall -19 mplayer");
    }
    if (strcmp(msg, "play") == 0)
    {
        if (!start)
            return;
        system("killall -18 mplayer"); //播放器继续
        pthread_cond_signal(&cond);    //唤醒读写mplayer
        usleep(1000);
        pthread_cond_signal(&cond); //防止被再次暂停
    }
    if (strcmp(msg, "forward") == 0)
    {
        if (!start)
            return;
        usleep(1000);
        char cmd[1024] = {"seek +10\n"};
        write(fd_mplayer, cmd, strlen(cmd));
        strcpy(cmd, "get_percent_pos\n");
        write(fd_mplayer, cmd, strlen(cmd)); //发送获取时间命令
    }
    if (strcmp(msg, "back") == 0)
    {
        if (!start)
            return;
        usleep(10000);
        char cmd[1024] = {"seek -10\n"};
        write(fd_mplayer, cmd, strlen(cmd));
        strcpy(cmd, "get_percent_pos\n");
        write(fd_mplayer, cmd, strlen(cmd)); //发送获取时间命令
    }
    if (strcmp(msg, "next_music") == 0)
    {
        usleep(1000);
        //播放模式
        int mod = lv_dropdown_get_selected(play_mode);
        switch (mod)
        {
        case 1:
        {
            play_one_music();
            break;
        }
        case 0:
        case 2: //列表循环
        {
            if (++music_index >= music_num)
                music_index = 0;
            play_one_music();
            break;
        }
        case 3: //随机播放
        {
            srand((unsigned)time(NULL));
            music_index = (music_index + rand()) % music_num;
            printf("music_index %d\n", music_index);
            play_one_music();
            break;
        }
        default:
            break;
        }
    }
    if (strcmp(msg, "prev_music") == 0)
    {
        usleep(1000);
        //播放模式
        int mod = lv_dropdown_get_selected(play_mode);
        switch (mod)
        {
        case 1: //循环播放
        {
            char cmd[1024] = {"loop -1\n"};
            write(fd_mplayer, cmd, strlen(cmd));
            strcpy(cmd, "loop 1\n");
            write(fd_mplayer, cmd, strlen(cmd));
            play_one_music();
            break;
        }
        case 0: //单曲
        case 2: //列表循环
        {
            if (--music_index <= 0)
                music_index = music_num - 1;
            play_one_music();
            break;
        }
        case 3: //随机播放
        {
            srand((unsigned)time(NULL));
            music_index = (music_index + rand()) % music_num;
            play_one_music();
            break;
        }
        default:
            break;
        }
    }
    if (strcmp(msg, "show_list") == 0)
    {
        list_flag = !list_flag;
        if (list_flag)
            lv_obj_clear_flag(music_list, LV_OBJ_FLAG_HIDDEN);
        else
            lv_obj_add_flag(music_list, LV_OBJ_FLAG_HIDDEN);
    }
}
int flag_paly_pause = 0;
void play_pause_cb(lv_event_t *e)
{
    if (flag_paly_pause == 0)
    {
        lv_obj_add_event_cb((lv_obj_t *)lv_event_get_user_data(e), btn_handler, LV_EVENT_CLICKED, "pause");
        lv_obj_set_style_bg_color((lv_obj_t *)lv_event_get_user_data(e), lv_color_hex(0x00bc2eb), 0);
        lv_obj_set_style_bg_img_src((lv_obj_t *)lv_event_get_user_data(e), LV_SYMBOL_PLAY, 0);
    }
    else if (flag_paly_pause == 1)
    {
        lv_obj_add_event_cb((lv_obj_t *)lv_event_get_user_data(e), btn_handler, LV_EVENT_CLICKED, "play");
        lv_obj_set_style_bg_color((lv_obj_t *)lv_event_get_user_data(e), lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_set_style_bg_img_src((lv_obj_t *)lv_event_get_user_data(e), LV_SYMBOL_PAUSE, 0);
    }
    flag_paly_pause++;
    if (flag_paly_pause >= 2)
    {
        flag_paly_pause = 0;
    }
}

//显示按钮
void show_button()
{
    //风格设置
    static lv_style_t btn_style;
    lv_style_init(&btn_style);
    /*Set a background color and a radius*/
    lv_style_set_radius(&btn_style, 90);
    lv_style_set_bg_opa(&btn_style, LV_OPA_COVER);
    lv_style_set_bg_color(&btn_style, lv_palette_lighten(LV_PALETTE_BLUE, 1)); //按钮色
    /*Add a shadow*/
    lv_style_set_shadow_width(&btn_style, 55);
    lv_style_set_shadow_color(&btn_style, lv_palette_main(LV_PALETTE_BLUE)); //背景色

    //开始显示play
    lv_obj_t *btn_play = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn_play, play_pause_cb, LV_EVENT_SHORT_CLICKED, btn_play);
    lv_obj_align(btn_play, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_size(btn_play, 60, 60);
    lv_obj_set_style_bg_img_src(btn_play, LV_SYMBOL_PAUSE, 0);
    lv_obj_add_style(btn_play, &btn_style, 0);

    //下一首
    lv_obj_t *btn_next = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn_next, btn_handler, LV_EVENT_CLICKED, "next_music");
    lv_obj_set_size(btn_next, 60, 60);
    lv_obj_align_to(btn_next, btn_play, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style_bg_img_src(btn_next, LV_SYMBOL_NEXT, 0);
    lv_obj_add_style(btn_next, &btn_style, 0);
    //上一首
    lv_obj_t *btn_prev = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn_prev, btn_handler, LV_EVENT_CLICKED, "prev_music");
    lv_obj_set_size(btn_prev, 60, 60);
    lv_obj_align_to(btn_prev, btn_play, LV_ALIGN_OUT_LEFT_MID, -20, 0);
    lv_obj_set_style_bg_img_src(btn_prev, LV_SYMBOL_PREV, 0);
    lv_obj_add_style(btn_prev, &btn_style, 0);

    //快进
    lv_obj_t *btn_forward = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn_forward, btn_handler, LV_EVENT_CLICKED, "forward");
    lv_obj_set_size(btn_forward, 60, 60);
    lv_obj_align_to(btn_forward, btn_next, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style_bg_img_src(btn_forward, LV_SYMBOL_RIGHT LV_SYMBOL_RIGHT, 0);
    lv_obj_add_style(btn_forward, &btn_style, 0);
    //快退
    lv_obj_t *btn_back = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn_back, btn_handler, LV_EVENT_CLICKED, "back");
    lv_obj_set_size(btn_back, 60, 60);
    lv_obj_align_to(btn_back, btn_prev, LV_ALIGN_OUT_LEFT_MID, -20, 0);
    lv_obj_set_style_bg_img_src(btn_back, LV_SYMBOL_LEFT LV_SYMBOL_LEFT, 0);
    lv_obj_add_style(btn_back, &btn_style, 0);

    //显示,隐藏  歌单列表
    //风格设置
    static lv_style_t style;
    lv_style_init(&style);
    /*Set a background color and a radius*/
    lv_style_set_radius(&style, 5);
    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    lv_style_set_bg_color(&style, lv_palette_lighten(LV_PALETTE_GREY, 1));
    /*Add a shadow*/
    lv_style_set_shadow_width(&style, 55);
    lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_BLUE));
    //歌单列表
    lv_obj_t *btn_list = lv_btn_create(lv_scr_act());
    lv_obj_set_style_bg_color(btn_list, lv_color_hex(0x00fad0c4), 0);
    lv_obj_add_event_cb(btn_list, btn_handler, LV_EVENT_CLICKED, "show_list");
    lv_obj_set_size(btn_list, 60, 60);
    lv_obj_align(btn_list, LV_ALIGN_BOTTOM_LEFT, 20, -100);
    lv_obj_t *label = lv_label_create(btn_list);
    lv_obj_add_style(label, &font_style, 0);
    lv_label_set_text(label, "歌单");
    lv_obj_center(label);
    lv_obj_add_style(btn_list, &style, 0);
}

//下拉列表事件
static void dd_handler(lv_event_t *e)
{
    //获取事件码
    lv_event_code_t code = lv_event_get_code(e);
    //获取传递的参数
    char *msg = lv_event_get_user_data(e);
    if (code == LV_EVENT_VALUE_CHANGED) //值被修改
    {
        if (strcmp(msg, "speed") == 0)
        {
            //获取事件对象
            // lv_obj_t *speed = lv_event_get_target(e);
            int ret = lv_dropdown_get_selected(speed_obj);
            // printf("ret = %d\n",ret);
            //修改播放速度
            switch (ret)
            {
            case 0:
            {
                char cmd[1024] = {"speed_set 1\n"};
                write(fd_mplayer, cmd, strlen(cmd));
                break;
            }
            case 1:
            {
                char cmd[1024] = {"speed_set 1.25\n"};
                write(fd_mplayer, cmd, strlen(cmd));
                break;
            }
            case 2:
            {
                char cmd[1024] = {"speed_set 1.5\n"};
                write(fd_mplayer, cmd, strlen(cmd));
                break;
            }
            case 3:
            {
                char cmd[1024] = {"speed_set 2\n"};
                write(fd_mplayer, cmd, strlen(cmd));
                break;
            }
            default:
                break;
            }
        }
        if (strcmp(msg, "play_mode") == 0)
        {
            //播放模式
            int mod = lv_dropdown_get_selected(play_mode);
            switch (mod)
            {
            case 0:
            {
                char cmd[1024] = {"loop -1\n"};
                write(fd_mplayer, cmd, strlen(cmd));
                break;
            }
            case 1:
            {
                char cmd[1024] = {"loop -1\n"};
                write(fd_mplayer, cmd, strlen(cmd));
                strcpy(cmd, "loop 1\n");
                write(fd_mplayer, cmd, strlen(cmd));
                break;
            }
            case 2: //列表循环
            {
                break;
            }
            case 3: //随机播放
            {
                break;
            }
            default:
                break;
            }
        }
    }
}
//显示下拉列表
static void show_drop_down()
{
    static const char *opts = "x1\n"
                              "x1.25\n"
                              "x1.5\n"
                              "x2";
    static const char *opts1 = "单曲\n"
                               "循环\n"
                               "顺序\n"
                               "随机";
    //播放速度 创建一个对象
    speed_obj = lv_dropdown_create(lv_scr_act());
    // lv_dropdown_set_text(speed_obj, "Speed");
    //触发事件
    lv_obj_add_event_cb(speed_obj, dd_handler, LV_EVENT_ALL, "speed");
    //添加选项
    lv_dropdown_set_options_static(speed_obj, opts);
    //大小
    lv_obj_set_size(speed_obj, 80, 40);
    //位置
    lv_obj_align(speed_obj, LV_ALIGN_BOTTOM_RIGHT, -60, -30);
    //箭头位置，现在由低朝上
    lv_dropdown_set_dir(speed_obj, LV_DIR_BOTTOM);
    //列表弹出方向
    lv_dropdown_set_symbol(speed_obj, LV_SYMBOL_UP);
    //标签
    lv_obj_t *speed_label = lv_label_create(lv_scr_act());
    lv_obj_add_style(speed_label, &font_style, 0);
    lv_label_set_text(speed_label, "播放速度");
    lv_obj_align_to(speed_label, speed_obj, LV_ALIGN_OUT_TOP_MID, 0, -5);

    //播放模式
    play_mode = lv_dropdown_create(lv_scr_act());
    //添加字体
    // lv_obj_add_style(play_mode, &font_style, 0);
    lv_obj_add_event_cb(play_mode, dd_handler, LV_EVENT_ALL, "play_mode");
    lv_obj_add_style(play_mode, &font_style, 0);
    // lv_obj_set_style_text_font(play_mode, &lv_font_simsun_16_cjk, 0);
    lv_dropdown_set_options_static(play_mode, opts1);
    lv_obj_set_size(play_mode, 100, 40);
    //位置
    lv_obj_align(play_mode, LV_ALIGN_BOTTOM_LEFT, 40, -30);
    lv_dropdown_set_dir(play_mode, LV_DIR_BOTTOM);
    lv_obj_t *mode_label = lv_label_create(lv_scr_act());
    lv_obj_add_style(mode_label, &font_style, 0);
    lv_label_set_text(mode_label, "播放模式");
    lv_obj_align_to(mode_label, play_mode, LV_ALIGN_OUT_TOP_MID, 0, -5);
}

//滑动条事件
static void slider_event_cb(lv_event_t *e)
{
    if (!start)
        return;
    //获取传递的参数
    char *msg = lv_event_get_user_data(e);
    // puts(msg);
    if (strcmp(msg, "volume") == 0)
    {
        lv_obj_t *slider = lv_event_get_target(e); //获取事件对象
        char buf[8];
        int volume = (int)lv_slider_get_value(slider); //获取值
        lv_snprintf(buf, sizeof(buf), "%d", volume);
        lv_label_set_text(volume_label, buf); //更新音量标签值
        lv_obj_align_to(volume_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

        usleep(100); //修改音量值
        char cmd[1024] = {0};
        sprintf(cmd, "volume %d 1\n", volume);
        write(fd_mplayer, cmd, strlen(cmd));
    }
    if (strcmp(msg, "play") == 0)
    {
        puts("松开了");

        if (start) //启动就先暂停
        {
            pthread_kill(tid_read, 10); //暂停读mplayer返回内容
            system("killall -19 mplayer");
        }

        int rate = (int)lv_slider_get_value(play_slider); //获取值
        float new_time = time_length * rate * 0.01;
        int seek_time = new_time - time_pos;
        char cmd[1024] = {0};
        sprintf(cmd, "seek %d\n", seek_time);
        // puts(cmd);
        write(fd_mplayer, cmd, strlen(cmd));

        system("killall -18 mplayer"); //播放器继续
        pthread_cond_signal(&cond);    //唤醒读写mplayer
        usleep(1000);
        pthread_cond_signal(&cond); //防止被再次暂停
    }
}
//显示滑动条
static void show_slider(void)
{ //音量
    volume_slider = lv_slider_create(lv_scr_act());
    lv_obj_align(volume_slider, LV_ALIGN_RIGHT_MID, -20, 0);                               //位置
    lv_obj_add_event_cb(volume_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, "volume"); //事件
    lv_obj_set_size(volume_slider, 18, 180);                                               //大小
    lv_slider_set_value(volume_slider, 100, LV_ANIM_OFF);                                  //初始值
    /*标签 音量大小 */
    volume_label = lv_label_create(lv_scr_act());
    lv_label_set_text(volume_label, "100");
    lv_obj_set_style_text_font(volume_label, &lv_font_montserrat_18, 0);
    lv_obj_align_to(volume_label, volume_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_obj_add_style(label, &font_style, 0);
    lv_label_set_text(label, "音量");
    lv_obj_align_to(label, volume_slider, LV_ALIGN_OUT_TOP_MID, 0, -20);

    //播放进度条
    play_slider = lv_slider_create(lv_scr_act());
    lv_obj_align(play_slider, LV_ALIGN_CENTER, 0, 120);                           //位置
    lv_obj_set_width(play_slider, 400);                                           //宽度
    lv_obj_add_event_cb(play_slider, slider_event_cb, LV_EVENT_RELEASED, "play"); //事件
    lv_slider_set_value(play_slider, 0, LV_ANIM_OFF);                             //初始值
    lv_slider_set_range(play_slider, 0, 100);                                     //范围

    lv_obj_set_style_bg_opa(play_slider, LV_OPA_TRANSP, 196608);
    lv_obj_set_style_pad_all(play_slider, 20, 196608);
    lv_obj_set_style_bg_grad_dir(play_slider, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(play_slider, lv_color_hex(0x569af8), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(play_slider, lv_color_hex(0xa666f1), LV_PART_INDICATOR);

    /*在滑条下创建标签*/
    time_length_label = lv_label_create(lv_scr_act());
    lv_label_set_text(time_length_label, "0:00");
    lv_obj_set_style_text_color(time_length_label, lv_color_hex(0x8a86b8), 0);
    lv_obj_align_to(time_length_label, play_slider, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    time_pos_label = lv_label_create(lv_scr_act());
    lv_label_set_text(time_pos_label, "0:00");
    lv_obj_align_to(time_pos_label, play_slider, LV_ALIGN_OUT_LEFT_MID, -10, 0);
}

void paly_music(int num)
{
    char data_music_bin[64] = {0};
    int pipe_fd = open(MUSIC_FIFO_PIPE, O_RDWR);
    if (pipe_fd == -1)
    {
        printf("open出错了\n");
        return;
    }
    memset(&data_music_bin, 0, sizeof(data_music_bin));
    read(pipe_fd, data_music_bin, 32);
    printf("text:%s\n", data_music_bin);
    close(pipe_fd);
    strcpy(music_path[music_num], data_music_bin);
}

//显示界面
static void display_interface()
{
    //显示歌单列表
    show_list();
    //显示按钮
    show_button();
    //显示下拉列表
    show_drop_down();
    //显示滑动条
    show_slider();
}

//音乐播放器
void music_player()
{
    if (access(FIFO_PIPE, F_OK) != 0)
    {
        umask(0000);
        mkfifo(FIFO_PIPE, 0777);
        printf("正在建立管道文件\n");
    }
    //打开管道文件
    fd_mplayer = open(FIFO_PIPE, O_RDWR);
    if (fd_mplayer == -1)
    {
        perror("open");
        exit(0);
    }
    //检索本地歌单
    get_music_path();
    //初始化字体
    init_font_style();
    //初始化图片父对象
    init_pic_parent_obj();
    init_title_obj();
    //显示界面
    display_interface();
}