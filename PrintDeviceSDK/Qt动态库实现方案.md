# PrintDeviceSDK Qt动态库实现方案

## 📊 当前实现 vs Qt动态库实现

### 当前实现方式（C接口）

```cpp
// 当前方式
#ifdef PRINTDEVICESDK_EXPORTS
#define SDK_API __declspec(dllexport)
#else
#define SDK_API __declspec(dllimport)
#endif

// C接口
extern "C" {
    SDK_API int InitSDK(const char* log_dir);
    SDK_API void RegisterEventCallback(SdkEventCallback callback);
    SDK_API int ConnectByTCP(const char* ip, unsigned short port);
}

// 回调方式
typedef void(*SdkEventCallback)(const SdkEvent* event);
```

**特点**：
- ✅ 可被C/C++/C#/Python等多种语言调用
- ✅ 跨平台兼容性好
- ❌ 不符合Qt编程习惯
- ❌ 无法使用Qt信号槽
- ❌ 回调函数在不同线程中可能有问题

### Qt动态库实现方式

```cpp
// Qt方式
#include <QtGlobal>

#if defined(PRINTDEVICESDK_LIBRARY)
#  define PRINTDEVICESDK_EXPORT Q_DECL_EXPORT
#else
#  define PRINTDEVICESDK_EXPORT Q_DECL_IMPORT
#endif

// 导出Qt类
class PRINTDEVICESDK_EXPORT PrintDeviceController : public QObject
{
    Q_OBJECT
    
public:
    explicit PrintDeviceController(QObject *parent = nullptr);
    
    // 使用标准Qt类型
    bool initialize(const QString& logDir);
    int connectToDevice(const QString& ip, quint16 port);
    
signals:
    // 使用信号代替回调
    void connected();
    void disconnected();
    void errorOccurred(int code, const QString& message);
    void printProgressUpdated(int progress, int currentLayer, int totalLayer);
};
```

**特点**：
- ✅ 完全符合Qt编程习惯
- ✅ 使用信号槽机制
- ✅ 线程安全（Qt自动处理）
- ✅ 类型安全（QString, QByteArray等）
- ❌ 只能被Qt/C++程序使用
- ❌ 不能直接被C#/Python调用

---

## 🎯 方案选择

### 方案对比

| 方面 | C接口方式 | Qt动态库方式 | 推荐 |
|------|----------|-------------|------|
| **适用场景** | | | |
| Qt项目 | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | Qt方式 |
| C项目 | ⭐⭐⭐⭐⭐ | ❌ | C方式 |
| C#项目 | ⭐⭐⭐⭐ | ❌ | C方式 |
| Python项目 | ⭐⭐⭐⭐ | ❌ | C方式 |
| **技术特性** | | | |
| 信号槽支持 | ❌ | ⭐⭐⭐⭐⭐ | Qt方式 |
| 线程安全 | 需手动处理 | 自动处理 | Qt方式 |
| Qt集成 | 需包装 | 原生支持 | Qt方式 |
| 类型安全 | 低 | 高 | Qt方式 |
| **兼容性** | | | |
| 跨语言 | ⭐⭐⭐⭐⭐ | ❌ | C方式 |
| 跨平台 | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | 都好 |

### 推荐方案：**双接口共存** ✨

**最佳方案**：同时提供C接口和Qt接口！

```
PrintDeviceSDK
├── C接口（现有）          # 用于跨语言调用
│   └── PrintDeviceSDK_API.h
└── Qt接口（新增）✨       # 用于Qt项目
    └── PrintDeviceController.h
```

**优势**：
1. ✅ 保持跨语言兼容性
2. ✅ Qt项目可使用更好的Qt接口
3. ✅ 两种接口共享同一套底层实现
4. ✅ 无需修改现有代码

---

## 💻 Qt动态库实现代码

### 文件结构

```
PrintDeviceSDK/
├── PrintDeviceSDK_API.h           # C接口（保留）
├── PrintDeviceSDK_API.cpp         # C接口实现（保留）
├── PrintDeviceController.h        # Qt接口（新增）✨
├── PrintDeviceController.cpp      # Qt接口实现（新增）✨
├── src/
│   ├── SDKManager.h
│   └── ...（内部实现，共享）
└── PrintDeviceSDK.pro             # 项目文件（需更新）
```

---

## 📝 完整实现代码

### 1. PrintDeviceController.h（Qt接口）

