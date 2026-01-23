
#include "motionControlSDK.h"
#include "SDKManager.h"
#include "ProtocolPrint.h"

#include <QDebug>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QMetaObject>
#include <QVector>
#include <QList>

#include "comm/CMotionConfig.h"
#include "SpdlogMgr.h"

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
		, _bcfgSent2Dev(false)
	{
	}

	~Private()
	{
	}

	// 静态回调函数（桥接C回调到Qt信号）
	static void sdkEventCallback(const SdkEvent* event);

	// 统一的前置检查
	bool CheckPreconditions(bool needConnection = true) const 
	{
		if (!initialized) 
		{
			return false;
		}

		if (needConnection && !connectedState) 
		{
			return false;
		}
		return true;
	}

	motionControlSDK* q_ptr;
	
	bool initialized;
	bool connectedState;
	
	bool _bInit;
	bool _bConnected;					// 是否连接运动dev
	bool _bPrinting;					// 是否进行打印
	MoveAxisPos _curPos;				// 当前各轴数据
	MotionConfig motionConfig;			// 运动控制配置参数
	bool _bcfgSent2Dev;					// 标记配置是否已首次下发到设备, 调试阶段使用

	static motionControlSDK* s_instance;
	static QMutex s_mutex;


	//QString ip;
	//quint16 port;
};

// 静态成员初始化
motionControlSDK* motionControlSDK::Private::s_instance = nullptr;
QMutex motionControlSDK::Private::s_mutex;

//-----------------------------------motionControlSDK----------------------------------------
//-----------------------------------motionControlSDK----------------------------------------
//-----------------------------------motionControlSDK----------------------------------------
//-----------------------------------motionControlSDK----------------------------------------

// ==================== 单例实现 ====================

motionControlSDK* motionControlSDK::GetInstance()
{
	static motionControlSDK instance;
	return &instance;
}


// ==================== 构造和析构 ====================

motionControlSDK::motionControlSDK()
	: QObject(nullptr)
	, d(new Private(this))
{
	QMutexLocker locker(&Private::s_mutex);
	Private::s_instance = this;

	// spdlog初始化
	SpdlogWrapper::GetInstance()->Init("./logs", "mc_sdk_logic_moudle", 10 * 1024 * 1024, 5, true, true);
	auto netLogger = SpdlogWrapper::GetInstance()->GetLogger("netMoudle", "./logs/modules", 5 * 1024 * 1024, 3);
	auto logicLogger = SpdlogWrapper::GetInstance()->GetLogger("logicMoudle", "./logs/modules", 5 * 1024 * 1024, 3);
	SPDLOG_INFO("motion_moudle_sdk_init");
	NAMED_LOG_D("netMoudle", "motion_moudle_sdk net_moudle log_start");
	NAMED_LOG_D("logicMoudle", "motion_moudle_sdk logic_moudle log_start");
}

motionControlSDK::~motionControlSDK()
{
	// 将配置信息重新写入到配置文件
	CMotionConfig::GetInstance()->save(d->motionConfig);

	MC_Release();

	QMutexLocker locker(&Private::s_mutex);
	if (Private::s_instance == this) 
	{
		Private::s_instance = nullptr;
	}
	NAMED_LOG_D("netMoudle", "motion_moudle_sdk  log_end");
	NAMED_LOG_D("logicMoudle", "motion_moudle_sdk log_end");
	SPDLOG_INFO("motion_moudle_sdk log_end");

	// 释放日志
	SpdlogWrapper::GetInstance()->Shutdown();
	delete d;
}

// ==================== 生命周期管理 ====================

bool motionControlSDK::MC_Init(const QString& logDir)
{
	qRegisterMetaType<MoveAxisPos>("MoveAxisPos");
	qRegisterMetaType<PackParam>("PackParam");

	if (d->initialized) 
	{
		NAMED_LOG_D("logicMoudle", "motion_moudle_sdk SDK已经初始化");
		SPDLOG_INFO("motion_moudle_sdk SDK已经初始化");
		return true;
	}


	// 初始化SDK
	bool ret = SDKManager::GetInstance()->Init("./");
	
	if (!ret) 
	{
		QString errMsg = tr("SDK初始化失败，错误码：%1").arg(ret);
		NAMED_LOG_D("logicMoudle", "motion_moudle_sdk initialized failed");
		SPDLOG_INFO("motionControlSDK initialized failed");
		emit MC_SigErrOccurred(-1, errMsg);
		return false;
	}


	// 注册事件回调函数（桥接C回调到Qt信号）
	QMutexLocker lock(&g_callbackMutex);
	g_sdkCallback = &Private::sdkEventCallback;

	// 从配置文件加载运动控制配置
	if (!MC_LoadMotionConfig(d->motionConfig))
	{
		NAMED_LOG_E("logicMoudle", "motionControlSDK MC_LoadMotionConfig failed!");
		return false;
	}

	d->initialized = true;
	//emit MC_SigInfoMsg(tr("SDK初始化成功"));
	NAMED_LOG_D("logicMoudle", "motion_moudle_sdk initialized successfully");
	SPDLOG_INFO("motionControlSDK initialized successfully");
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
	SDKManager::GetInstance()->Release();

	d->initialized = false;
	d->connectedState = false;

	//d->ip.clear();
	//d->port = 0;

	SPDLOG_INFO("motion_moudle motionControlSDK released");
}


