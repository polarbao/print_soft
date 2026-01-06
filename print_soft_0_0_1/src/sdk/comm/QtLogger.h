#pragma once

/**
 * @file QtLogger.h
 * @brief Qt类型日志输出封装 - 提供便捷的Qt类型日志记录功能
 * @details 基于SpdlogMgr和QtTypesFormatter，提供更简洁的Qt类型日志API
 * @author SDK Team
 * @date 2025-12-23
 */

#include "SpdlogMgr.h"
#include "QtTypesFormatter.h"

#include <QString>
#include <QDebug>
#include <QMetaType>
#include <QMetaObject>

/**
 * @class QtLogger
 * @brief Qt专用日志记录器封装类
 * @details 提供静态方法，方便快速记录Qt对象和类型
 */
class QtLogger
{
public:
    /**
     * @brief 初始化Qt日志系统
     * @param logDir 日志目录
     * @param logFileName 日志文件名
     * @param maxFileSize 单个日志文件最大大小(字节)
     * @param maxFiles 保留的日志文件数量
     * @return true-成功，false-失败
     */
    static bool Init(const std::string& logDir,
                     const std::string& logFileName = "qt_app_log",
                     size_t maxFileSize = 10 * 1024 * 1024,
                     size_t maxFiles = 10)
    {
        auto wrapper = SpdlogWrapper::GetInstance();
        bool result = wrapper->Init(logDir, logFileName, maxFileSize, maxFiles, true, true);
        
        if (result) {
            // 设置日志模式，包含更多Qt相关信息
            wrapper->SetPattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
            LOG_I("Qt日志系统初始化成功");
        }
        
        return result;
    }

    /**
     * @brief 记录QObject信息
     * @param obj QObject指针
     * @param message 附加消息
     */
    static void LogQObject(const QObject* obj, const QString& message = "")
    {
        if (!obj) {
            LOG_W("QObject为空指针");
            return;
        }

        QString className = obj->metaObject()->className();
        QString objectName = obj->objectName();
        
        if (message.isEmpty()) {
            LOG_I("QObject[类型:{}, 名称:{}]", 
                  className.toStdString(), 
                  objectName.isEmpty() ? "<unnamed>" : objectName.toStdString());
        } else {
            LOG_I("QObject[类型:{}, 名称:{}] - {}", 
                  className.toStdString(),
                  objectName.isEmpty() ? "<unnamed>" : objectName.toStdString(),
                  message.toStdString());
        }
    }

    /**
     * @brief 记录容器信息（带统计）
     * @tparam Container Qt容器类型
     * @param container 容器对象
     * @param name 容器名称
     */
    template<typename Container>
    static void LogContainer(const Container& container, const QString& name)
    {
        LOG_I("容器[{}] 大小:{}, 内容:{}", 
              name.toStdString(), 
              container.size(), 
              container);
    }

    /**
     * @brief 记录几何信息
     * @param geometry 几何对象（QPoint/QSize/QRect等）
     * @param name 几何对象名称
     */
    template<typename GeometryType>
    static void LogGeometry(const GeometryType& geometry, const QString& name)
    {
        LOG_I("几何[{}]: {}", name.toStdString(), geometry);
    }

    /**
     * @brief 记录时间信息
     * @param dateTime 时间对象
     * @param name 时间名称
     */
    static void LogDateTime(const QDateTime& dateTime, const QString& name = "时间")
    {
        if (!dateTime.isValid()) {
            LOG_W("{}无效", name.toStdString());
            return;
        }
        LOG_I("{}: {}", name.toStdString(), dateTime);
    }

    /**
     * @brief 记录QVariant信息（详细模式）
     * @param variant QVariant对象
     * @param name 变量名称
     */
    static void LogVariant(const QVariant& variant, const QString& name)
    {
        if (!variant.isValid()) {
            LOG_W("QVariant[{}] 无效", name.toStdString());
            return;
        }

        QString typeName = variant.typeName();
        LOG_I("QVariant[{}] 类型:{}, 值:{}", 
              name.toStdString(),
              typeName.toStdString(),
              variant);
    }

