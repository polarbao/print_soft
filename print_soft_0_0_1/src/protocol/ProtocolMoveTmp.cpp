#include "ProtocolMoveTmp.h"
#include "SerialPortData.h"
#include "CLogManager.h"

//获取short类型的高字节
#define HI_OF_SHORT(X) (X >> 8)
//获取short类型的低字节
#define LO_OF_SHORT(X) (X & 0xFF)
//包尾，1字节
#define Package_Tail 0x0D
//协议数据包最小固定长度
#define DATAGRAM_MIN_SIZE 4

#if SerialVer1_0

//主动发送包头，2字节
#define Active_Head 0xAABB
//应答包头，2字节
#define Response_Head 0xAACC
//包尾，2字节
#define Package_Tail 0xDDEE


//解析数据包：格式如下：包头（2byte) + 长度（2byte） + 命令（2byte) + 地址（1byte) + 数据区（动态，从0到n） + crc16校验(2byte) + 包尾(2byte)
void ProtocolMoveTmp::Decode(QByteArray recvdata)
{
	QString str = recvdata.toHex(' ');
	LOG_DEBUG(u8"接收数据: " + str);

	//新数据加入缓冲区
	m_recvBuf.append(recvdata);
	if (m_recvBuf.size() < 11)
	{
		return;
	}



	//--------解析缓冲区，找到每一条数据包尾DD EE的位置
	int preIndex = -1;    
	QList<QByteArray> packageDatas;
	for (int i = 1; i < m_recvBuf.size(); i++)
	{
		uchar hi = (uchar)m_recvBuf.at(i - 1);
		uchar lo = (uchar)m_recvBuf.at(i);

		if (hi == HI_OF_SHORT(Package_Tail) && lo == LO_OF_SHORT(Package_Tail))
		{
			packageDatas.push_back(m_recvBuf.mid(preIndex + 1,i - preIndex));
			preIndex = i;
		}
	}

	//保留右侧不是一个完整包的数据
	m_recvBuf = m_recvBuf.right(m_recvBuf.size() - preIndex - 1);

	//解析完整包
	for (auto datagram : packageDatas)
	{
		Parse(datagram);
	}

}

QByteArray ProtocolMoveTmp::GetSendDatagram(FunCode code, QByteArray data)
{
	const int size = 100;
	uchar sendBuf[size];
	memset(sendBuf, 0, size);

	//包头
	sendBuf[0] = HI_OF_SHORT(Active_Head);
	sendBuf[1] = LO_OF_SHORT(Active_Head);

	ushort length = 7 + data.size();

	//长度字
	sendBuf[2] = HI_OF_SHORT(length);
	sendBuf[3] = LO_OF_SHORT(length);

	//命令字
	sendBuf[4] = HI_OF_SHORT(code);
	sendBuf[5] = LO_OF_SHORT(code);

	//地址，默认为0
	sendBuf[6] = 0x00;
	
	//数据内容
	if (data.size() > 0)
	{
		memcpy(&sendBuf[7], data.data(), data.size());
	}

	//校验
	ushort crc = MakeCRCCheck(sendBuf, length);
	sendBuf[length] = HI_OF_SHORT(crc);
	sendBuf[length + 1] = LO_OF_SHORT(crc);

	//包尾
	sendBuf[length + 2] = HI_OF_SHORT(Package_Tail);
	sendBuf[length + 3] = LO_OF_SHORT(Package_Tail);

	QByteArray senddata;
	senddata.resize(length + 4);

	memcpy(senddata.data(), sendBuf, length + 4);

	return senddata;
}

QByteArray ProtocolMoveTmp::GetResponseDatagram(FunCode code, MoveDirect direction, QByteArray data /*= QByteArray()*/)
{
	const int size = 100;
	uchar sendBuf[size];
	memset(sendBuf, 0, size);

	//包头
	sendBuf[0] = HI_OF_SHORT(Response_Head);
	sendBuf[1] = LO_OF_SHORT(Response_Head);

	ushort length = 7 + data.size();

	//长度字
	sendBuf[2] = HI_OF_SHORT(length);
	sendBuf[3] = LO_OF_SHORT(length);

	//命令字
	sendBuf[4] = HI_OF_SHORT(code);
	sendBuf[5] = LO_OF_SHORT(code);

	//地址，默认为0
	sendBuf[6] = 0x00;

	//数据内容
	if (data.size() > 0)
	{
		memcpy(&sendBuf[7], data.data(), data.size());
	}

	//包头
	ushort crc = MakeCRCCheck(sendBuf, length);
	sendBuf[length] = HI_OF_SHORT(crc);
	sendBuf[length + 1] = LO_OF_SHORT(crc);

	//包尾
	sendBuf[length + 2] = HI_OF_SHORT(Package_Tail);
	sendBuf[length + 3] = LO_OF_SHORT(Package_Tail);

	QByteArray senddata;
	senddata.resize(length + 4);

	memcpy(senddata.data(), sendBuf, length + 4);

	return senddata;
}

