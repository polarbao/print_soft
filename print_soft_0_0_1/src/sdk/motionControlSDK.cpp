
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
	MC_Release();

	QMutexLocker locker(&Private::s_mutex);
	if (Private::s_instance == this) 
	{
		Private::s_instance = nullptr;
	}

	delete d;
}

bool motionControlSDK::MC_Init(const QString& logDir)
{
	qRegisterMetaType<MoveAxisPos>("MoveAxisPos");
	qRegisterMetaType<PackParam>("PackParam");

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
		emit MC_SigErrOccurred(-1, errMsg);
		return false;
	}
	//LOG_INFO(QString(u8"SDK初始化成功"));

	// 注册事件回调函数（桥接C回调到Qt信号）
	//RegisterEventCallback(&Private::sdkEventCallback);
	QMutexLocker lock(&g_callbackMutex);
	g_sdkCallback = &Private::sdkEventCallback;

	d->initialized = true;
	//emit MC_SigInfoMsg(tr("SDK初始化成功"));

	qDebug() << "motionControlSDK initialized successfully";
	return true;
}

void motionControlSDK::MC_Release()
{
	if (!d->initialized) 
	{
		return;
	}

	// 断开连接
	if (d->connectedState) 
	{
		MC_DisconnectDev();
	}

	// 释放底层SDK
	SDKManager::instance()->release();


	d->initialized = false;
	d->connectedState = false;
	d->ip.clear();
	d->port = 0;

	qDebug() << "motionControlSDK released";
}


bool motionControlSDK::MC_IsInit() const
{
	return d->initialized;
}

bool motionControlSDK::MC_Connect2Dev(const QString& ip, quint16 port)
{
	LOG_INFO("1111111111111connectToDevice");
	if (!d->initialized) 
	{
		//emit MC_SigErrOccurred(-1, tr("SDK未初始化，请先调用MC_Init()"));
		//LOG_INFO(u8"SDK未初始化，请先调用MC_Init()");
		return false;
	}

	if (d->connectedState) 
	{
		//emit MC_SigInfoMsg(tr("设备已连接"));
		//LOG_INFO(u8"设备已连接");
		return true;
	}

	// 调用C接口连接设备
	int ret = SDKManager::instance()->connectByTCP(ip, port);
	if (ret != 0) 
	{
		QString errMsg = tr("连接失败，错误码：%1").arg(ret);
		//LOG_INFO(QString(u8"连接失败，错误码：%1").arg(ret));
		emit MC_SigErrOccurred(ret, errMsg);
		return false;
	}

	// 保存连接信息
	d->ip = ip;
	d->port = port;
	emit MC_SigDevIpChanged(ip);
	emit MC_SigDevPortChanged(port);
	emit MC_SigInfoMsg(tr("正在连接 %1:%2...").arg(ip).arg(port));
	LOG_INFO(QString(u8"正在连接 %1:%2...").arg(ip).arg(port));

	return true;
}

void motionControlSDK::MC_DisconnectDev()
{
	if (!d->initialized) 
	{
		return;
	}

	if (!d->connectedState) 
	{
		//emit MC_SigInfoMsg(tr("设备未连接"));
		return;
	}

	// 调用C接口断开连接
	SDKManager::instance()->disconnect();

	emit MC_SigInfoMsg(tr("正在断开连接..."));
}

bool motionControlSDK::MC_IsConnected() const
{
	return d->connectedState;
}

QString motionControlSDK::MC_GetDevIp() const
{
	return d->ip;
}

quint16 motionControlSDK::MC_GetDevPort() const
{
	return d->port;
}

void motionControlSDK::refreshConnectionStatus()
{
	bool connected = SDKManager::instance()->isConnected();
	if (d->connectedState != connected) 
	{
		d->connectedState = connected;
		emit MC_SigConnectedChanged(connected);

		//if (connected) 
		//{
		//	emit connected();
		//}
		//else 
		//{
		//	emit MC_SigDisconnected();
		//}
	}
}

// ==================== 运动控制 ====================

//全轴复位
bool motionControlSDK::MC_GoHome()
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, QString(u8"dev_unconnect"));
		return false;
	}

	// 所有轴回原点
	// axisFlag = 7 表示 X(1) + Y(2) + Z(4) = 全部轴 
	int ret = SDKManager::instance()->resetAxis(7);
	if (ret != 0) 
	{
		emit MC_SigErrOccurred(-1, QString(u8"go_home_cmd_failed"));
		return false;
	}

	emit MC_SigMoveStatusChanged(tr("go_homing..."));
	return true;
}

//单轴绝对运动
bool motionControlSDK::MC_moveXAxis(const MoveAxisPos& targetPos)
{
	if (!d->initialized) 
	{
		emit MC_SigErrOccurred(-1, tr(u8"SDK未初始化"));
		return false;
	}

	// 调用SDKManager的X轴移动
	int result = SDKManager::instance()->move2AbsXAxis(targetPos);
	return (result == 0);
}

