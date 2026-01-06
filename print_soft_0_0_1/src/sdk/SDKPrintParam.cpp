/**
 * @file SDKPrint.cpp
 * @brief 打印控制实现
 * @details 处理打印相关功能：开始、停止、暂停、恢复、图像加载等
 */

#include "SDKManager.h"
#include "TcpClient.h"
#include "ProtocolPrint.h"

#include <QFile>
#include <QImage>
#include <SpdlogMgr.h>
#include <MoveAxisPosFormat.h>

// ==================== 打印参数设置 ====================


int SDKManager::SetPrintStartPos(const MoveAxisPos& startPos)
{
    if (!IsConnected()) 
	{
        return -1;
    }


	NAMED_LOG_D("logicMoudle", "SetPrintStartPos => {}", startPos);
	SendCommand(ProtocolPrint::SetParam_PrintStartPos, startPos);
    return 0;
}

int SDKManager::SetPrintEndPos(const MoveAxisPos& endPos)
{
    if (!IsConnected()) 
	{
        return -1;
    }
    
	NAMED_LOG_D("logicMoudle", "SetPrintEndPos => {}", endPos);
	SendCommand(ProtocolPrint::SetParam_PrintEndPos, endPos);
	return 0;
}

int SDKManager::SetPrintCleanPos(const MoveAxisPos& cleanPos)
{
	if (!IsConnected())
	{
		return -1;
	}

	NAMED_LOG_D("logicMoudle", "SetPrintCleanPos => {}", cleanPos);
	SendCommand(ProtocolPrint::SetParam_CleanPos, cleanPos);
	return 0;
}

int SDKManager::SetAxisSpd(const MoveAxisPos& speed)
{
	if (!IsConnected())
	{
		return -1;
	}

	NAMED_LOG_D("logicMoudle", "SetAxisSpd => {}", speed);
	SendCommand(ProtocolPrint::SetParam_AxistSpd, speed);
	return 0;
}

int SDKManager::SetAxisUnitStep(const MoveAxisPos& step)
{
	if (!IsConnected())
	{
		return -1;
	}

	NAMED_LOG_D("logicMoudle", "SetAxisUnitStep => {}", step);
	SendCommand(ProtocolPrint::SetParam_AxisUnitMove, step);
	return 0;
}