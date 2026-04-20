#include <stdio.h>

#define N 100

void input(int *);
void sort(int *);
void output(int *);
int main()
{
    int a[N];
    input(a);
    sort(a);
    output(a);
    return 0;
}
void input(int *a)
{
    int i;
    for(i=0;i<N;i++)
    scanf("%d",&a[i]);
}
void sort(int *a)
{
    int i,j,temp;
for(i=0;i<N-1;i++)
    for(j=0;j<N-i-1;j++)
        if(*(a+j)>*(a+j+1))
        {
            temp=*(a+j);
            *(a+j)=*(a+j+1);
            *(a+j+1)=temp;
        }
}
void output(int *a)
{
    int i;
    for(i=0;i<N;i++)
        printf("%d ",*(a+i));
    printf("\n");
}