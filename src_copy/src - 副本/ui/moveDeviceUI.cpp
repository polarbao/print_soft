#include "moveDeviceUI.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <functional>
#include <memory>
#include <QComboBox>


MoveDeviceUI::MoveDeviceUI(QWidget *parent)
    : QWidget(parent)
{
	QPalette pal(this->palette());
	pal.setColor(QPalette::Background, Qt::blue);
	this->setAutoFillBackground(true);
	this->setPalette(pal);
	Init();
}

MoveDeviceUI::~MoveDeviceUI()
{
	
		
}

void MoveDeviceUI::Init()
{
	InitUI();

	connect(&m_btnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &MoveDeviceUI::OnBtnClicked);

	connect(&m_btnGroup, QOverload<int, bool>::of(&QButtonGroup::buttonToggled), this, &MoveDeviceUI::OnBtnToggled);

	connect(this, &MoveDeviceUI::SigAddShowLog, this, &MoveDeviceUI::OnAddOperLog);


	int a = 0;

	for (auto& it : m_btnGroup.buttons())
	{
		a++;
	}
	auto b = 1;
}

void MoveDeviceUI::InitUI()
{
	QVBoxLayout* pv = new QVBoxLayout(this);
	pv->setContentsMargins(10, 10, 10, 10);
	pv->setSpacing(20);

	QGridLayout* pv_param = new QGridLayout(this);
	{
		pv_param->setContentsMargins(0, 0, 0, 0);
		pv_param->setSpacing(0);
		QHBoxLayout* pv_combox = new QHBoxLayout(this);
		{
			pv_combox->setContentsMargins(0, 0, 0, 0);
			pv_combox->setSpacing(10);

		}
		auto addCombox = [&](const QString& str, const QStringList& param) 
		{
			QVBoxLayout* pv_title = new QVBoxLayout(this);
			QLabel* title = new QLabel(str);
			QComboBox* combox = new QComboBox();
			combox->setMinimumSize(50, 30);
			//std::shared_ptr<QComboBox> combox = std::make_shared<QComboBox>();
			combox->addItems(param);

			pv_title->setContentsMargins(0, 0, 0, 0);
			pv_title->setSpacing(2);
			pv_title->addWidget(title);
			pv_title->addWidget(combox);
			pv_combox->addLayout(pv_title);
		};
		
		//查端口
		QStringList list;
		for (auto& it : QSerialPortInfo::availablePorts())
		{
			list.append(it.portName());
		}

		addCombox(QString::fromLocal8Bit("端 口"), list);
		addCombox(QString::fromLocal8Bit("波特率"), { "2400", "4800", "9600" ,"19200" ,"38400" ,"57600", "115200" });
		addCombox(QString::fromLocal8Bit("数据位"), { "5", "6" ,"7", "8" });
		addCombox(QString::fromLocal8Bit("停止位"), { "1", "1.5" ,"2" });
		addCombox(QString::fromLocal8Bit("校验位"), { "none", "odd" ,"even" });

		pv_param->addWidget(new QLabel(QString::fromLocal8Bit("串口参数设置")), 0, 1);
		pv_param->addLayout(pv_combox,1, 1);
	}
;

	QHBoxLayout* ph_btn = new QHBoxLayout(this);
	ph_btn->setContentsMargins(0, 0, 0, 0);
	ph_btn->setSpacing(0);

	QHBoxLayout* ph_comm = new QHBoxLayout(this);
	{
		m_inputComm.setPlaceholderText(QString::fromLocal8Bit("请输入发送指令"));
		QPushButton* sendBtn = new QPushButton(QString::fromLocal8Bit("发送"));
		connect(sendBtn, &QPushButton::clicked, this, [&](bool bClicked)
		{
			auto serialStr = m_inputComm.text();
			emit SigSendSerialComm(serialStr);
		});

		ph_comm->setContentsMargins(0, 0, 0, 0);
		ph_comm->setSpacing(15);
		ph_comm->addWidget(&m_inputComm, 3);
		ph_comm->addWidget(sendBtn, 1);
	}


	
	QHBoxLayout* ph_showOper = new QHBoxLayout(this);
	{
		m_logShow.setReadOnly(true);
		QPushButton* clearBtn = new QPushButton(QString::fromLocal8Bit("清空"));

		connect(clearBtn, &QPushButton::clicked, this, [&](bool bClicked) 
		{
			m_logShow.clear();
			m_logShow.append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
			m_logShow.append(QString::fromLocal8Bit("完成清理日志操作"));
		});

		m_inputComm.setPlaceholderText(QString::fromLocal8Bit("请输入发送指令"));

		ph_showOper->setContentsMargins(0, 0, 0, 0);
		ph_showOper->setSpacing(15);
		ph_showOper->addWidget(&m_logShow);
		ph_showOper->addWidget(clearBtn);

	}

	
	pv->addWidget(new QLabel(QString::fromLocal8Bit("打印机控制模块")));
	pv->addLayout(pv_param, 2);
	pv->addLayout(ph_btn, 3);
	pv->addLayout(ph_comm, 2);
	pv->addLayout(ph_showOper, 4);
	pv->addStretch(2);


	std::function<void(const QString&, const EUI::EMUIBT&)> addBtn = [&](const QString& btnName, const EUI::EMUIBT& btnType)
	{
		QVBoxLayout*pv = new QVBoxLayout();
		pv->setContentsMargins(0, 0, 0, 0);
		pv->setSpacing(2);
		QPushButton* forwardBtn = new QPushButton();
		forwardBtn->setMaximumSize(80, 35);
		forwardBtn->setText(btnName + "+");
		QPushButton* backwardBtn = new QPushButton();
		backwardBtn->setMaximumSize(80, 35);
		backwardBtn->setText(btnName + "-");
	
		if (EUI::EMUIBT_XAxis <= btnType && EUI::EMUIBT_ZAxis >= btnType)
		{
			pv->addWidget(forwardBtn);
			pv->addWidget(backwardBtn);
		}
	
		switch (btnType)	
		{
		case EUI::EMUIBT_XAxis:
		{
			m_btnGroup.addButton(forwardBtn, EUI::EMUIBT_XAxisForward);
			m_btnGroup.addButton(backwardBtn, EUI::EMUIBT_XAxisForward);
			break;
		}

		case EUI::EMUIBT_YAxis:
		{			
			m_btnGroup.addButton(forwardBtn, EUI::EMUIBT_YAxisForward);
			m_btnGroup.addButton(backwardBtn, EUI::EMUIBT_YAxisForward);
			break;
		}

		case EUI::EMUIBT_ZAxis:
		{
			m_btnGroup.addButton(forwardBtn, EUI::EMUIBT_ZAxisForward);
			m_btnGroup.addButton(backwardBtn, EUI::EMUIBT_ZAxisForward);
			break;
		}
		case EUI::EMUIBT_Reset:
		case EUI::EMUIBT_EmergencyStop:
		case EUI::EMUIBT_Stop:
		{
			QPushButton* tmp = new QPushButton();
			tmp->setMaximumSize(80, 35);
			tmp->setText(btnName);
			pv->addWidget(tmp);
			m_btnGroup.addButton(tmp, btnType);
			break;
		}
		default:
			break;
		}
		ph_btn->addLayout(pv);
	};

	addBtn("xAxis", EUI::EMUIBT_XAxis);
	addBtn("yAxis", EUI::EMUIBT_YAxis);
	addBtn("zAxis", EUI::EMUIBT_ZAxis);
	addBtn("reset", EUI::EMUIBT_Reset);
	addBtn("emergencyStop", EUI::EMUIBT_EmergencyStop);
	addBtn("stop", EUI::EMUIBT_Stop);


}

