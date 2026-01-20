/**
 * @file SDKMotion.cpp
 * @brief 运动控制实现（使用微米单位）
 * @details 处理X/Y/Z轴的移动和复位功能，使用MoveAxisPos结构体
 * @date 2025-12-23
 */

#include "SDKManager.h"
#include "protocol/ProtocolPrint.h"
#include "SpdlogMgr.h"
#include "motionControlSDK.h"
#include "Utils.h"
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

	NAMED_LOG_T("logicMoudle", "{}轴 位置数据: {}μm ({:.3f}mm)",
		axis,
		position,
		pos_mm);

	// 打印十六进制（用于调试）
	//QString hex = data.toHex(' ').toUpper();
	NAMED_LOG_T("logicMoudle", "{}轴 协议数据(Hex): {}",  axis, data.toHex(' ').toUpper());
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

	NAMED_LOG_T("logicMoudle", "完整位置 转换为协议数据:");
	NAMED_LOG_T("logicMoudle", "X = {:.3f} mm ({} μm)", x_mm, pos.xPos);
	NAMED_LOG_T("logicMoudle", "Y = {:.3f} mm ({} μm)", y_mm, pos.yPos);
	NAMED_LOG_T("logicMoudle", "Z = {:.3f} mm ({} μm)", z_mm, pos.zPos);

	// 打印十六进制（用于调试）
	//QString hex = data.toHex(' ').toUpper();
	NAMED_LOG_T("logicMoudle", "完整位置 协议数据(12字节 Hex): {}", data.toHex(' ').toUpper());
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
int SDKManager::Move2AbsXAxis(const MoveAxisPos& targetPos)
{
	if (!IsConnected())
	{
		NAMED_LOG_T("logicMoudle", "X轴移动 失败：设备未连接");
		return -1;
	}

	// 转换为毫米用于日志
	double x_mm = static_cast<double>(targetPos.xPos) / 1000.0;
	auto posArr = Utils::GetInstance()->fullPositionToByteArray(targetPos);

	NAMED_LOG_T("logicMoudle", "X轴绝对移动，目标位置: X={:.3f}mm ({}μm), Y=0, Z=0; 协议数据(12字节 Hex): {}", 
		x_mm,
		targetPos.xPos,
		posArr.toHex(' ').toUpper());

	// 选择命令（根据X坐标的符号）
	ProtocolPrint::FunCode cmd = (targetPos.xPos >= 0) ? ProtocolPrint::Ctrl_XAxisRMove : ProtocolPrint::Ctrl_XAxisLMove;
	NAMED_LOG_T("logicMoudle", "手动相对运动模式: 相对移动_{:3.f}mm (当前 {}μm_偏移μm)",
		x_mm, 
		m_curAxisData.xPos);

	// 发送命令
	SendCommand(cmd, posArr);
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
int SDKManager::Move2AbsYAxis(const MoveAxisPos& targetPos)
{
	if (!IsConnected())
	{
		NAMED_LOG_T("logicMoudle", u8"Y轴移动 失败：设备未连接");
		return -1;
	}

	// 转换为毫米用于日志
	double y_mm = static_cast<double>(targetPos.yPos) / 1000.0;
	auto posArr = Utils::GetInstance()->fullPositionToByteArray(targetPos);
	NAMED_LOG_T("logicMoudle", u8"X轴绝对移动，目标位置: X=0, Y={:.3f}mm ({}μm), Z=0; 协议数据(12字节 Hex): {}", 
		y_mm,
		targetPos.yPos,
		posArr.toHex(' ').toUpper());


	// 选择命令（根据y坐标的符号）
	ProtocolPrint::FunCode cmd = (targetPos.yPos >= 0) ? ProtocolPrint::Ctrl_YAxisRMove : ProtocolPrint::Ctrl_YAxisLMove;
	NAMED_LOG_T("logicMoudle", "手动相对运动模式: 相对移动_{:.3f}mm (当前{}μm)",
		y_mm,
		m_curAxisData.yPos);

	SendCommand(cmd, posArr);
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
int SDKManager::Move2AbsZAxis(const MoveAxisPos& targetPos)
{
	if (!IsConnected())
	{
		NAMED_LOG_T("logicMoudle", "Z轴移动 失败：设备未连接");
		return -1;
	}

	// 转换为毫米用于日志
	double z_mm = static_cast<double>(targetPos.zPos) / 1000.0;
	auto posArr = Utils::GetInstance()->fullPositionToByteArray(targetPos);
	NAMED_LOG_T("logicMoudle", u8"Z轴绝对移动，目标位置: X=0, Y=0, Z={:.3f}mm ({}μm); 协议数据(12字节 Hex): {}",
		z_mm,
		targetPos.zPos,
		posArr.toHex(' ').toUpper());


	// 选择命令（根据y坐标的符号）
	ProtocolPrint::FunCode cmd = (targetPos.zPos >= 0) ? ProtocolPrint::Ctrl_ZAxisLMove : ProtocolPrint::Ctrl_ZAxisRMove;
	NAMED_LOG_T("logicMoudle", "手动相对运动模式: 相对移动_{:.3f}mm (当前{}μm)",
		z_mm,
		m_curAxisData.zPos);

	SendCommand(cmd, posArr);
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
int SDKManager::Move2RelXAxis(double distance)
{
	if (!IsConnected())
	{
		NAMED_LOG_T("logicMoudle", u8"X轴移动 失败：设备未连接");
		return -1;
	}
	NAMED_LOG_T("logicMoudle", u8"X轴移动相对位置参数: distance = {:.3f}mm", distance);

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
	NAMED_LOG_T("logicMoudle", "手动相对运动模式: 相对移动_{:.3f}mm (当前{}μm_偏移{}μm)", 
		distance,
		m_curAxisData.xPos,
		offset_um);

	SendCommand(cmd, data);
	return 0;
}

/**
 * @brief Y轴移动
 * @param distance 移动距离（毫米）
 * @param isAbsolute true=绝对移动，false=相对移动
 * @return 0=成功, -1=失败
 */
int SDKManager::Move2RelYAxis(double distance)
{
	if (!IsConnected())
	{
		NAMED_LOG_T("logicMoudle", "Y轴移动 失败：设备未连接");
		return -1;
	}

	NAMED_LOG_T("logicMoudle", u8"Y轴移动相对位置参数: distance={:.3f}mm ", distance);

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
	NAMED_LOG_T("logicMoudle", "手动相对运动模式:  相对移动_{:.3f}mm (当前{}μm_偏移{}μm)",
		distance,
		m_curAxisData.yPos,
		offset_um);

	SendCommand(cmd, data);
	return 0;
}

/**
 * @brief Z轴移动
 * @param distance 移动距离（毫米），正数=向上，负数=向下
 * @param isAbsolute true=绝对移动，false=相对移动
 * @return 0=成功, -1=失败
 */
int SDKManager::Move2RelZAxis(double distance)
{
	if (!IsConnected()) 
	{
		NAMED_LOG_T("logicMoudle", u8"Z轴移动 失败：设备未连接");
		return -1;
	}
	NAMED_LOG_T("logicMoudle", "Z轴移动相对位置参数: distance={:.3f} mm ", distance);

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

	NAMED_LOG_T("logicMoudle", "手动相对运动模式:  相对移动_{:.3f}mm (当前{}μm_偏移{}μm)",
		distance,
		m_curAxisData.zPos,
		offset_um);

	SendCommand(cmd, data);
	return 0;
}






// ==================== 3轴同时移动 ====================
int SDKManager::Move2RelPos(double dx, double dy, double dz)
{
	if (!IsConnected())
	{
		NAMED_LOG_T("logicMoudle", "X轴移动 失败：设备未连接");
		return -1;
	}

	//NAMED_LOG_T("logicMoudle", u8" X轴移动相对位置"));
	NAMED_LOG_T("logicMoudle", "参数: distance = {:.3f}mm", dx);
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
			NAMED_LOG_T("logicMoudle", "模式: 相对移动 +{:.3f}mm (当前{}μm + 偏移{}μm = 目标{}μm)", 
				dx,
				curPosUm,
				distance,
				targetPosUm);
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
				NAMED_LOG_T("logicMoudle", "警告: 移动距离超过当前位置，限制到0");
			}
			NAMED_LOG_T("logicMoudle", "模式: 相对移动 -{:.3f}mm(当前{}μm - 偏移{}μm = 目标{}μm)", 
				dx,
				curPosUm,
				distance,
				targetPosUm);
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
	NAMED_LOG_T("logicMoudle", "相对运动命令转换为绝对命令: Ctrl_AxisAbsMove 0x{:04X}，数据: 0x{:04X}", 
		QString::number(cmd, 16).toUpper(), 
		arrData.toHex().toUpper());

	// 发送命令
	SendCommand(cmd, arrData);
	return 0;
}


/**
 * @brief 3轴同时移动（结构体版本）
 * @param targetPos 目标位置（微米单位）
 * @return 0=成功, -1=失败
 */
int SDKManager::Move2AbsPosition(const MoveAxisPos& targetPos)
{
	if (!IsConnected())
	{
		return -1;
	}

	// 设置目标位置（用于后续操作）
	SetTargetPosition(targetPos);

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

	NAMED_LOG_T("logicMoudle", "3轴同时移动");
	NAMED_LOG_T("logicMoudle", "目标位置: X={:.3f}mm, Y={:.3f}mm, Z={:.3f}mm",
		x_mm,
		y_mm,
		z_mm);

	// 打印十六进制数据
	//QString hex = data.toHex(' ').toUpper();
	NAMED_LOG_T("logicMoudle", "协议数据(12字节 Hex): {}", data.toHex(' ').toUpper());

	// 发送命令：绝对移动 (Ctrl_AxisAbsMove = 0x3107)
	SendCommand(ProtocolPrint::Ctrl_AxisAbsMove, data);

	NAMED_LOG_T("logicMoudle", u8"3轴移动 命令已发送 ");
	return 0;
}

/**
 * @brief 3轴同时移动（字节数组版本）
 * @param positionData 位置数据（12字节：X4+Y4+Z4）
 * @return 0=成功, -1=失败
 */
int SDKManager::Move2AbsPosition(const QByteArray& positionData)
{
	if (!IsConnected())
	{
		return -1;
	}

	// 验证数据长度
	if (positionData.size() != 12)
	{
		NAMED_LOG_T("logicMoudle", "3轴移动 数据长度错误: 期望12字节，实际{}字节", positionData.size());
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

	NAMED_LOG_T("logicMoudle", "3轴同时移动（字节数组）");
	NAMED_LOG_T("logicMoudle", "目标位置: X={:.3f}mm, Y={:.3f}mm, Z={:.3f}mm",
		x_mm,
		y_mm,
		z_mm);

	// 打印十六进制数据
	QString hex = positionData.toHex(' ').toUpper();
	NAMED_LOG_T("logicMoudle", u8"协议数据(12字节 Hex): {}", positionData.toHex(' ').toUpper());

	// 发送命令：绝对移动 (Ctrl_AxisAbsMove = 0x3107)
	SendCommand(ProtocolPrint::Ctrl_AxisAbsMove, positionData);

	NAMED_LOG_T("logicMoudle", u8"3轴移动 命令已发送 ");
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
int SDKManager::ResetAxis(int axisFlag)
{
	if (!IsConnected())
	{
		NAMED_LOG_T("logicMoudle", u8"轴复位 失败：设备未连接");
		return -1;
	}

	NAMED_LOG_T("logicMoudle", u8"轴复位 axisFlag =  {} ", QString::number(axisFlag, 10).toUpper());
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);

	bool isX = (axisFlag & 1);
	bool isY = (axisFlag & 2);
	bool isZ = (axisFlag & 4);

	stream << quint32(isX ? 1 : 0);		 
	stream << quint32(isY ? 1 : 0);		
	stream << quint32(isZ ? 1 : 0);
	
	if (isX)
	{
		// 复位X轴
		m_dstAxisData.xPos = 0;		
		m_curAxisData.xPos = 0;
	}
	if (isY)
	{
		// 复位X轴
		m_dstAxisData.yPos = 0;
		m_curAxisData.yPos = 0;
	}
	if (isZ)
	{
		// 复位Z轴
		m_dstAxisData.zPos = 0;
		m_curAxisData.zPos = 0;
	}
	//QStringList axes;
	//if (axisFlag & 1) axes << "X";
	//if (axisFlag & 2) axes << "Y";
	//if (axisFlag & 4) axes << "Z";
	//NAMED_LOG_T("logicMoudle", u8"  要复位的轴: %1").arg(axes.join(", ")));


	SendCommand(ProtocolPrint::Ctrl_MoveOrigin, data);
	//NAMED_LOG_T("logicMoudle", u8"轴复位所有复位命令已发送"));
	return 0;
}




// ==================== 找原点操作（设置偏移量） ====================
/**
 * @brief 找原点操作
 * @param axisFlag 轴标志位：bit0(1)=X, bit1(2)=Y, bit2(4)=Z，可组合
 * @return 0=成功, -1=失败
 *
 * 示例：
 * - axisFlag=1: 仅复位X轴
 * - axisFlag=3: 复位X和Y轴
 * - axisFlag=7: 复位所有轴
 */
int SDKManager::SetOriginOffsetData(const MoveAxisPos& data)
{
	if (!IsConnected())
	{
		NAMED_LOG_T("logicMoudle", "设置原点偏移量数据失败, 设备未连接");
		return -1;
	}


	NAMED_LOG_T("logicMoudle", "设置原点偏移量数据 x_offset={}, y_offset={}, z_offset={}", data.xPos, data.yPos, data.zPos);
	SendCommand(ProtocolPrint::SetParam_OriginOffset, data);
	return 0;
}


