#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>  //malloc
#include <errno.h>   //perror
#include <stdbool.h> //bool
#ifndef DATATYPE_W
#define DATATYPE_W int
#endif

typedef DATATYPE_W datatype_weather;

typedef struct node
{
    datatype_weather data;
    struct node *next;
    struct node *prev;
} listnode_weather;

static listnode_weather *list_init(void)
{
    listnode_weather *head = malloc(sizeof(listnode_weather));
    if (head == NULL)
    {
        perror("malloc");
        return NULL;
    }

    head->next = head;
    head->prev = head;

    return head;
}

static void __NodeInsert(listnode_weather *first, listnode_weather *second, listnode_weather *newnode)
{
    first->next = newnode;
    newnode->next = second;

    second->prev = newnode;
    newnode->prev = first;
}

//头插尾插  0头插  1尾插
static int list_head_tail_insert(listnode_weather *head, datatype_weather data, int select)
{
    //给新节点分配空间
    listnode_weather *newnode = (listnode_weather *)malloc(sizeof(listnode_weather));
    if (newnode == NULL)
    {
        perror("malloc");
        return -1;
    }
    //处理数据域
    newnode->data = data;

    //处理指针域
    if (select == 0)
        __NodeInsert(head, head->next, newnode); //头插
    else if (select == 1)
        __NodeInsert(head->prev, head, newnode); //尾插

    return 0;
}

//查找
static listnode_weather *list_findnode(listnode_weather *head, bool (*isnode)(datatype_weather, datatype_weather), datatype_weather findcase)
{
    listnode_weather *p = head->next;
    while (p != head)
    {
        if (isnode(p->data, findcase))
        {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

static void __NodeDel(listnode_weather *first, listnode_weather *second)
{
    first->next = second;
    second->prev = first;
}

//删除节点
static int list_del(listnode_weather *head, bool (*isnode)(datatype_weather, datatype_weather), datatype_weather findcase)
{
    listnode_weather *node_del = list_findnode(head, isnode, findcase);
    if (node_del == NULL)
    {
        printf("没有找到节点\n");
        return -1;
    }

    printf("找到节点\n");

    __NodeDel(node_del->prev, node_del->next);

    free(node_del);
}

//遍历打印
static void list_print(listnode_weather *head, void (*print)(datatype_weather))
{
    listnode_weather *p = head->next;
    while (p != head)
    {
        print(p->data);
        p = p->next;
    }
}

//销毁链表
static int list_Destruction(listnode_weather *head)
{
    listnode_weather *p = head->next;
    while (p != head)
    {
        __NodeDel(p->prev, p->next);
        free(p);
        p = p->next;
    }
    p->next = NULL;
    return 0;
}

//修改
static int list_Revise(listnode_weather *head, bool (*isnode)(datatype_weather, datatype_weather), datatype_weather findcase, datatype_weather new_data)
{
    listnode_weather *node_revise = list_findnode(head, isnode, findcase);
    if (node_revise == NULL)
    {
        printf("没有找到节点\n");
        return -1;
    }
    node_revise->data = new_data;
}

/* //清空链表
static void clear_LinkList(listnode **head)
{
    if ((*head) == NULL)
    {
        return;
    }
    //清空链表,是不清空头节点的，因此从第一个有数据的节点开始释放
    while ((*head) != NULL)
    {
        //先保住下一个节点的位置
        listnode *nextNode = (*head)->next;
        free(*head);
        (*head) = nextNode;
    }
    //将头结点next指针置空
    if ((*head) != NULL)
    {
        free((*head));
        (*head) = NULL;
    }
}
 */
#endif