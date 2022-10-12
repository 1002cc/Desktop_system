#include "game_data.h"

static unsigned long *p_mmap;
static int fd_event0 = -1;
static int fildes;

int open_init(void)
{
    fd_event0 = open("/dev/input/event0", O_RDWR);
    if (-1 == fd_event0)
    {
        perror("打开触摸屏失败");
        return -1;
    }
    fildes = open("/dev/fb0", O_RDWR);
    p_mmap = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fildes, 0);

    return 0;
}

int close_ts(void)
{
    int result = 0;
    munmap(p_mmap, 800 * 480 * 4);
    result = close(fd_event0);
    if (-1 == result)
    {
        printf("同一文件重复执行关闭操作会失败返回，不会有其他副作用\n");
        return -1;
    }
    close(fildes);
    return 0;
}

//棋盘落子,让其中落地交点
void game_control(int x, int y)
{

    int xx, yy;
    if (x >= 165 && x <= 615 && y >= 15 && y < 465)
    {
        //处理坐标
        xx = x % 30; //求余
        yy = y % 30;
        if (xx > 15)
        {
            xx = x / 30 + 1;
        }
        else
        {
            xx = x / 30;
        }
        if (yy > 15)
        {
            yy = y / 30 + 1;
        }
        else
        {
            yy = y / 30;
        }
        x = xx * 30; //获取坐标
        y = yy * 30;
        xx -= 6;
        yy -= 1;
        if (game_buf[yy][xx] == 0)
        {
            if (flag)
            {
                game_buf[yy][xx] = 1;
                Dis_zi(y, x, flag);
            }
            else
            {
                game_buf[yy][xx] = 2;
                Dis_zi(y, x, flag);
            }
            flag = ~flag;
        }
    }
}

//清除二维数组数据,重新游戏
void clear()
{
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            game_buf[i][j] = 0;
        }
    }
}

//黑白子
void Dis_zi(int x, int y, int flag)
{
    int i, j;
    if (flag)
    {
        for (i = 0; i < 480; i++)
        {
            for (j = 0; j < 800; j++)
            {
                if ((i - x) * (i - x) + (j - y) * (j - y) <= 150)
                {
                    Display(0x00ffffff, j, i);
                }
            }
        }
    }
    else
    {
        for (i = 0; i < 480; i++)
        {
            for (j = 0; j < 800; j++)
            {
                if ((i - x) * (i - x) + (j - y) * (j - y) <= 150)
                {
                    Display(0x00, j, i);
                }
            }
        }
    }
}

//判断游戏是否输赢
int game_over()
{
    int h = 1, w = 1, x = 1, y = 1;
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (game_buf[i][j] != 0)
            {
                while (h != 5)
                {
                    if ((i + h) < 15 && game_buf[i][j] == game_buf[i + h][j]) //竖着
                        h++;
                    else
                    {
                        h = 1;
                        break;
                    }
                }
                while (w != 5)
                {
                    if ((j + w) < 15 && game_buf[i][j] == game_buf[i][j + w]) //横着
                        w++;
                    else
                    {
                        w = 1;
                        break;
                    }
                }
                while (x != 5)
                {
                    if ((j + x) < 15 && (i + x) < 15 && game_buf[i][j] == game_buf[i + x][j + x]) //斜着
                        x++;
                    else
                    {
                        x = 1;
                        break;
                    }
                }
                while (y != 5)
                {
                    if ((i + y) < 15 && (j - y) < 15 && game_buf[i][j] == game_buf[i + y][j - y]) //斜着
                        y++;
                    else
                    {
                        y = 1;
                        break;
                    }
                }
                if (h == 5 || w == 5 || x == 5 || y == 5)
                {
                    printf("%d:%d:%d:%d", h, w, x, y);
                    if (game_buf[i][j] == 2)
                    {
                        printf("黑子赢了\n");
                        printf("玩家1赢了\n");
                    }
                    else
                    {
                        printf("白子赢了\n");
                        printf("玩家2赢了\n");
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}

//画棋子
void Display(int color, int x, int y)
{
    if (x >= 0 && x <= 800 && y >= 0 && y <= 480)
    {
        *(p_mmap + y * 800 + x) = color;
    }
}

//棋盘15*15
void *Dis_pan(void *arg)
{
    while (1)
    {
        for (int i = 30; i <= 450; i++)
        {
            for (int j = 180; j <= 600; j++)
            {
                if (i % 30 == 0 || j % 30 == 0)
                    Display(0x00, j, i);
            }
        }
        if (r == 1)
        {
            break;
        }
    }
}

/*
XY_DATA_T xy;

int read_ts(int *p_get_x, int *p_get_y)
{
    ssize_t read_count = 0;
    struct input_event ts_info;
    while (1)
    {
        read_count = read(fd_event0, &ts_info, sizeof(ts_info));
        if (-1 == read_count)
        {
            perror("读取触摸屏信息失败");
            return -1;
        }
        if (ts_info.type == 3 && ts_info.code == 0)
        {
            *p_get_x = ts_info.value * 800.0 / 1024.0;
        }
        if (ts_info.type == 3 && ts_info.code == 1)
        {
            *p_get_y = ts_info.value * 480.0 / 600.0;
        }
        if (ts_info.type == 1 && ts_info.code == 330 && ts_info.value == 0)
        {
            printf("手指松开了触摸屏的坐标:(x,y):(%d,%d)\n", *p_get_x, *p_get_y);
            break;
        }
    }
    return 0;
}
 */