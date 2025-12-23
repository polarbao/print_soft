# MoveAxisPos结构体集成实施方案

**目标**: 将`MoveAxisPos`结构体集成到SDK公开接口，以微米为单位进行坐标传输  
**实施日期**: 2025-12-23  
**单位**: 所有位置数据以**微米(μm)**为单位

---

## 📋 需求分析

### 核心需求

1. ✅ 将`MoveAxisPos`结构体移到`motionControlSDK.h`公开接口中
2. ✅ 所有成员变量使用微米(μm)作为单位（1mm = 1000μm）
3. ✅ 提供设置目标位置的API接口
4. ✅ 在`SDKManager`中管理目标位置
5. ✅ 在`SDKMotion.cpp`中将`MoveAxisPos`转换为`QByteArray`协议数据
6. ✅ 支持从Qt项目中调用API设置移动坐标

### 数据流程

```
Qt应用 
  → motionControlSDK::moveToPosition(MoveAxisPos)  [微米单位]
  → SDKManager::setTargetPosition()  [微米单位]
  → SDKMotion::moveXAxis/moveYAxis/moveZAxis()
  → 转换为QByteArray协议数据  [转换单位]
  → 通过TCP发送到设备
```

---

## 🔧 实施步骤

### 步骤1：修改motionControlSDK.h

在`motionControlSDK.h`中，在`SdkEventCallback`定义之后添加：

```cpp
/**
 * @brief �ص�����ָ������
 */
typedef void(*SdkEventCallback)(const SdkEvent* event);

// --- ���ӿ����� ---

/**
 * @brief �����λ�ýṹ��
 * @details ��λ��΢�ף�μm��
 * 
 * ʹ��˵����
 * - ������Ա����λ��΢�ף�1mm = 1000΢��
 * - ֧�ָ���λ�ú�����λ���˶�
 * - ����Χ��0 ~ 4294967295΢�ף�Լ4294mm��
 */
struct MOTIONCONTROLSDK_EXPORT MoveAxisPos
{
	quint32 xPos;  ///< X������λ����λ��΢�ף�
	quint32 yPos;  ///< Y������λ����λ��΢�ף�
	quint32 zPos;  ///< Z������λ����λ��΢�ף�

	/**
	 * @brief Ĭ�Ϲ��캯����λ����Ϊ0
	 */
	MoveAxisPos() : xPos(0), yPos(0), zPos(0) {}

	/**
	 * @brief ���캯��
	 * @param x X������λ����λ��΢�ף�
	 * @param y Y������λ����λ��΢�ף�
	 * @param z Z������λ����λ��΢�ף�
	 */
	MoveAxisPos(quint32 x, quint32 y, quint32 z) 
		: xPos(x), yPos(y), zPos(z) {}

	/**
	 * @brief �������캯����λ����λ��mm��
	 * @param x_mm X������λ����λ��mm��
	 * @param y_mm Y������λ����λ��mm��
	 * @param z_mm Z������λ����λ��mm��
	 * @return MoveAxisPos����������΢��Ϊ��λ��
	 */
	static MoveAxisPos fromMillimeters(double x_mm, double y_mm, double z_mm)
	{
		return MoveAxisPos(
			static_cast<quint32>(x_mm * 1000.0),
			static_cast<quint32>(y_mm * 1000.0),
			static_cast<quint32>(z_mm * 1000.0)
		);
	}

	/**
	 * @brief ת��Ϊ���ף�mm��
	 * @param x_out �����X������λ����λ��mm��
	 * @param y_out �����Y������λ����λ��mm��
	 * @param z_out �����Z������λ����λ��mm��
	 */
	void toMillimeters(double& x_out, double& y_out, double& z_out) const
	{
		x_out = static_cast<double>(xPos) / 1000.0;
		y_out = static_cast<double>(yPos) / 1000.0;
		z_out = static_cast<double>(zPos) / 1000.0;
	}
};

// ע��Ϊ Qt Ԫ���ͣ���֧���ź�۲�����
Q_DECLARE_METATYPE(MoveAxisPos)
```

