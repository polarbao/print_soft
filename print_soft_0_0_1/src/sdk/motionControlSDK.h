#pragma once

#include "motioncontrolsdk_define.h"

class MOTIONCONTROLSDK_EXPORT motionControlSDK : public QObject
{
	Q_OBJECT

	// Qt属性定义
	Q_PROPERTY(bool connected READ MC_IsConnected NOTIFY MC_SigConnectedChanged)
	Q_PROPERTY(QString deviceIp READ MC_GetDevIp NOTIFY MC_SigDevIpChanged)
	Q_PROPERTY(quint16 devicePort READ MC_GetDevPort NOTIFY MC_SigDevPortChanged)

public:

	/**
	 * @brief 构造函数
	 * @param parent 父对象（Qt对象树管理）
	 */
	explicit motionControlSDK(QObject *parent = nullptr);

	/**
	 * @brief 析构函数
	 * @note 自动释放SDK资源
	 */
	~motionControlSDK();


	// ==================== 生命周期管理 ====================

	/**
	 * @brief 初始化SDK
	 * @param logDir 日志目录（可为空）
	 * @return true=成功, false=失败
	 */
	bool MC_Init(const QString& logDir = QString());

	/**
	 * @brief 释放SDK资源
	 * @note 通常无需手动调用，析构时自动释放
	 */
	void MC_Release();

	/**
	 * @brief 检查SDK是否已初始化
	 * @return true=已初始化, false=未初始化
	 */
	bool MC_IsInit() const;

	// ==================== 连接管理 ====================


	/**
	 * @brief 连接设备
	 * @param ip 设备IP地址
	 * @param port 端口号（默认5555）
	 * @return true=成功发起连接, false=失败
	 * @note 连接结果通过connected()或MC_SigErrOccurred()信号通知
	 */
	bool MC_Connect2Dev(const QString& ip, quint16 port = 5555);

	/**
	 * @brief 断开连接
	 * @note 断开结果通过MC_SigDisconnected()信号通知
	 */
	void MC_DisconnectDev();

	/**
	 * @brief 查询连接状态
	 * @return true=已连接, false=未连接
	 */
	bool MC_IsConnected() const;

	/**
	 * @brief 获取设备IP
	 * @return IP地址字符串
	 */
	QString MC_GetDevIp() const;

	/**
	 * @brief 获取设备端口
	 * @return 端口号
	 */
	quint16 MC_GetDevPort() const;


	// ==================== 运动控制配置信息 ====================
	/**
	 * @brief 从配置文件中加载轴运动控制信息
	 *		  此接口底层不会实际设置参数, 只是将参数配置返回, 如果需要修改参数, 需要主动调用运动控制相关配置接口
	 * @param config 配置结构对象引用, 如果加载成功, 则会被赋值
	 * @param path	配置文件路径
	 * @return true=配置加载成功, false=配置加载失败
	 */
	bool MC_LoadMotionConfig(MotionConfig& config, const QString& path="config/motionMoudleConfig.ini");


	/**
	 * @brief 加载当前的轴运动控制信息
	 *		  此接口会将SDK_Init时读到的参数返回, 一般是用作UI层获取配置, 然后同步显示用的
	 *		  需要注意, 在没有调用SDK_Init之前, 此接口无法工作
	 * @param config 配置结构体对象引用, 如果读取成功, 则会被赋值
	 * @return true=读取成功, false=读取失败
	 */
	bool MC_LoadCurrentMotionConfig(MotionConfig& config);


	/**
	 * @brief 设置轴运动控制配置
	 *		  此接口接收运动配置信息结构体, 然后将参数包全部设置到下位机设备中, 需要在设备连接成功的情况下调用
	 * @param config 要设置到下位机的配置结构对象const引用
	 * @return true=配置加载成功, false=配置加载失败
	 */
	bool MC_SetMotionConfig(const MotionConfig& config);


	// ==================== 运动控制 ====================

	/**
	 * @brief 移动到绝对坐标
	 * @param x X轴坐标（mm）
	 * @param y Y轴坐标（mm）
	 * @param z Z轴坐标（mm）
	 * @param speed 速度（mm/s，默认100）
	 * @return true=命令发送成功, false=失败
	 */
	//bool moveTo(const MoveAxisPos& posData);

	/**
	 * @brief 相对移动
	 * @param dx X轴增量（mm）
	 * @param dy Y轴增量（mm）
	 * @param dz Z轴增量（mm）
	 * @param speed 速度（mm/s，默认100）
	 * @return true=命令发送成功, false=失败
	 */
	//bool moveBy(double dx, double dy, double dz, double speed = 100.0);

	/**
	 * @brief 回原点（所有轴复位）
	 * @return true=命令发送成功, false=失败
	 */
	bool MC_GoHome();
	/**
	 * @brief X轴移动（结构体版本）
	 * @param targetPos 目标位置结构体（仅使用xPos）
	 * @return true=命令发送成功, false=失败
	 *
	 * @code
	 * MoveAxisPos pos(100000, 0, 0);  // X轴移动100mm
	 * sdk.moveXAxis(pos);
	 * @endcode
	 */
	bool MC_moveXAxis(const MoveAxisPos& targetPos);



	/**
	 * @brief Y轴移动（结构体版本）
	 * @param targetPos 目标位置结构体（仅使用yPos）
	 * @return true=命令发送成功, false=失败
	 *
	 * @code
	 * MoveAxisPos pos(0, 200000, 0);  // Y轴移动200mm
	 * sdk.moveYAxis(pos);
	 * @endcode
	 */
	bool MC_moveYAxis(const MoveAxisPos& targetPos);

