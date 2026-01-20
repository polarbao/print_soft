/**
 * @file QtLogger使用示例.cpp
 * @brief 展示如何使用QtLogger进行Qt类型日志输出
 * @details 包含各种Qt类型的日志记录示例
 */

#include "comm/QtLogger.h"

#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QList>
#include <QVector>
#include <QMap>
#include <QHash>
#include <QSet>
#include <QDateTime>
#include <QVariant>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QUrl>
#include <QFile>
#include <QTimer>

// ============================================================================
//                         示例1: 基础类型日志
// ============================================================================

void example1_BasicTypes()
{
    LOG_I("\n========== 示例1: 基础类型日志 ==========");
    
    // QString
    QString userName = "张三";
    QString userRole = "管理员";
    LOG_I("用户信息 - 姓名: {}, 角色: {}", userName, userRole);
    
    // 数值类型（Qt会自动转换）
    int userId = 1001;
    double balance = 1234.56;
    LOG_I("用户ID: {}, 余额: {:.2f}元", userId, balance);
    
    // 布尔值
    bool isActive = true;
    LOG_I("账户状态: {}", isActive ? "激活" : "未激活");
}

// ============================================================================
//                         示例2: 容器类型日志
// ============================================================================

void example2_Containers()
{
    LOG_I("\n========== 示例2: 容器类型日志 ==========");
    
    // QList
    QList<int> scoreList = {95, 87, 92, 88, 96};
    QT_LOG_CONTAINER(scoreList, "成绩列表");
    
    // QVector
    QVector<double> temperatureVec = {36.5, 36.8, 37.1, 36.9};
    QT_LOG_CONTAINER(temperatureVec, "体温记录");
    
    // QStringList
    QStringList fileList = {"file1.txt", "file2.doc", "file3.pdf"};
    QT_LOG_CONTAINER(fileList, "文件列表");
    
    // QMap
    QMap<QString, int> ageMap;
    ageMap["张三"] = 25;
    ageMap["李四"] = 30;
    ageMap["王五"] = 28;
    QT_LOG_CONFIG(ageMap, "年龄表");
    
    // QHash
    QHash<int, QString> deptHash;
    deptHash[101] = "研发部";
    deptHash[102] = "市场部";
    deptHash[103] = "财务部";
    QT_LOG_CONTAINER(deptHash, "部门映射");
    
    // QSet
    QSet<QString> tagSet = {"重要", "紧急", "待办"};
    QT_LOG_CONTAINER(tagSet, "标签集合");
    
    // 空容器
    QList<int> emptyList;
    QT_LOG_CONTAINER(emptyList, "空列表");
}

// ============================================================================
//                         示例3: 日期时间日志
// ============================================================================

void example3_DateTime()
{
    LOG_I("\n========== 示例3: 日期时间日志 ==========");
    
    // QDateTime
    QDateTime now = QDateTime::currentDateTime();
    QT_LOG_DATETIME_MSG(now, "当前时间");
    
    // QDate
    QDate today = QDate::currentDate();
    LOG_I("今天日期: {}", today);
    
    // QTime
    QTime currentTime = QTime::currentTime();
    LOG_I("当前时刻: {}", currentTime);
    
    // 自定义时间
    QDateTime customTime = QDateTime::fromString("2025-12-25 10:30:00", "yyyy-MM-dd HH:mm:ss");
    QT_LOG_DATETIME_MSG(customTime, "圣诞节");
    
    // 无效时间
    QDateTime invalidTime;
    QT_LOG_DATETIME_MSG(invalidTime, "无效时间测试");
}

// ============================================================================
//                         示例4: 几何类型日志
// ============================================================================

void example4_Geometry()
{
    LOG_I("\n========== 示例4: 几何类型日志 ==========");
    
    // QPoint
    QPoint mousePos(150, 280);
    QT_LOG_GEOMETRY(mousePos, "鼠标位置");
    
    // QPointF
    QPointF precisePos(150.5, 280.75);
    QT_LOG_GEOMETRY(precisePos, "精确坐标");
    
    // QSize
    QSize windowSize(1920, 1080);
    QT_LOG_GEOMETRY(windowSize, "窗口尺寸");
    
    // QSizeF
    QSizeF imageSize(1920.5, 1080.25);
    QT_LOG_GEOMETRY(imageSize, "图像尺寸");
    
    // QRect
    QRect windowRect(100, 50, 800, 600);
    QT_LOG_GEOMETRY(windowRect, "窗口矩形");
    
    // QRectF
    QRectF selectionRect(10.5, 20.5, 300.25, 200.75);
    QT_LOG_GEOMETRY(selectionRect, "选区矩形");
}

