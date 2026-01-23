/**
 * @file SDKManager.cpp
 * @brief SDK管理器基础实现
 * @details 包含单例实现、初始化、资源管理和辅助函数
 */

#include <QTimer>
#include "SDKManager.h"
#include "TcpClient.h"
#include "ProtocolPrint.h"

// ==================== 单例实现 ====================

SDKManager* SDKManager::GetInstance()
{
	static SDKManager manager;
	return &manager;
}


// ==================== 构造和析构 ====================

SDKManager::SDKManager() 
    : m_initialized(false)
    , m_heartbeatTimeout(0) 
{
    // 私有构造函数
}

SDKManager::~SDKManager() 
{
    Release();
}

// ==================== 生命周期管理 ====================

bool SDKManager::Init(const QString& log_dir) 
{
    if (m_initialized) 
	{
        return true;  // 已经初始化
    }
    
    // 创建TCP客户端和协议处理器
    m_tcpClient = std::make_unique<TcpClient>();
    m_protocol = std::make_unique<ProtocolPrint>();
    
    // 连接TCP客户端信号
    connect(m_tcpClient.get(), &TcpClient::sigNewData, this, &SDKManager::OnRecvData);
    connect(m_tcpClient.get(), &TcpClient::sigError, this, &SDKManager::OnTcpError);
    connect(m_tcpClient.get(), &TcpClient::sigSocketStateChanged, this, &SDKManager::OnStateChanged);
    
    // 连接协议处理器信号
	connect(m_protocol.get(), &ProtocolPrint::SigHeartBeat, this, &SDKManager::OnHeartbeat);
	connect(m_protocol.get(), &ProtocolPrint::SigCmdReply, this, &SDKManager::OnCmdReply);
	connect(m_protocol.get(), &ProtocolPrint::SigPackFailRetransport, this, &SDKManager::OnFaileHandleReTransport);
	connect(m_protocol.get(), &ProtocolPrint::SigHandleRespFunOper, this, &SDKManager::OnHandleRecvFunOper);
	connect(m_protocol.get(), &ProtocolPrint::SigHandleAxisPosData, this, &SDKManager::OnHandleRecvDataOper);
    
    // 初始化心跳定时器
    m_heartbeatSendTimer = std::make_unique<QTimer>();
    m_heartbeatCheckTimer = std::make_unique<QTimer>();
    
    //m_heartbeatSendTimer->setInterval(2000);  // 2秒发送一次心跳
    //m_heartbeatCheckTimer->setInterval(5000);   // 5秒检查心跳超时
    
    connect(m_heartbeatSendTimer.get(), &QTimer::timeout, this, &SDKManager::OnSendHeartbeat);
    connect(m_heartbeatCheckTimer.get(), &QTimer::timeout, this, &SDKManager::OnCheckHeartbeat);
    
    m_initialized = true;
	//SendEvent(EVENT_TYPE_SEND_MSG, 0, packet.toHex().toUpper().constData());
    return true;
}

