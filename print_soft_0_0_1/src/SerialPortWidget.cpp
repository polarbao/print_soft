#include "SerialPortWidget.h"
#include <QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "QPushButton"

SerialPortWidget::SerialPortWidget(SerialPortData* data, QWidget* parent /*= 0*/) 
	:QDialog(parent)
{
	m_pSerialPortObj = data;
	initUI();
	resize(400, 600);
	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
}

SerialPortWidget::~SerialPortWidget()
{
}

void SerialPortWidget::onDataComing(QByteArray msg)
{
	if (m_outTextEdit->toPlainText().count() > 10000)
	{
		m_outTextEdit->clear();
	}
	//msg.append("\n");
	m_outTextEdit->append(msg.toHex(' '));
}

void SerialPortWidget::onError(QSerialPort::SerialPortError error)
{
	switch (error)
	{
	case QSerialPort::NoError:
		m_pstatusLab->setText(QStringLiteral("串口%1打开成功").arg(m_pSerialPortObj->m_serialPortName));
		m_pstatusLab->setStyleSheet("color:blue");
		m_setComBtn->setText(u8"点击关闭串口");
		break;
	case QSerialPort::OpenError:
		m_pstatusLab->setText(u8"串口打开失败");
		m_pstatusLab->setStyleSheet("color:red");
		m_setComBtn->setText(u8"点击打开串口");
		//LOG_DEBUG(u8"串口打开失败");
		break;
	case QSerialPort::UnknownError:
		m_pstatusLab->setText(QStringLiteral("未知错误"));
		m_pstatusLab->setStyleSheet("color:red");
		m_setComBtn->setText(u8"点击打开串口");
		//LOG_DEBUG(QStringLiteral("未知错误"));
		break;
	case QSerialPort::ReadError:
		m_pstatusLab->setText(QStringLiteral("读取数据出错"));
		m_pstatusLab->setStyleSheet("color:red");
		m_setComBtn->setText(u8"点击打开串口");
		//LOG_DEBUG(QStringLiteral("读取数据出错"));
		break;
	case QSerialPort::PermissionError:
		m_pstatusLab->setText(QStringLiteral("串口%1打开失败").arg(m_pSerialPortObj->m_serialPortName));
		m_pstatusLab->setStyleSheet("color:red");
		m_setComBtn->setText(u8"点击打开串口");
		break;
	default:
		m_pstatusLab->setText(QStringLiteral("串口%1打开失败").arg(m_pSerialPortObj->m_serialPortName));
		m_pstatusLab->setStyleSheet("color:red");
		m_setComBtn->setText(u8"点击打开串口");
	}
}

