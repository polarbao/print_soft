
#include "CINIConfig.h"
#include "CLogManager.h"
#include <QDir>






CINIConfig::CINIConfig(QObject* parent)
	: QObject(parent)
{
	// 判断是否为release路径
	auto iniPath = QDir::currentPath() + QString("/config/motionMoudleConfig.ini");
	m_setting = std::make_unique<QSettings>(iniPath, QSettings::IniFormat);
	LOG_INFO(QString("motion_moudle_sdk read_motion_cfg_file_path: %1").arg(iniPath));

}


void CINIConfig::SetStrData(const QString& str, const QString& data)
{
	m_setting->setValue(str, data);
}

void CINIConfig::SetDoubleData(const QString& str, double& data)
{
	m_setting->setValue(str, data);
}

void CINIConfig::SetPosData(const QString& str, const QString& cfgType, const MoveAxisPos& data)
{
	QString prefixStr = "_axis_" + cfgType;

}

void CINIConfig::GetStrData(const QString& str, QString& data)
{
	data = m_setting->value(str).toString();
}

void CINIConfig::GetDoubleData(const QString& str, double& data)
{
	data = m_setting->value(str).toDouble();
}

//SIniCfgParamD CINIConfig::GetIniCfgMsg()
//{
//	ReadINIFile();
//	return m_iniData;
//}
//
//void CINIConfig::ReadINIFile()
//{
//	SIniCfgParamD tmpData;
//	//base_param
//	tmpData.folderPath = m_setting->value("/base_param/floder_path").toString();
//	tmpData.user = m_setting->value("/base_param/user_authority").toString();
//
//	//net_param
//	tmpData.listenIP = m_setting->value("/net_param/listen_ip").toString();
//	tmpData.listenPort = m_setting->value("/net_param/listen_port").toString();
//	if (tmpData.listenIP.isEmpty() || tmpData.listenPort.isEmpty())
//	{
//		tmpData.listenIP = tmpData.listenIP.isEmpty() ? QString("127.0.0.1") : tmpData.listenIP;
//		tmpData.listenPort = tmpData.listenPort.isEmpty() ? QString("6666") : tmpData.listenPort;
//		//更新文件ip、port
//		SetINIFile(tmpData);
//	}
//
//	//finger_dispose
//	tmpData.pathDisposePath = m_setting->value("/finger_param/finger_exe_path").toString();
//	tmpData.meshFilePath = m_setting->value("/finger_param/mesh_file_path").toString();
//	tmpData.fingerOutputPath = m_setting->value("/finger_param/output_path").toString();
//	tmpData.distScaleVal = m_setting->value("/finger_param/dist_scale_val").toFloat();
//	tmpData.zAxistMaxVal = m_setting->value("/finger_param/z_axist_max_val").toFloat();
//
//	SetIniCfgMsg(tmpData);
//}
//
//
//void CINIConfig::SetINIFile(const SIniCfgParamD& data)
//{
//
//	//base_param
//	m_setting->setValue("/base_param/user_authority", data.user);
//	m_setting->setValue("/base_param/floder_path", data.folderPath);
//
//	//net_param
//	m_setting->setValue("/net_param/listen_ip", data.listenIP);
//	m_setting->setValue("/net_param/listen_port", data.listenPort);
//	
//	//finger_param
//	m_setting->setValue("/finger_param/finger_exe_path", data.pathDisposePath);
//	m_setting->setValue("/finger_param/mesh_file_path", data.meshFilePath);
//	m_setting->setValue("/finger_param/output_path", data.fingerOutputPath);
//	m_setting->setValue("/finger_param/dist_scale_val", data.distScaleVal);
//	m_setting->setValue("/finger_param/z_axist_max_val", data.zAxistMaxVal);
//}
//
//void CINIConfig::SetPathOptParam(const QString& str)
//{
//	m_setting->setValue("/finger_param/mesh_file_path", str);
//
//}
//
//SIniCfgParamD CINIConfig::GetPathOptParam()
//{
//	m_iniData.pathDisposePath = m_setting->value("/finger_param/finger_exe_path").toString();
//	m_iniData.meshFilePath = m_setting->value("/finger_param/mesh_file_path").toString();
//	m_iniData.fingerOutputPath = m_setting->value("/finger_param/output_path").toString();
//	m_iniData.distScaleVal = m_setting->value("/finger_param/dist_scale_val").toFloat();
//	m_iniData.zAxistMaxVal = m_setting->value("/finger_param/z_axist_max_val").toFloat();
//	return m_iniData;
//}

