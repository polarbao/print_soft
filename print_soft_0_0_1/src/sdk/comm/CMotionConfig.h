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

namespace ConfigKeys {
	const QString Group_Net = "net_param";
	const QString Key_IP = "ip";
	const QString Key_Port = "port";

	const QString Group_PrintPos	= "print_pos_param";
	const QString Key_StartPos		= "print_start_pos";
	const QString Key_EndPos		= "print_start_pos";
	const QString Key_CleanPos		= "print_start_pos";

	const QString Group_MotionStep	= "print_pos_param";
	const QString Key_XStep			= "x_axis_step";
	const QString Key_YStep			= "x_axis_step";
	const QString Key_ZStep			= "x_axis_step";

	const QString Group_MotionLimit = "print_pos_param";
	const QString Key_XLimit		= "x_axis_limit";
	const QString Key_YLimit		= "x_axis_limit";
	const QString Key_ZLimit		= "x_axis_limit";

	const QString Group_MotionSpd	= "print_pos_param";
	const QString Key_XSpeed		= "x_axis_speed";
	const QString Key_YSpeed		= "x_axis_speed";
	const QString Key_ZSpeed		= "x_axis_speed";

	const QString Group_MotionAcc	= "print_pos_param";
	const QString Key_XAcc			= "x_axis_accelerate";
	const QString Key_YAcc			= "x_axis_accelerate";
	const QString Key_ZAcc			= "x_axis_accelerate";


	const QString Group_MotionOffset = "print_pos_param";
	const QString Key_XOffset		= "x_axis_offset";
	const QString Key_YOffset		= "x_axis_offset";
	const QString Key_ZOffset		= "x_axis_offset";

	const QString Group_MotionLayerData = "print_pos_param";
	const QString Key_XLayer		= "x_axis_total";
	const QString Key_YLayer		= "x_axis_total";
	const QString Key_ZLayer		= "x_axis_total";

}

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