void ProtocolMoveTmp::Parse(QByteArray& datagram)
{
	//接收到数据长度
	int recvLength = datagram.length();
	//数据包固定字节长度为11字节，变化长度为数据区，从0到n
	if (recvLength < DATAGRAM_MIN_SIZE)
	{
		LOG_DEBUG(u8"数据长度错误");
		return;
	}

	//数据转变成uchar类型的缓存
	const int size = 512;
	uchar recvBuf[size];
	memset(recvBuf, 0, size);
	int templen = datagram.size() > size ? size : datagram.size();
	memcpy(&recvBuf[0], datagram, templen);

	//比较包头
	if (!(recvBuf[0] == HI_OF_SHORT(Active_Head) && (recvBuf[1] == LO_OF_SHORT(Active_Head) || recvBuf[1] == LO_OF_SHORT(Response_Head))))
	{
		LOG_DEBUG(u8"数据包头错误。");
		return;
	}
	//比较包尾
	if (recvBuf[recvLength - 2] != HI_OF_SHORT(Package_Tail) || recvBuf[recvLength - 1] != LO_OF_SHORT(Package_Tail))
	{
		LOG_DEBUG(u8"数据包尾错误。");
		return;
	}
	//判断crc校验
	if (!CheckCRC(recvBuf, recvLength - 2))
	{
		LOG_DEBUG(u8"crc校验错误");
		LOG_DEBUG(QString(u8"crc校验错误次数统计：%1").arg(++m_crcErrorNum));
		return;
	}

	//-------------正确的包，开始解析-------------------//

	//长度字段判断
	ushort lenByte;
	lenByte = recvBuf[2] << 8;
	lenByte += recvBuf[3];
	if (lenByte != recvLength - 4)
	{
		LOG_DEBUG(u8"长度字段错误");
		return;
	}

	//命令码
	ushort code;
	code = recvBuf[4] << 8;
	code += recvBuf[5];

	//地址字段，默认为0x00
	uchar address;
	address = recvBuf[6];

	//如果是下位机回应上位机的包
	if (recvBuf[1] == LO_OF_SHORT(Response_Head))
	{
		HandleResponseData(code, &recvBuf[7], recvLength - DATAGRAM_MIN_SIZE);
		return;
	}
	//如果是下位机主动发送到上位机的包
	else
	{
		//下位机周期推送的数据包
		if (code == Period_AllPara)
		{
			HandlePeriodData(&recvBuf[7], recvLength - DATAGRAM_MIN_SIZE);
		}
	}
}


void ProtocolMoveTmp::HandleResponseData(ushort code, uchar* data, ushort length)
{
	//如果是心跳包
	if (code == Con_Breath)
	{
		emit SigHeartBeat();
		return;
	}
	//错误发生，返回错误编码，原命令字+0x8000
	else if (code >= 0x8000)
	{
		uchar errcode = data[0];

		LOG_DEBUG(u8"下位机返回错误码,错误为: " + GetErrString(errcode));
		LOG_DEBUG(QString(u8"下位机返回错误码次数统计：%1").arg(++m_codeErrorNum));

		emit SigCmdReply(code - 0x8000, errcode);
		return;
	}
	//如果是读取数据请求返回的应答数据包
	else if (code >= Read_Switch && code < Period_AllPara)
	{
		//解析数据域
		if (length < 1)
		{
			return;
		}
		switch (code)
		{
		case Read_HardwareVersion:
		{
			QByteArray version((const char*)data, length);
			g_hardwareVersion = version;
		}
		break;
		case Read_SoftVersion:
		{
			QByteArray version((const char*)data, length);
			g_lowPcVersion = version;
		}
		break;
		case Read_AllPara:
			HandlePeriodData(data, length);
			break;
		case Read_AllSetPara:    //读取参数设置参数
		{
			if (length == 26)
			{
				SpeedRotatePara para;
				int index = 0;
				para.m_deviceSpeed1 = data[index++] << 8;
				para.m_deviceSpeed1 += data[index++];
				para.m_deviceSpeed2 = data[index++] << 8;
				para.m_deviceSpeed2 += data[index++];
				para.m_guideWireSpeed1 = data[index++] << 8;
				para.m_guideWireSpeed1 += data[index++];
				para.m_guideWireSpeed2 = data[index++] << 8;
				para.m_guideWireSpeed2 += data[index++];
				para.m_guideWireRotateSpeed = data[index++] << 8;
				para.m_guideWireRotateSpeed += data[index++];
				para.m_catheterSpeed1 = data[index++] << 8;
				para.m_catheterSpeed1 += data[index++];
				para.m_catheterSpeed2 = data[index++] << 8;
				para.m_catheterSpeed2 += data[index++];
				para.m_catheterRotateSpeed = data[index++] << 8;
				para.m_catheterRotateSpeed += data[index++];
				para.m_smallGuideWireAngle = data[index++] << 8;
				para.m_smallGuideWireAngle += data[index++];
				para.m_bigGuideWireAngle = data[index++] << 8;
				para.m_bigGuideWireAngle += data[index++];
				para.m_rorAngle = data[index++] << 8;
				para.m_rorAngle += data[index++];
				para.m_smallGuideWireSpeed = data[index++] << 8;
				para.m_smallGuideWireSpeed += data[index++];
				para.m_bigGuideWireSpeed = data[index++] << 8;
				para.m_bigGuideWireSpeed += data[index++];

				emit SigSpeedRotatePara(para);
			}
		}
		break;
		}
	}

	emit SigCmdReply(code, 0);
}

