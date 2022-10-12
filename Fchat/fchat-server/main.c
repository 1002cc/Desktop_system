#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <arpa/inet.h> //tcp网络信息结构体  struct sockaddr_in

//#include "fchat_app_data.h"

/*
设计方案：
        建立服务器：管理用户信息

*/

typedef struct user_data
{
    char user_ip[64];
    int socket;
} USER_DATA_T; //记录用户

#define DATATYPE USER_DATA_T

#include "list.h"

int nclient; //记录当前用户数量
CRITICAL_SECTION cs;

#define IP_ADDR "192.168.61.51"
#define Port_Number 2022

listnode *head;
//处理用户状态信息
void processMag(int clientsocket, char message[], int msglen)
{
    int mtype = message[0] - '0'; // 0:公聊 1：私聊 2：上线  3：离线
    char ulist[10 * 81];

    char from[81]; //信息发送方
    char to[81];   //信息接收方
    char who[81];  //上线或离线方
    char msg[81];  //信息
    int nquit = 0; //离线人数
    int ret;
    int K = 0;
    int i;
    switch (mtype)
    {
    case 0: //处理公聊
        listnode *p = head->next;
        while (p != head)
        {
            ret = send(p->data.socket, message, strlen(message) + 1, 0); //发送信息给所有用户
            if (ret == -1)                                               //发送失败则用户离线
            {
                perror("send");
                p->data.socket = 0;
                nquit++; //记录离线人数
            }
            p = p->next;
        }
        break;
    case 1:
        for (i = 2; i < msglen; i++)
        {
            if (message[i] != ':')
            {
                from[k] = message[i];
                k++;
            }
            else
            {
                from[k] = '\0';
                break;
            }
            k = i;
            int t = 0;
            for (i = k + 1; i < msglen; i++)
            {
                if (message[i] != ':')
                {
                    to[t] = message[i];
                    t++;
                }
                else
                {
                    to[t] = '\0';
                    break;
                }
            }
            strcpy(msg, from);
            strcat(msg, ":");
            strcat(msg, message + i + 1);
            listnode *p1 = head->next;
            while (p1 != head)
            {
                if (strcmp(p1->data.user_ip, to) == 0)
                {
                    ret = send(p->data.socket, msg, strlen(msg) + 1, 0);
                    if (rer == -1)
                    {
                        p->data.socket = 0;
                        nquit++; //记录离线人数
                    }
                    break;
                }
            }
        }
        while (nquit)
        {
            listnode *p = head->next;
            while (p != head)
            {
                if (p->data.socket != 0)
                {
                    continue;
                }
                else
                {
                    __NodeDel(p->prev, p->next);
                    free(p);
                    nquit--;
                    bread;
                }
            }
        }
        break;
    case 2: //处理上线信息
        listnode *p = head->next;
        while (p != head)
        {
            if (p->data.socket == clientsocket)
            {
                strcpy(ulist, "4:");
                strcpy(p->data.user_ip, who); //将上线用户ip记录到链表
                listnode *p1 = head->next;
                while (p1 != head)
                {
                    strcat(ulist, p->data.user_ip);
                    strcat(ulist, ":");
                }
                send(p->data.socket, ulist, strlen(ulist) + 1, 0);
            }
            ret = send(p->data.socket, message, strlen(message) + 1, 0);
            if (ret == -1)
            {
                p->data.socket = 0;
                nquit++; //记录离线人数
            }
        }
        break;
    case 3: //处理离线
        listnode *p = head->next;
        while (p != head)
        {
            if (p->data.socket != clientsocket)
            {
                ret = send(p->data.socket, message, strlen(message) + 1, 0);
                if (ret == -1)
                {
                    p->data.socket = 0;
                    nquit++; //记录离线人数
                }
            }
        }
        break;
    }
}
//建立信息的线程
void receive_msg_thread(void *arg)
{
    int client_fd = (int)arg;
    char message[1024];
    int msglen; //接收返回字节数
    while (1)
    {
        //清空数组
        memset(message, 0, sizeof(message));
        msglen = recv(client_fd, message, sizeof(message), 0);
        listnode *p = head->next;
        if (msglen == -1)
        {
            while (p != head)
            {
                if (client_fd == p->data.socket)
                {
                    printf("用户IP:%s已经离线!", p->data.user_ip);
                    strcat(message, "3:");
                    strcat(message, p->data.user_ip);
                    nclient--;
                    processMag(client_fd, message, strlen(message) + 1);
                    break;
                }
            }
            close(client_fd);
            return;
        }
        message[msglen] = '\0';
        printf("%s\n", message);
        processMag(client_fd, message, msglen);
    }
}

int main(int argc, char *argv[])
{
    //定义一条链表

    //初始化链表
    head = list_init();

    //创建套接字
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket");
        return -1;
    }
    // struct sockaddr *addr; //网络信息结构体
    //绑定 ---设置服务器ip和socket进程的端口号  一般网络通信都会有端口号
    struct sockaddr_in host_info;
    host_info.sin_family = AF_INET;
    host_info.sin_port = htons(Port_Number); // 1023~65535
    host_info.sin_addr.s_addr = inet_addr(IP_ADDR);
    if (-1 == bind(server_fd, (struct sockaddr *)&host_info, sizeof(struct sockaddr)))
    {
        perror("bind");
        return -1;
    }

    if (-1 == listen(server_fd, 1))
    {
        perror("listen");
        return -1;
    }
    printf("监听成功\n");
    //等待连接 -连接成功后，获取信息
    struct sockaddr_in clientinfo;
    socklen_t addrlen = sizeof(clientinfo);
    int client_fd; //
    datatype data;
    //服务器循环
    while (1)
    {
        client_fd = accept(server_fd, (struct sockaddr *)&clientinfo, &addrlen);
        if (-1 == client_fd)
        {
            perror("accept");

            return -1;
        }
        printf("当前连接的客户端IP:%s 端口号:%d\n", inet_ntoa(clientinfo.sin_addr), ntohs(clientinfo.sin_port));

        //记录用户数量
        nclient++;
        //记录信息到链表中
        strcpy(data.user_ip, inet_ntoa(clientinfo.sin_addr));
        data.socket = client_fd;
        list_head_tail_insert(head, data, 1);

        //清理数据
        client_fd = 0;
        memset(data, 0, sizeof(data));
    }
}