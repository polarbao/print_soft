# MoveAxisPos快速集成清单

**目标**: 将`MoveAxisPos`结构体集成到SDK，实现微米级精度的位置控制  
**预计时间**: 2小时  
**单位**: 所有位置数据以**微米(μm)**为单位

---

## ✅ 文件修改清单

### 1. motionControlSDK.h（已完成）

**位置**: `print_soft_0_0_1/src/sdk/motionControlSDK.h`

**修改内容**:
- [x] 在`SdkEventCallback`定义之后添加`MoveAxisPos`结构体定义
- [x] 添加`Q_DECLARE_METATYPE(MoveAxisPos)`
- [x] 在类中添加新的API接口：
  - `moveToPosition(const MoveAxisPos&)`
  - `setTargetPosition(const MoveAxisPos&)`
  - `getTargetPosition()`
  - `getCurrentPosition()`

**状态**: ✅ 已完成

---

### 2. motionControlSDK.cpp（需要添加）

**位置**: `print_soft_0_0_1/src/sdk/motionControlSDK.cpp`

**需要添加的内容**:

```cpp
// 在构造函数中注册元类型
motionControlSDK::motionControlSDK(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
    QMutexLocker locker(&Private::s_mutex);
    Private::s_instance = this;
    
    // ✅ 添加这一行
    qRegisterMetaType<MoveAxisPos>("MoveAxisPos");
}

// 在文件末尾添加新API实现
bool motionControlSDK::moveToPosition(const MoveAxisPos& targetPos)
{
    if (!isConnected()) {
        emit errorOccurred(-1, tr("设备未连接"));
        return false;
    }
    
    if (!setTargetPosition(targetPos)) {
        return false;
    }
    
    double x_mm, y_mm, z_mm;
    targetPos.toMillimeters(x_mm, y_mm, z_mm);
    
    LOG_INFO(QString(u8"移动到目标位置: X=%1mm, Y=%2mm, Z=%3mm")
        .arg(x_mm, 0, 'f', 3)
        .arg(y_mm, 0, 'f', 3)
        .arg(z_mm, 0, 'f', 3));
    
    return moveTo(x_mm, y_mm, z_mm);
}

bool motionControlSDK::setTargetPosition(const MoveAxisPos& targetPos)
{
    if (!d->initialized) {
        emit errorOccurred(-1, tr("SDK未初始化"));
        return false;
    }
    
    SDKManager::instance()->setTargetPosition(targetPos);
    
    double x_mm, y_mm, z_mm;
    targetPos.toMillimeters(x_mm, y_mm, z_mm);
    
    LOG_INFO(QString(u8"设置目标位置: X=%1mm, Y=%2mm, Z=%3mm")
        .arg(x_mm, 0, 'f', 3)
        .arg(y_mm, 0, 'f', 3)
        .arg(z_mm, 0, 'f', 3));
    
    return true;
}

MoveAxisPos motionControlSDK::getTargetPosition() const
{
    if (!d->initialized) {
        return MoveAxisPos();
    }
    
    return SDKManager::instance()->getTargetPosition();
}

MoveAxisPos motionControlSDK::getCurrentPosition() const
{
    if (!d->initialized) {
        return MoveAxisPos();
    }
    
    return SDKManager::instance()->getCurrentPosition();
}
```

**状态**: ⚠️ 需要添加

---

### 3. SDKManager.h（已部分完成）

**位置**: `print_soft_0_0_1/src/sdk/SDKManager.h`

**需要确认的内容**:
- [x] 包含`motionControlSDK.h`或前向声明`struct MoveAxisPos`
- [x] 在公开部分添加位置管理方法
- [x] 在槽函数部分有`onHandleRecvDataOper(int, const MoveAxisPos&)`

**补充代码**（如果没有）:

```cpp
// 在文件顶部（第21行附近）
#include "motionControlSDK.h"  // 或者 struct MoveAxisPos;

// 在公开方法部分添加
public:
    /**
     * @brief 设置目标位置（微米单位）
     */
    void setTargetPosition(const MoveAxisPos& targetPos);
    
    /**
     * @brief 获取目标位置（微米单位）
     */
    MoveAxisPos getTargetPosition() const;
    
    /**
     * @brief 获取当前位置（微米单位）
     */
    MoveAxisPos getCurrentPosition() const;

// 在私有槽函数部分确认有
private slots:
    /**
     * @brief 处理接收到的位置数据
     */
    void onHandleRecvDataOper(int code, const MoveAxisPos& pos);
```

