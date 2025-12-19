# MotionControl Library v1.0

## 概述

`MotionControl` 是一个用于控制打印机XYZ轴运动的动态链接库。它提供了标准C接口，可以方便地被 C++ (Qt, MFC)、C# 等多种语言和框架调用。

本库的内部实现基于 Qt 5，因此在部署时需要附带相关的 Qt 运行时库。

## 文件组成

- `MotionControl.dll`: 动态链接库文件，程序运行时需要。
- `MotionControl.lib`: 链接时需要的导入库文件。
- `MotionControlAPI.h`: 包含所有导出函数声明的头文件。

## API 参考

### `int InitMotionControl(const char* config_path);`
初始化库。必须在使用其他任何API之前调用。

### `void ReleaseMotionControl();`
释放库所占用的资源。程序退出时调用。

### `int ConnectDevice(const char* ip, unsigned short port);`
通过TCP/IP连接到运动控制器。

### `void DisconnectDevice();`
断开与设备的连接。

### `int MoveTo(double x, double y, double speed);`
控制设备绝对移动到指定的 (x, y) 坐标。

### `int MoveBy(double dx, double dy, double speed);`
控制设备相对当前位置移动指定的距离。

### `int GoHome();`
控制设备执行回原点操作。

### `void RegisterStatusCallback(StatusCallback callback);`
注册一个回调函数，用于接收设备状态的实时更新。
`typedef void(*StatusCallback)(const char* status);`

## 使用方法

### C++ (Qt/MFC)

1.  在项目中包含 `MotionControlAPI.h`。
2.  将 `MotionControl.lib` 添加到项目的链接器依赖中。
3.  在程序启动时调用 `InitMotionControl()`。
4.  调用其他API函数执行操作。
5.  在程序退出前调用 `ReleaseMotionControl()`。
6.  确保 `MotionControl.dll` 以及以下Qt运行时库与您的可执行文件位于同一目录：
    - `Qt5Core.dll`
    - `Qt5Network.dll`
    - `platforms/qwindows.dll` (及其他Qt插件)

### 示例代码
```cpp
#include "MotionControlAPI.h"
#include <stdio.h>

// 回调函数的实现
void MyStatusCallback(const char* status) {
    printf("Device Status: %s\n", status);
}

int main() {
    printf("Initializing...\n");
    if (InitMotionControl(nullptr) != 0) {
        printf("Failed to initialize.\n");
        return -1;
    }

    RegisterStatusCallback(MyStatusCallback);

    printf("Connecting...\n");
    if (ConnectDevice("192.168.1.100", 8080) == 0) {
        printf("Connected. Moving...\n");
        MoveTo(10.0, 20.5, 100.0);
    } else {
        printf("Failed to connect.\n");
    }
    
    // ...等待操作完成... 

    printf("Releasing...\n");
    ReleaseMotionControl();
    
    return 0;
}
```