bool motionControlSDK::MC_moveYAxis(const MoveAxisPos& targetPos)
{
	if (!d->initialized) 
	{
		emit MC_SigErrOccurred(-1, tr(u8"SDK未初始化"));
		return false;
	}

	// 调用SDKManager的Y轴移动
	int result = SDKManager::instance()->move2AbsYAxis(targetPos);
	return (result == 0);
}

bool motionControlSDK::MC_moveZAxis(const MoveAxisPos& targetPos)
{
	if (!d->initialized) 
	{
		emit MC_SigErrOccurred(-1, tr(u8"SDK未初始化"));
		return false;
	}

	// 调用SDKManager的Z轴移动
	int result = SDKManager::instance()->move2AbsZAxis(targetPos);
	return (result == 0);
}

//单轴相对运动
bool motionControlSDK::MC_move2RelSingleAxisPos(double dx, double dy, double dz)
{
	if (!d->initialized)
	{
		emit MC_SigErrOccurred(-1, tr((u8"SDK未初始化")));
		return false;
	}

	int ret = 0;
	// 调用SDKManager的相对移动
	if (dx != 0)
	{
		ret |= SDKManager::instance()->move2RelXAxis(dx);
	}
	if (dy != 0)
	{
		ret |= SDKManager::instance()->move2RelXAxis(dy);
	}
	if (dz != 0)
	{
		ret |= SDKManager::instance()->move2RelXAxis(dz);
	}
	if (ret != 0)
	{
		emit MC_SigErrOccurred(ret, tr(u8"MoveBy命令失败"));
		return false;
	}

	emit MC_SigMoveStatusChanged(tr(u8"motion_moudle 相对移动单一轴 (%.2f, %.2f, %.2f)").arg(dx).arg(dy).arg(dz));
	return true;
}

bool motionControlSDK::MC_move2AbsSingleAxisPos(const MoveAxisPos& targetPos)
{
	if (!d->initialized)
	{
		emit MC_SigErrOccurred(-1, tr((u8"SDK未初始化")));
		return false;
	}

	int ret = 0;
	// 调用SDKManager的绝对移动
	if (targetPos.xPos != 0)
	{
		ret |= SDKManager::instance()->move2AbsXAxis(targetPos);
	}
	if (targetPos.yPos != 0)
	{
		ret |= SDKManager::instance()->move2AbsYAxis(targetPos);
	}
	if (targetPos.zPos != 0)
	{
		ret |= SDKManager::instance()->move2AbsZAxis(targetPos);
	}
	if (ret != 0)
	{
		emit MC_SigErrOccurred(ret, tr(u8"move2AbsAxis命令失败"));
		return false;
	}

	emit MC_SigMoveStatusChanged(tr(u8"motion_moudle 绝对移动单一轴 (%.2f, %.2f, %.2f)")
		.arg(targetPos.xPos)
		.arg(targetPos.yPos)
		.arg(targetPos.zPos));

	return false;
}

//3轴同时移动 

bool motionControlSDK::MC_move2RelAxisPos(double dx, double dy, double dz)
{
	if (!d->initialized)
	{
		emit MC_SigErrOccurred(-1, tr((u8"SDK未初始化")));
		return false;
	}

	int ret = 0;
	// 调用SDKManager的相对移动
	if (dx != 0)
	{
		ret |= SDKManager::instance()->move2RelXAxis(dx);
	}
	if (dy != 0)
	{
		ret |= SDKManager::instance()->move2RelXAxis(dy);
	}
	if (dz != 0)
	{
		ret |= SDKManager::instance()->move2RelXAxis(dz);
	}
	if (ret != 0)
	{
		emit MC_SigErrOccurred(ret, tr(u8"MoveBy命令失败"));
		return false;
	}

	emit MC_SigMoveStatusChanged(tr(u8"相对移动 (%.2f, %.2f, %.2f)").arg(dx).arg(dy).arg(dz));
	return true;
}

bool motionControlSDK::MC_move2AbsAxisPos(const MoveAxisPos& targetPos)
{
	if (!d->initialized) 
	{
		emit MC_SigErrOccurred(-1, tr(u8"SDK未初始化"));
		return false;
	}

	// 调用SDKManager的3轴同时移动
	// 将MoveAxisPos结构体转换为QbyteArray，使用字节数据的moveToPosition()进行调用
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	// 写入X/Y/Z坐标（各4字节，大端序，微米单位）
	stream << targetPos.xPos;
	stream << targetPos.yPos;
	stream << targetPos.zPos;

	int result = SDKManager::instance()->move2AbsPosition(data);
	return (result == 0);
}

bool motionControlSDK::MC_move2AbsAxisPos(const QByteArray& positionData)
{
	if (!d->initialized)
	{
		emit MC_SigErrOccurred(-1, tr(u8"SDK未初始化"));
		return false;
	}

	// 验证数据长度
	if (positionData.size() != 12)
	{
		emit MC_SigErrOccurred(-1, tr(u8"位置数据长度必须为12字节"));
		return false;
	}

	// 调用SDKManager的3轴同时移动
	int result = SDKManager::instance()->move2AbsPosition(positionData);
	return (result == 0);
}


