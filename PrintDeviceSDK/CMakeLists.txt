cmake_minimum_required(VERSION 3.10)
project(PrintDeviceSDK VERSION 1.0 LANGUAGES CXX)

# ==================== 基本设置 ====================

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 输出构建信息
message(STATUS "Building PrintDeviceSDK (Modular Version with Qt Interface)")
message(STATUS "CMake Version: ${CMAKE_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# ==================== 查找Qt5 ====================

find_package(Qt5 REQUIRED COMPONENTS Core Network)

message(STATUS "Qt5 Version: ${Qt5_VERSION}")
message(STATUS "Qt5 Core: ${Qt5Core_FOUND}")
message(STATUS "Qt5 Network: ${Qt5Network_FOUND}")

# 自动MOC
set(CMAKE_AUTOMOC ON)

# ==================== 包含目录 ====================

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../print_soft_0_0_1/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../print_soft_0_0_1/src/communicate
    ${CMAKE_CURRENT_SOURCE_DIR}/../print_soft_0_0_1/src/protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/../print_soft_0_0_1/src/comm
    ${CMAKE_CURRENT_SOURCE_DIR}/../print_soft_0_0_1/src/service
)

# ==================== SDK源文件（模块化） ====================

set(SDK_HEADERS
    PrintDeviceSDK_API.h
    PrintDeviceController.h
    src/SDKManager.h
)

set(SDK_SOURCES
    PrintDeviceSDK_API.cpp
    PrintDeviceController.cpp
    src/SDKManager.cpp
    src/SDKConnection.cpp
    src/SDKMotion.cpp
    src/SDKPrint.cpp
    src/SDKCallback.cpp
)

# ==================== 原项目源文件 ====================

set(ORIGINAL_HEADERS
    ../print_soft_0_0_1/src/communicate/TcpClient.h
    ../print_soft_0_0_1/src/protocol/ProtocolPrint.h
    ../print_soft_0_0_1/src/comm/CLogManager.h
    ../print_soft_0_0_1/src/comm/CLogThread.h
    ../print_soft_0_0_1/src/comm/CSingleton.h
    ../print_soft_0_0_1/src/comm/global.h
    ../print_soft_0_0_1/src/comm/utils.h
)

set(ORIGINAL_SOURCES
    ../print_soft_0_0_1/src/communicate/TcpClient.cpp
    ../print_soft_0_0_1/src/protocol/ProtocolPrint.cpp
    ../print_soft_0_0_1/src/comm/CLogManager.cpp
    ../print_soft_0_0_1/src/comm/CLogThread.cpp
    ../print_soft_0_0_1/src/comm/global.cpp
    ../print_soft_0_0_1/src/comm/utils.cpp
)

# ==================== 创建共享库 ====================

add_library(PrintDeviceSDK SHARED 
    ${SDK_SOURCES} 
    ${SDK_HEADERS}
    ${ORIGINAL_SOURCES}
    ${ORIGINAL_HEADERS}
)

# 定义导出宏
target_compile_definitions(PrintDeviceSDK PRIVATE 
    PRINTDEVICESDK_EXPORTS 
    PRINTDEVICESDK_LIBRARY
)

# 链接Qt库
target_link_libraries(PrintDeviceSDK
    Qt5::Core
    Qt5::Network
)

# Windows特定设置
if(WIN32)
    target_compile_options(PrintDeviceSDK PRIVATE /utf-8)
endif()

# 设置输出目录
set_target_properties(PrintDeviceSDK PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# ==================== 安装规则 ====================

# 安装库文件
install(TARGETS PrintDeviceSDK
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 安装头文件
install(FILES 
    PrintDeviceSDK_API.h
    PrintDeviceController.h
    DESTINATION include
)

# ==================== 构建信息摘要 ====================

message(STATUS "")
message(STATUS "==================================================")
message(STATUS "PrintDeviceSDK Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  SDK Sources: ${SDK_SOURCES}")
message(STATUS "  SDK Headers: ${SDK_HEADERS}")
message(STATUS "  Qt5 Core: ${Qt5Core_DIR}")
message(STATUS "  Qt5 Network: ${Qt5Network_DIR}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "==================================================")
message(STATUS "")
