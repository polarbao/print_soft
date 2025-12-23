/**
 * @file SDKManager_Position.cpp
 * @brief 位置管理实现
 * @details 管理目标位置和当前位置（单位：微米）
 * @date 2025-12-23
 */

#include "SDKManager.h"
#include "comm/CLogManager.h"
#include "motionControlSDK.h"
#include <QMutexLocker>
#include <QString>

// ==================== 位置管理 ====================

/**
 * @brief 设置目标位置
 * @param targetPos 目标位置（微米单位）
 */
void SDKManager::setTargetPosition(const MoveAxisPos& targetPos)
{
	// 保存目标位置
	m_dstAxisData.xPos = targetPos.xPos;
	m_dstAxisData.yPos = targetPos.yPos;
	m_dstAxisData.zPos = targetPos.zPos;
	
	// 转换为毫米用于日志
	double x_mm, y_mm, z_mm;
	targetPos.toMillimeters(x_mm, y_mm, z_mm);
	
	LOG_INFO(QString(u8"[位置管理] 设置目标位置:"));
	LOG_INFO(QString(u8"  X = %1 mm (%2 μm)")
		.arg(x_mm, 0, 'f', 3).arg(targetPos.xPos));
	LOG_INFO(QString(u8"  Y = %1 mm (%2 μm)")
		.arg(y_mm, 0, 'f', 3).arg(targetPos.yPos));
	LOG_INFO(QString(u8"  Z = %1 mm (%2 μm)")
		.arg(z_mm, 0, 'f', 3).arg(targetPos.zPos));
}

/**
 * @brief 获取目标位置
 * @return 目标位置（微米单位）
 */
MoveAxisPos SDKManager::getTargetPosition() const
{
	return m_dstAxisData;
}

/**
 * @brief 获取当前位置
 * @return 当前位置（微米单位）
 */
MoveAxisPos SDKManager::getCurrentPosition() const
{
	return m_curAxisData;
}

// ==================== 位置数据接收处理 ====================

/**
 * @brief 处理接收到的位置数据（槽函数）
 * @param code 命令码
 * @param pos 位置数据（微米单位，从协议解析得到）
 * 
 * 数据流程：
 * 1. 设备发送位置数据（协议格式）
 * 2. ProtocolPrint解析为MoveAxisPos（微米）
 * 3. 通过信号SigHandleFunOper2传递到这里
 * 4. 更新m_curAxisData
 * 5. 通过sendEvent发送到上层（转换为mm）
 */
void SDKManager::onHandleRecvDataOper(int code, const MoveAxisPos& pos)
{
	LOG_INFO(QString(u8"[位置数据] 收到位置数据回复，命令码: 0x%1")
		.arg(QString::number(code, 16).toUpper().rightJustified(4, '0')));
	
	// 更新当前位置
	m_curAxisData.xPos = pos.xPos;
	m_curAxisData.yPos = pos.yPos;
	m_curAxisData.zPos = pos.zPos;
	
	// 转换为毫米用于日志和事件
	double x_mm, y_mm, z_mm;
	pos.toMillimeters(x_mm, y_mm, z_mm);
	
	LOG_INFO(QString(u8"位置数据 当前位置:"));
	LOG_INFO(QString(u8"  X = %1 mm (%2 μm)")
		.arg(x_mm, 0, 'f', 3).arg(pos.xPos));
	LOG_INFO(QString(u8"  Y = %1 mm (%2 μm)")
		.arg(y_mm, 0, 'f', 3).arg(pos.yPos));
	LOG_INFO(QString(u8"  Z = %1 mm (%2 μm)")
		.arg(z_mm, 0, 'f', 3).arg(pos.zPos));
	
	// 计算与目标位置的偏差
	double dx_mm = (static_cast<int>(m_dstAxisData.xPos) - static_cast<int>(pos.xPos)) / 1000.0;
	double dy_mm = (static_cast<int>(m_dstAxisData.yPos) - static_cast<int>(pos.yPos)) / 1000.0;
	double dz_mm = (static_cast<int>(m_dstAxisData.zPos) - static_cast<int>(pos.zPos)) / 1000.0;
	
	double distance = sqrt(dx_mm*dx_mm + dy_mm*dy_mm + dz_mm*dz_mm);
	
	if (distance > 0.001)
	{  // 偏差大于1微米
		LOG_INFO(QString(u8"位置数据 与目标位置偏差: %.3f mm (X=%.3f, Y=%.3f, Z=%.3f)")
			.arg(distance)
			.arg(dx_mm)
			.arg(dy_mm)
			.arg(dz_mm));
	} 
	else 
	{
		//LOG_INFO(QString(u8"位置数据 已到达目标位置"));
	}
	
	// 发送位置更新事件到上层（毫米单位）
	sendEvent(EVENT_TYPE_MOVE_STATUS, 0, "Position updated", x_mm, y_mm, z_mm);
}

