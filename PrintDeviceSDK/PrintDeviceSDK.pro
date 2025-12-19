#-------------------------------------------------
# PrintDeviceSDK - 打印设备控制动态库
# 模块化版本 - 代码已按功能拆分
#-------------------------------------------------

QT += core network
QT -= gui

TARGET = PrintDeviceSDK
TEMPLATE = lib
CONFIG += dll c++11

# 定义导出宏
DEFINES += PRINTDEVICESDK_EXPORTS PRINTDEVICESDK_LIBRARY

# 包含路径
INCLUDEPATH += ../print_soft_0_0_1/src \
               ../print_soft_0_0_1/src/communicate \
               ../print_soft_0_0_1/src/protocol \
               ../print_soft_0_0_1/src/comm \
               ../print_soft_0_0_1/src/service \
               src

# ==================== SDK头文件 ====================

HEADERS += \
    PrintDeviceSDK_API.h \
    PrintDeviceController.h \
    src/SDKManager.h

# ==================== SDK实现文件（已模块化） ====================

SOURCES += \
    PrintDeviceSDK_API.cpp \
    PrintDeviceController.cpp \
    src/SDKManager.cpp \
    src/SDKConnection.cpp \
    src/SDKMotion.cpp \
    src/SDKPrint.cpp \
    src/SDKCallback.cpp

# ==================== 原项目的必要源文件 ====================

HEADERS += \
    ../print_soft_0_0_1/src/communicate/TcpClient.h \
    ../print_soft_0_0_1/src/protocol/ProtocolPrint.h \
    ../print_soft_0_0_1/src/comm/CLogManager.h \
    ../print_soft_0_0_1/src/comm/CLogThread.h \
    ../print_soft_0_0_1/src/comm/CSingleton.h \
    ../print_soft_0_0_1/src/comm/global.h \
    ../print_soft_0_0_1/src/comm/utils.h

SOURCES += \
    ../print_soft_0_0_1/src/communicate/TcpClient.cpp \
    ../print_soft_0_0_1/src/protocol/ProtocolPrint.cpp \
    ../print_soft_0_0_1/src/comm/CLogManager.cpp \
    ../print_soft_0_0_1/src/comm/CLogThread.cpp \
    ../print_soft_0_0_1/src/comm/global.cpp \
    ../print_soft_0_0_1/src/comm/utils.cpp

# ==================== 编译器选项 ====================

QMAKE_CXXFLAGS += /utf-8

# ==================== 输出路径配置 ====================

win32 {
    CONFIG(release, debug|release) {
        DESTDIR = $$PWD/bin/release
        OBJECTS_DIR = $$PWD/build/release
        MOC_DIR = $$PWD/build/release/moc
        RCC_DIR = $$PWD/build/release/rcc
    }
    CONFIG(debug, debug|release) {
        DESTDIR = $$PWD/bin/debug
        OBJECTS_DIR = $$PWD/build/debug
        MOC_DIR = $$PWD/build/debug/moc
        RCC_DIR = $$PWD/build/debug/rcc
    }
}

# ==================== 安装配置 ====================

# 安装头文件
headers.files = PrintDeviceSDK_API.h PrintDeviceController.h
headers.path = $$PWD/include

# 安装库文件
target.path = $$PWD/lib

INSTALLS += target headers

# ==================== 生成导入库 ====================

win32 {
    # 编译后复制.lib文件到lib目录
    QMAKE_POST_LINK += $$quote(if not exist \"$$PWD/lib\" mkdir \"$$PWD/lib\"$$escape_expand(\n\t))
    QMAKE_POST_LINK += $$quote(copy /Y \"$(DESTDIR)\$(TARGET).lib\" \"$$PWD\lib\$(TARGET).lib\"$$escape_expand(\n\t))
}

# ==================== 构建信息 ====================

message("Building PrintDeviceSDK (Modular Version with Qt Interface)")
message("Target: $$TARGET")
message("Qt Version: $$QT_VERSION")
message("Build Type: $$CONFIG")
message("Interfaces: C API + Qt API")
