#pragma once
#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>

class TcpClientImpl;

/** 
*  @author      
*  @class       TcpClient 
*  @brief       tcp客户端操作类 
*/
class TcpClient : public QObject
{
	Q_OBJECT
public:
	TcpClient(QObject* parent = 0);

	~TcpClient();

	/** 
	*  @brief       设置ip和端口 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	void setIpAndPort(QString strIp, ushort port);

	/** 
	*  @brief       连接到服务器 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	void connectToHost();

	/** 
	*  @brief       断开连接 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	void disconnectFromHost();

	/** 
	*  @brief       连接状态 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	bool isConnected();

	/** 
	*  @brief       发送数据 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	void sendData(QByteArray data);


signals:
	//新的数据到来信号
	void sigNewData(QByteArray msg);

	//错误信号
	void sigError(QAbstractSocket::SocketError socketError);

	//连接信号变化
	void sigSocketStateChanged(QAbstractSocket::SocketState state);

private:
	QThread* m_workThread;
	TcpClientImpl* m_impl;
};


/** 
*  @author      
*  @class       TcpClientImpl 
*  @brief       Tcp客户端实现类 
*/
class TcpClientImpl : public QObject
{
	Q_OBJECT
public:
	TcpClientImpl(QObject* parent = nullptr);
	~TcpClientImpl();

	void sendData(QByteArray data);
	void setIpPort(QString strIp, ushort port);


signals:
	void sigNewData(QByteArray msg);
	void sigError(QAbstractSocket::SocketError socketError);
	void sigSocketState(QAbstractSocket::SocketState state);

public slots:
	bool isConnected();
	void onConnect();
	void onDisconnect();
	void onReadData();
	void onTimeout();
	void onError(QAbstractSocket::SocketError socketError);
	void onStateChanged(QAbstractSocket::SocketState state);

private:
	QTcpSocket* m_tcpsocket;
	QList<QByteArray> m_sendLists;
	QMutex m_sendMutex;
	QTimer* m_timer;
	ushort m_port;
	QString m_destinationIp;
};