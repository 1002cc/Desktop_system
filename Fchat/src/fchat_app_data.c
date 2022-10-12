#include "fchat_app_data.h"
#define CHAT_FIFO_PIPE "/Desktop/chat_pipe"
#define TEST printf("正在执行%s函数第%d行代码\n", __FILE__, __LINE__)
extern pthread_mutex_t mutex_lv; // lvgl线程锁

void get_user_data(listnode *head)
{
    //创建套接字
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        perror("socket");
        return -1;
    }
    Fchat_Init();

    // struct sockaddr *addr; //网络信息结构体
    //绑定 ---设置服务器ip和socket进程的端口号  一般网络通信都会有端口号
    struct sockaddr_in server_info;
    server_info.sin_family = AF_INET;
    server_info.sin_port = htons(Port_Number); // 1023~65535
    server_info.sin_addr.s_addr = inet_addr(IP_ADDR);
    if (connect(client_fd, &server_info, sizeof(server_info)) != 0)
    {
        perror("bind");
        return -1;
    }
    
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

// Fchat聊天软件
void Fchat_Init()
{
    listnode *head=list_init();
    //用户加载
    get_user_data();
    //初始化字体
    init_font_style();
    //显示界面
    display_interface();
}