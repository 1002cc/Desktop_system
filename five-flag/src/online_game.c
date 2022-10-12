#include "online_game.h"

XY_DATA_T xy_online;

void get_online_user()
{
    int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd == -1)
    {
        perror("socket");
        return -1;
    }
    printf("创建套接字成功\n");

    // 2、设置--绑定上面的套接字一个网络信息---IP+端口
    struct sockaddr_in udpaddr;
    udpaddr.sin_family = AF_INET;
    udpaddr.sin_port = htons(8889);
    udpaddr.sin_addr.s_addr = htonl(INADDR_ANY); // inet_addr("192.168.61.3");//
    if (-1 == bind(udp_fd, (struct sockaddr *)&udpaddr, sizeof(udpaddr)))
    {
        perror("bind");
        return -1;
    }
    printf("绑定信息成功\n");
}

void *udp_msgrecv(void *arg)
{
    int udp_fd = (int)arg;
    char recvbuff[64];
    while (1)
    {
        memset(recvbuff, 0, sizeof(recvbuff));
        recvfrom(udp_fd, recvbuff, sizeof(recvbuff), 0, NULL, NULL); // read+accept
        printf("收到的消息:%s\n", recvbuff);
        xy_online.x = strtok(recvbuff, ":");
        xy_online.y = strtok(NULL, ":");
    }
}

void user_online(XY_DATA_T xy)
{

    pthread_t pthinfo;
    pthread_create(&pthinfo, NULL, udp_msgrecv, (void *)udp_fd);

    char sendbuff[64];
    sprintf(sendbuff, "%d:%d:", xy.x, xy.y);
    //修改为对方主机进程的IP和端口号8889
    udpaddr.sin_family = AF_INET;
    udpaddr.sin_port = htons(8888);
    udpaddr.sin_addr.s_addr = inet_addr("192.168.61.52");
    while (1)
    {
        strcpy(sendbuff, sendbuff);
        sendto(udp_fd, sendbuff, strlen(sendbuff), 0, &udpaddr, sizeof(udpaddr)); // write+bind
        memset(sendbuff, 0, sizeof(sendbuff));
    }
}