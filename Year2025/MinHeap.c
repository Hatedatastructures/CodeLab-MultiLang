/*小堆*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
typedef int Heap_array;

typedef struct Complete_binary_tree
{
    Heap_array* data;
    int size;       
    int capacity;
}Complete_binary_tree;//数组实现完全二叉树

void exchange(Heap_array* former,Heap_array* after)//参数：两个指针，指向要交换的两个元素
//数据交换
{
    Heap_array temp = *former;
    *former = *after;
    *after = temp;
}
void Adjust_downwards(Complete_binary_tree* tree, int parent)//参数：排序根节点，根节点在数组中的下标
//向下调整
{
    int child = 2 * parent + 1;//左孩子
    while (child < tree->size)
    {
        if(child + 1 < tree->size && tree->data[child] > tree->data[child + 1])//找最小的节点
        {
            child++;
        }
        if(tree->data[child] < tree->data[parent])//如果孩子节点比父节点小，则交换，否则退出循环
        {
            exchange(&tree->data[child], &tree->data[parent]);
            parent = child;
            child = 2 * parent + 1;
        }
        else
        {
            break;
        }
    }
}
void Small_pile_sorting(Complete_binary_tree* tree)//参数：完全二叉树
{
    for(int i = (tree->size - 2) / 2; i >= 0; i--)//从非叶子节点开始，向上调整
    {
        Adjust_downwards(tree, i);
    }
}
int main()
{
    /*小堆排序*/
    Heap_array array[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40};
    Complete_binary_tree* tree =(Complete_binary_tree*)malloc(sizeof(Complete_binary_tree));
    tree->data = array;
    tree->size = sizeof(array) / sizeof(array[0]);
    tree->capacity = tree->size;
    Small_pile_sorting(tree);
    
    for (int i = 0; i < tree->size; i++)
    {
        printf("%d ", tree->data[i]);
    }
    return 0;
}