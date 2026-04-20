#include <windows.h>

// 窗口过程函数的声明
LRESULT CALLBACK WindowPro(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// 窗口过程函数的实现
LRESULT CALLBACK WindowPro(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    switch (uMsg) 
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    // 定义窗口类
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowPro; // 指定窗口过程函数
    wc.hInstance = hInstance;
    wc.lpszClassName = "SampleWindowClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    // 注册窗口类
    RegisterClass(&wc);

    // 创建窗口
    HWND hwnd = CreateWindow(
        "SampleWindowClass", // 窗口类名
        "胡桃工具箱",         // 窗口标题
        WS_OVERLAPPEDWINDOW, // 窗口样式
        CW_USEDEFAULT,       // 初始X坐标
        CW_USEDEFAULT,       // 初始Y坐标
        2560,                // 窗口宽度
        1440,                // 窗口高度
        NULL,                // 父窗口句柄
        NULL,                // 菜单句柄
        hInstance,           // 实例句柄
        NULL                 // 附加参数
    );

    // 显示窗口
    ShowWindow(hwnd, nCmdShow);

    // 消息循环
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