// ============================================================================
//                         示例5: QVariant日志
// ============================================================================

void example5_Variant()
{
    LOG_I("\n========== 示例5: QVariant日志 ==========");
    
    // 不同类型的QVariant
    QVariant varInt(42);
    QT_LOG_VARIANT(varInt, "整数变量");
    
    QVariant varDouble(3.14159);
    QT_LOG_VARIANT(varDouble, "浮点变量");
    
    QVariant varString("Hello Qt!");
    QT_LOG_VARIANT(varString, "字符串变量");
    
    QVariant varBool(true);
    QT_LOG_VARIANT(varBool, "布尔变量");
    
    QVariant varDateTime(QDateTime::currentDateTime());
    QT_LOG_VARIANT(varDateTime, "时间变量");
    
    // 无效QVariant
    QVariant invalidVar;
    QT_LOG_VARIANT(invalidVar, "无效变量");
    
    // 配置示例
    QMap<QString, QVariant> appConfig;
    appConfig["window_width"] = 1920;
    appConfig["window_height"] = 1080;
    appConfig["fullscreen"] = false;
    appConfig["title"] = "My Application";
    appConfig["version"] = 1.0;
    QT_LOG_CONFIG(appConfig, "应用配置");
}

// ============================================================================
//                         示例6: 二进制数据日志
// ============================================================================

void example6_BinaryData()
{
    LOG_I("\n========== 示例6: 二进制数据日志 ==========");
    
    // 文本数据
    QByteArray textData("Hello Qt Logger!");
    QT_LOG_BYTES(textData, "文本数据");
    
    // 二进制数据
    QByteArray binaryData;
    binaryData.append((char)0xAA);
    binaryData.append((char)0xBB);
    binaryData.append((char)0xCC);
    binaryData.append((char)0xDD);
    QT_LOG_BYTES(binaryData, "二进制数据");
    
    // 大数据（仅预览）
    QByteArray largeData;
    for (int i = 0; i < 100; ++i) {
        largeData.append((char)(i % 256));
    }
    QT_LOG_BYTES_PREVIEW(largeData, "大数据块", 16);
    
    // 空数据
    QByteArray emptyData;
    QT_LOG_BYTES(emptyData, "空数据");
}

// ============================================================================
//                         示例7: 网络类型日志
// ============================================================================

void example7_Network()
{
    LOG_I("\n========== 示例7: 网络类型日志 ==========");
    
    // QUrl
    QUrl httpUrl("https://www.example.com:8080/api/data?query=test#section1");
    QT_LOG_URL_MSG(httpUrl, "HTTP接口");
    
    QUrl ftpUrl("ftp://ftp.example.com/files/document.pdf");
    QT_LOG_URL(ftpUrl);
    
    // 无效URL
    QUrl invalidUrl("not a valid url");
    QT_LOG_URL_MSG(invalidUrl, "无效URL测试");
    
    // QUuid
    QUuid uuid = QUuid::createUuid();
    LOG_I("生成的UUID: {}", uuid);
}

// ============================================================================
//                         示例8: 错误和警告日志
// ============================================================================

void example8_ErrorHandling()
{
    LOG_I("\n========== 示例8: 错误和警告日志 ==========");
    
    // 模拟文件操作错误
    QFile file("/nonexistent/path/file.txt");
    if (!file.open(QIODevice::ReadOnly)) {
        QT_LOG_ERROR("文件操作", file.errorString());
    }
    
    // 模拟警告
    int userAge = -5;
    if (userAge < 0) {
        QT_LOG_WARNING("数据验证", QString("年龄值无效: %1").arg(userAge));
    }
    
    // 边界检查
    int score = 105;
    QT_LOG_BOUNDS("考试成绩", score, 0, 100);
    
    score = 85;
    QT_LOG_BOUNDS("考试成绩", score, 0, 100);
}

// ============================================================================
//                         示例9: 性能计时
// ============================================================================

