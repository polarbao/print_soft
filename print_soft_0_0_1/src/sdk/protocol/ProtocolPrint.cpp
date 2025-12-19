#include "ProtocolPrint.h"
//#include "globalHeader.h"
//#include "SerialPortData.h"
#include "CLogManager.h"
#include <QDataStream>
#include <QtEndian>
#include "utils.h"

//启动CRC检测
//#define TurnOnCRC 

//获取short类型的高字节
#define HI_OF_SHORT(X) (X >> 8)
//获取short类型的低字节
#define LO_OF_SHORT(X) (X & 0xFF)
//包头，2字节
#define Req_Package_Head 0xAABB
#define Resp_Package_Head_Succ 0xAACC
#define Resp_Package_Head_Err 0xAADD

//取消--包尾，2字节
#define Package_Tail 0xDDEE

#define SER_HIGH_OFFSET 0x10

//协议数据包最小固定长度
#define DATAGRAM_MIN_SIZE 10


ProtocolPrint::ProtocolPrint(QObject* parent /*= 0*/)
	:QObject(parent)
{
	qRegisterMetaType<DataFieldInfo1>("DataFieldInfo1");
}



void ProtocolPrint::HandleRecvDatagramData(QByteArray recvdata)
{
	QString str = recvdata.toHex(' ');
	LOG_INFO(QString(u8"接收数据: %1").arg(str));

	//新数据加入缓冲区
	m_recvBuf.append(recvdata);

	if (m_recvBuf.size() < DATAGRAM_MIN_SIZE)
	{
		return;
	}

	//--------解析缓冲区，找到每一条数据包尾DD EE的位置
	int preIndex = -1;    
	//ori
	QList<QByteArray> packageDatas;
	for (int i = 1; i < m_recvBuf.size(); i++)
	{
		uchar hi = (uchar)m_recvBuf.at(i - 1);
		uchar lo = (uchar)m_recvBuf.at(i);

		if (hi == HI_OF_SHORT(Package_Tail) && lo == LO_OF_SHORT(Package_Tail))
		{
			packageDatas.push_back(m_recvBuf.mid(preIndex + 1, i - preIndex));
			preIndex = i;
		}
	}

	// new
	// 存储所有包头的起始索引（包头占2字节，索引为第一个字节位置）
	int friIndex = -2;
	QList<QByteArray> packageDatas;
	for (int i = 2; i < m_recvBuf.size(); i++)
	{
		uchar hi = (uchar)m_recvBuf.at(i);
		uchar lo = (uchar)m_recvBuf.at(i + 1);

		if (hi == HI_OF_SHORT(Req_Package_Head) && lo == LO_OF_SHORT(Req_Package_Head) ||
			hi == HI_OF_SHORT(Resp_Package_Head_Succ) && lo == LO_OF_SHORT(Resp_Package_Head_Succ) ||
			hi == HI_OF_SHORT(Resp_Package_Head_Err) && lo == LO_OF_SHORT(Resp_Package_Head_Err))
		{
			packageDatas.push_back(m_recvBuf.mid(friIndex - 2, i - preIndex));
			preIndex = i;
		}
	}

	// new
	// 存储所有包头的起始索引（包头占2字节，索引为第一个字节位置）
	QList<int> headIndexes;
	for (int i = 0; i < m_recvBuf.size()-1; i++)
	{
		uchar hi = (uchar)m_recvBuf.at(i);
		uchar lo = (uchar)m_recvBuf.at(i+1);

		if (hi == HI_OF_SHORT(Req_Package_Head) && lo == LO_OF_SHORT(Req_Package_Head) || 
			hi == HI_OF_SHORT(Resp_Package_Head_Succ) && lo == LO_OF_SHORT(Resp_Package_Head_Succ) ||
			hi == HI_OF_SHORT(Resp_Package_Head_Err) && lo == LO_OF_SHORT(Resp_Package_Head_Err))
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
		ParsePackageData(datagram);
	}
}

int ProtocolPrint::HandleCheckPackageHead(const QByteArray& data, int pos)
{
	int msgType = Req_Package_Head;
	if (pos < 0 || pos >= data.size() - 1)
	{
		return 0x0000;
	}

	uchar hi = (uchar)data.at(pos);
	uchar lo = (uchar)data.at(pos + 1);
	quint16 headValue = (hi << 8) | lo; // 拼接成16位包头值

	if (headValue == Req_Package_Head) return Req_Package_Head;
	if (headValue == Resp_Package_Head_Succ) return Resp_Package_Head_Succ;
	if (headValue == Resp_Package_Head_Err) return Resp_Package_Head_Err;
	return 0x0000;
}


