
#include "motionControlSDK.h"
#include "SDKManager.h"
#include "ProtocolPrint.h"

#include <QDebug>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QMetaObject>
#include "CLogManager.h"

/**
 * @class motionControlSDK::Private
 * @brief 私有实现类（Pimpl模式）
 *
 * 优点：
 * 1. 隐藏实现细节
 * 2. 减少头文件依赖
 * 3. 保持ABI稳定性
 * 4. 方便内部修改
 */
class motionControlSDK::Private
{
public:
	explicit Private(motionControlSDK* q)
		: q_ptr(q)
		, initialized(false)
		, connectedState(false)
		, port(0)
	{
	}

	~Private()
	{
	}

	motionControlSDK* q_ptr;
	bool initialized;
	bool connectedState;
	QString ip;
	quint16 port;

	// 静态回调函数（桥接C回调到Qt信号）
	static void sdkEventCallback(const SdkEvent* event);

	// 全局实例管理（用于C回调中访问Qt对象）
	static motionControlSDK* s_instance;
	static QMutex s_mutex;
};

// 静态成员初始化
motionControlSDK* motionControlSDK::Private::s_instance = nullptr;
QMutex motionControlSDK::Private::s_mutex;


motionControlSDK::motionControlSDK(QObject *parent)
	: QObject(parent)
	, d(new Private(this))
{
	QMutexLocker locker(&Private::s_mutex);
	Private::s_instance = this;
}

motionControlSDK::~motionControlSDK()
{
	release();

	QMutexLocker locker(&Private::s_mutex);
	if (Private::s_instance == this) 
	{
		Private::s_instance = nullptr;
	}

	delete d;
}

bool motionControlSDK::initialize(const QString& logDir)
{
	//qRegisterMetaType<MoveAxisPos>("MoveAxisPos");
	//qRegisterMetaType<PackParam>("PackParam");

	if (d->initialized) 
	{
		//qDebug() << "SDK已经初始化";
		return true;
	}

	// 初始化SDK
	// 注意：如果SDK独立运行（不在Qt应用程序中），需要创建QCoreApplication
	// 这里假设调用者会管理Qt事件循环
	bool ret = SDKManager::instance()->init("./");

	if (!ret) 
	{
		QString errMsg = tr("SDK初始化失败，错误码：%1").arg(ret);
		//LOG_INFO(QString(u8"SDK初始化失败"));
		emit errorOccurred(-1, errMsg);
		return false;
	}
	//LOG_INFO(QString(u8"SDK初始化成功"));

	// 注册事件回调函数（桥接C回调到Qt信号）
	//RegisterEventCallback(&Private::sdkEventCallback);
	QMutexLocker lock(&g_callbackMutex);
	g_sdkCallback = &Private::sdkEventCallback;

	d->initialized = true;
	//emit infoMessage(tr("SDK初始化成功"));

	qDebug() << "motionControlSDK initialized successfully";
	return true;
}

void motionControlSDK::release()
{
	if (!d->initialized) 
	{
		return;
	}

	// 断开连接
	if (d->connectedState) 
	{
		disconnectFromDevice();
	}

	// 释放底层SDK
	SDKManager::instance()->release();


	d->initialized = false;
	d->connectedState = false;
	d->ip.clear();
	d->port = 0;

	qDebug() << "motionControlSDK released";
}


bool motionControlSDK::isInitialized() const
{
	return d->initialized;
}

