#include "printDeviceUI.h"
#include <QPushButton>
#include "global.h"
#include "utils.h"




#define TurnOffHeartBeat

PrintDeviceUI::PrintDeviceUI(QWidget *parent)
    : QDialog(parent)
	//, m_motionSDK(new motionControlSDK())
	//, m_tcpClient(std::make_unique<TcpClient>())
	//, m_protocol(std::make_unique<ProtocolPrint>())
	, m_printBtnGroup(new QButtonGroup())
	, m_funBtnGroup(new QButtonGroup())
	//, m_heartOutCnt(0)
	, m_portLine(new QLineEdit())

{
	Init();
}

PrintDeviceUI::~PrintDeviceUI()
{
	//if (m_heartSendTimer.isActive() || m_heartOutTimer.isActive())
	//{
	//	m_heartSendTimer.stop();
	//	m_heartOutTimer.stop();
	//}
}

void PrintDeviceUI::Init() 
{
	m_motionSDK = new motionControlSDK(this);
	m_motionSDK->initialize("./");

	connect(m_motionSDK, &motionControlSDK::connected, this, [this]() 
	{
		LOG_INFO(QString(u8"motion_SDK_链接成功"));
	});

	connect(m_motionSDK, &motionControlSDK::disconnected, this, [this]() 
	{
		LOG_INFO(QString(u8"motion_SDK_断开成功"));

	});

	connect(m_motionSDK, &motionControlSDK::connectedChanged, this, [this](bool bConn)
	{
		LOG_INFO(QString(u8"motion_SDK_链接状态%1").arg(bConn));
	});

	connect(m_motionSDK, &motionControlSDK::printProgressUpdated, [this]()
	{
	
	});

	connect(m_motionSDK, &motionControlSDK::errorOccurred, this, [this](int ec, const QString& emsg)
	{
		LOG_INFO(QString(u8"motion_SDK_当前错误code:%1, 当前错误msg:%2").arg(ec).arg(emsg));
	});

	connect(m_motionSDK, &motionControlSDK::infoMessage, this, [this](const QString& emsg)
	{
		LOG_INFO(QString(u8"motion_SDK_当前msg:%1").arg(emsg));
	});

	connect(m_motionSDK, &motionControlSDK::logMessage, this, [this](const QString& msg)
	{
		LOG_INFO(QString(u8"motion_SDK_当前打印日志msg:%1").arg(msg));
	});

	//m_heartSendTimer.setInterval(10000); // 10秒心跳
	//m_heartOutTimer.setInterval(5000);
	InitUI();

	//connect(m_tcpClient.get(), &TcpClient::sigNewData, this, &PrintDeviceUI::OnRecvMsg);
	//connect(m_tcpClient.get(), &TcpClient::sigError, this, &PrintDeviceUI::OnErrMsg);
	//connect(m_tcpClient.get(), &TcpClient::sigSocketStateChanged, this, &PrintDeviceUI::OnSockChangeState);


	connect(m_printBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &PrintDeviceUI::OnPrintFunClicked);
	connect(m_funBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &PrintDeviceUI::OnListenBtnClicked);

	//heart
	//connect(&m_heartSendTimer, &QTimer::timeout, this, &PrintDeviceUI::OnSendHeartComm);
	//connect(&m_heartOutTimer, &QTimer::timeout, this, &PrintDeviceUI::OnRecvHeartTimeout);
	//connect(m_protocol.get(), &ProtocolPrint::SigHeartBeat, this, &PrintDeviceUI::OnRecvHeartComm);
	//connect(m_protocol.get(), &ProtocolPrint::SigCmdReply, this, &PrintDeviceUI::OnProtocolReply);


	//UI
	connect(this, &PrintDeviceUI::SigShowOperComm, this, &PrintDeviceUI::OnAppendShowComm);


}

