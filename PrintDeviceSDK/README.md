# PrintDeviceSDK v1.0 - 全功能设备控制库

## 概述

`PrintDeviceSDK` 是一个用于控制特定打印设备的、无UI的全功能动态链接库。它封装了设备的通信、运动控制、打印控制等所有后端逻辑，并提供统一的C语言API接口，可被 C++, C#, Python 等多种语言和框架（如Qt, MFC）调用。

本库内部实现基于 Qt 5，因此在部署时需要附带相关的 Qt 运行时库。

## 文件组成

- `PrintDeviceSDK.dll`: 动态链接库主文件。
- `PrintDeviceSDK.lib`: 链接时所需的导入库。
- `PrintDeviceSDK_API.h`: 包含所有SDK导出函数、枚举和结构体定义的头文件。

## API 核心概念：事件回调

本SDK采用异步事件驱动模型。调用一个动作（如`MoveTo`）会立即返回，而该动作的执行结果和过程状态（如 "Moving", "MoveFinished", "Error: Out of bounds"）将通过一个注册的回调函数异步推送给调用者。

**`SdkEvent` 结构体** 是所有信息的载体，包含：
- `type`: 事件类型 (连接状态, 打印进度, 错误等)。
- `message`: 人类可读的事件描述信息。
- `code`, `value1`, `value2`, `value3`: 根据事件类型携带不同的数据。

## API 参考

请参见 `PrintDeviceSDK_API.h` 获取完整的API函数列表。关键函数包括：

- **初始化/资源管理**: `InitSDK`, `ReleaseSDK`, `RegisterEventCallback`
- **连接管理**: `ConnectByTCP`, `ConnectBySerial`, `Disconnect`, `IsConnected`
- **运动控制**: `MoveTo`, `MoveBy`, `GoHome`
- **打印控制**: `LoadPrintData`, `StartPrint`, `PausePrint`, `ResumePrint`, `StopPrint`

## 快速开始

### 1. 链接库

- **Qt (.pro)**:
  ```pro
  LIBS += -L/path/to/lib/ -lPrintDeviceSDK
  INCLUDEPATH += /path/to/include/
  ```
- **Visual Studio (MFC/C++)**:
  - 添加头文件目录 (`PrintDeviceSDK_API.h`所在目录)。
  - 添加库目录 (`PrintDeviceSDK.lib`所在目录)。
  - 在链接器输入中添加 `PrintDeviceSDK.lib`。

### 2. 实现代码

```cpp
#include "PrintDeviceSDK_API.h"
#include <stdio.h>
#include <windows.h> // for Sleep

// 1. 实现一个回调函数来处理所有来自SDK的事件
void MyAwesomeCallback(const SdkEvent* event) {
    printf("[SDK EVENT] Type: %d, Msg: %s, Val1: %.2f\n", 
           event->type, event->message, event->value1);
    
    // 你可以在这里根据 event->type 更新UI或执行逻辑
    if (event->type == EVENT_TYPE_PRINT_STATUS) {
        printf("  - Print Progress: %.1f%%, Layer: %.0f/%.0f\n",
               event->value1, event->value2, event->value3);
    }
}

int main() {
    // 2. 初始化SDK并注册回调
    printf("Initializing SDK...\n");
    InitSDK("./logs"); // 指定日志目录
    RegisterEventCallback(MyAwesomeCallback);

    // 3. 连接设备
    printf("Connecting to device...\n");
    ConnectByTCP("192.168.1.100", 8080);

    // 等待连接成功的回调... (在实际应用中，这里应该是事件驱动的)
    Sleep(2000); 

    if (IsConnected()) {
        printf("Device connected. Starting print job.\n");
        // 4. 执行操作
        LoadPrintData("path/to/my/printfile.gcode");
        StartPrint();
    } else {
        printf("Failed to connect.\n");
    }

    // 主循环，等待打印完成...
    printf("Print job running. Press Ctrl+C to exit.\n");
    while(IsConnected()) {
        Sleep(5000); // 阻塞主线程以观察回调
    }

    // 5. 清理资源
    printf("Releasing SDK...\n");
    ReleaseSDK();

    return 0;
}
```