#endif
//----------------------------------Ver 1.0---------------------------------------------------
//----------------------------------Ver 1.0---------------------------------------------------
//----------------------------------Ver 1.0---------------------------------------------------
//----------------------------------Ver 1.0---------------------------------------------------

QString g_hardwareVersion;
QString g_lowPcVersion;


//通用函数
static QString GetErrString(uchar code)
{
	switch (code)
	{
	case ProtocolMoveTmp::Command_Err:
		return QString(u8"命令码错误");
	case ProtocolMoveTmp::DataField_Err:
		return QString(u8"数据域错误");
	case ProtocolMoveTmp::Crc_Err:
		return QString(u8"校验错误");
	case ProtocolMoveTmp::Status_Err:
		return QString(u8"状态错误");
	default:
		return QString(u8"未知错误");
	}
}


short GetPos(uchar* data)
{
	uchar direct;
	short pos;
	direct = data[0];
	pos = data[1] << 8;
	pos += data[2];
	if (direct == 0x02)
	{
		pos = -pos;
	}
	return pos;
}

short GetSpeed(uchar* data)
{
	uchar direct;
	short speed;
	direct = data[0];
	speed = data[1] << 8;
	speed += data[2];
	if (direct == 0x02)
	{
		speed = -speed;
	}
	return speed;
}

ProtocolMoveTmp::ProtocolMoveTmp(QObject* parent /*= 0*/)
	:QObject(parent)
{
	//qRegisterMetaType<DataFieldInfo>("DataFieldInfo");
	qRegisterMetaType<MoveSpeedPara>("MoveSpeedPara");
}

void ProtocolMoveTmp::SetSerialPort(SerialPortData* serialport)
{
	connect(serialport, &SerialPortData::sigNewData, this, &ProtocolMoveTmp::Decode, Qt::DirectConnection);
	//connect(serialport, SIGNAL(sigNewData(QByteArray)), this, SLOT(Decode(QByteArray)), Qt::DirectConnection);
}

//解析数据包：格式如下：包头（2byte) + 长度（2byte） + 命令（2byte) + 地址（1byte) + 数据区（动态，从0到n） + crc16校验(2byte) + 包尾(2byte)
void ProtocolMoveTmp::Decode(QByteArray recvdata)
{
	QString str = recvdata.toHex(' ');
	LOG_DEBUG(u8"接收数据: " + str);

	//新数据加入缓冲区
	m_recvBuf.append(recvdata);
	if (m_recvBuf.size() < 3)
	{
		return;
	}

	//--------解析缓冲区，找到每一条数据包尾0D的位置
	int preIndex = -1;
	QList<QByteArray> packageDatas;
	for (int i = 0; i < m_recvBuf.size(); i++)
	{
		uchar lo = (uchar)m_recvBuf.at(i);

		if (lo == LO_OF_SHORT(Package_Tail))
		{
			packageDatas.push_back(m_recvBuf.mid(preIndex + 1, i - preIndex));
			preIndex = i;
		}
	}

	//保留右侧不是一个完整包的数据
	m_recvBuf = m_recvBuf.right(m_recvBuf.size() - preIndex - 1);

	//解析完整包
	for (auto datagram : packageDatas)
	{
		Parse(datagram);
	}
}