    /**
     * @brief 记录字节数组（带大小和预览）
     * @param byteArray 字节数组
     * @param name 数组名称
     * @param previewSize 预览字节数（0=全部）
     */
    static void LogByteArray(const QByteArray& byteArray, 
                            const QString& name,
                            int previewSize = 16)
    {
        int size = byteArray.size();
        
        if (size == 0) {
            LOG_I("QByteArray[{}] 为空", name.toStdString());
            return;
        }

        if (previewSize <= 0 || previewSize >= size) {
            // 输出全部
            LOG_I("QByteArray[{}] 大小:{} bytes, 内容:{}", 
                  name.toStdString(), 
                  size, 
                  byteArray);
        } else {
            // 输出预览
            QByteArray preview = byteArray.left(previewSize);
            LOG_I("QByteArray[{}] 大小:{} bytes, 预览(前{}字节):{}", 
                  name.toStdString(), 
                  size,
                  previewSize,
                  preview);
        }
    }

    /**
     * @brief 记录网络URL信息
     * @param url URL对象
     * @param name URL名称
     */
    static void LogUrl(const QUrl& url, const QString& name = "URL")
    {
        if (!url.isValid()) {
            LOG_W("QUrl[{}] 无效", name.toStdString());
            return;
        }

        LOG_I("QUrl[{}]: {}", name.toStdString(), url);
        LOG_D("  - 协议: {}", url.scheme().toStdString());
        LOG_D("  - 主机: {}", url.host().toStdString());
        LOG_D("  - 端口: {}", url.port());
        LOG_D("  - 路径: {}", url.path().toStdString());
    }

    /**
     * @brief 记录配置映射（QMap/QHash形式）
     * @tparam MapType 映射类型
     * @param config 配置映射
     * @param name 配置名称
     */
    template<typename MapType>
    static void LogConfig(const MapType& config, const QString& name)
    {
        LOG_I("配置[{}] 项数:{}, 内容:{}", 
              name.toStdString(),
              config.size(),
              config);
    }

    /**
     * @brief 性能计时辅助类
     */
    class PerformanceTimer
    {
    public:
        explicit PerformanceTimer(const QString& operationName)
            : m_operationName(operationName)
            , m_startTime(QDateTime::currentDateTime())
        {
            LOG_D("开始计时: {}", m_operationName.toStdString());
        }

        ~PerformanceTimer()
        {
            QDateTime endTime = QDateTime::currentDateTime();
            qint64 elapsed = m_startTime.msecsTo(endTime);
            LOG_I("操作[{}] 耗时: {}ms", 
                  m_operationName.toStdString(), 
                  elapsed);
        }

    private:
        QString m_operationName;
        QDateTime m_startTime;
    };

    /**
     * @brief 错误日志辅助宏
     */
    static void LogError(const QString& context, const QString& error)
    {
        LOG_E("错误 [{}]: {}", context.toStdString(), error.toStdString());
    }

    /**
     * @brief 警告日志辅助
     */
    static void LogWarning(const QString& context, const QString& warning)
    {
        LOG_W("警告 [{}]: {}", context.toStdString(), warning.toStdString());
    }

    /**
     * @brief 调试边界检查日志
     */
    template<typename T>
    static void LogBounds(const QString& name, T value, T minVal, T maxVal)
    {
        if (value < minVal || value > maxVal) {
            LOG_W("边界检查 [{}]: 值{} 超出范围[{}, {}]", 
                  name.toStdString(), value, minVal, maxVal);
        } else {
            LOG_D("边界检查 [{}]: 值{} 在范围内[{}, {}]", 
                  name.toStdString(), value, minVal, maxVal);
        }
    }

    /**
     * @brief 函数进入/退出跟踪
     */
    class FunctionTracer
    {
    public:
        explicit FunctionTracer(const char* funcName, const char* fileName, int line)
            : m_funcName(funcName)
        {
            LOG_T(">>> 进入函数: {} [{}:{}]", m_funcName, fileName, line);
        }

        ~FunctionTracer()
        {
            LOG_T("<<< 退出函数: {}", m_funcName);
        }

    private:
        const char* m_funcName;
    };
};

// ============================================================================
//                         便捷宏定义
// ============================================================================

// Qt对象日志
#define QT_LOG_OBJECT(obj) QtLogger::LogQObject(obj)
#define QT_LOG_OBJECT_MSG(obj, msg) QtLogger::LogQObject(obj, msg)

