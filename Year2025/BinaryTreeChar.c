#include <stdio.h>
#include <stdlib.h>
typedef struct tree
{
    char data;
    struct tree *left;
    struct tree *right;
}tree;

int Preamble_size(tree* root)
{
    /*前序计数*/
    if (root == NULL)
    {
        return 0;
    }
    else
    {
        return Preamble_size(root->left) + Preamble_size(root->right) + 1;
    }
}
void Preamble_print(tree* root)
{
    /*前序打印*/
    if(root == NULL)
    {
        return;
    }
    else
    {
        printf("%c ", root->data);
        Preamble_print(root->left);
        Preamble_print(root->right);
    }
}
void Medium_order_print(tree* root)
{
    /*中序打印*/
    if(root == NULL)
    {
        return;
    }
    else
    {
        Medium_order_print(root->left);
        printf("%c ", root->data);
        Medium_order_print(root->right);
    }
}
void Post_order_print(tree* root)
{
    /*后序打印*/
    if(root == NULL)
    {
        return;
    }
    else
    {
        Post_order_print(root->left);
        Post_order_print(root->right);
        printf("%c ", root->data);
    }
}
tree* data_malloc()
{
    tree* root = (tree*)malloc(sizeof(tree));
    root->left = NULL;
    root->right = NULL;
    return root;
}
int main()
{
    
    tree* A = data_malloc();
    tree* B = data_malloc();
    tree* C = data_malloc();
    tree* D = data_malloc();
    tree* E = data_malloc();
    tree* F = data_malloc();
    tree* G = data_malloc();
    A->data = 'A';
    B->data = 'B';
    C->data = 'C';
    D->data = 'D';
    E->data = 'E';
    F->data = 'F';
    G->data = 'G';
    A->left = B;
    A->right = C;
    B->left = D;
    B->right = E;
    D->left = F;
    D->right = G;
    printf("前序遍历：\n");
    Preamble_print(A);

    printf("\n中序遍历：\n");
    Medium_order_print(A);

    printf("\n后序遍历：\n");
    Post_order_print(A);
    printf("\n前序遍历节点个数：%d\n", Preamble_size(A));
    return 0;
}