
#ifdef SDK_TEST
#include <iostream>
#include <iomanip>
#include <cmath>
#include <QFile>
#include <QDir>
#include "comm/CMotionConfig.h"


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
#include <QUuid>
#include <QLine>
#include <QMargins>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif


#include "SpdlogMgr.h"


void printSeparator()
{
	std::cout << "========================================\n";
}
void printSection(const std::string& sectionName)
{
	std::cout << "\n[" << sectionName << "]\n";
	printSeparator();
}
void testMotionConfig()
{
	const QString configPath = "config/motionMoudleConfig.ini";
	const QString backupPath = "config/motionMoudleConfig.ini.backup";

	std::cout << "=== CMotionConfig 配置文件读取测试 ===\n\n";

	// 备份原始配置文件
	std::cout << "正在备份原始配置文件...\n";
	if (QFile::exists(configPath)) {
		if (QFile::exists(backupPath)) {
			QFile::remove(backupPath);
		}
		if (!QFile::copy(configPath, backupPath)) {
			std::cout << "警告: 无法创建备份文件，继续测试...\n";
		}
		else {
			std::cout << "备份文件创建成功: " << backupPath.toStdString() << "\n";
		}
	}
	else {
		std::cout << "警告: 配置文件不存在: " << configPath.toStdString() << "\n";
	}

	// 测试加载配置文件
	std::cout << "\n正在加载配置文件: " << configPath.toStdString() << "\n";
	MotionConfig config;
	bool loaded = CMotionConfig::GetInstance()->load(config, configPath);

	if (!loaded) {
		std::cout << "错误: 配置文件加载失败！\n";
		// 恢复备份
		if (QFile::exists(backupPath)) {
			QFile::remove(configPath);
			QFile::rename(backupPath, configPath);
		}
		return;
	}

	std::cout << "配置文件加载成功！\n\n";

	// 测试基础参数
	printSection("基础参数");
	std::cout << "文件夹路径: " << config.floderPath.toStdString() << "\n";

	// 测试网络参数
	printSection("网络参数");
	std::cout << "IP地址: " << config.ip.toStdString() << "\n";
	std::cout << "端口: " << config.port << "\n";

	// 测试打印坐标参数
	printSection("打印坐标参数");
	std::cout << "起始位置: (" << config.startPos.xPos << ", "
		<< config.startPos.yPos << ", " << config.startPos.zPos << ") um\n";
	std::cout << "结束位置: (" << config.endPos.xPos << ", "
		<< config.endPos.yPos << ", " << config.endPos.zPos << ") um\n";
	std::cout << "清理位置: (" << config.cleanPos.xPos << ", "
		<< config.cleanPos.yPos << ", " << config.cleanPos.zPos << ") um\n";

	// 测试轴步长参数
	printSection("轴步长参数");
	std::cout << "X轴步长: " << config.step.xPos << " um\n";
	std::cout << "Y轴步长: " << config.step.yPos << " um\n";
	std::cout << "Z轴步长: " << config.step.zPos << " um\n";

	// 测试轴最大距离限制参数
	printSection("轴最大距离限制参数");
	std::cout << "X轴最大距离: " << config.limit.xPos << " um\n";
	std::cout << "Y轴最大距离: " << config.limit.yPos << " um\n";
	std::cout << "Z轴最大距离: " << config.limit.zPos << " um\n";

	// 测试轴速度参数
	printSection("轴速度参数");
	std::cout << "X轴速度: " << config.speed.xPos << " um\n";
	std::cout << "Y轴速度: " << config.speed.yPos << " um\n";
	std::cout << "Z轴速度: " << config.speed.zPos << " um\n";

	// 验证关键值是否正确读取（与INI文件中的值对比）
	printSection("数据验证");
	bool allValid = true;

	// 验证网络参数
	if (config.ip != "192.168.1.100") {
		std::cout << "错误: IP地址不匹配！期望: 192.168.1.100, 实际: "
			<< config.ip.toStdString() << "\n";
		allValid = false;
	}
	if (config.port != 12355) {
		std::cout << "错误: 端口不匹配！期望: 12355, 实际: " << config.port << "\n";
		allValid = false;
	}

	// 验证坐标参数（整数比较，单位：微米）
	if (config.startPos.xPos != 1000 ||
		config.startPos.yPos != 1000 ||
		config.startPos.zPos != 0) {
		std::cout << "错误: 起始位置不匹配！期望: (1000, 1000, 0) um, 实际: ("
			<< config.startPos.xPos << ", " << config.startPos.yPos << ", " << config.startPos.zPos << ") um\n";
		allValid = false;
	}
	if (config.endPos.xPos != 2000 ||
		config.endPos.yPos != 2000 ||
		config.endPos.zPos != 20) {
		std::cout << "错误: 结束位置不匹配！期望: (2000, 2000, 20) um, 实际: ("
			<< config.endPos.xPos << ", " << config.endPos.yPos << ", " << config.endPos.zPos << ") um\n";
		allValid = false;
	}
	if (config.cleanPos.xPos != 100 ||
		config.cleanPos.yPos != 100 ||
		config.cleanPos.zPos != 0) {
		std::cout << "错误: 清理位置不匹配！期望: (100, 100, 0) um, 实际: ("
			<< config.cleanPos.xPos << ", " << config.cleanPos.yPos << ", " << config.cleanPos.zPos << ") um\n";
		allValid = false;
	}

	// 验证步长参数（单位：微米）
	if (config.step.xPos != 1000 ||
		config.step.yPos != 1000 ||
		config.step.zPos != 1000) {
		std::cout << "错误: 轴步长参数不匹配！期望: (1000, 1000, 1000) um, 实际: ("
			<< config.step.xPos << ", " << config.step.yPos << ", " << config.step.zPos << ") um\n";
		allValid = false;
	}

	// 验证速度参数（单位：微米）
	if (config.speed.xPos != 10 ||
		config.speed.yPos != 10 ||
		config.speed.zPos != 10) {
		std::cout << "错误: 轴速度参数不匹配！期望: (10, 10, 10) um, 实际: ("
			<< config.speed.xPos << ", " << config.speed.yPos << ", " << config.speed.zPos << ") um\n";
		allValid = false;
	}

	if (allValid) {
		std::cout << "所有配置项验证通过！\n";
	}

	// 测试保存功能
	printSection("保存功能测试");

	// 保存原始配置值
	MotionConfig originalConfig = config;
	MotionConfig testConfig = config;

	std::cout << "正在测试保存多个配置项...\n\n";

	// 修改多个配置项进行测试（单位：微米，整数）
	testConfig.ip = "192.168.1.200";
	testConfig.port = 9999;
	testConfig.startPos = MoveAxisPos(1500500, 1500500, 5500);  // 1500.5mm = 1500500um
	testConfig.endPos = MoveAxisPos(2500500, 2500500, 25500);    // 2500.5mm = 2500500um
	testConfig.cleanPos = MoveAxisPos(200500, 200500, 1500);     // 200.5mm = 200500um
	testConfig.step = MoveAxisPos(1500500, 1500500, 1500500);    // 1500.5mm = 1500500um
	testConfig.limit = MoveAxisPos(2000500, 2000500, 2000500);   // 2000.5mm = 2000500um
	testConfig.speed = MoveAxisPos(15500, 16500, 17500);         // 15.5mm = 15500um, 16.5mm = 16500um, 17.5mm = 17500um
	testConfig.floderPath = "D:\\Test\\NewPath";

	bool saved = CMotionConfig::GetInstance()->save(testConfig);
	if (!saved) {
		std::cerr << "错误: 配置文件保存失败！\n";
	}
	else {
		std::cout << "配置文件保存成功！\n";

		// 重新加载验证
		std::cout << "\n正在重新加载配置文件进行验证...\n";

		MotionConfig reloadedConfig;
		CMotionConfig::GetInstance()->load(reloadedConfig, configPath);
		bool saveTestPassed = true;

		// 验证网络参数
		if (reloadedConfig.ip != "192.168.1.200") {
			std::cout << "错误: IP地址保存验证失败！期望: 192.168.1.200, 实际: "
				<< reloadedConfig.ip.toStdString() << "\n";
			saveTestPassed = false;
		}
		if (reloadedConfig.port != 9999) {
			std::cout << "错误: 端口保存验证失败！期望: 9999, 实际: " << reloadedConfig.port << "\n";
			saveTestPassed = false;
		}

		// 验证坐标参数（单位：微米，整数）
		if (reloadedConfig.startPos.xPos != 1500500 ||
			reloadedConfig.startPos.yPos != 1500500 ||
			reloadedConfig.startPos.zPos != 5500) {
			std::cout << "错误: 起始位置保存验证失败！期望: (1500500, 1500500, 5500) um, 实际: ("
				<< reloadedConfig.startPos.xPos << ", " << reloadedConfig.startPos.yPos << ", " << reloadedConfig.startPos.zPos << ") um\n";
			saveTestPassed = false;
		}
		if (reloadedConfig.endPos.xPos != 2500500 ||
			reloadedConfig.endPos.yPos != 2500500 ||
			reloadedConfig.endPos.zPos != 25500) {
			std::cout << "错误: 结束位置保存验证失败！期望: (2500500, 2500500, 25500) um, 实际: ("
				<< reloadedConfig.endPos.xPos << ", " << reloadedConfig.endPos.yPos << ", " << reloadedConfig.endPos.zPos << ") um\n";
			saveTestPassed = false;
		}
		if (reloadedConfig.cleanPos.xPos != 200500 ||
			reloadedConfig.cleanPos.yPos != 200500 ||
			reloadedConfig.cleanPos.zPos != 1500) {
			std::cout << "错误: 清理位置保存验证失败！期望: (200500, 200500, 1500) um, 实际: ("
				<< reloadedConfig.cleanPos.xPos << ", " << reloadedConfig.cleanPos.yPos << ", " << reloadedConfig.cleanPos.zPos << ") um\n";
			saveTestPassed = false;
		}

		// 验证步长参数（单位：微米）
		if (reloadedConfig.step.xPos != 1500500 ||
			reloadedConfig.step.yPos != 1500500 ||
			reloadedConfig.step.zPos != 1500500) {
			std::cout << "错误: 轴步长参数保存验证失败！期望: (1500500, 1500500, 1500500) um, 实际: ("
				<< reloadedConfig.step.xPos << ", " << reloadedConfig.step.yPos << ", " << reloadedConfig.step.zPos << ") um\n";
			saveTestPassed = false;
		}

		// 验证限制参数（单位：微米）
		if (reloadedConfig.limit.xPos != 2000500 ||
			reloadedConfig.limit.yPos != 2000500 ||
			reloadedConfig.limit.zPos != 2000500) {
			std::cout << "错误: 轴最大距离限制参数保存验证失败！期望: (2000500, 2000500, 2000500) um, 实际: ("
				<< reloadedConfig.limit.xPos << ", " << reloadedConfig.limit.yPos << ", " << reloadedConfig.limit.zPos << ") um\n";
			saveTestPassed = false;
		}

		// 验证速度参数（单位：微米）
		if (reloadedConfig.speed.xPos != 15500 ||
			reloadedConfig.speed.yPos != 16500 ||
			reloadedConfig.speed.zPos != 17500) {
			std::cout << "错误: 轴速度参数保存验证失败！期望: (15500, 16500, 17500) um, 实际: ("
				<< reloadedConfig.speed.xPos << ", " << reloadedConfig.speed.yPos << ", " << reloadedConfig.speed.zPos << ") um\n";
			saveTestPassed = false;
		}

		// 验证文件夹路径
		if (reloadedConfig.floderPath != "D:\\Test\\NewPath") {
			std::cout << "错误: 文件夹路径保存验证失败！期望: D:\\Test\\NewPath, 实际: "
				<< reloadedConfig.floderPath.toStdString() << "\n";
			saveTestPassed = false;
		}

		if (saveTestPassed) {
			std::cout << "\n所有保存的值验证通过！\n";
		}
		else {
			std::cout << "\n部分保存的值验证失败！\n";
		}
	}


	// 恢复原始配置文件
	printSection("恢复原始配置");
	std::cout << "正在恢复原始配置文件...\n";
	if (QFile::exists(backupPath)) {
		if (QFile::exists(configPath)) {
			QFile::remove(configPath);
		}
		if (QFile::rename(backupPath, configPath)) {
			std::cout << "原始配置文件已恢复！\n";
		}
		else {
			std::cout << "警告: 无法恢复原始配置文件，请手动恢复: " << backupPath.toStdString() << "\n";
		}
	}
	else {
		std::cout << "警告: 备份文件不存在，无法恢复！\n";
	}

	printSeparator();
	std::cout << "\n测试完成！\n";

}


