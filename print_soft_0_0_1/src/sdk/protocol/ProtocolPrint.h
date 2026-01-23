#pragma once
#include <QtCore/QtCore>
#include "communicate/TcpClient.h"
// 导入事件类型定义
#include "motionControlSDK_define.h"


//class DataFieldInfo1;

/** 
*  @author      
*  @class       ProtocolPrint
*  @brief       打印机协议
*/
class ProtocolPrint : public QObject
{
	Q_OBJECT
public:
	ProtocolPrint(QObject* parent = 0);


	/**  命令组功能码  **/
	enum PackageHeadType 
	{
		Head_AABB = 0xAABB,
		Head_AACC = 0xAACC,
		Head_AADD = 0xAADD
	};

	enum ECmdType 
	{
		SetParamCmd		= 0x0001,
		GetCmd			= 0x0010,
		CtrlCmd			= 0x0011,
		PrintCommCmd	= 0x00F0
	};

	enum FunCode
	{
		// 设置参数
		SetParam_CleanPos = 0x1000,
		SetParam_PrintStartPos = 0x1001,
		SetParam_PrintEndPos = 0x1002,
		SetParam_OriginOffset = 0x1003,

		//设置经验值步进移动（Y Z
		SetParam_AxisUnitMove = 0x1010,
		SetParam_MaxLimitPos = 0x1020,
		SetParam_AxistSpd = 0x1030,
		SetParam_AxistAccSpd = 0x1040,
		SetParam_LayerPassNum = 0x1050,
		SetParam_End = 0x1FFF,

		// 获取命令
		Get_AxisPos = 0x2000,
		Get_Breath = 0x2010,
		Get_End = 0x2FFF,


		// 控制命令
		Ctrl_StartPrint		= 0x3000,
		Ctrl_PasusePrint	= 0x3001,
		Ctrl_ContinuePrint	= 0x3002,
		Ctrl_StopPrint		= 0x3003,
		Ctrl_MoveOrigin		= 0x3004,
		Ctrl_MoveCleanPos	= 0x3005,
		Ctrl_EmergenctStop	= 0x3006,
		Ctrl_RecoveryEmergenctStop = 0x3007,


		//[手动控制] x 左右 y 前后 z 上下
		Ctrl_XAxisLMove = 0x3101,
		Ctrl_XAxisRMove = 0x3102,
		Ctrl_YAxisLMove = 0x3103,
		Ctrl_YAxisRMove = 0x3104,
		Ctrl_ZAxisLMove = 0x3105,
		Ctrl_ZAxisRMove = 0x3106,

		Ctrl_AxisAbsMove = 0x3107,		//移动到绝对位置
		Ctrl_AxisRelMove = 0x3108,		//移动到相对位置

		Ctrl_End = 0xEFFF,

		// 打印通讯
		// Y轴移动pass举例，Z轴移动的层数数据
		Print_AxisMovePos = 0xF000,
		Print_SetLayerData = 0xF001,
		//Print_SetAxisStep = 0xF001,
		//Print_LayerHome = 0xF002,
		//Print_EnablePass = 0xF003,

		Print_PeriodData = 0xF0010,
		Print_End = 0xFFFF


		//// =============================================================================
		//// =============================================================================
		//// =============================================================================

		////-----------------下位机定时周期推送的数据命令，上位机不需要回复--------------------------//
		//Period_AllPara = 0x0430							//周期发送过来的数据命令

	};

	enum ErrorCode
	{
		Command_Err = 0x01,      //命令码错误
		DataField_Err = 0x02,    //数据域错误
		Crc_Err = 0x03,          //校验错误
		Status_Err = 0x04        //状态错误
	};


public:
		//数据包格式：
		//格式如下：包头（2byte) + 长度（2byte） + 命令（2byte) + 地址（1byte) + 数据区（动态，从0到n） + crc16校验(2byte) + 包尾(2byte)

		/**
		*  @brief       处理接收报文
		*  @param[in]
		*  @param[out]
		*  @return
		*/
		void HandleRecvDatagramData(QByteArray datagram);
		void HandleRecvDatagramData1(QByteArray datagram);

		/**
		*  @brief       解析报文数据
		*  @param[in]   datagram:报文数据
		*  @param[out]
		*  @return
		*/
		void ParsePackageData(QByteArray& datagram, PackageHeadType type = PackageHeadType::Head_AABB);

		void ParseReqPackageData(QByteArray& datagram, PackageHeadType type);

		void ParseRespPackageData(QByteArray& datagram, PackageHeadType type);

		//void HandleFailedRespPackageData(QByteArray& datagram, PackageHeadType type);

		/**
		*  @brief       根据参数组成主动请求的包
		*  @param[in]
		*  @param[out]
		*  @return
		*/
		static QByteArray GetSendDatagram(ECmdType cmdType, FunCode code,  QByteArray data = QByteArray());

		/**
		*  @brief       根据参数组成回复包（整合2个PackData
		*  @param[in]
		*  @param[out]
		*  @return
		*/
		static QByteArray GetRespDatagram(FunCode code, QByteArray data = QByteArray());

signals:
	//--------------------------解析完数据发送相关信号-----------------------------//
	//心跳信号
	void SigHeartBeat();

	void SigCmdReply(int cmd, uchar errCode, QByteArray arr = QByteArray());

	//设备运动参数 SpeedRotatePara
	void sigSpeedRotatePara();

	// 接收报文数据显示错误，进入重发模式
	void SigPackFailRetransport(QByteArray& arr);

	// 获取下位机请求报文数据信号
	void SigHandleReqFunOper(int codeType, int command);

	// 获取下位机返回报文数据信号
	void SigHandleRespFunOper(const PackParam& data);
	
	// 获取下位机各轴位置数据信号
	void SigHandleAxisPosData(int cmdType, const MoveAxisPos& data);


private:

	int HandleCheckPackageHead(const QByteArray& data, int pos);


	/** 
	*  @brief       处理下位机回复包
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/ 
	void HandleResponseData(ushort code, uchar* data, ushort length, QByteArray arr = QByteArray());



	// 预留部分接口
	/**
	*  @brief       处理下位机周期发送过来的数据
	*  @param[in]    data:数据域地址  lSigCmdReply
	*  @param[out]
	*  @return
	*/
	void HandlePeriodData(uchar* data, ushort length);


private:
	//接受数据待处理缓存
	QByteArray m_recvBuf;

	//下位机返回错误码次数
	int m_codeErrorNum = 0;
};

///** 
//*  @author      
//*  @class       DataFieldInfo 
//*  @brief       协议数据信息
//*/
//class DataFieldInfo1
//{
//public:
//	DataFieldInfo1();
//
//	//控制权状态  0x01: 控制权在执行机构  0x02: 控制权在远程操作台（摇杆和触摸屏可控制执行机构)
//	uchar _controlStatus = 1;  
//	//按键切换状态  0x01表示左， 0x02表示右
//	uchar _switchStatus;
//	//速度提示音状态  0x00: 无声音  0x01: 低速声音  0x02: 中速声音 0x03：高速声音
//	uchar _speedSoudStatus = 0;
//	//设备故障码
//	short _deviceFaultCode = 0;
//	//下位机软件版本号
//	QString _softVersion;
//	//下位机硬件版本号
//	QString _hardwareVersion;
//	
//};