```cpp
/**
 * @file PrintDeviceController.h
 * @brief Qt风格的SDK接口
 * @details 提供完整的Qt信号槽支持，适合Qt项目使用
 */

#ifndef PRINTDEVICECONTROLLER_H
#define PRINTDEVICECONTROLLER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QtGlobal>

// Qt导出宏
#if defined(PRINTDEVICESDK_LIBRARY)
#  define PRINTDEVICESDK_EXPORT Q_DECL_EXPORT
#else
#  define PRINTDEVICESDK_EXPORT Q_DECL_IMPORT
#endif

/**
 * @class PrintDeviceController
 * @brief 打印设备控制器（Qt版本）
 * 
 * 这是Qt风格的SDK接口，使用信号槽机制代替回调函数。
 * 适合Qt项目使用，提供更好的类型安全和线程安全。
 */
class PRINTDEVICESDK_EXPORT PrintDeviceController : public QObject
{
    Q_OBJECT
    
    // 属性定义（Qt风格）
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(QString deviceIp READ deviceIp NOTIFY deviceIpChanged)
    Q_PROPERTY(quint16 devicePort READ devicePort NOTIFY devicePortChanged)

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit PrintDeviceController(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~PrintDeviceController();
    
    // ==================== 生命周期管理 ====================
    
    /**
     * @brief 初始化SDK
     * @param logDir 日志目录（可为空）
     * @return true=成功, false=失败
     */
    bool initialize(const QString& logDir = QString());
    
    /**
     * @brief 释放SDK资源
     */
    void release();
    
    // ==================== 连接管理 ====================
    
    /**
     * @brief 连接设备
     * @param ip 设备IP地址
     * @param port 端口号
     * @return true=成功发起连接, false=失败
     */
    bool connectToDevice(const QString& ip, quint16 port);
    
    /**
     * @brief 断开连接
     */
    void disconnectFromDevice();
    
    /**
     * @brief 查询连接状态
     * @return true=已连接, false=未连接
     */
    bool isConnected() const;
    
    /**
     * @brief 获取设备IP
     */
    QString deviceIp() const;
    
    /**
     * @brief 获取设备端口
     */
    quint16 devicePort() const;
    
    // ==================== 运动控制 ====================
    
    /**
     * @brief 移动到绝对坐标
     * @param x X轴坐标（mm）
     * @param y Y轴坐标（mm）
     * @param z Z轴坐标（mm）
     * @param speed 速度（mm/s）
     * @return true=成功, false=失败
     */
    bool moveTo(double x, double y, double z, double speed = 100.0);
    
    /**
     * @brief 相对移动
     * @param dx X轴增量（mm）
     * @param dy Y轴增量（mm）
     * @param dz Z轴增量（mm）
     * @param speed 速度（mm/s）
     * @return true=成功, false=失败
     */
    bool moveBy(double dx, double dy, double dz, double speed = 100.0);
    
    /**
     * @brief 回原点
     * @return true=成功, false=失败
     */
    bool goHome();
    
    /**
     * @brief X轴移动
     * @param distance 移动距离（mm，正数向前，负数向后）
     * @param speed 速度
     * @return true=成功, false=失败
     */
    bool moveXAxis(double distance, double speed = 100.0);
    
    /**
     * @brief Y轴移动
     * @param distance 移动距离（mm）
     * @param speed 速度
     * @return true=成功, false=失败
     */
    bool moveYAxis(double distance, double speed = 100.0);
    
    /**
     * @brief Z轴移动
     * @param distance 移动距离（mm，正数向上，负数向下）
     * @param speed 速度
     * @return true=成功, false=失败
     */
    bool moveZAxis(double distance, double speed = 100.0);
    
    // ==================== 打印控制 ====================
    
    /**
     * @brief 加载打印数据
     * @param filePath 图像文件路径
     * @return true=成功, false=失败
     */
    bool loadPrintData(const QString& filePath);
    
    /**
     * @brief 开始打印
     * @return true=成功, false=失败
     */
    bool startPrint();
    
    /**
     * @brief 暂停打印
     * @return true=成功, false=失败
     */
    bool pausePrint();
    
    /**
     * @brief 恢复打印
     * @return true=成功, false=失败
     */
    bool resumePrint();
    
    /**
     * @brief 停止打印
     * @return true=成功, false=失败
     */
    bool stopPrint();

signals:
    // ==================== 连接相关信号 ====================
    
    /**
     * @brief 连接成功
     */
    void connected();
    
    /**
     * @brief 连接断开
     */
    void disconnected();
    
    /**
     * @brief 连接状态改变
     * @param isConnected 是否已连接
     */
    void connectedChanged(bool isConnected);
    
    /**
     * @brief 设备IP改变
     * @param ip IP地址
     */
    void deviceIpChanged(const QString& ip);
    
    /**
     * @brief 设备端口改变
     * @param port 端口号
     */
    void devicePortChanged(quint16 port);
    
    // ==================== 错误和状态信号 ====================
    
    /**
     * @brief 发生错误
     * @param errorCode 错误码
     * @param errorMessage 错误信息
     */
    void errorOccurred(int errorCode, const QString& errorMessage);
    
    /**
     * @brief 一般信息
     * @param message 消息文本
     */
    void infoMessage(const QString& message);
    
    /**
     * @brief 日志消息
     * @param message 日志文本
     */
    void logMessage(const QString& message);
    
    // ==================== 打印相关信号 ====================
    
    /**
     * @brief 打印进度更新
     * @param progress 进度百分比（0-100）
     * @param currentLayer 当前层
     * @param totalLayers 总层数
     */
    void printProgressUpdated(int progress, int currentLayer, int totalLayers);
    
    /**
     * @brief 打印状态改变
     * @param status 状态描述
     */
    void printStatusChanged(const QString& status);
    
    // ==================== 运动相关信号 ====================
    
    /**
     * @brief 运动状态改变
     * @param status 状态描述
     */
    void moveStatusChanged(const QString& status);
    
    /**
     * @brief 位置更新
     * @param x X坐标
     * @param y Y坐标
     * @param z Z坐标
     */
    void positionUpdated(double x, double y, double z);

private:
    class Private;
    Private* d;  // Pimpl模式，隐藏实现细节
};

#endif // PRINTDEVICECONTROLLER_H
```