**关键点**：
- ✅ 使用`quint32`类型，范围0~4294967295微米（约4.3米）
- ✅ 提供`fromMillimeters()`静态方法，方便从毫米转换
- ✅ 提供`toMillimeters()`方法，方便转换为毫米
- ✅ 使用`Q_DECLARE_METATYPE`注册为Qt元类型

---

### 步骤2：添加API接口到motionControlSDK类

在`motionControlSDK`类的公开接口部分，`moveTo()`函数之后添加：

```cpp
public:
	// ==================== �˶����� ====================

	/**
	 * @brief �ƶ�����������
	 * @param x X�����꣨mm��
	 * @param y Y�����꣨mm��
	 * @param z Z�����꣨mm��
	 * @param speed �ٶȣ�mm/s��Ĭ��100��
	 * @return true=����ͳɹ�, false=ʧ��
	 */
	bool moveTo(double x, double y, double z, double speed = 100.0);

	/**
	 * @brief �ƶ����������ṹ��汾��
	 * @param targetPos Ŀ��λ�ã���λ��΢�ף�
	 * @return true=����ͳɹ�, false=ʧ��
	 * 
	 * ʹ��ʾ����
	 * @code
	 * // ���ɣ�ʹ��΢��
	 * MoveAxisPos pos(100000, 200000, 50000);  // X=100mm, Y=200mm, Z=50mm
	 * sdk.moveToPosition(pos);
	 * 
	 * // ���ɶ���ʹ�ô����콨��
	 * sdk.moveToPosition(MoveAxisPos::fromMillimeters(100, 200, 50));
	 * @endcode
	 */
	bool moveToPosition(const MoveAxisPos& targetPos);

	/**
	 * @brief ���õ�ǰ��Ŀ��λ�ã���δִ���˶���
	 * @param targetPos Ŀ��λ�ã���λ��΢�ף�
	 * @return true=���óɹ�, false=ʧ��
	 * @note �˽ӿ�ֻ����Ŀ��λ�ã�������ִ���˶���Ҫ���ù���ִ������
	 */
	bool setTargetPosition(const MoveAxisPos& targetPos);

	/**
	 * @brief ��ȡ��ǰ��Ŀ��λ��
	 * @return Ŀ��λ�ã���λ��΢�ף�
	 */
	MoveAxisPos getTargetPosition() const;

	/**
	 * @brief ��ȡ��ǰ��ʵ��λ�ã�����豸��ѯ��
	 * @return ��ǰλ�ã���λ��΢�ף�
	 * @note ��Ҫ�豸֧�ֲ�ѯλ�ù���
	 */
	MoveAxisPos getCurrentPosition() const;
```

**新增API说明**：
1. `moveToPosition()`: 直接使用`MoveAxisPos`移动到目标位置
2. `setTargetPosition()`: 仅设置目标位置，不立即执行
3. `getTargetPosition()`: 获取当前的目标位置
4. `getCurrentPosition()`: 获取设备当前实际位置

---

### 步骤3：修改motionControlSDK.cpp实现

在`motionControlSDK.cpp`中添加实现：