bool motionControlSDK::connectToDevice(const QString& ip, quint16 port)
{
	LOG_INFO("1111111111111connectToDevice");
	if (!d->initialized) 
	{
		//emit errorOccurred(-1, tr("SDK未初始化，请先调用initialize()"));
		//LOG_INFO(u8"SDK未初始化，请先调用initialize()");
		return false;
	}

	if (d->connectedState) 
	{
		//emit infoMessage(tr("设备已连接"));
		//LOG_INFO(u8"设备已连接");
		return true;
	}

	// 调用C接口连接设备
	int ret = SDKManager::instance()->connectByTCP(ip, port);
	if (ret != 0) 
	{
		QString errMsg = tr("连接失败，错误码：%1").arg(ret);
		//LOG_INFO(QString(u8"连接失败，错误码：%1").arg(ret));
		emit errorOccurred(ret, errMsg);
		return false;
	}

	// 保存连接信息
	d->ip = ip;
	d->port = port;
	emit deviceIpChanged(ip);
	emit devicePortChanged(port);
	emit infoMessage(tr("正在连接 %1:%2...").arg(ip).arg(port));
	LOG_INFO(QString(u8"正在连接 %1:%2...").arg(ip).arg(port));

	return true;
}

void motionControlSDK::disconnectFromDevice()
{
	if (!d->initialized) 
	{
		return;
	}

	if (!d->connectedState) 
	{
		//emit infoMessage(tr("设备未连接"));
		return;
	}

	// 调用C接口断开连接
	SDKManager::instance()->disconnect();

	emit infoMessage(tr("正在断开连接..."));
}

bool motionControlSDK::isConnected() const
{
	return d->connectedState;
}

QString motionControlSDK::deviceIp() const
{
	return d->ip;
}

quint16 motionControlSDK::devicePort() const
{
	return d->port;
}

void motionControlSDK::refreshConnectionStatus()
{
	bool connected = SDKManager::instance()->isConnected();
	if (d->connectedState != connected) 
	{
		d->connectedState = connected;
		emit connectedChanged(connected);

		//if (connected) 
		//{
		//	emit connected();
		//}
		//else 
		//{
		//	emit disconnected();
		//}
	}
}

// ==================== 运动控制 ====================
//单轴绝对
bool motionControlSDK::moveTo(const MoveAxisPos& posdData)
{
	if (!isConnected()) 
	{
		emit errorOccurred(-1, QString(u8"11"));
		return false;
	}
	int ret = 0;
	if (posdData.xPos != 0) 
	{
		ret |= SDKManager::instance()->moveXAxis(posdData);
	}
	if (posdData.yPos != 0)
	{
		ret |= SDKManager::instance()->moveYAxis(posdData);
	}
	if (posdData.zPos != 0)
	{
		ret |= SDKManager::instance()->moveZAxis(posdData);
	}

	if (ret != 0) 
	{
		emit errorOccurred(ret, tr(u8"MoveTo命令失败"));
		return false;
	}
	emit moveStatusChanged(tr(u8"移动到位置 (%.2f, %.2f, %.2f)").arg(posdData.xPos).arg(posdData.yPos).arg(posdData.zPos));
	return true;
}

//单轴相对
bool motionControlSDK::moveBy(double dx, double dy, double dz, double speed)
{
	if (!isConnected()) 
	{
		emit errorOccurred(-1, QString(u8"11"));
		return false;
	}

	int ret = 0;
	
	if (dx != 0) 
	{
		ret |= SDKManager::instance()->moveRelXAxis(dx);
	}
	if (dy != 0) 
	{
		ret |= SDKManager::instance()->moveRelXAxis(dy);
	}
	if (dz != 0) 
	{
		ret |= SDKManager::instance()->moveRelXAxis(dz);
	}	
	if (ret != 0) 
	{
		emit errorOccurred(ret, tr("MoveBy命令失败"));
		return false;
	}

	emit moveStatusChanged(tr("相对移动 (%.2f, %.2f, %.2f)").arg(dx).arg(dy).arg(dz));
	return true;
}

bool motionControlSDK::goHome()
{
	if (!isConnected()) 
	{
		emit errorOccurred(-1, QString(u8"dev_unconnect"));
		return false;
	}

	// 所有轴回原点
	// axisFlag = 7 表示 X(1) + Y(2) + Z(4) = 全部轴 
	int ret = SDKManager::instance()->resetAxis(7);
	if (ret != 0) 
	{
		emit errorOccurred(-1, QString(u8"go_home_cmd_failed"));
		return false;
	}

	emit moveStatusChanged(tr("go_homing..."));
	return true;
}