bool motionControlSDK::MC_IsInit() const
{
	return d->initialized;
}

// ==================== 连接管理 ====================

bool motionControlSDK::MC_Connect2Dev(const QString& ip, quint16 port)
{
	SPDLOG_TRACE("motion_moudle motionControlSDK connect_dev");

	if (!d->initialized) 
	{
		//emit MC_SigErrOccurred(-1, tr("SDK未初始化，请先调用MC_Init()"));
		SPDLOG_ERROR("SDK未初始化，请先调用MC_Init()");
		return false;
	}

	if (d->connectedState) 
	{
		//emit MC_SigInfoMsg(tr("设备已连接"));
		//SPDLOG_INFO(u8"设备已连接");
		return true;
	}

	// 调用C接口连接设备
	int ret = SDKManager::GetInstance()->Connect2MotionDev(ip, port);
	if (ret != 0) 
	{
		QString errMsg = tr("连接失败，错误码：%1").arg(ret);
		SPDLOG_INFO("连接失败，错误码: {}", ret);
		emit MC_SigErrOccurred(ret, errMsg);
		return false;
	}

	// 保存连接信息
	d->motionConfig.ip = ip;
	d->motionConfig.port = port;

	//d->ip = ip;
	//d->port = port;
	//emit MC_SigDevIpChanged(ip);
	//emit MC_SigDevPortChanged(port);
	emit MC_SigInfoMsg(tr("正在连接 %1:%2...").arg(ip).arg(port));
	SPDLOG_INFO("正在连接 {} {}...", ip, port);

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
		emit MC_SigInfoMsg(tr("设备未连接"));
		return;
	}

	// 调用C接口断开连接
	SDKManager::GetInstance()->Disconnect();

	emit MC_SigInfoMsg(tr("正在断开连接..."));
}

bool motionControlSDK::MC_IsConnected() const
{
	//return true;
	return d->connectedState;
}

QString motionControlSDK::MC_GetDevIp() const
{
	return d->motionConfig.ip;
}

quint16 motionControlSDK::MC_GetDevPort() const
{
	return  d->motionConfig.port;
}

bool motionControlSDK::MC_LoadMotionConfig(MotionConfig & config, const QString & path)
{
	return CMotionConfig::GetInstance()->load(config, path);
}

bool motionControlSDK::MC_LoadCurrentMotionConfig(MotionConfig & config)
{
	if (!d->initialized)
	{
		NAMED_LOG_E("logicMoudle", "MC_LoadCurrentMotionConfig failed. d->initialized is false");
		return false;
	}
	
	config = d->motionConfig;
	return true;
}

bool motionControlSDK::MC_SetMotionConfig(const MotionConfig & config)
{
	if (!d->initialized)
	{
		NAMED_LOG_E("logicMoudle", "MC_SetMotionConfig failed. d->initialized is false");
		return false;
	}

	if (!d->connectedState)
	{
		NAMED_LOG_E("logicMoudle", "MC_SetMotionConfig failed. d->connectedState is false");
		return false;
	}

	// 结构体信息同步（先更新本地配置）
	d->motionConfig = config;

	// 仅在首次调用时将配置参数下发至下位机
	if (!d->_bcfgSent2Dev)
	{
		NAMED_LOG_D("logicMoudle", "MC_SetMotionConfig first call, sending config to device: SetPrintStartPos, SetPrintEndPos, SetPrintCleanPos, SetAxisSpd, SetAxisUnitStep, SetOriginOffsetData, SetPrtLayerData");
		
		// 将配置参数下发至下位机
		SDKManager::GetInstance()->SetPrintStartPos(d->motionConfig.startPos);
		SDKManager::GetInstance()->SetPrintEndPos(d->motionConfig.endPos);
		SDKManager::GetInstance()->SetPrintCleanPos(d->motionConfig.cleanPos);
		SDKManager::GetInstance()->SetAxisSpd(d->motionConfig.speed);
		SDKManager::GetInstance()->SetAxisUnitStep(d->motionConfig.step);
		SDKManager::GetInstance()->SetAxisAccelerated(d->motionConfig.acc);
		
		// 设置原点偏移和图层数据
		SDKManager::GetInstance()->SetOriginOffsetData(d->motionConfig.offset);
		SDKManager::GetInstance()->SetPrtLayerData(d->motionConfig.layerData);

		// 标记已完成首次下发
		d->_bcfgSent2Dev = true;
		NAMED_LOG_D("logicMoudle", "MC_SetMotionConfig config sent to device successfully");
	}
	else
	{
		// 后续调用只更新本地配置，不下发到设备
		NAMED_LOG_D("logicMoudle", "MC_SetMotionConfig subsequent call, only update local config, no sending to device");
	}

	return true;
}

