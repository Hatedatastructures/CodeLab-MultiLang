// 声明模块名（必须是全局唯一的）
export module mymodule;

// 导出函数（其他文件导入模块后可直接使用）
export int add(const int a, const int b)
{
    return a + b;
}

// 导出类
export class Calculator
{
public:
  static int multiply(const int a, const int b) { return a * b; }
};