void SerialPortWidget::initUI()
{
	setWindowTitle(QStringLiteral("串口通讯设置"));

	QGroupBox* box1 = new QGroupBox(QStringLiteral("串口设置"));
	{
	
	}
	QGridLayout* layout = new QGridLayout;
	box1->setLayout(layout);

	QLabel* lab1 = new QLabel(u8"串口");
	QLabel* lab2 = new QLabel(QStringLiteral("波特率"));
	QLabel* lab3 = new QLabel(QStringLiteral("校验位"));
	QLabel* lab4 = new QLabel(QStringLiteral("数据位"));
	QLabel* lab5 = new QLabel(QStringLiteral("停止位"));
	QLabel* lab6 = new QLabel(QStringLiteral("状态"));

	m_portNameBox = new QComboBox;
	QSet<QString> m_portNames;
	QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
	for (auto it : infos)
	{
		m_portNames.insert(it.portName());
		m_portNameBox->addItem(it.portName(), it.portName());
	}

	m_baudRateBox = new QComboBox;
	m_baudRateBox->addItem("1200", QSerialPort::Baud1200);
	m_baudRateBox->addItem("2400", QSerialPort::Baud2400);
	m_baudRateBox->addItem("4800", QSerialPort::Baud4800);
	m_baudRateBox->addItem("9600", QSerialPort::Baud9600);
	m_baudRateBox->addItem("19200", QSerialPort::Baud19200);
	m_baudRateBox->addItem("38400", QSerialPort::Baud38400);
	m_baudRateBox->addItem("57600", QSerialPort::Baud57600);
	m_baudRateBox->addItem("115200", QSerialPort::Baud115200);

	m_parityBitBox = new QComboBox;
	m_parityBitBox->addItem(QStringLiteral("无"), QSerialPort::NoParity);
	m_parityBitBox->addItem(QStringLiteral("偶校验"), QSerialPort::EvenParity);
	m_parityBitBox->addItem(QStringLiteral("奇校验"), QSerialPort::OddParity);

	m_dataBitBox = new QComboBox;
	m_dataBitBox->addItem("5", QSerialPort::Data5);
	m_dataBitBox->addItem("6", QSerialPort::Data6);
	m_dataBitBox->addItem("7", QSerialPort::Data7);
	m_dataBitBox->addItem("8", QSerialPort::Data8);

	m_stopBitBox = new QComboBox;
	m_stopBitBox->addItem("1", QSerialPort::OneStop);
	m_stopBitBox->addItem("1.5", QSerialPort::OneAndHalfStop);
	m_stopBitBox->addItem("2", QSerialPort::TwoStop);

	m_setComBtn = new QPushButton(QStringLiteral("确定"));
	connect(m_setComBtn, SIGNAL(clicked()), this, SLOT(onSetComBtn()));

	m_pstatusLab = new QLabel;
	m_pstatusLab->setText(QStringLiteral("未打开"));

	m_inTextEdit = new QTextEdit;
	m_outTextEdit = new QTextEdit;


	QGroupBox* box2 = new QGroupBox(QStringLiteral("发送信息"));
	QPushButton* sendBtn = new QPushButton(QStringLiteral("发送"));
	{	
		QGridLayout* layout2 = new QGridLayout;
		box2->setLayout(layout2);
		layout2->addWidget(m_inTextEdit, 0, 0, 2, 3);
		layout2->addWidget(sendBtn, 3, 0, 1, 1);

	}


	QGroupBox* box3 = new QGroupBox(QStringLiteral("接收信息"));
	{
		QGridLayout* layout3 = new QGridLayout;
		box3->setLayout(layout3);
		layout3->addWidget(m_outTextEdit, 0, 0, 2, 3);

	}


	QGroupBox* box4 = new QGroupBox(QStringLiteral("按钮操作"));
	{
		QGridLayout* layout4 = new QGridLayout;
		QPushButton* sendBtn = new QPushButton(QStringLiteral("发送"));

		box4->setLayout(layout4);

	}


	layout->addWidget(lab1, 0, 0);
	layout->addWidget(m_portNameBox, 0, 1, 1, 1);
	layout->addWidget(lab2, 1, 0);
	layout->addWidget(m_baudRateBox, 1, 1);
	layout->addWidget(lab3, 2, 0);
	layout->addWidget(m_parityBitBox, 2, 1);
	layout->addWidget(lab4, 3, 0);
	layout->addWidget(m_dataBitBox, 3, 1);
	layout->addWidget(lab5, 4, 0);
	layout->addWidget(m_stopBitBox, 4, 1);
	layout->addWidget(m_setComBtn, 0, 2);
	layout->addWidget(lab6, 5, 0);
	layout->addWidget(m_pstatusLab, 5, 1, 1, 2);





	QGridLayout* mainlayout = new QGridLayout;
	mainlayout->addWidget(box1, 0, 0);
	mainlayout->addWidget(box2, 1, 0);
	mainlayout->addWidget(box3, 2, 0);
	mainlayout->addWidget(box3, 3, 0);

	setLayout(mainlayout);

	//可以调试时候打开观察数据
	connect(m_pSerialPortObj, SIGNAL(sigNewData(QByteArray)), this, SLOT(onDataComing(QByteArray)));
	connect(m_pSerialPortObj, SIGNAL(sigError(QSerialPort::SerialPortError)), this, SLOT(onError(QSerialPort::SerialPortError)));
	connect(sendBtn, &QPushButton::clicked, this, [&](bool bClicked) 
	{
		auto sendStr = m_inTextEdit->toPlainText();
		m_pSerialPortObj->sendData("11111");			
	});

	if (m_portNames.contains(m_pSerialPortObj->m_serialPortName))
	{
		m_portNameBox->setCurrentText(m_pSerialPortObj->m_serialPortName);
		int i = m_baudRateBox->findData(m_pSerialPortObj->m_eBaudRate);
		m_baudRateBox->setCurrentIndex(i);
		i = m_dataBitBox->findData(m_pSerialPortObj->m_eDataBit);
		m_dataBitBox->setCurrentIndex(i);
		i = m_parityBitBox->findData(m_pSerialPortObj->m_eParityBit);
		m_parityBitBox->setCurrentIndex(i);
		i = m_stopBitBox->findData(m_pSerialPortObj->m_eStopBit);
		m_stopBitBox->setCurrentIndex(i);

		if (m_pSerialPortObj->openStatus())
		{
			m_pstatusLab->setText(QStringLiteral("串口%1打开成功").arg(m_pSerialPortObj->m_serialPortName));
			m_pstatusLab->setStyleSheet("color:blue");
			m_setComBtn->setText(u8"点击关闭串口");
		}
		else
		{
			m_pstatusLab->setText(QStringLiteral("串口%1未打开").arg(m_pSerialPortObj->m_serialPortName));
			m_pstatusLab->setStyleSheet("color:red");
			m_setComBtn->setText(u8"点击打开串口");
		}
	}
	else
	{
		int i = m_baudRateBox->findData(m_pSerialPortObj->m_eBaudRate);
		m_baudRateBox->setCurrentIndex(i);
		i = m_dataBitBox->findData(m_pSerialPortObj->m_eDataBit);
		m_dataBitBox->setCurrentIndex(i);
		i = m_parityBitBox->findData(m_pSerialPortObj->m_eParityBit);
		m_parityBitBox->setCurrentIndex(i);
		i = m_stopBitBox->findData(m_pSerialPortObj->m_eStopBit);
		m_stopBitBox->setCurrentIndex(i);
	}


}


void SerialPortWidget::onSetComBtn()
{
	if (m_pSerialPortObj->openStatus())
	{
		m_pSerialPortObj->closeSerialPort();
		m_setComBtn->setText(u8"点击打开串口");
		m_pstatusLab->setText(QStringLiteral("串口%1已关闭").arg(m_pSerialPortObj->m_serialPortName));
		m_pstatusLab->setStyleSheet("color:red");
	}
	else
	{
		m_pSerialPortObj->m_eBaudRate = (QSerialPort::BaudRate)m_baudRateBox->currentData().toInt();
		m_pSerialPortObj->m_eDataBit = (QSerialPort::DataBits)m_dataBitBox->currentData().toInt();
		m_pSerialPortObj->m_eParityBit = (QSerialPort::Parity)m_parityBitBox->currentData().toInt();
		m_pSerialPortObj->m_eStopBit = (QSerialPort::StopBits)m_stopBitBox->currentData().toInt();
		m_pSerialPortObj->m_serialPortName = m_portNameBox->currentData().toString();

		m_pSerialPortObj->openSerialPort();

	}

}
