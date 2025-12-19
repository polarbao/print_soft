/**
 * @file SDKMotion.cpp
 * @brief 运动控制实现
 * @details 处理X/Y/Z轴的移动和复位功能
 */

#include "SDKManager.h"
#include "../../print_soft_0_0_1/src/protocol/ProtocolPrint.h"

// ==================== 运动控制 ====================

int SDKManager::moveXAxis(double distance, bool isAbsolute) {
    if (!isConnected()) {
        return -1;
    }
    
    // 根据距离选择命令
    // X轴在原项目中没有自动移动，这里使用复位和移动到打印位置
    if (distance > 0) {
        sendCommand(ProtocolPrint::Set_XAxisMovePrintPos);
    } else {
        sendCommand(ProtocolPrint::Set_XAxisReset);
    }
    
    return 0;
}

int SDKManager::moveYAxis(double distance, bool isAbsolute) {
    if (!isConnected()) {
        return -1;
    }
    
    // Y轴控制
    if (distance > 0) {
        sendCommand(ProtocolPrint::Set_YAxisMovePrintPos);
    } else {
        sendCommand(ProtocolPrint::Set_YAxisReset);
    }
    
    return 0;
}

int SDKManager::moveZAxis(double distance, bool isAbsolute) {
    if (!isConnected()) {
        return -1;
    }
    
    // 根据距离选择命令
    if (distance > 0) {
        // 向上移动
        if (distance >= 10.0) {
            // 大于等于10mm，使用自动上升
            sendCommand(ProtocolPrint::Set_ZAxisUpAuto);
        } else {
            // 小于10mm，使用1CM移动
            sendCommand(ProtocolPrint::Set_ZAxisUp1CM);
        }
    } else {
        // 向下移动
        if (distance <= -10.0) {
            // 小于等于-10mm，使用自动下降
            sendCommand(ProtocolPrint::Set_ZAxisDownAuto);
        } else {
            // 大于-10mm，使用1CM移动
            sendCommand(ProtocolPrint::Set_ZAxisDown1CM);
        }
    }
    
    return 0;
}

int SDKManager::resetAxis(int axisFlag) {
    if (!isConnected()) {
        return -1;
    }
    
    // axisFlag位标志：
    // bit 0 (1) = X轴
    // bit 1 (2) = Y轴
    // bit 2 (4) = Z轴
    // 可以组合，如 7 = X+Y+Z
    
    if (axisFlag & 1) {
        // 复位X轴
        sendCommand(ProtocolPrint::Set_XAxisReset);
    }
    
    if (axisFlag & 2) {
        // 复位Y轴
        sendCommand(ProtocolPrint::Set_YAxisReset);
    }
    
    if (axisFlag & 4) {
        // 复位Z轴
        sendCommand(ProtocolPrint::Set_ZAxisReset);
    }
    
    return 0;
}

