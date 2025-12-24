#pragma once
#include <QtCore/QtCore>
#include "communicate/TcpClient.h"


// 导入事件类型定义
#include "motionControlSDK.h"



class DataFieldInfo1;
//class DataFieldInfo1;

////Coordinates
//struct MoveAxisPos
//{
//	uint32_t xPos;
//	uint32_t yPos;
//	uint32_t zPos;
//
//	MoveAxisPos();
//};

//struct PackParam
//{
//
//	uint16_t head;
//	uint16_t operType;
//	uint16_t cmdFun;
//	uint16_t dataLen;
//	uint8_t data[DATA_LEN_12];
//	uint16_t crc16;
//
//	PackParam();
//};


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
		
		//设置经验值步进移动（Y Z
		SetParam_AxisUnitMove = 0x1010,

		SetParam_MaxLimitPos = 0x1020,

		SetParam_AxistSpd = 0x1030,
		SetParam_End = 0x1FFF,

		// 获取命令
		Get_AxisPos = 0x2000,

		Get_Breath = 0x2010,
		Get_End = 0x2FFF,

		
		// 控制命令
		Ctrl_StartPrint = 0x3000,
		Ctrl_PasusePrint = 0x3001,
		Ctrl_ContinuePrint = 0x3002,
		Ctrl_StopPrint = 0x3003,

		Ctrl_ResetPos = 0x3004,

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
		Print_PeriodData = 0xF0001,
		Print_End = 0xFFFF


		//// =============================================================================
		//// =============================================================================
		//// =============================================================================

		//// 参数控制
		////上位机发送控制命令,下位机会返回数据包
		//Con_Breath = 0x0000,							//心跳

		//Set_StartPrint = 0x12A3,						//开始打印
		//Set_PausePrint = 0x1201,						//暂停打印
		//Set_StopPrint = 0x1202,							//停止打印
		//Set_continuePrint = 0x1203,						//继续打印
		//Set_ResetPrint = 0x1204,						//重置打印
		//Set_TransData = 0x1205,							//传输打印数据


		////--------------------上位机读下位机命令，下位机会返回数据包----------------------------//
		//Read_PrintStatus = 0x0300,						//读取打印状态
		//Read_PrintFinsh = 0x0301,						//读取打印数据

		////-----------------客户端推送操作命令，需树莓派确认回复（回复指令最高位+1--------------------------//
		////-----------------客户端推送操作命令，需树莓派确认回复（回复指令最高位+1--------------------------//
		//
		////打印位置
		//Set_XAxisMovePrintPos = 0x12A2,
		//Set_YAxisMovePrintPos = 0x12A5,
		//Set_ZAxisMovePrintPos = 0x12A2,

		////TODO: 移动位置协议更新指令操作
		////移动操作
		////Set_XAxisUpAuto = 0x12A7,
		////Set_XAxisDownAuto = 0x1207,
		////Set_XAxisUp1CM = 0x12A9,
		////Set_XAxisDown1CM = 0x1209,

		////Set_YAxisUpAuto = 0x12A7,
		////Set_YAxisDownAuto = 0x1207,
		////Set_YAxisUp1CM = 0x12A9,
		////Set_YAxisDown1CM = 0x1209,

		//Set_ZAxisUpAuto = 0x12A7,
		//Set_ZAxisDownAuto = 0x1207,
		//Set_ZAxisUp1CM = 0x12A9,
		//Set_ZAxisDown1CM = 0x1209,

		////复位操作
		//Set_XAxisReset = 0x12A1,
		//Set_YAxisReset = 0x12A4,
		//Set_ZAxisReset = 0x12A8,


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

public slots:




	/**
	*  @brief       根据图形数据组成图形数据包
	*  @param[in]
	*  @param[out]
	*  @return
	*/
	//static QByteArray GetSendImgDatagram(FunCode code, QByteArray data = QByteArray());
	static QList<QByteArray> GetSendImgDatagram(quint16 w, quint16 h, quint8 Imgtype, const QByteArray &hexData);



	// 拆分命令字段为高8位和低8位
	static void SplitComm(uint16_t command, uint8_t& highByte, uint8_t& lowByte);

	// 生成服务器回复命令（客户端命令高8位+0x10）
	static uint16_t GetSerResponseComm(FunCode clientCmd);

	// 从服务器命令获取对应的客户端原始命令
	static FunCode GetClientCommFromSer(uint16_t serverCommand);

	// 服务器回复的命令（2字节）与客户端命令的映射关系
	//const std::map<uint16_t, FunCode> Ser2ClientCommMap =
	//{
	//	{0x1000, ClientCommand::Heartbeat},    // 服务器回复0x1000 → 对应客户端0x0000
	//	{0x3000, ClientCommand::StartPrint},   // 服务器回复0x3000 → 对应客户端0x2000
	//	{0x5000, ClientCommand::PausePrint}    // 服务器回复0x5000 → 对应客户端0x4000
	//};


signals:
	//--------------------------解析完数据发送相关信号-----------------------------//
	//心跳信号
	void SigHeartBeat();

	void SigCmdReply(int cmd, uchar errCode, QByteArray arr = QByteArray());

	//数据集信号
	void sigDataInfo(DataFieldInfo1);

	//设备运动参数 SpeedRotatePara
	void sigSpeedRotatePara();

	// 接收报文数据显示错误，进入重发模式
	void SigPackFailRetransport(QByteArray& arr);

	//
	void SigHandleFunOper(int codeType, int command);
	void SigHandleFunOper1(const PackParam& data);
	void SigHandleFunOper2(int cmdType, const MoveAxisPos& data);

	void SigResData();

private:

	int HandleCheckPackageHead(const QByteArray& data, int pos);


	/** 
	*  @brief       处理下位机周期发送过来的数据 
	*  @param[in]    data:数据域地址  lSigCmdReply
	*  @param[out]   
	*  @return                    
	*/
	void HandlePeriodData(uchar* data, ushort length);

	/** 
	*  @brief       处理下位机回复包
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/ 
	void HandleResponseData(ushort code, uchar* data, ushort length, QByteArray arr = QByteArray());


private:

	//接受数据待处理缓存
	QByteArray m_recvBuf;
	//crc校验错误次数
	int m_crcErrorNum = 0;
	//下位机返回错误码次数
	int m_codeErrorNum = 0;
};

/** 
*  @author      
*  @class       DataFieldInfo 
*  @brief       协议数据信息
*/
class DataFieldInfo1
{
public:
	DataFieldInfo1();

	//控制权状态  0x01: 控制权在执行机构  0x02: 控制权在远程操作台（摇杆和触摸屏可控制执行机构)
	uchar _controlStatus = 1;  
	//按键切换状态  0x01表示左， 0x02表示右
	uchar _switchStatus;
	//速度提示音状态  0x00: 无声音  0x01: 低速声音  0x02: 中速声音 0x03：高速声音
	uchar _speedSoudStatus = 0;
	//设备故障码
	short _deviceFaultCode = 0;
	//下位机软件版本号
	QString _softVersion;
	//下位机硬件版本号
	QString _hardwareVersion;
	
};

