# MoveAxisPos API使用手册

**版本**: 1.0  
**日期**: 2025-12-23  
**单位**: 所有位置数据以**微米(μm)**为单位

---

## 📖 目录

1. [快速开始](#快速开始)
2. [MoveAxisPos结构体](#moveaxispos结构体)
3. [API接口说明](#api接口说明)
4. [完整示例代码](#完整示例代码)
5. [常见问题](#常见问题)

---

## 🚀 快速开始

### 最简单的例子（30秒上手）

```cpp
#include "motionControlSDK.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 创建SDK实例
    motionControlSDK sdk;
    sdk.initialize();
    sdk.connectToDevice("192.168.100.57", 5555);
    
    // 移动到坐标（100mm, 200mm, 50mm）
    MoveAxisPos pos = MoveAxisPos::fromMillimeters(100, 200, 50);
    sdk.moveToPosition(pos);
    
    return app.exec();
}
```

**就这么简单！** 🎉

---

## 📐 MoveAxisPos结构体

### 定义

```cpp
struct MoveAxisPos
{
    quint32 xPos;  ///< X轴坐标（微米）
    quint32 yPos;  ///< Y轴坐标（微米）
    quint32 zPos;  ///< Z轴坐标（微米）
    
    // 构造函数
    MoveAxisPos();
    MoveAxisPos(quint32 x, quint32 y, quint32 z);
    
    // 静态工厂方法
    static MoveAxisPos fromMillimeters(double x_mm, double y_mm, double z_mm);
    
    // 转换方法
    void toMillimeters(double& x_out, double& y_out, double& z_out) const;
};
```

### 单位说明

| 单位 | 符号 | 换算 | 精度 | 范围 |
|------|------|------|------|------|
| 微米 | μm | 1mm = 1000μm | 1μm | 0 ~ 4,294,967 μm |
| 毫米 | mm | 1mm = 1000μm | 0.001mm | 0 ~ 4,294 mm |

**为什么使用微米？**
- ✅ 精度高：1μm = 0.001mm
- ✅ 无浮点误差：使用整数运算
- ✅ 符合工业标准：精密设备常用微米

---

### 创建MoveAxisPos的方法

#### 方法1：直接使用微米（推荐用于精密控制）

```cpp
// 100.123mm = 100123μm
MoveAxisPos pos(100123, 200456, 50789);
```

**优点**: 最精确，无舍入误差

---

#### 方法2：从毫米转换（推荐用于UI）

```cpp
// 从毫米转换
MoveAxisPos pos = MoveAxisPos::fromMillimeters(100.123, 200.456, 50.789);

// 结果：pos.xPos = 100123μm, pos.yPos = 200456μm, pos.zPos = 50789μm
```

**优点**: 方便，符合人类习惯

---

#### 方法3：默认构造（原点）

```cpp
MoveAxisPos pos;  // (0, 0, 0)
```

---

### 读取MoveAxisPos的值

#### 读取微米值

```cpp
MoveAxisPos pos(100000, 200000, 50000);

quint32 x_um = pos.xPos;  // 100000μm
quint32 y_um = pos.yPos;  // 200000μm
quint32 z_um = pos.zPos;  // 50000μm

qDebug() << "X:" << x_um << "μm";
```

---

#### 转换为毫米

```cpp
MoveAxisPos pos(100000, 200000, 50000);

double x_mm, y_mm, z_mm;
pos.toMillimeters(x_mm, y_mm, z_mm);

// x_mm = 100.0, y_mm = 200.0, z_mm = 50.0

qDebug() << "X:" << x_mm << "mm";
```

---

## 🔌 API接口说明

### motionControlSDK类提供的接口

| 方法 | 功能 | 参数 | 返回值 |
|------|------|------|--------|
| `moveToPosition()` | 移动到指定位置 | `MoveAxisPos` | `bool` |
| `setTargetPosition()` | 设置目标位置 | `MoveAxisPos` | `bool` |
| `getTargetPosition()` | 获取目标位置 | 无 | `MoveAxisPos` |
| `getCurrentPosition()` | 获取当前位置 | 无 | `MoveAxisPos` |

---

### 1. moveToPosition()

**功能**: 移动到指定的绝对位置

**原型**:
```cpp
bool moveToPosition(const MoveAxisPos& targetPos);
```

**参数**:
- `targetPos`: 目标位置（微米单位）

**返回值**:
- `true`: 命令发送成功
- `false`: 失败（设备未连接或SDK未初始化）

**示例**:
```cpp
// 移动到 (100mm, 200mm, 50mm)
MoveAxisPos target = MoveAxisPos::fromMillimeters(100, 200, 50);
if (sdk.moveToPosition(target)) {
    qDebug() << "移动命令发送成功";
} else {
    qDebug() << "移动失败";
}
```

**注意事项**:
- ⚠️ 会立即发送移动命令到设备
- ⚠️ 移动结果通过信号`positionUpdated()`异步通知

---

### 2. setTargetPosition()

**功能**: 仅设置目标位置，不立即执行移动

**原型**:
```cpp
bool setTargetPosition(const MoveAxisPos& targetPos);
```

**用途**:
1. 预先设置目标位置
2. 后续可以通过其他命令执行移动
3. 用于分步操作

**示例**:
```cpp
// 设置目标位置
MoveAxisPos target = MoveAxisPos::fromMillimeters(100, 200, 50);
sdk.setTargetPosition(target);

// 稍后执行移动
sdk.moveToPosition(sdk.getTargetPosition());
```

---

### 3. getTargetPosition()

**功能**: 获取当前设置的目标位置

**原型**:
```cpp
MoveAxisPos getTargetPosition() const;
```

**示例**:
```cpp
MoveAxisPos target = sdk.getTargetPosition();

double x, y, z;
target.toMillimeters(x, y, z);

qDebug() << "目标位置:" << x << y << z << "mm";
```

---

### 4. getCurrentPosition()

**功能**: 获取设备当前的实际位置

**原型**:
```cpp
MoveAxisPos getCurrentPosition() const;
```

**示例**:
```cpp
MoveAxisPos current = sdk.getCurrentPosition();

qDebug() << "当前位置:"
         << current.xPos << "μm"
         << current.yPos << "μm"
         << current.zPos << "μm";
```

**注意**: 需要设备支持位置查询功能

---

## 📝 完整示例代码

### 示例1：基本移动操作

```cpp
#include <QApplication>
#include <QDebug>
#include "motionControlSDK.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 创建SDK实例
    motionControlSDK sdk;
    
    // 初始化
    if (!sdk.initialize()) {
        qDebug() << "SDK初始化失败";
        return -1;
    }
    
    // 连接设备
    if (!sdk.connectToDevice("192.168.100.57", 5555)) {
        qDebug() << "连接设备失败";
        return -1;
    }
    
    // 等待连接
    QEventLoop loop;
    QObject::connect(&sdk, &motionControlSDK::connected, [&]() {
        qDebug() << "设备已连接";
        
        // 移动到第一个位置（100mm, 200mm, 50mm）
        MoveAxisPos pos1 = MoveAxisPos::fromMillimeters(100, 200, 50);
        sdk.moveToPosition(pos1);
        
        // 5秒后移动到第二个位置
        QTimer::singleShot(5000, [&]() {
            MoveAxisPos pos2 = MoveAxisPos::fromMillimeters(150, 250, 75);
            sdk.moveToPosition(pos2);
        });
        
        // 10秒后退出
        QTimer::singleShot(10000, [&]() {
            loop.quit();
        });
    });
    
    loop.exec();
    
    // 断开连接
    sdk.disconnectFromDevice();
    
    return 0;
}
```

---

### 示例2：UI集成（带进度显示）

```cpp
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "motionControlSDK.h"

class PositionControlWidget : public QWidget
{
    Q_OBJECT
public:
    PositionControlWidget(QWidget *parent = nullptr) 
        : QWidget(parent)
    {
        setupUI();
        setupSDK();
    }
    
private:
    void setupUI()
    {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        
        // 目标位置输入
        QHBoxLayout *targetLayout = new QHBoxLayout;
        targetLayout->addWidget(new QLabel("目标位置 (mm):"));
        
        xSpinBox = new QDoubleSpinBox;
        xSpinBox->setRange(0, 4000);
        xSpinBox->setDecimals(3);
        xSpinBox->setValue(100.0);
        targetLayout->addWidget(new QLabel("X:"));
        targetLayout->addWidget(xSpinBox);
        
        ySpinBox = new QDoubleSpinBox;
        ySpinBox->setRange(0, 4000);
        ySpinBox->setDecimals(3);
        ySpinBox->setValue(200.0);
        targetLayout->addWidget(new QLabel("Y:"));
        targetLayout->addWidget(ySpinBox);
        
        zSpinBox = new QDoubleSpinBox;
        zSpinBox->setRange(0, 4000);
        zSpinBox->setDecimals(3);
        zSpinBox->setValue(50.0);
        targetLayout->addWidget(new QLabel("Z:"));
        targetLayout->addWidget(zSpinBox);
        
        mainLayout->addLayout(targetLayout);
        
        // 移动按钮
        QPushButton *moveBtn = new QPushButton("移动到目标位置");
        connect(moveBtn, &QPushButton::clicked, this, &PositionControlWidget::onMoveClicked);
        mainLayout->addWidget(moveBtn);
        
        // 当前位置显示
        currentPosLabel = new QLabel("当前位置: 等待连接...");
        mainLayout->addWidget(currentPosLabel);
        
        // 状态显示
        statusLabel = new QLabel("状态: 就绪");
        mainLayout->addWidget(statusLabel);
    }
    
    void setupSDK()
    {
        sdk = new motionControlSDK(this);
        
        // 连接信号
        connect(sdk, &motionControlSDK::connected, this, [this]() {
            statusLabel->setText("状态: 已连接 ✓");
        });
        
        connect(sdk, &motionControlSDK::disconnected, this, [this]() {
            statusLabel->setText("状态: 已断开");
        });
        
        connect(sdk, &motionControlSDK::positionUpdated, 
                this, &PositionControlWidget::onPositionUpdated);
        
        connect(sdk, &motionControlSDK::errorOccurred, 
                this, [this](int code, const QString& msg) {
            statusLabel->setText(QString("错误 [%1]: %2").arg(code).arg(msg));
        });
        
        // 初始化并连接
        sdk->initialize();
        sdk->connectToDevice("192.168.100.57", 5555);
    }
    
private slots:
    void onMoveClicked()
    {
        // 从UI读取目标位置（毫米）
        double x_mm = xSpinBox->value();
        double y_mm = ySpinBox->value();
        double z_mm = zSpinBox->value();
        
        // 转换为MoveAxisPos
        MoveAxisPos target = MoveAxisPos::fromMillimeters(x_mm, y_mm, z_mm);
        
        // 显示微米值（可选）
        statusLabel->setText(QString("移动到: (%1μm, %2μm, %3μm)")
            .arg(target.xPos)
            .arg(target.yPos)
            .arg(target.zPos));
        
        // 执行移动
        if (sdk->moveToPosition(target)) {
            statusLabel->setText("移动命令已发送...");
        } else {
            statusLabel->setText("移动失败！");
        }
    }
    
    void onPositionUpdated(double x, double y, double z)
    {
        // 更新当前位置显示（毫米）
        currentPosLabel->setText(
            QString("当前位置: X=%1mm, Y=%2mm, Z=%3mm")
                .arg(x, 0, 'f', 3)
                .arg(y, 0, 'f', 3)
                .arg(z, 0, 'f', 3));
        
        // 也可以获取微米值
        MoveAxisPos current = sdk->getCurrentPosition();
        qDebug() << "当前位置(μm):" << current.xPos << current.yPos << current.zPos;
        
        statusLabel->setText("移动完成 ✓");
    }
    
private:
    motionControlSDK *sdk;
    QDoubleSpinBox *xSpinBox;
    QDoubleSpinBox *ySpinBox;
    QDoubleSpinBox *zSpinBox;
    QLabel *currentPosLabel;
    QLabel *statusLabel;
};

#include "main.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    PositionControlWidget widget;
    widget.show();
    
    return app.exec();
}
```

---

### 示例3：精密移动（微米级）

```cpp
#include <QDebug>
#include "motionControlSDK.h"

void preciseMoveExample()
{
    motionControlSDK sdk;
    sdk.initialize();
    sdk.connectToDevice("192.168.100.57", 5555);
    
    // 场景：需要移动到非常精确的位置
    // 例如：X=100.123mm, Y=200.456mm, Z=50.789mm
    
    // 方法1：使用fromMillimeters（会有微小的浮点转换误差）
    MoveAxisPos pos1 = MoveAxisPos::fromMillimeters(100.123, 200.456, 50.789);
    qDebug() << "方法1结果:" << pos1.xPos << pos1.yPos << pos1.zPos;
    // 输出: 100123 200456 50789
    
    // 方法2：直接使用微米值（最精确）
    MoveAxisPos pos2(100123, 200456, 50789);
    qDebug() << "方法2结果:" << pos2.xPos << pos2.yPos << pos2.zPos;
    // 输出: 100123 200456 50789
    
    // 执行移动
    sdk.moveToPosition(pos2);
    
    // 验证目标位置
    MoveAxisPos target = sdk.getTargetPosition();
    double x, y, z;
    target.toMillimeters(x, y, z);
    qDebug() << "验证: X=" << x << "mm"
             << "Y=" << y << "mm"
             << "Z=" << z << "mm";
    // 输出: X=100.123mm Y=200.456mm Z=50.789mm
}
```

---

### 示例4：相对移动

```cpp
void relativeMoveExample()
{
    motionControlSDK sdk;
    sdk.initialize();
    sdk.connectToDevice("192.168.100.57", 5555);
    
    // 获取当前位置
    MoveAxisPos current = sdk.getCurrentPosition();
    qDebug() << "当前位置:" << current.xPos << current.yPos << current.zPos;
    
    // 计算新位置（相对移动+10mm, +20mm, +5mm）
    MoveAxisPos newPos;
    newPos.xPos = current.xPos + 10000;  // +10mm = +10000μm
    newPos.yPos = current.yPos + 20000;  // +20mm = +20000μm
    newPos.zPos = current.zPos + 5000;   // +5mm = +5000μm
    
    // 执行移动
    sdk.moveToPosition(newPos);
    
    // 或者使用SDK的相对移动接口
    sdk.moveBy(10, 20, 5);  // 参数单位是mm
}
```

---

## ❓ 常见问题

### Q1: 为什么使用微米而不是毫米？

**A**: 
1. **精度**: 微米提供0.001mm的精度，适合精密设备
2. **整数运算**: 避免浮点数的舍入误差
3. **工业标准**: 精密机械行业常用微米

---

### Q2: 如何从UI的毫米值转换？

**A**: 使用`fromMillimeters()`方法：

```cpp
double x_mm = ui->xSpinBox->value();  // 从UI读取（毫米）
double y_mm = ui->ySpinBox->value();
double z_mm = ui->zSpinBox->value();

MoveAxisPos pos = MoveAxisPos::fromMillimeters(x_mm, y_mm, z_mm);
sdk.moveToPosition(pos);
```

---

### Q3: 如何显示当前位置到UI？

**A**: 连接`positionUpdated()`信号：

```cpp
connect(&sdk, &motionControlSDK::positionUpdated,
        [this](double x, double y, double z) {
    // x, y, z已经是毫米单位
    ui->xLabel->setText(QString("%1 mm").arg(x, 0, 'f', 3));
    ui->yLabel->setText(QString("%1 mm").arg(y, 0, 'f', 3));
    ui->zLabel->setText(QString("%1 mm").arg(z, 0, 'f', 3));
});
```

---

### Q4: 坐标范围是多少？

**A**: 
- **微米**: 0 ~ 4,294,967,295 μm（`quint32`最大值）
- **毫米**: 0 ~ 4,294.967 mm（约4.3米）

---

### Q5: 如何处理负数坐标？

**A**: 当前版本使用`quint32`（无符号整数），不支持负数。如需支持负数：

1. 使用偏移量：将所有坐标加上一个基准值
2. 修改结构体使用`qint32`（需要修改SDK）

---

### Q6: 移动精度是多少？

**A**: 
- **数据精度**: 1微米 = 0.001毫米
- **实际精度**: 取决于设备硬件能力

---

### Q7: 如何判断是否到达目标位置？

**A**: 监听`positionUpdated()`信号并比较：

```cpp
MoveAxisPos target = sdk.getTargetPosition();
MoveAxisPos current = sdk.getCurrentPosition();

// 计算距离（微米）
int dx = abs(static_cast<int>(target.xPos) - static_cast<int>(current.xPos));
int dy = abs(static_cast<int>(target.yPos) - static_cast<int>(current.yPos));
int dz = abs(static_cast<int>(target.zPos) - static_cast<int>(current.zPos));

// 允许1mm误差
if (dx < 1000 && dy < 1000 && dz < 1000) {
    qDebug() << "已到达目标位置";
}
```

---

## 📚 相关文档

- [MoveAxisPos集成实施方案.md](./MoveAxisPos集成实施方案.md) - 技术实现细节
- [PackParam信号槽传输实施方案.md](./PackParam信号槽传输实施方案.md) - 协议解析
- [SDK代码审核报告.md](../../PrintDeviceSDK/SDK代码审核报告.md) - 代码审核

---

## 📞 技术支持

如有问题，请参考：
1. 本文档的常见问题章节
2. 详细的实施方案文档
3. 示例代码

**祝使用愉快！** 🚀

