/**
 * @file PrintDeviceController.h
 * @brief Qt风格的SDK接口
 * @details 提供完整的Qt信号槽支持，适合Qt项目使用
 * 
 * 这是Qt风格的动态库接口，使用Qt信号槽机制代替C回调函数。
 * 与C接口（PrintDeviceSDK_API.h）共存，用户可根据需求选择：
 * - Qt项目推荐使用本接口（更符合Qt编程习惯）
 * - 跨语言项目使用C接口（兼容C#/Python等）
 */

#ifndef PRINTDEVICECONTROLLER_H
#define PRINTDEVICECONTROLLER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QtGlobal>

// Qt导出宏
#if defined(PRINTDEVICESDK_LIBRARY)
#  define PRINTDEVICESDK_EXPORT Q_DECL_EXPORT
#else
#  define PRINTDEVICESDK_EXPORT Q_DECL_IMPORT
#endif

/**
 * @class PrintDeviceController
 * @brief 打印设备控制器（Qt版本）
 * 
 * 这是Qt风格的SDK接口，具有以下特点：
 * 1. 使用Qt信号槽机制，自动线程安全
 * 2. 使用Qt标准类型（QString, QByteArray等）
 * 3. 支持Qt属性系统
 * 4. 完美集成Qt应用程序
 * 
 * 使用示例：
 * @code
 * PrintDeviceController controller;
 * controller.initialize("./logs");
 * 
 * connect(&controller, &PrintDeviceController::connected, []() {
 *     qDebug() << "设备已连接";
 * });
 * 
 * controller.connectToDevice("192.168.100.57", 5555);
 * @endcode
 */
class PRINTDEVICESDK_EXPORT PrintDeviceController : public QObject
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
    explicit PrintDeviceController(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     * @note 自动释放SDK资源
     */
    ~PrintDeviceController();
    
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
    bool moveTo(double x, double y, double z, double speed = 100.0);
    
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
    bool goHome();
    
    /**
     * @brief X轴移动
     * @param distance 移动距离（mm，正数向前，负数向后）
     * @param speed 速度（mm/s，默认100）
     * @return true=命令发送成功, false=失败
     */
    bool moveXAxis(double distance, double speed = 100.0);
    
    /**
     * @brief Y轴移动
     * @param distance 移动距离（mm）
     * @param speed 速度（mm/s，默认100）
     * @return true=命令发送成功, false=失败
     */
    bool moveYAxis(double distance, double speed = 100.0);
    
    /**
     * @brief Z轴移动
     * @param distance 移动距离（mm，正数向上，负数向下）
     * @param speed 速度（mm/s，默认100）
     * @return true=命令发送成功, false=失败
     */
    bool moveZAxis(double distance, double speed = 100.0);
    
    // ==================== 打印控制 ====================
    
    /**
     * @brief 加载打印数据
     * @param filePath 图像文件路径（支持JPG/PNG/BMP）
     * @return true=成功, false=失败
     */
    bool loadPrintData(const QString& filePath);
    
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
    
    Q_DISABLE_COPY(PrintDeviceController)  // 禁止拷贝
};

#endif // PRINTDEVICECONTROLLER_H

