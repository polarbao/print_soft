#include <iostream>
#include <QList>
#include <QVector>
#include <QMap>
#include <QHash>
#include <QString>
#include <QByteArray>
#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QSet>
#include <QUrl>

// 包含spdlog相关头文件
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// 包含我们的SpdlogMgr头文件
#include "comm/SpdlogMgr.h"
#include "comm/QtTypesFormatter.h"  // 包含扩展类型支持

// 测试基础容器
void testBasicContainers() {
    std::cout << "\n=== 测试基础容器类型 ===\n";

    // 测试QList<int>
    QList<int> intList = {1, 2, 3, 4, 5};
    NAMED_LOG_I("test", "QList<int>: {}", intList);
    std::cout << "✓ QList<int>测试完成\n";

    // 测试QVector<double>
    QVector<double> doubleVec = {1.1, 2.2, 3.3};
    NAMED_LOG_I("test", "QVector<double>: {}", doubleVec);
    std::cout << "✓ QVector<double>测试完成\n";

    // 测试QMap<QString, int>
    QMap<QString, int> strIntMap;
    strIntMap["key1"] = 100;
    strIntMap["key2"] = 200;
    strIntMap["key3"] = 300;
    NAMED_LOG_I("test", "QMap<QString,int>: {}", strIntMap);
    std::cout << "✓ QMap<QString,int>测试完成\n";

    // 测试QHash<int, QString>
    QHash<int, QString> intStrHash;
    intStrHash[1] = "value1";
    intStrHash[2] = "value2";
    intStrHash[3] = "value3";
    NAMED_LOG_I("test", "QHash<int,QString>: {}", intStrHash);
    std::cout << "✓ QHash<int,QString>测试完成\n";

    // 测试QByteArray
    QByteArray byteArray("Hello Qt!");
    NAMED_LOG_I("test", "QByteArray: {}", byteArray);
    std::cout << "✓ QByteArray测试完成\n";

    // 测试空容器
    QList<int> emptyList;
    QMap<QString, int> emptyMap;
    NAMED_LOG_I("test", "Empty QList: {}", emptyList);
    NAMED_LOG_I("test", "Empty QMap: {}", emptyMap);
    std::cout << "✓ 空容器测试完成\n";
}

// 测试扩展类型
void testExtendedTypes() {
    std::cout << "\n=== 测试扩展类型 ===\n";

    // 测试QStringList
    QStringList files = {"file1.txt", "file2.doc", "file3.pdf"};
    NAMED_LOG_I("test", "QStringList: {}", files);
    std::cout << "✓ QStringList测试完成\n";

    // 测试QDateTime
    QDateTime now = QDateTime::currentDateTime();
    NAMED_LOG_I("test", "QDateTime: {}", now);
    std::cout << "✓ QDateTime测试完成\n";

    // 测试QDate
    QDate date = QDate::currentDate();
    NAMED_LOG_I("test", "QDate: {}", date);
    std::cout << "✓ QDate测试完成\n";

    // 测试QTime
    QTime time = QTime::currentTime();
    NAMED_LOG_I("test", "QTime: {}", time);
    std::cout << "✓ QTime测试完成\n";

    // 测试QVariant
    QVariant varInt(42);
    QVariant varStr("Hello World");
    QVariant varBool(true);
    QVariant varDouble(3.14159);
    NAMED_LOG_I("test", "QVariant(int): {}", varInt);
    NAMED_LOG_I("test", "QVariant(string): {}", varStr);
    NAMED_LOG_I("test", "QVariant(bool): {}", varBool);
    NAMED_LOG_I("test", "QVariant(double): {}", varDouble);
    std::cout << "✓ QVariant测试完成\n";
}

