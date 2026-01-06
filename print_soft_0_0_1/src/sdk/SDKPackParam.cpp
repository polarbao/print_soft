/**
 * @file SDKPackParam.cpp
 * @brief PackParam数据包处理实现
 * @details 处理协议解析后的PackParam结构体，实现业务逻辑分发
 * @date 2025-12-23
 */

#include "SDKManager.h"
#include "protocol/ProtocolPrint.h"
#include "CLogManager.h"
#include "SpdlogMgr.h"
#include <QString>

// ==================== PackParam主处理函数 ====================

/**
 * @brief 处理PackParam数据包（槽函数）
 * @param packData 从ProtocolPrint解析出的数据包
 * 
 * 功能：接收协议层解析的PackParam结构体，根据操作类型分发到具体处理函数
 */
void SDKManager::OnHandleRecvFunOper(const PackParam& packData)
{
   NAMED_LOG_T("netMoudle","收到PackParam数据包  ");
   NAMED_LOG_T("netMoudle","包头(head):0x{:04X}", QString::number(packData.head, 16).toUpper().rightJustified(4, '0'));
   NAMED_LOG_T("netMoudle", "操作类型(operType):0x{:02X}", QString::number(packData.operType, 16).toUpper().rightJustified(4, '0'));
   NAMED_LOG_T("netMoudle", "命令功能码(cmdFun):0x{:02X}", QString::number(packData.cmdFun, 16).toUpper().rightJustified(4, '0'));
   NAMED_LOG_T("netMoudle","数据长度(dataLen):{}字节", packData.dataLen);
    
    // 打印数据域（前12字节，十六进制）
    QString dataHex;
    for (int i = 0; i < packData.dataLen && i < DATA_LEN_12; ++i) 
	{
        dataHex += QString("%1 ").arg(packData.data[i], 2, 16, QChar('0')).toUpper();
    }
    if (!dataHex.isEmpty()) 
	{
       NAMED_LOG_T("netMoudle","数据域(data): {}", dataHex.trimmed());
    }
   NAMED_LOG_T("netMoudle","------------------------------------------");
    
    // 保存到成员变量（如果需要保留最后一次的数据）
    m_curParam = packData;
    
    // 根据操作类型分发到不同的处理函数
    switch (packData.operType)
    {
    case ProtocolPrint::SetParamCmd:  // 0x0001 - 设置参数命令应答
        HandleSetParamResponse(packData);
        break;
        
    case ProtocolPrint::GetCmd:  // 0x0010 - 获取命令应答
        HandleGetCmdResponse(packData);
        break;
        
    case ProtocolPrint::CtrlCmd:  // 0x0011 - 控制命令应答
        HandleCtrlCmdResponse(packData);
        break;
        
    case ProtocolPrint::PrintCommCmd:  // 0x00F0 - 打印通信命令应答
        HandlePrintCommCmdResponse(packData);
        break;
        
    default:
		NAMED_LOG_T("netMoudle"," 未知操作类型: 0x{:04X}", QString::number(packData.operType, 16).toUpper());
        sendEvent(EVENT_TYPE_ERROR, -1, "Unknown operation type in PackParam");
        break;
    }
}

// ==================== 分类处理函数 ====================

/**
 * @brief 处理设置参数命令的应答
 * @param packData 数据包参数
 * 
 * 处理命令：
 * - 0x1000: SetParam_CleanPos (设置清洁位置)
 * - 0x1001: SetParam_PrintStartPos (设置打印起始位置)
 * - 0x1002: SetParam_PrintEndPos (设置打印结束位置)
 * - 0x1010: SetParam_AxisUnitMove (设置轴单位移动)
 * - 0x1020: SetParam_MaxLimitPos (设置最大限位位置)
 * - 0x1030: SetParam_AxistSpd (设置轴速度)
 */
void SDKManager::HandleSetParamResponse(const PackParam& packData)
{
   NAMED_LOG_T("netMoudle","设置参数 命令应答: 0x{:04X}", QString::number(packData.cmdFun, 16).toUpper());
    
    QString message;
    switch (packData.cmdFun)
    {
    case ProtocolPrint::SetParam_CleanPos:
	{
		message = QString(u8"清洁位置设置成功");
		break;
	}
        
    case ProtocolPrint::SetParam_PrintStartPos:
	{
		message = QString(u8"打印起始位置设置成功");
		break;
	}
        
    case ProtocolPrint::SetParam_PrintEndPos:
	{
		message = QString(u8"打印结束位置设置成功");
		break;
	}
        
    case ProtocolPrint::SetParam_AxisUnitMove:
	{
		//message = QString(u8"轴单位移动参数设置成功");
		break;
	}
        
    case ProtocolPrint::SetParam_MaxLimitPos:
	{
		//message = QString("最大限位位置设置成功");
		break;
	}
        
    case ProtocolPrint::SetParam_AxistSpd:
	{
		message = QString(u8"轴速度设置成功");
		break;
	}
        
    default:
        message = QString("参数设置完成 (命令码: 0x%1)")
            .arg(QString::number(packData.cmdFun, 16).toUpper());
        break;
    }
    
   NAMED_LOG_T("netMoudle","当前设置参数数据：{}", message);
   sendEvent(EVENT_TYPE_GENERAL, 0, message.toUtf8().constData());
}