void PrintDeviceUI::InitUI()
{
	setWindowTitle(QStringLiteral("打印机模块设置"));

	QGroupBox* box1 = new QGroupBox(QStringLiteral("网络通信设置"));
	{
		QGridLayout* layout = new QGridLayout;
		box1->setLayout(layout);

		QLabel* ipLab = new QLabel(u8"IP");
		QLabel* portLab = new QLabel(u8"Port");
		QLabel* stateLab = new QLabel(u8"连接状态");
		m_connStateLab = new QLabel(u8"未链接");

		m_ipLine = new QLineEdit();
		//m_ipLine->setText("192.168.100.175");
		m_ipLine->setText("127.0.0.1");

		m_portLine = new QLineEdit();
		m_portLine->setText("5555");
		//m_portLine->setText("6666");



		QPushButton* connBtn = new QPushButton(QStringLiteral("开始连接"));
		QPushButton* disConnBtn = new QPushButton(QStringLiteral("断开连接"));
		QPushButton* clearBtn = new QPushButton(QStringLiteral("命令清除"));
		m_funBtnGroup->addButton(connBtn, ENF_StartConn);
		m_funBtnGroup->addButton(disConnBtn, ENF_DisConn);
		m_funBtnGroup->addButton(clearBtn, ENF_DisConn);
		QSpacerItem *vSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

		layout->addWidget(ipLab, 0, 0, 1, 1);
		layout->addWidget(m_ipLine, 0, 1, 1, 2);
		layout->addWidget(portLab, 1, 0, 1, 1);
		layout->addWidget(m_portLine, 1, 1, 1, 2);
		layout->addWidget(stateLab, 2, 0, 1, 1);
		layout->addWidget(m_connStateLab, 2, 1, 1, 1);
		layout->addWidget(connBtn, 3, 0, 1, 1);
		layout->addWidget(disConnBtn, 3, 1, 1, 1);
		//layout->addItem(vSpacer, 3, 2, 1, 1);
	}


	QGroupBox* box2 = new QGroupBox(QStringLiteral("打印功能操作"));
	{
		QGridLayout* layout = new QGridLayout;
		box2->setLayout(layout);

		auto addBtn = [&](const QString& text, PrintFun type, int row, int col)
		{
			QPushButton* btn = new QPushButton(text);
			m_printBtnGroup->addButton(btn, type);
			layout->addWidget(btn, row, col, 1, 1);
		};

		addBtn(u8"开始打印", EPF_StartPrint, 0, 0);
		addBtn(u8"停止打印", EPF_StopPrint, 0, 1);
		addBtn(u8"暂停打印", EPF_PausePrint, 0, 3);
		addBtn(u8"继续打印", EPF_ContinuePrint, 0, 4);
		addBtn(u8"选择图片", EPF_TransData, 1, 0);
		addBtn(u8"打印复位", EPF_ResetPrint, 1, 1);

#ifdef TurnOffHeartBeat
		m_printBtnGroup->button(EPF_PausePrint)->setEnabled(false);
		m_printBtnGroup->button(EPF_ContinuePrint)->setEnabled(false);
		m_printBtnGroup->button(EPF_TransData)->setEnabled(false);
		m_printBtnGroup->button(EPF_ResetPrint)->setEnabled(false);
#endif // TurnOffHeartBeat

	}

	QGroupBox* box3 = new QGroupBox(QStringLiteral("龙门架移动操作"));
	{
		QGridLayout* layout = new QGridLayout;
		box3->setLayout(layout);

		auto addBtn = [&](const QString& text, AxisFun type, int row, int col)
		{
			QLabel* titleLab = new QLabel(text);

			QPushButton* xBtn = new QPushButton(u8"X轴");
			QPushButton* yBtn = new QPushButton(u8"Y轴");
			QPushButton* zBtn = new QPushButton(u8"Z轴");
			//QPushButton* allBtn = new QPushButton(u8"全部");

#ifdef TurnOffHeartBeat			

			//设置按钮不可触发
			//xBtn->setEnabled(false);
			//yBtn->setEnabled(false);
			//zBtn->setEnabled(false);
			//allBtn->setEnabled(false);
#endif

			m_printBtnGroup->addButton(xBtn, static_cast<AxisFun>(type+6));
			m_printBtnGroup->addButton(yBtn, static_cast<AxisFun>(type+12));
			m_printBtnGroup->addButton(zBtn, static_cast<AxisFun>(type+18));
			//m_printBtnGroup->addButton(allBtn, static_cast<AxisFun>(type+4));

			layout->addWidget(titleLab, row, col, 1, 1);
			layout->addWidget(xBtn, row, col+1, 1, 1);
			layout->addWidget(yBtn, row, col+2, 1, 1);
			layout->addWidget(zBtn, row, col+3, 1, 1);
			//layout->addWidget(allBtn, row, col+4, 1, 1);

		};

		addBtn(u8"自动前进", EAF_AutoForward, 0, 0);
		addBtn(u8"自动后退", EAF_AutoBack, 1, 0);
		addBtn(u8"前进1CM", EAF_Forward1CM, 2, 0);
		addBtn(u8"后退1CM", EAF_Back1CM, 3, 0);
		addBtn(u8"复位", EAF_Reset, 4, 0);
		addBtn(u8"打印位置", EAF_MovePrintPos, 5, 0);

		//addBtn(u8"速度+", EAF_XAxisForward, 3, 0);
		//addBtn(u8"速度-", EAF_XAxisForward, 4, 0);
	}



	QGroupBox* box4 = new QGroupBox(QStringLiteral("操作信息"));
	{
		QGridLayout* layout = new QGridLayout;
		box4->setLayout(layout);
		m_operLogText = new QTextEdit();
		layout->addWidget(m_operLogText, 0, 0, 2, 3);
		QSpacerItem *vSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
		layout->addItem(vSpacer, 1, 0);
	}


	QGroupBox* box5 = new QGroupBox(QStringLiteral("发送/接收信息"));
	{
		QGridLayout* layout = new QGridLayout;
		box5->setLayout(layout);

		QLabel* inputLab = new QLabel(u8"发送Hex命令");
		QLabel* outputLab = new QLabel(u8"接收Hex命令");

		m_sendCommText = new QTextEdit();
		m_sendCommText->setMinimumWidth(300);
		m_recvCommText = new QTextEdit();
		m_recvCommText->setMinimumWidth(300);
		QSpacerItem *vSpacer = new QSpacerItem(1, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

		layout->addWidget(inputLab, 0, 0, 1, 1);
		layout->addWidget(m_sendCommText, 1, 0, 2, 3);
		layout->addWidget(outputLab, 3, 0, 1, 1);
		layout->addWidget(m_recvCommText, 4, 0, 2, 3);
		layout->addItem(vSpacer, 5, 0);
	}

	QHBoxLayout* ph = new QHBoxLayout(this);
	{
		QGridLayout* ph_l = new QGridLayout;
		{
			ph_l->addWidget(box1, 0, 0);
			ph_l->addWidget(box2, 1, 0);
			ph_l->addWidget(box3, 2, 0);
			ph_l->addWidget(box4, 3, 0);
		}

		QHBoxLayout* ph_r = new QHBoxLayout(this);
		{
			ph_r->addWidget(box5);
		}
		ph->addLayout(ph_l, 1);
		ph->addLayout(ph_r, 1);
	}
	setLayout(ph);
}


void PrintDeviceUI::HandlerMoveDeviceOper(const PrintFun& moveFun)
{
	QString logStr;
	QByteArray sendData;
	auto bTransData = false;
	int cmdType = 0;
	switch (moveFun)
	{
		//todo: 后续根据实际情况，针对报文数据进行修改，使用统一模板
	case EAF_XAxisReset:
	{
		//sendData = m_motionSDK->
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_XAxisReset);
		cmdType = static_cast<int>(ProtocolPrint::Set_XAxisReset);
		break;
	}
	case EAF_XAxisMovePrintPos:
	{
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_XAxisMovePrintPos);
		cmdType = static_cast<int>(ProtocolPrint::Set_XAxisMovePrintPos);

		break;
	}

	case EAF_ZAxisAutoForward:
	{
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_ZAxisUpAuto);
		cmdType = static_cast<int>(ProtocolPrint::Set_ZAxisUpAuto);

		break;
	}
	case EAF_ZAxisAutoBack:
	{	
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_ZAxisDownAuto);
		cmdType = static_cast<int>(ProtocolPrint::Set_ZAxisDownAuto);

		break;
	}
	case EAF_ZAxisForward1CM:
	{
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_ZAxisUp1CM);
		cmdType = static_cast<int>(ProtocolPrint::Set_ZAxisUp1CM);

		break;
	}

	case EAF_ZAxisBack1CM:
	{
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_ZAxisDown1CM);
		cmdType = static_cast<int>(ProtocolPrint::Set_ZAxisDown1CM);

		break;
	}
	case EAF_ZAxisReset:
	{
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_ZAxisReset);
		cmdType = static_cast<int>(ProtocolPrint::Set_ZAxisReset);

		break;
	}
	case EAF_ZAxisMovePrintPos:
	{
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_ZAxisMovePrintPos);
		cmdType = static_cast<int>(ProtocolPrint::Set_ZAxisMovePrintPos);

		break;
	}
	case EAF_YAxisReset:
	{
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_YAxisReset);
		cmdType = static_cast<int>(ProtocolPrint::Set_YAxisReset);

		break;
	}
	case EAF_YAxisMovePrintPos:
	{
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_YAxisMovePrintPos);
		cmdType = static_cast<int>(ProtocolPrint::Set_YAxisMovePrintPos);

		break;
	}
	default:
		break;
	}
	m_motionSDK->sendData(cmdType, sendData);
	//m_tcpClient->sendData(sendData);
	logStr.append(QString::fromLocal8Bit("开始进行移动操作"));
	emit SigShowOperComm(logStr, ESET_OperComm);
	emit SigShowOperComm(sendData.toHex().toUpper(), ESET_Sendomm);

}

