#include "TcpClient.h"

TcpClient::TcpClient(QObject* parent /*= 0*/)
	:QObject(parent)
{
	m_workThread = new QThread;
	m_impl = new TcpClientImpl;
	qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
	qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");

	connect(m_workThread, SIGNAL(finished()), m_workThread, SLOT(deleteLater()));
	connect(m_workThread, SIGNAL(finished()), m_impl, SLOT(deleteLater()));

	connect(m_impl, &TcpClientImpl::sigNewData, this, &TcpClient::sigNewData, Qt::DirectConnection);
	connect(m_impl, &TcpClientImpl::sigError, this, &TcpClient::sigError);
	connect(m_impl, &TcpClientImpl::sigSocketState, this, &TcpClient::sigSocketStateChanged);

	m_impl->moveToThread(m_workThread);

	m_workThread->start(); 
}

TcpClient::~TcpClient()
{
	if (m_workThread->isRunning())
	{
		m_workThread->quit();
		m_workThread->wait();
	}
}

void TcpClient::setIpAndPort(QString strIp, ushort port)
{
	m_impl->setIpPort(strIp, port);
}

void TcpClient::connectToHost()
{
	QMetaObject::invokeMethod(m_impl, [=]() {
		m_impl->onConnect();
	}, Qt::QueuedConnection);
}

void TcpClient::disconnectFromHost()
{
	QMetaObject::invokeMethod(m_impl, [=]() {
		m_impl->onDisconnect();
	}, Qt::QueuedConnection);
}

bool TcpClient::isConnected()
{
	bool ret = false;
	QMetaObject::invokeMethod(m_impl, "isConnected", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, ret));
	return ret;
}

void TcpClient::sendData(QByteArray data)
{
	QMetaObject::invokeMethod(m_impl, [=]() {
		m_impl->sendData(data);
	}, Qt::QueuedConnection);
}



TcpClientImpl::TcpClientImpl(QObject* parent /*= nullptr*/)
	:QObject(parent)
{
	m_tcpsocket = new QTcpSocket(this);
	m_tcpsocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

	m_timer = new QTimer(this);

	connect(m_tcpsocket, &QTcpSocket::stateChanged, this, &TcpClientImpl::onStateChanged);
	connect(m_tcpsocket, SIGNAL(readyRead()), this, SLOT(onReadData()), Qt::DirectConnection);
	connect(m_tcpsocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
	connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()), Qt::DirectConnection);

	m_timer->start(20);
}

TcpClientImpl::~TcpClientImpl()
{
	/*
	1. 停timer
	2. 断开socket
	3. 清空队列
	4. 删除对象
	*/

	if (m_timer) 
	{
		m_timer->stop();
		disconnect(m_timer, nullptr, this, nullptr);  // 断开所有连接
	}

	if (m_tcpsocket) 
	{
		disconnect(m_tcpsocket, nullptr, this, nullptr);
		if (m_tcpsocket->state() != QAbstractSocket::UnconnectedState) 
		{
			m_tcpsocket->abort();  // 立即断开
		}
	}
	{
		QMutexLocker lock(&m_sendMutex);
		m_sendLists.clear();
	}

	delete m_tcpsocket;
	delete m_timer;
	m_tcpsocket = nullptr;
	m_timer = nullptr;
}

void TcpClientImpl::sendData(QByteArray data)
{
	QMutexLocker lock(&m_sendMutex);
	m_sendLists.push_back(data);
}

void TcpClientImpl::setIpPort(QString strIp, ushort port)
{
	m_destinationIp = strIp;
	m_port = port;
}

bool TcpClientImpl::isConnected()
{
	QAbstractSocket::SocketState state = m_tcpsocket->state();
	if (state == QAbstractSocket::ConnectedState)
	{
		return true;
	}
	return false;
}

void TcpClientImpl::onConnect()
{
	m_tcpsocket->connectToHost(m_destinationIp, m_port);
}

void TcpClientImpl::onDisconnect()
{
	m_tcpsocket->disconnectFromHost();
}

void TcpClientImpl::onReadData()
{
	while (m_tcpsocket->bytesAvailable())
	{
		QNetworkDatagram data = m_tcpsocket->readAll();
		emit sigNewData(data.data());
	}
}

void TcpClientImpl::onTimeout()
{
	QMutexLocker lock(&m_sendMutex);

	if (m_sendLists.size() == 0)
	{
		return;
	}

	if (m_tcpsocket->state() == QAbstractSocket::ConnectedState)
	{
		m_tcpsocket->write(m_sendLists.first());

		m_sendLists.removeFirst();
	}
	else
	{
		m_sendLists.clear();
	}
}

void TcpClientImpl::onError(QAbstractSocket::SocketError socketError)
{
	emit sigError(socketError);
}

void TcpClientImpl::onStateChanged(QAbstractSocket::SocketState state)
{
	emit sigSocketState(state);
}
