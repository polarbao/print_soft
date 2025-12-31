#pragma once


// 后续更改为按组名进行设置，获取
#include <memory>  
#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>
#include <QVector>

#include "comm/CSingleton.h"

// 导入事件类型定义
#include "motionControlSDK.h"

class CINIConfig : public QObject, public CSingleton<CINIConfig>
{
	friend class CSingleton<CINIConfig>;
	Q_OBJECT
	
	struct SIniCfgParamD
	{
		QVector<qint32> vPosData;
		QVector<QString> vStr;
	};

public:

	/**
	*  @brief       brief
	*  @param[in]
	*  @param[out]
	*  @return
	*/
	void SetStrData(const QString& str, const QString& data);
	void SetDoubleData(const QString& str, double& data);
	void SetPosData(const QString& str, const QString& cfgType, const MoveAxisPos& data);


	void GetStrData(const QString& str, QString& data);
	void GetDoubleData(const QString& str, double& data);

	///**
	//*  @brief       brief
	//*  @param[in]
	//*  @param[out]
	//*  @return
	//*/
	//SIniCfgParamD GetIniCfgMsg();

	//void ReadINIFile();

	//void SetINIFile(const SIniCfgParamD& data);

	//void SetPathOptParam(const QString& str);

	//SIniCfgParamD GetPathOptParam();


private:
	/** 
	*  @brief       brief 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	CINIConfig(QObject * parent = nullptr);


	/** 
	*  @brief       brief 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	~CINIConfig() = default;

	/** 
	*  @brief       brief 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	void Sync();




private:
	
	//SIniCfgParamD m_iniData;
	std::unique_ptr<QSettings> m_setting;  
};