**状态**: ✅ 已完成

---

### 4. SDKManager_Position.cpp（新建）

**位置**: `print_soft_0_0_1/src/sdk/SDKManager_Position.cpp`

**状态**: ✅ 已创建（完整文件）

**内容**: 位置管理和接收处理的完整实现

---

### 5. SDKMotion.cpp（需要替换）

**位置**: `print_soft_0_0_1/src/sdk/SDKMotion.cpp`

**操作**: 
1. 备份原文件：`SDKMotion.cpp` → `SDKMotion_old.cpp`
2. 将`SDKMotion_New.cpp`重命名为`SDKMotion.cpp`

或者直接替换内容。

**新实现的功能**:
- ✅ 使用`MoveAxisPos`结构体（微米单位）
- ✅ 转换为`QByteArray`协议数据
- ✅ 详细的日志输出
- ✅ 支持绝对和相对移动

**状态**: ✅ 新文件已创建（`SDKMotion_New.cpp`）

---

### 6. ProtocolPrint.h（需要确认）

**位置**: `print_soft_0_0_1/src/sdk/protocol/ProtocolPrint.h`

**需要确认**:
- [x] 移除旧的`MoveAxisPos`定义（如果有的话）
- [x] 确认有`SigHandleFunOper2(int, MoveAxisPos)`信号

**补充代码**（如果信号不存在）:

```cpp
signals:
    // ...其他信号...
    
    /**
     * @brief 功能操作信号2（带位置数据）
     * @param code 命令码
     * @param data 位置数据
     */
    void SigHandleFunOper2(int code, MoveAxisPos data);
```

**状态**: ✅ 已确认（根据用户修改记录）

---

### 7. ProtocolPrint.cpp（已修改）

**位置**: `print_soft_0_0_1/src/sdk/protocol/ProtocolPrint.cpp`

**已完成的修改**（根据用户提供的diff）:
- [x] 在`ParseRespPackageData()`中填充`PackParam`
- [x] 解析位置数据到`MoveAxisPos`
- [x] 发射`SigHandleFunOper2(code, posData)`信号

**状态**: ✅ 已完成

---

### 8. SDKManager.cpp（需要添加信号连接）

**位置**: `print_soft_0_0_1/src/sdk/SDKManager.cpp`

**在`init()`方法中添加**:

```cpp
bool SDKManager::init(const QString& log_dir) 
{
    // ...现有代码...
    
    // 连接协议处理器信号
    connect(m_protocol.get(), &ProtocolPrint::SigHeartBeat, 
            this, &SDKManager::onHeartbeat);
    connect(m_protocol.get(), &ProtocolPrint::SigCmdReply, 
            this, &SDKManager::onCmdReply);
    connect(m_protocol.get(), &ProtocolPrint::SigHandleFunOper1, 
            this, &SDKManager::onHandleRecvFunOper);
    
    // ✅ 添加这一行
    connect(m_protocol.get(), &ProtocolPrint::SigHandleFunOper2,
            this, &SDKManager::onHandleRecvDataOper);
    
    // ...
}
```

**状态**: ⚠️ 需要添加

---

## 📂 项目配置更新

### Visual Studio项目（.vcxproj）

在`motionControlSDK.vcxproj`中添加：

```xml
<ItemGroup>
  <ClCompile Include="..\..\src\sdk\SDKManager_Position.cpp" />
  <!-- SDKMotion.cpp 已存在，只需确保路径正确 -->
  <ClCompile Include="..\..\src\sdk\SDKMotion.cpp" />
</ItemGroup>
```

---

### QMake项目（.pro）

在`.pro`文件中添加：

```qmake
SOURCES += \
    src/sdk/SDKManager_Position.cpp \
    src/sdk/SDKMotion.cpp
```

---

## 🧪 编译和测试

### 步骤1：清理并编译

```bash
# Visual Studio
1. 右键项目 → 清理
2. 右键项目 → 重新生成

# QMake
nmake clean
qmake
nmake
```

**预期结果**: ✅ 编译成功，无错误

---

### 步骤2：基本功能测试

```cpp
// 测试代码
motionControlSDK sdk;
sdk.initialize();
sdk.connectToDevice("192.168.100.57", 5555);

// 测试1：设置目标位置
MoveAxisPos target = MoveAxisPos::fromMillimeters(100, 200, 50);
sdk.setTargetPosition(target);

// 测试2：读取目标位置
MoveAxisPos readTarget = sdk.getTargetPosition();
assert(readTarget.xPos == 100000);  // 100mm = 100000μm

// 测试3：执行移动
bool ok = sdk.moveToPosition(target);
assert(ok == true);
```

