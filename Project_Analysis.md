# manu_print_soft 项目分析

## 1. 项目概述

`manu_print_soft` 是一个基于 C++/Qt5 的打印设备控制软件，开发环境为 Visual Studio。其主要功能是与打印设备进行通信，并控制其执行打印和运动等任务。

## 2. 技术栈

- **编程语言**: C++
- **核心框架**: Qt 5.x
- **构建系统**: MSVC (Visual Studio)
- **通信协议**: TCP/IP (通过 `TcpClient`)，可能包含串口通信。

## 3. 模块划分

根据源代码结构，项目可分为以下几个核心模块：

- **UI模块 (`src/ui/`)**:
  - `printDeviceUI`, `moveDeviceUI`
  - 负责提供用户交互界面，并调用服务层的功能。

- **服务模块 (`src/service/`)**:
  - `printDevice`, `moveDevice`
  - 封装了核心业务逻辑，是功能实现的主体。例如，`moveDevice` 封装了所有与运动控制相关的操作。

- **协议模块 (`src/protocol/`)**:
  - `ProtocolPrint`, `ProtocolMoveTmp`
  - 负责将服务层的指令打包成设备能够识别的二进制数据格式，并解析设备返回的数据。

- **通信模块 (`src/communicate/`)**:
  - `TcpClient`, `SerialPortData`
  - 负责底层的数据收发，建立和管理与硬件设备的物理连接。

- **通用模块 (`src/comm/`)**:
  - `CLogManager`, `global`
  - 提供日志记录、全局变量/函数等通用支持功能。

## 4. 关键文件识别

- **运动控制核心逻辑**:
  - `src/service/moveDevice.h` / `.cpp`
  - `src/protocol/ProtocolMoveTmp.h` / `.cpp`
- **通信实现**:
  - `src/communicate/TcpClient.h` / `.cpp`
- **主程序入口**:
  - `src/main.cpp`
- **UI定义**:
  - `src/printDeviceMoudle.ui`
