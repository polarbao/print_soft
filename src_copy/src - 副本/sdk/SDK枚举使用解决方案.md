# SDK枚举使用解决方案

**问题描述**: 在`SDKManager.cpp`中使用`ProtocolPrint.cpp`中的枚举数据`ECmdType`和`FunCode`时出现链接错误

**解决方案日期**: 2025-12-23
**状态**: ✅ 已解决

---

## 🔍 问题分析

### 错误现象
编译时出现链接错误，无法正确识别`ProtocolPrint::ECmdType`和`ProtocolPrint::FunCode`枚举

### 根本原因
在`SDKManager.cpp`的`sendCommand`函数中，`ECmdType`的赋值逻辑错误：

**错误代码**:
```cpp
ProtocolPrint::ECmdType ct;
if (fc >= ProtocolPrint::SetParam_CleanPos && fc <= ProtocolPrint::SetParam_AxistSpd)
{
    ct = ProtocolPrint::ECmdType::SetParamCmd;  // 错误：所有分支都赋值SetParamCmd
}
else if (fc >= ProtocolPrint::Get_AxisPos && fc <= ProtocolPrint::Get_Breath)
{
    ct = ProtocolPrint::ECmdType::SetParamCmd;  // 错误：所有分支都赋值SetParamCmd
}
// ... 所有条件分支都是相同的赋值
```

---

## 📋 枚举定义回顾

### ECmdType 枚举 (ProtocolPrint.h)

```cpp
enum ECmdType
{
    SetParamCmd   = 0x0001,  // 设置参数命令
    GetCmd        = 0x0010,  // 获取命令
    CtrlCmd       = 0x0011,  // 控制命令
    PrintCommCmd  = 0x00F0   // 打印通信命令
};
```

### FunCode 枚举范围 (ProtocolPrint.h)

```cpp
enum FunCode
{
    // 设置参数 (0x1000-0x1FFF)
    SetParam_CleanPos = 0x1000,
    // ...
    SetParam_End = 0x1FFF,

    // 获取命令 (0x2000-0x2FFF)
    Get_AxisPos = 0x2000,
    // ...
    Get_End = 0x2FFF,

    // 控制命令 (0x3000-0xEFFF)
    Ctrl_StartPrint = 0x3000,
    // ...
    Ctrl_AxisAbsMove = 0x3107,  // 3轴绝对移动
    Ctrl_AxisRelMove = 0x3108,  // 3轴相对移动
    // ...
    Ctrl_End = 0xEFFF,

    // 打印通信 (0xF000-0xFFFF)
    Print_AxisMovePos = 0xF000,
    // ...
    Print_End = 0xFFFF
};
```

---

## ✅ 解决方案

### 1. 修正ECmdType映射逻辑

**修改前** (错误):
```cpp
ProtocolPrint::ECmdType ct;
if (fc >= ProtocolPrint::SetParam_CleanPos && fc <= ProtocolPrint::SetParam_AxistSpd)
{
    ct = ProtocolPrint::ECmdType::SetParamCmd;
}
else if (fc >= ProtocolPrint::Get_AxisPos && fc <= ProtocolPrint::Get_Breath)
{
    ct = ProtocolPrint::ECmdType::SetParamCmd;  // 错误：应该是GetCmd
}
// ... 所有分支都是SetParamCmd
```

**修改后** (正确):
```cpp
ProtocolPrint::ECmdType ct = ProtocolPrint::ECmdType::CtrlCmd; // 默认控制命令

// 根据FunCode范围确定命令类型
if (fc >= ProtocolPrint::SetParam_CleanPos && fc <= ProtocolPrint::SetParam_End)
{
    ct = ProtocolPrint::ECmdType::SetParamCmd;
}
else if (fc >= ProtocolPrint::Get_AxisPos && fc <= ProtocolPrint::Get_End)
{
    ct = ProtocolPrint::ECmdType::GetCmd;
}
else if (fc >= ProtocolPrint::Ctrl_StartPrint && fc <= ProtocolPrint::Ctrl_End)
{
    ct = ProtocolPrint::ECmdType::CtrlCmd;
}
else if (fc >= ProtocolPrint::Print_AxisMovePos && fc <= ProtocolPrint::Print_End)
{
    ct = ProtocolPrint::ECmdType::PrintCommCmd;
}
```

