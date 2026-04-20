//二叉树
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>


#define NAME_MAX 20

typedef struct students
{
    uint32_t num;                                         /*学号*/
    char name[NAME_MAX];                                  /*姓名*/
} students;

typedef struct TreeNode 
{
    students data;
    struct TreeNode *_left_treeNode;
    struct TreeNode *_right_treeNode;
}TreeNode;

typedef struct TreeNode_tree_Queue
{
    TreeNode* data;
    struct TreeNode_tree_Queue* next;
}Queue;

typedef struct TreeNode_Queue
{
    Queue* front;//队头
    Queue* Rear;//队尾
}Tree_Queue;

void initQueue(Tree_Queue* tree_Queue)
{
    //初始化队列
    tree_Queue->front = NULL;
    tree_Queue->Rear = NULL;
}
TreeNode* tree_malloc()
{
    TreeNode* tree = (TreeNode*)malloc(sizeof(TreeNode));
    tree->_left_treeNode = NULL;
    tree->_right_treeNode = NULL;
    return tree;
}

void enqueue(Tree_Queue* tree_Queue, TreeNode* tree)
//入队
{
    //队尾入队
    if(tree_Queue == NULL)
    {
        return;
    }
    Queue* new_node = (Queue*)malloc(sizeof(Queue));
    new_node->data = tree;
    new_node->next = NULL;
    if(tree_Queue->Rear == NULL)
    {
        tree_Queue->front = new_node;
    }
    else
    {
        tree_Queue->Rear->next = new_node;
        tree_Queue->Rear = new_node;
    }
}
// 出队操作
TreeNode* dequeue(Tree_Queue* queue) 
{
    //队头出队
    if(queue == NULL)
    {
        return NULL;
    }
    if(queue->front == NULL)
    {
        return NULL;
    }
    Queue* temp = queue->front;
    TreeNode* tree = temp->data;
    queue->front = temp->next;
    if(queue->front == NULL)
    {
        queue->Rear = NULL;
    }
    return tree;
}

//计算二叉树的深度
int32_t deepness(TreeNode* tree)
{
    if (tree == NULL)
    {
        return 0;
    }
    else
    {
        int32_t left_deep = deepness(tree->_left_treeNode);
        int32_t right_deep = deepness(tree->_right_treeNode);
        return (left_deep > right_deep) ? (left_deep + 1) : (right_deep + 1);
    }
}
//先序遍历二叉树
void Precedent(TreeNode* tree)
{
    if(tree == NULL)
    {
        return;
    }
    else
    {
        printf("%d ", tree->data.num);
        Precedent(tree->_left_treeNode);
        Precedent(tree->_right_treeNode);
    }
}

//中序遍历二叉树
void Medium_order(TreeNode* tree)
{
    if(tree == NULL)
    {
        return;
    }
    else
    {
        Medium_order(tree->_left_treeNode);
        printf("%d ", tree->data.num);
        Medium_order(tree->_right_treeNode);
    }
}
//后序遍历二叉树
void Postscript(TreeNode* tree)
{
    if(tree == NULL)
    {
        return;
    }
    else
    {
        Postscript(tree->_left_treeNode);
        Postscript(tree->_right_treeNode);
        printf("%d ", tree->data.num);
    }
}

//计算二叉树的节点个数
uint64_t TreeNode_number(TreeNode* tree)
{
    if(tree == NULL)
    {
        return 0;
    }
    else
    {
        return TreeNode_number(tree->_left_treeNode) + TreeNode_number(tree->_right_treeNode) + 1;
    }
}
//计算二叉树的叶子节点个数

uint64_t TreeNode_leaf_number(TreeNode* tree)
{
    if(tree == NULL)
    {
        return 0;
    }
    else
    {
        if(tree->_left_treeNode == NULL && tree->_right_treeNode == NULL)
        {
            return 1;
        }
        else
        {
            return TreeNode_leaf_number(tree->_left_treeNode) + TreeNode_leaf_number(tree->_right_treeNode);
        }
    }
}

//计算二叉树的第k层节点个数

