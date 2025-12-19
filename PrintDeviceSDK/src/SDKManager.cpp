/**
 * @file SDKManager.cpp
 * @brief SDK管理器基础实现
 * @details 包含单例实现、初始化、资源管理和辅助函数
 */

#include "SDKManager.h"
#include "../../print_soft_0_0_1/src/communicate/TcpClient.h"
#include "../../print_soft_0_0_1/src/protocol/ProtocolPrint.h"
#include "../../print_soft_0_0_1/src/comm/CLogManager.h"
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

bool SDKManager::init(const char* log_dir) {
    if (m_initialized) {
        return true;  // 已经初始化
    }
    
    // 初始化日志系统
    if (log_dir && strlen(log_dir) > 0) {
        // CLogManager可以在这里初始化，如果需要的话
        // CLogManager::instance()->init(log_dir);
    }

    // 创建TCP客户端和协议处理器
    m_tcpClient = std::make_unique<TcpClient>();
    m_protocol = std::make_unique<ProtocolPrint>();
    
    // 连接TCP客户端信号
    connect(m_tcpClient.get(), &TcpClient::sigNewData, 
            this, &SDKManager::onRecvData);
    connect(m_tcpClient.get(), &TcpClient::sigError, 
            this, &SDKManager::onTcpError);
    connect(m_tcpClient.get(), &TcpClient::sigSocketStateChanged, 
            this, &SDKManager::onStateChanged);
    
    // 连接协议处理器信号
    connect(m_protocol.get(), &ProtocolPrint::SigHeartBeat, 
            this, &SDKManager::onHeartbeat);
    connect(m_protocol.get(), &ProtocolPrint::SigCmdReply, 
            this, &SDKManager::onCmdReply);
    
    // 设置协议的串口（实际上是TCP客户端）
    m_protocol->SetSerialPort(m_tcpClient.get());
    
    // 初始化心跳定时器
    m_heartbeatSendTimer = std::make_unique<QTimer>();
    m_heartbeatCheckTimer = std::make_unique<QTimer>();
    
    m_heartbeatSendTimer->setInterval(10000);  // 10秒发送一次心跳
    m_heartbeatCheckTimer->setInterval(5000);   // 5秒检查心跳超时
    
    connect(m_heartbeatSendTimer.get(), &QTimer::timeout, 
            this, &SDKManager::onSendHeartbeat);
    connect(m_heartbeatCheckTimer.get(), &QTimer::timeout, 
            this, &SDKManager::onCheckHeartbeat);
    
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

void SDKManager::sendCommand(int code, const QByteArray& data) {
    if (!m_tcpClient) {
        return;
    }
    
    // 使用协议打包数据
    QByteArray packet = ProtocolPrint::GetSendDatagram(
        static_cast<ProtocolPrint::FunCode>(code), data);
    
    // 发送数据
    m_tcpClient->sendData(packet);
}

void SDKManager::sendEvent(SdkEventType type, int code, const char* message, 
                          double v1, double v2, double v3) {
    QMutexLocker lock(&g_callbackMutex);
    
    if (!g_sdkCallback) {
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