void example9_Performance()
{
    LOG_I("\n========== 示例9: 性能计时 ==========");
    
    {
        QT_PERF_TIMER("快速操作");
        // 模拟快速操作
        int sum = 0;
        for (int i = 0; i < 1000; ++i) {
            sum += i;
        }
    }
    
    {
        QT_PERF_TIMER("中等耗时操作");
        // 模拟中等耗时操作
        QList<int> list;
        for (int i = 0; i < 100000; ++i) {
            list.append(i);
        }
    }
    
    {
        QT_PERF_TIMER("复杂数据处理");
        QMap<int, QString> dataMap;
        for (int i = 0; i < 10000; ++i) {
            dataMap[i] = QString("Item_%1").arg(i);
        }
    }
}

// ============================================================================
//                         示例10: 函数跟踪
// ============================================================================

void nestedFunction2()
{
    QT_FUNC_TRACE();
    LOG_I("  执行最内层逻辑");
}

void nestedFunction1()
{
    QT_FUNC_TRACE();
    LOG_I("  调用内层函数");
    nestedFunction2();
}

void example10_FunctionTrace()
{
    LOG_I("\n========== 示例10: 函数跟踪 ==========");
    QT_FUNC_TRACE();
    
    LOG_I("外层函数开始");
    nestedFunction1();
    LOG_I("外层函数结束");
}

// ============================================================================
//                         示例11: QObject日志
// ============================================================================

class TestObject : public QObject
{
    Q_OBJECT
public:
    explicit TestObject(const QString& name, QObject* parent = nullptr)
        : QObject(parent)
    {
        setObjectName(name);
    }
};

void example11_QObject()
{
    LOG_I("\n========== 示例11: QObject日志 ==========");
    
    TestObject* obj1 = new TestObject("主窗口");
    TestObject* obj2 = new TestObject("子窗口");
    TestObject* obj3 = new TestObject("");  // 无名对象
    
    QT_LOG_OBJECT_MSG(obj1, "创建完成");
    QT_LOG_OBJECT(obj2);
    QT_LOG_OBJECT(obj3);
    
    delete obj1;
    delete obj2;
    delete obj3;
}

// ============================================================================
//                         示例12: 复杂嵌套类型
// ============================================================================

void example12_ComplexTypes()
{
    LOG_I("\n========== 示例12: 复杂嵌套类型 ==========");
    
    // QList<QVector<int>>
    QList<QVector<int>> nestedList;
    nestedList.append(QVector<int>{1, 2, 3});
    nestedList.append(QVector<int>{4, 5});
    nestedList.append(QVector<int>{6, 7, 8, 9});
    LOG_I("嵌套列表: {}", nestedList);
    
    // QMap<QString, QStringList>
    QMap<QString, QStringList> groupMap;
    groupMap["开发组"] = {"张三", "李四", "王五"};
    groupMap["测试组"] = {"赵六", "孙七"};
    groupMap["运维组"] = {"周八"};
    LOG_I("分组数据: {}", groupMap);
    
    // QList<QVariant>
    QList<QVariant> mixedList;
    mixedList.append(42);
    mixedList.append("Hello");
    mixedList.append(3.14);
    mixedList.append(true);
    mixedList.append(QDateTime::currentDateTime());
    LOG_I("混合类型列表: {}", mixedList);
}

// ============================================================================
//                         主函数
// ============================================================================

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // 初始化Qt日志系统
    if (!QtLogger::Init("./logs", "qt_logger_demo")) {
        qWarning() << "日志系统初始化失败！";
        return -1;
    }
    
    LOG_I("\n╔════════════════════════════════════════════════════╗");
    LOG_I("║      Qt Logger 完整使用示例                    ║");
    LOG_I("╚════════════════════════════════════════════════════╝\n");
    
    // 运行所有示例
    example1_BasicTypes();
    example2_Containers();
    example3_DateTime();
    example4_Geometry();
    example5_Variant();
    example6_BinaryData();
    example7_Network();
    example8_ErrorHandling();
    example9_Performance();
    example10_FunctionTrace();
    example11_QObject();
    example12_ComplexTypes();
    
    LOG_I("\n╔════════════════════════════════════════════════════╗");
    LOG_I("║         所有示例执行完成！                     ║");
    LOG_I("╚════════════════════════════════════════════════════╝\n");
    
    LOG_I("日志文件保存在: ./logs/qt_logger_demo.txt");
    
    return 0;
}

#include "QtLogger使用示例.moc"