// Qt容器日志
#define QT_LOG_CONTAINER(container, name) QtLogger::LogContainer(container, name)

// Qt几何对象日志
#define QT_LOG_GEOMETRY(geometry, name) QtLogger::LogGeometry(geometry, name)

// Qt时间日志
#define QT_LOG_DATETIME(dt) QtLogger::LogDateTime(dt)
#define QT_LOG_DATETIME_MSG(dt, name) QtLogger::LogDateTime(dt, name)

// QVariant日志
#define QT_LOG_VARIANT(var, name) QtLogger::LogVariant(var, name)

// QByteArray日志
#define QT_LOG_BYTES(ba, name) QtLogger::LogByteArray(ba, name)
#define QT_LOG_BYTES_PREVIEW(ba, name, size) QtLogger::LogByteArray(ba, name, size)

// URL日志
#define QT_LOG_URL(url) QtLogger::LogUrl(url)
#define QT_LOG_URL_MSG(url, name) QtLogger::LogUrl(url, name)

// 配置日志
#define QT_LOG_CONFIG(config, name) QtLogger::LogConfig(config, name)

// 性能计时
#define QT_PERF_TIMER(name) QtLogger::PerformanceTimer __perf_timer__(name)

// 错误/警告日志
#define QT_LOG_ERROR(context, error) QtLogger::LogError(context, error)
#define QT_LOG_WARNING(context, warning) QtLogger::LogWarning(context, warning)

// 边界检查
#define QT_LOG_BOUNDS(name, value, min, max) QtLogger::LogBounds(name, value, min, max)

// 函数跟踪
#define QT_FUNC_TRACE() QtLogger::FunctionTracer __func_tracer__(__FUNCTION__, __FILE__, __LINE__)

// ============================================================================
//                         使用示例
// ============================================================================

/*
使用示例1: 基础日志输出

void example1() {
    QString name = "张三";
    QList<int> scores = {95, 87, 92, 88};
    
    // 直接使用LOG_I等宏
    LOG_I("学生姓名: {}", name);
    LOG_I("成绩列表: {}", scores);
    
    // 使用Qt专用宏
    QT_LOG_CONTAINER(scores, "成绩");
}

使用示例2: 容器和几何对象

void example2() {
    QStringList files = {"file1.txt", "file2.doc"};
    QPoint position(100, 200);
    QSize windowSize(1920, 1080);
    
    QT_LOG_CONTAINER(files, "文件列表");
    QT_LOG_GEOMETRY(position, "窗口位置");
    QT_LOG_GEOMETRY(windowSize, "窗口尺寸");
}

使用示例3: QVariant和配置

void example3() {
    QVariant port(8080);
    QVariant host("localhost");
    
    QT_LOG_VARIANT(port, "端口");
    QT_LOG_VARIANT(host, "主机");
    
    QMap<QString, QVariant> config;
    config["port"] = 8080;
    config["debug"] = true;
    QT_LOG_CONFIG(config, "服务器配置");
}

使用示例4: 性能计时

void example4() {
    QT_PERF_TIMER("数据处理");
    
    // 执行耗时操作
    for (int i = 0; i < 1000000; ++i) {
        // ...
    }
    
    // 离开作用域时自动输出耗时
}

使用示例5: 函数跟踪

void example5() {
    QT_FUNC_TRACE();
    
    LOG_I("执行业务逻辑");
    // ...
    
    // 离开函数时自动输出退出日志
}

使用示例6: 错误处理

void example6() {
    QFile file("test.txt");
    if (!file.open(QIODevice::ReadOnly)) {
        QT_LOG_ERROR("文件操作", file.errorString());
        return;
    }
    
    QByteArray data = file.readAll();
    QT_LOG_BYTES_PREVIEW(data, "文件内容", 32);
}

使用示例7: 复杂对象

void example7(QObject* obj) {
    QT_LOG_OBJECT_MSG(obj, "开始处理");
    
    QDateTime startTime = QDateTime::currentDateTime();
    QT_LOG_DATETIME_MSG(startTime, "开始时间");
    
    // 业务逻辑
    
    QDateTime endTime = QDateTime::currentDateTime();
    QT_LOG_DATETIME_MSG(endTime, "结束时间");
}
*/