bool motionControlSDK::MC_SetPrintStartPos(const MoveAxisPos& data)
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, QString(u8"dev_unconnect"));
		return false;
	}

	// 设置打印起点
	int ret = SDKManager::GetInstance()->SetPrintStartPos(data);
	if (ret != 0)
	{
		emit MC_SigErrOccurred(-1, QString("set_print_start_pos_failed"));
		return false;
	}
	d->motionConfig.startPos = data;
	emit MC_SigMoveStatusChanged(tr("set_start_pos_opering..."));
	return true;
}

bool motionControlSDK::MC_SetPrintEndPos(const MoveAxisPos& data)
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, QString(u8"dev_unconnect"));
		return false;
	}

	// 设置打印终点
	int ret = SDKManager::GetInstance()->SetPrintEndPos(data);
	if (ret != 0)
	{
		emit MC_SigErrOccurred(-1, QString("set_print_start_pos_failed"));
		return false;
	}
	d->motionConfig.endPos = data;
	emit MC_SigMoveStatusChanged(tr("set_start_pos_opering..."));
	return true;
}

bool motionControlSDK::MC_SetPrintStep(const MoveAxisPos & data)
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, QString(u8"dev_unconnect"));
		return false;
	}

	// 设置打印步长
	int ret = SDKManager::GetInstance()->SetAxisUnitStep(data);
	if (ret != 0)
	{
		emit MC_SigErrOccurred(-1, QString("set_print_step_failed"));
		return false;
	}
	d->motionConfig.step = data;
	emit MC_SigMoveStatusChanged(tr("set_print_step_opering..."));
	return true;
}

void motionControlSDK::refreshConnectionStatus()
{
	bool connected = SDKManager::GetInstance()->IsConnected();
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

// 复位
bool motionControlSDK::MC_GoHome(int x, int y, int z)
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, QString(u8"dev_unconnect"));
		return false;
	}

	// 所有轴回原点X(1) Y(2) Z(4) 7 = 全部轴
	int axisFlag = (x ? 1 : 0) | (y ? 2 : 0) | (z ? 4 : 0);
	int ret = SDKManager::GetInstance()->ResetAxis(axisFlag);
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
	int result = SDKManager::GetInstance()->Move2AbsXAxis(targetPos);
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
	int result = SDKManager::GetInstance()->Move2AbsYAxis(targetPos);
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
	int result = SDKManager::GetInstance()->Move2AbsZAxis(targetPos);
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
		ret |= SDKManager::GetInstance()->Move2RelXAxis(dx);
	}
	if (dy != 0)
	{
		ret |= SDKManager::GetInstance()->Move2RelYAxis(dy);
	}
	if (dz != 0)
	{
		ret |= SDKManager::GetInstance()->Move2RelZAxis(dz);
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
		ret |= SDKManager::GetInstance()->Move2AbsXAxis(targetPos);
	}
	if (targetPos.yPos != 0)
	{
		ret |= SDKManager::GetInstance()->Move2AbsYAxis(targetPos);
	}
	if (targetPos.zPos != 0)
	{
		ret |= SDKManager::GetInstance()->Move2AbsZAxis(targetPos);
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
		ret |= SDKManager::GetInstance()->Move2RelXAxis(dx);
	}
	if (dy != 0)
	{
		ret |= SDKManager::GetInstance()->Move2RelYAxis(dy);
	}
	if (dz != 0)
	{
		ret |= SDKManager::GetInstance()->Move2RelZAxis(dz);
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

	int result = SDKManager::GetInstance()->Move2AbsPosition(data);
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
	int result = SDKManager::GetInstance()->Move2AbsPosition(positionData);
	return (result == 0);
}