```cpp
#include "motionControlSDK.h"
#include "SDKManager.h"
#include <QDebug>

// 在构造函数中注册MoveAxisPos元类型
motionControlSDK::motionControlSDK(QObject *parent)
	: QObject(parent)
	, d(new Private(this))
{
	QMutexLocker locker(&Private::s_mutex);
	Private::s_instance = this;
	
	// ✅ 注册MoveAxisPos为Qt元类型
	qRegisterMetaType<MoveAxisPos>("MoveAxisPos");
}

// 新增API实现

bool motionControlSDK::moveToPosition(const MoveAxisPos& targetPos)
{
	if (!isConnected()) {
		emit errorOccurred(-1, tr("设备未连接"));
		return false;
	}
	
	// 设置目标位置
	if (!setTargetPosition(targetPos)) {
		return false;
	}
	
	// 执行移动（转换为毫米）
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
	
	// 调用SDKManager设置目标位置
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

---

### 步骤4：修改SDKManager.h

在`SDKManager.h`中添加：

```cpp
// 在SDKManager类的公开部分添加
public:
	/**
	 * @brief 设置目标位置（微米单位）
	 * @param targetPos 目标位置
	 */
	void setTargetPosition(const MoveAxisPos& targetPos);
	
	/**
	 * @brief 获取目标位置（微米单位）
	 * @return 目标位置
	 */
	MoveAxisPos getTargetPosition() const;
	
	/**
	 * @brief 获取当前位置（微米单位）
	 * @return 当前位置
	 */
	MoveAxisPos getCurrentPosition() const;

// 在私有槽函数中添加（已存在）
private slots:
	/**
	 * @brief 处理功能操作指令
	 */
	void onHandleRecvFunOper(const PackParam& arr);

	/**
	 * @brief 处理接收到的位置数据
	 * @param code 命令码
	 * @param pos 位置数据（微米单位）
	 */
	void onHandleRecvDataOper(int code, const MoveAxisPos& pos);
```

---

### 步骤5：实现SDKManager位置管理

创建新文件`SDKManager_Position.cpp`：

```cpp
/**
 * @file SDKManager_Position.cpp
 * @brief 位置管理实现
 * @details 管理目标位置和当前位置（单位：微米）
 */

#include "SDKManager.h"
#include "comm/CLogManager.h"
#include <QMutexLocker>
#include <QString>

// ==================== 位置管理 ====================

void SDKManager::setTargetPosition(const MoveAxisPos& targetPos)
{
	// 保存目标位置
	m_dstAxisData = targetPos;
	
	double x_mm, y_mm, z_mm;
	targetPos.toMillimeters(x_mm, y_mm, z_mm);
	
	LOG_INFO(QString(u8"设置目标位置: X=%1mm(%2μm), Y=%3mm(%4μm), Z=%5mm(%6μm)")
		.arg(x_mm, 0, 'f', 3).arg(targetPos.xPos)
		.arg(y_mm, 0, 'f', 3).arg(targetPos.yPos)
		.arg(z_mm, 0, 'f', 3).arg(targetPos.zPos));
}

MoveAxisPos SDKManager::getTargetPosition() const
{
	return m_dstAxisData;
}

MoveAxisPos SDKManager::getCurrentPosition() const
{
	return m_curAxisData;
}

// ==================== 位置数据接收处理 ====================

/**
 * @brief 处理接收到的位置数据（槽函数）
 * @param code 命令码
 * @param pos 位置数据（微米单位，从协议解析得到）
 */
void SDKManager::onHandleRecvDataOper(int code, const MoveAxisPos& pos)
{
	LOG_INFO(QString(u8"收到位置数据: 命令码=0x%1")
		.arg(QString::number(code, 16).toUpper()));
	
	// 更新当前位置
	m_curAxisData = pos;
	
	// 转换为毫米用于日志
	double x_mm, y_mm, z_mm;
	pos.toMillimeters(x_mm, y_mm, z_mm);
	
	LOG_INFO(QString(u8"当前位置: X=%1mm(%2μm), Y=%3mm(%4μm), Z=%5mm(%6μm)")
		.arg(x_mm, 0, 'f', 3).arg(pos.xPos)
		.arg(y_mm, 0, 'f', 3).arg(pos.yPos)
		.arg(z_mm, 0, 'f', 3).arg(pos.zPos));
	
	// 发送位置更新事件到上层
	sendEvent(EVENT_TYPE_MOVE_STATUS, 0, "Position updated", 
		x_mm, y_mm, z_mm);
}
```

---

### 步骤6：修改SDKMotion.cpp

修改`SDKMotion.cpp`以使用`MoveAxisPos`并转换为协议数据：

```cpp
/**
 * @file SDKMotion.cpp
 * @brief 运动控制实现（使用微米单位）
 * @details 处理X/Y/Z轴的移动和复位功能，使用MoveAxisPos结构体
 */

