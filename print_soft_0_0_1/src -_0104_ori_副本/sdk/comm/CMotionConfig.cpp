#include "CMotionConfig.h"
#include <QSettings>
#include <QStringList>
#include <QFileInfo>
#include <QVariant>
#include <QTextCodec>


// 辅助函数：从 QSettings 中读取包含逗号的字符串值
// QSettings 会将包含逗号的值解析为 QStringList，需要特殊处理
static QString readCommaSeparatedValue(QSettings& settings, const QString& key, const QString& defaultValue)
{
	QVariant var = settings.value(key, defaultValue);
	
	// 如果 QSettings 将值解析为 QStringList（因为包含逗号）
	if (var.type() == QVariant::StringList) {
		QStringList list = var.toStringList();
		// 将列表用逗号连接
		return list.join(",");
	}
	
	// 如果是普通字符串，直接返回
	return var.toString().trimmed();
}

// 辅助函数：解析坐标字符串 "x,y,z" 为 MoveAxisPos
// 配置文件中的值是微米（um），直接转换为 quint32 存储
static MoveAxisPos parseMoveAxisPos(const QString& posStr)
{
	quint32 x = 0, y = 0, z = 0;
	QStringList parts = posStr.split(',');
	if (parts.size() >= 1) {
		x = parts[0].trimmed().toUInt();
	}
	if (parts.size() >= 2) {
		y = parts[1].trimmed().toUInt();
	}
	if (parts.size() >= 3) {
		z = parts[2].trimmed().toUInt();
	}
	// 配置文件存储的是微米，直接使用
	return MoveAxisPos(x, y, z);
}

// 辅助函数：将 MoveAxisPos 格式化为字符串 "x,y,z"（单位：微米）
static QString formatMoveAxisPos(const MoveAxisPos& pos)
{
	// 直接使用微米值保存
	return QString("%1,%2,%3").arg(pos.xPos).arg(pos.yPos).arg(pos.zPos);
}

bool CMotionConfig::load(const QString & iniPath)
{
	// 检查文件是否存在
	QFileInfo fileInfo(iniPath);
	if (!fileInfo.exists()) {
		return false;
	}

	QSettings settings(iniPath, QSettings::IniFormat);

	// 保存路径供 save 函数使用
	m_iniFilePath = iniPath;

	// 读取基础参数
	settings.beginGroup("base_param");
	m_config.floderPath = settings.value("floder_path", "").toString();
	settings.endGroup();

	// 读取网络参数
	settings.beginGroup("net_param");
	m_config.ip = settings.value("motion_ip", "192.168.1.100").toString();
	m_config.port = settings.value("motion_port", 12355).toInt();
	settings.endGroup();

	// 读取打印坐标参数
	settings.beginGroup("print_pos_param");
	// 使用特殊函数处理包含逗号的值（QSettings 会将其解析为 QStringList）
	QString startPosStr = readCommaSeparatedValue(settings, "print_start_pos", "1000,1000,0");
	QString endPosStr = readCommaSeparatedValue(settings, "print_end_pos", "1000,1000,0");
	QString cleanPosStr = readCommaSeparatedValue(settings, "print_clean_pos", "1000,1000,0");
	
	m_config.startPos = parseMoveAxisPos(startPosStr);
	m_config.endPos = parseMoveAxisPos(endPosStr);
	m_config.cleanPos = parseMoveAxisPos(cleanPosStr);
	settings.endGroup();

	// 读取轴步长参数（单位：微米）
	settings.beginGroup("motion_axis_step_param");
	quint32 xStep = settings.value("x_axis_step", 1000).toUInt();
	quint32 yStep = settings.value("y_axis_step", 1000).toUInt();
	quint32 zStep = settings.value("z_axis_step", 1000).toUInt();
	m_config.step = MoveAxisPos(xStep, yStep, zStep);
	settings.endGroup();

	// 读取轴最大距离限制参数（单位：微米）
	settings.beginGroup("motion_max_limit_param");
	quint32 xLimit = settings.value("x_axis_limit", 1000).toUInt();
	quint32 yLimit = settings.value("y_axis_limit", 1000).toUInt();
	quint32 zLimit = settings.value("z_axis_limit", 1000).toUInt();
	m_config.limit = MoveAxisPos(xLimit, yLimit, zLimit);
	settings.endGroup();

	// 读取轴速度参数（单位：微米）
	settings.beginGroup("motion_speed_param");
	quint32 xSpeed = settings.value("x_axis_speed", 10).toUInt();
	quint32 ySpeed = settings.value("y_axis_speed", 10).toUInt();
	quint32 zSpeed = settings.value("z_axis_speed", 10).toUInt();
	m_config.speed = MoveAxisPos(xSpeed, ySpeed, zSpeed);
	settings.endGroup();

	return true;
}

bool CMotionConfig::save(const QString& iniPath)
{
	// 确定保存路径：如果提供了路径参数则使用参数，否则使用加载时的路径
	QString savePath = iniPath.isEmpty() ? m_iniFilePath : iniPath;
	
	// 如果路径为空，无法保存
	if (savePath.isEmpty()) {
		return false;
	}

	QSettings settings(savePath, QSettings::IniFormat);

	// 保存基础参数
	settings.beginGroup("base_param");
	settings.setValue("floder_path", m_config.floderPath);
	settings.endGroup();

	// 保存网络参数
	settings.beginGroup("net_param");
	settings.setValue("motion_ip", m_config.ip);
	settings.setValue("motion_port", m_config.port);
	settings.endGroup();

	// 保存打印坐标参数
	settings.beginGroup("print_pos_param");
	settings.setValue("print_start_pos", formatMoveAxisPos(m_config.startPos));
	settings.setValue("print_end_pos", formatMoveAxisPos(m_config.endPos));
	settings.setValue("print_clean_pos", formatMoveAxisPos(m_config.cleanPos));
	settings.endGroup();

	// 保存轴步长参数（单位：微米）
	settings.beginGroup("motion_axis_step_param");
	settings.setValue("x_axis_step", m_config.step.xPos);
	settings.setValue("y_axis_step", m_config.step.yPos);
	settings.setValue("z_axis_step", m_config.step.zPos);
	settings.endGroup();

	// 保存轴最大距离限制参数（单位：微米）
	settings.beginGroup("motion_max_limit_param");
	settings.setValue("x_axis_limit", m_config.limit.xPos);
	settings.setValue("y_axis_limit", m_config.limit.yPos);
	settings.setValue("z_axis_limit", m_config.limit.zPos);
	settings.endGroup();

	// 保存轴速度参数（单位：微米）
	settings.beginGroup("motion_speed_param");
	settings.setValue("x_axis_speed", m_config.speed.xPos);
	settings.setValue("y_axis_speed", m_config.speed.yPos);
	settings.setValue("z_axis_speed", m_config.speed.zPos);
	settings.endGroup();

	// 同步写入文件
	settings.sync();
	
	return settings.status() == QSettings::NoError;
}

MotionConfig& CMotionConfig::config()
{
	return m_config;
}