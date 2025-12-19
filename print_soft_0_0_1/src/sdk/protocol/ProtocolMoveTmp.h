#pragma once
#include <QtCore/QtCore>

class DataFieldInfo2;
class SerialPortData;
class MoveSpeedPara;

/** 
*  @author      
*  @class       ProtocolMoveTmp
*  @brief       龙门架控制协议
*/
class ProtocolMoveTmp : public QObject
{
	Q_OBJECT

public:
		/**  命令组功能码  **/
	enum MoveDirect
	{
		Move_Up = 0x00,
		Move_Down = 0x01
	};

	enum FunCode
	{
		//上位机发送控制命令,下位机会返回数据包
		Con_Breath = 0x0110,                        //心跳

		//打印机--设置
		Set_XMachineRun = 0x00A3,			//设置打印机启动命令
		Set_XMachineStop = 0x00A4,				//设置打印机停止命令
		Set_XMachineAutoRun = 0x00A5,			//设置打印机复位命令
		Set_XMachineAutoReset = 0x00A6,			//设置打印机运行命令

		Set_YMachineRun = 0x00A7,			//设置打印机启动命令
		Set_YMachineStop = 0x00A8,				//设置打印机停止命令
		Set_YMachineAutoRun = 0x00A9,			//设置打印机复位命令
		Set_YMachineAutoReset = 0x00AA,			//设置打印机运行命令

		Set_ZMachineRun = 0x00AB,			//设置打印机启动命令
		Set_ZMachineStop = 0x00AC,				//设置打印机停止命令
		Set_ZMachineAutoRun = 0x00AD,			//设置打印机复位命令
		Set_ZMachineAutoReset = 0x00AE,			//设置打印机运行命令


		//打印机--读取
		Read_PrintStart = 0x0153,			//读取打印机启动命令
		Read_PrintStop = 0x0154,			//读取打印机停止命令
		Read_PrintReset = 0x0155,			//读取打印机复位命令

		//龙门架--回复
		Reply_XMachineRun = 0x0156,			//读取打印机启动命令
		Reply_XMachineStop = 0x0157,			//读取打印机停止命令
		Reply_XMachineAutoRun = 0x0158,			//读取打印机复位命令
		Reply_XMachineAutoReset = 0x0159,			//读取打印机运行命令

		//龙门架--回复
		Reply_YMachineRun = 0x015A,			//读取打印机启动命令
		Reply_YMachineStop = 0x015B,			//读取打印机停止命令
		Reply_YMachineAutoRun = 0x015C,			//读取打印机复位命令
		Reply_YMachineAutoReset = 0x015D,			//读取打印机运行命令

		//龙门架--回复
		Reply_ZMachineRun = 0x015E,			//读取打印机启动命令
		Reply_ZMachineStop = 0x015F,			//读取打印机停止命令
		Reply_ZMachineAutoRun = 0x0160,			//读取打印机复位命令
		Reply_ZMachineAutoReset = 0x0161,			//读取打印机运行命令


		//--------------------上位机设置参数命令组---------------------------//
		Set_TotalRunPara = 0x0231,                  //设置整机运动参数

		//--------------------上位机读下位机命令，下位机会返回数据包----------------------------//
		Read_Switch = 0x0311,						//读切换按键状态
		Read_HardwareVersion = 0x0312,				//读硬件版本号
		Read_SoftVersion = 0x0313,				//读软件版本号
		Read_Status = 0x0314,						//读状态
		Read_Error = 0x0315,						//读错误状态
		Read_AllPara = 0x0316,						//读所有参数
		Read_AllSetPara = 0x0317,					//读所有设置参数


		//-----------------下位机定时周期推送的数据命令，上位机不需要回复--------------------------//
		Period_AllPara = 0x0430						//周期发送过来的数据命令

	};

	//err_code_enum
	enum ErrorCode
	{
		Command_Err = 0x01,      //命令码错误
		DataField_Err = 0x02,    //数据域错误
		Crc_Err = 0x03,          //校验错误
		Status_Err = 0x04        //状态错误
	};

	ProtocolMoveTmp(QObject* parent = 0);

	/**
	*  @brief       设置串口对象
	*  @param[in]
	*  @param[out]
	*  @return
	*/
	void SetSerialPort(SerialPortData* serialport);

private:
	/** 
	*  @brief       brief 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	void Parse(QByteArray& datagram);

	/**
	*  @brief       处理下位机周期发送过来的数据
	*  @param[in]    data:数据域地址  length:数据域长度
	*  @param[out]
	*  @return
	*/
	//modify
	//void HandlePeriodData(uchar* data, ushort length);

	/**
	*  @brief       处理下位机回复包
	*  @param[in]
	*  @param[out]
	*  @return
	*/
	//modify
	void HandleResponseData(ushort code, uchar* data, ushort length);

	/**
	*  @brief       对接收到的数据进行crc校验
	*  @param[in]
	*  @param[out]
	*  @return
	*/
	static bool CheckCRC(uchar* data, int datalen);

	/**
	*  @brief       创建crc校验码
	*  @param[in]
	*  @param[out]
	*  @return
	*/
	static ushort MakeCRCCheck(uchar* data, int datalen);

public slots:
	/** 
	*  @brief       解析收到的数据包 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	void Decode(QByteArray datagram);

	/** 
	*  @brief       根据参数组成主动请求的包
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	static QByteArray GetSendDatagram(FunCode code, MoveDirect direction, QByteArray data = QByteArray());

	/** 
	*  @brief       根据参数组成回复包 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	static QByteArray GetResponseDatagram(FunCode code, MoveDirect direction, QByteArray data = QByteArray());


signals:
	//--------------------------解析完数据发送相关信号-----------------------------//
	//心跳信号
	void SigHeartBeat();

	//数据集信号
	void SigDataInfo(DataFieldInfo2);

	/** 
	*  @brief       上位机发送命令，下位机返回的执行结果 
	*  @param[in]   cmd: 执行的命令 （FunCode类型) errCode: 下位机返回的错误码 0代表无错误，其他有错误
	*  @param[out]   
	*  @return                    
	*/
	void SigCmdReply(int cmd, uchar errCode);

