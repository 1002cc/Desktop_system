#include "app_data.h"

#define ICON_DIR "./icon/"
#define TEST printf("正在执行%s文件第%d行代码\n", __FILE__, __LINE__)

//查找链表是否有该数据(用于链表查询调用)
bool isname(datatype arg, datatype data)
{
    if (strcmp(arg.icon_name, data.icon_name) == 0)
        return true;
    else
        return false;
}

/*
······  初始化数据(重新加载数据：安装软件使用)
定义一个结构体存储数据----->然后增加判断是否链表中该数据------>插入数据到链表中
*/
void File_Data_Init(listnode *head)
{
    datatype data_user;
    char *pro_name = NULL;
    DIR *dp = opendir(ICON_DIR);
    struct dirent *ptr = NULL;
    while ((ptr = readdir(dp)) != NULL)
    {
        if (strcmp(".", ptr->d_name) == 0 || strcmp("..", ptr->d_name) == 0 || strcmp("Desktop.bg.png", ptr->d_name) == 0)
            continue;
        //保存数据到结构体中
        strcpy(data_user.icon_name, ptr->d_name); //图标名字
        pro_name = strtok(ptr->d_name, ".");
        strcpy(data_user.process_name, pro_name); //程序名字
        //判断链表是否有该数据
        // list_findnode返回值为NULL--->没有找到数据
        //找到则返回该链表节点
        if (list_findnode(head, isname, data_user) == NULL)
        {
            printf("正在添加软件:%s\n", data_user.process_name);
            data_user.program_flag = APP_CLOSE;        //运行状态初始化为APP_CLOSE:1
            list_head_tail_insert(head, data_user, 1); //尾插数据
        }
        //清空数据,循环使用
        pro_name = NULL;
        memset(&data_user, 0, sizeof(data_user));
    }
    closedir(dp);
}

/*
    建立进程
用户点击软件图标
判断软件运行状态：
                APP_CLOSE    软件处于关闭状态---->创建新进程
                APP_Backstage 软件处于后台运行状态-->则切换软件运行状态,并发送信号，重新显示软件
创建进程时:记录软件数据 pid 设置运行状态为APP_RUN
*/
void fork_newpid(listnode *p)
{
    char process_bin[64];

    if (p->data.program_flag == APP_Backstage || p->data.program_flag == APP_RUN)
    {
        printf("该程序为正在运行的后台程序\n");
        p->data.program_flag = APP_RUN;
        kill(p->data.program_pid, SIGRTMIN);
    }
    else
    {
        pid_t app_pid = fork();

        if (app_pid == -1)
        {
            perror("fork");
        }
        else if (app_pid == 0) //子进程
        {
            sprintf(process_bin, "/Desktop/app/%s", p->data.process_name);
            execl(process_bin, p->data.process_name, NULL);
        }
        else if (app_pid > 0) //父进程
        {
            p->data.program_pid = app_pid;
            p->data.program_flag = APP_RUN;
        }
    }
}