bool motionControlSDK::moveXAxis(double distance, double speed)
{
	return moveBy(distance, 0, 0, speed);
}

bool motionControlSDK::moveXAxis(const MoveAxisPos& targetPos)
{
	if (!d->initialized) 
	{
		emit errorOccurred(-1, tr(u8"SDK未初始化"));
		return false;
	}

	// 调用SDKManager的X轴移动
	int result = SDKManager::instance()->moveXAxis(targetPos);
	return (result == 0);
}

bool motionControlSDK::moveYAxis(double distance, double speed)
{
	return moveBy(0, distance, 0, speed);
}

bool motionControlSDK::moveYAxis(const MoveAxisPos& targetPos)
{
	if (!d->initialized) 
	{
		emit errorOccurred(-1, tr(u8"SDK未初始化"));
		return false;
	}

	// 调用SDKManager的Y轴移动
	int result = SDKManager::instance()->moveYAxis(targetPos);
	return (result == 0);
}

bool motionControlSDK::moveZAxis(double distance, double speed)
{
	return moveBy(0, 0, distance, speed);
}

bool motionControlSDK::moveZAxis(const MoveAxisPos& targetPos)
{
	if (!d->initialized) 
	{
		emit errorOccurred(-1, tr(u8"SDK未初始化"));
		return false;
	}

	// 调用SDKManager的Z轴移动
	int result = SDKManager::instance()->moveZAxis(targetPos);
	return (result == 0);
}



bool motionControlSDK::MC_move2RelSingleAxisPos(double dx, double dy, double dz)
{
	if (!d->initialized)
	{
		emit errorOccurred(-1, tr((u8"SDK未初始化")));
		return false;
	}

	int ret = 0;
	// 调用SDKManager的相对移动
	if (dx != 0)
	{
		ret |= SDKManager::instance()->moveRelXAxis(dx);
	}
	if (dy != 0)
	{
		ret |= SDKManager::instance()->moveRelXAxis(dy);
	}
	if (dz != 0)
	{
		ret |= SDKManager::instance()->moveRelXAxis(dz);
	}
	if (ret != 0)
	{
		emit errorOccurred(ret, tr(u8"MoveBy命令失败"));
		return false;
	}

	emit moveStatusChanged(tr(u8"motion_moudle 相对移动单一轴 (%.2f, %.2f, %.2f)").arg(dx).arg(dy).arg(dz));
	return true;
}

bool motionControlSDK::MC_move2AbsSingleAxisPos(const MoveAxisPos& targetPos)
{
	return false;
}

// ==================== 3轴同时移动 ====================

bool motionControlSDK::MC_move2RelAxisPos(double dx, double dy, double dz)
{
	if (!d->initialized)
	{
		emit errorOccurred(-1, tr((u8"SDK未初始化")));
		return false;
	}

	int ret = 0;
	// 调用SDKManager的相对移动
	if (dx != 0)
	{
		ret |= SDKManager::instance()->moveRelXAxis(dx);
	}
	if (dy != 0)
	{
		ret |= SDKManager::instance()->moveRelXAxis(dy);
	}
	if (dz != 0)
	{
		ret |= SDKManager::instance()->moveRelXAxis(dz);
	}
	if (ret != 0)
	{
		emit errorOccurred(ret, tr(u8"MoveBy命令失败"));
		return false;
	}

	emit moveStatusChanged(tr(u8"相对移动 (%.2f, %.2f, %.2f)").arg(dx).arg(dy).arg(dz));
	return true;
}


