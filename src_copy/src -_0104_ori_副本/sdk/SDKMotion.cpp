/**
 * @file SDKMotion.cpp
 * @brief 运动控制实现（使用微米单位）
 * @details 处理X/Y/Z轴的移动和复位功能，使用MoveAxisPos结构体
 * @date 2025-12-23
 */

#include "SDKManager.h"
#include "protocol/ProtocolPrint.h"
#include "CLogManager.h"
#include "motionControlSDK.h"
#include <QByteArray>
#include <QDataStream>
#include <QtEndian>

 // ==================== 辅助函数 ====================

 /**
  * @brief 将单个轴位置转换为QByteArray协议数据
  * @param position 位置值（微米单位）
  * @param axis 轴标识（'X', 'Y', 'Z'），用于日志
  * @return 协议数据包（4字节，大端序）
  *
  * 协议格式：
  * - 4字节无符号整数（大端序）
  * - 单位：微米
  * - 范围：0 ~ 4294967295 微米（约4294mm）
  */
static QByteArray positionToByteArray(quint32 position, char axis)
{
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);

	// 写入位置（4字节，大端序）
	stream << position;

	// 转换为毫米用于日志
	double pos_mm = static_cast<double>(position) / 1000.0;

	LOG_INFO(QString(u8"%1轴 位置数据: %2 μm (%3 mm)")
		.arg(axis)
		.arg(position)
		.arg(pos_mm, 0, 'f', 3));

	// 打印十六进制（用于调试）
	QString hex = data.toHex(' ').toUpper();
	LOG_INFO(QString(u8"%1轴 协议数据(Hex): %2").arg(axis).arg(hex));

	return data;
}

/**
 * @brief 将完整MoveAxisPos转换为QByteArray
 * @param pos 位置数据（微米单位）
 * @return 协议数据包（12字节：X(4)+Y(4)+Z(4)，大端序）
 *
 * 协议格式：
 * - Byte 0-3:  X坐标（4字节无符号整数，大端序，微米）
 * - Byte 4-7:  Y坐标（4字节无符号整数，大端序，微米）
 * - Byte 8-11: Z坐标（4字节无符号整数，大端序，微米）
 */
static QByteArray fullPositionToByteArray(const MoveAxisPos& pos)
{
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);

	// 写入X/Y/Z坐标（各4字节，大端序，微米单位）
	stream << pos.xPos;
	stream << pos.yPos;
	stream << pos.zPos;

	// 转换为毫米用于日志
	double x_mm, y_mm, z_mm;
	pos.toMillimeters(x_mm, y_mm, z_mm);

	LOG_INFO(QString(u8"完整位置 转换为协议数据:"));
	LOG_INFO(QString(u8"  X = %1 mm (%2 μm)").arg(x_mm, 0, 'f', 3).arg(pos.xPos));
	LOG_INFO(QString(u8"  Y = %1 mm (%2 μm)").arg(y_mm, 0, 'f', 3).arg(pos.yPos));
	LOG_INFO(QString(u8"  Z = %1 mm (%2 μm)").arg(z_mm, 0, 'f', 3).arg(pos.zPos));

	// 打印十六进制（用于调试）
	QString hex = data.toHex(' ').toUpper();
	LOG_INFO(QString(u8"完整位置 协议数据(12字节 Hex): %1").arg(hex));

	return data;
}

// ==================== 运动控制实现 ====================

// ==================== 绝对运动 ====================
/**
 * @brief X轴移动（MoveAxisPos版本）
 * @param targetPos 目标位置结构体（仅使用xPos，其他轴补0）
 * @return 0=成功, -1=失败
 *
 * 协议格式：
 * - 命令类型: 0x0011 (控制命令)
 * - 数据区: 12字节 (X坐标 + Y=0 + Z=0)
 */
int SDKManager::move2AbsXAxis(const MoveAxisPos& targetPos)
{
	if (!isConnected())
	{
		LOG_INFO(QString(u8"X轴移动 失败：设备未连接"));
		return -1;
	}

	// 转换为毫米用于日志
	double x_mm = static_cast<double>(targetPos.xPos) / 1000.0;
	auto posArr = fullPositionToByteArray(targetPos);

	LOG_INFO(QString(u8"X轴绝对移动，目标位置: X=%1mm (%2μm), Y=0, Z=0; 协议数据(12字节 Hex): %3")
		.arg(x_mm, 0, 'f', 3)
		.arg(targetPos.xPos)
		.arg(QString(posArr.toHex(' ').toUpper())));

	// 选择命令（根据X坐标的符号）
	ProtocolPrint::FunCode cmd = (targetPos.xPos >= 0) ? ProtocolPrint::Ctrl_XAxisRMove : ProtocolPrint::Ctrl_XAxisLMove;
	LOG_INFO(QString(u8"手动相对运动模式: 相对移动_%1mm (当前%2μm_偏移%3μm)")
		.arg(x_mm, 0, 'f', 3)
		.arg(m_curAxisData.xPos));

	// 发送命令
	sendCommand(cmd, posArr);
	return 0;
}

