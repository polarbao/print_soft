/**
 * @file SDKManager_Position.cpp
 * @brief 位置管理实现
 * @details 管理目标位置和当前位置（单位：微米）
 * @date 2025-12-23
 */

#include "SDKManager.h"
#include "motionControlSDK.h"
#include <QMutexLocker>
#include <QString>
#include "SpdlogMgr.h"

// ==================== 位置管理 ====================

/**
 * @brief 设置目标位置
 * @param targetPos 目标位置（微米单位）
 */
void SDKManager::SetTargetPosition(const MoveAxisPos& targetPos)
{
	// 保存目标位置
	m_dstAxisData.xPos = targetPos.xPos;
	m_dstAxisData.yPos = targetPos.yPos;
	m_dstAxisData.zPos = targetPos.zPos;
	
	// 转换为毫米用于日志
	double x_mm, y_mm, z_mm;
	targetPos.toMillimeters(x_mm, y_mm, z_mm);
	
	NAMED_LOG_T("logicMoudle","位置管理 设置目标位置:");
	NAMED_LOG_T("logicMoudle", "  X = {:.3f} mm, {}μm", x_mm, targetPos.xPos);
	NAMED_LOG_T("logicMoudle", "  Y = {:.3f} mm, {}μm", y_mm, targetPos.yPos);
	NAMED_LOG_T("logicMoudle", "  Z = {:.3f} mm, {}μm", z_mm, targetPos.zPos);
}

/**
 * @brief 获取目标位置
 * @return 目标位置（微米单位）
 */
MoveAxisPos SDKManager::GetTargetPosition() const
{
	return m_dstAxisData;
}

/**
 * @brief 获取当前位置
 * @return 当前位置（微米单位）
 */
MoveAxisPos SDKManager::GetCurrentPosition() const
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
 * 3. 通过信号SigHandleAxisPosData传递到这里
 * 4. 更新m_curAxisData
 * 5. 通过SendEvent发送到上层（转换为mm）
 */
void SDKManager::OnHandleRecvDataOper(int code, const MoveAxisPos& pos)
{
	NAMED_LOG_T("logicMoudle", "位置数据 收到位置数据回复，命令码: 0x{:04X}", code);
	// 更新当前位置
	m_curAxisData = pos;

	
	// 转换为毫米用于日志和事件
	double x_mm, y_mm, z_mm;
	pos.toMillimeters(x_mm, y_mm, z_mm);
	
	NAMED_LOG_T("logicMoudle", "位置管理 当前位置:");
	NAMED_LOG_T("logicMoudle", "  X = {:.3f}mm, {}μm", x_mm, pos.xPos);
	NAMED_LOG_T("logicMoudle", "  Y = {:.3f}mm, {}μm", y_mm, pos.yPos);
	NAMED_LOG_T("logicMoudle", "  Z = {:.3f}mm, {}μm", z_mm, pos.zPos);
	
	// 计算与目标位置的偏差
	double dx_mm = (static_cast<int>(m_dstAxisData.xPos) - static_cast<int>(pos.xPos)) / 1000.0;
	double dy_mm = (static_cast<int>(m_dstAxisData.yPos) - static_cast<int>(pos.yPos)) / 1000.0;
	double dz_mm = (static_cast<int>(m_dstAxisData.zPos) - static_cast<int>(pos.zPos)) / 1000.0;
	
	double distance = sqrt(dx_mm*dx_mm + dy_mm*dy_mm + dz_mm*dz_mm);
	
	if (distance > 0.001)
	{  // 偏差大于1微米
		NAMED_LOG_T("logicMoudle", "位置数据 与目标位置偏差: {:.3f} mm  (X={:.3f}, Y={:.3f}, Z={:.3f}",
			distance,
			dx_mm,
			dy_mm,
			dz_mm);
	} 
	else 
	{
		NAMED_LOG_T("logicMoudle", "位置数据 已到达目标位置");
	}
	
	// 发送位置更新事件到上层
	SendEvent(EVENT_TYPE_MOVE_STATUS, 0, "Position updated", m_curAxisData.xPos, m_curAxisData.yPos, m_curAxisData.zPos);
	//SendEvent(EVENT_TYPE_POS_STATUS, 0, "Position updated", m_curAxisData.xPos, m_curAxisData.yPos, m_curAxisData.zPos);
}

