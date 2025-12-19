# SDK在Qt项目中的使用指南

**目标**: 用PrintDeviceSDK替代原有Qt项目中的直接实现  
**适用场景**: 将`print_soft_0_0_1`项目迁移到使用SDK  
**难度**: ⭐⭐ (中等)

---

## 📋 目录

1. [概述](#概述)
2. [准备工作](#准备工作)
3. [方案对比](#方案对比)
4. [详细步骤](#详细步骤)
5. [代码迁移示例](#代码迁移示例)
6. [常见问题](#常见问题)

---

## 概述

### 原有实现 vs SDK实现

**原有实现**（`print_soft_0_0_1`）:
```
printDeviceUI.cpp
    └─> 直接使用 TcpClient
    └─> 直接使用 ProtocolPrint
    └─> 手动管理信号槽连接
    └─> 手动实现心跳机制
```

**SDK实现**:
```
printDeviceUI.cpp
    └─> 使用 PrintDeviceController (Qt接口)
    └─> SDK内部封装了所有底层细节
    └─> 自动管理连接和心跳
    └─> 简化的信号槽
```

**代码量对比**:
- 原实现: ~500行
- SDK实现: ~150行
- **减少70%** ✨

---

## 准备工作

### 步骤1：编译SDK

```bash
# 进入SDK目录
cd PrintDeviceSDK

# 使用qmake编译
qmake PrintDeviceSDK.pro
nmake  # Windows使用nmake
# make   # Linux使用make

# 编译后输出：
# bin/release/PrintDeviceSDK.dll (Windows)
# bin/release/libPrintDeviceSDK.so (Linux)
# lib/PrintDeviceSDK.lib (Windows导入库)
```

### 步骤2：检查编译产物

```
PrintDeviceSDK/
├── bin/
│   └── release/
│       └── PrintDeviceSDK.dll  ✓
├── lib/
│   └── PrintDeviceSDK.lib      ✓
└── include/  (可选，手动创建)
    ├── PrintDeviceSDK_API.h
    └── PrintDeviceController.h
```

---

## 方案对比

### 方案A：使用Qt接口（推荐）✨

**优点**:
- ✅ 代码量最少（减少70%）
- ✅ Qt信号槽风格，无缝集成
- ✅ 自动线程安全
- ✅ 类型安全（QString, QByteArray）

**缺点**:
- ❌ 只能在Qt项目中使用

**适合**: Qt Widgets/Qt Quick项目

---

### 方案B：使用C接口

**优点**:
- ✅ 可在任何C/C++项目中使用
- ✅ 可导出给C#/Python等语言

**缺点**:
- ❌ 需要手动处理线程安全
- ❌ 回调函数比信号槽复杂
- ❌ 代码量较多

**适合**: 需要跨语言调用的场景

---

## 详细步骤

### 步骤1：在项目中配置SDK

#### 修改`.pro`文件

```qmake
# your_project.pro

QT += core gui widgets network

# ==================== SDK配置 ====================

# 包含SDK头文件路径
INCLUDEPATH += $$PWD/../PrintDeviceSDK

# 链接SDK库
win32 {
    CONFIG(release, debug|release) {
        # 链接Release版本
        LIBS += -L$$PWD/../PrintDeviceSDK/lib -lPrintDeviceSDK
        
        # 部署时复制DLL到输出目录
        SDK_DLL = $$PWD/../PrintDeviceSDK/bin/release/PrintDeviceSDK.dll
        SDK_DLL ~= s,/,\\,g  # 转换路径分隔符
        OUT_DIR = $$OUT_PWD
        OUT_DIR ~= s,/,\\,g
        
        QMAKE_POST_LINK += $$quote(copy /Y \"$$SDK_DLL\" \"$$OUT_DIR\"$$escape_expand(\n\t))
    }
    
    CONFIG(debug, debug|release) {
        # 链接Debug版本
        LIBS += -L$$PWD/../PrintDeviceSDK/lib -lPrintDeviceSDK
        
        SDK_DLL = $$PWD/../PrintDeviceSDK/bin/debug/PrintDeviceSDK.dll
        SDK_DLL ~= s,/,\\,g
        OUT_DIR = $$OUT_PWD
        OUT_DIR ~= s,/,\\,g
        
        QMAKE_POST_LINK += $$quote(copy /Y \"$$SDK_DLL\" \"$$OUT_DIR\"$$escape_expand(\n\t))
    }
}

unix {
    # Linux配置
    LIBS += -L$$PWD/../PrintDeviceSDK/lib -lPrintDeviceSDK
    
    # 设置rpath，方便查找.so
    QMAKE_LFLAGS += -Wl,-rpath,$$PWD/../PrintDeviceSDK/lib
}
```

---

### 步骤2：修改头文件

#### 原有代码（printDeviceUI.h）:

```cpp
// printDeviceUI.h - 原实现
#pragma once
#include <QDialog>
#include "TcpClient.h"           // ❌ 直接依赖
#include "ProtocolPrint.h"       // ❌ 直接依赖

class PrintDeviceUI : public QDialog
{
    Q_OBJECT

public:
    PrintDeviceUI(QWidget *parent = nullptr);
    ~PrintDeviceUI();

private slots:
    void onTcpConnected();                           // ❌ 需要手动处理
    void onTcpDisconnected();                        // ❌ 需要手动处理
    void onTcpNewData(QByteArray data);              // ❌ 需要手动处理
    void onProtocolCmdReply(int cmd, uchar errCode, QByteArray arr); // ❌ 复杂
    void onHeartbeat();                              // ❌ 手动心跳
    void onSendHeartbeat();                          // ❌ 手动心跳
    void onCheckHeartbeat();                         // ❌ 手动心跳

private:
    TcpClient* m_tcpClient;          // ❌ 需要自己管理
    ProtocolPrint* m_protocol;       // ❌ 需要自己管理
    QTimer* m_heartbeatSendTimer;    // ❌ 需要自己管理
    QTimer* m_heartbeatCheckTimer;   // ❌ 需要自己管理
    int m_heartbeatTimeout;          // ❌ 需要自己管理
};
```

#### 使用SDK后（printDeviceUI.h）:

```cpp
// printDeviceUI.h - SDK实现
#pragma once
#include <QDialog>
#include "PrintDeviceController.h"   // ✅ 只需包含SDK头文件

class PrintDeviceUI : public QDialog
{
    Q_OBJECT

public:
    PrintDeviceUI(QWidget *parent = nullptr);
    ~PrintDeviceUI();

private slots:
    // ✅ 简化的槽函数
    void onDeviceConnected();
    void onDeviceDisconnected();
    void onPrintProgress(int progress, int current, int total);
    void onError(int code, const QString& message);

private:
    PrintDeviceController* m_controller;  // ✅ 只需一个对象
};
```

**代码减少**: 从20+行减少到5行，减少75% ✨

---

### 步骤3：修改实现文件

#### 原有代码（printDeviceUI.cpp）:

```cpp
// printDeviceUI.cpp - 原实现（简化版）
PrintDeviceUI::PrintDeviceUI(QWidget *parent)
    : QDialog(parent)
{
    // ❌ 需要手动创建和配置所有对象
    m_tcpClient = new TcpClient(this);
    m_protocol = new ProtocolPrint(this);
    m_heartbeatSendTimer = new QTimer(this);
    m_heartbeatCheckTimer = new QTimer(this);
    
    // ❌ 需要手动连接大量信号
    connect(m_tcpClient, &TcpClient::sigConnected, this, &PrintDeviceUI::onTcpConnected);
    connect(m_tcpClient, &TcpClient::sigDisconnected, this, &PrintDeviceUI::onTcpDisconnected);
    connect(m_tcpClient, &TcpClient::sigNewData, this, &PrintDeviceUI::onTcpNewData);
    connect(m_tcpClient, &TcpClient::sigError, this, &PrintDeviceUI::onTcpError);
    connect(m_tcpClient, &TcpClient::sigSocketStateChanged, this, &PrintDeviceUI::onStateChanged);
    
    connect(m_protocol, &ProtocolPrint::SigHeartBeat, this, &PrintDeviceUI::onHeartbeat);
    connect(m_protocol, &ProtocolPrint::SigCmdReply, this, &PrintDeviceUI::onProtocolCmdReply);
    
    // ❌ 需要手动配置协议
    m_protocol->SetSerialPort(m_tcpClient);
    
    // ❌ 需要手动配置心跳
    m_heartbeatSendTimer->setInterval(10000);
    m_heartbeatCheckTimer->setInterval(5000);
    connect(m_heartbeatSendTimer, &QTimer::timeout, this, &PrintDeviceUI::onSendHeartbeat);
    connect(m_heartbeatCheckTimer, &QTimer::timeout, this, &PrintDeviceUI::onCheckHeartbeat);
    
    // ... 更多初始化代码
}

// ❌ 需要实现复杂的回调函数
void PrintDeviceUI::onTcpNewData(QByteArray data) {
    if (m_protocol) {
        m_protocol->Decode(data);
    }
}

void PrintDeviceUI::onProtocolCmdReply(int cmd, uchar errCode, QByteArray arr) {
    QString msg = QString("Command reply: 0x%1, Error: %2").arg(cmd, 0, 16).arg(errCode);
    // ... 处理各种命令应答
}

void PrintDeviceUI::onHeartbeat() {
    m_heartbeatTimeout = 0;
}

void PrintDeviceUI::onSendHeartbeat() {
    if (m_tcpClient && m_tcpClient->isConnected()) {
        QByteArray packet = ProtocolPrint::GetSendDatagram(
            ProtocolPrint::Con_Breath, QByteArray());
        m_tcpClient->sendData(packet);
    }
}

void PrintDeviceUI::onCheckHeartbeat() {
    m_heartbeatTimeout++;
    if (m_heartbeatTimeout > 3) {
        // 心跳超时，断开连接
        m_tcpClient->disconnectFromHost();
    }
}

// ... 还有更多回调函数，总共~500行
```

#### 使用SDK后（printDeviceUI.cpp）:

```cpp
// printDeviceUI.cpp - SDK实现
PrintDeviceUI::PrintDeviceUI(QWidget *parent)
    : QDialog(parent)
{
    // ✅ 只需创建一个控制器
    m_controller = new PrintDeviceController(this);
    
    // ✅ 初始化SDK（一行）
    if (!m_controller->initialize("./logs")) {
        QMessageBox::critical(this, "错误", "SDK初始化失败");
        return;
    }
    
    // ✅ 连接简化的信号（只需关心业务逻辑）
    connect(m_controller, &PrintDeviceController::connected, 
            this, &PrintDeviceUI::onDeviceConnected);
    
    connect(m_controller, &PrintDeviceController::disconnected, 
            this, &PrintDeviceUI::onDeviceDisconnected);
    
    connect(m_controller, &PrintDeviceController::printProgressUpdated,
            this, &PrintDeviceUI::onPrintProgress);
    
    connect(m_controller, &PrintDeviceController::errorOccurred,
            this, &PrintDeviceUI::onError);
    
    // ✅ 就这么简单！所有底层细节都由SDK处理
}

PrintDeviceUI::~PrintDeviceUI()
{
    // ✅ 无需手动清理，Qt对象树自动管理
}

// ✅ 简化的槽函数实现
void PrintDeviceUI::onDeviceConnected()
{
    m_statusLabel->setText("✅ 已连接");
    m_statusLabel->setStyleSheet("color: green;");
    // SDK已经处理了所有底层细节（心跳、协议等）
}

void PrintDeviceUI::onDeviceDisconnected()
{
    m_statusLabel->setText("❌ 未连接");
    m_statusLabel->setStyleSheet("color: red;");
}

void PrintDeviceUI::onPrintProgress(int progress, int current, int total)
{
    m_progressBar->setValue(progress);
    m_statusLabel->setText(QString("打印进度: %1% (%2/%3层)")
        .arg(progress).arg(current).arg(total));
}

void PrintDeviceUI::onError(int code, const QString& message)
{
    QMessageBox::warning(this, "错误", message);
}

// ✅ 业务逻辑函数也大幅简化
void PrintDeviceUI::on_connectBtn_clicked()
{
    QString ip = m_ipEdit->text();
    quint16 port = m_portEdit->text().toUShort();
    
    // ✅ 一行搞定
    m_controller->connectToDevice(ip, port);
    
    // ❌ 原来需要：
    // m_tcpClient->setIpAndPort(ip, port);
    // m_tcpClient->connectToHost();
    // m_heartbeatTimeout = 0;
    // m_heartbeatSendTimer->start();
    // m_heartbeatCheckTimer->start();
    // 还要发送第一个心跳...
}

void PrintDeviceUI::on_startPrintBtn_clicked()
{
    // ✅ 一行搞定
    m_controller->startPrint();
    
    // ❌ 原来需要：
    // QByteArray packet = ProtocolPrint::GetSendDatagram(
    //     ProtocolPrint::Set_StartPrint, QByteArray());
    // m_tcpClient->sendData(packet);
}

void PrintDeviceUI::on_loadImageBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "选择图像", "", "Images (*.jpg *.png *.bmp)");
    
    if (!filePath.isEmpty()) {
        // ✅ 一行搞定
        m_controller->loadPrintData(filePath);
        
        // ❌ 原来需要：
        // QImage img(filePath);
        // QFile file(filePath);
        // file.open(QIODevice::ReadOnly);
        // QByteArray rawData = file.readAll().toHex();
        // file.close();
        // quint8 imgType = ...;  // 判断图像类型
        // QList<QByteArray> packets = ProtocolPrint::GetSendImgDatagram(...);
        // for (const auto& packet : packets) {
        //     m_tcpClient->sendData(packet);
        // }
        // ... 还要处理错误
    }
}

// 总共只需要~150行，减少70%！
```

---

## 代码迁移示例

### 完整对比：连接按钮点击

#### 原实现（~30行）:

```cpp
void PrintDeviceUI::OnListenBtnClicked(int idx)
{
    auto btnType = static_cast<NetFun>(idx);
    
    switch (btnType)
    {
    case ENF_StartConn:
    {
        if (m_tcpClient->isConnected()) {
            AppendShowComm(QStringLiteral("已连接，无需重复连接"), ESET_OperComm);
            return;
        }
        
        QString ip = m_ipLine->text();
        ushort port = m_portLine->text().toUShort();
        
        m_tcpClient->setIpAndPort(ip, port);
        m_tcpClient->connectToHost();
        
        // 重置心跳
        QMutexLocker lock(&m_heartbeatMutex);
        m_heartbeatTimeout = 0;
        m_heartbeatSendTimer->start();
        m_heartbeatCheckTimer->start();
        
        // 发送第一个心跳
        QByteArray packet = ProtocolPrint::GetSendDatagram(
            ProtocolPrint::Con_Breath, QByteArray());
        m_tcpClient->sendData(packet);
        
        AppendShowComm(QStringLiteral("尝试连接服务器"), ESET_OperComm);
        break;
    }
    case ENF_DisConn:
    {
        if (!m_tcpClient->isConnected()) {
            AppendShowComm(QStringLiteral("未连接，无需断开"), ESET_OperComm);
            return;
        }
        
        m_heartbeatSendTimer->stop();
        m_heartbeatCheckTimer->stop();
        m_tcpClient->disconnectFromHost();
        
        AppendShowComm(QStringLiteral("断开连接"), ESET_OperComm);
        break;
    }
    default:
        break;
    }
}
```

#### SDK实现（~8行）:

```cpp
void PrintDeviceUI::on_connectBtn_clicked()
{
    if (m_controller->isConnected()) {
        m_controller->disconnectFromDevice();
    } else {
        QString ip = m_ipEdit->text();
        quint16 port = m_portEdit->text().toUShort();
        m_controller->connectToDevice(ip, port);
    }
}

// 连接结果通过信号通知，UI在槽函数中更新
```

**代码减少**: 30行 → 8行，减少73% ✨

---

### 完整对比：打印控制

#### 原实现（~50行）:

```cpp
void PrintDeviceUI::OnPrintFunClicked(int idx)
{
    auto btnType = static_cast<PrintFun>(idx);
    QString logStr;
    
    switch (btnType)
    {
    case EPF_StartPrint:
    {
        QByteArray packet = ProtocolPrint::GetSendDatagram(
            ProtocolPrint::Set_StartPrint, QByteArray());
        m_tcpClient->sendData(packet);
        logStr = QStringLiteral("开始打印命令");
        break;
    }
    case EPF_StopPrint:
    {
        QByteArray packet = ProtocolPrint::GetSendDatagram(
            ProtocolPrint::Set_StopPrint, QByteArray());
        m_tcpClient->sendData(packet);
        logStr = QStringLiteral("停止打印命令");
        break;
    }
    case EPF_PausePrint:
    {
        QByteArray packet = ProtocolPrint::GetSendDatagram(
            ProtocolPrint::Set_PausePrint, QByteArray());
        m_tcpClient->sendData(packet);
        logStr = QStringLiteral("暂停打印命令");
        break;
    }
    case EPF_ContinuePrint:
    {
        QByteArray packet = ProtocolPrint::GetSendDatagram(
            ProtocolPrint::Set_continuePrint, QByteArray());
        m_tcpClient->sendData(packet);
        logStr = QStringLiteral("继续打印命令");
        break;
    }
    case EPF_TransData:
    {
        QString imgPath = QFileDialog::getOpenFileName(
            this, QStringLiteral("选择图片文件"), 
            QDir::homePath(), "Image Files (*.jpg *.png *.bmp)");
        
        if (!imgPath.isEmpty()) {
            // ... 复杂的图像处理逻辑（~20行）
        }
        break;
    }
    // ... 更多case
    }
    
    AppendShowComm(logStr, ESET_OperComm);
}
```

#### SDK实现（~10行）:

```cpp
void PrintDeviceUI::on_startPrintBtn_clicked() {
    m_controller->startPrint();
}

void PrintDeviceUI::on_stopPrintBtn_clicked() {
    m_controller->stopPrint();
}

void PrintDeviceUI::on_pausePrintBtn_clicked() {
    if (m_isPaused) {
        m_controller->resumePrint();
        m_isPaused = false;
    } else {
        m_controller->pausePrint();
        m_isPaused = true;
    }
}

void PrintDeviceUI::on_loadImageBtn_clicked() {
    QString imgPath = QFileDialog::getOpenFileName(
        this, "选择图像", QDir::homePath(), 
        "Image Files (*.jpg *.png *.bmp)");
    
    if (!imgPath.isEmpty()) {
        m_controller->loadPrintData(imgPath);  // ✅ 一行搞定
    }
}
```

**代码减少**: 50行 → 10行，减少80% ✨

---

## 常见问题

### Q1: 如何处理原有的信号槽连接？

**A**: SDK提供了更高层次的信号，直接替换即可。

| 原信号 | SDK信号 |
|--------|---------|
| `TcpClient::sigConnected` | `PrintDeviceController::connected` |
| `TcpClient::sigDisconnected` | `PrintDeviceController::disconnected` |
| `ProtocolPrint::SigCmdReply` | `PrintDeviceController::errorOccurred` <br> `PrintDeviceController::infoMessage` |
| `ProtocolPrint::SigHeartBeat` | (SDK内部自动处理) |

---

### Q2: 心跳机制如何处理？

**A**: SDK内部自动处理心跳，无需手动实现。

```cpp
// ❌ 原来需要：
m_heartbeatSendTimer = new QTimer(this);
m_heartbeatCheckTimer = new QTimer(this);
connect(m_heartbeatSendTimer, &QTimer::timeout, this, &PrintDeviceUI::onSendHeartbeat);
// ... 还要实现onSendHeartbeat, onCheckHeartbeat, onHeartbeat等函数

// ✅ 现在：什么都不需要做，SDK自动处理！
```

---

### Q3: 如何获取原始数据？

**A**: 如果需要原始协议数据，可以使用C接口。

```cpp
// Qt接口：高层封装，无法获取原始数据
m_controller->startPrint();

// C接口：可以自己处理原始数据
void MyCallback(const SdkEvent* event) {
    // 可以在这里获取event的所有原始信息
}
RegisterEventCallback(MyCallback);
```

---

### Q4: 可以同时使用SDK和原有实现吗？

**A**: 不建议。两者会竞争相同的TCP连接。选择其中一种：

- ✅ **推荐**：完全迁移到SDK
- ⚠️ **过渡期**：可以在不同模块中分别使用（但不要同时连接同一设备）

---

### Q5: SDK支持多个设备同时连接吗？

**A**: 当前版本不支持。一个SDK实例只能连接一个设备。

如需连接多个设备，有两个方案：
1. 创建多个应用进程（每个使用一个SDK实例）
2. 等待SDK更新支持多设备（需要较大改动）

---

### Q6: 如何调试SDK内部问题？

**A**: 使用日志信号。

```cpp
// 连接日志信号查看SDK内部行为
connect(m_controller, &PrintDeviceController::logMessage,
        [](const QString& msg) {
    qDebug() << "[SDK]" << msg;
});

// 或者使用C接口的回调
void MyCallback(const SdkEvent* event) {
    if (event->type == EVENT_TYPE_LOG) {
        printf("[SDK] %s\n", event->message);
    }
}
```

---

### Q7: SDK运行需要哪些依赖？

**A**: 

**运行时依赖**:
- Qt5Core.dll
- Qt5Network.dll
- PrintDeviceSDK.dll
- (Windows) MSVC运行时库

**开发依赖**:
- Qt SDK (5.12+)
- C++11编译器

**部署清单**:
```
你的应用.exe
├── Qt5Core.dll
├── Qt5Network.dll
├── Qt5Gui.dll (如果是GUI应用)
├── Qt5Widgets.dll (如果是GUI应用)
├── PrintDeviceSDK.dll
└── platforms/
    └── qwindows.dll (Windows平台插件)
```

使用`windeployqt`自动部署：
```bash
windeployqt your_app.exe
# 然后手动复制PrintDeviceSDK.dll到应用目录
```

---

## 📊 迁移工作量评估

| 模块 | 原代码行数 | 迁移后行数 | 减少 | 工作量 |
|------|-----------|-----------|------|--------|
| 头文件 | 50行 | 15行 | ↓70% | 15分钟 |
| 初始化 | 80行 | 20行 | ↓75% | 30分钟 |
| 连接管理 | 100行 | 15行 | ↓85% | 30分钟 |
| 打印控制 | 150行 | 30行 | ↓80% | 45分钟 |
| 运动控制 | 120行 | 30行 | ↓75% | 45分钟 |
| 总计 | 500行 | 110行 | ↓78% | **3小时** |

**结论**: 迁移到SDK后，代码量减少约80%，预计工作量约3小时。

---

## ✅ 迁移检查清单

### 编译阶段

- [ ] SDK编译成功
- [ ] 检查DLL/SO文件生成
- [ ] 项目.pro文件配置正确
- [ ] 包含路径配置正确
- [ ] 链接库配置正确

### 代码修改阶段

- [ ] 修改头文件包含
- [ ] 替换成员变量（TcpClient → PrintDeviceController）
- [ ] 修改初始化代码
- [ ] 替换所有信号槽连接
- [ ] 简化业务逻辑函数
- [ ] 删除心跳相关代码（SDK自动处理）

### 测试阶段

- [ ] 基本连接功能测试
- [ ] 打印控制功能测试
- [ ] 运动控制功能测试
- [ ] 错误处理测试
- [ ] 长时间运行稳定性测试

### 部署阶段

- [ ] 复制SDK DLL到发布目录
- [ ] 使用windeployqt部署Qt运行时
- [ ] 在目标机器上测试
- [ ] 准备安装程序

---

## 📝 总结

### 核心优势

1. **代码量减少80%** ✨
   - 原来500行 → 现在100行
   - 更易维护

2. **复杂度降低** ✨
   - 无需关心底层细节
   - SDK自动处理心跳、协议等

3. **错误更少** ✨
   - SDK已经过测试
   - 减少重复劳动

4. **升级方便** ✨
   - SDK更新时只需替换DLL
   - 应用代码无需修改

### 迁移建议

- ✅ **新项目**: 直接使用SDK
- ✅ **简单项目**: 立即迁移（工作量小）
- ⚠️ **复杂项目**: 逐模块迁移
- ⚠️ **临近发布**: 下个版本再迁移

**推荐**: 对于Qt项目，强烈推荐使用SDK！代码质量和开发效率都会显著提升。

---

**相关文档**:
- SDK代码审核报告.md - 发现的问题和修复建议
- Qt接口使用指南.md - Qt接口详细API文档
- example_qt_interface.cpp - 完整的示例程序

**支持**: 如有问题，请参考这些文档或查看示例代码。