	//设备运动参数
	void SigSpeedRotatePara(MoveSpeedPara);



private:
	/**  crc高字节  **/
	static const uchar gabyCRCHi[256];
	/**  crc低字节  **/
	static const uchar gabyCRCLo[256];
	//接受数据待处理缓存
	QByteArray m_recvBuf;
	//crc校验错误次数
	int m_crcErrorNum = 0;
	//下位机返回错误码次数
	int m_codeErrorNum = 0;

};

/** 
*  @author      
*  @class       DataFieldInfo2
*  @brief       协议数据信息
*/
class DataFieldInfo2
{
public:
	DataFieldInfo2();

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

	//导丝小角度旋转状态   0x01:启动  0x02:停止
	uchar _guideWireSmallAngleRotateStatus = 0;

	//导丝大角度旋转状态   0x01:启动  0x02:停止
	uchar _guideWireBigAngleRotateStatus = 0;

	//器械高频往复运动     0x01:启动  0x02:停止
	uchar _deviceHighFreqRecipMotionStatus = 0;

	//导管运动导丝自动补偿状态  0x01:启动 0x02:停止
	uchar _autoRecoup = 0;

	//器械摇杆使能状态  0x01:锁定状态  0x02:解锁状态
	uchar _deviceRockerEnable = 0;

	//导丝摇杆使能状态 0x01:锁定状态  0x02:解锁状态
	uchar _guideWireRockerEnable = 0;

	//导管摇杆使能状态 0x01:锁定状态  0x02:解锁状态
	uchar _guideCatheterRockerEnable = 0;

	//器械前进后退速度 ,为负数表示后退
	short _deviceMoveSpeed = 0;

	//导丝前进后退速度，为负数表示后退
	short _guideWireMoveSpeed = 0;

	//导管前进后退速度，为负数表示后退
	short _guideCatheterMoveSpeed = 0;

	//导丝旋转速度，为正数表示顺时针，为负数表示逆时针
	short _guideWireRotSpeed = 0;

	//导管旋转速度，为正数表示顺时针，为负数表示逆时针
	short _guideCatheterRotSpeed = 0;

	//左器械位置
	int _devicePos = 0;

	//左导丝位置
	int _guideWirePos = 0;

	//左导丝旋转角度
	int _guideWireRotPos = 0;

	//导管位置
	int _guideCatheterPos = 0;

	//导管旋转角度位置
	int _guideCatheterRotPos = 0;

	//右器械前进速度
	short _rdeviceMoveSpeed = 0;

	//右导丝前进速度
	short _rguideWireMoveSpeed = 0;

	//右导丝旋转速度
	short _rguideWireRotSpeed = 0;

	//右器械位置
	int _rdevicePos = 0;

	//右导丝位置
	int _rguideWirePos = 0;

	//右导丝旋转角度位置
	int _rguideWireRotPos = 0;

	//左导丝前进后退电流
	short _guideWireMoveCurrent = 0;

	//右导丝前进后退电流
	short _rguideWireMoveCurrent = 0;

	//行程回原点状态  0x00 未回原点  0x01 已回原点，如果回了原点，直接取位置作为行程预警值判断 如果未回原点，使用上次为1时候保存的数字加上当前位置作为行程预警值判断
	uchar _seekhome_state = 1;

	//档位状态 每个位表示一个档位，位0表示无档位，位为1表示有档位(bit0到bit5分别表示器械运行、导丝运行、导丝旋转、导管运行、导管旋转、其他位保留
	uchar _gear_state = 0;

	//导丝ROR旋转功能状态   0x01:启动  0x02:停止
	uchar _rorStatus;

	//导管预警状态 0x00=无预警，0x01=预警 0x02=正极限预警  0x82=负极线预警
	uchar _catheterWarnStatus = 0;

	//轨道2启用状态  0:禁用 0x01:启用状态   0x02:禁用状态
	uchar _track2Status = 0;

	//导管旋转警示状态 0无警示  1 警示
	uchar _catheterRotWarnStatus = 0;
};

/** 
*  @author      
*  @class       SpeedRotatePara
*  @brief       运动控制设置参数
*/
class MoveSpeedPara
{
public:
	ushort m_deviceSpeed1 = 0;                              //器械操作杆推送一档速度
	ushort m_deviceSpeed2 = 0;                             //器械操作杆推送二挡速度 
	ushort m_guideWireSpeed1 = 0;                           //导丝操作杆推送一档速度
	ushort m_guideWireSpeed2 = 0;                          //导丝操作杆推送二挡速度
	ushort m_guideWireRotateSpeed = 0;                    //导丝操作杆旋转速度    
	ushort m_catheterSpeed1 = 0;							//导管操作杆推送一档速度 
	ushort m_catheterSpeed2 = 0;                           //导管操作杆推送二挡速度 
	ushort m_catheterRotateSpeed = 0;                     //导管操作杆旋转速度     
	ushort m_smallGuideWireAngle = 0;                      //小搓丝旋转角度      
	ushort m_bigGuideWireAngle = 0;                       //大搓丝旋转角度       
	ushort m_rorAngle = 0;                                 //ROR旋转角度          
	ushort m_smallGuideWireSpeed = 0;                     //小搓丝旋转速度 
	ushort m_bigGuideWireSpeed = 0;                       //大搓丝旋转速度 
};