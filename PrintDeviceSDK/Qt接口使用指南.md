# PrintDeviceSDK Qt接口使用指南

## 📖 目录

1. [概述](#概述)
2. [快速开始](#快速开始)
3. [详细API说明](#详细api说明)
4. [完整示例](#完整示例)
5. [信号说明](#信号说明)
6. [最佳实践](#最佳实践)
7. [常见问题](#常见问题)

---

## 概述

### 什么是Qt接口？

`PrintDeviceController` 是PrintDeviceSDK的Qt风格接口，专为Qt项目设计。

**主要特点**：
- ✅ **信号槽机制** - 使用Qt信号槽，自动线程安全
- ✅ **Qt类型** - QString、QByteArray等标准Qt类型
- ✅ **Qt属性** - 支持Qt属性系统，可用于QML
- ✅ **自动内存管理** - 利用Qt对象树自动管理内存
- ✅ **完美集成** - 无缝集成到Qt应用程序

### 与C接口对比

| 特性 | C接口 | Qt接口 |
|------|-------|--------|
| 线程安全 | 手动处理 | ✅ 自动 |
| 事件通知 | 回调函数 | ✅ 信号槽 |
| 类型系统 | char*, int | ✅ QString, QByteArray |
| 内存管理 | 手动 | ✅ 自动 |
| Qt集成 | 需包装 | ✅ 原生 |
| 跨语言 | ✅ 支持 | ❌ 仅C++ |

**推荐**：Qt项目优先使用Qt接口！

---

## 快速开始

### 步骤1：包含头文件

```cpp
#include "PrintDeviceController.h"
```

### 步骤2：创建控制器对象

```cpp
PrintDeviceController* controller = new PrintDeviceController(this);
```

### 步骤3：初始化SDK

```cpp
if (!controller->initialize("./logs")) {
    qCritical() << "初始化失败";
    return;
}
```

### 步骤4：连接信号

```cpp
// 连接成功
connect(controller, &PrintDeviceController::connected, this, []() {
    qDebug() << "设备已连接！";
});

// 连接断开
connect(controller, &PrintDeviceController::disconnected, this, []() {
    qDebug() << "设备已断开";
});

// 错误处理
connect(controller, &PrintDeviceController::errorOccurred, 
        this, [](int code, const QString& msg) {
    qWarning() << "错误:" << code << msg;
});
```

### 步骤5：连接设备

```cpp
controller->connectToDevice("192.168.100.57", 5555);
```

### 步骤6：使用功能

```cpp
// 打印控制
controller->startPrint();
controller->pausePrint();
controller->resumePrint();
controller->stopPrint();

// 运动控制
controller->goHome();
controller->moveTo(100, 50, 20);
controller->moveZAxis(10);  // Z轴上移10mm
```

---

## 详细API说明

### 生命周期管理

#### `initialize(const QString& logDir)`

初始化SDK（必须首先调用）

```cpp
bool initialize(const QString& logDir = QString());
```

**参数**：
- `logDir` - 日志目录路径（可选，为空则不记录日志）

**返回值**：
- `true` - 初始化成功
- `false` - 初始化失败（通过errorOccurred信号通知）

**示例**：

```cpp
// 不记录日志
controller->initialize();

// 记录日志到指定目录
controller->initialize("./logs");
controller->initialize("C:/MyApp/logs");
```

#### `release()`

释放SDK资源

```cpp
void release();
```

**注意**：
- 析构时会自动调用，通常无需手动调用
- 会自动断开设备连接

#### `isInitialized()`

检查SDK是否已初始化

```cpp
bool isInitialized() const;
```

---

### 连接管理

#### `connectToDevice(const QString& ip, quint16 port)`

连接设备

```cpp
bool connectToDevice(const QString& ip, quint16 port = 5555);
```

**参数**：
- `ip` - 设备IP地址
- `port` - 端口号（默认5555）

**返回值**：
- `true` - 连接命令发送成功
- `false` - 发送失败

**信号**：
- 连接成功时触发 `connected()`
- 连接失败时触发 `errorOccurred()`

**示例**：

```cpp
// 使用默认端口
controller->connectToDevice("192.168.100.57");

// 指定端口
controller->connectToDevice("192.168.100.57", 8888);

// 带信号处理
connect(controller, &PrintDeviceController::connected, []() {
    qDebug() << "连接成功，可以开始操作";
});
controller->connectToDevice("192.168.100.57");
```

#### `disconnectFromDevice()`

断开连接

```cpp
void disconnectFromDevice();
```

**信号**：
- 断开成功时触发 `disconnected()`

#### `isConnected()`

查询连接状态

```cpp
bool isConnected() const;
```

**返回值**：
- `true` - 已连接
- `false` - 未连接

**示例**：

```cpp
if (controller->isConnected()) {
    controller->startPrint();
} else {
    QMessageBox::warning(this, "错误", "设备未连接");
}
```

#### `deviceIp()` / `devicePort()`

获取设备连接信息

```cpp
QString deviceIp() const;
quint16 devicePort() const;
```

---

### 运动控制

#### `moveTo(x, y, z, speed)`

移动到绝对坐标

```cpp
bool moveTo(double x, double y, double z, double speed = 100.0);
```

**参数**：
- `x` - X轴坐标（mm）
- `y` - Y轴坐标（mm）
- `z` - Z轴坐标（mm）
- `speed` - 速度（mm/s，默认100）

**示例**：

```cpp
// 移动到(100, 50, 20)位置
controller->moveTo(100, 50, 20);

// 指定速度
controller->moveTo(100, 50, 20, 50.0);  // 以50mm/s速度移动
```

#### `moveBy(dx, dy, dz, speed)`

相对移动

```cpp
bool moveBy(double dx, double dy, double dz, double speed = 100.0);
```

**参数**：
- `dx` - X轴增量（mm）
- `dy` - Y轴增量（mm）
- `dz` - Z轴增量（mm）
- `speed` - 速度（mm/s）

**示例**：

```cpp
// 向右移动10mm
controller->moveBy(10, 0, 0);

// Z轴向上移动5mm
controller->moveBy(0, 0, 5);
```

#### `goHome()`

回原点（所有轴复位）

```cpp
bool goHome();
```

**示例**：

```cpp
controller->goHome();

// 带信号处理
connect(controller, &PrintDeviceController::moveStatusChanged, 
        [](const QString& status) {
    qDebug() << "运动状态:" << status;
});
controller->goHome();
```

#### 单轴移动

```cpp
bool moveXAxis(double distance, double speed = 100.0);
bool moveYAxis(double distance, double speed = 100.0);
bool moveZAxis(double distance, double speed = 100.0);
```

**参数**：
- `distance` - 移动距离（mm）
  - 正数：X/Y向前，Z向上
  - 负数：X/Y向后，Z向下
- `speed` - 速度（mm/s）

**示例**：

```cpp
// X轴向前移动20mm
controller->moveXAxis(20);

// Y轴向后移动10mm
controller->moveYAxis(-10);

// Z轴向上移动5mm
controller->moveZAxis(5);

// Z轴向下移动5mm
controller->moveZAxis(-5);
```

---

### 打印控制

#### `loadPrintData(filePath)`

加载打印数据

```cpp
bool loadPrintData(const QString& filePath);
```

**参数**：
- `filePath` - 图像文件路径（支持JPG/PNG/BMP）

**示例**：

```cpp
// 加载图像文件
controller->loadPrintData("C:/images/print.jpg");

// 使用文件对话框
QString file = QFileDialog::getOpenFileName(
    this, "选择图像", "", "Images (*.jpg *.png *.bmp)");
if (!file.isEmpty()) {
    controller->loadPrintData(file);
}
```

#### `startPrint()`

开始打印

```cpp
bool startPrint();
```

**信号**：
- `printStatusChanged()` - 打印状态改变
- `printProgressUpdated()` - 打印进度更新

**示例**：

```cpp
// 连接进度信号
connect(controller, &PrintDeviceController::printProgressUpdated,
        [](int progress, int current, int total) {
    qDebug() << QString("进度: %1% (%2/%3层)")
                .arg(progress).arg(current).arg(total);
});

// 开始打印
controller->startPrint();
```

#### `pausePrint()` / `resumePrint()` / `stopPrint()`

打印流程控制

```cpp
bool pausePrint();   // 暂停打印
bool resumePrint();  // 恢复打印
bool stopPrint();    // 停止打印
```

**示例**：

```cpp
// 暂停
pauseBtn->clicked.connect([=]() {
    controller->pausePrint();
});

// 恢复
resumeBtn->clicked.connect([=]() {
    controller->resumePrint();
});

// 停止
stopBtn->clicked.connect([=]() {
    controller->stopPrint();
});
```

---

## 完整示例

### 示例1：最小示例

```cpp
#include <QCoreApplication>
#include "PrintDeviceController.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // 创建控制器
    PrintDeviceController controller;
    
    // 初始化
    if (!controller.initialize("./logs")) {
        qCritical() << "初始化失败";
        return -1;
    }
    
    // 连接信号
    QObject::connect(&controller, &PrintDeviceController::connected, []() {
        qDebug() << "✅ 设备已连接";
    });
    
    // 连接设备
    controller.connectToDevice("192.168.100.57", 5555);
    
    return app.exec();
}
```

### 示例2：GUI应用

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QMessageBox>
#include "PrintDeviceController.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QWidget(parent)
    {
        setupUI();
        setupController();
    }

private:
    void setupUI()
    {
        // 创建UI控件
        statusLabel = new QLabel("未连接");
        ipEdit = new QLineEdit("192.168.100.57");
        portEdit = new QLineEdit("5555");
        
        connectBtn = new QPushButton("连接");
        startPrintBtn = new QPushButton("开始打印");
        pausePrintBtn = new QPushButton("暂停打印");
        stopPrintBtn = new QPushButton("停止打印");
        homeBtn = new QPushButton("回原点");
        
        progressBar = new QProgressBar();
        
        // 初始状态
        startPrintBtn->setEnabled(false);
        pausePrintBtn->setEnabled(false);
        stopPrintBtn->setEnabled(false);
        homeBtn->setEnabled(false);
        
        // 布局
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel("状态:"));
        layout->addWidget(statusLabel);
        layout->addWidget(new QLabel("IP:"));
        layout->addWidget(ipEdit);
        layout->addWidget(new QLabel("Port:"));
        layout->addWidget(portEdit);
        layout->addWidget(connectBtn);
        layout->addWidget(progressBar);
        layout->addWidget(startPrintBtn);
        layout->addWidget(pausePrintBtn);
        layout->addWidget(stopPrintBtn);
        layout->addWidget(homeBtn);
        
        // 连接按钮信号
        connect(connectBtn, &QPushButton::clicked, this, &MainWindow::onConnect);
        connect(startPrintBtn, &QPushButton::clicked, this, &MainWindow::onStartPrint);
        connect(pausePrintBtn, &QPushButton::clicked, this, &MainWindow::onPausePrint);
        connect(stopPrintBtn, &QPushButton::clicked, this, &MainWindow::onStopPrint);
        connect(homeBtn, &QPushButton::clicked, this, &MainWindow::onHome);
    }
    
    void setupController()
    {
        controller = new PrintDeviceController(this);
        
        // 初始化SDK
        if (!controller->initialize("./logs")) {
            QMessageBox::critical(this, "错误", "SDK初始化失败");
            return;
        }
        
        // 连接控制器信号
        connect(controller, &PrintDeviceController::connected, this, [this]() {
            statusLabel->setText("✅ 已连接");
            statusLabel->setStyleSheet("color: green;");
            connectBtn->setText("断开");
            startPrintBtn->setEnabled(true);
            homeBtn->setEnabled(true);
        });
        
        connect(controller, &PrintDeviceController::disconnected, this, [this]() {
            statusLabel->setText("❌ 未连接");
            statusLabel->setStyleSheet("color: red;");
            connectBtn->setText("连接");
            startPrintBtn->setEnabled(false);
            pausePrintBtn->setEnabled(false);
            stopPrintBtn->setEnabled(false);
            homeBtn->setEnabled(false);
        });
        
        connect(controller, &PrintDeviceController::errorOccurred, 
                this, [this](int code, const QString& msg) {
            QMessageBox::warning(this, "错误", QString("错误码 %1: %2").arg(code).arg(msg));
        });
        
        connect(controller, &PrintDeviceController::printProgressUpdated,
                this, [this](int progress, int current, int total) {
            progressBar->setValue(progress);
            statusLabel->setText(QString("打印中: %1% (%2/%3层)")
                .arg(progress).arg(current).arg(total));
        });
        
        connect(controller, &PrintDeviceController::moveStatusChanged,
                this, [this](const QString& status) {
            statusLabel->setText("运动: " + status);
        });
    }
    
private slots:
    void onConnect()
    {
        if (controller->isConnected()) {
            controller->disconnectFromDevice();
        } else {
            QString ip = ipEdit->text();
            quint16 port = portEdit->text().toUShort();
            controller->connectToDevice(ip, port);
        }
    }
    
    void onStartPrint()
    {
        // 先选择图像文件
        QString file = QFileDialog::getOpenFileName(
            this, "选择打印图像", "", "Images (*.jpg *.png *.bmp)");
        
        if (file.isEmpty()) {
            return;
        }
        
        // 加载并开始打印
        if (controller->loadPrintData(file)) {
            controller->startPrint();
            startPrintBtn->setEnabled(false);
            pausePrintBtn->setEnabled(true);
            stopPrintBtn->setEnabled(true);
        }
    }
    
    void onPausePrint()
    {
        if (pausePrintBtn->text() == "暂停打印") {
            controller->pausePrint();
            pausePrintBtn->setText("继续打印");
        } else {
            controller->resumePrint();
            pausePrintBtn->setText("暂停打印");
        }
    }
    
    void onStopPrint()
    {
        controller->stopPrint();
        startPrintBtn->setEnabled(true);
        pausePrintBtn->setEnabled(false);
        pausePrintBtn->setText("暂停打印");
        stopPrintBtn->setEnabled(false);
        progressBar->setValue(0);
    }
    
    void onHome()
    {
        controller->goHome();
    }

private:
    PrintDeviceController* controller;
    
    QLabel* statusLabel;
    QLineEdit* ipEdit;
    QLineEdit* portEdit;
    QPushButton* connectBtn;
    QPushButton* startPrintBtn;
    QPushButton* pausePrintBtn;
    QPushButton* stopPrintBtn;
    QPushButton* homeBtn;
    QProgressBar* progressBar;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    MainWindow window;
    window.setWindowTitle("PrintDevice控制面板");
    window.resize(400, 500);
    window.show();
    
    return app.exec();
}
```

---

## 信号说明

### 连接相关信号

```cpp
void connected();                          // 连接成功
void disconnected();                       // 连接断开
void connectedChanged(bool isConnected);   // 连接状态改变
void deviceIpChanged(const QString& ip);   // IP改变
void devicePortChanged(quint16 port);      // 端口改变
```

### 状态和错误信号

```cpp
void errorOccurred(int errorCode, const QString& errorMessage);  // 错误
void infoMessage(const QString& message);                         // 一般信息
void logMessage(const QString& message);                          // 日志
```

### 打印相关信号

```cpp
void printProgressUpdated(int progress, int currentLayer, int totalLayers);  // 进度更新
void printStatusChanged(const QString& status);                              // 状态改变
```

### 运动相关信号

```cpp
void moveStatusChanged(const QString& status);            // 运动状态
void positionUpdated(double x, double y, double z);      // 位置更新
```

---

## 最佳实践

### 1. 总是检查初始化状态

```cpp
if (!controller->initialize()) {
    // 处理初始化失败
    return;
}
```

### 2. 使用对象树管理内存

```cpp
// ✅ 推荐：指定parent，自动内存管理
PrintDeviceController* controller = new PrintDeviceController(this);

// ❌ 不推荐：需要手动delete
PrintDeviceController* controller = new PrintDeviceController();
```

### 3. 连接操作前检查连接状态

```cpp
if (!controller->isConnected()) {
    QMessageBox::warning(this, "提示", "请先连接设备");
    return;
}
controller->startPrint();
```

### 4. 处理所有重要信号

```cpp
// 至少处理这些信号
connect(controller, &PrintDeviceController::connected, ...);
connect(controller, &PrintDeviceController::disconnected, ...);
connect(controller, &PrintDeviceController::errorOccurred, ...);
```

### 5. 使用Lambda表达式简化代码

```cpp
// ✅ 简洁
connect(controller, &PrintDeviceController::connected, [this]() {
    statusLabel->setText("已连接");
});

// 也可以
connect(controller, &PrintDeviceController::connected, 
        this, &MainWindow::onConnected);
```

---

## 常见问题

### Q1: Qt接口和C接口可以同时使用吗？

**A**: 不建议。两个接口会竞争同一个底层实例。选择一个使用即可：
- Qt项目 → 使用Qt接口
- 跨语言项目 → 使用C接口

### Q2: 为什么信号没有触发？

**A**: 检查以下几点：
1. 是否调用了`initialize()`
2. 信号是否正确连接
3. 应用是否进入了事件循环（`app.exec()`）

### Q3: 如何在QML中使用？

**A**: 将控制器注册到QML：

```cpp
// main.cpp
PrintDeviceController controller;
controller.initialize();

QQmlApplicationEngine engine;
engine.rootContext()->setContextProperty("deviceController", &controller);

// main.qml
Button {
    text: "连接"
    onClicked: deviceController.connectToDevice("192.168.100.57", 5555)
}

Text {
    text: deviceController.connected ? "已连接" : "未连接"
}
```

### Q4: 线程安全吗？

**A**: 是的。所有信号都通过Qt的事件系统自动在正确的线程中执行。

### Q5: 如何获取更多日志信息？

**A**: 连接`logMessage`信号：

```cpp
connect(controller, &PrintDeviceController::logMessage, [](const QString& msg) {
    qDebug() << "SDK Log:" << msg;
});
```

---

## 总结

Qt接口相比C接口的优势：

| 方面 | 改进 |
|------|------|
| 代码量 | ↓ 减少60-80% |
| 线程安全 | ✅ 自动处理 |
| 类型安全 | ✅ QString vs char* |
| 易用性 | ⬆️ 信号槽 vs 回调 |
| Qt集成 | ✅ 原生支持 |

**强烈推荐Qt项目使用Qt接口！** ✨