// 测试几何类型
void testGeometryTypes() {
    std::cout << "\n=== 测试几何类型 ===\n";

    // 测试QPoint
    QPoint pt(100, 200);
    NAMED_LOG_I("test", "QPoint: {}", pt);
    std::cout << "✓ QPoint测试完成\n";

    // 测试QPointF
    QPointF ptf(10.5, 20.75);
    NAMED_LOG_I("test", "QPointF: {}", ptf);
    std::cout << "✓ QPointF测试完成\n";

    // 测试QSize
    QSize sz(1920, 1080);
    NAMED_LOG_I("test", "QSize: {}", sz);
    std::cout << "✓ QSize测试完成\n";

    // 测试QSizeF
    QSizeF szf(1920.5, 1080.25);
    NAMED_LOG_I("test", "QSizeF: {}", szf);
    std::cout << "✓ QSizeF测试完成\n";

    // 测试QRect
    QRect rect(10, 20, 300, 400);
    NAMED_LOG_I("test", "QRect: {}", rect);
    std::cout << "✓ QRect测试完成\n";

    // 测试QRectF
    QRectF rectf(10.5, 20.5, 300.75, 400.25);
    NAMED_LOG_I("test", "QRectF: {}", rectf);
    std::cout << "✓ QRectF测试完成\n";
}

// 测试其他类型
void testOtherTypes() {
    std::cout << "\n=== 测试其他类型 ===\n";

    // 测试QSet
    QSet<int> intSet = {1, 2, 3, 5, 8, 13};
    NAMED_LOG_I("test", "QSet<int>: {}", intSet);
    std::cout << "✓ QSet测试完成\n";

    // 测试QUrl
    QUrl url("https://www.example.com:8080/path?query=value#anchor");
    NAMED_LOG_I("test", "QUrl: {}", url);
    std::cout << "✓ QUrl测试完成\n";

    // 测试QUuid
    QUuid uuid = QUuid::createUuid();
    NAMED_LOG_I("test", "QUuid: {}", uuid);
    std::cout << "✓ QUuid测试完成\n";

    // 测试QPair
    QPair<int, QString> pair(42, "answer");
    NAMED_LOG_I("test", "QPair<int,QString>: {}", pair);
    std::cout << "✓ QPair测试完成\n";
}

// 测试嵌套和复杂类型
void testComplexTypes() {
    std::cout << "\n=== 测试复杂类型 ===\n";

    // 测试嵌套容器
    QList<QVector<int>> nestedList;
    QVector<int> vec1 = {1, 2};
    QVector<int> vec2 = {3, 4, 5};
    nestedList.append(vec1);
    nestedList.append(vec2);
    NAMED_LOG_I("test", "QList<QVector<int>>: {}", nestedList);
    std::cout << "✓ 嵌套容器测试完成\n";

    // 测试QList<QStringList>
    QList<QStringList> listOfLists;
    listOfLists.append({"a", "b"});
    listOfLists.append({"x", "y", "z"});
    NAMED_LOG_I("test", "QList<QStringList>: {}", listOfLists);
    std::cout << "✓ QList<QStringList>测试完成\n";

    // 测试QMap<QString, QVariant>
    QMap<QString, QVariant> configMap;
    configMap["port"] = 8080;
    configMap["host"] = "localhost";
    configMap["debug"] = true;
    NAMED_LOG_I("test", "QMap<QString,QVariant>: {}", configMap);
    std::cout << "✓ QMap<QString,QVariant>测试完成\n";
}

// 主测试函数
void runAllTests() {
    // 初始化日志系统
    auto logger = SpdlogWrapper::GetInstance();
    logger->Init(".", "qt_types_test", 10*1024*1024, 5, true, false);

    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║  Qt类型spdlog格式化完整测试套件   ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";

    testBasicContainers();
    testExtendedTypes();
    testGeometryTypes();
    testOtherTypes();
    testComplexTypes();

    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║         所有测试全部通过！         ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << "\n请检查控制台输出和日志文件 qt_types_test.txt\n";
    std::cout << "验证Qt类型是否正确格式化输出\n\n";
}

int main() {
    runAllTests();
    return 0;
}
