//队列
#include <stdio.h>
#include <stdlib.h>
typedef struct Queue_Node
{
    char* val;
    struct Queue_Node* next;
}Queue_Node;

typedef struct Queue
{
    Queue_Node* front;//队头
    Queue_Node* rear; //队尾
}Queue;
//初始化
Queue* initialize(Queue* Node)
{
   Node->front = NULL;
   Node->rear = NULL;
   return Node;
}
//入队
void Team (Queue* Queue, char* val)
{
    if(Queue == NULL)
    {
        
        printf("Queue is NULL\n");
        return;
    }
    if(Queue->front == NULL)
    {
        Queue->front = (Queue_Node*)malloc(sizeof(Queue_Node));
        Queue->rear = Queue->front;
        Queue->front->val = val;
        Queue->front->next = NULL;
        return;
    }
    Queue_Node* new_Node = (Queue_Node*)malloc(sizeof(Queue_Node));
    new_Node->val = val;
    Queue->rear->next = new_Node; //  将当前后端节点的下一个节点指向新节点
    Queue->rear = new_Node; //  更新后端节点为新节点
}

//出队
Queue_Node* OutTeam(Queue* Queue)
//可以不返回，直接在函数中free
{
    if(Queue == NULL)
    {
        printf("Queue is NULL\n");
        return NULL;
    }
    if(Queue->front == NULL)
    {
        printf("Queue is empty\n");
        return NULL;
    }
    Queue_Node* temp = Queue->front; //  保存当前队首节点
    Queue->front = temp->next; //  将队首指针指向下一个节点
    if(Queue->front == NULL)
    {
        Queue->rear = NULL;
    }
    return temp;
}
int main()
{
    Queue Queue ;
    initialize(&Queue);
    Team(&Queue, "hello");
    Team(&Queue, "world");
    Queue_Node* temp = OutTeam(&Queue);
    Queue_Node* temp2 = OutTeam(&Queue);
    printf("%s\n", temp->val);
    printf("%s\n", temp2->val);
    free(temp);
    free(temp2);
    return 0;
}