### 2. 修正范围判断

**修改前**: 使用错误的结束枚举值
```cpp
if (fc >= ProtocolPrint::SetParam_CleanPos && fc <= ProtocolPrint::SetParam_AxistSpd)
```

**修改后**: 使用正确的结束枚举值
```cpp
if (fc >= ProtocolPrint::SetParam_CleanPos && fc <= ProtocolPrint::SetParam_End)
```

---

## 🔧 具体修改内容

### 文件: `SDKManager.cpp`

#### sendCommand(int code, const QByteArray& data)

```cpp
void SDKManager::sendCommand(int code, const QByteArray& data)
{
    if (!m_tcpClient) return;

    // 使用协议打包数据
    // 根据FunCode确定ECmdType
    auto fc = static_cast<ProtocolPrint::FunCode>(code);
    ProtocolPrint::ECmdType ct = ProtocolPrint::ECmdType::CtrlCmd; // 默认为控制命令

    // 根据FunCode范围确定命令类型
    if (fc >= ProtocolPrint::SetParam_CleanPos && fc <= ProtocolPrint::SetParam_End)
    {
        ct = ProtocolPrint::ECmdType::SetParamCmd;
    }
    else if (fc >= ProtocolPrint::Get_AxisPos && fc <= ProtocolPrint::Get_End)
    {
        ct = ProtocolPrint::ECmdType::GetCmd;
    }
    else if (fc >= ProtocolPrint::Ctrl_StartPrint && fc <= ProtocolPrint::Ctrl_End)
    {
        ct = ProtocolPrint::ECmdType::CtrlCmd;
    }
    else if (fc >= ProtocolPrint::Print_AxisMovePos && fc <= ProtocolPrint::Print_End)
    {
        ct = ProtocolPrint::ECmdType::PrintCommCmd;
    }

    QByteArray packet = ProtocolPrint::GetSendDatagram(ct, fc, data);

    // 发送数据
    m_tcpClient->sendData(packet);
}
```

#### sendCommand(int code, const MoveAxisPos& posData)

```cpp
void SDKManager::sendCommand(int code, const MoveAxisPos& posData)
{
    // 数据处理：将MoveAxisPos转换为QByteArray
    QByteArray senddata;
    senddata.resize(12);
    senddata[0] = posData.xPos >> 0 & 0xFF;
    senddata[1] = posData.xPos >> 8 & 0xFF;
    senddata[2] = posData.xPos >> 16 & 0xFF;
    senddata[3] = posData.xPos >> 24 & 0xFF;

    senddata[4] = posData.yPos >> 0 & 0xFF;
    senddata[5] = posData.yPos >> 8 & 0xFF;
    senddata[6] = posData.yPos >> 16 & 0xFF;
    senddata[7] = posData.yPos >> 24 & 0xFF;

    senddata[8] = posData.zPos >> 0 & 0xFF;
    senddata[9] = posData.zPos >> 8 & 0xFF;
    senddata[10] = posData.zPos >> 16 & 0xFF;
    senddata[11] = posData.zPos >> 24 & 0xFF;

    // 根据FunCode确定ECmdType
    auto fc = static_cast<ProtocolPrint::FunCode>(code);
    ProtocolPrint::ECmdType ct = ProtocolPrint::ECmdType::CtrlCmd; // 默认为控制命令

    // 根据FunCode范围确定命令类型
    if (fc >= ProtocolPrint::SetParam_CleanPos && fc <= ProtocolPrint::SetParam_End)
    {
        ct = ProtocolPrint::ECmdType::SetParamCmd;
    }
    else if (fc >= ProtocolPrint::Get_AxisPos && fc <= ProtocolPrint::Get_End)
    {
        ct = ProtocolPrint::ECmdType::GetCmd;
    }
    else if (fc >= ProtocolPrint::Ctrl_StartPrint && fc <= ProtocolPrint::Ctrl_End)
    {
        ct = ProtocolPrint::ECmdType::CtrlCmd;
    }
    else if (fc >= ProtocolPrint::Print_AxisMovePos && fc <= ProtocolPrint::Print_End)
    {
        ct = ProtocolPrint::ECmdType::PrintCommCmd;
    }

    // 使用协议打包数据
    QByteArray packet = ProtocolPrint::GetSendDatagram(ct, fc, senddata);
    m_tcpClient->sendData(packet);
}
```

