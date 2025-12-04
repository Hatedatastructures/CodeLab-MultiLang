#include <iostream>
#include <qt6/QtWidgets/qapplication.h>
#include <qt6/QtWidgets/qwidget.h>
#include <qt6/QtGui/QtGui>



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget w;          // 创建窗体对象
    w.setWindowTitle("Qt 空白窗体");
    w.resize(400, 300); // 宽 400 高 300
    w.show();           // 显示
    std::cout << "Hello World!" << std::endl;
    QString str = "Hello World!";
    std::cout << str.toStdString() << std::endl;
    return 0;    // 进入事件循环
}