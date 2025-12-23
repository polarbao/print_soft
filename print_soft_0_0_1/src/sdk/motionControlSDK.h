#pragma once

#include "motioncontrolsdk_global.h"
#define DATA_LEN_12 12

// --- 事件回调定义 ---
/**
 * @brief SDK事件类型枚举
 */
typedef enum 
{
	EVENT_TYPE_GENERAL,     // 普通信息事件 (如: "Connected", "Disconnected")
	EVENT_TYPE_ERROR,       // 错误事件
	EVENT_TYPE_PRINT_STATUS,// 打印状态更新 (如: 进度, 层数)
	EVENT_TYPE_MOVE_STATUS, // 运动状态更新 (如: "Moving", "Idle")
	EVENT_TYPE_LOG          // 内部日志事件
} SdkEventType;

/**
 * @brief SDK事件结构体
 */
typedef struct 
{
	SdkEventType type;      // 事件类型
	int code;               // 状态/错误码
	const char* message;    // 事件消息
	double value1;          // 附加数据1 (例如: 打印进度, X坐标)
	double value2;          // 附加数据2 (例如: 当前层, Y坐标)
	double value3;          // 附加数据3 (例如: 总层数, Z坐标)
} SdkEvent;

/**
 * @brief 回调函数指针类型
 */
typedef void(*SdkEventCallback)(const SdkEvent* event);


struct MOTIONCONTROLSDK_EXPORT MoveAxisPos
{
	quint32 xPos;  
	quint32 yPos;  
	quint32 zPos;  

	MoveAxisPos() : xPos(0), yPos(0), zPos(0) {}

	MoveAxisPos(quint32 x, quint32 y, quint32 z) 
		: xPos(x), yPos(y), zPos(z) {}

	static MoveAxisPos fromMillimeters(double x_mm, double y_mm, double z_mm)
	{
		return MoveAxisPos(
			static_cast<quint32>(x_mm * 1000.0),
			static_cast<quint32>(y_mm * 1000.0),
			static_cast<quint32>(z_mm * 1000.0)
		);
	}

	void toMillimeters(double& x_out, double& y_out, double& z_out) const
	{
		x_out = static_cast<double>(xPos) / 1000.0;
		y_out = static_cast<double>(yPos) / 1000.0;
		z_out = static_cast<double>(zPos) / 1000.0;
	}
};
Q_DECLARE_METATYPE(MoveAxisPos)

struct MOTIONCONTROLSDK_EXPORT PackParam
{
	uint16_t head;
	uint16_t operType;
	uint16_t cmdFun;
	uint16_t dataLen;
	uint8_t data[DATA_LEN_12];
	uint16_t crc16;

	//PackParam() ;
};
Q_DECLARE_METATYPE(PackParam)


class MOTIONCONTROLSDK_EXPORT motionControlSDK : public QObject
{
	Q_OBJECT

	// Qt属性定义
	Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
	Q_PROPERTY(QString deviceIp READ deviceIp NOTIFY deviceIpChanged)
	Q_PROPERTY(quint16 devicePort READ devicePort NOTIFY devicePortChanged)

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
	bool initialize(const QString& logDir = QString());

	/**
	 * @brief 释放SDK资源
	 * @note 通常无需手动调用，析构时自动释放
	 */
	void release();

	/**
	 * @brief 检查SDK是否已初始化
	 * @return true=已初始化, false=未初始化
	 */
	bool isInitialized() const;

	// ==================== 连接管理 ====================


	/**
	 * @brief 连接设备
	 * @param ip 设备IP地址
	 * @param port 端口号（默认5555）
	 * @return true=成功发起连接, false=失败
	 * @note 连接结果通过connected()或errorOccurred()信号通知
	 */
	bool connectToDevice(const QString& ip, quint16 port = 5555);

	/**
	 * @brief 断开连接
	 * @note 断开结果通过disconnected()信号通知
	 */
	void disconnectFromDevice();

	/**
	 * @brief 查询连接状态
	 * @return true=已连接, false=未连接
	 */
	bool isConnected() const;

	/**
	 * @brief 获取设备IP
	 * @return IP地址字符串
	 */
	QString deviceIp() const;

	/**
	 * @brief 获取设备端口
	 * @return 端口号
	 */
	quint16 devicePort() const;

	// ==================== 运动控制 ====================

	/**
	 * @brief 移动到绝对坐标
	 * @param x X轴坐标（mm）
	 * @param y Y轴坐标（mm）
	 * @param z Z轴坐标（mm）
	 * @param speed 速度（mm/s，默认100）
	 * @return true=命令发送成功, false=失败
	 */
	bool moveTo(const MoveAxisPos& posData);