void SDKManager::Release() 
{
    if (!m_initialized) 
	{
        return;  // 未初始化，无需释放
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
    
    // 断开连接
    if (m_tcpClient) 
	{
        m_tcpClient->disconnectFromHost();
    }
    
    // 清理资源
    m_heartbeatSendTimer.reset();
    m_heartbeatCheckTimer.reset();
    m_protocol.reset();
    m_tcpClient.reset();
    
    m_initialized = false;
}

// ==================== 辅助函数 ====================

//fc + dataArr
void SDKManager::SendCommand(int code, const QByteArray& data) 
{
    if (!m_tcpClient) 
	{
        return;
    }
    
	// 使用协议打包数据
	// 根据FunCode确定ECmdType
	auto fc = static_cast<ProtocolPrint::FunCode>(code);
	ProtocolPrint::ECmdType ct = ProtocolPrint::ECmdType::CtrlCmd; // 默认为控制命令

	// 根据FunCode范围确定命令类型
	if (fc >= ProtocolPrint::SetParam_CleanPos && fc <= ProtocolPrint::SetParam_End)
	{
		ct = ProtocolPrint::ECmdType::SetParamCmd;
	}
	else if (fc >= ProtocolPrint::Get_AxisPos && fc <= ProtocolPrint::Get_End)
	{
		ct = ProtocolPrint::ECmdType::GetCmd;
	}
	else if (fc >= ProtocolPrint::Ctrl_StartPrint && fc <= ProtocolPrint::Ctrl_End)
	{
		ct = ProtocolPrint::ECmdType::CtrlCmd;
	}
	else if (fc >= ProtocolPrint::Print_AxisMovePos && fc <= ProtocolPrint::Print_End)
	{
		ct = ProtocolPrint::ECmdType::PrintCommCmd;
	}

	QByteArray packet = ProtocolPrint::GetSendDatagram(ct, fc, data);
    
    // 发送数据
    m_tcpClient->sendData(packet);
	NAMED_LOG_D("netMoudle", "motion_sdk print_protocol_moudle cur_send_data: {}", packet.toHex().toUpper());
	SendEvent(EVENT_TYPE_SEND_MSG, 0, packet.toHex().toUpper().constData());

}

//重发数据
void SDKManager::SendCommand(const QByteArray& data /*= QByteArray()*/)
{
	// 重发失败数据
	m_tcpClient->sendData(data);
	SendEvent(EVENT_TYPE_SEND_MSG, 0, data.toHex().toUpper().constData());

}

//fc类型+坐标数据
void SDKManager::SendCommand(int code, const MoveAxisPos& posData)
{
	//数据处理
	QByteArray senddata;
	senddata.resize(12);
	senddata[0] = posData.xPos >> 0 & 0xFF;
	senddata[1] = posData.xPos >> 8 & 0xFF;
	senddata[2] = posData.xPos >> 16 & 0xFF;
	senddata[3] = posData.xPos >> 24 & 0xFF;

	senddata[4] = posData.yPos >> 0 & 0xFF;
	senddata[5] = posData.yPos >> 8 & 0xFF;
	senddata[6] = posData.yPos >> 16 & 0xFF;
	senddata[7] = posData.yPos >> 24 & 0xFF;

	senddata[8] = posData.zPos >> 0 & 0xFF;
	senddata[9] = posData.zPos >> 8 & 0xFF;
	senddata[10] = posData.zPos >> 16 & 0xFF;
	senddata[11] = posData.zPos >> 24 & 0xFF;


	// 根据FunCode确定ECmdType
	auto fc = static_cast<ProtocolPrint::FunCode>(code);
	ProtocolPrint::ECmdType ct = ProtocolPrint::ECmdType::CtrlCmd; // 默认为控制命令

	// 根据FunCode范围确定命令类型
	if (fc >= ProtocolPrint::SetParam_CleanPos && fc <= ProtocolPrint::SetParam_End)
	{
		ct = ProtocolPrint::ECmdType::SetParamCmd;
	}
	else if (fc >= ProtocolPrint::Get_AxisPos && fc <= ProtocolPrint::Get_End)
	{
		ct = ProtocolPrint::ECmdType::GetCmd;
	}
	else if (fc >= ProtocolPrint::Ctrl_StartPrint && fc <= ProtocolPrint::Ctrl_End)
	{
		ct = ProtocolPrint::ECmdType::CtrlCmd;
	}
	else if (fc >= ProtocolPrint::Print_AxisMovePos && fc <= ProtocolPrint::Print_End)
	{
		ct = ProtocolPrint::ECmdType::PrintCommCmd;
	}

	// 使用协议打包数据
	QByteArray packet = ProtocolPrint::GetSendDatagram(ct, fc, senddata);
	m_tcpClient->sendData(packet);
	SendEvent(EVENT_TYPE_SEND_MSG, 0, packet.toHex().toUpper().constData());

}

void SDKManager::SendEvent(SdkEventType type, int code, const char* message, double v1, double v2, double v3) 
{
    QMutexLocker lock(&g_callbackMutex);
    
    if (!g_sdkCallback) 
	{
        return;  // 没有注册回调函数
    }
    
    // 保存消息到全局缓冲区以确保生命周期
    g_messageBuffer = QByteArray(message);
    
    // 构造事件结构
    SdkEvent event;
    event.type = type;
    event.code = code;
    event.message = g_messageBuffer.constData();
    event.value1 = v1;
    event.value2 = v2;
    event.value3 = v3;
    
    // 调用回调函数
    g_sdkCallback(&event);
}