/**
 * @brief Y轴移动（MoveAxisPos版本）
 * @param targetPos 目标位置结构体（仅使用yPos，其他轴补0）
 * @return 0=成功, -1=失败
 *
 * 协议格式：
 * - 命令类型: 0x0011 (控制命令)
 * - 数据区: 12字节 (X=0 + Y坐标 + Z=0)
 */
int SDKManager::move2AbsYAxis(const MoveAxisPos& targetPos)
{
	if (!isConnected())
	{
		LOG_INFO(QString(u8"Y轴移动 失败：设备未连接"));
		return -1;
	}

	// 转换为毫米用于日志
	double y_mm = static_cast<double>(targetPos.yPos) / 1000.0;
	auto posArr = fullPositionToByteArray(targetPos);
	LOG_INFO(QString(u8"X轴绝对移动，目标位置: X=0, Y=%1mm (%2μm), Z=0; 协议数据(12字节 Hex): %3")
		.arg(y_mm, 0, 'f', 3)
		.arg(targetPos.yPos)
		.arg(QString(posArr.toHex(' ').toUpper())));


	// 选择命令（根据y坐标的符号）
	ProtocolPrint::FunCode cmd = (targetPos.yPos >= 0) ? ProtocolPrint::Ctrl_YAxisRMove : ProtocolPrint::Ctrl_YAxisLMove;
	LOG_INFO(QString(u8"手动相对运动模式: 相对移动_%1mm (当前%2μm_偏移%3μm)")
		.arg(y_mm, 0, 'f', 3)
		.arg(m_curAxisData.yPos));

	sendCommand(cmd, posArr);
	return 0;
}

/**
 * @brief Z轴移动（MoveAxisPos版本）
 * @param targetPos 目标位置结构体（仅使用zPos，其他轴补0）
 * @return 0=成功, -1=失败
 *
 * 协议格式：
 * - 命令类型: 0x0011 (控制命令)
 * - 数据区: 12字节 (X=0 + Y=0 + Z坐标)
 */
int SDKManager::move2AbsZAxis(const MoveAxisPos& targetPos)
{
	if (!isConnected()) 
	{
		LOG_INFO(QString(u8"Z轴移动 失败：设备未连接"));
		return -1;
	}

	// 转换为毫米用于日志
	double z_mm = static_cast<double>(targetPos.zPos) / 1000.0;
	auto posArr = fullPositionToByteArray(targetPos);
	LOG_INFO(QString(u8"X轴绝对移动，目标位置: X=0, Y= 0, Z=%1mm (%2μm); 协议数据(12字节 Hex): %3")
		.arg(z_mm, 0, 'f', 3)
		.arg(targetPos.zPos)
		.arg(QString(posArr.toHex(' ').toUpper())));


	// 选择命令（根据y坐标的符号）
	ProtocolPrint::FunCode cmd = (targetPos.zPos >= 0) ? ProtocolPrint::Ctrl_ZAxisLMove : ProtocolPrint::Ctrl_ZAxisRMove;
	LOG_INFO(QString(u8"手动相对运动模式: 相对移动_%1mm (当前%2μm_偏移%3μm)")
		.arg(z_mm, 0, 'f', 3)
		.arg(m_curAxisData.yPos));

	sendCommand(cmd, posArr);
	return 0;

}

// ==================== 运动控制实现 ====================
// ==================== 相对运动 ====================

/**
 * @brief X轴移动
 * @param distance 移动距离（毫米）
 * @return 0=成功, -1=失败
 *
 * 流程：
 * 1. 根据isAbsolute确定目标位置
 * 2. 将目标位置（微米）转换为QByteArray
 * 3. 发送命令到设备
 */