void PrintDeviceUI::OnRecvMsg(QByteArray msg)
{
	//m_protocol->Decode(msg);
}

void PrintDeviceUI::OnErrMsg(QAbstractSocket::SocketError err)
{

}

void PrintDeviceUI::OnSockChangeState(QAbstractSocket::SocketState state)
{
//	if (QAbstractSocket::ConnectedState == state)
//	{
//		m_connStateLab->setText(QString::fromLocal8Bit("已连接"));
//		//启动心跳数据
//#ifndef TurnOffHeartBeat
//		m_heartOutTimer.start();
//		m_heartSendTimer.start();
//#endif
//		auto sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Con_Breath);
//		m_tcpClient->sendData(sendData);
//		LOG_DEBUG(QString::fromLocal8Bit("启动心跳"));
//		auto logStr(QString::fromLocal8Bit("连接服务器成功，并启动心跳检测"));
//		m_funBtnGroup->button(ENF_StartConn)->setEnabled(false);
//		m_funBtnGroup->button(ENF_DisConn)->setEnabled(true);
//		emit SigShowOperComm(logStr, ESET_OperComm, sendData.toHex().toUpper());
//		emit SigShowOperComm(sendData.toHex().toUpper(), ESET_Sendomm);
//
//
//	}
//	else if (QAbstractSocket::ClosingState == state)
//	{
//		if (m_heartSendTimer.isActive() || m_heartOutTimer.isActive())
//		{
//			m_heartOutTimer.stop();
//			m_heartSendTimer.stop();
//		}
//		m_connStateLab->setText(QString::fromLocal8Bit("未连接"));
//		m_funBtnGroup->button(ENF_StartConn)->setEnabled(true);
//		m_funBtnGroup->button(ENF_DisConn)->setEnabled(false);
//		auto logStr(QString::fromLocal8Bit("断连服务器成功，并停止心跳检测"));
//		emit SigShowOperComm(logStr, ESET_OperComm);
//	}
}