// ======================= 打印数据整体传输 ====================

bool motionControlSDK::MC_SendData(int cmdType, const QByteArray& data)
{
	SDKManager::instance()->sendCommand(cmdType, data);
	return true;
}

// ======================= 打印控制 ====================

bool motionControlSDK::MC_loadPrintData(const QString& filePath)
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, tr(u8"dev_unconnect"));
		return false;
	}

	if (filePath.isEmpty()) 
	{
		emit MC_SigErrOccurred(-1, tr(u8"文件路径为空"));
		return false;
	}

	int ret = SDKManager::instance()->loadImageData(filePath.toUtf8().constData());
	if (ret != 0) 
	{
		emit MC_SigErrOccurred(ret, tr(u8"加载打印数据失败"));
		return false;
	}

	emit MC_SigInfoMsg(tr(u8"图像数据已加载：%1").arg(filePath));
	return true;
}

bool motionControlSDK::MC_StartPrint()
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	int ret = SDKManager::instance()->startPrint();
	if (ret != 0) 
	{
		emit MC_SigErrOccurred(ret, tr("start_print_cmd_failed"));
		return false;
	}

	emit MC_SigPrintStatusChanged(tr("start_print"));
	return true;
}

bool motionControlSDK::MC_PausePrint()
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	int ret = SDKManager::instance()->pausePrint();
	if (ret != 0) 
	{
		emit MC_SigErrOccurred(ret, tr(u8"暂停打印命令失败"));
		return false;
	}

	emit MC_SigPrintStatusChanged(tr(u8"打印暂停"));
	return true;
}

bool motionControlSDK::MC_ResumePrint()
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	int ret = SDKManager::instance()->resumePrint();
	if (ret != 0) 
	{
		emit MC_SigErrOccurred(ret, tr(u8"恢复打印命令失败"));
		return false;
	}

	emit MC_SigPrintStatusChanged(tr(u8"打印恢复"));
	return true;
}

bool motionControlSDK::MC_StopPrint()
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	int ret = SDKManager::instance()->stopPrint();
	if (ret != 0) 
	{
		emit MC_SigErrOccurred(ret, tr(u8"停止打印命令失败"));
		return false;
	}

	emit MC_SigPrintStatusChanged(tr(u8"打印停止"));
	return true;
}

bool motionControlSDK::MC_CleanPrint()
{
	return true;
}

void motionControlSDK::MC_SendCmd(int operCmd, const QByteArray& arrData)
{
	SDKManager::instance()->sendCommand(operCmd, arrData);
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
				emit s_instance->MC_SigConnectedChanged(true);
				qDebug() << "Device connected";
			}
			else if (message.contains("Disconnected", Qt::CaseInsensitive)) 
			{
				s_instance->d->connectedState = false;
				emit s_instance->MC_SigDisconnected();
				emit s_instance->MC_SigConnectedChanged(false);
				qDebug() << "Device MC_SigDisconnected";
			}

			emit s_instance->MC_SigInfoMsg(message);
			break;
		}

		case EVENT_TYPE_ERROR: 
		{
			emit s_instance->MC_SigErrOccurred(code, message);
			qWarning() << "SDK Error:" << code << message;
			break;
		}

		case EVENT_TYPE_PRINT_STATUS: 
		{
			int progress = static_cast<int>(v1);
			int currentLayer = static_cast<int>(v2);
			int totalLayers = static_cast<int>(v3);

			emit s_instance->MC_SigPrintProgUpdated(progress, currentLayer, totalLayers);

			QString statusMsg = QString(u8"打印进度: %1% (%2/%3层)").arg(progress).arg(currentLayer).arg(totalLayers);
			emit s_instance->MC_SigPrintStatusChanged(statusMsg);

			qDebug() << "Print progress:" << progress << "%"
				<< currentLayer << "/" << totalLayers;
			break;
		}

		case EVENT_TYPE_MOVE_STATUS: 
		{
			emit s_instance->MC_SigMoveStatusChanged(message);

			// 如果有坐标信息，发送位置更新
			if (v1 != 0 || v2 != 0 || v3 != 0) {
				emit s_instance->MC_SigPosChanged(v1, v2, v3);
				qDebug() << "Position:" << v1 << v2 << v3;
			}
			break;
		}

		case EVENT_TYPE_LOG: 
		{
			emit s_instance->MC_SigLogMsg(message);
			// qDebug() << "SDK Log:" << message;  // 可选：打印到调试输出
			break;
		}
		case EVENT_TYPE_SEND_MSG:
		{
			emit s_instance->MC_SigSend2DevCmdMsg(message);
			break;
		}
		case EVENT_TYPE_RECV_MSG:
		{
			emit s_instance->MC_SigRecv2DevCmdMsg(message);
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