bool motionControlSDK::MC_move2AbsAxisPos(const MoveAxisPos& targetPos)
{
	if (!d->initialized) 
	{
		emit errorOccurred(-1, tr(u8"SDK未初始化"));
		return false;
	}

	// 调用SDKManager的3轴同时移动
	// 将MoveAxisPos结构体转换为QbyteArray，使用字节数据的moveToPosition()进行调用
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::BigEndian);
	// 写入X/Y/Z坐标（各4字节，大端序，微米单位）
	stream << targetPos.xPos;
	stream << targetPos.yPos;
	stream << targetPos.zPos;

	int result = SDKManager::instance()->moveToPosition(data);
	return (result == 0);
}

bool motionControlSDK::MC_move2AbsAxisPos(const QByteArray& positionData)
{
	if (!d->initialized)
	{
		emit errorOccurred(-1, tr(u8"SDK未初始化"));
		return false;
	}

	// 验证数据长度
	if (positionData.size() != 12)
	{
		emit errorOccurred(-1, tr(u8"位置数据长度必须为12字节"));
		return false;
	}

	// 调用SDKManager的3轴同时移动
	int result = SDKManager::instance()->moveToPosition(positionData);
	return (result == 0);
}

// ==================== 3轴同时移动 ====================

bool motionControlSDK::MC_moveToPosition(const MoveAxisPos& targetPos)
{
	if (!d->initialized) 
	{
		emit errorOccurred(-1, tr(u8"SDK未初始化"));
		return false;
	}

	// 调用SDKManager的3轴同时移动
	// 将MoveAxisPos结构体转换为QbyteArray，使用字节数据的moveToPosition()进行调用
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::BigEndian);
	// 写入X/Y/Z坐标（各4字节，大端序，微米单位）
	stream << targetPos.xPos;
	stream << targetPos.yPos;
	stream << targetPos.zPos;

	int result = SDKManager::instance()->moveToPosition(data);
	return (result == 0);
}

bool motionControlSDK::MC_moveToPosition(const QByteArray& positionData)
{
	if (!d->initialized) 
	{
		emit errorOccurred(-1, tr(u8"SDK未初始化"));
		return false;
	}

	// 验证数据长度
	if (positionData.size() != 12) 
	{
		emit errorOccurred(-1, tr(u8"位置数据长度必须为12字节"));
		return false;
	}

	// 调用SDKManager的3轴同时移动
	int result = SDKManager::instance()->moveToPosition(positionData);
	return (result == 0);
}

bool motionControlSDK::MC_sendData(int cmdType, const QByteArray& data)
{
	SDKManager::instance()->sendCommand(cmdType, data);
	return true;
}

// ======================= 打印控制 ====================

bool motionControlSDK::MC_loadPrintData(const QString& filePath)
{
	if (!isConnected()) 
	{
		emit errorOccurred(-1, tr(u8"dev_unconnect"));
		return false;
	}

	if (filePath.isEmpty()) 
	{
		emit errorOccurred(-1, tr(u8"文件路径为空"));
		return false;
	}

	int ret = SDKManager::instance()->loadImageData(filePath.toUtf8().constData());
	if (ret != 0) 
	{
		emit errorOccurred(ret, tr(u8"加载打印数据失败"));
		return false;
	}

	emit infoMessage(tr(u8"图像数据已加载：%1").arg(filePath));
	return true;
}

