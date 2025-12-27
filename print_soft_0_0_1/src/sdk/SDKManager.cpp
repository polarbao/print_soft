/**
 * @file SDKManager.cpp
 * @brief SDK管理器基础实现
 * @details 包含单例实现、初始化、资源管理和辅助函数
 */

#include "SDKManager.h"
#include "TcpClient.h"
#include "ProtocolPrint.h"
#include "CLogManager.h"
#include <QTimer>

// ==================== 单例实现 ====================

SDKManager* SDKManager::instance() {
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

SDKManager::~SDKManager() {
    release();
}

// ==================== 生命周期管理 ====================

bool SDKManager::init(const QString& log_dir) 
{
    if (m_initialized) 
	{
        return true;  // 已经初始化
    }
    
    // 初始化日志系统
    if (log_dir.isEmpty())
	{
		CLogManager::getInstance()->startLog("./");
    }
	else
	{
		CLogManager::getInstance()->startLog(log_dir);
	}
	LOG_INFO(QString(u8"motion_moudle_sdk_init"));


    // 创建TCP客户端和协议处理器
    m_tcpClient = std::make_unique<TcpClient>();
    m_protocol = std::make_unique<ProtocolPrint>();
    
    // 连接TCP客户端信号
    connect(m_tcpClient.get(), &TcpClient::sigNewData, this, &SDKManager::onRecvData);
    connect(m_tcpClient.get(), &TcpClient::sigError, this, &SDKManager::onTcpError);
    connect(m_tcpClient.get(), &TcpClient::sigSocketStateChanged, this, &SDKManager::onStateChanged);
    
    // 连接协议处理器信号
    connect(m_protocol.get(), &ProtocolPrint::SigHeartBeat, this, &SDKManager::onHeartbeat);
    connect(m_protocol.get(), &ProtocolPrint::SigCmdReply, this, &SDKManager::onCmdReply);
	connect(m_protocol.get(), &ProtocolPrint::SigPackFailRetransport, this, &SDKManager::onHeartbeat);
	connect(m_protocol.get(), &ProtocolPrint::SigHandleFunOper1, this, &SDKManager::onHandleRecvFunOper);
	connect(m_protocol.get(), &ProtocolPrint::SigHandleFunOper2, this, &SDKManager::onHandleRecvDataOper);

    
    // 设置协议的串口（实际上是TCP客户端）
   //m_protocol->SetSerialPort(m_tcpClient.get());
    
    // 初始化心跳定时器
    m_heartbeatSendTimer = std::make_unique<QTimer>();
    m_heartbeatCheckTimer = std::make_unique<QTimer>();
    
    m_heartbeatSendTimer->setInterval(2000);  // 2秒发送一次心跳
    m_heartbeatCheckTimer->setInterval(5000);   // 5秒检查心跳超时
    
    connect(m_heartbeatSendTimer.get(), &QTimer::timeout, this, &SDKManager::onSendHeartbeat);
    connect(m_heartbeatCheckTimer.get(), &QTimer::timeout, this, &SDKManager::onCheckHeartbeat);
    
    m_initialized = true;
    return true;
}

void SDKManager::release() {
    if (!m_initialized) {
        return;  // 未初始化，无需释放
    }
    
    // 停止心跳定时器
    if (m_heartbeatSendTimer && m_heartbeatSendTimer->isActive()) {
        m_heartbeatSendTimer->stop();
    }
    if (m_heartbeatCheckTimer && m_heartbeatCheckTimer->isActive()) {
        m_heartbeatCheckTimer->stop();
    }
    
    // 断开连接
    if (m_tcpClient) {
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
void SDKManager::sendCommand(int code, const QByteArray& data) 
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
	sendEvent(EVENT_TYPE_SEND_MSG, 0, packet.toHex().constData());

}

//重发数据
void SDKManager::sendCommand(const QByteArray& data /*= QByteArray()*/)
{
	// 重发失败数据
	m_tcpClient->sendData(data);
	sendEvent(EVENT_TYPE_SEND_MSG, 0, data.toHex().constData());

}

//fc类型+坐标数据
void SDKManager::sendCommand(int code, const MoveAxisPos& posData)
{
	//数据处理
	//const int dataSize = 12;
	//uchar sendBuf[dataSize];
	//memset(sendBuf, 0, dataSize);
	//sendBuf[0] = posData.xPos >> 0 & 0xFF;
	//sendBuf[1] = posData.xPos >> 8 & 0xFF;
	//sendBuf[2] = posData.xPos >> 16 & 0xFF;
	//sendBuf[3] = posData.xPos >> 24 & 0xFF;
	//sendBuf[4] = posData.yPos >> 0 & 0xFF;
	//sendBuf[5] = posData.yPos >> 8 & 0xFF;
	//sendBuf[6] = posData.yPos >> 16 & 0xFF;
	//sendBuf[7] = posData.yPos >> 24 & 0xFF;
	//sendBuf[8] = posData.zPos >> 0 & 0xFF;
	//sendBuf[9] = posData.zPos >> 8 & 0xFF;
	//sendBuf[10] = posData.zPos >> 16 & 0xFF;
	//sendBuf[11] = posData.zPos >> 24 & 0xFF;
	//QByteArray senddata;
	//senddata.resize(12);
	//memcpy(senddata.data(), sendBuf, 12);

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
	sendEvent(EVENT_TYPE_SEND_MSG, 0, packet.toHex().constData());

}

void SDKManager::sendEvent(SdkEventType type, int code, const char* message, double v1, double v2, double v3) 
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