### 2. PrintDeviceController.cpp（Qt接口实现）

```cpp
/**
 * @file PrintDeviceController.cpp
 * @brief Qt风格SDK接口的实现
 */

#include "PrintDeviceController.h"
#include "PrintDeviceSDK_API.h"  // 复用C接口的底层实现
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>

/**
 * @class PrintDeviceController::Private
 * @brief 私有实现类（Pimpl模式）
 */
class PrintDeviceController::Private
{
public:
    Private(PrintDeviceController* q) 
        : q_ptr(q)
        , initialized(false)
        , connectedState(false)
    {
    }
    
    PrintDeviceController* q_ptr;
    bool initialized;
    bool connectedState;
    QString ip;
    quint16 port;
    
    // 静态回调函数（桥接到Qt信号）
    static void sdkEventCallback(const SdkEvent* event);
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
        return true;  // 已经初始化
    }
    
    // 调用C接口初始化
    int ret = InitSDK(logDir.isEmpty() ? nullptr : logDir.toUtf8().constData());
    if (ret != 0) {
        emit errorOccurred(-1, tr("SDK初始化失败"));
        return false;
    }
    
    // 注册回调函数
    RegisterEventCallback(&Private::sdkEventCallback);
    
    d->initialized = true;
    emit infoMessage(tr("SDK初始化成功"));
    return true;
}

void PrintDeviceController::release()
{
    if (!d->initialized) {
        return;
    }
    
    ReleaseSDK();
    d->initialized = false;
    d->connectedState = false;
}

// ==================== 连接管理 ====================

bool PrintDeviceController::connectToDevice(const QString& ip, quint16 port)
{
    if (!d->initialized) {
        emit errorOccurred(-1, tr("SDK未初始化"));
        return false;
    }
    
    int ret = ConnectByTCP(ip.toUtf8().constData(), port);
    if (ret != 0) {
        emit errorOccurred(ret, tr("连接失败"));
        return false;
    }
    
    d->ip = ip;
    d->port = port;
    emit deviceIpChanged(ip);
    emit devicePortChanged(port);
    
    return true;
}

void PrintDeviceController::disconnectFromDevice()
{
    if (!d->initialized) {
        return;
    }
    
    Disconnect();
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

// ==================== 运动控制 ====================

bool PrintDeviceController::moveTo(double x, double y, double z, double speed)
{
    if (!isConnected()) {
        emit errorOccurred(-1, tr("设备未连接"));
        return false;
    }
    
    int ret = MoveTo(x, y, z, speed);
    return ret == 0;
}

bool PrintDeviceController::moveBy(double dx, double dy, double dz, double speed)
{
    if (!isConnected()) {
        emit errorOccurred(-1, tr("设备未连接"));
        return false;
    }
    
    int ret = MoveBy(dx, dy, dz, speed);
    return ret == 0;
}

bool PrintDeviceController::goHome()
{
    if (!isConnected()) {
        emit errorOccurred(-1, tr("设备未连接"));
        return false;
    }
    
    int ret = GoHome();
    if (ret == 0) {
        emit moveStatusChanged(tr("回原点"));
    }
    return ret == 0;
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
    
    int ret = LoadPrintData(filePath.toUtf8().constData());
    if (ret == 0) {
        emit infoMessage(tr("图像数据已加载"));
    }
    return ret == 0;
}

bool PrintDeviceController::startPrint()
{
    if (!isConnected()) {
        emit errorOccurred(-1, tr("设备未连接"));
        return false;
    }
    
    int ret = StartPrint();
    if (ret == 0) {
        emit printStatusChanged(tr("打印开始"));
    }
    return ret == 0;
}

bool PrintDeviceController::pausePrint()
{
    int ret = PausePrint();
    if (ret == 0) {
        emit printStatusChanged(tr("打印暂停"));
    }
    return ret == 0;
}

bool PrintDeviceController::resumePrint()
{
    int ret = ResumePrint();
    if (ret == 0) {
        emit printStatusChanged(tr("打印恢复"));
    }
    return ret == 0;
}

bool PrintDeviceController::stopPrint()
{
    int ret = StopPrint();
    if (ret == 0) {
        emit printStatusChanged(tr("打印停止"));
    }
    return ret == 0;
}

// ==================== 回调函数（桥接到Qt信号）====================

void PrintDeviceController::Private::sdkEventCallback(const SdkEvent* event)
{
    QMutexLocker locker(&s_mutex);
    
    if (!s_instance) {
        return;
    }
    
    // 将C回调转换为Qt信号（自动线程安全）
    QString message = QString::fromUtf8(event->message);
    
    // 使用QMetaObject::invokeMethod确保信号在正确的线程中发射
    QMetaObject::invokeMethod(s_instance, [=]() {
        switch (event->type) {
        case EVENT_TYPE_GENERAL:
            if (message.contains("Connected", Qt::CaseInsensitive)) {
                s_instance->d->connectedState = true;
                emit s_instance->connected();
                emit s_instance->connectedChanged(true);
            } else if (message.contains("Disconnected", Qt::CaseInsensitive)) {
                s_instance->d->connectedState = false;
                emit s_instance->disconnected();
                emit s_instance->connectedChanged(false);
            }
            emit s_instance->infoMessage(message);
            break;
            
        case EVENT_TYPE_ERROR:
            emit s_instance->errorOccurred(event->code, message);
            break;
            
        case EVENT_TYPE_PRINT_STATUS:
            emit s_instance->printProgressUpdated(
                static_cast<int>(event->value1),
                static_cast<int>(event->value2),
                static_cast<int>(event->value3));
            break;
            
        case EVENT_TYPE_MOVE_STATUS:
            emit s_instance->moveStatusChanged(message);
            if (event->value1 != 0 || event->value2 != 0 || event->value3 != 0) {
                emit s_instance->positionUpdated(
                    event->value1, event->value2, event->value3);
            }
            break;
            
        case EVENT_TYPE_LOG:
            emit s_instance->logMessage(message);
            break;
        }
    }, Qt::QueuedConnection);
}
```