void testSpdlog() 
{
	SpdlogWrapper::GetInstance()->Init("logs");

	QString q1("qt english");
	QString q2("字符串");

	LOG_D("LOG ==================== LOG_T =====================");
	LOG_T("LOG hello {}", 123456);
	LOG_D("LOG 中文日志 {}", "附加中文");
	LOG_I("LOG qt str {}", q1);
	LOG_W("LOG qt 中文  {}", q2);
	LOG_E("LOG =====================");


	LOGP->debug("=================== LOGP =================");
	LOGP->trace("LOGP hello {}", 123456);
	LOGP->debug("LOGP 中文日志 {}", "附加中文");
	LOGP->info("LOGP qt str {}", q1);
	LOGP->warn("LOGP qt 中文  {}", q2);
	LOGP->error("LOGP =====================");


	LOGP->debug("=================== LOGPN =================");

	LOGPN("AAA")->trace("LOGPN hello {}", 123456);
	LOGPN("AAA")->debug("LOGPN 中文 {}", 11111);
	LOGPN("AAA")->info("LOGPN aaa file {} {} {}", 11111, q1, q2);
	LOGPN("AAA")->warn("LOGPN 11111 {} {} {}", "中文", "123", "english");
	LOGPN("AAA")->error("LOGPN =================================");


	LOGP->debug("=================== NAMED_LOG ===================");
	NAMED_LOG_T("AAA", "NAMED_LOG_T hello {}", q1);
	NAMED_LOG_D("AAA", "NAMED_LOG_T hello {}", q2);
	NAMED_LOG_I("AAA", "NAMED_LOG_T 中文 {} {}", 123, 456);
	NAMED_LOG_W("AAA", "NAMED_LOG_T 中文1 {}", "中文2");
	NAMED_LOG_E("AAA", "NAMED_LOG_T ================================");
}