	/**
	 * @brief Z轴移动（结构体版本）
	 * @param targetPos 目标位置结构体（仅使用zPos）
	 * @return true=命令发送成功, false=失败
	 *
	 * @code
	 * MoveAxisPos pos(0, 0, 50000);  // Z轴移动50mm向上
	 * sdk.moveZAxis(pos);
	 * @endcode
	 */
	bool MC_moveZAxis(const MoveAxisPos& targetPos);
	
	// 单轴移动
	bool MC_move2RelSingleAxisPos(double dx, double dy, double dz);
	bool MC_move2AbsSingleAxisPos(const MoveAxisPos& targetPos);


	// 3轴同步移动
	bool MC_move2RelAxisPos(double dx, double dy, double dz);
	bool MC_move2AbsAxisPos(const MoveAxisPos& targetPos);
	bool MC_move2AbsAxisPos(const QByteArray& targetPos);



	/**
	 * @brief 3轴同时移动（结构体版本）
	 * @param targetPos 目标位置（微米单位）
	 * @return true=命令发送成功, false=失败
	 *
	 * @code
	 * // 使用微米
	 * MoveAxisPos pos(100000, 200000, 50000);  // X=100mm, Y=200mm, Z=50mm
	 * sdk.MC_moveToPosition(pos);
	 *
	 * // 使用毫米转换
	 * sdk.MC_moveToPosition(MoveAxisPos::fromMillimeters(100, 200, 50));
	 * @endcode
	 */
	//bool MC_moveToPosition(const MoveAxisPos& targetPos);

	/**
	 * @brief 3轴同时移动（字节数组版本）
	 * @param positionData 位置数据（12字节：X4+Y4+Z4）
	 * @return true=命令发送成功, false=失败
	 *
	 * @code
	 * QByteArray data(12, 0);
	 * // 填充X坐标（100000微米 = 100mm）
	 * data[0] = 0x00; data[1] = 0x01; data[2] = 0x86; data[3] = 0xA0;
	 * sdk.MC_moveToPosition(data);
	 * @endcode
	 */
	//bool MC_moveToPosition(const QByteArray& positionData);

	/**
	* @brief 发送数据
	* @return true=命令发送成功, false=失败
	*/
	bool MC_SendData(int cmdType,const QByteArray& data);

	// ==================== 打印控制 ====================

	/**
	 * @brief 开始打印
	 * @return true=命令发送成功, false=失败
	 */
	bool MC_StartPrint();

	/**
	 * @brief 暂停打印
	 * @return true=命令发送成功, false=失败
	 */
	bool MC_PausePrint();

	/**
	 * @brief 恢复打印
	 * @return true=命令发送成功, false=失败
	 */
	bool MC_ResumePrint();

	/**
	 * @brief 停止打印
	 * @return true=命令发送成功, false=失败
	 */
	bool MC_StopPrint();

	/**
	 * @brief 清洗逻辑 add
	 * @return true=命令发送成功, false=失败
	 */
	bool MC_CleanPrint();

	/**
	 * @brief 停止打印
	 * @return true=命令发送成功, false=失败
	 */
	void MC_SendCmd(int operCmd, const QByteArray& arrData);

public slots:
	/**
	 * @brief 槽函数：刷新连接状态
	 */
	void refreshConnectionStatus();

signals:
	// ==================== 连接相关信号 ====================

	/**
	 * @brief 连接成功
	 */
	void connected();

	/**
	 * @brief 连接断开
	 */
	void MC_SigDisconnected();

	/**
	 * @brief 连接状态改变
	 * @param isConnected 是否已连接
	 */
	void MC_SigConnectedChanged(bool isConnected);

	/**
	 * @brief 设备IP改变
	 * @param ip IP地址
	 */
	void MC_SigDevIpChanged(const QString& ip);

	/**
	 * @brief 设备端口改变
	 * @param port 端口号
	 */
	void MC_SigDevPortChanged(quint16 port);

	// ==================== 错误和状态信号 ====================

	/**
	 * @brief 发生错误
	 * @param errorCode 错误码
	 * @param errorMessage 错误信息
	 */
	void MC_SigErrOccurred(int errorCode, const QString& errorMessage);

	/**
	 * @brief 一般信息
	 * @param message 消息文本
	 */
	void MC_SigInfoMsg(const QString& message);

	/**
	 * @brief 日志消息
	 * @param message 日志文本
	 */
	void MC_SigLogMsg(const QString& message);

	/**
	 * @brief 下发至设备的详细命令
	 * @param message 详细命令
	 */
	void MC_SigSend2DevCmdMsg(const QString& message);

	/**
	 * @brief 设备返回的详细命令
	 * @param message 详细命令
	 */
	void MC_SigRecv2DevCmdMsg(const QString& message);


	// ==================== 打印相关信号 ====================

	/**
	 * @brief 打印进度更新
	 * @param progress 进度百分比（0-100）
	 * @param currentLayer 当前层
	 * @param totalLayers 总层数
	 */
	void MC_SigPrintProgUpdated(int progress, int currentLayer, int totalLayers);

	/**
	 * @brief 打印状态改变
	 * @param status 状态描述
	 */
	void MC_SigPrintStatusChanged(const QString& status);

	// ==================== 运动相关信号 ====================

	/**
	 * @brief 运动状态改变
	 * @param status 状态描述
	 */
	void MC_SigMoveStatusChanged(const QString& status);

	/**
	 * @brief 位置更新
	 * @param x X坐标（mm）
	 * @param y Y坐标（mm）
	 * @param z Z坐标（mm）
	 */
	void MC_SigPosChanged(double x, double y, double z);

private:
	// Pimpl模式：隐藏实现细节，保持ABI稳定性
	class Private;
	Private* d;

	//Q_DISABLE_COPY(PrintDeviceController)  // 禁止拷贝

};