#include "SDKManager.h"
#include "protocol/ProtocolPrint.h"
#include "comm/CLogManager.h"
#include <QByteArray>
#include <QDataStream>

// ==================== 辅助函数 ====================

/**
 * @brief 将MoveAxisPos转换为QByteArray协议数据
 * @param pos 位置数据（微米单位）
 * @param axis 轴标识（'X', 'Y', 'Z'）
 * @return 协议数据包
 * 
 * 协议格式（假设）：
 * - 每个轴4字节（大端序）
 * - 单位转换：根据设备要求可能需要转换
 */
static QByteArray positionToByteArray(const MoveAxisPos& pos, char axis)
{
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::BigEndian);
	
	switch (axis)
	{
	case 'X':
		stream << pos.xPos;  // 微米单位
		LOG_INFO(QString(u8"X轴位置数据: %1μm (%2mm)")
			.arg(pos.xPos)
			.arg(static_cast<double>(pos.xPos) / 1000.0, 0, 'f', 3));
		break;
		
	case 'Y':
		stream << pos.yPos;  // 微米单位
		LOG_INFO(QString(u8"Y轴位置数据: %1μm (%2mm)")
			.arg(pos.yPos)
			.arg(static_cast<double>(pos.yPos) / 1000.0, 0, 'f', 3));
		break;
		
	case 'Z':
		stream << pos.zPos;  // 微米单位
		LOG_INFO(QString(u8"Z轴位置数据: %1μm (%2mm)")
			.arg(pos.zPos)
			.arg(static_cast<double>(pos.zPos) / 1000.0, 0, 'f', 3));
		break;
		
	default:
		LOG_INFO(QString(u8"未知轴标识: %1").arg(axis));
		break;
	}
	
	return data;
}

/**
 * @brief 将完整MoveAxisPos转换为QByteArray
 * @param pos 位置数据（微米单位）
 * @return 协议数据包（12字节：X(4)+Y(4)+Z(4)）
 */
static QByteArray fullPositionToByteArray(const MoveAxisPos& pos)
{
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::BigEndian);
	
	// 写入X/Y/Z坐标（各4字节，大端序）
	stream << pos.xPos;  // 微米
	stream << pos.yPos;  // 微米
	stream << pos.zPos;  // 微米
	
	double x_mm, y_mm, z_mm;
	pos.toMillimeters(x_mm, y_mm, z_mm);
	
	LOG_INFO(QString(u8"完整位置数据: X=%1mm, Y=%2mm, Z=%3mm (12字节)")
		.arg(x_mm, 0, 'f', 3)
		.arg(y_mm, 0, 'f', 3)
		.arg(z_mm, 0, 'f', 3));
	
	// 打印十六进制
	QString hex = data.toHex(' ').toUpper();
	LOG_INFO(QString(u8"协议数据 (Hex): %1").arg(hex));
	
	return data;
}

// ==================== 运动控制实现 ====================

