#include "SerialPortData.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QApplication>
#include <QDir>
#include <QSettings>

SerialPortData::SerialPortData(QObject* parent, QString configFileName)
	: QObject(parent)
{
	qRegisterMetaType<QSerialPort::SerialPortError>("QSerialPort::SerialPortError");

	m_configFileName = configFileName;
	m_serialPortName = "COM1";
	m_eBaudRate = QSerialPort::Baud115200;
	m_eDataBit = QSerialPort::Data8;
	m_eParityBit = QSerialPort::NoParity;
	m_eStopBit = QSerialPort::OneStop;
	m_eFlowControl = QSerialPort::NoFlowControl;

	readConfig();

	m_thread = new QThread;
	m_impl = new SerialPortImpl;

	connect(m_thread, SIGNAL(finished()), m_thread, SLOT(deleteLater()));
	connect(m_thread, SIGNAL(finished()), m_impl, SLOT(deleteLater()));

	connect(m_impl, &SerialPortImpl::sigNewData, this, &SerialPortData::sigNewData);
	connect(m_impl, &SerialPortImpl::sigError, this, &SerialPortData::sigError);

	m_impl->moveToThread(m_thread);

	m_thread->start();

}

SerialPortData::~SerialPortData()
{
	if (m_thread->isRunning())
	{
		m_thread->quit();
		m_thread->wait();
	}
}

bool SerialPortData::openStatus()
{
	bool ret = m_impl->openStatus();
	//QMetaObject::invokeMethod(m_impl, "openStatus", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, ret));
	return ret;
}

void SerialPortData::openSerialPort()
{
	//阻塞等待结果
	/*bool ret = false;
	QMetaObject::invokeMethod(m_impl, "onOpen", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, ret), Q_ARG(QString, m_serialPortName),
		Q_ARG(QSerialPort::BaudRate, m_eBaudRate),Q_ARG(QSerialPort::DataBits, m_eDataBit),Q_ARG(QSerialPort::Parity, m_eParityBit),
		Q_ARG(QSerialPort::StopBits, m_eStopBit), Q_ARG(QSerialPort::FlowControl, m_eFlowControl) );*/

	//非阻塞不用等待结果
	QMetaObject::invokeMethod(m_impl, [=]() {
		m_impl->onOpen(m_serialPortName, m_eBaudRate, m_eDataBit, m_eParityBit, m_eStopBit, m_eFlowControl);
	}, Qt::QueuedConnection);

}

void SerialPortData::closeSerialPort()
{
	QMetaObject::invokeMethod(m_impl, [=]() {
		m_impl->onClose();
	}, Qt::QueuedConnection);
}

void SerialPortData::readConfig()
{
	QString sFilePath = qApp->applicationDirPath() + "/data/config";
	QDir dir(sFilePath);
	if (!dir.exists())
	{
		dir.mkdir(sFilePath);
	}
	QFile file(sFilePath + "/" + m_configFileName);
	if (!file.exists())
	{
		file.open(QFile::ReadWrite);
		file.close();
		QSettings settings(sFilePath + "/" + m_configFileName, QSettings::IniFormat);
		settings.beginGroup("com");
		settings.setValue("PortName", "COM1");
		settings.setValue("BaudRate", m_eBaudRate);
		settings.setValue("DataBit", m_eDataBit);
		settings.setValue("ParityBit", m_eParityBit);
		settings.setValue("StopBit", m_eStopBit);
		settings.endGroup();
	}
	else
	{
		QSettings settings(sFilePath + "/" + m_configFileName, QSettings::IniFormat);
		settings.beginGroup("com");
		m_serialPortName = settings.value("PortName", "COM1").toString();
		m_eBaudRate = (QSerialPort::BaudRate)settings.value("BaudRate", QSerialPort::Baud9600).toInt();
		m_eDataBit = (QSerialPort::DataBits)settings.value("DataBit", QSerialPort::Data8).toInt();
		m_eParityBit = (QSerialPort::Parity)settings.value("ParityBit", QSerialPort::NoParity).toInt();
		m_eStopBit = (QSerialPort::StopBits)settings.value("StopBit", QSerialPort::OneStop).toInt();
		settings.endGroup();
	}

}

void SerialPortData::sendData(QByteArray data)
{
	QMetaObject::invokeMethod(m_impl, [=]() {
		m_impl->sendData(data);
	}, Qt::QueuedConnection);

}

SerialPortImpl::SerialPortImpl(QObject* parent /*= nullptr*/)
{
	qRegisterMetaType<QSerialPort::SerialPortError>("QSerialPort::SerialPortError");

	m_Timer = new QTimer(this);
	m_pSerialPort = new QSerialPort(this);

	connect(m_pSerialPort, &QSerialPort::readyRead, this, &SerialPortImpl::onReadData, Qt::DirectConnection);
	connect(m_pSerialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(onError(QSerialPort::SerialPortError)));
	connect(m_Timer, SIGNAL(timeout()), this, SLOT(onTimeout()), Qt::DirectConnection);

	m_Timer->start(20);

}

SerialPortImpl::~SerialPortImpl()
{
	m_Timer->stop();
	m_sendLists.clear();
	delete m_Timer;
	delete m_pSerialPort;
}

void SerialPortImpl::RefreshPorts()
{
	//portCombo->clear();
	//foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
	//	portCombo->addItem(info.portName() + " (" + info.description() + ")");
	//}
}

void SerialPortImpl::sendData(QByteArray data)
{
	QMutexLocker lock(&m_sendMutex);
	m_sendLists.append(data);
}

bool SerialPortImpl::openStatus()
{
	return m_pSerialPort->isOpen(); 
}

bool SerialPortImpl::onOpen(QString serialPortName, QSerialPort::BaudRate baudrate, QSerialPort::DataBits databit, QSerialPort::Parity parity, QSerialPort::StopBits stopBit, QSerialPort::FlowControl flowControl)
{
	if (!m_pSerialPort)
	{
		return false;
	}
	if (m_pSerialPort->portName() == serialPortName && m_pSerialPort->isOpen())
	{
		return true;
	}
	if (m_pSerialPort->isOpen())
	{
		m_pSerialPort->close();
	}

	m_pSerialPort->setPortName(serialPortName);   // 串口号
	m_pSerialPort->setBaudRate(baudrate);        // 波特率
	m_pSerialPort->setDataBits(databit);         // 数据位
	m_pSerialPort->setParity(parity);         // 奇偶校验位
	m_pSerialPort->setStopBits(stopBit);         // 停止位
	m_pSerialPort->setFlowControl(flowControl);  // 流控制

	bool bRtn = m_pSerialPort->open(QIODevice::ReadWrite);
	if (false == bRtn)
	{
		return false;
	}

	return true;
}

void SerialPortImpl::onClose()
{
	if (m_pSerialPort->isOpen())
	{
		m_pSerialPort->close();
	}
}

void SerialPortImpl::onReadData()
{
	QByteArray data = m_pSerialPort->readAll();

	emit sigNewData(data);
}

void SerialPortImpl::onError(QSerialPort::SerialPortError error)
{
	emit sigError(error);
}

void SerialPortImpl::onTimeout()
{
	QMutexLocker lock(&m_sendMutex);
	if (!m_pSerialPort->isOpen())
	{
		return;
	}

	if (m_sendLists.size() == 0)
	{
		return;
	}

	m_pSerialPort->write(m_sendLists.first());

	m_sendLists.removeFirst();
}
