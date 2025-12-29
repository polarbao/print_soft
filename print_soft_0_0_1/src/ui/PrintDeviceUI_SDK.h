/**
 * @file PrintDeviceUI_SDK.h
 * @brief 使用PrintDeviceSDK的打印设备UI类
 * @details 这是原PrintDeviceUI的SDK版本，使用封装的SDK库而不是直接使用TcpClient
 */

#pragma once

#include <QDialog>
#include <QMutex>
#include <QTimer>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QButtonGroup>
#include <QString>
#include <QDateTime>

// 包含SDK头文件
#include "../../../PrintDeviceSDK/PrintDeviceSDK_API.h"

/**
 * @class PrintDeviceUI_SDK
 * @brief 打印设备控制界面（SDK版本）
 * 
 * 使用PrintDeviceSDK进行设备控制，不直接使用TcpClient和ProtocolPrint
 */
class PrintDeviceUI_SDK : public QDialog
{
    Q_OBJECT

public:
    // 网络功能枚举
    enum NetFun
    {
        ENF_Begin,
        ENF_StartConn,
        ENF_DisConn,
        ENF_ChangePort,
        ENF_ClearAllText,
        ENF_End
    };

    // 打印功能枚举
    enum PrintFun
    {
        EPF_Begin = ENF_End + 1,
        EPF_StartPrint,
        EPF_StopPrint,
        EPF_PausePrint,
        EPF_ContinuePrint,
        EPF_TransData,
        EPF_ResetPrint,
        EPF_End
    };

    // 轴功能枚举
    enum AxisFun
    {
        EAF_Begin = EPF_End + 1,
        // Z轴控制
        EAF_ZAxisAutoForward,
        EAF_ZAxisAutoBack,
        EAF_ZAxisForward1CM,
        EAF_ZAxisBack1CM,
        EAF_ZAxisReset,
        EAF_ZAxisMovePrintPos,
        // X轴控制
        EAF_XAxisReset,
        EAF_XAxisMovePrintPos,
        // Y轴控制
        EAF_YAxisReset,
        EAF_YAxisMovePrintPos,
        EAF_End
    };

    // 显示文本类型枚举
    enum ShowEditType
    {
        ESET_OperComm = 0x01,   // 操作命令
        ESET_SendComm = 0x02,   // 发送命令
        ESET_RecvComm = 0x03    // 接收命令
    };

    PrintDeviceUI_SDK(QWidget *parent = nullptr);
    ~PrintDeviceUI_SDK();

private:
    // 初始化函数
    void Init();
    void InitUI();
    void InitSDK();
    
    // SDK事件处理
    static void OnSDKEvent(const SdkEvent* event);
    static PrintDeviceUI_SDK* s_instance;  // 静态实例指针，用于在回调中访问

private slots:
    // 按钮点击处理
    void OnPrintFunClicked(int idx);
    void OnListenBtnClicked(int idx);
    
    // 显示消息
    void OnAppendShowComm(const QString& msg, const ShowEditType& type);

signals:
    // 用于从静态回调函数发送信号到UI线程
    void SigSDKEvent(const QString& message, int type, int code);
    void SigShowOperComm(const QString& msg, const ShowEditType& type);

private:
    // UI控件
    QLineEdit* m_ipLine;
    QLineEdit* m_portLine;
    QLabel* m_connStateLab;
    QTextEdit* m_operLogText;
    QTextEdit* m_sendCommText;
    QTextEdit* m_recvCommText;
    
    QButtonGroup* m_printBtnGroup;
    QButtonGroup* m_funBtnGroup;
    
    // SDK初始化标志
    bool m_sdkInitialized;
};