### 3. 更新PrintDeviceSDK.pro

```qmake
# 添加Qt接口文件
HEADERS += \
    PrintDeviceSDK_API.h \
    PrintDeviceController.h \    # 新增
    src/SDKManager.h

SOURCES += \
    PrintDeviceSDK_API.cpp \
    PrintDeviceController.cpp \  # 新增
    src/SDKManager.cpp \
    src/SDKConnection.cpp \
    src/SDKMotion.cpp \
    src/SDKPrint.cpp \
    src/SDKCallback.cpp

# 定义导出宏
DEFINES += PRINTDEVICESDK_EXPORTS PRINTDEVICESDK_LIBRARY
```

---

## 📖 使用示例

### Qt项目中使用Qt接口

```cpp
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "PrintDeviceController.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 创建控制器
    PrintDeviceController controller;
    
    // 初始化
    if (!controller.initialize("./logs")) {
        qCritical() << "初始化失败";
        return -1;
    }
    
    // 创建简单UI
    QWidget window;
    QVBoxLayout* layout = new QVBoxLayout(&window);
    
    QLabel* statusLabel = new QLabel("未连接");
    QPushButton* connectBtn = new QPushButton("连接设备");
    QPushButton* printBtn = new QPushButton("开始打印");
    
    layout->addWidget(statusLabel);
    layout->addWidget(connectBtn);
    layout->addWidget(printBtn);
    
    // 连接信号槽（Qt风格，简洁优雅）✨
    QObject::connect(&controller, &PrintDeviceController::connected, [=]() {
        statusLabel->setText("已连接");
        printBtn->setEnabled(true);
    });
    
    QObject::connect(&controller, &PrintDeviceController::disconnected, [=]() {
        statusLabel->setText("已断开");
        printBtn->setEnabled(false);
    });
    
    QObject::connect(&controller, &PrintDeviceController::errorOccurred, 
                    [](int code, const QString& msg) {
        qWarning() << "错误:" << code << msg;
    });
    
    QObject::connect(&controller, &PrintDeviceController::printProgressUpdated,
                    [=](int progress, int current, int total) {
        statusLabel->setText(QString("打印进度: %1% (%2/%3)")
            .arg(progress).arg(current).arg(total));
    });
    
    // 按钮点击
    QObject::connect(connectBtn, &QPushButton::clicked, [&]() {
        controller.connectToDevice("192.168.100.57", 5555);
    });
    
    QObject::connect(printBtn, &QPushButton::clicked, [&]() {
        controller.startPrint();
    });
    
    window.show();
    return app.exec();
}
```