uint32_t TreeNode_Unknown_number(TreeNode* tree, uint32_t k)
{
    if(k == 0)
    {
        return 1;
    }
    if(tree == NULL)
    {
        return 0;
    }
    else
    {
        if(k == 1)
        {
            return 1;
        }
        else
        {
            return TreeNode_Unknown_number(tree->_left_treeNode, k - 1) + TreeNode_Unknown_number(tree->_right_treeNode, k - 1);
        }
    }
}
//层序遍历二叉树
void Layer_order(TreeNode* tree)
{
    if(tree == NULL)
    {
        return;
    }
    if(tree->_left_treeNode == NULL && tree->_right_treeNode == NULL)
    {
        printf("%d \n", tree->data.num);
        return;
    }
    else
    {
        Tree_Queue tree_Queue;
        initQueue(&tree_Queue);
        enqueue(&tree_Queue, tree);
        while(tree_Queue.front != NULL)
        {
            TreeNode* temp = dequeue(&tree_Queue);
            printf("%d ", temp->data.num);
            if(temp->_left_treeNode != NULL)
            {
                enqueue(&tree_Queue, temp->_left_treeNode);
            }
            if(temp->_right_treeNode != NULL)
            {
                enqueue(&tree_Queue, temp->_right_treeNode);
            }
            free(temp);
        } 
        printf("\n");
    }
}

//判断是不是完全二叉树
BOOL TreeNode_complete(TreeNode* tree)
{
    if(tree == NULL)
    {
        return true;
    }
    else
    {
        if(tree->_left_treeNode == NULL && tree->_right_treeNode == NULL)
        {
            return true;
        }
        else
        {
            Tree_Queue tree_Queue;
            initQueue(&tree_Queue);
            enqueue(&tree_Queue, tree);
            while(tree_Queue.front != NULL)
            {
                TreeNode* temp = dequeue(&tree_Queue);
                if(temp->_left_treeNode != NULL)
                {
                    enqueue(&tree_Queue, temp->_left_treeNode);
                }
                if(temp->_right_treeNode != NULL)
                {
                    enqueue(&tree_Queue, temp->_right_treeNode);
                }
                if(temp->_left_treeNode == NULL || temp->_right_treeNode == NULL)
                {
                    while(tree_Queue.front != NULL)
                    {
                        TreeNode* temp = dequeue(&tree_Queue);
                        //如果剩下来的队列里有不为空的节点，则不是完全二叉树
                        if(temp->_left_treeNode != NULL || temp->_right_treeNode != NULL)
                        {
                            return false;
                        }
                    }
                }
            }
            return true;
        }
       
    }
}

//判断是不是满二叉树
BOOL TreeNode_full(TreeNode* tree)
{
    if(tree == NULL)
    {
        return true;
    }
    else
    {
        if(tree->_left_treeNode == NULL && tree->_right_treeNode == NULL)
        {
            return true;
        }
        else if(tree->_left_treeNode != NULL && tree->_right_treeNode != NULL)
        {
            return TreeNode_full(tree->_left_treeNode) && TreeNode_full(tree->_right_treeNode);
        }
        else
        {
            return false;
        }
    }
}

//计算二叉树的最长路径长度
uint64_t TreeNode_longest(TreeNode* tree)
{
    if(tree == NULL)
    {
        return 0;
    }
    else
    {
        uint64_t left_longest = TreeNode_longest(tree->_left_treeNode);
        uint64_t right_longest = TreeNode_longest(tree->_right_treeNode);
        return (left_longest > right_longest) ? (left_longest + 1) : (right_longest + 1);
    }
}

//镜像二叉树
void TreeNode_mirror(TreeNode* tree)
{
    if(tree == NULL)
    {
        return;
    }
    else
    {
       TreeNode* temp = tree->_left_treeNode;
       tree->_left_treeNode = tree->_right_treeNode;
       tree->_right_treeNode = temp;
       TreeNode_mirror(tree->_left_treeNode);
       TreeNode_mirror(tree->_right_treeNode);
    }
    printf("反转完成！");
}

