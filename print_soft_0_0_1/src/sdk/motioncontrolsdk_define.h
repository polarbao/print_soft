#pragma once

#include <QtCore/qglobal.h>
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QtGlobal>


#ifndef BUILD_STATIC
# if defined(MOTIONCONTROLSDK_LIB)
#  define MOTIONCONTROLSDK_EXPORT Q_DECL_EXPORT
# else
#  define MOTIONCONTROLSDK_EXPORT Q_DECL_IMPORT
# endif
#else
# define MOTIONCONTROLSDK_EXPORT
#endif


#define DATA_LEN_12 12


// --- 事件回调定义 ---
/**
 * @brief SDK事件类型枚举
 */
typedef enum
{
	EVENT_TYPE_GENERAL,     // 普通信息事件 (如: "Connected", "Disconnected")
	EVENT_TYPE_ERROR,       // 错误事件
	EVENT_TYPE_PRINT_STATUS,// 打印状态更新 (如: 打印操作 开始、暂停、停止、恢复、pass打印完成)
	EVENT_TYPE_MOVE_STATUS, // 运动状态更新 (如: 回原点，手动控制移动)
	EVENT_TYPE_LOG,         // 内部日志事件

	EVENT_TYPE_SEND_MSG,
	EVENT_TYPE_RECV_MSG
	
	// 预留接口
	//EVENT_TYPE_MOVE_STATUS		// 运动状态更新
	//EVENT_TYPE_DEV_CONNECT		// 设备层数更新 
	//EVENT_TYPE_PRT_LAYOUT_STATUS	// 打印层数更新 
	//EVENT_TYPE_POS_STATUS			// 位置信息更新 

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

	constexpr MoveAxisPos() noexcept : xPos(0), yPos(0), zPos(0) {}

	constexpr MoveAxisPos(quint32 x, quint32 y, quint32 z) noexcept
		: xPos(x), yPos(y), zPos(z) {}

	bool operator==(const MoveAxisPos& other) const noexcept 
	{
		return xPos == other.xPos && yPos == other.yPos && zPos == other.zPos;
	}
	bool operator!=(const MoveAxisPos& other) const noexcept 
	{
		return !(*this == other);
	}

	void Clear()
	{
		xPos = 0;
		yPos = 0;
		zPos = 0;
	}

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

	QStringList toStrList() const
	{
		QStringList tmp;
		tmp.reserve(3);
		tmp.push_back(QString::number(xPos));
		tmp.push_back(QString::number(yPos));
		tmp.push_back(QString::number(zPos));
		return tmp;
	}


};
Q_DECLARE_METATYPE(MoveAxisPos)


struct MOTIONCONTROLSDK_EXPORT MotionConfig {

	QString		floderPath;				// 文件夹路径

	QString		ip;						// 下位机通信参数 - IP地址
	int			port;					// 下位机通信参数 - 端口

	MoveAxisPos		startPos;			// 打印坐标参数	->	起始位置
	MoveAxisPos		endPos;				// 打印坐标参数	->	结束位置
	MoveAxisPos		cleanPos;			// 打印坐标参数	->	清理位置

	MoveAxisPos offset;					// 打印原点偏移量(在y轴数据区设置数据
	MoveAxisPos layerData;				// 当前图层pass总数，Z轴移动所需图层数    Y表示PASS总数, Z表示打印几层图层后, 位移轴动一次Z
	MoveAxisPos step;					// 打印步长参数
	MoveAxisPos	limit;					// 轴最大距离限制参数
	MoveAxisPos	speed;					// 轴速度参数
	MoveAxisPos	acc;					// 轴加速度参数

};
Q_DECLARE_METATYPE(MotionConfig)


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