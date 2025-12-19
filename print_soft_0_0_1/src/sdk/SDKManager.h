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
class SDKManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 获取单例实例
     */
    static SDKManager* instance();
    
    // ==================== 生命周期管理 ====================
    
    /**
     * @brief 初始化SDK
     * @param log_dir 日志目录路径
     * @return true=成功, false=失败
     */
    bool init(const QString& log_dir);
    
    /**
     * @brief 释放SDK资源
     */
    void release();
    
    // ==================== 连接管理（实现在SDKConnection.cpp） ====================
    
    /**
     * @brief 通过TCP连接设备
     * @param ip 设备IP地址
     * @param port 端口号
     * @return 0=成功, -1=失败
     */
    int connectByTCP(const QString& ip, unsigned short port);
    
    /**
     * @brief 断开设备连接
     */
    void disconnect();
    
    /**
     * @brief 查询连接状态
     * @return true=已连接, false=未连接
     */
    bool isConnected() const;
    
    // ==================== 运动控制（实现在SDKMotion.cpp） ====================
    
    /**
     * @brief X轴移动
     * @param distance 移动距离（正数向前，负数向后）
     * @param isAbsolute 是否为绝对坐标
     * @return 0=成功, -1=失败
     */
    int moveXAxis(double distance, bool isAbsolute = false);
    
    /**
     * @brief Y轴移动
     * @param distance 移动距离
     * @param isAbsolute 是否为绝对坐标
     * @return 0=成功, -1=失败
     */
    int moveYAxis(double distance, bool isAbsolute = false);
    
    /**
     * @brief Z轴移动
     * @param distance 移动距离（正数向上，负数向下）
     * @param isAbsolute 是否为绝对坐标
     * @return 0=成功, -1=失败
     */
    int moveZAxis(double distance, bool isAbsolute = false);
    
    /**
     * @brief 轴复位
     * @param axisFlag 轴标志位：1=X, 2=Y, 4=Z, 可以组合（如7=全部）
     * @return 0=成功, -1=失败
     */
    int resetAxis(int axisFlag);
    
    // ==================== 打印控制（实现在SDKPrint.cpp） ====================
    
    /**
     * @brief 开始打印
     * @return 0=成功, -1=失败
     */
    int startPrint();
    
    /**
     * @brief 停止打印
     * @return 0=成功, -1=失败
     */
    int stopPrint();
    
    /**
     * @brief 暂停打印
     * @return 0=成功, -1=失败
     */
    int pausePrint();
    
    /**
     * @brief 恢复打印
     * @return 0=成功, -1=失败
     */
    int resumePrint();
    
    /**
     * @brief 打印复位
     * @return 0=成功, -1=失败
     */
    int resetPrint();
    
    /**
     * @brief 加载图像数据
     * @param imagePath 图像文件路径
     * @return 0=成功, -1=失败
     */
    int loadImageData(const QString& imagePath);
    
    // ==================== 辅助方法 ====================
    
    /**
     * @brief 发送协议命令
     * @param code 功能码
     * @param data 附加数据
     */
    void sendCommand(int code, const QByteArray& data = QByteArray());
    
    /**
     * @brief 发送事件到回调函数
     * @param type 事件类型
     * @param code 状态码
     * @param message 消息文本
     * @param v1 附加值1
     * @param v2 附加值2
     * @param v3 附加值3
     */
    void sendEvent(SdkEventType type, int code, const char* message, 
                   double v1 = 0.0, double v2 = 0.0, double v3 = 0.0);

private slots:
    // ==================== 信号处理（实现在SDKCallback.cpp） ====================
    
    /**
     * @brief 接收到数据
     */
    void onRecvData(QByteArray data);
    
    /**
     * @brief TCP错误
     */
    void onTcpError(QAbstractSocket::SocketError error);
    
    /**
     * @brief 连接状态改变
     */
    void onStateChanged(QAbstractSocket::SocketState state);
    
    /**
     * @brief 收到心跳
     */
    void onHeartbeat();
    
    /**
     * @brief 命令应答
     */
    void onCmdReply(int cmd, uchar errCode, QByteArray arr);
    
    /**
     * @brief 发送心跳定时器
     */
    void onSendHeartbeat();
    
    /**
     * @brief 检查心跳超时
     */
    void onCheckHeartbeat();

private:
    /**
     * @brief 私有构造函数（单例模式）
     */
    SDKManager();
    
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
};

#endif // SDK_MANAGER_H

