#include "printDeviceUI.h"
#include <QPushButton>
#include "global.h"
#include "utils.h"


#include "sdk/protocol/ProtocolPrint.h"

#define TurnOffHeartBeat

PrintDeviceUI::PrintDeviceUI(QWidget *parent)
    : QDialog(parent)
	, m_netBtnGroup(new QButtonGroup())
	, m_printOperBtnGroup(new QButtonGroup())
	, m_printParamBtnGroup(new QButtonGroup())
	, m_printMoveBtnGroup(new QButtonGroup())

	, m_portLine(new QLineEdit())

{
	Init();
}

PrintDeviceUI::~PrintDeviceUI()
{


}

void PrintDeviceUI::Init() 
{
	m_motionSDK = new motionControlSDK(this);
	m_motionSDK->MC_Init("./");

	connect(m_motionSDK, &motionControlSDK::connected, this, [this]()
	{
		m_connStateLab->setText(QString::fromLocal8Bit("连接成功"));

		emit SigAddShowOperCmd(QString::fromLocal8Bit("连接成功"), "", ShowEditType::ESET_RecvComm);

		LOG_INFO(QString(u8"motion_SDK_moudle, 链接成功"));
	});

	connect(m_motionSDK, &motionControlSDK::MC_SigDisconnected, this, [this]()
	{
		m_connStateLab->setText(QString::fromLocal8Bit("断开连接"));

		emit SigAddShowOperCmd(QString::fromLocal8Bit("断开连接"), "", ShowEditType::ESET_RecvComm);

		LOG_INFO(QString(u8"motion_SDK_moudle, 断开成功"));
	});

	connect(m_motionSDK, &motionControlSDK::MC_SigConnectedChanged, this, [this](bool bConn)
	{
		auto logInfo = QString(u8"motion_SDK_moudle, 链接状态%1").arg(bConn);

		emit SigAddShowOperCmd(logInfo, "", ShowEditType::ESET_RecvComm);

		LOG_INFO(QString(u8"motion_SDK_moudle, 链接状态%1").arg(bConn));
	});

	connect(m_motionSDK, &motionControlSDK::MC_SigPrintProgUpdated, [this]()
	{

	});

	connect(m_motionSDK, &motionControlSDK::MC_SigErrOccurred, this, [this](int ec, const QString& emsg)
	{
		QString logStr = QString(u8"motion_SDK_moudle, 当前错误code: %1, 当前错误msg: %2").arg(ec).arg(emsg);
		emit SigAddShowOperCmd(logStr, "", ShowEditType::ESET_RecvComm);

		LOG_INFO(QString(u8"motion_SDK_当前错误code:%1, 当前错误msg: %2").arg(ec).arg(emsg));
	});

	connect(m_motionSDK, &motionControlSDK::MC_SigInfoMsg, this, [this](const QString& emsg)
	{
		QString logStr = QString::fromLocal8Bit("motion_SDK_moudle, 当前msg: %1").arg(emsg);
		emit SigAddShowOperCmd(logStr, "", ShowEditType::ESET_RecvComm);

		LOG_INFO(logStr);
	});

	connect(m_motionSDK, &motionControlSDK::MC_SigLogMsg, this, [this](const QString& msg)
	{
		QString logStr = QString::fromLocal8Bit("motion_SDK_moudle, 当前打印日志msg: %1").arg(msg);
		emit SigAddShowOperCmd(logStr, "", ShowEditType::ESET_RecvComm);

		LOG_INFO(logStr);
	});


	connect(m_motionSDK, &motionControlSDK::MC_SigSend2DevCmdMsg, this, [this](const QString& msg)
	{
		QString logStr = QString::fromLocal8Bit("motion_SDK_moudle, 下发设备msg: %1").arg(msg);
		emit SigAddShowOperCmd(logStr, "", ShowEditType::ESET_Sendomm);

		LOG_INFO(logStr);
	});

	connect(m_motionSDK, &motionControlSDK::MC_SigRecv2DevCmdMsg, this, [this](const QString& msg)
	{
		QString logStr = QString::fromLocal8Bit("motion_SDK_moudle, 接收设备msg: %1").arg(msg);
		emit SigAddShowOperCmd(logStr, "", ShowEditType::ESET_RecvComm);

		LOG_INFO(logStr);
	});

	InitUI();

	//net
	connect(m_netBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &PrintDeviceUI::OnListenBtnClicked);

	//motion
	connect(m_printOperBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &PrintDeviceUI::OnHandlePrintOperFun);
	connect(m_printParamBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &PrintDeviceUI::OnHandlePrintParamFun);	
	connect(m_printMoveBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &PrintDeviceUI::OnHandlePrintMoveFun);
	
	//UI
	connect(this, &PrintDeviceUI::SigAddShowOperCmd, this, &PrintDeviceUI::OnShowOperCmd);




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
		m_netBtnGroup->addButton(connBtn, ENF_StartConn);
		m_netBtnGroup->addButton(disConnBtn, ENF_DisConn);
		m_netBtnGroup->addButton(clearBtn, ENF_DisConn);
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

		auto addBtn = [&](const QString& text, EPrintOperFun type, int row, int col)
		{
			QPushButton* btn = new QPushButton(text);
			m_printOperBtnGroup->addButton(btn, type);
			layout->addWidget(btn, row, col, 1, 1);
		};

		addBtn(u8"开始打印", EPOF_StartPrint, 0, 0);
		addBtn(u8"停止打印", EPOF_StopPrint, 0, 1);
		addBtn(u8"暂停打印", EPOF_PausePrint, 0, 3);
		addBtn(u8"继续打印", EPOF_ContinuePrint, 0, 4);
		addBtn(u8"清洗位置", EPOF_CleanPos, 0, 5);
		addBtn(u8"全轴复位", EPOF_GoHomnePos, 0, 6);



#ifdef TurnOffHeartBeat
		//m_printBtnGroup->button(EPF_PausePrint)->setEnabled(false);
		//m_printBtnGroup->button(EPF_ContinuePrint)->setEnabled(false);
		//m_printBtnGroup->button(EPF_TransData)->setEnabled(false);
		//m_printBtnGroup->button(EPF_ResetPrint)->setEnabled(false);
#endif // TurnOffHeartBeat

	}


	QGroupBox* box21 = new QGroupBox(QStringLiteral("打印参数设置"));
	{
		QGridLayout* layout = new QGridLayout;
		box21->setLayout(layout);

		auto addUI = [&](const QString& text, const QString& objText, EPrintParamType type, int row, int col)
		{
			QLabel* titleLab = new QLabel(text);
			QLineEdit* dataLine = new QLineEdit();
			QPushButton* btn = new QPushButton(u8"更新");
			
			titleLab->setMaximumWidth(60);
			dataLine->setMinimumWidth(80);
			dataLine->setPlaceholderText(u8"请输入数据");
			dataLine->setObjectName(objText);
			btn->setMaximumWidth(40);


#ifdef TurnOffHeartBeat			

			//设置按钮不可触发
			//xBtn->setEnabled(false);
			//yBtn->setEnabled(false);
			//zBtn->setEnabled(false);
			//allBtn->setEnabled(false);
#endif
			m_printParamBtnGroup->addButton(btn, static_cast<EPrintParamType>(type));

			layout->addWidget(titleLab, row, col, 1, 1);
			layout->addWidget(dataLine, row, col + 1, 1, 1);
			layout->addWidget(btn, row, col + 2, 1, 1);
		};

		addUI(u8"打印起点", "EPPT_SetStartPos", EPPT_SetStartPos, 0, 0);
		addUI(u8"打印终点", "EPPT_SetStopPos", EPPT_SetStopPos, 0, 3);
		addUI(u8"清洗位置", "EPPT_SetClenaPos", EPPT_SetClenaPos, 0, 6);

		addUI(u8"X轴步进", "EPPT_SetStepX", EPPT_SetStepX, 1, 0);
		addUI(u8"Y轴步进", "EPPT_SetStepY", EPPT_SetStepY, 1, 3);
		addUI(u8"Z轴步进", "EPPT_SetStepZ",EPPT_SetStepZ, 1, 6);

		addUI(u8"Z轴限位", "EPPT_SetLimitXPos", EPPT_SetLimitXPos, 2, 6);
		addUI(u8"X轴限位", "EPPT_SetLimitXPos", EPPT_SetLimitXPos, 2, 0);
		addUI(u8"Y轴限位", "EPPT_SetLimitXPos", EPPT_SetLimitXPos, 2, 3);
		
		addUI(u8"X轴速度", "EPPT_SetSpdX", EPPT_SetSpdX, 3, 0);
		addUI(u8"Y轴速度", "EPPT_SetSpdY", EPPT_SetSpdY, 3, 3);
		addUI(u8"Z轴速度", "EPPT_SetSpdZ", EPPT_SetSpdZ, 3, 6);

	}


	QGroupBox* box31 = new QGroupBox(QStringLiteral("打印移动操作"));
	{
		QGridLayout* layout = new QGridLayout;
		box31->setLayout(layout);

		auto addUI = [&](const QString& text, const QString& objText, EPrintMoveOperFun type, int row, int col)
		{
			QLabel* titleLab = new QLabel(text);
			QLineEdit* dataLine = new QLineEdit();
			QPushButton* lBtn = new QPushButton(u8"左移");
			QPushButton* rBtn = new QPushButton(u8"右移");
			titleLab->setMaximumWidth(60);
			dataLine->setObjectName(objText);
			dataLine->setMinimumWidth(100);
			dataLine->setPlaceholderText(u8"请输入数据");
			lBtn->setMaximumWidth(50);
			rBtn->setMaximumWidth(50);

			m_printMoveBtnGroup->addButton(lBtn, static_cast<EPrintMoveOperFun>(type));
			m_printMoveBtnGroup->addButton(rBtn, static_cast<EPrintMoveOperFun>(type+1));

			layout->addWidget(titleLab, row, col, 1, 1);
			layout->addWidget(dataLine, row, col + 1, 1, 1);
			layout->addWidget(lBtn, row, col + 2, 1, 1);
			layout->addWidget(rBtn, row, col + 3, 1, 1);

		};
		addUI(u8"X轴", "EPMOF_XAxisLMove", EPMOF_XAxisLMove, 0, 0);
		addUI(u8"Y轴", "EPMOF_YAxisLMove", EPMOF_YAxisLMove, 0, 4);
		addUI(u8"Z轴", "EPMOF_ZAxisLMove", EPMOF_ZAxisLMove, 1, 0);
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
			ph_l->addWidget(box21, 2, 0);
			ph_l->addWidget(box31, 3, 0);
			ph_l->addWidget(box4, 4, 0);
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


void PrintDeviceUI::OnListenBtnClicked(int idx)
{
	QString logStr;
	switch (static_cast<NetFun>(idx))
	{
	case ENF_StartConn:
	{
		auto ip = m_ipLine->text();
		auto port = m_portLine->text();

		if (m_motionSDK->MC_IsConnected())
		{
			logStr.append(QString::fromLocal8Bit("开始连接服务器，但已处于在线状态"));
			return;
		}
		m_motionSDK->MC_Connect2Dev(m_ipLine->text(), m_portLine->text().toShort());
		logStr.append(QString::fromLocal8Bit("连接服务器"));
		break;
	}
	case ENF_DisConn:
	{
		if (!m_motionSDK->MC_IsConnected())
		{
			logStr.append(QString::fromLocal8Bit("处于离线状态"));
			return;
		}
		m_motionSDK->MC_DisconnectDev();
		logStr.append(QString::fromLocal8Bit("断连服务器"));
		break;
	}
	case ENF_ChangePort:
	{
		auto newPort = m_portLine->text();
		m_motionSDK->MC_DisconnectDev();
		m_motionSDK->MC_Connect2Dev(m_ipLine->text(), m_portLine->text().toShort());
		logStr.append(QString::fromLocal8Bit("改变连接服务器状态"));
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
	emit SigAddShowOperCmd(logStr, "", ESET_OperComm);
	LOG_INFO(logStr);
}

void PrintDeviceUI::OnShowOperCmd(const QString& msg, QByteArray arr, const ShowEditType& type)
{
	QString showStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
	showStr.append("\n");
	switch (type)
	{
	case PrintDeviceUI::ESET_OperComm:
	{
		showStr.append(QString::fromLocal8Bit("当前操作："));
		showStr.append(msg + "\n");
		m_operLogText->append(showStr);
		break;
	}
	case PrintDeviceUI::ESET_Sendomm:
	{
		showStr.append(QString::fromLocal8Bit("当前操作："));
		showStr.append(msg + "\n");
		showStr.append(arr + "\n");
		m_sendCommText->append(showStr);
		break;
	}
	case PrintDeviceUI::ESET_RecvComm:
	{
		showStr.append(QString::fromLocal8Bit("接收命令："));
		showStr.append(msg + "\n");
		//showStr.append(QString::fromLocal8Bit("接收Hex命令："));
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


void PrintDeviceUI::OnHandlePrintOperFun(int idx)
{

	auto btnType = static_cast<EPrintOperFun>(idx);
	QString logStr;
	QByteArray sendData;
	int cmdType = 0;
	
	// 1.判断是否连接运动控制模块
	// 2.判断是否设置基础参数（起始、终点、清洗、步进、速度、
	//if (m_motionSDK->MC_IsConnected() && m_motionSDK->MC_IsSetParam())

	if (!m_motionSDK->MC_IsConnected())
	{
		auto logStr = QString(u8"未连接运动模块，无法进行相关操作");
		emit SigAddShowOperCmd(logStr, "", ShowEditType::ESET_OperComm);
		LOG_INFO(logStr);
		return;
	}


	switch (btnType)
	{
	case EPOF_StartPrint:
	{
		cmdType = static_cast<int>(ProtocolPrint::Ctrl_StartPrint);
		logStr.append(QString::fromLocal8Bit("开始打印操作"));

		m_motionSDK->MC_StartPrint();
		LOG_INFO(QString(u8"开始打印操作"));
		break;
	}
	case EPOF_StopPrint:
	{
		cmdType = static_cast<int>(ProtocolPrint::Ctrl_StopPrint);	
		logStr.append(QString::fromLocal8Bit("停止打印操作"));

		m_motionSDK->MC_StopPrint();
		LOG_INFO(QString(u8"停止打印操作"));

		break;
	}
	case EPOF_PausePrint:
	{
		cmdType = static_cast<int>(ProtocolPrint::Ctrl_PasusePrint);
		logStr.append(QString::fromLocal8Bit("暂停打印操作"));

		m_motionSDK->MC_PausePrint();
		LOG_INFO(QString(u8"暂停打印操作"));

		break;
	}
	case EPOF_ContinuePrint:
	{
		cmdType = static_cast<int>(ProtocolPrint::Ctrl_ContinuePrint);
		logStr.append(QString::fromLocal8Bit("继续打印操作"));

		m_motionSDK->MC_ResumePrint();
		LOG_INFO(QString(u8"继续打印操作"));
		break;
	}
	case EPOF_CleanPos:
	{
		cmdType = static_cast<int>(ProtocolPrint::Ctrl_ContinuePrint);
		logStr.append(QString::fromLocal8Bit("移动至清洗位置操作"));

		m_motionSDK->MC_CleanPrint();
		LOG_INFO(QString(u8"移动至清洗位置操作"));
		break;
	}
	case EPOF_GoHomnePos:
	{
		cmdType = static_cast<int>(ProtocolPrint::Ctrl_ResetPos);
		logStr.append(QString::fromLocal8Bit("各轴复位操作"));

		m_motionSDK->MC_GoHome();
		LOG_INFO(QString(u8"各轴复位操作"));
		break;
	}

	default:
		break;
	}
	
	emit SigAddShowOperCmd(logStr, "" ,ShowEditType::ESET_Sendomm);
	emit SigAddShowOperCmd(logStr, "", ShowEditType::ESET_OperComm);
	LOG_INFO(QString("print_UI_module print_btn_oper cur_oper_comm = %1")
		.arg(logStr.toStdString().c_str()));
}

void PrintDeviceUI::OnHandlePrintParamFun(int idx)
{
	auto btnType = static_cast<EPrintOperFun>(idx);
	QString operStr, logStr;
	QByteArray hexArr;
	auto bTransData = false;
	int cmdType = 0;

	//判断是否有默认参数设置
	if (!m_motionSDK->MC_IsConnected())
	{
		auto logStr = QString(u8"未连接运动模块，无法进行相关操作");
		emit SigAddShowOperCmd(logStr, "", ShowEditType::ESET_OperComm);
		LOG_INFO(logStr);
		return;
	}


	switch (btnType)
	{
		case EPPT_SetStartPos:
		{
			cmdType = static_cast<int>(ProtocolPrint::SetParam_PrintStartPos);
			// 获取移动的数据，单位um
			// 根据,拆分数据，往下分发
			auto valData = this->findChild<QLineEdit*>("EPPT_SetStartPos")->text();
			hexArr = Utils::GetInstance().MultiMicroDisStrTo12BytesHex(valData, false, false);
			operStr.append(QString::fromLocal8Bit("设置打印起点信息\n"));
			logStr.append(QString::fromLocal8Bit("设置打印起点信息\n"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(QString(hexArr.toHex(' ').toUpper()))));

			m_motionSDK->MC_SendCmd(cmdType, hexArr);

			break;
		}
		case EPPT_SetStopPos:
		{
			cmdType = static_cast<int>(ProtocolPrint::SetParam_PrintEndPos);
			//获取移动的数据，单位um
			auto valData = this->findChild<QLineEdit*>("EPPT_SetStopPos")->text();
			hexArr = Utils::GetInstance().MultiMicroDisStrTo12BytesHex(valData, false, false);
			operStr.append(QString::fromLocal8Bit("设置打印终点信息"));
			logStr.append(QString::fromLocal8Bit("设置打印终点信息"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(hexArr.toHex(' ').toUpper())));

			m_motionSDK->MC_SendCmd(cmdType, hexArr);			break;
		}
		case EPPT_SetStepX:
		{
			cmdType = static_cast<int>(ProtocolPrint::SetParam_AxisUnitMove);
			auto valData = this->findChild<QLineEdit*>("EPPT_SetStepX")->text();
			hexArr = Utils::GetInstance().MultiMicroDisStrTo12BytesHex(valData, false, false);
			operStr.append(QString::fromLocal8Bit("设置X轴步进值"));
			logStr.append(QString::fromLocal8Bit("设置打印终点信息"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(hexArr.toHex(' ').toUpper())));

			m_motionSDK->MC_SendCmd(cmdType, hexArr);
			break;
		}
		case EPPT_SetStepY:
		{
			cmdType = static_cast<int>(ProtocolPrint::SetParam_AxisUnitMove);
			auto valData = this->findChild<QLineEdit*>("EPPT_SetStepY")->text();
			hexArr = Utils::GetInstance().MultiMicroDisStrTo12BytesHex(valData, false, false);
			operStr.append(QString::fromLocal8Bit("设置Y轴步进值"));
			logStr.append(QString::fromLocal8Bit("设置打印终点信息"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(hexArr.toHex(' ').toUpper())));
			
			m_motionSDK->MC_SendCmd(cmdType, hexArr);
			break;
		}
		case EPPT_SetStepZ:
		{
			cmdType = static_cast<int>(ProtocolPrint::SetParam_AxisUnitMove);
			auto valData = this->findChild<QLineEdit*>("EPPT_SetStepZ")->text();
			hexArr = Utils::GetInstance().MultiMicroDisStrTo12BytesHex(valData, false, false);
			operStr.append(QString::fromLocal8Bit("设置Z轴步进值"));
			logStr.append(QString::fromLocal8Bit("设置打印终点信息"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(hexArr.toHex(' ').toUpper())));
			
			m_motionSDK->MC_SendCmd(cmdType, hexArr);
			break;
		}
		case EPPT_SetClenaPos:
		{
			cmdType = static_cast<int>(ProtocolPrint::SetParam_CleanPos);
			auto valData = this->findChild<QLineEdit*>("EPPT_SetClenaPos")->text();
			hexArr = Utils::GetInstance().MultiMicroDisStrTo12BytesHex(valData, false, false);
			operStr.append(QString::fromLocal8Bit("设置清洗位置"));
			logStr.append(QString::fromLocal8Bit("设置打印终点信息"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(hexArr.toHex(' ').toUpper())));
			
			m_motionSDK->MC_SendCmd(cmdType, hexArr);
			break;
		}
		case EPPT_SetLimitXPos:
		{
			cmdType = static_cast<int>(ProtocolPrint::SetParam_MaxLimitPos);
			auto valData = this->findChild<QLineEdit*>("EPPT_SetLimitXPos")->text();
			hexArr = Utils::GetInstance().MultiMicroDisStrTo12BytesHex(valData, false, false);
			operStr.append(QString::fromLocal8Bit("设置X轴限位"));
			logStr.append(QString::fromLocal8Bit("设置打印终点信息"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(hexArr.toHex(' ').toUpper())));

			m_motionSDK->MC_SendCmd(cmdType, hexArr);
			break;
		}
		case EPPT_SetLimitYPos:
		{
			cmdType = static_cast<int>(ProtocolPrint::SetParam_MaxLimitPos);
			auto valData = this->findChild<QLineEdit*>("EPPT_SetLimitYPos")->text();
			hexArr = Utils::GetInstance().MultiMicroDisStrTo12BytesHex(valData, false, false);
			operStr.append(QString::fromLocal8Bit("设置Y轴限位"));
			logStr.append(QString::fromLocal8Bit("设置打印终点信息"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(hexArr.toHex(' ').toUpper())));

			m_motionSDK->MC_SendCmd(cmdType, hexArr);
			break;
		}
		case EPPT_SetLimitZPos:
		{
			cmdType = static_cast<int>(ProtocolPrint::SetParam_MaxLimitPos);
			auto valData = this->findChild<QLineEdit*>("EPPT_SetLimitZPos")->text();
			hexArr = Utils::GetInstance().MultiMicroDisStrTo12BytesHex(valData, false, false);
			operStr.append(QString::fromLocal8Bit("设置Z轴限位"));
			logStr.append(QString::fromLocal8Bit("设置打印终点信息"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(hexArr.toHex(' ').toUpper())));

			m_motionSDK->MC_SendCmd(cmdType, hexArr);
			break;
		}
		case EPPT_SetSpdX:
		{
			cmdType = static_cast<int>(ProtocolPrint::SetParam_AxistSpd);
			auto valData = this->findChild<QLineEdit*>("EPPT_SetSpdX")->text();
			hexArr = Utils::GetInstance().MultiMicroDisStrTo12BytesHex(valData, false, false);
			operStr.append(QString::fromLocal8Bit("设置X轴速度"));
			logStr.append(QString::fromLocal8Bit("设置打印终点信息"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(hexArr.toHex(' ').toUpper())));

			m_motionSDK->MC_SendCmd(cmdType, hexArr);
			break;
		}
		case EPPT_SetSpdY:
		{
			cmdType = static_cast<int>(ProtocolPrint::SetParam_AxistSpd);
			auto valData = this->findChild<QLineEdit*>("EPPT_SetSpdY")->text();
			hexArr = Utils::GetInstance().MultiMicroDisStrTo12BytesHex(valData, false, false);
			operStr.append(QString::fromLocal8Bit("设置Y轴速度"));
			logStr.append(QString::fromLocal8Bit("设置打印终点信息"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(hexArr.toHex(' ').toUpper())));
			
			m_motionSDK->MC_SendCmd(cmdType, hexArr);
			break;
		}
		case EPPT_SetSpdZ:
		{
			cmdType = static_cast<int>(ProtocolPrint::SetParam_AxistSpd);
			auto valData = this->findChild<QLineEdit*>("EPPT_SetSpdZ")->text();
			hexArr = Utils::GetInstance().MultiMicroDisStrTo12BytesHex(valData, false, false);
			operStr.append(QString::fromLocal8Bit("设置Z轴速度"));
			logStr.append(QString::fromLocal8Bit("设置打印终点信息"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(hexArr.toHex(' ').toUpper())));
			
			m_motionSDK->MC_SendCmd(cmdType, hexArr);
			break;
		}
		default:
			break;
	}

	emit SigAddShowOperCmd(operStr,"" , ShowEditType::ESET_OperComm);
	emit SigAddShowOperCmd(logStr);
	LOG_INFO(QString("print_UI_module print_btn_oper cur_oper_comm = %1")
		.arg(logStr.toStdString().c_str()));
}

void PrintDeviceUI::OnHandlePrintMoveFun(int idx)
{
	auto btnType = static_cast<EPrintMoveOperFun>(idx);
	QString operStr, logStr;
	QByteArray sendData;

	auto bTransData = false;
	int cmdType = 0;

	switch (btnType)
	{
		case EPMOF_XAxisLMove:
		{
			cmdType = static_cast<int>(ProtocolPrint::Ctrl_XAxisLMove);
			//获取移动的数据，单位um
			auto valData = this->findChild<QLineEdit*>("EPMOF_XAxisLMove")->text();
			auto data = Utils::GetInstance().MicroDisStrTo4BytesHex(valData, false,false);
			sendData.append(data);
			sendData.append(static_cast<char>(0x00));		// Y轴补0
			sendData.append(static_cast<char>(0x00));		// Z轴补0

			operStr.append(QString::fromLocal8Bit("开始X轴L移操作"));
			logStr.append(QString::fromLocal8Bit("开始X轴L移操作"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(sendData.toHex(' ').toUpper())));
			break;
		}
		case EPMOF_XAxisRMove:
		{
			cmdType = static_cast<int>(ProtocolPrint::Ctrl_XAxisRMove);
			//获取移动的数据，单位um
			auto valData = this->findChild<QLineEdit*>("EPMOF_XAxisLMove")->text();
			auto data = Utils::GetInstance().MicroDisStrTo4BytesHex(valData, false, false);
			sendData.append(data);
			sendData.append(static_cast<char>(0x00));		// Y轴补0
			sendData.append(static_cast<char>(0x00));		// Z轴补0

			operStr.append(QString::fromLocal8Bit("开始X轴R移操作"));
			logStr.append(QString::fromLocal8Bit("开始X轴R移操作"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(sendData.toHex(' ').toUpper())));
			break;
		}
		case EPMOF_YAxisLMove:
		{
			cmdType = static_cast<int>(ProtocolPrint::Ctrl_YAxisLMove);
			//获取移动的数据，单位um
			auto valData = this->findChild<QLineEdit*>("EPMOF_XAxisLMove")->text();
			auto data = Utils::GetInstance().MicroDisStrTo4BytesHex(valData, false, false);
			sendData.append(data);
			sendData.prepend(static_cast<char>(0x00));		// X轴补0
			sendData.append(static_cast<char>(0x00));		// Z轴补0

			operStr.append(QString::fromLocal8Bit("开始Y轴L移操作"));
			logStr.append(QString::fromLocal8Bit("开始Y轴L移操作"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(sendData.toHex(' ').toUpper())));

			break;
		}
		case EPMOF_YAxisRMove:
		{
			cmdType = static_cast<int>(ProtocolPrint::Ctrl_YAxisRMove);
			//获取移动的数据，单位um
			auto valData = this->findChild<QLineEdit*>("EPMOF_YAxisRMove")->text();
			auto data = Utils::GetInstance().MicroDisStrTo4BytesHex(valData, false, false);
			sendData.append(data);
			sendData.prepend(static_cast<char>(0x00));		// X轴补0
			sendData.append(static_cast<char>(0x00));		// Z轴补0

			operStr.append(QString::fromLocal8Bit("开始Y轴R移操作"));
			logStr.append(QString::fromLocal8Bit("开始Y轴R移操作"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(sendData.toHex(' ').toUpper())));
			break;
		}
		case EPMOF_ZAxisLMove:
		{
			cmdType = static_cast<int>(ProtocolPrint::Ctrl_ZAxisLMove);
			//获取移动的数据，单位um
			auto valData = this->findChild<QLineEdit*>("EPMOF_ZAxisLMove")->text();
			auto data = Utils::GetInstance().MicroDisStrTo4BytesHex(valData, false, false);
			sendData.append(data);
			sendData.prepend(static_cast<char>(0x00));		// X轴补0
			sendData.prepend(static_cast<char>(0x00));		// Z轴补0

			operStr.append(QString::fromLocal8Bit("开始Z轴L移操作"));
			logStr.append(QString::fromLocal8Bit("开始Z轴L移操作"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(sendData.toHex(' ').toUpper())));
			break;
		}
		case EPMOF_ZAxisRMove:
		{
			cmdType = static_cast<int>(ProtocolPrint::Ctrl_ZAxisRMove);
			//获取移动的数据，单位um
			auto valData = this->findChild<QLineEdit*>("EPMOF_ZAxisRMove")->text();
			auto data = Utils::GetInstance().MicroDisStrTo4BytesHex(valData, false, false);
			sendData.append(data);
			sendData.prepend(static_cast<char>(0x00));		// X轴补0
			sendData.prepend(static_cast<char>(0x00));		// Z轴补0

			operStr.append(QString::fromLocal8Bit("开始Z轴R移操作"));
			logStr.append(QString::fromLocal8Bit("开始Z轴R移操作"));
			logStr.append(QString::fromLocal8Bit("数据区发送数据：%1").arg(QString(sendData.toHex(' ').toUpper())));
			break;
		}
		default:
			break;
	}
	emit SigAddShowOperCmd(logStr);
	emit SigAddShowOperCmd(operStr, "", ShowEditType::ESET_OperComm);
	m_motionSDK->MC_SendCmd(cmdType, sendData);
	LOG_INFO(QString("print_UI_module print_btn_oper cur_oper_comm = %1")
		.arg(logStr.toStdString().c_str()));
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

