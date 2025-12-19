/**
 * @file SDKCallback.cpp
 * @brief 事件回调处理实现
 * @details 处理所有信号槽回调，包括TCP事件、协议事件、心跳等
 */

#include "SDKManager.h"
#include "../../print_soft_0_0_1/src/communicate/TcpClient.h"
#include "../../print_soft_0_0_1/src/protocol/ProtocolPrint.h"
#include <QMutexLocker>
#include <QMetaObject>
#include <QString>

// ==================== 全局变量定义 ====================

SdkEventCallback g_sdkCallback = nullptr;
QMutex g_callbackMutex;
QByteArray g_messageBuffer;

// ==================== TCP事件处理 ====================

void SDKManager::onRecvData(QByteArray data) {
    if (m_protocol) {
        // 将接收到的数据交给协议处理器解码
        m_protocol->Decode(data);
    }
}

void SDKManager::onTcpError(QAbstractSocket::SocketError error) {
    // 构造错误消息
    QString errorMsg = QString("TCP Error: %1").arg(static_cast<int>(error));
    
    // 发送错误事件
    sendEvent(EVENT_TYPE_ERROR, static_cast<int>(error), 
              errorMsg.toUtf8().constData());
}

void SDKManager::onStateChanged(QAbstractSocket::SocketState state) {
    if (state == QAbstractSocket::ConnectedState) {
        // 连接成功
        sendEvent(EVENT_TYPE_GENERAL, 0, "Connected to device");
        
        // 启动心跳机制
        m_heartbeatTimeout = 0;
        if (m_heartbeatSendTimer) {
            m_heartbeatSendTimer->start();
        }
        if (m_heartbeatCheckTimer) {
            m_heartbeatCheckTimer->start();
        }
        
        // 发送第一个心跳包
        sendCommand(ProtocolPrint::Con_Breath);
        
    } else if (state == QAbstractSocket::UnconnectedState) {
        // 连接断开
        sendEvent(EVENT_TYPE_GENERAL, 0, "Disconnected from device");
        
        // 停止心跳定时器
        if (m_heartbeatSendTimer) {
            m_heartbeatSendTimer->stop();
        }
        if (m_heartbeatCheckTimer) {
            m_heartbeatCheckTimer->stop();
        }
    } else if (state == QAbstractSocket::ConnectingState) {
        // 正在连接
        sendEvent(EVENT_TYPE_GENERAL, 0, "Connecting to device...");
    }
}

// ==================== 协议事件处理 ====================

void SDKManager::onHeartbeat() {
    // 收到心跳应答
    QMutexLocker lock(&m_heartbeatMutex);
    
    // 重置心跳超时计数
    m_heartbeatTimeout = 0;
    
    // 发送心跳接收事件（可选，用于调试）
    // sendEvent(EVENT_TYPE_GENERAL, 0, "Heartbeat received");
}

void SDKManager::onCmdReply(int cmd, uchar errCode, QByteArray arr) {
    // 命令应答处理
    QString msg = QString("Command reply: 0x%1, Error: %2")
        .arg(cmd, 0, 16)
        .arg(errCode);
    
    if (errCode != 0) {
        // 命令执行出错
        sendEvent(EVENT_TYPE_ERROR, errCode, msg.toUtf8().constData());
    } else {
        // 命令执行成功
        sendEvent(EVENT_TYPE_GENERAL, cmd, msg.toUtf8().constData());
    }
}

// ==================== 心跳机制 ====================

void SDKManager::onSendHeartbeat() {
    // 定时发送心跳
    if (isConnected()) {
        sendCommand(ProtocolPrint::Con_Breath);
    }
}

void SDKManager::onCheckHeartbeat() {
    // 检查心跳超时
    QMutexLocker lock(&m_heartbeatMutex);
    
    m_heartbeatTimeout++;
    
    if (m_heartbeatTimeout > 3) {
        // 心跳超时（连续3次未收到应答）
        sendEvent(EVENT_TYPE_ERROR, -1, 
                  "Heartbeat timeout, disconnecting...");
        
        // 异步断开连接（避免在信号处理中直接操作可能导致的问题）
        QMetaObject::invokeMethod(this, [this]() {
            disconnect();
        }, Qt::QueuedConnection);
    }
}