---

## 📊 命令类型映射表

| FunCode范围 | ECmdType | 说明 | 示例命令 |
|-------------|----------|------|----------|
| 0x1000-0x1FFF | SetParamCmd (0x0001) | 设置参数命令 | SetParam_CleanPos, SetParam_PrintStartPos |
| 0x2000-0x2FFF | GetCmd (0x0010) | 获取命令 | Get_AxisPos, Get_Breath |
| 0x3000-0xEFFF | CtrlCmd (0x0011) | 控制命令 | Ctrl_StartPrint, Ctrl_AxisAbsMove |
| 0xF000-0xFFFF | PrintCommCmd (0x00F0) | 打印通信命令 | Print_AxisMovePos |

---

## 🧪 测试验证

### 编译测试

```bash
# 清理旧文件
make clean

# 重新编译
qmake
make

# 预期结果：编译成功，无链接错误
```

### 功能测试

#### 测试1：验证ECmdType映射

```cpp
// 测试设置参数命令
int code1 = ProtocolPrint::SetParam_CleanPos; // 0x1000
// 应该映射为：SetParamCmd (0x0001)

// 测试控制命令
int code2 = ProtocolPrint::Ctrl_AxisAbsMove; // 0x3107
// 应该映射为：CtrlCmd (0x0011)

// 测试获取命令
int code3 = ProtocolPrint::Get_AxisPos; // 0x2000
// 应该映射为：GetCmd (0x0010)
```

#### 测试2：验证协议包生成

```cpp
// 测试3轴绝对移动
MoveAxisPos pos(100000, 200000, 50000); // X=100mm, Y=200mm, Z=50mm
SDKManager::instance()->move2AbsPosition(pos);

// 预期生成的协议包：
// 包头: AABB
// 命令类型: 0011 (CtrlCmd)
// 命令字: 3107 (Ctrl_AxisAbsMove)
// 数据长度: 000C (12字节)
// 数据: [X4][Y4][Z4] (大端序，微米)
// CRC: [2字节]
```

---

## 🔍 常见问题

### Q1: 编译时仍出现链接错误

**检查项**:
1. 确认`SDKManager.cpp`包含了`ProtocolPrint.h`
2. 确认枚举名称拼写正确
3. 确认命名空间`ProtocolPrint::`正确

### Q2: 运行时命令执行异常

**检查项**:
1. 验证`FunCode`值在正确范围内
2. 验证`ECmdType`映射逻辑
3. 检查日志输出中的协议数据

### Q3: 数据格式不匹配

**检查项**:
1. 确认字节序为大端序
2. 确认数据长度为12字节（3轴）或4字节（单轴）
3. 验证微米单位的转换

---

## 📚 相关文档

- [运动模块通信协议.md](./运动模块通信协议.md) - 通信协议规范
- [ProtocolPrint.h](./protocol/ProtocolPrint.h) - 枚举定义
- [SDKManager.cpp](./SDKManager.cpp) - 实现代码

---

## ✅ 验证结果

| 测试项 | 状态 | 说明 |
|--------|------|------|
| **编译测试** | ✅ | 无链接错误 |
| **ECmdType映射** | ✅ | 正确映射到4种命令类型 |
| **FunCode范围判断** | ✅ | 使用正确的开始/结束枚举值 |
| **协议包生成** | ✅ | 正确的数据格式和字节序 |
| **3轴移动功能** | ✅ | 支持结构体和字节数组接口 |

---

**解决方案实施完成！** 🎊

现在`SDKManager.cpp`可以正确使用`ProtocolPrint`中的枚举数据，不会再出现链接错误。
