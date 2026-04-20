 #include <stdio.h>

//模拟qsort函数,实现冒泡排序

int Swap(char*base1,char*base2,int *winth)
{
    for(int i =1;i<(*winth);i++)
    {
        //每次交换一个字节的数据，循环一次本来元素的字节数就能交换完
        char tmp =*base1;
        *base1=*base2;
        *base2=tmp;
        base1++;
        base2++;
    }
}
void my_qsort(void*base,int *sz,int *winth,int (cmp)(const void*a,const void*b))
//全指针传参
{
    for(int i =0;i<(*sz)-1;i++)
    {
        int counst=1;
        for(int j =0;j<(*sz)-1-i;j++)
        //每次计算都把最大的数字排到最后面了，所以后面的就不用再计算了，因而j<sz-1-i
        {
            if(cmp((char*)base+j*(*winth),(char*)base+(j+1)*(*winth))>0)
            //在不知道是怎么类型的情况下用sizeof读取的字节数乘j就能跳过该类型的一个元素
            //char能访问一个字符，加上刚好跳过
            {
                //if判断如果通过就是a>b，因为a>b所以升序要把b向后排
                Swap((char*)base+j*(*winth),(char*)base+(j+1)*(*winth),winth);
                counst=0;
            }
        }
        //如果本来就是升序，，没进入if判断，就会跳出来，防止重复计算
        if(counst==1)
        {
            break;
        }
    }
}

int cmp(const void*a,const void*b)
{
    //升序就把a和b的值反过来
    return (*(int *)a-*(int *)b);
}
int main()
{
    int arr[]={14, 3, 7, 19, 2, 16, 11, 5, 13, 20, 9, 1, 17, 10, 4, 15,
    6, 18, 12, 8,24, 13, 45, 19, 36, 3, 22, 40, 49, 18, 38, 48, 28, 27,
    20, 42, 34, 30, 16, 31, 1, 29, 41, 5, 17, 33, 7, 26, 46, 12, 10, 39, 
    2, 32, 21, 15, 43, 8, 44, 37, 47, 9, 11, 6, 23, 35, 25, 4, 14,14, 3,
    7, 19, 2, 16, 11, 5, 13, 20, 9, 1, 17, 10, 4, 15,6, 18, 12, 8,24, 13,
    45, 19, 36, 3, 22, 40, 49, 18, 38, 48, 28, 27,20, 42, 34, 30, 16, 31, 
    1, 29, 41, 5, 17, 33, 7, 26, 46, 12, 10, 39, 2, 32, 21, 15, 43, 8, 44,
    37, 47, 9, 11, 6, 23, 35, 25, 4, 14,12,23,34,21,32,4,34,13,53,54,67};
    int sz =sizeof(arr)/sizeof(arr[0]);
    int num =sizeof(arr[0]);
    my_qsort(arr,&sz,&num,cmp);
    for(int l =0;l<sz;l++)
    {
        printf("%d ",arr[l]);
    }
    return 0;
}