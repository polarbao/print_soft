# PrintDeviceSDK 构建指南

## 前置要求

1. **Qt 5.x** (推荐 5.12 或更高版本)
   - Qt5Core
   - Qt5Network

2. **编译器**
   - Windows: Visual Studio 2015/2017/2019 (MSVC)
   - Linux: GCC 5.0+
   - macOS: Clang

3. **构建工具** (二选一)
   - qmake (Qt自带)
   - CMake 3.10+

## Windows下使用Visual Studio构建

### 方法1: 使用qmake

1. 打开Qt命令提示符 (Qt 5.x for Desktop (MSVC))

2. 进入SDK目录：
```cmd
cd D:\work_namespace\Code\print_moudle\manu_print_soft\PrintDeviceSDK
```

3. 生成项目文件：
```cmd
qmake PrintDeviceSDK.pro
```

4. 编译（Release版本）：
```cmd
nmake release
```

或编译Debug版本：
```cmd
nmake debug
```

5. 生成的文件位于：
   - `bin/release/PrintDeviceSDK.dll`
   - `bin/release/PrintDeviceSDK.lib`

### 方法2: 使用CMake

1. 打开命令提示符并进入SDK目录

2. 创建构建目录：
```cmd
mkdir build
cd build
```

3. 生成VS项目文件：
```cmd
cmake .. -G "Visual Studio 15 2017"
```
或使用VS 2019：
```cmd
cmake .. -G "Visual Studio 16 2019"
```

4. 编译：
```cmd
cmake --build . --config Release
```

5. 生成的文件位于 `build/Release/`

## 构建示例程序

进入example目录并编译：

```cmd
cd PrintDeviceSDK\example
qmake example.pro
nmake release
```

运行示例：
```cmd
.\bin\release\example_usage.exe
```

**注意**: 运行前需要确保：
1. PrintDeviceSDK.dll在系统PATH中或与exe在同一目录
2. 所有Qt运行时DLL可访问

## 在Visual Studio中打开项目

1. 使用qmake生成.sln文件：
```cmd
qmake -tp vc PrintDeviceSDK.pro
```

2. 在Visual Studio中打开生成的.sln文件

3. 选择Release配置并构建

## 常见问题

### 1. 找不到Qt

设置Qt路径环境变量：
```cmd
set QTDIR=C:\Qt\5.12.0\msvc2017_64
set PATH=%QTDIR%\bin;%PATH%
```

### 2. 编译错误：找不到头文件

确保原项目 `print_soft_0_0_1` 文件夹与 `PrintDeviceSDK` 在同一目录下。

目录结构应该是：
```
manu_print_soft/
├── print_soft_0_0_1/     # 原Qt项目
│   └── src/
├── PrintDeviceSDK/        # SDK项目
└── ...
```

### 3. 链接错误

确保Qt库版本与编译器匹配：
- MSVC 2017 -> 使用 Qt MSVC 2017
- MSVC 2019 -> 使用 Qt MSVC 2019

### 4. 运行时找不到DLL

将以下DLL复制到exe同目录：
- PrintDeviceSDK.dll
- Qt5Core.dll
- Qt5Network.dll

或使用Qt自带的windeployqt工具：
```cmd
windeployqt example_usage.exe
```

## 部署

### 创建发布包

1. 编译Release版本

2. 收集必要文件：
```
YourApp/
├── your_app.exe
├── PrintDeviceSDK.dll
├── PrintDeviceSDK_API.h
├── Qt5Core.dll
├── Qt5Network.dll
└── platforms/
    └── qwindows.dll
```

3. 使用windeployqt自动收集Qt依赖：
```cmd
windeployqt --release --no-translations your_app.exe
```

## 集成到现有项目

### 使用qmake

在你的.pro文件中添加：
```qmake
INCLUDEPATH += path/to/PrintDeviceSDK
LIBS += -Lpath/to/PrintDeviceSDK/lib -lPrintDeviceSDK
```

### 使用CMake

```cmake
include_directories(path/to/PrintDeviceSDK)
link_directories(path/to/PrintDeviceSDK/lib)
target_link_libraries(your_target PrintDeviceSDK)
```

### 使用Visual Studio

1. 项目属性 -> C/C++ -> 常规 -> 附加包含目录
   - 添加 `PrintDeviceSDK_API.h` 所在目录

2. 项目属性 -> 链接器 -> 常规 -> 附加库目录
   - 添加 `PrintDeviceSDK.lib` 所在目录

3. 项目属性 -> 链接器 -> 输入 -> 附加依赖项
   - 添加 `PrintDeviceSDK.lib`

4. 确保运行时能找到 `PrintDeviceSDK.dll`

## 验证构建

构建成功后，检查以下文件是否存在：

- SDK库文件：
  - `PrintDeviceSDK/bin/release/PrintDeviceSDK.dll`
  - `PrintDeviceSDK/bin/release/PrintDeviceSDK.lib`
  
- 头文件：
  - `PrintDeviceSDK/PrintDeviceSDK_API.h`

- 示例程序：
  - `PrintDeviceSDK/example/bin/release/example_usage.exe`

运行示例程序测试SDK功能。

