#include <stdio.h>
#include <stdlib.h>

typedef int ElemType;
typedef struct Node 
{
    ElemType data;
    struct Node* next;    //注：是 struct Node*, 不是 Node*
}Node;

Node* InitList() 
{        
    Node* head = (Node*)malloc(sizeof(Node));
    if (head == NULL) 
    {
        printf("头节点分配失败\n");
        return NULL;
    }
    head->next = NULL;
    return head;
}

int listlen(Node* list)
{
    Node* current = list;
    int len = 0;
    while (current->next != NULL) 
    {
        len++;
        current = current->next;
    }
    return len;
}

void ErgodicList(Node* list) 
{        
    Node* current = list;
    while (current ->next != NULL) 
    {
        printf("%d -> ", current->next->data);
        current = current->next;
    }
    printf("NULL\n");
}

void FreeList(Node* list) 
{
    Node* current = list;        //注：释放时不释放头节点，头节点不是第一个节点
    while (current->next != NULL) 
    {
        Node* temp = current->next;    
        current->next = temp->next;        //不用管，current肯定不是空指针（在这段代码里current一直等于list）
        free(temp);
    }
}

Node* get_tail(Node* list) 
{
    Node* tail = list;
    while (tail->next != NULL) 
    {    //while(tail != NULL)会导致tail最终为NULL,而不是指向NULL的尾节点
        tail = tail->next;
    }
    return tail;
}

Node* InsertTail(Node* tail, ElemType data) 
{
    Node* newTail = (Node*)malloc(sizeof(ElemType));
    if (newTail == NULL) 
    {
        printf("新尾节点分配失败\n");
        return NULL;
    }
    newTail->data = data;
    newTail->next = NULL;
    tail->next = newTail;
    return newTail;
}

void reverseList(Node* list) 
{
    if (list->next == NULL || list->next->next == NULL)
        return;
    int len = listlen(list);
    Node** help = (Node**)malloc(sizeof(Node*) * (len + 1));
    if (help == NULL) 
    {
        printf("辅助数组分配失败\n");
        return;
    }
    Node* current = list;
    for (int i = 1; i <= len; i++) 
    {
        help[i] = current->next;
        current = current->next;
    }
    current = list;
    for (int i = len; i >= 1; i--) 
    {
        current->next = help[i];
        current = current->next;
    }
    current->next = NULL;
    free(help);
}

int main() 
{
    Node* list = InitList();

    Node* tail = get_tail(list);

    tail = InsertTail(tail, 11);
    tail = InsertTail(tail, 22);
    tail = InsertTail(tail, 33);
    ErgodicList(list);

    printf("%d\n", listlen(list));

    reverseList(list);
    ErgodicList(list);

    FreeList(list);

    return 0;
}