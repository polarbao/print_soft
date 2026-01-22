# motionControlSDK 单例改造分析与实现方案

**文档版本**: 1.0  
**创建日期**: 2026-01-21  
**适用版本**: print_soft_0_0_1

---

## 📋 目录

1. [是否需要改为单例模式](#1-是否需要改为单例模式)
2. [CNewSingleton 模板适用性分析](#2-cnewsingleton-模板适用性分析)
3. [推荐实现方案](#3-推荐实现方案)
4. [完整代码实现](#4-完整代码实现)
5. [迁移指南](#5-迁移指南)
6. [测试验证](#6-测试验证)

---

## 1. 是否需要改为单例模式

### 1.1 当前问题分析

**当前代码存在的问题**:

```cpp
// motionControlSDK.cpp 当前实现
class motionControlSDK::Private {
    static motionControlSDK* s_instance;  // ⚠️ 静态实例指针
    static QMutex s_mutex;
};

motionControlSDK::motionControlSDK(QObject *parent)
    : QObject(parent), d(new Private(this))
{
    QMutexLocker locker(&Private::s_mutex);
    Private::s_instance = this;  // ⚠️ 每次构造都会覆盖
    // ...
}
```

**问题**:
1. ❌ **允许多次实例化**: 用户可以创建多个 `motionControlSDK` 对象
2. ❌ **野指针风险**: 后创建的对象会覆盖 `s_instance`，删除旧对象后变成野指针
3. ❌ **资源冲突**: 多个实例会导致资源冲突（TCP 连接、日志系统、配置文件）
4. ❌ **回调混乱**: 全局回调函数 `sdkEventCallback` 只能指向一个实例

**危险场景**:
```cpp
// 场景 1: 多次创建导致野指针
motionControlSDK* sdk1 = new motionControlSDK();  // s_instance = sdk1
motionControlSDK* sdk2 = new motionControlSDK();  // s_instance = sdk2 (覆盖!)
delete sdk1;  // s_instance 现在指向已删除的对象
// 回调触发 -> 访问 s_instance -> 崩溃! 💥

// 场景 2: 资源冲突
motionControlSDK sdk1;
sdk1.MC_Init();
sdk1.MC_Connect2Dev("192.168.1.100", 5555);  // TCP 连接成功

motionControlSDK sdk2;
sdk2.MC_Init();  // ❌ 重复初始化 SDKManager（已经是单例）
sdk2.MC_Connect2Dev("192.168.1.100", 5555);  // ❌ TCP 端口冲突
```

### 1.2 是否需要单例？

**✅ 强烈建议改为单例模式**

**理由**:

#### 理由 1: 设计意图已经是单例
- 代码中已经使用 `s_instance` 静态指针，说明原始设计意图就是单例
- 回调函数 `sdkEventCallback` 依赖唯一的实例指针

#### 理由 2: 底层 SDKManager 是单例
```cpp
// SDKManager 已经是单例
class SDKManager {
    static SDKManager* GetInstance();
private:
    SDKManager();  // 私有构造
};
```
如果 `motionControlSDK` 不是单例，会导致：
- 多个 `motionControlSDK` 实例共享同一个 `SDKManager`
- 状态管理混乱（连接状态、运动状态等）

#### 理由 3: 资源唯一性要求
- **TCP 连接**: 同一时间只能有一个连接到设备
- **日志系统**: SpdlogWrapper 也是单例
- **配置文件**: 只能有一个实例管理配置
- **全局回调**: 只能注册一个回调函数

#### 理由 4: 防止误用
- SDK 作为库提供给用户，单例模式可以防止用户错误地创建多个实例
- 提供更清晰的 API 语义

### 1.3 架构一致性

| 类 | 当前模式 | 建议模式 | 理由 |
|------|----------|---------|------|
| `SDKManager` | ✅ 单例 | 单例 | 管理全局资源 |
| `SpdlogWrapper` | ✅ 单例 | 单例 | 日志系统 |
| `CMotionConfig` | ✅ 单例 | 单例 | 配置管理 |
| `motionControlSDK` | ❌ 非单例 | **✅ 单例** | **对外 API 层** |

**结论**: 为了保持架构一致性，`motionControlSDK` 应该改为单例模式。

---

## 2. CNewSingleton 模板适用性分析

### 2.1 CNewSingleton 模板分析

```cpp
// CNewSingleton.h 的实现
template <typename T>
class CNewSingleton {
protected:
    CNewSingleton() = default;
    CNewSingleton(const CNewSingleton<T>&) = delete;
    CNewSingleton& operator=(const CNewSingleton<T>& st) = delete;
    
    static std::shared_ptr<T> m_instance;
    
public:
    static std::shared_ptr<T> GetInstance() {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
            m_instance = std::shared_ptr<T>(new T);
        });
        return m_instance;
    }
    
    ~CNewSingleton() {};
};
```

**特点**:
- ✅ 使用 `std::once_flag` 保证线程安全
- ✅ 使用 `std::shared_ptr` 自动管理内存
- ✅ 禁止拷贝和赋值
- ❌ 返回 `shared_ptr`（与 Qt 对象管理冲突）
- ❌ 默认构造函数无参数（QObject 需要 parent）

### 2.2 为什么不能使用 CNewSingleton？

#### 问题 1: QObject 不能用 shared_ptr

```cpp
// ❌ 错误示例
class motionControlSDK : public QObject, 
                         public CNewSingleton<motionControlSDK> {
    Q_OBJECT
};

// 使用时
std::shared_ptr<motionControlSDK> sdk = motionControlSDK::GetInstance();
```

**问题**:
- QObject 使用父子对象树管理内存，不能用 `shared_ptr`
- Qt 文档明确警告: "Do not use QSharedPointer with QObject"
- 会导致双重释放（Qt 父对象删除 + shared_ptr 释放）

#### 问题 2: 多重继承与 Q_OBJECT 宏

```cpp
// ❌ 编译错误
class motionControlSDK : public QObject, 
                         public CNewSingleton<motionControlSDK> {
    Q_OBJECT  // ❌ Q_OBJECT 宏与多重继承冲突
};
```

**问题**:
- `Q_OBJECT` 宏要求类只能有一个基类继承 QObject
- 多重继承会导致 `moc` 编译器报错

#### 问题 3: 构造函数参数

```cpp
// motionControlSDK 需要 parent 参数
explicit motionControlSDK(QObject *parent = nullptr);

// 但 CNewSingleton 只支持无参构造
std::shared_ptr<T>(new T);  // ❌ 没有传递 parent
```

#### 问题 4: 信号槽机制

```cpp
// motionControlSDK 使用信号槽
signals:
    void MC_SigErrOccurred(int errorCode, const QString& errorMessage);
    void connected();

// 如果使用 shared_ptr，信号槽的生命周期管理会变得复杂
```

### 2.3 结论

**❌ 不推荐使用 CNewSingleton 模板**

**原因总结**:
1. QObject 与 `shared_ptr` 不兼容
2. 多重继承导致 Q_OBJECT 宏冲突
3. 构造函数参数不匹配
4. Qt 信号槽机制的特殊要求

**推荐**: 使用传统的 Qt 单例模式（类似 SDKManager 的实现）

---

## 3. 推荐实现方案

### 3.1 方案选择

有三种 Qt 单例实现方案：

#### 方案 A: Meyer's Singleton（推荐）⭐

```cpp
class motionControlSDK : public QObject {
    Q_OBJECT
public:
    static motionControlSDK* GetInstance() {
        static motionControlSDK instance;  // C++11 线程安全
        return &instance;
    }
    
private:
    motionControlSDK();  // 私有构造
    ~motionControlSDK();
    Q_DISABLE_COPY(motionControlSDK)
};
```

**优点**:
- ✅ C++11 保证线程安全（编译器自动加锁）
- ✅ 简单清晰，无需手动管理内存
- ✅ 延迟初始化（首次调用时才创建）
- ✅ 自动释放（程序结束时）

**缺点**:
- ⚠️ 无法手动控制销毁时机
- ⚠️ 无法传递构造参数

#### 方案 B: 静态指针 + 锁

```cpp
class motionControlSDK : public QObject {
    Q_OBJECT
public:
    static motionControlSDK* GetInstance() {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!s_instance) {
            s_instance = new motionControlSDK();
        }
        return s_instance;
    }
    
    static void DestroyInstance() {
        QMutexLocker locker(&s_mutex);
        delete s_instance;
        s_instance = nullptr;
    }
    
private:
    motionControlSDK();
    static motionControlSDK* s_instance;
    static QMutex s_mutex;
};
```

**优点**:
- ✅ 可以手动控制销毁
- ✅ 可以在销毁后重新创建

**缺点**:
- ⚠️ 需要手动管理内存
- ⚠️ 更复杂的代码

#### 方案 C: Q_GLOBAL_STATIC（Qt 宏）

```cpp
Q_GLOBAL_STATIC(motionControlSDK, sdk_instance)

// 使用
motionControlSDK* sdk = sdk_instance();
```

**优点**:
- ✅ Qt 官方推荐
- ✅ 线程安全

**缺点**:
- ⚠️ 需要在 .cpp 文件中使用
- ⚠️ 不够灵活

### 3.2 最终选择：方案 A（Meyer's Singleton）

**理由**:
1. **简单性**: 代码最简洁，易于理解和维护
2. **安全性**: C++11 保证线程安全，无需手动加锁
3. **一致性**: 与 `SDKManager` 的实现方式一致
4. **Qt 兼容**: 完全兼容 Qt 对象树管理
5. **生命周期**: SDK 通常在程序整个生命周期内存在，自动释放即可

---

## 4. 完整代码实现

### 4.1 头文件修改 (motionControlSDK.h)

```cpp
#pragma once

#include "motioncontrolsdk_define.h"

/**
 * @class motionControlSDK
 * @brief Motion Control SDK 主接口类（单例模式）
 * 
 * @details
 * 该类采用单例模式，确保整个应用程序中只有一个 SDK 实例。
 * 
 * @note 使用方式:
 * @code
 * auto* sdk = motionControlSDK::GetInstance();
 * sdk->MC_Init();
 * sdk->MC_Connect2Dev("192.168.1.100", 5555);
 * @endcode
 * 
 * @warning 不要尝试创建多个实例，构造函数已私有化
 */
class MOTIONCONTROLSDK_EXPORT motionControlSDK : public QObject
{
    Q_OBJECT

    // Qt属性定义
    Q_PROPERTY(bool connected READ MC_IsConnected NOTIFY MC_SigConnectedChanged)

public:
    /**
     * @brief 获取单例实例
     * @return SDK 实例指针（永不为空）
     * 
     * @note 线程安全（C++11 保证）
     * @note 首次调用时创建实例，后续调用返回同一实例
     * 
     * @code
     * // 推荐用法
     * auto* sdk = motionControlSDK::GetInstance();
     * sdk->MC_Init();
     * @endcode
     */
    static motionControlSDK* GetInstance();

    /**
     * @brief 析构函数
     * @note 自动释放SDK资源（程序结束时）
     * @warning 不要手动 delete，由系统自动管理
     */
    ~motionControlSDK();

    // ==================== 生命周期管理 ====================

    /**
     * @brief 初始化SDK
     * @param logDir 日志目录（可为空）
     * @return true=成功, false=失败
     */
    bool MC_Init(const QString& logDir = QString());

    /**
     * @brief 释放SDK资源
     * @note 通常无需手动调用，析构时自动释放
     */
    void MC_Release();

    /**
     * @brief 检查SDK是否已初始化
     * @return true=已初始化, false=未初始化
     */
    bool MC_IsInit() const;

    // ==================== 连接管理 ====================

    /**
     * @brief 连接设备
     * @param ip 设备IP地址
     * @param port 端口号（默认12355）
     * @return true=成功发起连接, false=失败
     * @note 连接结果通过connected()或MC_SigErrOccurred()信号通知
     */
    bool MC_Connect2Dev(const QString& ip, quint16 port = 12355);

    /**
     * @brief 断开连接
     * @note 断开结果通过MC_SigDisconnected()信号通知
     */
    void MC_DisconnectDev();

    /**
     * @brief 查询连接状态
     * @return true=已连接, false=未连接
     */
    bool MC_IsConnected() const;

    /**
     * @brief 获取设备IP
     * @return IP地址字符串
     */
    QString MC_GetDevIp() const;

    /**
     * @brief 获取设备端口
     * @return 端口号
     */
    quint16 MC_GetDevPort() const;

    // ==================== 运动控制配置信息 ====================
    
    bool MC_LoadMotionConfig(MotionConfig& config, const QString& path="config/motionMoudleConfig.ini");
    bool MC_LoadCurrentMotionConfig(MotionConfig& config);
    bool MC_SetMotionConfig(const MotionConfig& config);
    bool MC_SetPrintStartPos(const MoveAxisPos& data);
    bool MC_SetPrintEndPos(const MoveAxisPos& data);
    bool MC_SetPrintStep(const MoveAxisPos& data);
    bool MC_SetLayerNumData(quint32 passNum, quint32 zStep);
    bool MC_SetOffsetData(quint32 xOffset, quint32 yOffset, quint32 zOffset);

    // ==================== 运动控制 ====================
    
    bool MC_GoHome(int x, int y, int z);
    bool MC_moveXAxis(const MoveAxisPos& targetPos);
    bool MC_moveYAxis(const MoveAxisPos& targetPos);
    bool MC_moveZAxis(const MoveAxisPos& targetPos);
    bool MC_move2RelSingleAxisPos(double dx, double dy, double dz);
    bool MC_move2AbsSingleAxisPos(const MoveAxisPos& targetPos);
    bool MC_move2RelAxisPos(double dx, double dy, double dz);
    bool MC_move2AbsAxisPos(const MoveAxisPos& targetPos);
    bool MC_move2AbsAxisPos(const QByteArray& targetPos);
    bool MC_SendData(int cmdType, const QByteArray& data);

    // ==================== 打印控制 ====================
    
    bool MC_StartPrint();
    bool MC_PausePrint();
    bool MC_ResumePrint();
    bool MC_StopPrint();
    bool MC_CleanPrint();
    void MC_SendCmd(int operCmd, const QByteArray& arrData);
    bool MC_PrtMoveLayer(quint32 layerIdx, quint32 passIdx);

public slots:
    void refreshConnectionStatus();

signals:
    // ==================== 连接相关信号 ====================
    void connected();
    void MC_SigDisconnected();
    void MC_SigConnectedChanged(bool isConnected);

    // ==================== 错误和状态信号 ====================
    void MC_SigErrOccurred(int errorCode, const QString& errorMessage);
    void MC_SigInfoMsg(const QString& message);
    void MC_SigLogMsg(const QString& message);
    void MC_SigSend2DevCmdMsg(const QString& message);
    void MC_SigRecv2DevCmdMsg(const QString& message);

    // ==================== 打印相关信号 ====================
    void MC_SigPrintProgUpdated(int progress, int currentLayer, int totalLayers);
    void MC_SigPrintStatusChangedText(const QString& status);
    void MC_SigPrintPassFinished(const QString& msg);

    // ==================== 运动相关信号 ====================
    void MC_SigMoveStatusChanged(const QString& status);
    void MC_SigPosChanged(double x, double y, double z);
    void MC_SigMove2OriginFinished(const QString& status);

private:
    /**
     * @brief 私有构造函数（单例模式）
     * @note 防止外部创建实例
     */
    explicit motionControlSDK();

    // 禁止拷贝和赋值
    Q_DISABLE_COPY(motionControlSDK)

    // Pimpl模式：隐藏实现细节
    class Private;
    Private* d;
};
```

### 4.2 实现文件修改 (motionControlSDK.cpp)

```cpp
#include "motionControlSDK.h"
#include "SDKManager.h"
#include "ProtocolPrint.h"

#include <QDebug>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QMetaObject>

#include "comm/CMotionConfig.h"
#include "SpdlogMgr.h"

/**
 * @class motionControlSDK::Private
 * @brief 私有实现类（Pimpl模式）
 */
class motionControlSDK::Private
{
public:
    explicit Private(motionControlSDK* q)
        : q_ptr(q)
        , initialized(false)
        , connectedState(false)
        , _bInit(false)
        , _bConnected(false)
        , _bPrinting(false)
    {
    }

    ~Private()
    {
    }

    // 静态回调函数（桥接C回调到Qt信号）
    static void sdkEventCallback(const SdkEvent* event);

    motionControlSDK* q_ptr;
    
    bool initialized;
    bool connectedState;
    bool _bInit;
    bool _bConnected;
    bool _bPrinting;
    
    MoveAxisPos _curPos;
    MotionConfig motionConfig;
    
    QString currentLocale{"zh_CN"};  // 当前语言
};

// ==================== 单例实现 ====================

motionControlSDK* motionControlSDK::GetInstance()
{
    // Meyer's Singleton（C++11 线程安全）
    static motionControlSDK instance;
    return &instance;
}

// ==================== 构造和析构 ====================

motionControlSDK::motionControlSDK()
    : QObject(nullptr)  // 单例不需要 parent
    , d(new Private(this))
{
    // spdlog初始化
    SpdlogWrapper::GetInstance()->Init("./logs", "sdk_log", 10 * 1024 * 1024, 5, true, true);
    auto netLogger = SpdlogWrapper::GetInstance()->GetLogger("netMoudle", "./logs/modules", 5 * 1024 * 1024, 3);
    auto logicLogger = SpdlogWrapper::GetInstance()->GetLogger("logicMoudle", "./logs/modules", 5 * 1024 * 1024, 3);
    NAMED_LOG_D("netMoudle", "motion_moudle_sdk net_moudle log_start");
    NAMED_LOG_D("logicMoudle", "motion_moudle_sdk logic_moudle log_start");
    
    SPDLOG_INFO("motionControlSDK singleton instance created");
}

motionControlSDK::~motionControlSDK()
{
    // 将配置信息重新写入到配置文件
    CMotionConfig::GetInstance()->save(d->motionConfig);

    MC_Release();

    delete d;
    
    SPDLOG_INFO("motionControlSDK singleton instance destroyed");
}

// ==================== 生命周期管理 ====================

bool motionControlSDK::MC_Init(const QString& logDir)
{
    qRegisterMetaType<MoveAxisPos>("MoveAxisPos");
    qRegisterMetaType<PackParam>("PackParam");

    if (d->initialized) 
    {
        SPDLOG_INFO("SDK已经初始化");
        return true;
    }

    // 初始化SDK
    bool ret = SDKManager::GetInstance()->Init("./");
    
    if (!ret) 
    {
        QString errMsg = tr("SDK初始化失败，错误码：%1").arg(ret);
        SPDLOG_INFO("SDK初始化失败");
        emit MC_SigErrOccurred(-1, errMsg);
        return false;
    }

    // 注册事件回调函数（桥接C回调到Qt信号）
    QMutexLocker lock(&g_callbackMutex);
    g_sdkCallback = &Private::sdkEventCallback;

    // 从配置文件加载运动控制配置
    if (!MC_LoadMotionConfig(d->motionConfig))
    {
        NAMED_LOG_E("logicMoudle", "motionControlSDK MC_LoadMotionConfig failed!");
        return false;
    }

    d->initialized = true;
    SPDLOG_INFO("motionControlSDK initialized successfully");
    return true;
}

void motionControlSDK::MC_Release()
{
    if (!d->initialized) 
    {
        return;
    }

    // 断开连接
    if (d->connectedState) 
    {
        MC_DisconnectDev();
    }

    // 释放底层SDK
    SDKManager::GetInstance()->Release();

    d->initialized = false;
    d->connectedState = false;

    SPDLOG_INFO("motion_moudle motionControlSDK released");
}

bool motionControlSDK::MC_IsInit() const
{
    return d->initialized;
}

// ==================== 连接管理 ====================

bool motionControlSDK::MC_Connect2Dev(const QString& ip, quint16 port)
{
    SPDLOG_TRACE("motion_moudle motionControlSDK connect_dev");

    if (!d->initialized) 
    {
        SPDLOG_ERROR("SDK未初始化，请先调用MC_Init()");
        return false;
    }

    if (d->connectedState) 
    {
        return true;
    }

    // 调用C接口连接设备
    int ret = SDKManager::GetInstance()->Connect2MotionDev(ip, port);
    if (ret != 0) 
    {
        QString errMsg = tr("连接失败，错误码：%1").arg(ret);
        SPDLOG_INFO("连接失败，错误码: {}", ret);
        emit MC_SigErrOccurred(ret, errMsg);
        return false;
    }

    // 保存连接信息
    d->motionConfig.ip = ip;
    d->motionConfig.port = port;

    emit MC_SigInfoMsg(tr("正在连接 %1:%2...").arg(ip).arg(port));
    SPDLOG_INFO("正在连接 {} {}...", ip, port);

    return true;
}

void motionControlSDK::MC_DisconnectDev()
{
    if (!d->initialized) 
    {
        return;
    }

    if (!d->connectedState) 
    {
        emit MC_SigInfoMsg(tr("设备未连接"));
        return;
    }

    SDKManager::GetInstance()->Disconnect();
    emit MC_SigInfoMsg(tr("正在断开连接..."));
}

bool motionControlSDK::MC_IsConnected() const
{
    return d->connectedState;
}

QString motionControlSDK::MC_GetDevIp() const
{
    return d->motionConfig.ip;
}

quint16 motionControlSDK::MC_GetDevPort() const
{
    return d->motionConfig.port;
}

// ... (其他方法实现保持不变) ...

// ==================== 回调函数（桥接C回调到Qt信号）====================

void motionControlSDK::Private::sdkEventCallback(const SdkEvent* event)
{
    if (!event) 
    {
        return;
    }

    // 获取单例实例
    motionControlSDK* instance = motionControlSDK::GetInstance();
    
    // 将C回调转换为Qt信号
    QString msg = QString::fromUtf8(event->message);
    SdkEventType type = event->type;
    int code = event->code;
    double v1 = event->value1;
    double v2 = event->value2;
    double v3 = event->value3;

    // 使用Qt::QueuedConnection确保在主线程中执行
    QMetaObject::invokeMethod(instance, [=]() 
    {
        switch (type) 
        {
            case EVENT_TYPE_GENERAL: 
            {
                // 检测连接状态变化
                if (msg.contains("connected_2_dev", Qt::CaseInsensitive))
                {
                    instance->d->connectedState = true;
                    instance->MC_SetMotionConfig(instance->d->motionConfig);
                    emit instance->connected();
                    emit instance->MC_SigConnectedChanged(true);
                    SPDLOG_INFO("motion_moudle sdk_connected_dev");
                }
                else if (msg.contains("disconnected_from_dev", Qt::CaseInsensitive))
                {
                    instance->d->connectedState = false;
                    emit instance->MC_SigDisconnected();
                    emit instance->MC_SigConnectedChanged(false);
                    SPDLOG_INFO("motion_moudle sdk_disconnected_dev");
                }
                emit instance->MC_SigInfoMsg(msg);
                break;
            }

            case EVENT_TYPE_ERROR: 
            {
                emit instance->MC_SigErrOccurred(code, msg);
                SPDLOG_INFO("SDK Error:{} {} ", code, msg);
                break;
            }

            case EVENT_TYPE_PRINT_STATUS: 
            {
                int xData = static_cast<int>(v1);
                int passIdx = static_cast<int>(v2);
                int curLayerIdx = static_cast<int>(v3);

                if (msg.contains("print_pass_unit_finished", Qt::CaseInsensitive))
                {
                    emit instance->MC_SigPrintPassFinished(msg);
                    emit instance->MC_SigPrintProgUpdated(xData, passIdx, curLayerIdx);
                    SPDLOG_INFO("motion_moudle SDK_print_pass_unit_finished: {}", msg);
                    break;
                }
                
                emit instance->MC_SigPrintStatusChangedText(msg);
                SPDLOG_INFO("motion_moudle Print progress:{} %{} /{}", xData, passIdx, curLayerIdx);
                break;
            }

            case EVENT_TYPE_MOVE_STATUS: 
            {
                if (msg.contains("move_origin_finished", Qt::CaseInsensitive))
                {
                    emit instance->MC_SigMove2OriginFinished(msg);
                    SPDLOG_INFO("motion_moudle SDK_move_2_origin_pos_finished: {}", msg);
                    break;
                }
                emit instance->MC_SigMoveStatusChanged(msg);
                if (v1 != 0 || v2 != 0 || v3 != 0)
                {
                    emit instance->MC_SigPosChanged(v1, v2, v3);
                    SPDLOG_INFO("motion_moudle Position:数据区1:{} 数据区2:{} 数据区3:{} ", v1, v2, v3);
                }
                break;
            }

            case EVENT_TYPE_LOG: 
            {
                emit instance->MC_SigLogMsg(msg);
                SPDLOG_INFO("motion_moudle SDK_Log: {}", msg);
                break;
            }

            case EVENT_TYPE_SEND_MSG:
            {
                emit instance->MC_SigSend2DevCmdMsg(msg);
                SPDLOG_INFO("motion_moudle sdk_send_hex_msg: {}", msg);
                break;
            }

            case EVENT_TYPE_RECV_MSG:
            {
                emit instance->MC_SigRecv2DevCmdMsg(msg);
                SPDLOG_INFO("motion_moudle sdk_recv_hex_msg: {}", msg);
                break;
            }

            default: 
            {
                qWarning() << "Unknown event type:" << type;
                break;
            }
        }
    }, Qt::QueuedConnection);
}

// ... (其他运动控制、打印控制方法实现保持不变) ...
```

---

## 5. 迁移指南

### 5.1 用户代码迁移

#### 旧代码（多实例）

```cpp
// ❌ 旧代码：允许创建多个实例
class MyApp {
private:
    motionControlSDK* sdk;  // 成员变量
};

MyApp::MyApp() {
    sdk = new motionControlSDK(this);  // 每个 MyApp 创建一个 SDK
    sdk->MC_Init();
}

MyApp::~MyApp() {
    delete sdk;
}
```

#### 新代码（单例）

```cpp
// ✅ 新代码：使用单例
class MyApp {
    // 不需要成员变量
};

MyApp::MyApp() {
    auto* sdk = motionControlSDK::GetInstance();
    sdk->MC_Init();
}

MyApp::~MyApp() {
    // 不需要手动释放
}
```

### 5.2 迁移步骤

#### 步骤 1: 全局搜索替换

在整个项目中搜索 `new motionControlSDK` 和 `delete`：

```bash
# 查找所有创建实例的地方
grep -rn "new motionControlSDK" ./src/

# 查找所有删除实例的地方
grep -rn "delete.*sdk" ./src/
```

#### 步骤 2: 替换创建代码

**替换模式**:
```cpp
// 旧代码
motionControlSDK* sdk = new motionControlSDK(this);

// 新代码
auto* sdk = motionControlSDK::GetInstance();
```

#### 步骤 3: 移除删除代码

**删除所有 `delete sdk` 相关代码**:
```cpp
// 旧代码
delete m_sdk;
m_sdk = nullptr;

// 新代码
// 完全移除，单例自动管理
```

#### 步骤 4: 更新成员变量

**替换模式**:
```cpp
// 旧代码
class MyClass {
private:
    motionControlSDK* m_sdk;
};

// 新代码（不需要成员变量）
class MyClass {
    // 直接使用 motionControlSDK::GetInstance()
};
```

### 5.3 常见迁移场景

#### 场景 1: UI 类

**旧代码**:
```cpp
// printDeviceUI.h
class PrintDeviceUI : public QWidget {
private:
    motionControlSDK* m_sdk;
};

// printDeviceUI.cpp
PrintDeviceUI::PrintDeviceUI(QWidget* parent)
    : QWidget(parent)
{
    m_sdk = new motionControlSDK(this);
    connect(m_sdk, &motionControlSDK::connected, 
            this, &PrintDeviceUI::onConnected);
}
```

**新代码**:
```cpp
// printDeviceUI.h
class PrintDeviceUI : public QWidget {
    // 移除成员变量
};

// printDeviceUI.cpp
PrintDeviceUI::PrintDeviceUI(QWidget* parent)
    : QWidget(parent)
{
    auto* sdk = motionControlSDK::GetInstance();
    connect(sdk, &motionControlSDK::connected, 
            this, &PrintDeviceUI::onConnected);
}
```

#### 场景 2: 逻辑类

**旧代码**:
```cpp
// printLogic.cpp
void PrintLogic::startPrint() {
    m_sdk->MC_StartPrint();
}
```

**新代码**:
```cpp
// printLogic.cpp
void PrintLogic::startPrint() {
    motionControlSDK::GetInstance()->MC_StartPrint();
}
```

### 5.4 编译和链接

#### 更新后重新编译

```bash
# 清理旧的编译文件
cd build
cmake --build . --target clean

# 重新编译
cmake --build . --config Release
```

---

## 6. 测试验证

### 6.1 单元测试

```cpp
// test_motionControlSDK_singleton.cpp
#include <QtTest/QtTest>
#include "motionControlSDK.h"

class TestMotionControlSDKSingleton : public QObject
{
    Q_OBJECT
    
private slots:
    void testGetInstance() {
        // 测试获取实例
        auto* sdk1 = motionControlSDK::GetInstance();
        auto* sdk2 = motionControlSDK::GetInstance();
        
        // 验证是同一个实例
        QCOMPARE(sdk1, sdk2);
        QVERIFY(sdk1 != nullptr);
    }
    
    void testSingletonBehavior() {
        auto* sdk = motionControlSDK::GetInstance();
        
        // 初始化一次
        bool ret1 = sdk->MC_Init();
        QVERIFY(ret1);
        
        // 再次初始化应该返回 true（已初始化）
        bool ret2 = sdk->MC_Init();
        QVERIFY(ret2);
        
        // 验证状态一致
        QVERIFY(sdk->MC_IsInit());
    }
    
    void testThreadSafety() {
        // 测试多线程环境下的线程安全
        QVector<QThread*> threads;
        
        for (int i = 0; i < 10; ++i) {
            QThread* thread = QThread::create([]() {
                auto* sdk = motionControlSDK::GetInstance();
                // 所有线程应该获取同一个实例
                qDebug() << "Thread" << QThread::currentThread() 
                         << "got SDK instance:" << sdk;
            });
            threads.append(thread);
            thread->start();
        }
        
        // 等待所有线程完成
        for (auto* thread : threads) {
            thread->wait();
            delete thread;
        }
    }
};

QTEST_MAIN(TestMotionControlSDKSingleton)
#include "test_motionControlSDK_singleton.moc"
```

### 6.2 功能测试

```cpp
// 测试完整工作流程
void testWorkflow() {
    auto* sdk = motionControlSDK::GetInstance();
    
    // 1. 初始化
    QVERIFY(sdk->MC_Init());
    
    // 2. 连接设备
    QVERIFY(sdk->MC_Connect2Dev("192.168.1.100", 5555));
    
    // 等待连接成功（通过信号）
    QSignalSpy spy(sdk, &motionControlSDK::connected);
    QVERIFY(spy.wait(5000));  // 等待5秒
    
    // 3. 检查连接状态
    QVERIFY(sdk->MC_IsConnected());
    
    // 4. 运动控制
    MoveAxisPos pos(100000, 0, 0);  // X轴移动100mm
    QVERIFY(sdk->MC_moveXAxis(pos));
    
    // 5. 断开连接
    sdk->MC_DisconnectDev();
}
```

### 6.3 集成测试

在实际应用中测试：

```cpp
// main.cpp
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 获取SDK单例
    auto* sdk = motionControlSDK::GetInstance();
    sdk->MC_Init();
    
    // 创建主窗口
    MainWindow w;
    w.show();
    
    return app.exec();
    
    // 程序结束时，单例自动释放
}
```

---

## 附录

### A. 迁移检查清单

- [ ] 修改 `motionControlSDK.h` 头文件（添加 GetInstance，私有化构造）
- [ ] 修改 `motionControlSDK.cpp` 实现文件（Meyer's Singleton）
- [ ] 移除所有 `Private::s_instance` 相关代码
- [ ] 搜索并替换所有 `new motionControlSDK` 为 `GetInstance()`
- [ ] 移除所有 `delete sdk` 代码
- [ ] 移除 UI/Logic 类中的 `motionControlSDK*` 成员变量
- [ ] 更新所有信号连接代码
- [ ] 重新编译整个项目
- [ ] 运行单元测试
- [ ] 运行集成测试
- [ ] 更新文档和示例代码

### B. 优势总结

| 方面 | 改造前 | 改造后 |
|------|--------|--------|
| **线程安全** | ❌ 不安全 | ✅ C++11 保证 |
| **资源冲突** | ❌ 可能冲突 | ✅ 唯一实例 |
| **内存管理** | ⚠️ 手动管理 | ✅ 自动管理 |
| **野指针风险** | ❌ 高风险 | ✅ 无风险 |
| **API 清晰度** | ⚠️ 中等 | ✅ 非常清晰 |
| **误用防护** | ❌ 无防护 | ✅ 编译期防护 |

### C. 常见问题

**Q: 为什么不能手动 delete 单例？**  
A: 单例在程序结束时自动释放。手动 delete 会导致程序崩溃。

**Q: 如何在多个线程中使用？**  
A: 直接调用 `GetInstance()` 即可，C++11 保证线程安全。

**Q: 单例会影响性能吗？**  
A: 不会。首次调用有轻微开销，后续调用直接返回指针，性能极高。

**Q: 如何重置单例状态？**  
A: 调用 `MC_Release()` 方法，但实例本身不会销毁。

---

**文档结束**

**维护者**: Development Team  
**最后更新**: 2026-01-21  
**版本**: 1.0