/**
 * @brief 处理获取命令的应答
 * @param packData 数据包参数
 * 
 * 处理命令：
 * - 0x2000: Get_AxisPos (获取轴位置)
 * - 0x2010: Get_Breath (心跳)
 */
void SDKManager::HandleGetCmdResponse(const PackParam& packData)
{
   NAMED_LOG_T("netMoudle","获取命令 命令应答: 0x{:04X}", QString::number(packData.cmdFun, 16).toUpper());
    
    switch (packData.cmdFun)
    {
    case ProtocolPrint::Get_AxisPos:  // 获取轴位置
    {
        // 验证数据长度（假设位置数据为12字节：X(4) Y(4) Z(4)）
        if (packData.dataLen < 12) {
           NAMED_LOG_T("netMoudle","位置数据长度不足: 期望12字节，实际{}字节", packData.dataLen);
            sendEvent(EVENT_TYPE_ERROR, -1, "Invalid axis position data length");
            break;
        }
        
        // 解析轴位置数据（大端序，单位假设为0.01mm）
        uint32_t xPos = (static_cast<uint32_t>(packData.data[0]) << 24) | (static_cast<uint32_t>(packData.data[1]) << 16) | 
                       (static_cast<uint32_t>(packData.data[2]) << 8) | static_cast<uint32_t>(packData.data[3]);
                        
        uint32_t yPos = (static_cast<uint32_t>(packData.data[4]) << 24) | (static_cast<uint32_t>(packData.data[5]) << 16) | 
                       (static_cast<uint32_t>(packData.data[6]) << 8) | static_cast<uint32_t>(packData.data[7]);
                        
        uint32_t zPos = (static_cast<uint32_t>(packData.data[8]) << 24) | (static_cast<uint32_t>(packData.data[9]) << 16) | 
                       (static_cast<uint32_t>(packData.data[10]) << 8) | static_cast<uint32_t>(packData.data[11]);
        
        // 更新当前位置到成员变量
        m_curAxisData.xPos = xPos;
        m_curAxisData.yPos = yPos;
        m_curAxisData.zPos = zPos;
        
        // 转换为毫米单位（假设原始单位是0.01mm）
        double xMM = static_cast<double>(xPos) / 100.0;
        double yMM = static_cast<double>(yPos) / 100.0;
        double zMM = static_cast<double>(zPos) / 100.0;
        
		NAMED_LOG_T("netMoudle", " 当前轴位置: X={:.3f} mm, Y={:.3f} mm, Z={:.3f} mm", xMM, yMM, zMM);
        
        // 发送位置更新事件到上层
        sendEvent(EVENT_TYPE_MOVE_STATUS, 0, "Position updated", xMM, yMM, zMM);
        break;
    }
    
    case ProtocolPrint::Get_Breath:  // 心跳
        // 心跳应答已在SDKCallback.cpp的onHeartbeat()中处理
       NAMED_LOG_T("netMoudle"," 心跳应答（已在onHeartbeat中处理）");
        break;
        
    default:
       NAMED_LOG_T("netMoudle","其他获取命令应答: 0x{:04X}", QString::number(packData.cmdFun, 16).toUpper());
        break;
    }
}

/**
 * @brief 处理控制命令的应答
 * @param packData 数据包参数
 * 
 * 处理命令：
 * - 0x3000: Ctrl_StartPrint (开始打印)
 * - 0x3001: Ctrl_PasusePrint (暂停打印)
 * - 0x3002: Ctrl_ContinuePrint (继续打印)
 * - 0x3003: Ctrl_StopPrint (停止打印)
 * - 0x3004: Ctrl_ResetPos (复位)
 * - 0x3101~0x3106: 轴移动命令
 * - 0x3107: Ctrl_AxisAbsMove (绝对移动)
 */
