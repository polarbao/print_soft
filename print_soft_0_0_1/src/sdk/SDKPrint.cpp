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

// ==================== 打印控制 ====================

int SDKManager::StartPrint()
{
    if (!IsConnected()) 
	{
        return -1;
    }
    
	SendCommand(ProtocolPrint::Ctrl_StartPrint);
    return 0;
}

int SDKManager::StopPrint() 
{
    if (!IsConnected()) 
	{
        return -1;
    }
    
	SendCommand(ProtocolPrint::Ctrl_PasusePrint);
    return 0;
}

int SDKManager::PausePrint() 
{
    if (!IsConnected()) 
	{
        return -1;
    }
    
   SendCommand(ProtocolPrint::Ctrl_StopPrint);
    return 0;
}

int SDKManager::ResumePrint() 
{
    if (!IsConnected()) 
	{
        return -1;
    }
    
    SendCommand(ProtocolPrint::Ctrl_ContinuePrint);
    return 0;
}

int SDKManager::ResetPrint() 
{
    if (!IsConnected()) 
	{
        return -1;
    }
    
    SendCommand(ProtocolPrint::Ctrl_ResetPos);
    return 0;
}