bool motionControlSDK::startPrint()
{
	if (!isConnected()) 
	{
		emit errorOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	int ret = SDKManager::instance()->startPrint();
	if (ret != 0) 
	{
		emit errorOccurred(ret, tr("start_print_cmd_failed"));
		return false;
	}

	emit printStatusChanged(tr("start_print"));
	return true;
}

bool motionControlSDK::pausePrint()
{
	if (!isConnected()) 
	{
		emit errorOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	int ret = SDKManager::instance()->pausePrint();
	if (ret != 0) 
	{
		emit errorOccurred(ret, tr(u8"暂停打印命令失败"));
		return false;
	}

	emit printStatusChanged(tr(u8"打印暂停"));
	return true;
}

bool motionControlSDK::resumePrint()
{
	if (!isConnected()) 
	{
		emit errorOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	int ret = SDKManager::instance()->resumePrint();
	if (ret != 0) 
	{
		emit errorOccurred(ret, tr(u8"恢复打印命令失败"));
		return false;
	}

	emit printStatusChanged(tr(u8"打印恢复"));
	return true;
}

bool motionControlSDK::stopPrint()
{
	if (!isConnected()) 
	{
		emit errorOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	int ret = SDKManager::instance()->stopPrint();
	if (ret != 0) 
	{
		emit errorOccurred(ret, tr(u8"停止打印命令失败"));
		return false;
	}

	emit printStatusChanged(tr(u8"打印停止"));
	return true;
}

// ==================== 回调函数（桥接C回调到Qt信号）====================

void motionControlSDK::Private::sdkEventCallback(const SdkEvent* event)
{
	if (!event) 
	{
		return;
	}

	QMutexLocker locker(&s_mutex);

	if (!s_instance) 
	{
		return;
	}

	// 将C回调转换为Qt信号
	// 使用QMetaObject::invokeMethod确保信号在正确的线程中发射（线程安全）
	QString message = QString::fromUtf8(event->message);
	SdkEventType type = event->type;
	int code = event->code;
	double v1 = event->value1;
	double v2 = event->value2;
	double v3 = event->value3;

	// 使用Qt::QueuedConnection确保在主线程中执行
	QMetaObject::invokeMethod(s_instance, [=]() 
	{
		switch (type) 
		{
		case EVENT_TYPE_GENERAL: 
		{
			// 检测连接状态变化
			if (message.contains("Connected", Qt::CaseInsensitive) &&
				message.contains("device", Qt::CaseInsensitive)) 
			{
				s_instance->d->connectedState = true;
				emit s_instance->connected();
				emit s_instance->connectedChanged(true);
				qDebug() << "Device connected";
			}
			else if (message.contains("Disconnected", Qt::CaseInsensitive)) 
			{
				s_instance->d->connectedState = false;
				emit s_instance->disconnected();
				emit s_instance->connectedChanged(false);
				qDebug() << "Device disconnected";
			}

			emit s_instance->infoMessage(message);
			break;
		}

		case EVENT_TYPE_ERROR: 
		{
			emit s_instance->errorOccurred(code, message);
			qWarning() << "SDK Error:" << code << message;
			break;
		}

		case EVENT_TYPE_PRINT_STATUS: 
		{
			int progress = static_cast<int>(v1);
			int currentLayer = static_cast<int>(v2);
			int totalLayers = static_cast<int>(v3);

			emit s_instance->printProgressUpdated(progress, currentLayer, totalLayers);

			QString statusMsg = QString(u8"打印进度: %1% (%2/%3层)").arg(progress).arg(currentLayer).arg(totalLayers);
			emit s_instance->printStatusChanged(statusMsg);

			qDebug() << "Print progress:" << progress << "%"
				<< currentLayer << "/" << totalLayers;
			break;
		}

		case EVENT_TYPE_MOVE_STATUS: 
		{
			emit s_instance->moveStatusChanged(message);

			// 如果有坐标信息，发送位置更新
			if (v1 != 0 || v2 != 0 || v3 != 0) {
				emit s_instance->positionUpdated(v1, v2, v3);
				qDebug() << "Position:" << v1 << v2 << v3;
			}
			break;
		}

		case EVENT_TYPE_LOG: 
		{
			emit s_instance->logMessage(message);
			// qDebug() << "SDK Log:" << message;  // 可选：打印到调试输出
			break;
		}

		default: 
		{
			qWarning() << "Unknown event type:" << type;
			break;
		}
		}
	}, Qt::QueuedConnection);
}
