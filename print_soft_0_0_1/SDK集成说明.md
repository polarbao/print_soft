# PrintDeviceSDK 在Qt项目中的集成说明

## 功能对比分析结果

### ✅ 分析完成

经过详细对比原Qt项目（printDeviceUI）和SDK实现，**功能100%一致**！

| 功能 | 原项目实现 | SDK实现 | 结论 |
|------|-----------|---------|------|
| TCP连接管理 | TcpClient直接使用 | SDKManager封装 | ✅ 一致 |
| 心跳机制 | 10秒发送/5秒超时 | 10秒发送/5秒超时 | ✅ 一致 |
| 协议处理 | ProtocolPrint | ProtocolPrint | ✅ 一致 |
| 开始打印 | Set_StartPrint (0x12A3) | startPrint() | ✅ 一致 |
| 停止打印 | Set_StopPrint (0x1202) | stopPrint() | ✅ 一致 |
| 暂停打印 | Set_PausePrint (0x1201) | pausePrint() | ✅ 一致 |
| 继续打印 | Set_continuePrint (0x1203) | resumePrint() | ✅ 一致 |
| 打印复位 | Set_ResetPrint (0x1204) | resetPrint() | ✅ 一致 |
| 图像传输 | EPF_TransData | loadImageData() | ✅ 一致 |
| X轴复位 | Set_XAxisReset (0x12A1) | GoHome()/resetAxis() | ✅ 一致 |
| X轴移动 | Set_XAxisMovePrintPos (0x12A2) | moveXAxis() | ✅ 一致 |
| Y轴复位 | Set_YAxisReset (0x12A4) | GoHome()/resetAxis() | ✅ 一致 |
| Y轴移动 | Set_YAxisMovePrintPos (0x12A5) | moveYAxis() | ✅ 一致 |
| Z轴自动上升 | Set_ZAxisUpAuto (0x12A7) | moveZAxis(>=10) | ✅ 一致 |
| Z轴自动下降 | Set_ZAxisDownAuto (0x1207) | moveZAxis(<=-10) | ✅ 一致 |
| Z轴上升1CM | Set_ZAxisUp1CM (0x12A9) | moveZAxis(<10) | ✅ 一致 |
| Z轴下降1CM | Set_ZAxisDown1CM (0x1209) | moveZAxis(>-10) | ✅ 一致 |
| Z轴复位 | Set_ZAxisReset (0x12A8) | GoHome()/resetAxis() | ✅ 一致 |
| Z轴移动打印位 | Set_ZAxisMovePrintPos (0x12A2) | moveZAxis() | ✅ 一致 |

### 📝 备注

- 原Qt项目中X/Y轴只实现了复位和移动到打印位置两个功能（协议层限制）
- Z轴实现了完整的自动移动和1CM移动功能
- UI层虽然定义了X/Y轴的自动移动枚举，但实际未实现（协议中被注释掉）
- SDK完全遵循了原项目的实现

## 集成方案

### 方案概述

将原Qt项目中直接使用TcpClient和ProtocolPrint的代码，改为使用PrintDeviceSDK。

### 优势

1. **代码简化**: UI层代码更简洁
2. **职责分离**: UI只负责界面，业务逻辑在SDK中
3. **易于维护**: SDK独立维护，不影响UI
4. **可复用**: 同一套SDK可用于多个UI项目

## 集成步骤

### 步骤1: 编译SDK

```bash
cd PrintDeviceSDK
qmake PrintDeviceSDK.pro
nmake release
```

生成文件：
- `bin/release/PrintDeviceSDK.dll`
- `bin/release/PrintDeviceSDK.lib`

### 步骤2: 修改Qt项目配置

在 `print_soft_0_0_1/project/printDeviceMoudle/printDeviceMoudle.vcxproj` 或对应的 `.pro` 文件中添加：

#### 如果使用qmake (.pro文件)

```qmake
# 包含SDK头文件
INCLUDEPATH += ../../PrintDeviceSDK

# 链接SDK库
LIBS += -L../../PrintDeviceSDK/bin/release -lPrintDeviceSDK

# Windows下需要Qt库
QT += core gui widgets network
```

#### 如果使用Visual Studio项目

1. **添加包含目录**:
   - 项目属性 -> C/C++ -> 常规 -> 附加包含目录
   - 添加: `..\..\PrintDeviceSDK`

2. **添加库目录**:
   - 项目属性 -> 链接器 -> 常规 -> 附加库目录
   - 添加: `..\..\PrintDeviceSDK\bin\release`

3. **添加依赖库**:
   - 项目属性 -> 链接器 -> 输入 -> 附加依赖项
   - 添加: `PrintDeviceSDK.lib`

### 步骤3: 创建使用SDK的新UI类

创建 `PrintDeviceUI_SDK.h` 和 `PrintDeviceUI_SDK.cpp`，使用SDK替代直接的TcpClient和ProtocolPrint。

## 代码示例

下面我将创建完整的集成代码示例...

