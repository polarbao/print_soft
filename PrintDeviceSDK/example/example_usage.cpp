/**
 * @file example_usage.cpp
 * @brief PrintDeviceSDK使用示例
 * 
 * 该示例展示了如何使用PrintDeviceSDK控制打印设备
 */

#include "../PrintDeviceSDK_API.h"
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <iostream>

// 全局变量用于控制程序流程
static bool g_connected = false;
static bool g_shouldExit = false;

/**
 * @brief SDK事件回调函数
 * 
 * 该函数会在SDK产生事件时被调用
 */
void MyEventCallback(const SdkEvent* event) {
    // 打印事件信息
    std::cout << "[SDK Event] ";
    
    switch (event->type) {
        case EVENT_TYPE_GENERAL:
            std::cout << "GENERAL - ";
            break;
        case EVENT_TYPE_ERROR:
            std::cout << "ERROR - ";
            break;
        case EVENT_TYPE_PRINT_STATUS:
            std::cout << "PRINT_STATUS - ";
            break;
        case EVENT_TYPE_MOVE_STATUS:
            std::cout << "MOVE_STATUS - ";
            break;
        case EVENT_TYPE_LOG:
            std::cout << "LOG - ";
            break;
    }
    
    std::cout << event->message;
    
    // 如果有附加数据，打印出来
    if (event->value1 != 0.0 || event->value2 != 0.0 || event->value3 != 0.0) {
        std::cout << " [" << event->value1 << ", " 
                  << event->value2 << ", " 
                  << event->value3 << "]";
    }
    
    std::cout << std::endl;
    
    // 处理特定事件
    if (std::string(event->message).find("Connected") != std::string::npos) {
        g_connected = true;
    }
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[]) {
    // 创建Qt应用程序（SDK需要Qt事件循环）
    QCoreApplication app(argc, argv);
    
    std::cout << "==================================" << std::endl;
    std::cout << "PrintDeviceSDK 使用示例" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;
    
    // 1. 初始化SDK
    std::cout << "[1] 初始化SDK..." << std::endl;
    if (InitSDK("./logs") != 0) {
        std::cerr << "错误: SDK初始化失败!" << std::endl;
        return -1;
    }
    std::cout << "    SDK初始化成功" << std::endl << std::endl;
    
    // 2. 注册事件回调
    std::cout << "[2] 注册事件回调..." << std::endl;
    RegisterEventCallback(MyEventCallback);
    std::cout << "    回调注册成功" << std::endl << std::endl;
    
    // 3. 连接到设备
    std::cout << "[3] 连接到设备..." << std::endl;
    const char* device_ip = "192.168.100.57";  // 修改为实际的设备IP
    unsigned short device_port = 5555;
    
    std::cout << "    目标地址: " << device_ip << ":" << device_port << std::endl;
    
    if (ConnectByTCP(device_ip, device_port) != 0) {
        std::cerr << "错误: 连接失败!" << std::endl;
        ReleaseSDK();
        return -1;
    }
    
    // 等待连接建立
    std::cout << "    等待连接建立..." << std::endl;
    
    // 使用定时器等待连接
    QTimer waitTimer;
    int waitCount = 0;
    QObject::connect(&waitTimer, &QTimer::timeout, [&]() {
        waitCount++;
        if (g_connected) {
            waitTimer.stop();
            
            std::cout << "    连接已建立!" << std::endl << std::endl;
            
            // 4. 执行一些操作
            std::cout << "[4] 执行设备操作..." << std::endl;
            
            // 复位所有轴
            std::cout << "    4.1 复位所有轴..." << std::endl;
            if (GoHome() == 0) {
                std::cout << "        复位命令已发送" << std::endl;
            }
            
            // 延时
            QTimer::singleShot(2000, [&]() {
                // Z轴移动
                std::cout << "    4.2 Z轴上移10mm..." << std::endl;
                if (MoveBy(0, 0, 10.0, 100) == 0) {
                    std::cout << "        移动命令已发送" << std::endl;
                }
                
                QTimer::singleShot(2000, [&]() {
                    // 开始打印
                    std::cout << "    4.3 开始打印..." << std::endl;
                    if (StartPrint() == 0) {
                        std::cout << "        打印命令已发送" << std::endl;
                    }
                    
                    // 5秒后停止打印
                    QTimer::singleShot(5000, [&]() {
                        std::cout << "    4.4 停止打印..." << std::endl;
                        if (StopPrint() == 0) {
                            std::cout << "        停止命令已发送" << std::endl;
                        }
                        
                        // 再等2秒后断开连接
                        QTimer::singleShot(2000, [&]() {
                            std::cout << std::endl << "[5] 断开连接..." << std::endl;
                            Disconnect();
                            
                            QTimer::singleShot(1000, [&]() {
                                std::cout << "    连接已断开" << std::endl << std::endl;
                                
                                // 6. 释放SDK
                                std::cout << "[6] 释放SDK..." << std::endl;
                                ReleaseSDK();
                                std::cout << "    SDK已释放" << std::endl << std::endl;
                                
                                std::cout << "==================================" << std::endl;
                                std::cout << "示例程序完成" << std::endl;
                                std::cout << "==================================" << std::endl;
                                
                                // 退出应用
                                QCoreApplication::quit();
                            });
                        });
                    });
                });
            });
            
        } else if (waitCount > 50) {  // 10秒超时
            waitTimer.stop();
            std::cerr << "错误: 连接超时!" << std::endl;
            ReleaseSDK();
            QCoreApplication::quit();
        }
    });
    
    waitTimer.start(200);  // 每200ms检查一次
    
    // 运行事件循环
    return app.exec();
}

