#pragma once

#include <atomic>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <regex>

#include <QObject>
#include <QMetaType>
#include <QString>
#include <QDateTime>
#include <QMap>

#include "CLogManager.h"


#define SerialVer1_0

//----------------------------enum-------------------------------------
//----------------------------enum-------------------------------------


//----------------------------LOGIC_ENUM----------------------------
//----------------------------LOGIC_ENUM----------------------------
//----------------------------LOGIC_ENUM----------------------------

//服务器返回错误类型
namespace ELogic
{
	typedef enum DeviceMoveDirectionType
	{
		EDMDT_Begin,
		EDMDT_XAxis,
		EDMDT_YAxis,
		EDMDT_ZAxis,
		EDMDT_Reset,
		EDMDT_End

	}EDMDT;
	Q_DECLARE_METATYPE(EDMDT)

	typedef enum MoveBtnType : int
	{
		EMBT_Begin = EDMDT_End + 1,
		EMBT_XAxis,
		EMBT_YAxis,
		EMBT_ZAxis,
		EMBT_Reset,
		EMBT_End

	}EMBT;
	Q_DECLARE_METATYPE(EMBT)
}



//----------------------------UI_ENUM----------------------------
//----------------------------UI_ENUM----------------------------
//----------------------------UI_ENUM----------------------------
namespace EUI
{
	typedef enum PrintUIBtnType
	{
		EPUIBT_Begin = ELogic::EMBT_End + 1,
		EPUIBT_Start,
		EPUIBT_Pause,
		EPUIBT_Stop,
		EPUIBT_Reset,
		EPUIBT_End
	}EPUIBT;
	Q_DECLARE_METATYPE(EPUIBT)

	typedef enum MoveUIBtnType
	{
		EMUIBT_Begin = EPUIBT_End + 1,
		//按钮分类
		EMUIBT_XAxis,
		EMUIBT_YAxis,
		EMUIBT_ZAxis,
		//具体btn类型
		EMUIBT_XAxisForward,
		EMUIBT_XAxisBackward,

		EMUIBT_YAxisForward,
		EMUIBT_YAxisBackword,

		EMUIBT_ZAxisForward,
		EMUIBT_ZAxisBackward,
		EMUIBT_Reset,
		EMUIBT_EmergencyStop,
		EMUIBT_Stop,
		EMUIBT_End
	}EMUIBT;
	Q_DECLARE_METATYPE(EMUIBT)
}




//----------------------------struct-------------------------------------
//----------------------------struct-------------------------------------

typedef struct SerialParam
{
	QString comNum;		//string
	QString baudRate;	//int
	QString dataBits;	//int
	QString stopBits;	//float
	QString checkBits;	//string


	SerialParam();
	//HttpCommBody& operator = (const HttpCommBody& other);
	void IsEmpty();
	void Clear();

}SSP;
Q_DECLARE_METATYPE(SSP)

