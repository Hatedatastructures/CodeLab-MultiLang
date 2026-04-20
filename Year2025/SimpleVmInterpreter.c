#include <stdio.h>
#include <stdlib.h>

// 二叉树节点定义
struct TreeNode 
{
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};

int** levelOrder(struct TreeNode* root, int* returnSize, int** returnColumnSizes) 
//二叉树层序遍历
{
    if (!root) 
    {
        
        *returnSize = 0;
        *returnColumnSizes = NULL;
        return NULL;
    }

    // 初始化队列（动态数组实现）
    int queueCapacity = 256;
    struct TreeNode** queue = malloc(queueCapacity * sizeof(struct TreeNode*));
    int front = 0, rear = 0;
    queue[rear++] = root;

    // 初始化结果存储
    int resultCapacity = 128;
    int** result = malloc(resultCapacity * sizeof(int*));
    int* columnSizes = malloc(resultCapacity * sizeof(int));
    int level = 0;

    while (front < rear) 
    {
        int levelSize = rear - front;
        
        // 动态扩容结果数组
        if (level >= resultCapacity) 
        {
            resultCapacity *= 2;
            result = realloc(result, resultCapacity * sizeof(int*));
            columnSizes = realloc(columnSizes, resultCapacity * sizeof(int));
        }

        // 创建当前层数组
        int* currentLevel = malloc(levelSize * sizeof(int));
        
        for (int i = 0; i < levelSize; i++) 
        {
            struct TreeNode* node = queue[front++];
            currentLevel[i] = node->val;

            // 动态扩容队列
            if (rear + 2 >= queueCapacity) 
            { // +2预留左右子节点空间
                queueCapacity *= 2;
                queue = realloc(queue, queueCapacity * sizeof(struct TreeNode*));
            }

            if (node->left) queue[rear++] = node->left;
            if (node->right) queue[rear++] = node->right;
        }

        result[level] = currentLevel;
        columnSizes[level++] = levelSize;
    }

    // 释放队列内存
    free(queue);

    // 调整结果数组到实际大小
    result = realloc(result, level * sizeof(int*));
    *returnColumnSizes = realloc(columnSizes, level * sizeof(int));
    *returnSize = level;

    return result;
}

// 测试用例辅助函数
struct TreeNode* createNode(int val) 
{
    struct TreeNode* node = malloc(sizeof(struct TreeNode));
    node->val = val;
    node->left = node->right = NULL;
    return node;
}

void freeTree(struct TreeNode* root) 
{
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int main() 
{
    // 构建测试树
    //       3
    //      / /
    //     9 20
    //      / /
    //     15 7
    struct TreeNode* root = createNode(3);
    root->left = createNode(9);
    root->right = createNode(20);
    root->right->left = createNode(15);
    root->right->right = createNode(7);

    int returnSize;
    int* returnColumnSizes;
    int** res = levelOrder(root, &returnSize, &returnColumnSizes);

    // 打印结果
    for (int i = 0; i < returnSize; i++) 
    {
        printf("Level %d: ", i);
        for (int j = 0; j < returnColumnSizes[i]; j++) 
        {
            printf("%d ", res[i][j]);
        }
        printf("\n");
        free(res[i]); // 释放每层内存
    }

    // 释放内存
    free(res);
    free(returnColumnSizes);
    freeTree(root);

    return 0;
}