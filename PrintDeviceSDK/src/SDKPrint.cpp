/**
 * @file SDKPrint.cpp
 * @brief 打印控制实现
 * @details 处理打印相关功能：开始、停止、暂停、恢复、图像加载等
 */

#include "SDKManager.h"
#include "../../print_soft_0_0_1/src/communicate/TcpClient.h"
#include "../../print_soft_0_0_1/src/protocol/ProtocolPrint.h"
#include <QFile>
#include <QImage>

// ==================== 打印控制 ====================

int SDKManager::startPrint() {
    if (!isConnected()) {
        return -1;
    }
    
    sendCommand(ProtocolPrint::Set_StartPrint);
    return 0;
}

int SDKManager::stopPrint() {
    if (!isConnected()) {
        return -1;
    }
    
    sendCommand(ProtocolPrint::Set_StopPrint);
    return 0;
}

int SDKManager::pausePrint() {
    if (!isConnected()) {
        return -1;
    }
    
    sendCommand(ProtocolPrint::Set_PausePrint);
    return 0;
}

int SDKManager::resumePrint() {
    if (!isConnected()) {
        return -1;
    }
    
    sendCommand(ProtocolPrint::Set_continuePrint);
    return 0;
}

int SDKManager::resetPrint() {
    if (!isConnected()) {
        return -1;
    }
    
    sendCommand(ProtocolPrint::Set_ResetPrint);
    return 0;
}

int SDKManager::loadImageData(const QString& imagePath) {
    if (!isConnected()) {
        return -1;
    }
    
    // 加载图像文件
    QImage img(imagePath);
    if (img.isNull()) {
        sendEvent(EVENT_TYPE_ERROR, -1, "Failed to load image");
        return -1;
    }
    
    // 读取原始文件数据
    QFile file(imagePath);
    if (!file.open(QIODevice::ReadOnly)) {
        sendEvent(EVENT_TYPE_ERROR, -1, "Failed to open image file");
        return -1;
    }
    
    QByteArray rawData = file.readAll().toHex();
    file.close();
    
    // 根据文件扩展名确定图像类型
    quint8 imgType = 0x01; // 默认JPG
    if (imagePath.endsWith(".png", Qt::CaseInsensitive)) {
        imgType = 0x02;  // PNG
    } else if (imagePath.endsWith(".bmp", Qt::CaseInsensitive)) {
        imgType = 0x03;  // BMP
    } else if (imagePath.endsWith(".raw", Qt::CaseInsensitive)) {
        imgType = 0x04;  // RAW
    }
    
    // 使用协议打包图像数据
    // 图像会被分包成多个数据包
    QList<QByteArray> packets = ProtocolPrint::GetSendImgDatagram(
        img.width(), img.height(), imgType, rawData);
    
    // 发送所有数据包
    for (const auto& packet : packets) {
        m_tcpClient->sendData(packet);
    }
    
    // 发送成功事件
    QString msg = QString("Image data sent: %1 packets, size: %2x%3")
        .arg(packets.size())
        .arg(img.width())
        .arg(img.height());
    sendEvent(EVENT_TYPE_GENERAL, 0, msg.toUtf8().constData());
    
    return 0;
}