//判断是不是一个二叉树是不是另一个二叉树的子树
BOOL TreeNode_is_sub(TreeNode* tree, TreeNode* sub_tree)
{
    //只适合浅度遍历
    if(tree == NULL&& sub_tree == NULL)
    {
        return true;
    }
    if(tree == NULL || sub_tree == NULL)
    {
        return false;
    }
    else
    {
        if(tree->data.num == sub_tree->data.num)
        {
            return TreeNode_is_sub(tree->_left_treeNode, sub_tree->_left_treeNode) && TreeNode_is_sub(tree->_right_treeNode, sub_tree->_right_treeNode);
        }
        else
        {
            return TreeNode_is_sub(tree->_left_treeNode, sub_tree) && TreeNode_is_sub(tree->_right_treeNode, sub_tree);
        }
    }
}

//二叉树的合并
void TreeNode_merge(TreeNode* tree, TreeNode* sub_tree)
{
    if(tree == NULL || sub_tree == NULL)
    {
        return;
    }
    else
    {
        TreeNode_merge(tree->_left_treeNode, sub_tree->_left_treeNode);
        TreeNode_merge(tree->_right_treeNode, sub_tree->_right_treeNode); 
        if(tree->data.num == sub_tree->data.num && tree->data.name == sub_tree->data.name)
        {
            tree->_left_treeNode = sub_tree->_left_treeNode;
            tree->_right_treeNode = sub_tree->_right_treeNode;
        }
        else
        {
            return;
        }
    }
}

//检查二叉树是不是单值二叉树
BOOL TreeNode_is_single(TreeNode* tree)
{
    if(tree == NULL)
    {
        return true;
    }
    else
    {
        if(tree->_left_treeNode == NULL && tree->_right_treeNode == NULL)
        {
            return true;
        }
        else
        {
            uint32_t num = tree->data.num;
            Tree_Queue tree_temp_Queue;
            initQueue(&tree_temp_Queue);
            enqueue(&tree_temp_Queue, tree);
            while(tree_temp_Queue.front!= NULL)
            {
                TreeNode* temp = dequeue(&tree_temp_Queue);
                if(temp->data.num == num)
                {
                    if(temp->_left_treeNode != NULL)
                    {
                        enqueue(&tree_temp_Queue, temp->_left_treeNode);
                    }
                    if(temp->_right_treeNode != NULL)
                    {
                        enqueue(&tree_temp_Queue, temp->_right_treeNode);
                    }
                }
                else
                {
                    return false;
                }
            }
            return true;
        }
    }
}

int main()
{
    
    TreeNode* A = tree_malloc();
    TreeNode* B = tree_malloc();
    TreeNode* C = tree_malloc();
    TreeNode* D = tree_malloc();
    TreeNode* E = tree_malloc();
    TreeNode* F = tree_malloc();
    TreeNode* G = tree_malloc();
    TreeNode* H = tree_malloc();
    TreeNode* I = tree_malloc();
    A->_left_treeNode = B;
    A->_right_treeNode = C;
    B->_left_treeNode = D;
    B->_right_treeNode = E;
    C->_left_treeNode = F;
    C->_right_treeNode = G;
    D->_left_treeNode = H;
    D->_right_treeNode = I;
    A->data.num = 1;
    B->data.num = 2;
    C->data.num = 3;
    D->data.num = 4;
    E->data.num = 5;
    F->data.num = 6;
    G->data.num = 7;
    H->data.num = 8;
    I->data.num = 9;
    printf("二叉树A的深度是%llu\n", TreeNode_longest(A));
    if(TreeNode_is_single(A))
    {
        printf("二叉树A是单值二叉树\n");
    }
    else
    {
        printf("二叉树A不是单值二叉树\n");
    }
    printf("二叉树A节点个数为%llu\n",TreeNode_number(A));
    if(TreeNode_complete(A))
    {
        printf("二叉树A是完全二叉树\n");
    }
    else    
    {
        printf("二叉树A不是完全二叉树\n");
    }
    printf("二叉树A的叶子节点个数为%llu\n",TreeNode_leaf_number(A));
    printf("先序遍历：->");
    Precedent(A);
    printf("\n");

    printf("中序遍历：->");
    Medium_order(A);
    printf("\n");

    printf("后序遍历：->");
    Postscript(A);
    printf("\n");
    
    printf("二叉树第%d节点数量为%u\n", 3,TreeNode_Unknown_number(A, 3));
    
    free(A);
    free(B);
    free(C);
    free(D);
    free(E);
    free(F);
    free(G);
    free(H);
    free(I);
    return 0;
}