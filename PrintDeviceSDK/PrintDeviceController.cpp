/**
 * @file PrintDeviceController.cpp
 * @brief Qt风格SDK接口的实现
 */

#include "PrintDeviceController.h"
#include "PrintDeviceSDK_API.h"  // 复用C接口的底层实现
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QMetaObject>

/**
 * @class PrintDeviceController::Private
 * @brief 私有实现类（Pimpl模式）
 * 
 * 优点：
 * 1. 隐藏实现细节
 * 2. 减少头文件依赖
 * 3. 保持ABI稳定性
 * 4. 方便内部修改
 */
class PrintDeviceController::Private
{
public:
    explicit Private(PrintDeviceController* q) 
        : q_ptr(q)
        , initialized(false)
        , connectedState(false)
        , port(0)
    {
    }
    
    ~Private()
    {
    }
    
    PrintDeviceController* q_ptr;
    bool initialized;
    bool connectedState;
    QString ip;
    quint16 port;
    
    // 静态回调函数（桥接C回调到Qt信号）
    static void sdkEventCallback(const SdkEvent* event);
    
    // 全局实例管理（用于C回调中访问Qt对象）
    static PrintDeviceController* s_instance;
    static QMutex s_mutex;
};

// 静态成员初始化
PrintDeviceController* PrintDeviceController::Private::s_instance = nullptr;
QMutex PrintDeviceController::Private::s_mutex;

// ==================== 构造和析构 ====================

PrintDeviceController::PrintDeviceController(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
    QMutexLocker locker(&Private::s_mutex);
    Private::s_instance = this;
}

PrintDeviceController::~PrintDeviceController()
{
    release();
    
    QMutexLocker locker(&Private::s_mutex);
    if (Private::s_instance == this) {
        Private::s_instance = nullptr;
    }
    
    delete d;
}

// ==================== 生命周期管理 ====================

bool PrintDeviceController::initialize(const QString& logDir)
{
    if (d->initialized) {
        qDebug() << "SDK已经初始化";
        return true;
    }
    
    // 调用C接口初始化底层SDK
    int ret = InitSDK(logDir.isEmpty() ? nullptr : logDir.toUtf8().constData());
    if (ret != 0) {
        QString errMsg = tr("SDK初始化失败，错误码：%1").arg(ret);
        emit errorOccurred(-1, errMsg);
        return false;
    }
    
    // 注册事件回调函数（桥接C回调到Qt信号）
    RegisterEventCallback(&Private::sdkEventCallback);
    
    d->initialized = true;
    emit infoMessage(tr("SDK初始化成功"));
    
    qDebug() << "PrintDeviceController initialized successfully";
    return true;
}

void PrintDeviceController::release()
{
    if (!d->initialized) {
        return;
    }
    
    // 断开连接
    if (d->connectedState) {
        disconnectFromDevice();
    }
    
    // 释放底层SDK
    ReleaseSDK();
    
    d->initialized = false;
    d->connectedState = false;
    d->ip.clear();
    d->port = 0;
    
    qDebug() << "PrintDeviceController released";
}

bool PrintDeviceController::isInitialized() const
{
    return d->initialized;
}

// ==================== 连接管理 ====================

bool PrintDeviceController::connectToDevice(const QString& ip, quint16 port)
{
    if (!d->initialized) {
        emit errorOccurred(-1, tr("SDK未初始化，请先调用initialize()"));
        return false;
    }
    
    if (d->connectedState) {
        emit infoMessage(tr("设备已连接"));
        return true;
    }
    
    // 调用C接口连接设备
    int ret = ConnectByTCP(ip.toUtf8().constData(), port);
    if (ret != 0) {
        QString errMsg = tr("连接失败，错误码：%1").arg(ret);
        emit errorOccurred(ret, errMsg);
        return false;
    }
    
    // 保存连接信息
    d->ip = ip;
    d->port = port;
    emit deviceIpChanged(ip);
    emit devicePortChanged(port);
    
    emit infoMessage(tr("正在连接 %1:%2...").arg(ip).arg(port));
    
    return true;
}