### 3. 部署

确保 `PrintDeviceSDK.dll` 以及它所依赖的所有 Qt 运行时库 (如 `Qt5Core.dll`, `Qt5Network.dll` 等) 都位于您的主可执行文件 (`.exe`) 所在的目录中。

---

## 构建说明

### 使用qmake构建

```bash
cd PrintDeviceSDK
qmake PrintDeviceSDK.pro
make        # Linux/Mac
nmake       # Windows (MSVC)
```

### 使用CMake构建

```bash
cd PrintDeviceSDK
mkdir build
cd build
cmake ..
cmake --build .
```

## 项目结构

```
PrintDeviceSDK/
├── PrintDeviceSDK_API.h      # SDK公共接口头文件
├── PrintDeviceSDK_API.cpp    # SDK实现文件（已完整实现）
├── PrintDeviceSDK.pro         # Qt项目文件
├── CMakeLists.txt            # CMake构建文件
├── README.md                 # 本文档
├── example/                  # 使用示例
│   ├── example_usage.cpp    # 示例程序源码
│   └── example.pro          # 示例程序项目文件
├── bin/                      # 编译输出目录
└── include/                  # 导出头文件目录
```

## 实现状态

本SDK已**完整实现**原Qt项目的所有功能，包括：

### 核心功能
- ✅ TCP连接管理（连接、断开、状态查询）
- ✅ 心跳机制（自动维持连接）
- ✅ 协议编解码（完整的通信协议支持）
- ✅ 事件回调系统（异步通知）

### 运动控制
- ✅ X/Y/Z轴绝对移动 (MoveTo)
- ✅ X/Y/Z轴相对移动 (MoveBy)
- ✅ 轴复位 (GoHome)
- ✅ 支持所有原项目的运动命令：
  - 自动前进/后退
  - 1CM移动
  - 移动到打印位置
  - 轴复位

### 打印控制
- ✅ 开始打印 (StartPrint)
- ✅ 停止打印 (StopPrint)
- ✅ 暂停打印 (PausePrint)
- ✅ 恢复打印 (ResumePrint)
- ✅ 打印复位 (ResetPrint)
- ✅ 图像数据加载和传输 (LoadPrintData)

### 通信协议
- ✅ 所有功能码命令
- ✅ 图像数据分包传输
- ✅ 命令应答处理
- ✅ 错误处理和报告

## 技术实现细节

1. **SDKManager类**: 内部管理类，负责：
   - TCP客户端管理
   - 协议处理器管理
   - 心跳定时器管理
   - 信号槽连接和事件分发

2. **线程安全**: 使用QMutex保护回调函数和共享数据

3. **事件驱动**: 采用Qt信号槽机制，所有操作结果通过回调异步通知

4. **内存管理**: 使用智能指针管理资源，自动释放

## 注意事项

1. **Qt事件循环**: SDK需要Qt事件循环支持。如果在非Qt应用中使用，需要创建QCoreApplication并运行事件循环。

2. **头文件路径**: SDK实现依赖原项目的源文件，编译时需要正确设置包含路径。

3. **依赖库**: 需要Qt5 Core和Network模块。

4. **部署**: 部署时需要包含：
   - PrintDeviceSDK.dll
   - Qt5Core.dll
   - Qt5Network.dll
   - 其他Qt依赖库

## 与原Qt项目的差异

本SDK**完全实现**了原Qt项目的后端功能，主要变化：

1. **去除UI**: 移除了所有UI相关代码（QWidget, QDialog等）
2. **C接口**: 提供标准C接口，可被多种语言调用
3. **事件回调**: 用回调函数替代Qt信号槽（对外接口）
4. **单例模式**: 使用单例管理器简化API设计
