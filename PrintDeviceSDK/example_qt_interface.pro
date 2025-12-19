#-------------------------------------------------
# PrintDeviceSDK Qt接口使用示例
# 展示如何使用Qt接口控制打印设备
#-------------------------------------------------

QT += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PrintDeviceExample_Qt
TEMPLATE = app

CONFIG += c++11

# ==================== 源文件 ====================

SOURCES += example_qt_interface.cpp

# ==================== SDK链接配置 ====================

# 包含SDK头文件路径
INCLUDEPATH += $$PWD

# 链接SDK库
win32 {
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/bin/release -lPrintDeviceSDK
        # 运行时需要将PrintDeviceSDK.dll复制到输出目录
        QMAKE_POST_LINK += $$quote(copy /Y \"$$PWD\\bin\\release\\PrintDeviceSDK.dll\" \"$(DESTDIR)\\PrintDeviceSDK.dll\"$$escape_expand(\n\t))
    }
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/bin/debug -lPrintDeviceSDK
        # 运行时需要将PrintDeviceSDK.dll复制到输出目录
        QMAKE_POST_LINK += $$quote(copy /Y \"$$PWD\\bin\\debug\\PrintDeviceSDK.dll\" \"$(DESTDIR)\\PrintDeviceSDK.dll\"$$escape_expand(\n\t))
    }
}

unix {
    LIBS += -L$$PWD/lib -lPrintDeviceSDK
    # Linux下设置rpath，方便运行时找到.so
    QMAKE_LFLAGS += -Wl,-rpath,$$PWD/lib
}

# ==================== 构建信息 ====================

message("Building PrintDeviceSDK Qt Interface Example")
message("SDK Path: $$PWD")
message("Qt Version: $$QT_VERSION")

# ==================== 使用说明 ====================
#
# 构建步骤:
#
# 1. 首先构建PrintDeviceSDK库:
#    cd PrintDeviceSDK
#    qmake PrintDeviceSDK.pro
#    make (或nmake/mingw32-make)
#
# 2. 然后构建示例程序:
#    qmake example_qt_interface.pro
#    make (或nmake/mingw32-make)
#
# 3. 运行示例:
#    ./PrintDeviceExample_Qt (Linux)
#    PrintDeviceExample_Qt.exe (Windows)
#
# 注意事项:
# - 确保PrintDeviceSDK.dll/.so在可执行文件目录或系统路径中
# - Windows下会自动复制DLL到输出目录
# - Linux下已设置rpath，无需手动复制.so
#

