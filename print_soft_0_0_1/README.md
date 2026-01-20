# 打印设备运动控制系统 (Print Motion Control System)

[![Platform](https://img.shields.io/badge/platform-Windows-blue.svg)](https://www.microsoft.com/windows)
[![Qt](https://img.shields.io/badge/Qt-5.9+-green.svg)](https://www.qt.io/)
[![VS](https://img.shields.io/badge/Visual%20Studio-2017-purple.svg)](https://visualstudio.microsoft.com/)
[![License](https://img.shields.io/badge/license-Proprietary-red.svg)]()

> 基于 Qt 5 的工业级 3 轴运动控制 SDK 与应用程序

---

## 📋 目录

- [简介](#-简介)
- [功能特性](#-功能特性)
- [快速开始](#-快速开始)
- [项目结构](#-项目结构)
- [技术架构](#-技术架构)
- [开发文档](#-开发文档)
- [常见问题](#-常见问题)
- [更新日志](#-更新日志)

---

## 🎯 简介

**print_soft_0_0_1** 是一个专业的工业打印设备运动控制解决方案，提供了完整的 **客户端-SDK** 分离架构。系统支持 **微米级精度** 的 XYZ 三轴运动控制，通过 TCP 协议与下位机通信，适用于喷墨打印、3D 打印、激光雕刻等工业自动化场景。

### 核心亮点

- ✅ **高精度控制**: 微米级 (μm) 精度，支持单轴/多轴同步运动
- ✅ **模块化设计**: SDK 与应用解耦，易于集成到第三方项目
- ✅ **实时反馈**: 丰富的 Qt 信号机制，实时监控设备状态
- ✅ **完善日志**: 集成 spdlog，支持 25+ Qt 类型格式化输出
- ✅ **稳定通信**: 自定义 TCP 协议 + CRC16 校验 + 心跳机制
- ✅ **配置驱动**: 通过 INI 文件灵活配置运动参数

---

## ⚡ 功能特性

### 运动控制
- 单轴绝对/相对移动 (X/Y/Z 独立控制)
- 三轴同步移动 (支持结构体/字节数组双接口)
- 轴复位 (回原点)
- 实时位置反馈 (毫米/微米可选)
- 速度/加速度动态调整

### 打印流程
- 开始/暂停/恢复/停止打印
- 打印进度实时更新
- PASS 完成通知
- 打印区域配置 (起点/终点/清洗位)

### 参数管理
- INI 配置文件加载/保存
- 运动参数动态下发 (速度/加速度/步长/限位)
- 原点偏移设置
- 图层数据配置

### 通信协议
- TCP Socket 连接管理
- 自定义二进制协议 (小端序)
- CRC16 数据校验
- 自动重连 + 心跳检测
- 命令应答机制

---

## 🚀 快速开始

### 环境要求

| 组件 | 版本 | 备注 |
|------|------|------|
| **操作系统** | Windows 10/11 (x64) | - |
| **Visual Studio** | 2017 (v141) | 需安装 Qt VS Tools |
| **Qt** | 5.9 ~ 5.15 | MSVC 2017 64-bit |
| **Qt 模块** | Core, Widgets, Network | - |

### 5 分钟快速构建

```bash
# 1. 克隆/解压项目
cd print_soft_0_0_1

# 2. 打开 Visual Studio 解决方案
start project/manu_print_soft.sln

# 3. 选择配置: Release | x64

# 4. 生成解决方案 (F7)
# 构建输出: bin/Release/

# 5. 部署 Qt 依赖
cd bin/Release
windeployqt print_moudle.exe

# 6. 运行程序
print_moudle.exe
```

### 简单示例

```cpp
#include "motionControlSDK.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 1. 创建 SDK 实例
    motionControlSDK sdk;
    sdk.MC_Init("./logs");
    
    // 2. 连接设备
    QObject::connect(&sdk, &motionControlSDK::connected, []() {
        qDebug() << "设备连接成功";
    });
    sdk.MC_Connect2Dev("192.168.1.100", 12355);
    
    // 3. 移动到指定坐标 (X=100mm, Y=200mm, Z=50mm)
    MoveAxisPos pos = MoveAxisPos::fromMillimeters(100, 200, 50);
    sdk.MC_move2AbsAxisPos(pos);
    
    // 4. 监听位置变化
    QObject::connect(&sdk, &motionControlSDK::MC_SigPosChanged, 
                     [](double x, double y, double z) {
        qDebug() << "当前坐标:" << x << y << z;
    });
    
    return app.exec();
}
```

---

## 📂 项目结构

```
print_soft_0_0_1/
│
├── 📁 project/                     # Visual Studio 工程文件
│   ├── manu_print_soft.sln         # VS 解决方案
│   ├── motionControlSDK/           # SDK 项目 (DLL)
│   └── printDeviceMoudle/          # UI 应用项目 (EXE)
│
├── 📁 src/                         # 源代码
│   ├── main.cpp                    # 应用程序入口
│   ├── printDeviceMoudle.h/.cpp    # 主窗口
│   ├── 📁 sdk/                     # **SDK 核心代码**
│   │   ├── motionControlSDK.h      # 公共 API 头文件 ⭐
│   │   ├── SDKManager.h/.cpp       # SDK 管理器
│   │   ├── SDKMotion_New.cpp       # 运动控制实现
│   │   ├── SDKPrint.cpp            # 打印控制实现
│   │   ├── 📁 protocol/            # 协议层
│   │   │   └── ProtocolPrint.h/.cpp
│   │   └── 📁 communicate/         # 通信层
│   │       └── TcpClient.h/.cpp
│   ├── 📁 ui/                      # UI 层
│   │   └── printDeviceUI.h/.cpp
│   └── 📁 comm/                    # 公共组件
│       └── SpdlogMgr.h/.cpp        # 日志管理
│
├── 📁 bin/Release/                 # 发布目录
│   ├── print_moudle.exe            # 主程序 ⭐
│   ├── motionControlSDK.dll        # SDK 动态库 ⭐
│   ├── config/                     # 配置文件
│   │   └── motionMoudleConfig.ini
│   └── log/                        # 日志目录
│
├── 📁 ext/                         # 第三方库
│   ├── inc/spdlog/                 # spdlog 头文件
│   └── lib/spdlog/                 # spdlog 静态库
│
├── 📄 README.md                    # 本文件
└── 📄 项目技术文档.md               # 详细技术文档 📚
```

---

## 🏗️ 技术架构

### 系统架构图

```
┌─────────────────────────────────────────┐
│      UI Application Layer               │
│  ┌───────────────┐  ┌───────────────┐  │
│  │printDeviceUI  │  │printDeviceMoudle│ │
│  │ (用户界面)    │  │  (主窗口)      │  │
│  └───────┬───────┘  └───────────────┘  │
└──────────┼──────────────────────────────┘
           │ Qt Signals/Slots
┌──────────▼──────────────────────────────┐
│    Motion Control SDK (DLL)             │
│  ┌───────────────────────────────────┐  │
│  │  motionControlSDK (公共 API)      │  │
│  └────────────┬──────────────────────┘  │
│               │ Pimpl 模式               │
│  ┌────────────▼──────────────────────┐  │
│  │  SDKManager (内部管理器)          │  │
│  │  - 运动控制  - 打印控制           │  │
│  │  - 参数管理  - 连接管理           │  │
│  └────────────┬──────────────────────┘  │
│               │                          │
│  ┌────────────▼──────────────────────┐  │
│  │  ProtocolPrint (协议层)           │  │
│  │  - 数据封装/解析  - CRC16 校验    │  │
│  └────────────┬──────────────────────┘  │
│               │                          │
│  ┌────────────▼──────────────────────┐  │
│  │  TcpClient (TCP 通信层)           │  │
│  │  - Socket 管理  - 心跳检测        │  │
│  └───────────────────────────────────┘  │
└──────────────┬──────────────────────────┘
               │ TCP (Port: 5555/12355)
┌──────────────▼──────────────────────────┐
│       下位机运动控制器                   │
│     (XYZ 三轴电机驱动板)                 │
└─────────────────────────────────────────┘
```

### 关键设计模式

- **Pimpl (Pointer to Implementation)**: SDK 公共 API 隐藏实现细节
- **单例模式**: `SDKManager`, `SpdlogWrapper` 全局唯一实例
- **观察者模式**: Qt 信号槽机制实现事件驱动
- **工厂模式**: 协议数据包的创建

### 核心数据类型

```cpp
// 三轴坐标 (微米单位)
struct MoveAxisPos {
    quint32 xPos;  // X 轴 (μm)
    quint32 yPos;  // Y 轴 (μm)
    quint32 zPos;  // Z 轴 (μm)
    
    // 毫米 → 微米
    static MoveAxisPos fromMillimeters(double x_mm, double y_mm, double z_mm);
};

// 运动配置
struct MotionConfig {
    QString ip;             // 设备 IP
    int port;               // 端口
    MoveAxisPos startPos;   // 打印起点
    MoveAxisPos endPos;     // 打印终点
    MoveAxisPos speed;      // 速度
    MoveAxisPos acc;        // 加速度
    // ... 更多参数
};
```

---

## 📚 开发文档

### 完整技术文档

详细的技术文档请参阅: **[项目技术文档.md](项目技术文档.md)**

文档包含:
- ✅ 详细的 API 参考手册
- ✅ 通信协议完整规范
- ✅ 数据结构说明
- ✅ 开发指南和最佳实践
- ✅ 故障排查与调试技巧
- ✅ 常见问题解答

### API 快速查阅

```cpp
// === 生命周期 ===
bool MC_Init(const QString& logDir);       // 初始化 SDK
void MC_Release();                         // 释放资源

// === 连接管理 ===
bool MC_Connect2Dev(const QString& ip, quint16 port = 12355);
void MC_DisconnectDev();
bool MC_IsConnected() const;

// === 运动控制 ===
bool MC_moveXAxis(const MoveAxisPos& pos);        // 单轴移动
bool MC_moveYAxis(const MoveAxisPos& pos);
bool MC_moveZAxis(const MoveAxisPos& pos);
bool MC_move2AbsAxisPos(const MoveAxisPos& pos);  // 三轴同步移动
bool MC_GoHome(int x, int y, int z);              // 回原点

// === 打印控制 ===
bool MC_StartPrint();                      // 开始打印
bool MC_PausePrint();                      // 暂停打印
bool MC_ResumePrint();                     // 恢复打印
bool MC_StopPrint();                       // 停止打印

// === 配置管理 ===
bool MC_LoadMotionConfig(MotionConfig& config, const QString& path);
bool MC_SetMotionConfig(const MotionConfig& config);

// === 信号 ===
void connected();                          // 连接成功
void MC_SigDisconnected();                 // 连接断开
void MC_SigPosChanged(double x, double y, double z);  // 位置更新
void MC_SigErrOccurred(int code, const QString& msg); // 错误发生
```

### 配置文件示例

**config/motionMoudleConfig.ini**:
```ini
[net_param]
motion_ip = 192.168.1.100
motion_port = 12355

[print_pos_param]
print_start_pos = 1000, 1000, 0      # 单位: 微米
print_end_pos = 2000, 2000, 20
print_clean_pos = 100, 100, 0

[motion_speed_param]
x_axis_speed = 10                     # 单位: mm/s
y_axis_speed = 10
z_axis_speed = 10
```

---

## ❓ 常见问题

### Q1: 启动时提示缺少 Qt5Core.dll
**A**: 运行 `windeployqt print_moudle.exe` 自动部署 Qt 依赖库。

### Q2: 连接设备失败
**A**: 
1. 检查网络: `ping 192.168.1.100`
2. 确认端口: 默认 5555 或 12355
3. 查看防火墙规则
4. 检查日志: `log/print_motion_moudle.txt`

### Q3: 坐标移动不准确
**A**: 
- 确认使用微米单位: `MoveAxisPos::fromMillimeters(100, 200, 50)`
- 检查设备是否需要校准

### Q4: 如何集成 SDK 到自己的项目？
**A**: 
```cpp
// 1. 添加头文件搜索路径
//    项目属性 → C/C++ → 常规 → 附加包含目录
//    添加: $(ProjectDir)..\..\src\sdk

// 2. 添加库依赖
//    项目属性 → 链接器 → 输入 → 附加依赖项
//    添加: motionControlSDK.lib

// 3. 复制 DLL 到输出目录
//    项目属性 → 生成事件 → 后期生成事件
//    添加: copy "$(SolutionDir)..\..\bin\Release\motionControlSDK.dll" "$(OutDir)"

// 4. 在代码中使用
#include "motionControlSDK.h"
motionControlSDK* sdk = new motionControlSDK();
sdk->MC_Init();
```

### Q5: 如何启用详细日志？
**A**:
```cpp
// 在 main.cpp 中
SpdlogWrapper::GetInstance()->Init("./logs");
SpdlogWrapper::GetInstance()->SetLogLevel(SpdlogWrapper::LogLevel::Debug);
```

---

## 🔄 更新日志

### Version 0.0.1 (2026-01-20)
**初始版本**
- ✅ 完成 SDK 核心架构
- ✅ 实现 TCP 通信协议
- ✅ 支持三轴运动控制
- ✅ 集成 spdlog 日志系统
- ✅ 提供完整的 UI 应用示例
- ✅ 编写技术文档和 README

---

## 📞 技术支持

- **问题反馈**: [请填写 Issue 地址]
- **技术文档**: [项目技术文档.md](项目技术文档.md)
- **代码仓库**: [请填写 Git 地址]

---

## 📄 许可证

本项目为商业软件，版权所有。未经授权不得用于商业用途。

---

## 🙏 致谢

- [spdlog](https://github.com/gabime/spdlog) - 高性能日志库
- [Qt](https://www.qt.io/) - 跨平台 GUI 框架
- [Visual Studio](https://visualstudio.microsoft.com/) - 强大的 IDE

---

**最后更新**: 2026-01-20  
**文档版本**: 1.0.0  
**维护者**: Development Team