**预期日志**:
```
[INFO] 设置目标位置: X=100.000mm, Y=200.000mm, Z=50.000mm
[INFO] X轴移动: distance=100.000mm, isAbsolute=true
[INFO] [X轴] 位置数据: 100000 μm (100.000 mm)
[INFO] [X轴移动] 命令已发送 ✓
```

---

### 步骤3：UI集成测试

创建简单的测试UI：

```cpp
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "motionControlSDK.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);
    
    motionControlSDK sdk;
    sdk.initialize();
    sdk.connectToDevice("192.168.100.57", 5555);
    
    QPushButton *btn1 = new QPushButton("移动到(100,200,50)");
    QObject::connect(btn1, &QPushButton::clicked, [&]() {
        sdk.moveToPosition(MoveAxisPos::fromMillimeters(100, 200, 50));
    });
    layout->addWidget(btn1);
    
    QPushButton *btn2 = new QPushButton("移动到(150,250,75)");
    QObject::connect(btn2, &QPushButton::clicked, [&]() {
        sdk.moveToPosition(MoveAxisPos::fromMillimeters(150, 250, 75));
    });
    layout->addWidget(btn2);
    
    window.show();
    return app.exec();
}
```

**预期行为**: 
- ✅ 点击按钮后，设备开始移动
- ✅ 日志输出移动信息
- ✅ 无崩溃无错误

---

## ✅ 最终验收清单

完成所有修改后，请检查：

### 代码修改
- [ ] `motionControlSDK.h` 已添加`MoveAxisPos`结构体和新API
- [ ] `motionControlSDK.cpp` 已实现新API并注册元类型
- [ ] `SDKManager.h` 已添加位置管理方法声明
- [ ] `SDKManager_Position.cpp` 已创建并实现
- [ ] `SDKMotion.cpp` 已替换为新实现
- [ ] `SDKManager.cpp` 已添加信号连接
- [ ] `ProtocolPrint.cpp` 已发射`SigHandleFunOper2`信号

### 项目配置
- [ ] VS项目或QMake项目已更新
- [ ] 所有新文件已添加到编译列表

### 编译测试
- [ ] 编译无错误
- [ ] 编译无警告
- [ ] 元类型注册成功
- [ ] 信号槽连接成功

### 功能测试
- [ ] 可以设置目标位置
- [ ] 可以读取目标位置
- [ ] 可以执行移动命令
- [ ] 日志输出正确（显示微米和毫米）
- [ ] 位置数据转换正确（1mm = 1000μm）
- [ ] UI集成正常

### 精度验证
- [ ] 微米级精度保持（无舍入误差）
- [ ] 单位转换正确
- [ ] 数据传输正确

---

## 📊 时间估算

| 任务 | 预计时间 | 难度 |
|------|---------|------|
| 修改`motionControlSDK.h` | 10分钟 | ⭐ |
| 实现`motionControlSDK.cpp`新API | 20分钟 | ⭐⭐ |
| 添加`SDKManager_Position.cpp` | 5分钟 | ⭐（已有文件） |
| 替换`SDKMotion.cpp` | 10分钟 | ⭐（已有文件） |
| 更新项目配置 | 5分钟 | ⭐ |
| 编译调试 | 30分钟 | ⭐⭐ |
| 功能测试 | 40分钟 | ⭐⭐⭐ |
| **总计** | **2小时** | |

---

## 🎯 核心要点

### 单位系统

| 层级 | 单位 | 说明 |
|------|------|------|
| 结构体存储 | μm | `MoveAxisPos`内部使用微米 |
| 协议传输 | μm | `QByteArray`包含微米值 |
| API参数 | μm | `MoveAxisPos`参数 |
| UI显示 | mm | 通过`toMillimeters()`转换 |
| 日志输出 | μm+mm | 同时显示两种单位 |

### 转换公式

```cpp
// mm → μm
quint32 um = static_cast<quint32>(mm * 1000.0);

// μm → mm
double mm = static_cast<double>(um) / 1000.0;
```

---

## 📞 技术支持

如遇问题，请参考：
1. [MoveAxisPos集成实施方案.md](./MoveAxisPos集成实施方案.md) - 详细技术方案
2. [MoveAxisPos_API使用手册.md](./MoveAxisPos_API使用手册.md) - API使用文档
3. 本清单的"常见问题"章节

---

**祝集成顺利！** 🚀

