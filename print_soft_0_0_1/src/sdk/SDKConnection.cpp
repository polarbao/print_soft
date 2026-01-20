/**
 * @file SDKConnection.cpp
 * @brief 连接管理实现
 * @details 处理TCP连接、断开、状态查询等功能
 */

#include "SDKManager.h"
#include "TcpClient.h"
#include "ProtocolPrint.h"
#include <QTimer>
#include "SpdlogMgr.h"

// ==================== TCP连接管理 ====================

int SDKManager::Connect2MotionDev(const QString& ip, unsigned short port) 
{
    if (!m_initialized || !m_tcpClient) 
	{
        return -1;
    }
  
    // 设置IP和端口
    m_tcpClient->setIpAndPort(ip, port);
    
    // 发起连接
    m_tcpClient->connectToHost();
   
    return 0;
}

void SDKManager::Disconnect() 
{
    if (!m_tcpClient) 
	{
        return;
    }
    
    // 停止心跳定时器
    if (m_heartbeatSendTimer && m_heartbeatSendTimer->isActive()) 
	{
        m_heartbeatSendTimer->stop();
    }
    if (m_heartbeatCheckTimer && m_heartbeatCheckTimer->isActive()) 
	{
        m_heartbeatCheckTimer->stop();
    }
    
    // 断开TCP连接
    m_tcpClient->disconnectFromHost();
}

bool SDKManager::IsConnected() const 
{
    if (m_tcpClient) 
	{
        return m_tcpClient->isConnected();
    }
    return false;
}