void ProtocolMoveTmp::Parse(QByteArray& datagram)
{
	//接收到数据长度
	int recvLength = datagram.length();
	//数据包固定字节长度为11字节，变化长度为数据区，从0到n
	if (recvLength < DATAGRAM_MIN_SIZE)
	{
		LOG_DEBUG(u8"数据长度错误");
		return;
	}

	//数据转变成uchar类型的缓存
	const int size = 512;
	uchar recvBuf[size];
	memset(recvBuf, 0, size);
	int templen = datagram.size() > size ? size : datagram.size();
	memcpy(&recvBuf[0], datagram, templen);

	//比较包尾
	if (recvBuf[recvLength - 1] != LO_OF_SHORT(Package_Tail))
	{
		LOG_DEBUG(u8"数据包尾错误。");
		return;
	}

	//-------------正确的包，开始解析-------------------//

	//命令码
	ushort code;
	code = recvBuf[0] << 8;
	code += recvBuf[1];

	//方向，默认为0x00
	uchar address;
	address = recvBuf[2];

	//如果是下位机回应上位机的包
	//请求回复
	HandleResponseData(code, &recvBuf[3], recvLength - DATAGRAM_MIN_SIZE);
}

void ProtocolMoveTmp::HandleResponseData(ushort code, uchar* data, ushort length)
{
	//心跳包
	if (code == Con_Breath)
	{
		emit SigHeartBeat();
		return;
	}
	//错误发生，返回错误编码，原命令字+0x8000
	else if (code >= 0x8000)
	{
		uchar errcode = data[0];

		LOG_DEBUG(u8"下位机返回错误码,错误为: " + GetErrString(errcode));
		LOG_DEBUG(QString(u8"下位机返回错误码次数统计：%1").arg(++m_codeErrorNum));

		emit SigCmdReply(code - 0x8000, errcode);
		return;
	}
	emit SigCmdReply(code, 0);
}

//void ProtocolMoveTmp::HandlePeriodData(uchar* data, ushort length)
//{
//	//周期数据个数根据不同协议版本不一样
//	if (length < 65)
//	{
//		return;
//	}
//
//	int index = 0;
//	DataFieldInfo2 info;
//
//	info._switchStatus = data[index++];
//	info._deviceRockerEnable = data[index++];
//	info._guideWireRockerEnable = data[index++];
//	info._guideCatheterRockerEnable = data[index++];
//
//	//读取前进后退速度，0x01表示前进或顺时针旋转 0x02表示后退或逆时针
//	uchar direct;
//	short tempSpeed[5];
//	for (int i = 0; i < 5; i++)
//	{
//		direct = data[index++];
//		tempSpeed[i] = data[index++] << 8;
//		tempSpeed[i] += data[index++];
//		if (direct == 0x02)
//		{
//			tempSpeed[i] = -tempSpeed[i];
//		}
//	}
//
//	//位置
//	int tempPos[5];
//	for (int i = 0; i < 5; i++)
//	{
//		direct = data[index++];
//		tempPos[i] = data[index++] << 8;
//		tempPos[i] += data[index++];
//		if (direct == 0x02)
//		{
//			tempPos[i] = -tempPos[i];
//		}
//	}
//
//	//右器械导丝速度位置
//	int tempRightVal[6];
//	for (int i = 0; i < 6; i++)
//	{
//		direct = data[index++];
//		tempRightVal[i] = data[index++] << 8;
//		tempRightVal[i] += data[index++];
//		if (direct == 0x02)
//		{
//			tempRightVal[i] = -tempRightVal[i];
//		}
//	}
//
//	info._deviceMoveSpeed = tempSpeed[0];
//	info._guideWireMoveSpeed = tempSpeed[1];
//	info._guideCatheterMoveSpeed = tempSpeed[2];
//	info._guideWireRotSpeed = tempSpeed[3];
//	info._guideCatheterRotSpeed = tempSpeed[4];
//	info._devicePos = tempPos[0];
//	info._guideWirePos = tempPos[1];
//	info._guideWireRotPos = tempPos[2];
//	info._guideCatheterPos = tempPos[3];
//	info._guideCatheterRotPos = tempPos[4];
//
//	info._rdeviceMoveSpeed = tempRightVal[0];
//	info._rguideWireMoveSpeed = tempRightVal[1];
//	info._rguideWireRotSpeed = tempRightVal[2];
//	info._rdevicePos = tempRightVal[3];
//	info._rguideWirePos = tempRightVal[4];
//	info._rguideWireRotPos = tempRightVal[5];
//
//	//导丝小角度旋转功能状态
//	info._guideWireSmallAngleRotateStatus = data[index++];
//	//导丝大角度旋转功能状态
//	info._guideWireBigAngleRotateStatus = data[index++];
//	//器械高频往复运动状态
//	info._deviceHighFreqRecipMotionStatus = data[index++];
//	//导管运动导丝自动补偿状态 
//	info._autoRecoup = data[index++];
//	//控制权状态
//	info._controlStatus = data[index++];
//	//速度提示音状态
//	info._speedSoudStatus = data[index++];
//	//故障码
//	info._deviceFaultCode = data[index++] << 8;
//	info._deviceFaultCode += data[index++];
//	//左导丝前进后退电流
//	info._guideWireMoveCurrent = data[index++] << 8;
//	info._guideWireMoveCurrent += data[index++];
//	//右导丝前进后退电流
//	info._rguideWireMoveCurrent = data[index++] << 8;
//	info._rguideWireMoveCurrent += data[index++];
//	//器械行程是否回原点
//	info._seekhome_state = data[index++];
//
//	if (length >= 67)
//	{
//		//档位状态
//		info._gear_state = data[index++];
//		//ROR旋转功能状态
//		info._rorStatus = data[index++];
//	}
//	if (length >= 68)
//	{
//		//导管行程预警区间状态
//		info._catheterWarnStatus = data[index++];
//	}
//	if (length >= 69)
//	{
//		info._track2Status = data[index++];
//	}
//	if (length >= 70)
//	{
//		info._catheterRotWarnStatus = data[index++];
//	}
//
//	//发送信号
//	emit SigDataInfo(info);
//}

