/**
 * @file main_sdk.cpp
 * @brief 使用PrintDeviceSDK的主程序
 * @details 演示如何在Qt Widget应用程序中使用PrintDeviceSDK
 */

#include <QApplication>
#include <QCoreApplication>
#include "ui/PrintDeviceUI_SDK.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    QCoreApplication::setApplicationName("PrintDevice SDK Demo");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationName("PrintDevice");
    
    // 创建并显示SDK版本的UI
    PrintDeviceUI_SDK window;
    window.show();
    
    return app.exec();
}

