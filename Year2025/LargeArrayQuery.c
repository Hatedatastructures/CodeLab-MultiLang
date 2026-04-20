#include <stdio.h>
#include <stdlib.h>

int main()
{
    const int N=1e9;
    int *a=(int*)malloc(N*sizeof(int));
    int n,q;
    scanf("%d %d",&n,&q);
    for(int i=0;i<n;i++)
    {
        scanf("%d",&a[i]);
    }
    for(int i=0;i<q;i++)
    {
        int k=0;
        scanf("%d",&k);
        int ks,ke;
        if(k<a[0]||k>a[n-1])
        {
            ks=-1;
            ke=-1;
        }
        int ls=a[0],rs=a[n-1];
        while(ls<rs)
        {
            int mid=(ls+rs)/2;
            if(a[mid]<k) 
            {
                ls=mid+1;
            }
            else 
            {   
                rs=mid;
            }
        }
        ks=ls;
        int le=a[0],re=a[n-1];
        while(le<re)
        {
            int mid=(le+re+1)/2;
            if(a[mid]>k) 
            {
                re=mid-1;
            }
            else    
            {
                le=mid;
            }
        }
        ke=le;
        printf("%d %d\n",ks,ke);
    }
    return 0;
}
