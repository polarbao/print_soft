
#ifdef SDK_TEST
#include <iostream>
#include <iomanip>
#include <cmath>
#include <QFile>
#include <QDir>
#include "comm/CMotionConfig.h"

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif


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
	bool loaded = CMotionConfig::GetInstance().load(configPath);

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

	// 获取配置
	MotionConfig config = CMotionConfig::GetInstance().config();

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
	MotionConfig& testConfig = CMotionConfig::GetInstance().config();

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

	bool saved = CMotionConfig::GetInstance().save();
	if (!saved) {
		std::cout << "错误: 配置文件保存失败！\n";
	}
	else {
		std::cout << "配置文件保存成功！\n";

		// 重新加载验证
		std::cout << "\n正在重新加载配置文件进行验证...\n";
		CMotionConfig::GetInstance().load(configPath);
		MotionConfig reloadedConfig = CMotionConfig::GetInstance().config();

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


	return 0;
}

#endif
