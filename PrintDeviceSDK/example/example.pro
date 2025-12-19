#-------------------------------------------------
# PrintDeviceSDK 使用示例
#-------------------------------------------------

QT += core
QT -= gui

TARGET = example_usage
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

# 包含路径
INCLUDEPATH += ..

# 源文件
SOURCES += example_usage.cpp

# 链接SDK库
win32 {
    CONFIG(release, debug|release) {
        LIBS += -L../bin/release -lPrintDeviceSDK
    }
    CONFIG(debug, debug|release) {
        LIBS += -L../bin/debug -lPrintDeviceSDK
    }
}

# 输出路径
CONFIG(release, debug|release) {
    DESTDIR = ./bin/release
    OBJECTS_DIR = ./build/release
    MOC_DIR = ./build/release/moc
}
CONFIG(debug, debug|release) {
    DESTDIR = ./bin/debug
    OBJECTS_DIR = ./build/debug
    MOC_DIR = ./build/debug/moc
}

