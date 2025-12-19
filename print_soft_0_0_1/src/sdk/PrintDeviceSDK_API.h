#ifndef PRINT_DEVICE_SDK_API_H
#define PRINT_DEVICE_SDK_API_H

#ifdef __cplusplus
extern "C" {
#endif

// 用于导出/导入函数的宏
#ifdef PRINTDEVICESDK_EXPORTS
#define SDK_API __declspec(dllexport)
#else
#define SDK_API __declspec(dllimport)
#endif

// --- 事件回调定义 ---

///**
// * @brief SDK事件类型枚举
// */
//typedef enum {
//    EVENT_TYPE_GENERAL,     // 普通信息事件 (如: "Connected", "Disconnected")
//    EVENT_TYPE_ERROR,       // 错误事件
//    EVENT_TYPE_PRINT_STATUS,// 打印状态更新 (如: 进度, 层数)
//    EVENT_TYPE_MOVE_STATUS, // 运动状态更新 (如: "Moving", "Idle")
//    EVENT_TYPE_LOG          // 内部日志事件
//} SdkEventType;
//
///**
// * @brief SDK事件结构体
// */
//typedef struct {
//    SdkEventType type;      // 事件类型
//    int code;               // 状态/错误码
//    const char* message;    // 事件消息
//    double value1;          // 附加数据1 (例如: 打印进度, X坐标)
//    double value2;          // 附加数据2 (例如: 当前层, Y坐标)
//    double value3;          // 附加数据3 (例如: 总层数, Z坐标)
//} SdkEvent;

extern SdkEvent;

/**
 * @brief 回调函数指针类型
 */
typedef void(*SdkEventCallback)(const SdkEvent* event);


// --- API 函数声明 ---

/**
 * @brief 初始化SDK。必须最先调用。
 * @param log_dir 日志文件存放目录，可为NULL。
 * @return 0 成功, 其他值失败。
 */
SDK_API int InitSDK(const char* log_dir);

/**
 * @brief 释放SDK资源。程序退出时调用。
 */
SDK_API void ReleaseSDK();

/**
 * @brief 注册事件回调函数
 * @param callback 回调函数指针
 */
SDK_API void RegisterEventCallback(SdkEventCallback callback);

/**
 * @brief 通过TCP连接设备
 * @param ip 设备IP地址
 * @param port 端口
 * @return 0 表示调用成功 (连接结果将通过回调通知)
 */
SDK_API int ConnectByTCP(const char* ip, unsigned short port);

/**
 * @brief 通过串口连接设备
 * @param port_name 串口号 (如 "COM3")
 * @param baud_rate 波特率
 * @return 0 表示调用成功
 */
SDK_API int ConnectBySerial(const char* port_name, unsigned int baud_rate);

/**
 * @brief 断开设备连接
 */
SDK_API void Disconnect();

/**
 * @brief 查询当前连接状态
 * @return 1 已连接, 0 未连接
 */
SDK_API int IsConnected();

// --- 运动控制 ---

/**
 * @brief 移动到绝对坐标
 */
SDK_API int MoveTo(double x, double y, double z, double speed);

/**
 * @brief 相对移动
 */
SDK_API int MoveBy(double dx, double dy, double dz, double speed);

/**
 * @brief 执行回原点操作
 */
SDK_API int GoHome();

// --- 打印控制 ---

/**
 * @brief 加载打印数据/文件
 * @param data 打印数据或文件路径的C字符串
 * @return 0 成功
 */
SDK_API int LoadPrintData(const char* data);

/**
 * @brief 开始打印
 * @return 0 成功
 */
SDK_API int StartPrint();

/**
 * @brief 暂停打印
 * @return 0 成功
 */
SDK_API int PausePrint();

/**
 * @brief 恢复打印
 * @return 0 成功
 */
SDK_API int ResumePrint();

/**
 * @brief 停止打印
 * @return 0 成功
 */
SDK_API int StopPrint();


#ifdef __cplusplus
}
#endif

#endif // PRINT_DEVICE_SDK_API_H
