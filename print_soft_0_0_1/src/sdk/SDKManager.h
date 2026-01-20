/**
 * @file SDKManager.h
 * @brief SDK内部管理类头文件
 * @details 负责管理所有核心对象、通信和信号槽连接，采用单例模式
 */

#ifndef SDK_MANAGER_H
#define SDK_MANAGER_H


#include <QObject>
#include <QMutex>
#include <QAbstractSocket>
#include <memory>

// 前向声明
class TcpClient;
class ProtocolPrint;
class QTimer;


// 导入事件类型定义
#include "motionControlSDK.h"
#include "CNewSingleton.h"

// 全局回调相关（在SDKCallback.cpp中定义）
extern SdkEventCallback g_sdkCallback;
extern QMutex g_callbackMutex;
extern QByteArray g_messageBuffer;

/**
 * @class SDKManager
 * @brief SDK内部管理类
 * 
 * 职责：
 * - 管理TCP客户端和协议处理器
 * - 提供设备连接和控制接口
 * - 处理信号槽连接和事件分发
 * - 管理心跳机制
 */
class SDKManager : public QObject //, public CNewSingleton<SDKManager>
{
    Q_OBJECT

public:
    /**
     * @brief 获取单例实例
     */
   static SDKManager* GetInstance();
    
    // ==================== 生命周期管理 ====================
    
    /**
     * @brief 初始化SDK
     * @param log_dir 日志目录路径
     * @return true=成功, false=失败
     */
    bool Init(const QString& log_dir);
    
    /**
     * @brief 释放SDK资源
     */
    void Release();
    
    // ==================== 连接管理（实现在SDKConnection.cpp） ====================
    
    /**
     * @brief 通过TCP连接设备
     * @param ip 设备IP地址
     * @param port 端口号
     * @return 0=成功, -1=失败
     */
    int Connect2MotionDev(const QString& ip, unsigned short port);
    
    /**
     * @brief 断开设备连接
     */
    void Disconnect();
    
    /**
     * @brief 查询连接状态
     * @return true=已连接, false=未连接
     */
    bool IsConnected() const;
    
    // ==================== 运动控制（实现在SDKMotion.cpp） ====================
    
	//单轴移动
    /**
     * @brief X轴移动
     * @param distance 移动距离（正数向前，负数向后）
     * @param isAbsolute 是否为绝对坐标
     * @return 0=成功, -1=失败
     */
	int Move2AbsXAxis(const MoveAxisPos& targetPos);
	int Move2AbsYAxis(const MoveAxisPos& targetPos);
	int Move2AbsZAxis(const MoveAxisPos& targetPos);

	int Move2RelXAxis(double distance);
	int Move2RelYAxis(double distance);
	int Move2RelZAxis(double distance);



	//三轴同时移动
	int Move2RelPos(double dx, double dy, double dz);


	/**
	 * @brief 3轴同时移动（结构体版本
	 * @param targetPos 目标位置（微米单位）
	 * @return 0=成功, -1=失败
	 *
	 * 协议格式：
	 * - 命令类型: 0x0011 (控制命令)
	 * - 命令字: 0x3107 (绝对移动)
	 * - 数据区: 12字节 (X/Y/Z各4字节，大端序，微米)
	 */
	int Move2AbsPosition(const MoveAxisPos& targetPos);

	/**
	 * @brief 3轴同时移动（字节数组版本）
	 * @param positionData 位置数据（12字节：X4+Y4+Z4）
	 * @return 0=成功, -1=失败
	 */
	int Move2AbsPosition(const QByteArray& positionData);


	/**
	 * @brief 轴复位
	 * @param axisFlag 轴标志位：1=X, 2=Y, 4=Z, 可以组合（如7=全部）
	 * @return 0=成功, -1=失败
	 */
	int ResetAxis(int axisFlag);



	/**
	 * @brief 设置目标位置
	 * @param targetPos 目标位置（微米单位）
	 */
	void SetTargetPosition(const MoveAxisPos& targetPos);

	/**
	 * @brief 获取目标位置
	 * @return 目标位置（微米单位）
	 */
	MoveAxisPos GetTargetPosition() const;
    

	/**
	 * @brief 获取目标位置
	 * @return 目标位置（微米单位）
	 */
	MoveAxisPos GetCurrentPosition() const;
    // ==================== 打印控制（实现在SDKPrint.cpp） ====================
    
    /**
     * @brief 开始打印
     * @return 0=成功, -1=失败
     */
    int StartPrint();
    
    /**
     * @brief 停止打印
     * @return 0=成功, -1=失败
     */
    int StopPrint();
    
    /**
     * @brief 暂停打印
     * @return 0=成功, -1=失败
     */
    int PausePrint();
    
    /**
     * @brief 恢复打印
     * @return 0=成功, -1=失败
     */
    int ResumePrint();
    
	// ==================== 运动打印控制（实现在SDKPrint.cpp） ====================

	///**
	// * @brief 打印复位
	// * @param positionData 位置数据（12字节：X4 + Y4 + Z4）
	// * @return 0=成功, -1=失败
	// */
	//int SetPrtReset(const MoveAxisPos& startPos);

	///**
	// * @brief 打印复位
	// * @return 0=成功, -1=失败
	// */
	//int SetPrtPassEnable();

	/**
	 * @brief 设置当前图层打印信息：当前打印图层数、pass数
	 * @param prtData 打印层数信息（总层数，在Z数据区设置）
	 * @return 0=成功, -1=失败
	 */
	int SetPrtMoveLayer(const MoveAxisPos& prtData);

