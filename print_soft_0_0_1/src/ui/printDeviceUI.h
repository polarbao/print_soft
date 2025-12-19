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
#include "TcpClient.h"
#include "ProtocolPrint.h"


#include "motioncontrolsdk_global.h"
#include "motionControlSDK.h"


namespace UI { class PrintDeviceUI; }


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

	enum AxisFun
	{
		EAF_Begin = EPF_End + 1,
		//x
		EAF_AutoForward,
		EAF_AutoBack,
		EAF_Forward1CM,
		EAF_Back1CM,
		EAF_Reset,
		EAF_MovePrintPos,

		//x
		EAF_XAxisAutoForward,
		EAF_XAxisAutoBack,
		EAF_XAxisForward1CM,
		EAF_XAxisBack1CM,
		EAF_XAxisReset,
		EAF_XAxisMovePrintPos,
		//x
		EAF_YAxisAutoForward,
		EAF_YAxisAutoBack,
		EAF_YAxisForward1CM,
		EAF_YAxisBack1CM,
		EAF_YAxisReset,
		EAF_YAxisMovePrintPos,

		//x
		EAF_ZAxisAutoForward,
		EAF_ZAxisAutoBack,
		EAF_ZAxisForward1CM,
		EAF_ZAxisBack1CM,
		EAF_ZAxisReset,
		EAF_ZAxisMovePrintPos,

		//EAF_YAxisForward,
		//EAF_ZAxisForward,
		//EAF_XAxisBack,
		//EAF_YAxisBack,
		//EAF_ZAxisBack,
		//EAF_AxisReset,
		EAF_End
	};


	enum ImgType
	{
		EIT_JPG = 0x01,		// JPG格式
		EIT_PNG = 0x02,		// 可扩展其他格式
		EIT_BMP = 0x03,		// JPG格式
		EIT_RAW = 0x04,		// 可扩展其他格式
		EIT_Other = 0x05
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

	void HandlerMoveDeviceOper(const PrintFun& moveFun);

	 
public slots:
	//tcp
	void OnRecvMsg(QByteArray msg);
	void OnErrMsg(QAbstractSocket::SocketError err);
	void OnSockChangeState(QAbstractSocket::SocketState state);

	//UI
	void OnPrintFunClicked(int idx);
	void OnListenBtnClicked(int idx);
	void OnAppendShowComm(const QString& msg, const ShowEditType& type, QByteArray arr);


	//protocol
	void OnProtocolReply(int cmd, uchar errCode, QByteArray arr = QByteArray());

	//heart 
	void OnRecvHeartTimeout();
	void OnSendHeartComm();
	void OnRecvHeartComm();

signals:
	void SigAddShowLog(const QString& logStr);
	void SigHeartTimeout();
	void SigShowOperComm(const QString& msg, const ShowEditType& type, QByteArray arr = QByteArray());

private:
	//Data
	motionControlSDK* m_motionSDK;
	//std::unique_ptr<TcpClient>		m_tcpClient;
	//std::unique_ptr<ProtocolPrint>	m_protocol;
	//QTimer m_heartSendTimer;	//心跳定时器	
	//QTimer m_heartOutTimer;		//超时
	//int m_heartOutCnt;			//心跳超时计数

	QButtonGroup* m_printBtnGroup;
	QButtonGroup* m_funBtnGroup;

	QMutex m_heartMtx;			//心跳互斥锁



	//UI
	QLineEdit* m_ipLine;
	QLineEdit* m_portLine;
	QLabel* m_connStateLab;
	QTextEdit*	m_operLogText;
	QTextEdit*	m_sendCommText;
	QTextEdit*	m_recvCommText;

};