void ProtocolPrint::ParsePackageData(QByteArray& datagram)
{
	//接收到数据长度
	int recvLength = datagram.length();
	//数据包固定字节长度为10字节，变化长度为数据区，从0到n
	if (recvLength < DATAGRAM_MIN_SIZE)
	{
		LOG_INFO(u8"数据长度错误");
		return;
	}

	//数据转变成uchar类型的缓存
	const int size = 512;
	uchar recvBuf[size];
	memset(recvBuf, 0, size);
	int templen = datagram.size() > size ? size : datagram.size();
	memcpy(&recvBuf[0], datagram, templen);

	//比较包头
	if (!(recvBuf[0] == HI_OF_SHORT(Req_Package_Head) && (recvBuf[1] == LO_OF_SHORT(Req_Package_Head))))
	{
		LOG_INFO(u8"数据包头错误。");
		return;
	}
	//比较包尾
	if (recvBuf[recvLength - 2] != HI_OF_SHORT(Package_Tail) || recvBuf[recvLength - 1] != LO_OF_SHORT(Package_Tail))
	{
		LOG_INFO(u8"数据包尾错误。");
		return;
	}
	//判断crc校验
	if (!Utils::GetInstance().CheckCRC(recvBuf, recvLength - 2))
	{
		LOG_INFO(u8"crc校验错误");
		LOG_INFO(QString(u8"crc校验错误次数统计：%1").arg(++m_crcErrorNum));
#ifdef TurnOnCRC
		return;
#endif 
	}

	//-------------正确的包，开始解析-------------------//
	//长度字段判断
	ushort lenByte;
	lenByte = recvBuf[2] << 8;
	lenByte += recvBuf[3];
	if (lenByte != recvLength - 4)
	{
		LOG_INFO(u8"长度字段错误");
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
	if (recvBuf[1] == LO_OF_SHORT(Req_Package_Head))
	{
		//Note:解析回复数据
		//回复的命令最高位-0x1000
		code = (recvBuf[4] - SER_HIGH_OFFSET) << 8;
		code += recvBuf[5];
		HandleResponseData(code, &recvBuf[7], recvLength - DATAGRAM_MIN_SIZE, datagram);
		//
		//auto a = GetResponseCommData(datagram);
		//return;
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


QByteArray ProtocolPrint::GetSendDatagram(FunCode code, QByteArray data)
{
	const int size = 1024;
	uchar sendBuf[size];
	memset(sendBuf, 0, size);

	//包头+命令类型+命令+数据区长度+CRC

	//包头
	sendBuf[0] = LO_OF_SHORT(Req_Package_Head);
	sendBuf[1] = HI_OF_SHORT(Req_Package_Head);

	// 命令类型
	ECmdType cmd = ECmdType::SetParamCmd;
	sendBuf[2] = HI_OF_SHORT(cmd);
	sendBuf[3] = LO_OF_SHORT(cmd);

	//命令字
	sendBuf[4] = HI_OF_SHORT(code);
	sendBuf[5] = LO_OF_SHORT(code);

	//长度字
	// 数据区长度
	ushort length = data.size();
	sendBuf[6] = LO_OF_SHORT(length);
	sendBuf[7] = HI_OF_SHORT(length);
	
	//数据内容
	if (data.size() > 0)
	{
		memcpy(&sendBuf[8], data.data(), data.size());
	}

	//校验
	ushort crc = Utils::GetInstance().MakeCRCCheck(sendBuf, length+8);
	sendBuf[length] = HI_OF_SHORT(crc);
	sendBuf[length + 1] = LO_OF_SHORT(crc);

	//包尾
	//sendBuf[length + 2] = HI_OF_SHORT(Package_Tail);
	//sendBuf[length + 3] = LO_OF_SHORT(Package_Tail);

	QByteArray senddata;
	senddata.resize(length + 8 + 2);
	memcpy(senddata.data(), sendBuf, length + 8 + 2);

	return senddata;
}

QList<QByteArray> ProtocolPrint::GetSendImgDatagram(quint16 w, quint16 h, quint8 Imgtype, const QByteArray &hexData)
{
	QList<QByteArray> packets;
	//// 计算图像数据总长度
	//auto dataLength = hexData.size();
	//// 协议头长度: 7字节
	//const int headerSize = 7;
	//// 最大数据包大小
	//const int maxPacketSize = 1013;
	//// 每个数据包可携带的数据大小
	//const int maxDataPerPacket = maxPacketSize - headerSize;
	//// 计算需要的数据包数量
	//int packetCount = (dataLength + maxDataPerPacket - 1) / maxDataPerPacket;
	//for (int i = 0; i < packetCount; ++i) 
	//{	
	//	int startIndex = i * maxDataPerPacket;
	//	int endIndex = qMin((i + 1) * maxDataPerPacket, dataLength);
	//	int curDataLen = i == packetCount - 1 ? endIndex - startIndex : maxDataPerPacket;
	//	uchar sendBuf[maxPacketSize];
	//	memset(sendBuf, 0, maxPacketSize);
	//	//w h
	//	sendBuf[0] = HI_OF_SHORT(w);
	//	sendBuf[1] = LO_OF_SHORT(w);
	//	sendBuf[2] = HI_OF_SHORT(h);
	//	sendBuf[3] = LO_OF_SHORT(h);
	//	//imgType
	//	sendBuf[4] = LO_OF_SHORT(Imgtype);
	//	//数据内容
	//	QByteArray dataSegment = hexData.mid(startIndex, endIndex - startIndex);
	//	sendBuf[5] = HI_OF_SHORT(curDataLen);
	//	sendBuf[6] = LO_OF_SHORT(curDataLen);
	//	memcpy(&sendBuf[7], dataSegment, dataSegment.size());
	//	QByteArray senddata;
	//	senddata.resize(maxPacketSize);
	//	memcpy(senddata.data(), sendBuf, maxPacketSize);
	//	packets.append(senddata);
	//}
	return packets;
}

QByteArray ProtocolPrint::GetRespDatagram(FunCode code, QByteArray data /*= QByteArray()*/)
{
	const int size = 100;
	uchar sendBuf[size];
	memset(sendBuf, 0, size);

	//包头
	sendBuf[0] = LO_OF_SHORT(Resp_Package_Head_Succ);
	sendBuf[1] = HI_OF_SHORT(Resp_Package_Head_Succ);

	//命令类型
	ECmdType cmd = ECmdType::SetParamCmd;
	sendBuf[2] = LO_OF_SHORT(cmd);
	sendBuf[3] = HI_OF_SHORT(cmd);

	//命令字
	sendBuf[4] = LO_OF_SHORT(code);
	sendBuf[5] = HI_OF_SHORT(code);

	//长度字 数据区长度
	ushort length = data.size();
	sendBuf[6] = LO_OF_SHORT(length);
	sendBuf[7] = HI_OF_SHORT(length);

	//数据内容
	if (data.size() > 0)
	{
		memcpy(&sendBuf[8], data.data(), 102);
	}

	//包头
	ushort crc = Utils::GetInstance().MakeCRCCheck(sendBuf, length+8);
	sendBuf[length] = LO_OF_SHORT(crc);
	sendBuf[length + 1] = HI_OF_SHORT(crc);

	//包尾
	//sendBuf[length + 2] = HI_OF_SHORT(Package_Tail);
	//sendBuf[length + 3] = LO_OF_SHORT(Package_Tail);

	QByteArray senddata;
	senddata.resize(length + 2);
	memcpy(senddata.data(), sendBuf, length + 8 + 2);
	return senddata;
}




void ProtocolPrint::HandleCheckPackageHead(const QByteArray& data, int pos)
{

}

void ProtocolPrint::HandlePeriodData(uchar* data, ushort length)
{
	//周期数据个数根据不同协议版本不一样
	if (length < 65)
	{
		return;
	}

	int index = 0;
	DataFieldInfo1 info;
	info._switchStatus = data[index++];

	//读取前进后退速度，0x01表示前进或顺时针旋转 0x02表示后退或逆时针
	uchar direct;
	short tempSpeed[5];
	for (int i = 0; i < 5; i++)
	{
		direct = data[index++];
		tempSpeed[i] = data[index++] << 8;
		tempSpeed[i] += data[index++];
		if (direct == 0x02)
		{
			tempSpeed[i] = -tempSpeed[i];
		}
	}
	
	//位置
	int tempPos[5];
	for (int i = 0; i < 5; i++)
	{
		direct = data[index++];
		tempPos[i] = data[index++] << 8;
		tempPos[i] += data[index++];
		if (direct == 0x02)
		{
			tempPos[i] = -tempPos[i];
		}
	}

	//右器械导丝速度位置
	int tempRightVal[6];
	for (int i = 0; i < 6; i++)
	{
		direct = data[index++];
		tempRightVal[i] = data[index++] << 8;
		tempRightVal[i] += data[index++];
		if (direct == 0x02)
		{
			tempRightVal[i] = -tempRightVal[i];
		}
	}
	

	//控制权状态
	info._controlStatus = data[index++];
	//速度提示音状态
	info._speedSoudStatus = data[index++];
	//故障码
	info._deviceFaultCode = data[index++] << 8;
	info._deviceFaultCode += data[index++];

	//发送信号
	emit sigDataInfo(info);
}



void ProtocolPrint::HandleResponseData(ushort code, uchar* data, ushort length, QByteArray arr)
{
	//如果是心跳包
	if (code == Con_Breath)
	{
		emit SigHeartBeat();
		//return;
	}
	//Note: 回复包处理逻辑缓存
	////错误发生，返回错误编码，原命令字+0x8000
	//else if (code >= 0x8000)
	//{
	//	uchar errcode = data[0];
	//	LOG_INFO(u8"下位机返回错误码,错误为: " + getErrString(errcode));
	//	LOG_INFO(QString(u8"下位机返回错误码次数统计：%1").arg(++m_codeErrorNum));
	//	emit SigCmdReply(code - 0x8000, errcode);
	//	return;
	//}
	////如果是读取数据请求返回的应答数据包
	//else if (code >= Read_Switch && code < Period_AllPara)
	//{
	//	//解析数据域
	//	if (length < 1)
	//	{
	//		return;
	//	}
	//	//switch (code)
	//	//{
	//	//case Read_HardwareVersion:
	//	//{
	//	//	QByteArray version((const char*)data, length);
	//	//	//g_hardwareVersion = version;	
	//	//}
	//	//	break;
	//	//case Read_SoftVersion:
	//	//{
	//	//	QByteArray version((const char*)data, length);
	//	//	//g_lowPcVersion = version;
	//	//}
	//	//	break;
	//	//case Read_AllPara:
	//	//	HandlePeriodData(data, length);
	//	//	break;
	//	//case Read_AllSetPara:    //读取参数设置参数
	//	//{

	//	//}
	//	//break;
	//	//}
	//}
	emit SigCmdReply(code, 0, arr);
}

short getSpeed(uchar* data)
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

short getPos(uchar* data)
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

static QString getErrString(uchar code)
{
	switch (code)
	{
	case ProtocolPrint::Command_Err:
		return QString(u8"命令码错误");
	case ProtocolPrint::DataField_Err:
		return QString(u8"数据域错误");
	case ProtocolPrint::Crc_Err:
		return QString(u8"校验错误");
	case ProtocolPrint::Status_Err:
		return QString(u8"状态错误");
	default:
		return QString(u8"未知错误");
	}
}


void ProtocolPrint::SplitComm(uint16_t command, uint8_t& highByte, uint8_t& lowByte)
{
	highByte = static_cast<uint8_t>((command >> 8) & 0xFF);
	lowByte = static_cast<uint8_t>(command & 0xFF);
}

uint16_t ProtocolPrint::GetSerResponseComm(FunCode clientCmd)
{
	uint16_t clientValue = static_cast<uint16_t>(clientCmd);
	uint8_t highByte, lowByte;
	SplitComm(clientValue, highByte, lowByte);
	return (static_cast<uint16_t>(highByte - SER_HIGH_OFFSET) << 8) | lowByte;
}



ProtocolPrint::FunCode ProtocolPrint::GetClientCommFromSer(uint16_t serverCommand)
{
	//auto it = ServerToClientCommandMap.find(serverCommand);
	//if (it != ServerToClientCommandMap.end()) {
	//	return it->second;
	//}
	return Con_Breath;
}



DataFieldInfo1::DataFieldInfo1()
{

}