int SDKManager::move2RelXAxis(double distance)
{
	if (!isConnected())
	{
		LOG_INFO(QString(u8"X轴移动 失败：设备未连接"));
		return -1;
	}
	LOG_INFO(QString(u8"X轴移动相对位置参数: distance = %1mm").arg(distance, 0, 'f', 3));

	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	// 相对移动
	quint32 offset_um = static_cast<quint32>(abs(distance) * 1000.0);

	stream << offset_um;		// X轴坐标（微米）
	stream << (quint32)0;		// Y轴补0
	stream << (quint32)0;		// Z轴补0

	// 选择命令（根据移动方向）
	ProtocolPrint::FunCode cmd = (distance >= 0) ? ProtocolPrint::Ctrl_XAxisRMove : ProtocolPrint::Ctrl_XAxisLMove;
	LOG_INFO(QString(u8"手动相对运动模式: 相对移动_%1mm (当前%2μm_偏移%3μm)")
		.arg(distance, 0, 'f', 3)
		.arg(m_curAxisData.xPos)
		.arg(offset_um));

	sendCommand(cmd, data);
	return 0;
}

/**
 * @brief Y轴移动
 * @param distance 移动距离（毫米）
 * @param isAbsolute true=绝对移动，false=相对移动
 * @return 0=成功, -1=失败
 */
int SDKManager::move2RelYAxis(double distance)
{
	if (!isConnected())
	{
		LOG_INFO(QString(u8"Y轴移动 失败：设备未连接"));
		return -1;
	}

	LOG_INFO(QString(u8"Y轴移动相对位置参数: distance=%1mm ").arg(distance, 0, 'f', 3));

	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	// 相对移动
	quint32 offset_um = static_cast<quint32>(abs(distance) * 1000.0);

	stream << (quint32)0;		// X轴坐标（微米）
	stream << offset_um ;		// Y轴补0
	stream << (quint32)0;		// Z轴补0

	// 选择命令（根据移动方向）
	ProtocolPrint::FunCode cmd = (distance >= 0) ? ProtocolPrint::Ctrl_YAxisRMove : ProtocolPrint::Ctrl_YAxisLMove;
	LOG_INFO(QString(u8"手动相对运动模式: 相对移动_%1mm (当前%2μm_偏移%3μm)")
		.arg(distance, 0, 'f', 3)
		.arg(m_curAxisData.yPos)
		.arg(offset_um));

	sendCommand(cmd, data);
	return 0;
}

/**
 * @brief Z轴移动
 * @param distance 移动距离（毫米），正数=向上，负数=向下
 * @param isAbsolute true=绝对移动，false=相对移动
 * @return 0=成功, -1=失败
 */
int SDKManager::move2RelZAxis(double distance)
{
	if (!isConnected()) {
		LOG_INFO(QString(u8"Z轴移动 失败：设备未连接"));
		return -1;
	}
	LOG_INFO(QString(u8"Z轴移动相对位置参数: distance=%1mm ").arg(distance, 0, 'f', 3));

	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	// 相对移动
	quint32 offset_um = static_cast<quint32>(abs(distance) * 1000.0);

	stream << (quint32)0;		// X轴补0 
	stream << (quint32)0;		// Y轴补0
	stream << offset_um ;		// Z轴坐标（微米）

	// 选择命令（根据移动方向）
	ProtocolPrint::FunCode cmd = (distance >= 0) ? ProtocolPrint::Ctrl_ZAxisLMove : ProtocolPrint::Ctrl_ZAxisRMove;
	LOG_INFO(QString(u8"手动相对运动模式: 相对移动_%1mm (当前%2μm_偏移%3μm)")
		.arg(distance, 0, 'f', 3)
		.arg(m_curAxisData.zPos)
		.arg(offset_um));

	sendCommand(cmd, data);
	return 0;
}