	/**
	 * @brief 设置图像相关信息：当前图层的pass总数、Z轴几图层移动单位距离
	 * @param passNum 位置数据（12字节：X4+Y4+Z4）
	 * @param zStep   Z轴多少层进行一次移动（12字节：X4+Y4+Z4）
	 * @return 0=成功, -1=失败
	 */
	int SetPrtLayerData(const MoveAxisPos& data);

	// ==================== 打印参数控制（实现在SDKPrintParam.cpp） ====================


	int SetPrintStartPos(const MoveAxisPos& startPos);

	int SetPrintEndPos(const MoveAxisPos& endPos);

	int SetPrintCleanPos(const MoveAxisPos& cleanPos);

	int SetAxisSpd(const MoveAxisPos& speed);

	int SetAxisUnitStep(const MoveAxisPos& step);

	/**
	 * @brief 设置各轴加速度数据
	 * @param data 偏移量数据（12字节：X4+Y4+Z4）
	 * @return 0=成功, -1=失败
	 */
	int SetAxisAccelerated(const MoveAxisPos& step);

	/**
	 * @brief 设置原点偏移量
	 * @param data 偏移量数据（12字节：X4+Y4+Z4）
	 * @return 0=成功, -1=失败
	 */
	int SetOriginOffsetData(const MoveAxisPos& data);

    // ==================== 辅助方法 ====================
    
    /**
     * @brief 发送协议命令
     * @param code 功能码
     * @param data 附加数据
     */
    void SendCommand(int code, const QByteArray& data = QByteArray());


	/**
	 * @brief 发送协议命令（重发
	 * @param code 功能码
	 * @param data 附加数据
	 */
	void SendCommand(int code, const MoveAxisPos& posData);

	/**
	 * @brief 发送协议命令（重发
	 * @param code 功能码
	 * @param data 附加数据
	 */
	void SendCommand(const QByteArray& data = QByteArray());
    
    /**
     * @brief 发送事件到回调函数
     * @param type 事件类型
     * @param code 状态码
     * @param message 消息文本
     * @param v1 附加值1
     * @param v2 附加值2
     * @param v3 附加值3
     */
    void SendEvent(SdkEventType type, 
		int code, 
		const char* message, 
		double v1 = 0.0, 
		double v2 = 0.0, 
		double v3 = 0.0);



private slots:
    // ==================== 信号处理（实现在SDKCallback.cpp） ====================
    
    /**
     * @brief 接收到数据
     */
    void OnRecvData(QByteArray data);
    
    /**
     * @brief TCP错误
     */
    void OnTcpError(QAbstractSocket::SocketError error);
    
    /**
     * @brief 连接状态改变
     */
    void OnStateChanged(QAbstractSocket::SocketState state);
    
    /**
     * @brief 收到心跳
     */
    void OnHeartbeat();
    
    /**
     * @brief 命令应答
     */
    void OnCmdReply(int cmd, uchar errCode, QByteArray arr);
    
    /**
     * @brief 发送心跳定时器
     */
    void OnSendHeartbeat();
    
    /**
     * @brief 检查心跳超时
     */
    void OnCheckHeartbeat();

	/**
	 * @brief 失败操作命令重发
	 */
	void OnFaileHandleReTransport(QByteArray& arr);

	/**
	 * @brief 处理功能操作指令
	 */
	void OnHandleRecvFunOper(const PackParam& arr);

	/**
	 * @brief 处理功能操作指令
	 */
	void OnHandleRecvDataOper(int code, const MoveAxisPos& pos);


private:
    /**
     * @brief 私有构造函数（单例模式）
     */
	//friend class CNewSingleton<SDKManager>;
	SDKManager();
    
    // ==================== PackParam处理辅助方法 ====================
    
    /**
     * @brief 处理设置参数命令的应答
     * @param packData 数据包参数
     */
    void HandleSetParamResponse(const PackParam& packData);
    
    /**
     * @brief 处理获取命令的应答
     * @param packData 数据包参数
     */
    void HandleGetCmdResponse(const PackParam& packData);
    
    /**
     * @brief 处理控制命令的应答
     * @param packData 数据包参数
     */
    void HandleCtrlCmdResponse(const PackParam& packData);
    
    /**
     * @brief 处理打印通信命令的应答
     * @param packData 数据包参数
     */
    void HandlePrintCommCmdResponse(const PackParam& packData);
    
    /**
     * @brief 析构函数
     */
    ~SDKManager();
    
    // 禁止拷贝和赋值
    SDKManager(const SDKManager&) = delete;
    SDKManager& operator=(const SDKManager&) = delete;

    // ==================== 成员变量 ====================
    
    bool m_initialized;                             ///< 初始化标志
    std::unique_ptr<TcpClient> m_tcpClient;         ///< TCP客户端
    std::unique_ptr<ProtocolPrint> m_protocol;      ///< 协议处理器
    std::unique_ptr<QTimer> m_heartbeatSendTimer;   ///< 心跳发送定时器
    std::unique_ptr<QTimer> m_heartbeatCheckTimer;  ///< 心跳检查定时器
    QMutex m_heartbeatMutex;                        ///< 心跳互斥锁
    int m_heartbeatTimeout;                         ///< 心跳超时计数

	MoveAxisPos m_curAxisData;						/// 当前三轴位置信息
	MoveAxisPos m_dstAxisData;						/// 要到达三轴位置信息

	PackParam m_curParam;							/// 


};

#endif // SDK_MANAGER_H

