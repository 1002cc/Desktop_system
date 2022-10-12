#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>  //malloc
#include <errno.h>   //perror
#include <stdbool.h> //bool
#ifndef DATATYPE
#define DATATYPE int
#endif

typedef DATATYPE datatype;

typedef struct node
{
    datatype data;
    struct node *next;
    struct node *prev;
} listnode;

static listnode *list_init(void)
{
    listnode *head = malloc(sizeof(listnode));
    if (head == NULL)
    {
        perror("malloc");
        return NULL;
    }

    head->next = head;
    head->prev = head;

    return head;
}

static void __NodeInsert(listnode *first, listnode *second, listnode *newnode)
{
    first->next = newnode;
    newnode->next = second;

    second->prev = newnode;
    newnode->prev = first;
}

//头插尾插  0头插  1尾插
static int list_head_tail_insert(listnode *head, datatype data, int select)
{
    //给新节点分配空间
    listnode *newnode = (listnode *)malloc(sizeof(listnode));
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
static listnode *list_findnode(listnode *head, bool (*isnode)(datatype, datatype), datatype findcase)
{
    listnode *p = head->next;
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

static void __NodeDel(listnode *first, listnode *second)
{
    first->next = second;
    second->prev = first;
}

//删除节点
static int list_del(listnode *head, bool (*isnode)(datatype, datatype), datatype findcase)
{
    listnode *node_del = list_findnode(head, isnode, findcase);
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
static void list_print(listnode *head, void (*print)(datatype))
{
    listnode *p = head->next;
    while (p != head)
    {
        print(p->data);
        p = p->next;
    }
}

//销毁链表
static int list_Destruction(listnode *head)
{
    listnode *p = head->next;
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
static int list_Revise(listnode *head, bool (*isnode)(datatype, datatype), datatype findcase, datatype new_data)
{
    listnode *node_revise = list_findnode(head, isnode, findcase);
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