void testQtDataLog()
{

	LOGP->debug("=================== Qt容器日志测试 ===================\n");
	// 测试QList<int>
	QList<int> a = { 1, 2, 3, 4, 5 };
	LOGP->info("QList<int>格式化测试: ");
	NAMED_LOG_I("AAA","QList<int>: {}", a);

	// 测试QVector<double>
	QVector<double> doubleVec = { 1.1, 2.2, 3.3 };
	NAMED_LOG_I("AAA", "QVector<double>: {}", doubleVec);
	LOGP->info("QVector<double>格式化测试完成\n");

	// 测试QMap<QString, int>
	QMap<QString, int> strIntMap;
	strIntMap["key1"] = 100;
	strIntMap["key2"] = 200;
	strIntMap["key3"] = 300;
	NAMED_LOG_I("AAA", "QMap<QString,int>: {}", strIntMap);
	LOGP->info("QMap<QString,int>格式化测试完成\n");

	// 测试QHash<int, QString>
	QHash<int, QString> intStrHash;
	intStrHash[1] = "value1";
	intStrHash[2] = "value2";
	intStrHash[3] = "value3";
	NAMED_LOG_I("AAA", "QHash<int,QString>: {}", intStrHash);
	LOGP->info("QHash<int,QString>格式化测试完成\n");

	// 测试QByteArray
	QByteArray byteArray("Hello Qt!");
	NAMED_LOG_I("AAA", "QByteArray: {}", byteArray);
	LOGP->info("QByteArray格式化测试完成\n");

	// 测试嵌套容器
	QList<QVector<int>> nestedList;
	QVector<int> vec1 = { 1, 2 };
	QVector<int> vec2 = { 3, 4, 5 };
	nestedList.append(vec1);
	nestedList.append(vec2);
	NAMED_LOG_I("AAA", "QList<QVector<int>>: {}", nestedList);
	LOGP->info("嵌套容器格式化测试完成\n");

	// 测试空容器
	QList<int> emptyList;
	QMap<QString, int> emptyMap;
	NAMED_LOG_I("AAA", "Empty QList: {}", emptyList);
	NAMED_LOG_I("AAA", "Empty QMap: {}", emptyMap);
	LOGP->info("空容器格式化测试完成\n");

	// 测试QVariant
	QVariant varInt(42);
	QVariant varStr("Hello World");
	QVariant varBool(true);
	QVariant varDouble(3.14159);
	NAMED_LOG_I("AAA", "QVariant(int): {}", varInt);
	NAMED_LOG_I("AAA", "QVariant(string): {}", varStr);
	NAMED_LOG_I("AAA", "QVariant(bool): {}", varBool);
	NAMED_LOG_I("AAA", "QVariant(double): {}", varDouble);
	LOGP->info("QVariant格式化测试完成\n");

	// 测试 QStringList
	QStringList strList = { "Apple", "Banana", "Cherry" };
	NAMED_LOG_I("AAA", "QStringList: {}", strList);
	LOGP->info("QStringList格式化测试完成 \n");

	// 测试 QDateTime (通过 QVariant 打印)
	QDateTime now = QDateTime::currentDateTime();
	NAMED_LOG_I("AAA", "Current Time: {}", QVariant(now));

	// 测试QSet
	QSet<int> intSet = { 1, 2, 3, 5, 8, 13 };
	NAMED_LOG_I("AAA", "QSet<int>: {}", intSet);
	LOGP->info( "QSet测试完成\n");

	// 测试QPair
	QPair<int, QString> pair(42, "answer");
	NAMED_LOG_I("AAA", "QPair<int,QString>: {}", pair);
	LOGP->info( "QPair测试完成\n");

	// 测试QDateTime
	QDateTime now1 = QDateTime::currentDateTime();
	NAMED_LOG_I("AAA", "QDateTime: {}", now1);
	LOGP->info( "QDateTime测试完成\n");

	// 测试QDate
	QDate date = QDate::currentDate();
	NAMED_LOG_I("AAA", "QDate: {}", date);
	LOGP->info( "QDate测试完成\n");

	// 测试QTime
	QTime time = QTime::currentTime();	
	NAMED_LOG_I("AAA", "QTime: {}", time);
	LOGP->info( "QTime测试完成\n");

	// 测试QList<QStringList>
	QList<QStringList> listOfLists;
	listOfLists.append({ "a", "b" });
	listOfLists.append({ "x", "y", "z" });
	NAMED_LOG_I("AAA", "QList<QStringList>: {}", listOfLists);
	LOGP->info( "QList<QStringList>测试完成\n");

	// 测试QMap<QString, QVariant>
	QMap<QString, QVariant> configMap;
	configMap["port"] = 8080;
	configMap["host"] = "localhost";
	configMap["debug"] = true;
	NAMED_LOG_I("AAA", "QMap<QString,QVariant>: {}", configMap);
	LOGP->info( "QMap<QString,QVariant>测试完成\n");


	// 测试QPoint
	QPoint pt(100, 200);
	NAMED_LOG_I("AAA", "QPoint: {}", pt);
	LOGP->info( "QPoint测试完成\n");

	// 测试QPointF
	QPointF ptf(10.5, 20.75);
	NAMED_LOG_I("AAA", "QPointF: {}", ptf);
	LOGP->info( "QPointF测试完成\n");

	// 测试QSize
	QSize sz(1920, 1080);
	NAMED_LOG_I("AAA", "QSize: {}", sz);
	LOGP->info( "QSize测试完成\n");

	// 测试QSizeF
	QSizeF szf(1920.5, 1080.25);
	NAMED_LOG_I("AAA", "QSizeF: {}", szf);
	LOGP->info( "QSizeF测试完成\n");

	// 测试QRect
	QRect rect(10, 20, 300, 400);
	NAMED_LOG_I("AAA", "QRect: {}", rect);
	LOGP->info( "QRect测试完成\n");

	// 测试QRectF
	QRectF rectf(10.5, 20.5, 300.75, 400.25);
	NAMED_LOG_I("AAA", "QRectF: {}", rectf);
	LOGP->info( "QRectF测试完成\n");

	// 测试QUrl
	QUrl url("https://www.qt.io/download?param=1");
	NAMED_LOG_I("AAA", "QUrl: {}", url);
	LOGP->info( "QUrl测试完成\n");

	// 测试QUuid
	QUuid uuid = QUuid::createUuid();
	NAMED_LOG_I("AAA", "QUuid: {}", uuid);
	LOGP->info( "QUuid测试完成\n");

	// 测试QLine
	QLine line(0, 0, 100, 100);
	NAMED_LOG_I("AAA", "QLine: {}", line);
	LOGP->info( "QLine测试完成\n");

	// 测试QLineF
	QLineF linef(0.5, 0.5, 100.5, 100.5);
	NAMED_LOG_I("AAA", "QLineF: {}", linef);
	LOGP->info( "QLineF测试完成\n");

	// 测试QMargins
	QMargins margins(10, 20, 10, 20);
	NAMED_LOG_I("AAA", "QMargins: {}", margins);
	LOGP->info( "QMargins测试完成\n");

	// 测试QMarginsF
	QMarginsF marginsf(10.5, 20.5, 10.5, 20.5);
	NAMED_LOG_I("AAA", "QMarginsF: {}", marginsf);
	LOGP->info( "QMarginsF测试完成\n");

}


#include <QSettings>
#include <QDebug>
#include <QVariant>

int main()
{
#ifdef _WIN32
	// 设置控制台代码页为 UTF-8，解决中文乱码问题
	// 65001 是 UTF-8 的代码页编号
	SetConsoleOutputCP(65001);  // 设置输出代码页为 UTF-8
	SetConsoleCP(65001);         // 设置输入代码页为 UTF-8
#endif
	
	// 测试轴参数配置类
	testMotionConfig();


#if 0
	QSettings settings("config/motionMoudleConfig2.ini", QSettings::IniFormat);
	settings.beginGroup("print_pos_param");
	QVariant v1 = settings.value("print_start_pos");
	QVariant v2 = settings.value("print_end_pos");
	
	qDebug() << v1.type();
	qDebug() << v2.type();

	qDebug() << v1.toString();
	qDebug() << v1.toStringList();

	settings.endGroup();

#endif
	

	testSpdlog();
	testQtDataLog();
	

	return 0;
}

#endif
