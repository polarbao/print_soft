# PrintDeviceSDK 集成指南

本文档说明如何将PrintDeviceSDK集成到不同类型的应用程序中。

## 目录

1. [快速开始](#快速开始)
2. [Qt应用程序集成](#qt应用程序集成)
3. [MFC应用程序集成](#mfc应用程序集成)
4. [C#应用程序集成](#c应用程序集成)
5. [Python应用程序集成](#python应用程序集成)
6. [API详细说明](#api详细说明)

## 快速开始

### 最小化示例 (C++)

```cpp
#include "PrintDeviceSDK_API.h"
#include <QCoreApplication>
#include <iostream>

void MyCallback(const SdkEvent* event) {
    std::cout << event->message << std::endl;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);  // SDK需要Qt事件循环
    
    InitSDK("./logs");
    RegisterEventCallback(MyCallback);
    ConnectByTCP("192.168.100.57", 5555);
    
    // ... 执行操作 ...
    
    ReleaseSDK();
    return app.exec();
}
```

## Qt应用程序集成

### 1. 添加SDK到项目

在你的`.pro`文件中：

```qmake
# 包含SDK头文件
INCLUDEPATH += path/to/PrintDeviceSDK

# 链接SDK库
win32 {
    CONFIG(release, debug|release) {
        LIBS += -Lpath/to/PrintDeviceSDK/bin/release -lPrintDeviceSDK
    }
    CONFIG(debug, debug|release) {
        LIBS += -Lpath/to/PrintDeviceSDK/bin/debug -lPrintDeviceSDK
    }
}
```

### 2. 在Qt Widget中使用

```cpp
// MainWindow.h
#include "PrintDeviceSDK_API.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void onConnectClicked();
    void onStartPrintClicked();
    
private:
    static void sdkCallback(const SdkEvent* event);
    static MainWindow* s_instance;  // 用于在静态回调中访问实例
};

// MainWindow.cpp
#include "MainWindow.h"

MainWindow* MainWindow::s_instance = nullptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    s_instance = this;
    
    // 初始化SDK
    InitSDK("./logs");
    RegisterEventCallback(&MainWindow::sdkCallback);
}

MainWindow::~MainWindow() {
    ReleaseSDK();
}

void MainWindow::sdkCallback(const SdkEvent* event) {
    // 在回调中更新UI（需要使用信号槽或QMetaObject::invokeMethod）
    if (s_instance) {
        QMetaObject::invokeMethod(s_instance, [=]() {
            // 更新UI
            QString msg = QString(event->message);
            // 显示到UI控件...
        }, Qt::QueuedConnection);
    }
}

void MainWindow::onConnectClicked() {
    QString ip = ui->ipLineEdit->text();
    int port = ui->portSpinBox->value();
    ConnectByTCP(ip.toUtf8().constData(), port);
}

void MainWindow::onStartPrintClicked() {
    if (IsConnected()) {
        StartPrint();
    }
}
```

## MFC应用程序集成

### 1. 项目配置

在Visual Studio中：

1. **添加包含目录**：
   - 项目属性 -> C/C++ -> 常规 -> 附加包含目录
   - 添加: `D:\path\to\PrintDeviceSDK`
   - 添加: `C:\Qt\5.12.0\msvc2017_64\include`

2. **添加库目录**：
   - 项目属性 -> 链接器 -> 常规 -> 附加库目录
   - 添加: `D:\path\to\PrintDeviceSDK\bin\release`
   - 添加: `C:\Qt\5.12.0\msvc2017_64\lib`

3. **添加依赖库**：
   - 项目属性 -> 链接器 -> 输入 -> 附加依赖项
   - 添加: `PrintDeviceSDK.lib;Qt5Core.lib;Qt5Network.lib`

### 2. 初始化Qt环境

在MFC应用程序中使用SDK需要初始化Qt环境：

```cpp
// stdafx.h 或 pch.h
#include <QCoreApplication>

// App类
class CMyApp : public CWinApp {
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    
private:
    QCoreApplication* m_qtApp;
    QThread* m_qtThread;
};

// 实现
BOOL CMyApp::InitInstance() {
    CWinApp::InitInstance();
    
    // 初始化Qt
    int argc = 0;
    char** argv = nullptr;
    m_qtApp = new QCoreApplication(argc, argv);
    
    // 创建Qt事件循环线程
    m_qtThread = QThread::create([]() {
        QCoreApplication::exec();
    });
    m_qtThread->start();
    
    // 初始化SDK
    InitSDK("./logs");
    RegisterEventCallback(&MyCallback);
    
    return TRUE;
}

int CMyApp::ExitInstance() {
    ReleaseSDK();
    
    // 清理Qt
    QCoreApplication::quit();
    m_qtThread->wait();
    delete m_qtThread;
    delete m_qtApp;
    
    return CWinApp::ExitInstance();
}

void MyCallback(const SdkEvent* event) {
    // 处理事件
    CString msg(event->message);
    // 更新MFC UI...
}
```

### 3. 在对话框中使用

```cpp
// MyDialog.h
class CMyDialog : public CDialogEx {
public:
    CMyDialog(CWnd* pParent = nullptr);
    
protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedConnect();
    afx_msg void OnBnClickedStartPrint();
    DECLARE_MESSAGE_MAP()
};

// MyDialog.cpp
#include "PrintDeviceSDK_API.h"

BOOL CMyDialog::OnInitDialog() {
    CDialogEx::OnInitDialog();
    // 对话框初始化...
    return TRUE;
}

void CMyDialog::OnBnClickedConnect() {
    CString ip;
    m_ipEdit.GetWindowText(ip);
    
    ConnectByTCP(CStringA(ip), 5555);
}

void CMyDialog::OnBnClickedStartPrint() {
    if (IsConnected()) {
        StartPrint();
    }
}
```

## C#应用程序集成

### 1. P/Invoke声明

创建一个C#包装类：

```csharp
using System;
using System.Runtime.InteropServices;

namespace PrintDeviceSDK
{
    // 事件类型枚举
    public enum SdkEventType
    {
        EVENT_TYPE_GENERAL = 0,
        EVENT_TYPE_ERROR = 1,
        EVENT_TYPE_PRINT_STATUS = 2,
        EVENT_TYPE_MOVE_STATUS = 3,
        EVENT_TYPE_LOG = 4
    }

    // 事件结构体
    [StructLayout(LayoutKind.Sequential)]
    public struct SdkEvent
    {
        public SdkEventType type;
        public int code;
        public IntPtr message;  // const char*
        public double value1;
        public double value2;
        public double value3;
    }

    // 回调委托
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void SdkEventCallback([In] ref SdkEvent evt);

    public class PrintDevice
    {
        const string DLL_NAME = "PrintDeviceSDK.dll";

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern int InitSDK([MarshalAs(UnmanagedType.LPStr)] string log_dir);

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ReleaseSDK();

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RegisterEventCallback(SdkEventCallback callback);

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ConnectByTCP(
            [MarshalAs(UnmanagedType.LPStr)] string ip, 
            ushort port);

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Disconnect();

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern int IsConnected();

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern int StartPrint();

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern int StopPrint();

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern int MoveTo(double x, double y, double z, double speed);

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern int GoHome();
    }
}
```

### 2. 使用示例

```csharp
using System;
using System.Windows.Forms;
using PrintDeviceSDK;

public class MainForm : Form
{
    private SdkEventCallback callback;

    public MainForm()
    {
        // 保持回调引用以防止被GC回收
        callback = OnSdkEvent;
        
        PrintDevice.InitSDK("./logs");
        PrintDevice.RegisterEventCallback(callback);
    }

    protected override void Dispose(bool disposing)
    {
        PrintDevice.ReleaseSDK();
        base.Dispose(disposing);
    }

    private void OnSdkEvent(ref SdkEvent evt)
    {
        string message = Marshal.PtrToStringAnsi(evt.message);
        
        // 更新UI（需要Invoke到UI线程）
        this.Invoke(new Action(() => {
            textBox.AppendText($"[{evt.type}] {message}\r\n");
        }));
    }

    private void btnConnect_Click(object sender, EventArgs e)
    {
        PrintDevice.ConnectByTCP("192.168.100.57", 5555);
    }

    private void btnStartPrint_Click(object sender, EventArgs e)
    {
        if (PrintDevice.IsConnected() != 0)
        {
            PrintDevice.StartPrint();
        }
    }
}
```

## Python应用程序集成

### 1. 使用ctypes

```python
import ctypes
from ctypes import *
from enum import IntEnum

# 加载DLL
sdk = ctypes.CDLL("PrintDeviceSDK.dll")

# 定义枚举
class SdkEventType(IntEnum):
    EVENT_TYPE_GENERAL = 0
    EVENT_TYPE_ERROR = 1
    EVENT_TYPE_PRINT_STATUS = 2
    EVENT_TYPE_MOVE_STATUS = 3
    EVENT_TYPE_LOG = 4

# 定义结构体
class SdkEvent(Structure):
    _fields_ = [
        ("type", c_int),
        ("code", c_int),
        ("message", c_char_p),
        ("value1", c_double),
        ("value2", c_double),
        ("value3", c_double)
    ]

# 定义回调类型
SdkEventCallback = CFUNCTYPE(None, POINTER(SdkEvent))

# 声明函数
sdk.InitSDK.argtypes = [c_char_p]
sdk.InitSDK.restype = c_int

sdk.RegisterEventCallback.argtypes = [SdkEventCallback]
sdk.RegisterEventCallback.restype = None

sdk.ConnectByTCP.argtypes = [c_char_p, c_ushort]
sdk.ConnectByTCP.restype = c_int

sdk.IsConnected.restype = c_int
sdk.StartPrint.restype = c_int
sdk.StopPrint.restype = c_int

# 回调函数
def my_callback(event):
    evt = event.contents
    print(f"[{SdkEventType(evt.type).name}] {evt.message.decode('utf-8')}")

# 使用示例
callback = SdkEventCallback(my_callback)  # 保持引用

sdk.InitSDK(b"./logs")
sdk.RegisterEventCallback(callback)
sdk.ConnectByTCP(b"192.168.100.57", 5555)

# ... 执行操作 ...

sdk.ReleaseSDK()
```

## API详细说明

### 初始化和清理

```c
int InitSDK(const char* log_dir);
```
- 初始化SDK，必须最先调用
- 参数：日志目录路径（可为NULL）
- 返回：0=成功，其他=失败

```c
void ReleaseSDK();
```
- 释放SDK资源，程序退出时调用

### 事件回调

```c
void RegisterEventCallback(SdkEventCallback callback);
```
- 注册事件回调函数
- SDK的所有事件都通过该回调通知

### 连接管理

```c
int ConnectByTCP(const char* ip, unsigned short port);
```
- 通过TCP连接设备
- 参数：设备IP和端口
- 返回：0=成功

```c
void Disconnect();
```
- 断开设备连接

```c
int IsConnected();
```
- 查询连接状态
- 返回：1=已连接，0=未连接

### 运动控制

```c
int MoveTo(double x, double y, double z, double speed);
```
- 移动到绝对坐标

```c
int MoveBy(double dx, double dy, double dz, double speed);
```
- 相对移动

```c
int GoHome();
```
- 所有轴回原点

### 打印控制

```c
int StartPrint();    // 开始打印
int StopPrint();     // 停止打印
int PausePrint();    // 暂停打印
int ResumePrint();   // 恢复打印
```

```c
int LoadPrintData(const char* data);
```
- 加载打印数据（图像文件路径）

## 注意事项

### 1. Qt事件循环

SDK内部使用Qt，需要Qt事件循环：

- **Qt应用**: 自动处理
- **非Qt应用**: 需要创建QCoreApplication并在独立线程运行事件循环

### 2. 线程安全

- 所有API函数都是线程安全的
- 回调函数可能在不同线程中调用，更新UI时需要注意线程切换

### 3. 内存管理

- SDK管理所有内部资源
- 回调中的`event->message`指针仅在回调期间有效，如需保存需要复制

### 4. 部署

确保以下DLL在可执行文件同目录或系统PATH中：
- PrintDeviceSDK.dll
- Qt5Core.dll
- Qt5Network.dll
- 其他Qt依赖（使用windeployqt收集）

## 故障排除

### 问题：找不到DLL

**解决**：
1. 确保DLL在exe同目录
2. 检查系统PATH环境变量
3. 使用Dependency Walker查看依赖

### 问题：回调不触发

**解决**：
1. 确认已调用`RegisterEventCallback`
2. 确认Qt事件循环正在运行
3. 检查是否已连接设备

### 问题：程序崩溃

**解决**：
1. 检查Qt版本和编译器是否匹配
2. 确保Release/Debug配置一致
3. 检查回调函数中的内存访问