QByteArray ProtocolMoveTmp::GetSendDatagram(FunCode code, MoveDirect direction, QByteArray data)
{
	const int size = 100;
	uchar sendBuf[size];
	memset(sendBuf, 0, size);
	ushort length = 3 + data.size();

	//命令字
	sendBuf[0] = HI_OF_SHORT(code);
	sendBuf[1] = LO_OF_SHORT(code);

	//方向地址，默认为0
	sendBuf[2] = LO_OF_SHORT(direction);

	//数据内容
	if (data.size() > 0)
	{
		memcpy(&sendBuf[3], data.data(), data.size());
	}

	//包尾
	sendBuf[length] = LO_OF_SHORT(Package_Tail);

	QByteArray senddata;
	senddata.resize(length + 1);
	memcpy(senddata.data(), sendBuf, length + 1);
	return senddata;
}

QByteArray ProtocolMoveTmp::GetResponseDatagram(FunCode code, MoveDirect direction, QByteArray data /*= QByteArray()*/)
{
	const int size = 100;
	uchar sendBuf[size];
	memset(sendBuf, 0, size);
	ushort length = 3 + data.size();

	//命令字
	sendBuf[0] = HI_OF_SHORT(code);
	sendBuf[1] = LO_OF_SHORT(code);

	//方向地址，默认为0
	sendBuf[2] = LO_OF_SHORT(direction);

	//数据内容
	if (data.size() > 0)
	{
		memcpy(&sendBuf[3], data.data(), data.size());
	}

	//包尾
	sendBuf[length] = LO_OF_SHORT(Package_Tail);

	QByteArray senddata;
	senddata.resize(length + 1);
	memcpy(senddata.data(), sendBuf, length + 1);
	return senddata;
}



bool ProtocolMoveTmp::CheckCRC(uchar* data, int datalen)
{
	ushort crc = MakeCRCCheck(data, datalen - 2);
	uchar lo = LO_OF_SHORT(crc);
	uchar hi = HI_OF_SHORT(crc);
	//包中数据crc也是先高字节，再低字节
	if (hi == data[datalen - 2] && lo == data[datalen - 1])
	{
		return true;
	}
	else
	{
		return false;
	}
}

ushort ProtocolMoveTmp::MakeCRCCheck(uchar* data, int datalen)
{
	unsigned char byCRCHi = 0xff;
	unsigned char byCRCLo = 0xff;
	unsigned char byIdx;
	unsigned short crc;
	while (datalen--)
	{
		byIdx = byCRCLo ^ *data++;
		byCRCLo = byCRCHi ^ gabyCRCHi[byIdx];
		byCRCHi = gabyCRCLo[byIdx];
	}
	crc = byCRCHi;
	crc <<= 8;
	crc += byCRCLo;
	return crc;
}


const uchar ProtocolMoveTmp::gabyCRCHi[256] = {
	 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40
};

const uchar ProtocolMoveTmp::gabyCRCLo[256] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
};

DataFieldInfo2::DataFieldInfo2()
{

}
