#pragma once

#include "QtWidgets/QtWidgets"
#include <QMutex>
#include <QTimer>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QButtonGroup>
#include <memory>


#include "global.h"

#include "motioncontrolsdk_define.h"
#include "motionControlSDK.h"


namespace UI { class PrintDeviceUI; }

class motionControlSDK;

class PrintDeviceUI : public QDialog
{
    Q_OBJECT

public:
	enum NetFun
	{
		ENF_Begin,
		ENF_StartConn,
		ENF_DisConn,
		ENF_ChangePort,
		ENF_ClearAllText,
		ENF_End
	};

	enum EPrintParamType
	{
		EPPT_Begin = ENF_End + 1,
		EPPT_SetStartPos,
		EPPT_SetStopPos,
		EPPT_SetStepX,
		EPPT_SetStepY,
		EPPT_SetStepZ,
		EPPT_SetClenaPos,
		EPPT_SetLimitXPos,
		EPPT_SetLimitYPos,
		EPPT_SetLimitZPos,
		EPPT_SetSpdX,
		EPPT_SetSpdY,
		EPPT_SetSpdZ,
		EPPT_End
	};


	enum EPrintOperFun
	{
		EPOF_Begin = EPPT_End + 1,
		EPOF_StartPrint,
		EPOF_StopPrint,
		EPOF_PausePrint,
		EPOF_ContinuePrint,
		EPOF_CleanPos,
		EPOF_GoHomnePos,
		EPOF_End
	};


	enum EPrintMoveOperFun
	{
		EPMOF_Begin = EPOF_End + 1,
		EPMOF_XAxisLMove,	//L为正方向，R为负方向
		EPMOF_XAxisRMove,
		EPMOF_YAxisLMove,
		EPMOF_YAxisRMove,
		EPMOF_ZAxisLMove,
		EPMOF_ZAxisRMove,
		EPMOF_End
	};


	enum ShowEditType
	{
		ESET_OperComm = 0x01,		// 操作命令
		ESET_Sendomm = 0x02,		// 发送命令
		ESET_RecvComm = 0x03		// 接收命令
	};

	PrintDeviceUI(QWidget *parent = nullptr);


    ~PrintDeviceUI();


private:

	void Init();

	void InitUI();


	 
public slots:
	//UI
	void OnListenBtnClicked(int idx);
	
	//add_UI操作界面处理逻辑
	void OnShowOperCmd(const QString& msg, QByteArray arr, const ShowEditType& type);


	//Btn
	void OnHandlePrintOperFun(int idx);
	void OnHandlePrintParamFun(int idx);
	void OnHandlePrintMoveFun(int idx);



signals:
	void SigAddShowLog(const QString& logStr);
	void SigShowOperComm(const QString& msg, const ShowEditType& type, QByteArray arr = QByteArray());
	
	//add
	void SigAddShowOperCmd(const QString& msg, QByteArray arr = QByteArray(), const ShowEditType& type = ShowEditType::ESET_Sendomm);


private:
	//Data
	motionControlSDK* m_motionSDK;
	//std::unique_ptr<TcpClient>		m_tcpClient;
	//std::unique_ptr<ProtocolPrint>	m_protocol;
	//QTimer m_heartSendTimer;	//心跳定时器	
	//QTimer m_heartOutTimer;		//超时
	//int m_heartOutCnt;			//心跳超时计数

	QButtonGroup* m_netBtnGroup;


	QButtonGroup* m_printOperBtnGroup;
	QButtonGroup* m_printParamBtnGroup;
	QButtonGroup* m_printMoveBtnGroup;


	QMutex m_heartMtx;			//心跳互斥锁



	//UI
	QLineEdit* m_ipLine;
	QLineEdit* m_portLine;
	QLabel* m_connStateLab;
	QTextEdit*	m_operLogText;
	QTextEdit*	m_sendCommText;
	QTextEdit*	m_recvCommText;

};