void PrintDeviceController::disconnectFromDevice()
{
    if (!d->initialized) {
        return;
    }
    
    if (!d->connectedState) {
        emit infoMessage(tr("设备未连接"));
        return;
    }
    
    // 调用C接口断开连接
    Disconnect();
    
    emit infoMessage(tr("正在断开连接..."));
}

bool PrintDeviceController::isConnected() const
{
    return d->connectedState;
}

QString PrintDeviceController::deviceIp() const
{
    return d->ip;
}

quint16 PrintDeviceController::devicePort() const
{
    return d->port;
}

void PrintDeviceController::refreshConnectionStatus()
{
    bool connected = IsConnected();
    if (d->connectedState != connected) {
        d->connectedState = connected;
        emit connectedChanged(connected);
        
        if (connected) {
            emit connected();
        } else {
            emit disconnected();
        }
    }
}

// ==================== 运动控制 ====================

bool PrintDeviceController::moveTo(double x, double y, double z, double speed)
{
    if (!isConnected()) {
        emit errorOccurred(-1, tr("设备未连接"));
        return false;
    }
    
    int ret = MoveTo(x, y, z, speed);
    if (ret != 0) {
        emit errorOccurred(ret, tr("MoveTo命令失败"));
        return false;
    }
    
    emit moveStatusChanged(tr("移动到位置 (%.2f, %.2f, %.2f)").arg(x).arg(y).arg(z));
    return true;
}

bool PrintDeviceController::moveBy(double dx, double dy, double dz, double speed)
{
    if (!isConnected()) {
        emit errorOccurred(-1, tr("设备未连接"));
        return false;
    }
    
    int ret = MoveBy(dx, dy, dz, speed);
    if (ret != 0) {
        emit errorOccurred(ret, tr("MoveBy命令失败"));
        return false;
    }
    
    emit moveStatusChanged(tr("相对移动 (%.2f, %.2f, %.2f)").arg(dx).arg(dy).arg(dz));
    return true;
}

bool PrintDeviceController::goHome()
{
    if (!isConnected()) {
        emit errorOccurred(-1, tr("设备未连接"));
        return false;
    }
    
    int ret = GoHome();
    if (ret != 0) {
        emit errorOccurred(ret, tr("回原点命令失败"));
        return false;
    }
    
    emit moveStatusChanged(tr("正在回原点..."));
    return true;
}

bool PrintDeviceController::moveXAxis(double distance, double speed)
{
    return moveBy(distance, 0, 0, speed);
}

bool PrintDeviceController::moveYAxis(double distance, double speed)
{
    return moveBy(0, distance, 0, speed);
}

bool PrintDeviceController::moveZAxis(double distance, double speed)
{
    return moveBy(0, 0, distance, speed);
}

// ==================== 打印控制 ====================

bool PrintDeviceController::loadPrintData(const QString& filePath)
{
    if (!isConnected()) {
        emit errorOccurred(-1, tr("设备未连接"));
        return false;
    }
    
    if (filePath.isEmpty()) {
        emit errorOccurred(-1, tr("文件路径为空"));
        return false;
    }
    
    int ret = LoadPrintData(filePath.toUtf8().constData());
    if (ret != 0) {
        emit errorOccurred(ret, tr("加载打印数据失败"));
        return false;
    }
    
    emit infoMessage(tr("图像数据已加载：%1").arg(filePath));
    return true;
}

bool PrintDeviceController::startPrint()
{
    if (!isConnected()) {
        emit errorOccurred(-1, tr("设备未连接"));
        return false;
    }
    
    int ret = StartPrint();
    if (ret != 0) {
        emit errorOccurred(ret, tr("开始打印命令失败"));
        return false;
    }
    
    emit printStatusChanged(tr("打印开始"));
    return true;
}

