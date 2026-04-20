#include <stdio.h>

int main()
{
  int sum;
  scanf("%d", &sum);
  while (sum)
  {
    if (sum % 2 == 1)
    {
      printf("奇数");
    }
    else
    {
      printf("偶数");
    }
    sum++;
  }
  return 0;
}