void MoveDeviceUI::OnBtnClicked(int id)
{
	auto btnType = static_cast<EUI::EMUIBT>(id);
	auto x = ELogic::EDMDT_Begin;
	QString logStr(QString::fromLocal8Bit("当前操作："));
	switch (btnType)
	{
		case EUI::EMUIBT_XAxisForward:
		case EUI::EMUIBT_XAxisBackward:
		{
			x = ELogic::EDMDT_XAxis;
			logStr.append(QString::fromLocal8Bit(""));
			break;
		}
		case EUI::EMUIBT_YAxisForward:
		case EUI::EMUIBT_YAxisBackword:
		{
			break;
		}
		case EUI::EMUIBT_ZAxisForward:
		case EUI::EMUIBT_ZAxisBackward:
		{
			break;
		}
		case EUI::EMUIBT_Reset:
		{
			break;
		}
	default:
		break;
	}
	emit SigBtnClicked(x);
	emit SigAddShowLog(logStr);
	LOG_INFO("");
}

void MoveDeviceUI::OnBtnToggled(int id, bool bToggle)
{
	auto btnType = static_cast<EUI::EMUIBT>(id);

	switch (btnType)
	{
	case EUI::EMUIBT_XAxisForward:
	case EUI::EMUIBT_XAxisBackward:
	{
		//逻辑处理
		
		break;
	}
	case EUI::EMUIBT_YAxisForward:
	case EUI::EMUIBT_YAxisBackword:
	{
		break;
	}
	case EUI::EMUIBT_ZAxisForward:
	case EUI::EMUIBT_ZAxisBackward:
	{
		break;
	}
	case EUI::EMUIBT_Reset:
	{
		break;
	}
	default:
		break;
	}
	
}

void MoveDeviceUI::OnAddOperLog(const QString& logStr)
{
	m_logShow.append("\n");
	//添加时间
	m_logShow.append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	m_logShow.append(logStr);
}