bool PrintDeviceController::pausePrint()
{
    if (!isConnected()) {
        emit errorOccurred(-1, tr("设备未连接"));
        return false;
    }
    
    int ret = PausePrint();
    if (ret != 0) {
        emit errorOccurred(ret, tr("暂停打印命令失败"));
        return false;
    }
    
    emit printStatusChanged(tr("打印暂停"));
    return true;
}

bool PrintDeviceController::resumePrint()
{
    if (!isConnected()) {
        emit errorOccurred(-1, tr("设备未连接"));
        return false;
    }
    
    int ret = ResumePrint();
    if (ret != 0) {
        emit errorOccurred(ret, tr("恢复打印命令失败"));
        return false;
    }
    
    emit printStatusChanged(tr("打印恢复"));
    return true;
}

bool PrintDeviceController::stopPrint()
{
    if (!isConnected()) {
        emit errorOccurred(-1, tr("设备未连接"));
        return false;
    }
    
    int ret = StopPrint();
    if (ret != 0) {
        emit errorOccurred(ret, tr("停止打印命令失败"));
        return false;
    }
    
    emit printStatusChanged(tr("打印停止"));
    return true;
}

// ==================== 回调函数（桥接C回调到Qt信号）====================

void PrintDeviceController::Private::sdkEventCallback(const SdkEvent* event)
{
    if (!event) {
        return;
    }
    
    QMutexLocker locker(&s_mutex);
    
    if (!s_instance) {
        return;
    }
    
    // 将C回调转换为Qt信号
    // 使用QMetaObject::invokeMethod确保信号在正确的线程中发射（线程安全）
    QString message = QString::fromUtf8(event->message);
    SdkEventType type = event->type;
    int code = event->code;
    double v1 = event->value1;
    double v2 = event->value2;
    double v3 = event->value3;
    
    // 使用Qt::QueuedConnection确保在主线程中执行
    QMetaObject::invokeMethod(s_instance, [=]() {
        switch (type) {
        case EVENT_TYPE_GENERAL: {
            // 检测连接状态变化
            if (message.contains("Connected", Qt::CaseInsensitive) && 
                message.contains("device", Qt::CaseInsensitive)) {
                s_instance->d->connectedState = true;
                emit s_instance->connected();
                emit s_instance->connectedChanged(true);
                qDebug() << "Device connected";
            } 
            else if (message.contains("Disconnected", Qt::CaseInsensitive)) {
                s_instance->d->connectedState = false;
                emit s_instance->disconnected();
                emit s_instance->connectedChanged(false);
                qDebug() << "Device disconnected";
            }
            
            emit s_instance->infoMessage(message);
            break;
        }
            
        case EVENT_TYPE_ERROR: {
            emit s_instance->errorOccurred(code, message);
            qWarning() << "SDK Error:" << code << message;
            break;
        }
            
        case EVENT_TYPE_PRINT_STATUS: {
            int progress = static_cast<int>(v1);
            int currentLayer = static_cast<int>(v2);
            int totalLayers = static_cast<int>(v3);
            
            emit s_instance->printProgressUpdated(progress, currentLayer, totalLayers);
            
            QString statusMsg = QString("打印进度: %1% (%2/%3层)")
                .arg(progress).arg(currentLayer).arg(totalLayers);
            emit s_instance->printStatusChanged(statusMsg);
            
            qDebug() << "Print progress:" << progress << "%" 
                     << currentLayer << "/" << totalLayers;
            break;
        }
            
        case EVENT_TYPE_MOVE_STATUS: {
            emit s_instance->moveStatusChanged(message);
            
            // 如果有坐标信息，发送位置更新
            if (v1 != 0 || v2 != 0 || v3 != 0) {
                emit s_instance->positionUpdated(v1, v2, v3);
                qDebug() << "Position:" << v1 << v2 << v3;
            }
            break;
        }
            
        case EVENT_TYPE_LOG: {
            emit s_instance->logMessage(message);
            // qDebug() << "SDK Log:" << message;  // 可选：打印到调试输出
            break;
        }
            
        default: {
            qWarning() << "Unknown event type:" << type;
            break;
        }
        }
    }, Qt::QueuedConnection);
}