	/**
	 * @brief 相对移动
	 * @param dx X轴增量（mm）
	 * @param dy Y轴增量（mm）
	 * @param dz Z轴增量（mm）
	 * @param speed 速度（mm/s，默认100）
	 * @return true=命令发送成功, false=失败
	 */
	bool moveBy(double dx, double dy, double dz, double speed = 100.0);





	/**
	 * @brief 回原点（所有轴复位）
	 * @return true=命令发送成功, false=失败
	 */
	bool MC_GoHome();

	/**
	 * @brief X轴移动
	 * @param distance 移动距离（mm，正数向前，负数向后）
	 * @param speed 速度（mm/s，默认100）
	 * @return true=命令发送成功, false=失败
	 */
	bool MC_moveXAxis(double distance, double speed = 100.0);

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
	 * @brief Y轴移动
	 * @param distance 移动距离（mm）
	 * @param speed 速度（mm/s，默认100）
	 * @return true=命令发送成功, false=失败
	 */
	bool MC_moveYAxis(double distance, double speed = 100.0);

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
	 * @brief Z轴移动
	 * @param distance 移动距离（mm，正数向上，负数向下）
	 * @param speed 速度（mm/s，默认100）
	 * @return true=命令发送成功, false=失败
	 */
	bool MC_moveZAxis(double distance, double speed = 100.0);

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
	bool MC_moveToPosition(const MoveAxisPos& targetPos);

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
	bool MC_moveToPosition(const QByteArray& positionData);





	/**
	* @brief 发送数据
	* @return true=命令发送成功, false=失败
	*/
	bool MC_sendData(int cmdType,const QByteArray& data);

	// ==================== 打印控制 ====================

	/**
	 * @brief 加载打印数据
	 * @param filePath 图像文件路径（支持JPG/PNG/BMP）
	 * @return true=成功, false=失败
	 */
	bool MC_loadPrintData(const QString& filePath);

	/**
	 * @brief 开始打印
	 * @return true=命令发送成功, false=失败
	 */
	bool startPrint();

	/**
	 * @brief 暂停打印
	 * @return true=命令发送成功, false=失败
	 */
	bool pausePrint();

	/**
	 * @brief 恢复打印
	 * @return true=命令发送成功, false=失败
	 */
	bool resumePrint();

	/**
	 * @brief 停止打印
	 * @return true=命令发送成功, false=失败
	 */
	bool stopPrint();



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
	void disconnected();

	/**
	 * @brief 连接状态改变
	 * @param isConnected 是否已连接
	 */
	void connectedChanged(bool isConnected);

	/**
	 * @brief 设备IP改变
	 * @param ip IP地址
	 */
	void deviceIpChanged(const QString& ip);

	/**
	 * @brief 设备端口改变
	 * @param port 端口号
	 */
	void devicePortChanged(quint16 port);

	// ==================== 错误和状态信号 ====================

	/**
	 * @brief 发生错误
	 * @param errorCode 错误码
	 * @param errorMessage 错误信息
	 */
	void errorOccurred(int errorCode, const QString& errorMessage);

	/**
	 * @brief 一般信息
	 * @param message 消息文本
	 */
	void infoMessage(const QString& message);

	/**
	 * @brief 日志消息
	 * @param message 日志文本
	 */
	void logMessage(const QString& message);

	// ==================== 打印相关信号 ====================

	/**
	 * @brief 打印进度更新
	 * @param progress 进度百分比（0-100）
	 * @param currentLayer 当前层
	 * @param totalLayers 总层数
	 */
	void printProgressUpdated(int progress, int currentLayer, int totalLayers);

	/**
	 * @brief 打印状态改变
	 * @param status 状态描述
	 */
	void printStatusChanged(const QString& status);

	// ==================== 运动相关信号 ====================

	/**
	 * @brief 运动状态改变
	 * @param status 状态描述
	 */
	void moveStatusChanged(const QString& status);

	/**
	 * @brief 位置更新
	 * @param x X坐标（mm）
	 * @param y Y坐标（mm）
	 * @param z Z坐标（mm）
	 */
	void positionUpdated(double x, double y, double z);

private:
	// Pimpl模式：隐藏实现细节，保持ABI稳定性
	class Private;
	Private* d;

	//Q_DISABLE_COPY(PrintDeviceController)  // 禁止拷贝

};