**对比C接口使用**：

```cpp
// C接口方式（复杂）
void MyCallback(const SdkEvent* event) {
    // 需要手动判断事件类型
    // 需要考虑线程安全
    // 需要手动更新UI
}

InitSDK("./logs");
RegisterEventCallback(MyCallback);
ConnectByTCP("192.168.100.57", 5555);

// Qt接口方式（简洁）✨
PrintDeviceController controller;
controller.initialize("./logs");
connect(&controller, &PrintDeviceController::connected, []() {
    // Qt自动处理线程安全
    // 自动在UI线程更新
});
controller.connectToDevice("192.168.100.57", 5555);
```

---

## 📊 对比总结

### 代码量对比

| 操作 | C接口 | Qt接口 | 减少 |
|------|-------|--------|------|
| 初始化 | 3行 | 1行 | ↓67% |
| 注册回调 | 10行+ | 3行 | ↓70% |
| 连接设备 | 5行 | 2行 | ↓60% |
| 处理事件 | 30行+ | 5行 | ↓83% |

### 功能对比

| 功能 | C接口 | Qt接口 | 优势 |
|------|-------|--------|------|
| 信号槽 | ❌ | ✅ | Qt接口 |
| 线程安全 | 手动 | 自动 | Qt接口 |
| 类型安全 | char* | QString | Qt接口 |
| 跨语言 | ✅ | ❌ | C接口 |
| Qt集成 | 需包装 | 原生 | Qt接口 |

---

## ✅ 推荐实施方案

### 方案：双接口共存 ✨

**实施步骤**：

1. **保留C接口** - 不修改现有代码
2. **添加Qt接口** - 新增2个文件
3. **共享底层实现** - 两个接口都调用相同的SDK核心
4. **更新项目配置** - 在.pro文件中添加新文件

**优势**：
- ✅ 不影响现有C接口
- ✅ Qt项目可用更好的接口
- ✅ 实施简单（只需添加文件）
- ✅ 维护成本低（共享实现）

---

## 📋 是否需要修改原代码？

### ❌ 不需要修改原代码！

**原因**：
1. **添加而非替换** - 新增Qt接口，保留C接口
2. **共享实现** - Qt接口内部调用C接口
3. **向后兼容** - 不影响现有使用C接口的代码
4. **灵活选择** - 用户可根据需求选择接口

### 需要做的事情：

#### 1. 创建新文件（2个）
- [x] `PrintDeviceController.h` - Qt接口头文件
- [x] `PrintDeviceController.cpp` - Qt接口实现

#### 2. 更新项目文件（1处）
- [ ] 在`PrintDeviceSDK.pro`中添加新文件

#### 3. 更新文档（1个）
- [ ] 添加Qt接口使用说明

**工作量评估**: 约1-2小时

---

## 📚 后续文档

创建以下文档：

1. **Qt接口使用指南** - 详细使用方法
2. **接口对比文档** - C vs Qt接口
3. **迁移指南** - 从C接口迁移到Qt接口

---

**总结**: 建议采用**双接口共存**方案，无需修改原代码，只需添加新文件即可！

