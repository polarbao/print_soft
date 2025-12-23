/**
 * @file PrintDeviceSDK_API.cpp
 * @brief SDK C语言接口实现
 * @details 提供标准C接口，包装内部的C++ SDKManager类
 *          所有业务逻辑已拆分到独立的模块文件中
 */

#include "PrintDeviceSDK_API.h"
#include "src/SDKManager.h"
#include <QMutexLocker>
#include <QString>

// ==================== 初始化和资源管理 ====================

int InitSDK(const char* log_dir) {
    // 初始化SDK
    // 注意：如果SDK独立运行（不在Qt应用程序中），需要创建QCoreApplication
    // 这里假设调用者会管理Qt事件循环
    bool success = SDKManager::instance()->init(log_dir);
    return success ? 0 : -1;
}

void ReleaseSDK() {
    // 释放SDK资源
    SDKManager::instance()->release();
}

void RegisterEventCallback(SdkEventCallback callback) {
    // 注册事件回调函数
    QMutexLocker lock(&g_callbackMutex);
    g_sdkCallback = callback;
}

// ==================== 连接管理 ====================

int ConnectByTCP(const char* ip, unsigned short port) {
    if (!ip) {
    return -1;
    }
    
    return SDKManager::instance()->connectByTCP(QString(ip), port);
}

int ConnectBySerial(const char* port_name, unsigned int baud_rate) {
    // 串口连接在当前项目中已被移除（根据readme，通过树莓派TCP连接）
    // 保留接口以保持API兼容性
    return -1;
}

void Disconnect() {
    SDKManager::instance()->disconnect();
}

int IsConnected() {
    return SDKManager::instance()->isConnected() ? 1 : 0;
}

// ==================== 运动控制 ====================

int MoveTo(double x, double y, double z, double speed) {
    // 绝对移动：分别移动各轴到指定坐标
    // 注意：原项目中主要是相对移动，这里实现为调用各轴移动
    int result = 0;
    
    if (x != 0) {
        result |= SDKManager::instance()->moveXAxis(x, true);
    }
    if (y != 0) {
        result |= SDKManager::instance()->moveYAxis(y, true);
    }
    if (z != 0) {
        result |= SDKManager::instance()->moveZAxis(z, true);
    }
    
    return result;
}

int MoveBy(double dx, double dy, double dz, double speed) {
    // 相对移动：各轴相对当前位置移动
    int result = 0;
    
    if (dx != 0) {
        result |= SDKManager::instance()->moveXAxis(dx, false);
    }
    if (dy != 0) {
        result |= SDKManager::instance()->moveYAxis(dy, false);
    }
    if (dz != 0) {
        result |= SDKManager::instance()->moveZAxis(dz, false);
    }
    
    return result;
}

int GoHome() {
    // 所有轴回原点
    // axisFlag = 7 表示 X(1) + Y(2) + Z(4) = 全部轴
    return SDKManager::instance()->resetAxis(7);
}

// ==================== 打印控制 ====================

int LoadPrintData(const char* data) {
    if (!data) {
    return -1;
    }
    
    // data参数为图像文件路径
    QString path = QString::fromUtf8(data);
    return SDKManager::instance()->loadImageData(path);
}

int StartPrint() {
    return SDKManager::instance()->startPrint();
}

int PausePrint() {
    return SDKManager::instance()->pausePrint();
}

int ResumePrint() {
    return SDKManager::instance()->resumePrint();
}

int StopPrint() {
    return SDKManager::instance()->stopPrint();
}

// ==================== MOC文件包含 ====================

// 在cpp文件的最后，包含 MOC 生成的文件
// 这是Qt元对象系统所需要的
#include "src/SDKManager.moc"