void PrintDeviceUI::OnPrintFunClicked(int idx)
{
	auto btnType = static_cast<PrintFun>(idx);
	QString logStr;
	QByteArray sendData;
	auto bTransData = false;
	int cmdType = 0;
	//判断是否为龙门架移动操作
	if (EPF_End < btnType && EAF_End > btnType)
	{
		HandlerMoveDeviceOper(btnType);
		return;
	}

	switch (btnType)
	{
	case EPF_StartPrint:
	{
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_StartPrint);
		cmdType = static_cast<int>(ProtocolPrint::Set_StartPrint);
		m_motionSDK->sendData(cmdType, sendData);

		//m_tcpClient->sendData(sendData);	
		logStr.append(QString::fromLocal8Bit("开始打印操作"));
		break;
	}
	case EPF_StopPrint:
	{
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_StopPrint);
		cmdType = static_cast<int>(ProtocolPrint::Set_StopPrint);
		m_motionSDK->sendData(cmdType, sendData);

		//m_tcpClient->sendData(sendData);	
		logStr.append(QString::fromLocal8Bit("停止打印操作"));
		break;
	}
	case EPF_PausePrint:
	{
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_PausePrint);
		cmdType = static_cast<int>(ProtocolPrint::Set_PausePrint);
		m_motionSDK->sendData(cmdType, sendData);

		//m_tcpClient->sendData(sendData);	
		logStr.append(QString::fromLocal8Bit("暂停打印操作"));
		break;
	}
	case EPF_ContinuePrint:
	{		
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_continuePrint);
		cmdType = static_cast<int>(ProtocolPrint::Set_continuePrint);
		m_motionSDK->sendData(cmdType, sendData);

		//m_tcpClient->sendData(sendData);	
		logStr.append(QString::fromLocal8Bit("继续打印操作"));
		break;
	}
	case EPF_ResetPrint:
	{
		sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_ResetPrint);
		cmdType = static_cast<int>(ProtocolPrint::Set_ResetPrint);
		m_motionSDK->sendData(cmdType, sendData);

		//m_tcpClient->sendData(sendData);	
		logStr.append(QString::fromLocal8Bit("重置打印操作"));
		break;
	}
	case EPF_TransData:
	{
		auto imgPath = QFileDialog::getOpenFileName(0, u8"打开文件", QDir::homePath() + "/Desktop", "image Files(*.jpg *.png *.bmp *.raw))");
		QImage img(imgPath);
		if (img.isNull())
		{
			LOG_INFO(u8"打开图片错误，当前图片为空");
		}

		// 读取原始文件数据
		QFile file(imgPath);
		if (!file.open(QIODevice::ReadOnly))
		{
			LOG_INFO(u8"无法读取文件");
		}
		QByteArray rawData = file.readAll().toHex();
		file.close();
		auto sendData = ProtocolPrint::GetSendImgDatagram(img.width(), img.width(), ImgType::EIT_JPG, rawData);
		for (auto& it : sendData)
		{
			QByteArray sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Set_StartPrint, it);
			cmdType = static_cast<int>(ProtocolPrint::Set_StartPrint);
			m_motionSDK->sendData(cmdType, sendData);

			//m_tcpClient->sendData(sendData);	
		}
		break;
	}
	default:
		break;
	}
	if (!bTransData)
	{
		emit SigShowOperComm(logStr, ESET_OperComm);
		emit SigShowOperComm(sendData.toHex().toUpper(), ESET_Sendomm);
	}
	else
	{
		emit SigShowOperComm(logStr, ESET_OperComm);
	}
	LOG_INFO(QString("print_UI_module print_btn_oper cur_oper_comm = %1, sned_hex_comm = %2")
		.arg(logStr.toStdString().c_str()).arg(sendData.toHex().toStdString().c_str()));
}

