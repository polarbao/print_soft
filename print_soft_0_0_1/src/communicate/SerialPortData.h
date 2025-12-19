#pragma once
#include <QtCore/QMap>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QMutex>
#include <QtSerialPort/QSerialPort>

class SerialPortImpl;

/** 
*  @author      
*  @class       SerialPortData 
*  @brief       串口通讯对象 
*/
class SerialPortData : public QObject
{
	Q_OBJECT
public:
	/** 
	*  @brief       构造函数 
	*  @param[in]    parent: 父对象   configFileName:配置文件
	*  @param[out]   
	*  @return                    
	*/
	SerialPortData(QObject* parent = nullptr, QString configFileName = "set.ini");

	~SerialPortData();

	//串口打开状态
	bool openStatus();

	//打开串口
	void openSerialPort();

	//关闭串口
	void closeSerialPort();

	//发送数据
	void sendData(QByteArray data);

signals:
	//新消息信号
	void sigNewData(QByteArray msg);

	//错误信号
	void sigError(QSerialPort::SerialPortError error);
	

private:
	//读取配置
	void readConfig();

public:
	/** 串口名称  */
	QString                m_serialPortName;

	/** 波特率  */
	QSerialPort::BaudRate  m_eBaudRate;

	/** 数据位  */
	QSerialPort::DataBits  m_eDataBit;

	/** 奇偶校验位 */
	QSerialPort::Parity    m_eParityBit;

	/** 停止位  */
	QSerialPort::StopBits  m_eStopBit;

	/** 控制流  */
	QSerialPort::FlowControl m_eFlowControl;

private:
	//配置文件名称
	QString m_configFileName;

	SerialPortImpl* m_impl;

	QThread* m_thread;
};


class SerialPortImpl : public QObject
{
	Q_OBJECT
public:
	SerialPortImpl(QObject* parent = nullptr);
	~SerialPortImpl();

	/**
	*  @brief       刷新串口
	*  @param[in]
	*  @param[out]
	*  @return
	*/
	void RefreshPorts();

signals:
	//新消息信号
	void sigNewData(QByteArray msg);

	//连接返回的信号
	void sigError(QSerialPort::SerialPortError error);

public slots:
	/**
	*  @brief       发送数据
	*  @param[in]
	*  @param[out]
	*  @return
	*/
	void sendData(QByteArray data);

	bool onOpen(QString serialPortName, QSerialPort::BaudRate baudrate, QSerialPort::DataBits databit, QSerialPort::Parity parity, QSerialPort::StopBits stopBit, QSerialPort::FlowControl flowControl);
	void onClose();
	bool openStatus();

protected slots:
	void onReadData();
	void onError(QSerialPort::SerialPortError error);
	void onTimeout();
	
private:
	/**  串口对象  */
	QSerialPort*           m_pSerialPort;
	
	//线程定时器
	QTimer* m_Timer;

	//发送队列
	QList<QByteArray> m_sendLists;

	QMutex m_sendMutex;

};