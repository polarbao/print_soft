/**
 * @file        CMotionConfig.h
 * @brief       运动控制参数类
 * @author      
 * @date        2025-12-30 10:44:09
 * @details     运动控制参数类, 负责将运动控制相关参数从ini配置文件中加载, 以及重新写入到ini配置文件中
 */


#pragma once
#include <QObject>
#include "CSingleton.h"
#include "CNewSingleton.h"
#include "motioncontrolsdk_define.h"


class CMotionConfig: public CNewSingleton<CMotionConfig>
{
	friend CNewSingleton<CMotionConfig>;
public:
	// 加载配置文件
	bool load(MotionConfig& config, const QString& iniPath="config/motionMoudleConfig.ini");

	// 保存配置到指定路径，如果路径为空则保存到加载时的路径
	bool save(const MotionConfig& config, const QString& iniPath = QString());

private:
	QString m_iniFilePath;
};