void SDKManager::HandleCtrlCmdResponse(const PackParam& packData)
{
   NAMED_LOG_T("netMoudle","控制命令 命令应答: 0x{:04X}", QString::number(packData.cmdFun, 16).toUpper());
    
    QString message;
    SdkEventType eventType = EVENT_TYPE_GENERAL;
    
    switch (packData.cmdFun)
    {
    // 打印控制命令
    case ProtocolPrint::Ctrl_StartPrint:
	{
		//message = QString(u8"打印已启动");
		eventType = EVENT_TYPE_PRINT_STATUS;
		break;
	}
        
    case ProtocolPrint::Ctrl_PasusePrint:
	{
		//message = "打印已暂停";
		eventType = EVENT_TYPE_PRINT_STATUS;
		break;
	}
        
    case ProtocolPrint::Ctrl_ContinuePrint:
	{
		//message = "打印已恢复";
		eventType = EVENT_TYPE_PRINT_STATUS;
		break;
	}
        
    case ProtocolPrint::Ctrl_StopPrint:
	{
		//message = "打印已停止";
		eventType = EVENT_TYPE_PRINT_STATUS;
		break;

	}
    // 轴控制命令
    case ProtocolPrint::Ctrl_ResetPos:
	{
		//message = "轴复位完成";
		eventType = EVENT_TYPE_MOVE_STATUS;
		break;
	}
        
    case ProtocolPrint::Ctrl_XAxisLMove:
	{
		//message = "X轴向左移动完成";
		eventType = EVENT_TYPE_MOVE_STATUS;
		break;
	}
        
    case ProtocolPrint::Ctrl_XAxisRMove:
	{
		//message = "X轴向右移动完成";
		eventType = EVENT_TYPE_MOVE_STATUS;
		break;
	}
        
    case ProtocolPrint::Ctrl_YAxisLMove:
	{
		//message = "Y轴向左移动完成";
		eventType = EVENT_TYPE_MOVE_STATUS;
		break;
	}
        
    case ProtocolPrint::Ctrl_YAxisRMove:
	{
		//message = "Y轴向右移动完成";
		eventType = EVENT_TYPE_MOVE_STATUS;
		break;
	}
        
    case ProtocolPrint::Ctrl_ZAxisLMove:
	{
		//message = "Z轴向上移动完成";
		eventType = EVENT_TYPE_MOVE_STATUS;
		break;
	}
        
    case ProtocolPrint::Ctrl_ZAxisRMove:
	{
		//message = "Z轴向下移动完成";
		eventType = EVENT_TYPE_MOVE_STATUS;
		break;
	}
        
    case ProtocolPrint::Ctrl_AxisAbsMove:
	{
		//message = "轴绝对移动完成";
		eventType = EVENT_TYPE_MOVE_STATUS;
		break;
	}
        
    default:
        message = QString("控制命令执行完成 (命令码: 0x%1)").arg(QString::number(packData.cmdFun, 16).toUpper());
        break;
    }
    
	NAMED_LOG_T("netMoudle","当前设置命令 %1", message);
	sendEvent(eventType, 0, message.toUtf8().constData());
}

/**
 * @brief 处理打印通信命令的应答
 * @param packData 数据包参数
 * 
 * 处理命令：
 * - 0xF000: Print_AxisMovePos (打印轴移动位置)
 * - 0xF0001: Print_PeriodData (周期数据)
 */
void SDKManager::HandlePrintCommCmdResponse(const PackParam& packData)
{
   NAMED_LOG_T("netMoudle","打印通信 命令应答: 0x{:04X}", QString::number(packData.cmdFun, 16).toUpper());
    
    switch (packData.cmdFun)
    {
    case ProtocolPrint::Print_AxisMovePos:
    {
        //LOG_INFO(QString(u8"打印过程轴移动位置更新"));
        
        // 解析打印过程中的轴移动位置（根据实际协议定义）
        // TODO: 根据实际协议解析数据
        // 示例：
        // if (packData.dataLen >= 12) {
        //     解析Y轴和Z轴位置
        //     sendEvent(EVENT_TYPE_PRINT_STATUS, 0, "Printing position updated", y, z);
        // }
        break;
    }
    
    case ProtocolPrint::Print_PeriodData:
    {
		NAMED_LOG_T("netMoudle","周期数据 已在HandlePeriodData中处理）");
        // 周期数据已在ProtocolPrint::HandlePeriodData()中处理
        break;
    }
    
    default:
		NAMED_LOG_T("netMoudle","其他打印通信命令: 0x{:04X}", QString::number(packData.cmdFun, 16).toUpper());
        break;
    }
}