// ==================== 3轴同时移动 ====================
int SDKManager::move2RelPos(double dx, double dy, double dz)
{
	if (!isConnected())
	{
		LOG_INFO(QString(u8"X轴移动 失败：设备未连接"));
		return -1;
	}

	//LOG_INFO(QString(u8" X轴移动相对位置"));
	LOG_INFO(QString(u8"参数: distance = %1mm").arg(dx, 0, 'f', 3));
	MoveAxisPos movPos;
	// 相对移动：在当前位置基础上偏移
	auto calRealPos = [&](double distance, quint32 curPosUm)->quint32
	{
		quint32 targetPosUm;
		quint32 offsetUm = static_cast<quint32>(abs(distance) * 1000.0);

		if (distance > 0)
		{
			// 正方向移动
			targetPosUm = curPosUm + offsetUm;
			LOG_INFO(QString(u8"  模式: 相对移动 +%1mm (当前%2μm + 偏移%3μm = 目标%4μm)")
				.arg(dx, 0, 'f', 3)
				.arg(curPosUm)
				.arg(distance)
				.arg(targetPosUm));
		}
		else
		{
			// 负方向移动
			if (curPosUm > offsetUm)
			{
				targetPosUm = curPosUm - offsetUm;
			}
			else
			{
				targetPosUm = 0;
				LOG_INFO(QString(u8"  警告: 移动距离超过当前位置，限制到0"));
			}
			LOG_INFO(QString(u8"  模式: 相对移动 %1mm (当前%2μm - 偏移%3μm = 目标%4μm)")
				.arg(dx, 0, 'f', 3)
				.arg(curPosUm)
				.arg(distance)
				.arg(targetPosUm));
		}
		return targetPosUm;
	};
	movPos.xPos = calRealPos(dx, m_curAxisData.xPos);
	movPos.yPos = calRealPos(dy, m_curAxisData.yPos);
	movPos.zPos = calRealPos(dz, m_curAxisData.zPos);


	// 转换坐标数据转换为字节数据
	QByteArray arrData = fullPositionToByteArray(movPos);
	// 选择命令（根据移动方向）
	ProtocolPrint::FunCode cmd = ProtocolPrint::Ctrl_AxisAbsMove;
	LOG_INFO(QString(u8"相对运动命令转换为绝对命令: Ctrl_AxisAbsMove (0x%1)，数据: 0x%2").arg(QString::number(cmd, 16).toUpper()).arg(QString(arrData.toHex().toUpper()).data()));

	// 发送命令
	sendCommand(cmd, arrData);
	return 0;
}


/**
 * @brief 3轴同时移动（结构体版本）
 * @param targetPos 目标位置（微米单位）
 * @return 0=成功, -1=失败
 */
int SDKManager::move2AbsPosition(const MoveAxisPos& targetPos)
{
	if (!isConnected())
	{
		return -1;
	}

	// 设置目标位置（用于后续操作）
	setTargetPosition(targetPos);

	// 转换为12字节协议数据
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);

	// 写入X/Y/Z坐标（各4字节，大端序，微米单位）
	stream << targetPos.xPos;
	stream << targetPos.yPos;
	stream << targetPos.zPos;

	// 转换为毫米用于日志
	double x_mm, y_mm, z_mm;
	targetPos.toMillimeters(x_mm, y_mm, z_mm);

	LOG_INFO(QString(u8"  3轴同时移动  "));
	LOG_INFO(QString(u8"目标位置: X=%1mm, Y=%2mm, Z=%3mm")
		.arg(x_mm, 0, 'f', 3)
		.arg(y_mm, 0, 'f', 3)
		.arg(z_mm, 0, 'f', 3));

	// 打印十六进制数据
	QString hex = data.toHex(' ').toUpper();
	LOG_INFO(QString(u8"协议数据(12字节 Hex): %1").arg(hex));

	// 发送命令：绝对移动 (Ctrl_AxisAbsMove = 0x3107)
	sendCommand(ProtocolPrint::Ctrl_AxisAbsMove, data);

	LOG_INFO(QString(u8"3轴移动 命令已发送 "));
	return 0;
}

/**
 * @brief 3轴同时移动（字节数组版本）
 * @param positionData 位置数据（12字节：X4+Y4+Z4）
 * @return 0=成功, -1=失败
 */
int SDKManager::move2AbsPosition(const QByteArray& positionData)
{
	if (!isConnected())
	{
		return -1;
	}

	// 验证数据长度
	if (positionData.size() != 12)
	{
		LOG_INFO(QString(u8"3轴移动 数据长度错误: 期望12字节，实际%1字节")
			.arg(positionData.size()));
		return -1;
	}

	// 解析数据用于日志（大端序）
	QDataStream stream(positionData);
	stream.setByteOrder(QDataStream::LittleEndian);

	quint32 xPos, yPos, zPos;
	stream >> xPos >> yPos >> zPos;

	// 转换为毫米用于日志
	double x_mm = static_cast<double>(xPos) / 1000.0;
	double y_mm = static_cast<double>(yPos) / 1000.0;
	double z_mm = static_cast<double>(zPos) / 1000.0;

	LOG_INFO(QString(u8"  3轴同时移动（字节数组）  "));
	LOG_INFO(QString(u8"目标位置: X=%1mm, Y=%2mm, Z=%3mm")
		.arg(x_mm, 0, 'f', 3)
		.arg(y_mm, 0, 'f', 3)
		.arg(z_mm, 0, 'f', 3));

	// 打印十六进制数据
	QString hex = positionData.toHex(' ').toUpper();
	LOG_INFO(QString(u8"协议数据(12字节 Hex): %1").arg(hex));

	// 发送命令：绝对移动 (Ctrl_AxisAbsMove = 0x3107)
	sendCommand(ProtocolPrint::Ctrl_AxisAbsMove, positionData);

	LOG_INFO(QString(u8"3轴移动 命令已发送 "));
	return 0;
}