// ======================= 打印数据整体传输 ====================

bool motionControlSDK::MC_SendData(int cmdType, const QByteArray& data)
{
	SDKManager::GetInstance()->SendCommand(cmdType, data);
	return true;
}

// ======================= 打印控制 ====================

bool motionControlSDK::MC_StartPrint()
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	int ret = SDKManager::GetInstance()->StartPrint();
	if (ret != 0) 
	{
		emit MC_SigErrOccurred(ret, tr("start_print_cmd_failed"));
		return false;
	}

	//emit MC_SigPrintStatusChangedText(tr("start_print"));
	return true;
}

bool motionControlSDK::MC_PausePrint()
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	int ret = SDKManager::GetInstance()->PausePrint();
	if (ret != 0) 
	{
		emit MC_SigErrOccurred(ret, tr(u8"暂停打印命令失败"));
		return false;
	}

	//emit MC_SigPrintStatusChangedText(tr(u8"打印暂停"));
	return true;
}

bool motionControlSDK::MC_ResumePrint()
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	int ret = SDKManager::GetInstance()->ResumePrint();
	if (ret != 0) 
	{
		emit MC_SigErrOccurred(ret, tr(u8"恢复打印命令失败"));
		return false;
	}

	//emit MC_SigPrintStatusChangedText(tr(u8"打印恢复"));
	return true;
}

bool motionControlSDK::MC_StopPrint()
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	int ret = SDKManager::GetInstance()->StopPrint();
	if (ret != 0) 
	{
		emit MC_SigErrOccurred(ret, tr(u8"停止打印命令失败"));
		return false;
	}

	//emit MC_SigPrintStatusChangedText(tr(u8"打印停止"));
	return true;
}

bool motionControlSDK::MC_CleanPrint()
{
	//移动至清洗位置，开始清洗
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	//int ret = SDKManager::GetInstance()->StopPrint();
	//if (ret != 0)
	//{
	//	emit MC_SigErrOccurred(ret, tr(u8"停止打印命令失败"));
	//	return false;
	//}

	//emit MC_SigPrintStatusChangedText(tr(u8"打印停止"));
	return true;
}

bool motionControlSDK::MC_EmergencyStopPrint()
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	//int ret = SDKManager::GetInstance()->StopPrint();
	//if (ret != 0)
	//{
	//	emit MC_SigErrOccurred(ret, tr(u8"停止打印命令失败"));
	//	return false;
	//}

	return true;
}

void motionControlSDK::MC_SendCmd(int operCmd, const QByteArray& arrData)
{
	SDKManager::GetInstance()->SendCommand(operCmd, arrData);
}

//bool motionControlSDK::MC_PrtStep(const MoveAxisPos& targetPos)
//{
//	if (!MC_IsConnected())
//	{
//		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
//		return false;
//	}
//
//	d->motionConfig.step = targetPos;
//	SDKManager::GetInstance()->SetAxisUnitStep(targetPos);
//	return true;
//}
//
//bool motionControlSDK::MC_PrtReset(const MoveAxisPos& targetPos)
//{
//	if (!MC_IsConnected())
//	{
//		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
//		return false;
//	}
//
//	SDKManager::GetInstance()->SetPrtReset(targetPos);
//	return true;
//}
//
//bool motionControlSDK::MC_PrtPassEnable()
//{
//	if (!MC_IsConnected())
//	{
//		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
//		return false;
//	}
//	MoveAxisPos targetPos;
//	SDKManager::GetInstance()->SetPrtPassEnable();
//	return true;
//}


bool motionControlSDK::MC_PrtMoveLayer(quint32 layerIdx, quint32 passIdx)
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	MoveAxisPos layerData(0, passIdx, layerIdx);
	SDKManager::GetInstance()->SetPrtMoveLayer(layerData);
	return true;
}

bool motionControlSDK::MC_SetLayerNumData(quint32 passNum, quint32 zStep)
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	MoveAxisPos layerData(0, passNum, zStep);
	SDKManager::GetInstance()->SetPrtLayerData(layerData);
	d->motionConfig.layerData = layerData;
	return true;
}

