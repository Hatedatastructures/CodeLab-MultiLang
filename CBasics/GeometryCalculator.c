#include <stdio.h>
#define pai 3.1515926
double cylinder(double a, double b) // 圆柱表面积
// a为半径，b为高
{
    double q = (pai * (a * a) * 2) + (2 * pai * a * b);
    return q;
}

double sphere(double a) // 球表面积
                        // a为半径
{
    double q = 4 * pai * (a * a);
    return q;
}

double cone(double a, double c) // 圆锥表面积
// a是半径，是母线
{
    double q = pai * (a * a) * c; // c为l是母线
    return q;
}
double cuboid(double a, double b, double d) // 长方体表面积
// a为长，b为高，d为宽
{
    double volume = (2 * a * b) + (4 * a * d);
    return volume;
}

double Cube(double b) // 正方体表面积
// b为高
{
    double a = b * b * 6;
    return a;
}
void mad() // 表面积函
{
    printf("请输入计算的物体的表面积\n");
    printf("输入1为圆柱表面积\n");
    printf("输入2为球表面积\n");
    printf("输入3为圆锥表面积\n");
    printf("输入4为长方体表面积\n");
    printf("输入5为正方体表面积\n");
    double r;
    double h;
    double l;
    double j;
    double k = 0;
    int n;
    scanf("%d", &n);
    double m;
    int p;
    switch (n) // 判断
    {
    case 1:
        printf("请输入半径和高：\n");
        scanf("%lf %lf", &r, &h);
        m = cylinder(r, h);
        printf("%f", m);
        break;
    case 2:
        printf("请输入半径：\n");
        scanf("%lf", &r);
        m = sphere(r);
        printf("%lf", m);
        break;
    case 3:
        printf("请输入半径和母线：\n");
        scanf("%lf %lf", &r, &l);
        m = cone(r, l);
        printf("%lf", m);
        break;
    case 4:
        printf("请输入长宽高：\n");
        scanf("%lf %lf %lf", &j, &k, &h);
        p = cuboid(j, k, h);
        printf("%lf", p);
        break;
    case 5:
        printf("请输入高：\n");
        scanf("%lf", &h);
        p = Cube(h);
        printf("%lf", p);
        break;
    default:
        printf("输入错误");
        break;
    }
}
double cylindervolume(double R, double H) // 圆柱体积
{
    double a = pai * (R * R) * H;
    return a;
}
double Spherevolume(double R) // 球体积
{
    double a = (4 / 3) * pai * (R * R * R);
    return a;
}
double Conevolume(double R, double H) // 圆锥体体积
{
    double a = (1 / 3) * pai * (R * R) * H;
    return a;
}
double Boxvolume(double B, double C, double D) // 长方体体积
{
    double a = B * C * D;
    return a;
}

double Cubevolume(double B) // 正方体体积
{
    double a = B * B * B;
    return a;
}
void mac() // 体积
{
    printf("请输入计算的物体的体积\n");
    printf("输入1为圆柱体积\n");
    printf("输入2为球体积\n");
    printf("输入3为圆锥体积\n");
    printf("输入4为长方体体积\n");
    printf("输入5为正方体体积\n");
    double r;
    double h;
    double l;
    double j;
    double k = 0;
    int n;
    scanf("%d", &n);
    double m;
    int p;
    switch (n) // 判断
    {
    case 1:
        printf("请输入半径和高：\n");
        scanf("%lf %lf", &r, &h);
        m = cylindervolume(r, h);
        printf("%f", m);
        break;
    case 2:
        printf("请输入半径：\n");
        scanf("%lf", &r);
        m = Spherevolume(r);
        printf("%f", m);
        break;
    case 3:
        printf("请输入半径和高：\n");
        scanf("%lf %lf", &r, &h);
        m = Conevolume(r, h);
        printf("%f", m);
        break;
    case 4:
        printf("请输入长宽高：\n");
        scanf("%lf %lf %lf", &j, &k, &h);
        p = Boxvolume(j, k, h);
        printf("%lf", p);
        break;
    case 5:
        printf("请输入高：\n");
        scanf("%lf", &h);
        p = Cubevolume(h);
        printf("%lf", p);
        break;
    default:
        printf("输入错误");
        break;
    }
}
int main()
{
    printf("要计算什么？\n");
    printf("输入1.表面积\n");
    printf("输入2.体积\n");
    int v = 0;
    scanf("%d", &v);
    if (v == 1) // 通过判断v的值选择进入的函数
    {
        mad();
    }
    else if (v == 2)
    {
        mac();
    }
    printf("计算结束");
    return 0;
}