void PrintDeviceUI::OnListenBtnClicked(int idx)
{
	auto btnType = static_cast<NetFun>(idx);
	QString logStr;
	switch (btnType)
	{
	case ENF_StartConn:
	{
		auto ip = m_ipLine->text();
		auto port = m_portLine->text();

		if (m_motionSDK->isConnected())
		{
			logStr.append(QString::fromLocal8Bit("开始连接服务器，但已处于在线状态"));
			emit SigShowOperComm(logStr, ESET_OperComm);
			return;
		}
		m_motionSDK->connectToDevice(m_ipLine->text(), m_portLine->text().toShort());
		logStr.append(QString::fromLocal8Bit("连接服务器"));
		emit SigShowOperComm(logStr, ESET_OperComm);
		break;
	}
	case ENF_DisConn:
	{
		if (!m_motionSDK->isConnected())
		{
			logStr.append(QString::fromLocal8Bit("处于离线状态"));
			return;
		}
		m_motionSDK->disconnectFromDevice();
		logStr.append(QString::fromLocal8Bit("断连服务器"));
		emit SigShowOperComm(logStr, ESET_OperComm);
		break;
	}
	case ENF_ChangePort:
	{
		auto newPort = m_portLine->text();
		m_motionSDK->disconnectFromDevice();
		m_motionSDK->connectToDevice(m_ipLine->text(), m_portLine->text().toShort());
		logStr.append(QString::fromLocal8Bit("改变连接服务器状态"));
		emit SigShowOperComm(logStr, ESET_OperComm);
		break;
	}
	case ENF_ClearAllText:
	{
		m_operLogText->clear();
		m_sendCommText->clear();
		m_recvCommText->clear();
		break;
	}
	default:
		break;
	}
	LOG_INFO(logStr);
}

