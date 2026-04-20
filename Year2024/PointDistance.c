#include <stdio.h>
#include <math.h>

int main() 
{
    double x1, y1, x2, y2, distance;
    printf("请输入第一个点的坐标 (x1, y1): ");
    scanf("%lf %lf", &x1, &y1);
    printf("请输入第二个点的坐标 (x2, y2): ");
    scanf("%lf %lf", &x2, &y2);

    distance = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

    printf("两点之间的距离为: %.2f\n", distance);
    return 0;
}