bool motionControlSDK::MC_SetOffsetData(quint32 xOffset, quint32 yOffset, quint32 zOffset)
{
	if (!MC_IsConnected())
	{
		emit MC_SigErrOccurred(-1, tr("dev_unconnect"));
		return false;
	}

	MoveAxisPos offset(xOffset, yOffset, zOffset);
	SDKManager::GetInstance()->SetOriginOffsetData(offset);
	d->motionConfig.offset = offset;
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
	QString msg = QString::fromUtf8(event->message);
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
			// tcp连接信息
			case EVENT_TYPE_GENERAL: 
			{
				// 检测连接状态变化
				if (msg.contains("connected_2_dev", Qt::CaseInsensitive))
				{
					// 连接成功, 将配置参数设置到下位机
					s_instance->d->connectedState = true;
					// 连接标志位修改之后, 再调用参数下发接口
					s_instance->MC_SetMotionConfig(s_instance->d->motionConfig);

					emit s_instance->connected();
					emit s_instance->MC_SigConnectedChanged(true);
					SPDLOG_INFO("motion_moudle sdk_connected_dev");

				}
				else if (msg.contains("disconnected_from_dev", Qt::CaseInsensitive))
				{
					s_instance->d->connectedState = false;
					// 断开连接时重置配置下发标志，以便下次连接时重新下发配置
					s_instance->d->_bcfgSent2Dev = false;
					emit s_instance->MC_SigDisconnected();
					emit s_instance->MC_SigConnectedChanged(false);
					SPDLOG_INFO("motion_moudle sdk_disconnected_dev");

				}
				emit s_instance->MC_SigInfoMsg(msg);
				break;
			}

			case EVENT_TYPE_ERROR: 
			{
				emit s_instance->MC_SigErrOccurred(code, msg);
				SPDLOG_INFO("SDK Error:{} {} ", code, msg);
				break;
			}

			// 打印逻辑操作
			case EVENT_TYPE_PRINT_STATUS: 
			{
				int xData = static_cast<int>(v1);
				int passIdx = static_cast<int>(v2);
				int curLayerIdx = static_cast<int>(v3);

				// 判断是否为pass运动结束信号
				if (msg.contains("print_pass_unit_finished", Qt::CaseInsensitive))
				{
					emit s_instance->MC_SigPrintPassFinished(msg);
					emit s_instance->MC_SigPrintProgUpdated(xData, passIdx, curLayerIdx);
					SPDLOG_INFO("motion_moudle SDK_print_pass_unit_finished: {}", msg);
					break;
				}
				
				QString statusMsg = QString(u8"打印进度: %1% (%2/%3层)").arg(xData).arg(passIdx).arg(curLayerIdx);
				//emit s_instance->MC_SigPrintProgUpdated(progress, curLayer, totalLayers);
				emit s_instance->MC_SigPrintStatusChangedText(msg);
				SPDLOG_INFO("motion_moudle Print progress:{} %{} /{}", xData, passIdx, curLayerIdx);
				break;
			}
			// 运动逻辑操作
			case EVENT_TYPE_MOVE_STATUS: 
			{		
				// 判断是否为复位信号
				if (msg.contains("move_origin_finished", Qt::CaseInsensitive))
				{
					emit  s_instance->MC_SigMove2OriginFinished(msg);
					SPDLOG_INFO("motion_moudle SDK_move_2_origin_pos_finished: {}", msg);

					break;
				}
				emit s_instance->MC_SigMoveStatusChanged(msg);
				// 如果有坐标信息，发送位置更新
				if (v1 != 0 || v2 != 0 || v3 != 0)
				{
					emit s_instance->MC_SigPosChanged(v1, v2, v3);
					// 更新数据区数据信息
					SPDLOG_INFO("motion_moudle Position:数据区1:{} 数据区2:{} 数据区3:{} ", v1, v2, v3);
				}
				break;
			}
			// 本地日志信息
			case EVENT_TYPE_LOG: 
			{
				emit s_instance->MC_SigLogMsg(msg);
				SPDLOG_INFO("motion_moudle SDK_Log: {}", msg);
				break;
			}
			// 发送至下位机信息
			case EVENT_TYPE_SEND_MSG:
			{
				emit s_instance->MC_SigSend2DevCmdMsg(msg);
				SPDLOG_INFO("motion_moudle sdk_send_hex_msg: {}", msg);
				break;
			}
			// 接收下位机信息
			case EVENT_TYPE_RECV_MSG:
			{
				emit s_instance->MC_SigRecv2DevCmdMsg(msg);
				SPDLOG_INFO("motion_moudle sdk_recv_hex_msg: {}", msg);

				break;
			}
			default: 
			{
				//SPDLOG_INFO("motion_moudle Unknown event type:{}", type);
				qWarning() << "Unknown event type:" << type;
				break;
			}
		}
	}, Qt::QueuedConnection);
}