void PrintDeviceUI::OnAppendShowComm(const QString& msg, const ShowEditType& type, QByteArray arr)
{
	QObject *senderObj = sender();
	QString showStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	showStr.append("\n");
	switch (type)
	{
	case PrintDeviceUI::ESET_OperComm:
	{
		showStr.append(QString::fromLocal8Bit("当前操作："));
		showStr.append(msg);
		m_operLogText->append(showStr);
		break;
	}
	case PrintDeviceUI::ESET_Sendomm:
	{
		showStr.append(QString::fromLocal8Bit("发送Hex命令："));
		showStr.append(msg.toLocal8Bit());
		m_sendCommText->append(showStr);
		break;
	}	
	case PrintDeviceUI::ESET_RecvComm:
	{
		showStr.append(QString::fromLocal8Bit("接收命令："));
		showStr.append(msg.toLocal8Bit() + "\n");
		showStr.append(QString::fromLocal8Bit("接收Hex命令："));
		if (!arr.isEmpty())
		{
			showStr.append(arr.toHex().toUpper());
		}
		m_recvCommText->append(showStr);
		break;
	}
	default:
		break;
	}
}

void PrintDeviceUI::OnProtocolReply(int cmd, uchar errCode, QByteArray arr/* = QByteArray()*/)
{
	//auto senCmd = static_cast<ProtocolPrint::FunCode>(cmd);
	//if (errCode != 0)
	//{
	//	emit SigShowOperComm(QString::number(cmd, 16), ESET_RecvComm, arr);	
	//	//AddLogInfo("err");
	//}
	//else
	//{
	//	emit SigShowOperComm(QString::number(cmd, 16), ESET_RecvComm, arr);
	//	//AddLogInfo("ok");
	//}
}

void PrintDeviceUI::OnRecvHeartTimeout()
{
	//QMutexLocker locker(&m_heartMtx);
	//m_heartOutCnt++;
	//if (m_heartOutTimer.isActive())
	//{
	//	m_heartOutTimer.stop();
	//}
}

void PrintDeviceUI::OnSendHeartComm()
{
//#ifdef TurnOffHeartBeat	
//	m_heartOutTimer.start();
//	auto sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Con_Breath);
//	m_tcpClient->sendData(sendData);
//	emit SigShowOperComm(sendData.toHex().toUpper(), ShowEditType::ESET_Sendomm);
//
//#else
//	//发送心跳comm
//	if (m_heartOutCnt <= 2)
//	{
//		m_heartOutTimer.start();
//		auto sendData = ProtocolPrint::GetSendDatagram(ProtocolPrint::Con_Breath);
//		m_tcpClient->sendData(sendData);
//}
//	else
//	{
//		if (!m_tcpClient->isConnected())
//		{
//			m_connStateLab->setText(QString::fromLocal8Bit("未连接"));
//			//logStr.append(QString::fromLocal8Bit("处于离线状态"));
//			//更改状态显示
//			//return;
//		}
//		m_tcpClient->disconnectFromHost();
//		if (m_heartOutTimer.isActive() || m_heartSendTimer.isActive())
//		{
//			m_heartOutTimer.stop();
//			m_heartSendTimer.stop();
//		}
//	}
//#endif
}

void PrintDeviceUI::OnRecvHeartComm()
{
	//QMutexLocker locker(&m_heartMtx);
	//m_heartOutCnt = 0;
	//m_heartSendTimer.start();
	//m_heartOutTimer.start();
	//qDebug() << "Connected to server";
}