int SDKManager::moveXAxis(double distance, bool isAbsolute) 
{
	if (!isConnected()) {
		return -1;
	}
	
	LOG_INFO(QString(u8"X轴移动: distance=%1mm, isAbsolute=%2")
		.arg(distance, 0, 'f', 3)
		.arg(isAbsolute ? "true" : "false"));
	
	MoveAxisPos targetPos;
	
	if (isAbsolute) {
		// 绝对移动：使用目标位置
		targetPos = m_dstAxisData;
	} else {
		// 相对移动：在当前位置基础上偏移
		targetPos = m_curAxisData;
		// 将距离（mm）转换为微米并添加
		quint32 offset_um = static_cast<quint32>(abs(distance) * 1000.0);
		if (distance > 0) {
			targetPos.xPos += offset_um;
		} else {
			targetPos.xPos = (targetPos.xPos > offset_um) ? 
				(targetPos.xPos - offset_um) : 0;
		}
	}
	
	// 转换为协议数据
	QByteArray data = positionToByteArray(targetPos, 'X');
	
	// 发送命令
	ProtocolPrint::FunCode cmd = (distance > 0) ? 
		ProtocolPrint::Ctrl_XAxisRMove : ProtocolPrint::Ctrl_XAxisLMove;
	
	sendCommand(cmd, data);
	
	return 0;
}

int SDKManager::moveYAxis(double distance, bool isAbsolute) 
{
	if (!isConnected()) {
		return -1;
	}
	
	LOG_INFO(QString(u8"Y轴移动: distance=%1mm, isAbsolute=%2")
		.arg(distance, 0, 'f', 3)
		.arg(isAbsolute ? "true" : "false"));
	
	MoveAxisPos targetPos;
	
	if (isAbsolute) {
		targetPos = m_dstAxisData;
	} else {
		targetPos = m_curAxisData;
		quint32 offset_um = static_cast<quint32>(abs(distance) * 1000.0);
		if (distance > 0) {
			targetPos.yPos += offset_um;
		} else {
			targetPos.yPos = (targetPos.yPos > offset_um) ? 
				(targetPos.yPos - offset_um) : 0;
		}
	}
	
	// 转换为协议数据
	QByteArray data = positionToByteArray(targetPos, 'Y');
	
	// 发送命令
	ProtocolPrint::FunCode cmd = (distance > 0) ? 
		ProtocolPrint::Ctrl_YAxisRMove : ProtocolPrint::Ctrl_YAxisLMove;
	
	sendCommand(cmd, data);
	
	return 0;
}

int SDKManager::moveZAxis(double distance, bool isAbsolute) 
{
	if (!isConnected()) {
		return -1;
	}
	
	LOG_INFO(QString(u8"Z轴移动: distance=%1mm, isAbsolute=%2")
		.arg(distance, 0, 'f', 3)
		.arg(isAbsolute ? "true" : "false"));
	
	MoveAxisPos targetPos;
	
	if (isAbsolute) {
		targetPos = m_dstAxisData;
	} else {
		targetPos = m_curAxisData;
		quint32 offset_um = static_cast<quint32>(abs(distance) * 1000.0);
		if (distance > 0) {
			targetPos.zPos += offset_um;
		} else {
			targetPos.zPos = (targetPos.zPos > offset_um) ? 
				(targetPos.zPos - offset_um) : 0;
		}
	}
	
	// 转换为协议数据
	QByteArray data = positionToByteArray(targetPos, 'Z');
	
	// 发送命令
	ProtocolPrint::FunCode cmd = (distance > 0) ? 
		ProtocolPrint::Ctrl_ZAxisRMove : ProtocolPrint::Ctrl_ZAxisLMove;
	
	sendCommand(cmd, data);
	
	return 0;
}

