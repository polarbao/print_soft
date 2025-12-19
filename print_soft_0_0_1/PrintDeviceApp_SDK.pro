#-------------------------------------------------
# 使用PrintDeviceSDK的Qt应用程序项目
#-------------------------------------------------

QT += core gui widgets network

TARGET = PrintDeviceApp_SDK
TEMPLATE = app
CONFIG += c++11

# 定义
DEFINES += QT_DEPRECATED_WARNINGS

# 包含路径
INCLUDEPATH += src \
               src/ui \
               src/comm \
               ../PrintDeviceSDK

# 头文件
HEADERS += \
    src/ui/PrintDeviceUI_SDK.h

# 源文件
SOURCES += \
    src/main_sdk.cpp \
    src/ui/PrintDeviceUI_SDK.cpp

# 链接SDK库
win32 {
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../PrintDeviceSDK/bin/release -lPrintDeviceSDK
    }
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../PrintDeviceSDK/bin/debug -lPrintDeviceSDK
    }
}

# 输出目录
CONFIG(release, debug|release) {
    DESTDIR = $$PWD/bin/release
    OBJECTS_DIR = $$PWD/build/release/obj
    MOC_DIR = $$PWD/build/release/moc
    RCC_DIR = $$PWD/build/release/rcc
    UI_DIR = $$PWD/build/release/ui
}

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/bin/debug
    OBJECTS_DIR = $$PWD/build/debug/obj
    MOC_DIR = $$PWD/build/debug/moc
    RCC_DIR = $$PWD/build/debug/rcc
    UI_DIR = $$PWD/build/debug/ui
}

# Windows特定设置
win32 {
    QMAKE_CXXFLAGS += /utf-8
    
    # 确保运行时能找到DLL
    CONFIG(release, debug|release) {
        QMAKE_POST_LINK += $$quote(copy /Y \"$$PWD\\..\\PrintDeviceSDK\\bin\\release\\PrintDeviceSDK.dll\" \"$$DESTDIR\\PrintDeviceSDK.dll\"$$escape_expand(\\n\\t))
    }
    CONFIG(debug, debug|release) {
        QMAKE_POST_LINK += $$quote(copy /Y \"$$PWD\\..\\PrintDeviceSDK\\bin\\debug\\PrintDeviceSDK.dll\" \"$$DESTDIR\\PrintDeviceSDK.dll\"$$escape_expand(\\n\\t))
    }
}

# 其他设置
QMAKE_LFLAGS += /SUBSYSTEM:WINDOWS

# 构建信息
message("Building PrintDeviceApp with SDK")
message("Qt Version: $$QT_VERSION")
message("Target: $$TARGET")