// ==================== 3轴复位 ====================
/**
 * @brief 轴复位
 * @param axisFlag 轴标志位：bit0(1)=X, bit1(2)=Y, bit2(4)=Z，可组合
 * @return 0=成功, -1=失败
 *
 * 示例：
 * - axisFlag=1: 仅复位X轴
 * - axisFlag=3: 复位X和Y轴
 * - axisFlag=7: 复位所有轴
 */
int SDKManager::resetAxis(int axisFlag)
{
	if (!isConnected())
	{
		LOG_INFO(QString(u8"轴复位 失败：设备未连接"));
		return -1;
	}

	LOG_INFO(QString(u8"轴复位 axisFlag = %1").arg(QString::number(axisFlag, 10).toUpper()));
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);


	//QStringList axes;
	//if (axisFlag & 1) axes << "X";
	//if (axisFlag & 2) axes << "Y";
	//if (axisFlag & 4) axes << "Z";
	//LOG_INFO(QString(u8"  要复位的轴: %1").arg(axes.join(", ")));

	// 复位各轴
	switch (axisFlag)
	{
	case 1:
	{
		stream << (quint32)1;		// X轴补0 
		stream << (quint32)0;		// Y轴补0
		stream << (quint32)0;		// Z轴补0

		// 复位X轴
		m_dstAxisData.xPos = 0;
		m_curAxisData.xPos = 0;
		break;
	}
	case 2:
	{
		stream << (quint32)0;		// X轴补0 
		stream << (quint32)1;		// Y轴补0
		stream << (quint32)0;		// Z轴补0

		// 复位X轴
		m_dstAxisData.yPos = 0;
		m_curAxisData.yPos = 0;
		break;
	}
	case 3:
	{
		stream << (quint32)1;		// X轴补0 
		stream << (quint32)1;		// Y轴补0
		stream << (quint32)0;		// Z轴补0

		// 复位X轴
		m_dstAxisData.xPos = 0;
		m_curAxisData.xPos = 0;
		m_dstAxisData.yPos = 0;
		m_curAxisData.yPos = 0;
		break;
	}
	case 4:
	{
		stream << (quint32)0;		// X轴补0 
		stream << (quint32)0;		// Y轴补0
		stream << (quint32)1;		// Z轴补0

		// 复位X轴
		m_dstAxisData.zPos = 0;
		m_curAxisData.zPos = 0;
		break;
	}
	case 5:
	{
		stream << (quint32)1;		// X轴补0 
		stream << (quint32)0;		// Y轴补0
		stream << (quint32)1;		// Z轴补0

		// 复位X轴
		m_dstAxisData.xPos = 0;
		m_curAxisData.xPos = 0;
		m_dstAxisData.zPos = 0;
		m_curAxisData.zPos = 0;
		break;
	}
	case 6:
	{
		stream << (quint32)1;		// X轴补0 
		stream << (quint32)1;		// Y轴补0
		stream << (quint32)0;		// Z轴补0

		// 复位X轴
		m_dstAxisData.xPos = 0;
		m_curAxisData.xPos = 0;
		m_dstAxisData.yPos = 0;
		m_curAxisData.yPos = 0;
		break;
	}
	case 7:
	{
		stream << (quint32)1;		// X轴补0 
		stream << (quint32)1;		// Y轴补0
		stream << (quint32)1;		// Z轴补0

		// 复位X轴
		m_dstAxisData.xPos = 0;
		m_curAxisData.xPos = 0;
		m_dstAxisData.yPos = 0;
		m_curAxisData.yPos = 0;
		m_dstAxisData.zPos = 0;
		m_curAxisData.zPos = 0;

		break;
	}
	default:
		break;
	}

	sendCommand(ProtocolPrint::Ctrl_ResetPos, data);
	//LOG_INFO(QString(u8"轴复位所有复位命令已发送"));
	return 0;
}