int SDKManager::resetAxis(int axisFlag) 
{
	if (!isConnected()) {
		return -1;
	}
	
	LOG_INFO(QString(u8"复位轴: axisFlag=0x%1").arg(axisFlag, 0, 16));
	
	// 复位后将目标位置设为0
	if (axisFlag & 1) {
		m_dstAxisData.xPos = 0;
		sendCommand(ProtocolPrint::Ctrl_ResetPos);
	}
	
	if (axisFlag & 2) {
		m_dstAxisData.yPos = 0;
		sendCommand(ProtocolPrint::Ctrl_ResetPos);
	}
	
	if (axisFlag & 4) {
		m_dstAxisData.zPos = 0;
		sendCommand(ProtocolPrint::Ctrl_ResetPos);
	}
	
	return 0;
}
```

---

### 步骤7：更新信号槽连接

在`SDKManager::init()`中添加新的信号槽连接：

```cpp
bool SDKManager::init(const QString& log_dir) 
{
	// ...现有初始化代码...
	
	// 连接协议处理器信号
	connect(m_protocol.get(), &ProtocolPrint::SigHeartBeat, 
			this, &SDKManager::onHeartbeat);
	connect(m_protocol.get(), &ProtocolPrint::SigCmdReply, 
			this, &SDKManager::onCmdReply);
	connect(m_protocol.get(), &ProtocolPrint::SigHandleFunOper1, 
			this, &SDKManager::onHandleRecvFunOper);
	
	// ✅ 新增：连接位置数据信号
	connect(m_protocol.get(), &ProtocolPrint::SigHandleFunOper2,
			this, &SDKManager::onHandleRecvDataOper);
	
	// ...
	
	m_initialized = true;
	return true;
}
```

---

## 📊 完整的数据流程

### 发送数据（Qt应用 → 设备）

```
1. Qt应用调用：
   sdk.moveToPosition(MoveAxisPos::fromMillimeters(100, 200, 50));
   ↓ [微米单位: 100000, 200000, 50000]

2. motionControlSDK::moveToPosition()
   ↓ 转换为mm：100.0, 200.0, 50.0

3. motionControlSDK::moveTo(100.0, 200.0, 50.0)
   ↓

4. SDKManager::moveXAxis(100.0, true)
   ↓ 获取目标位置(微米)：100000

5. positionToByteArray(pos, 'X')
   ↓ QByteArray: [00 01 86 A0]  (100000的大端序)

6. sendCommand(Ctrl_XAxisRMove, data)
   ↓ 通过TCP发送

7. 设备接收并执行
```

### 接收数据（设备 → Qt应用）

```
1. 设备发送位置数据
   ↓ TCP数据包

2. SDKManager::onRecvData()
   ↓

3. ProtocolPrint::ParseRespPackageData()
   ↓ 解析12字节位置数据
   ↓ 创建MoveAxisPos(xPos, yPos, zPos)  [微米单位]

4. emit SigHandleFunOper2(code, posData)
   ↓

5. SDKManager::onHandleRecvDataOper(code, pos)
   ↓ m_curAxisData = pos  [保存当前位置，微米]
   ↓ sendEvent(EVENT_TYPE_MOVE_STATUS, 0, "Position updated", x_mm, y_mm, z_mm)

6. motionControlSDK::sdkEventCallback()
   ↓ emit positionUpdated(x_mm, y_mm, z_mm)

7. Qt应用接收信号
```

---

## 📝 使用示例

### 示例1：基本使用（微米单位）

```cpp
#include "motionControlSDK.h"

int main()
{
	QApplication app(argc, argv);
	
	motionControlSDK sdk;
	sdk.initialize();
	sdk.connectToDevice("192.168.100.57", 5555);
	
	// 方式1：直接使用微米
	MoveAxisPos pos1(100000, 200000, 50000);  // X=100mm, Y=200mm, Z=50mm
	sdk.moveToPosition(pos1);
	
	// 方式2：从毫米转换
	MoveAxisPos pos2 = MoveAxisPos::fromMillimeters(100, 200, 50);
	sdk.moveToPosition(pos2);
	
	// 获取目标位置
	MoveAxisPos target = sdk.getTargetPosition();
	double x, y, z;
	target.toMillimeters(x, y, z);
	qDebug() << "目标位置:" << x << y << z << "mm";
	
	return app.exec();
}
```

---

### 示例2：UI集成

```cpp
// UI类
class PrintDeviceUI : public QWidget
{
	Q_OBJECT
public:
	PrintDeviceUI()
	{
		sdk = new motionControlSDK(this);
		
		// 连接信号
		connect(sdk, &motionControlSDK::positionUpdated,
				this, &PrintDeviceUI::onPositionUpdated);
		
		sdk->initialize();
		sdk->connectToDevice("192.168.100.57", 5555);
	}
	
private slots:
	void onMoveButtonClicked()
	{
		// 从UI读取坐标（毫米）
		double x_mm = ui->xSpinBox->value();
		double y_mm = ui->ySpinBox->value();
		double z_mm = ui->zSpinBox->value();
		
		// 方式1：使用fromMillimeters
		MoveAxisPos pos = MoveAxisPos::fromMillimeters(x_mm, y_mm, z_mm);
		sdk->moveToPosition(pos);
		
		// 方式2：先设置，后执行
		sdk->setTargetPosition(pos);
		// ... 其他操作 ...
		sdk->moveToPosition(sdk->getTargetPosition());
	}
	
