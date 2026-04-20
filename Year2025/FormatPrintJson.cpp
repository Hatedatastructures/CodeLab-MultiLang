#include <iostream>
#include <algorithm>
#include <format>
#include <print>
#include <tuple>
#include <json/json.h>
// #include <opencv4/opencv2/opencv_modules.hpp>
// #include <opencv4/opencv2/opencv.hpp>
auto func() 
{
    struct type
    {
        int max;
        int min;
    };
    return type{ 1, 2 };
}
int main()
{
    // cv::Mat image = cv::imread("C:\\Users\\C1373\\Pictures\\Screenshots\\屏幕截图 2025-09-03 183820.png");

    // // 检查图片是否成功加载
    // if (image.empty()) 
    // {
    //     std::cout << "无法加载图片！" << std::endl;
    //     return -1;
    // }

    // // 显示图片
    // cv::imshow("Display Image", image);

    // // 等待按键
    // cv::waitKey(0);

    std::cout << std::format("Hello, {}!\n", "World");
    auto result = func();
    std::cout << result.max << " " << result.min << std::endl;
    std::tuple<int, double, std::string> t(1, 3.14, "hello");
    Json::Value root;
    root["int"] = 1;
    root["double"] = 3.14;
    root["string"] = "hello";
    root["array"] = Json::Value(Json::arrayValue);
    std::cout << root.toStyledString() << std::endl;
    return 0;
}