#ifndef MOTION_CONTROL_API_H
#define MOTION_CONTROL_API_H

// 用于C++编译器，告诉它下面的函数是C风格的
#ifdef __cplusplus
extern "C" {
#endif

// 定义宏，用于导出/导入函数
#ifdef MOTIONCONTROL_EXPORTS
#define MOTION_CONTROL_API __declspec(dllexport)
#else
#define MOTION_CONTROL_API __declspec(dllimport)
#endif

/**
 * @brief 初始化运动控制模块
 * @param config_path 配置文件路径 (如果需要)
 * @return 0 表示成功, 其他值表示失败
 */
MOTION_CONTROL_API int InitMotionControl(const char* config_path);

/**
 * @brief 销毁和释放运动控制模块资源
 */
MOTION_CONTROL_API void ReleaseMotionControl();

/**
 * @brief 连接运动控制器
 * @param ip 控制器IP地址
 * @param port 端口号
 * @return 0 表示成功, 其他值表示失败
 */
MOTION_CONTROL_API int ConnectDevice(const char* ip, unsigned short port);

/**
 * @brief 断开与运动控制器的连接
 */
MOTION_CONTROL_API void DisconnectDevice();

/**
 * @brief 移动到指定位置
 * @param x X坐标
 * @param y Y坐标
 * @param speed 移动速度
 * @return 0 表示成功, 其他值表示失败
 */
MOTION_CONTROL_API int MoveTo(double x, double y, double speed);

/**
 * @brief 相对移动
 * @param dx X方向增量
 * @param dy Y方向增量
 * @param speed 移动速度
 * @return 0 表示成功, 其他值表示失败
 */
MOTION_CONTROL_API int MoveBy(double dx, double dy, double speed);

/**
 * @brief 回到原点
 * @return 0 表示成功, 其他值表示失败
 */
MOTION_CONTROL_API int GoHome();

/**
 * @brief 注册状态回调函数
 * @param callback 函数指针，用于接收设备状态更新 (例如: "Connected", "Moving", "Idle")
 *
 *  C#/.NET delegate example:
 *  [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
 *  public delegate void StatusCallback(string status);
 */
typedef void(*StatusCallback)(const char* status);
MOTION_CONTROL_API void RegisterStatusCallback(StatusCallback callback);


#ifdef __cplusplus
}
#endif

#endif // MOTION_CONTROL_API_H