	void onPositionUpdated(double x, double y, double z)
	{
		// 更新UI显示（毫米）
		ui->xLabel->setText(QString("%1 mm").arg(x, 0, 'f', 3));
		ui->yLabel->setText(QString("%1 mm").arg(y, 0, 'f', 3));
		ui->zLabel->setText(QString("%1 mm").arg(z, 0, 'f', 3));
		
		// 也可以获取微米值
		MoveAxisPos current = sdk->getCurrentPosition();
		ui->statusLabel->setText(QString("当前位置: %1μm, %2μm, %3μm")
			.arg(current.xPos)
			.arg(current.yPos)
			.arg(current.zPos));
	}

private:
	motionControlSDK* sdk;
};
```

---

### 示例3：精确控制（微米级）

```cpp
// 精确移动到微米级坐标
void preciseMoveExample()
{
	motionControlSDK sdk;
	sdk.initialize();
	sdk.connectToDevice("192.168.100.57", 5555);
	
	// 微米级精度：100.123mm = 100123μm
	MoveAxisPos pos(100123, 200456, 50789);
	sdk.moveToPosition(pos);
	
	// 验证位置
	MoveAxisPos target = sdk.getTargetPosition();
	qDebug() << "精确目标位置:";
	qDebug() << "  X:" << target.xPos << "μm =" << (target.xPos / 1000.0) << "mm";
	qDebug() << "  Y:" << target.yPos << "μm =" << (target.yPos / 1000.0) << "mm";
	qDebug() << "  Z:" << target.zPos << "μm =" << (target.zPos / 1000.0) << "mm";
}
```

---

## ✅ 验收清单

完成实施后，请检查：

- [ ] `MoveAxisPos`已添加到`motionControlSDK.h`
- [ ] 所有成员变量使用`quint32`类型（微米单位）
- [ ] 提供了`fromMillimeters()`和`toMillimeters()`转换方法
- [ ] 添加了`moveToPosition()`等新API
- [ ] `SDKManager`中实现了位置管理
- [ ] `SDKMotion.cpp`中实现了`MoveAxisPos`到`QByteArray`的转换
- [ ] 信号槽连接正确（`SigHandleFunOper2` → `onHandleRecvDataOper`）
- [ ] 编译无错误无警告
- [ ] 单位转换正确（1mm = 1000μm）
- [ ] 日志输出正确
- [ ] 功能测试通过

---

## 🎯 关键点总结

### 单位转换

| 场景 | 输入单位 | 内部单位 | 协议单位 | 显示单位 |
|------|---------|---------|---------|---------|
| API调用 | mm | μm | μm | mm |
| 结构体存储 | - | μm | - | - |
| 协议传输 | - | - | μm | - |
| UI显示 | - | - | - | mm |
| 日志输出 | - | μm | - | mm (μm) |

### 转换公式

```cpp
// mm → μm
quint32 um = static_cast<quint32>(mm * 1000.0);

// μm → mm
double mm = static_cast<double>(um) / 1000.0;
```

---

**实施完成！** 🎉

所有代码和文档已准备就绪，可以开始集成测试。

