#include "ProtocolPrint.h"
//#include "globalHeader.h"
//#include "SerialPortData.h"
#include "CLogManager.h"
#include <QDataStream>
#include <QtEndian>
#include "utils.h"

//启动CRC检测
//#define TurnOnCRC 

//鑾峰彇short绫诲瀷鐨勯珮瀛楄妭
#define HI_OF_SHORT(X) (X >> 8)
//鑾峰彇short绫诲瀷鐨勪綆瀛楄妭
#define LO_OF_SHORT(X) (X & 0xFF)
//鍖呭ご锛?2瀛楄妭
#define Req_Package_Head 0xAABB
#define Resp_Package_Head_Succ 0xAACC
#define Resp_Package_Head_Err 0xAADD

//鍙栨秷--鍖呭熬锛?2瀛楄妭
#define Package_Tail 0xDDEE

#define SER_HIGH_OFFSET 0x10
#define ZERO 0x00

//鍗忚??鏁版嵁鍖呮渶灏忓浐瀹氶暱搴?
#define DATAGRAM_MIN_SIZE 10


ProtocolPrint::ProtocolPrint(QObject* parent /*= 0*/)
	:QObject(parent)
{
	qRegisterMetaType<DataFieldInfo1>("DataFieldInfo1");

}



void ProtocolPrint::HandleRecvDatagramData(QByteArray recvdata)
{
	//判断当前recv是req还是resp
	//分逻辑处理3个不同类型的报文数据
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

	//保留右侧不是一个完整包的数据
	m_recvBuf = m_recvBuf.right(m_recvBuf.size() - preIndex - 1);

	//解析完整包
	for (auto datagram : packageDatas)
	{
		ParsePackageData(datagram);
	}
}

void ProtocolPrint::HandleRecvDatagramData1(QByteArray recvdata)
{
	//判断当前recv是req还是resp
	//分逻辑处理3个不同类型的报文数据
	QString str = recvdata.toHex(' ');
	LOG_INFO(QString(u8"lrz_motion_sdk print_protocol_moudle cur_recv_data: %1").arg(str));

	//新数据加入缓冲区
	m_recvBuf.append(recvdata);

	if (m_recvBuf.size() < DATAGRAM_MIN_SIZE)
	{
		return;
	}

	// 存储所有包头的起始索引（包头占2字节，索引为第一个字节位置）
	// --------第一步：遍历缓冲区，收集所有合法包头的位置和类型
	struct HeadInfo
	{
		int pos;                // 包头起始位置（第一个字节的索引）
		PackageHeadType type;   // 包头类型
	};
	QList<HeadInfo> headList;   // 存储所有合法包头的信息

	// 遍历范围：0 ~ size-2（避免pos+1越界）
	for (int i = 0; i < m_recvBuf.size() - 1; i++)
	{
		PackageHeadType headType = static_cast<PackageHeadType>(HandleCheckPackageHead(m_recvBuf, i));
		if (headType != 0)
		{ // 匹配到合法包头
			headList.append({ i, headType });
			// 跳过下一个字节（包头占2字节，避免重复匹配，比如AABB的第二个字节不会被误判）
			i++;
		}
	}

	// 未找到任何合法包头，返回（可选清空脏数据）
	if (headList.isEmpty())
	{
		// m_recvBuf.clear(); // 按需启用：清空无合法包头的无效数据
		return;
	}

	// --------第二步：清理第一个包头前的脏数据
	int firstHeadPos = headList.first().pos;
	if (firstHeadPos > 0)
	{
		m_recvBuf.remove(0, firstHeadPos); // 移除前置脏数据
		// 修正所有包头的位置（缓冲区已裁剪）
		for (auto& head : headList)
		{
			head.pos -= firstHeadPos;
		}
	}
	int bufSize = m_recvBuf.size(); // 更新缓冲区长度

	  // --------第三步：分割完整数据包
	QList<QPair<QByteArray, PackageHeadType>> packageList; // 数据包 + 对应包头类型
	for (int i = 0; i < headList.size(); i++)
	{
		const HeadInfo& currHead = headList.at(i);
		// 下一个包头的位置（最后一个包头则取缓冲区末尾）
		int nextHeadPos = (i + 1 < headList.size()) ? headList.at(i + 1).pos : bufSize;
		int packageLen = nextHeadPos - currHead.pos;

		// 检查包长度是否满足最小要求，不足则保留在缓冲区
		if (packageLen >= 11) {
			QByteArray packageData = m_recvBuf.mid(currHead.pos, packageLen);
			packageList.append({ packageData, currHead.type });
		}
		else {
			break; // 最后一个包不完整，停止遍历
		}
	}

	// --------第四步：更新缓冲区，保留未处理的剩余数据
	int lastProcessedPos = 0;
	if (!packageList.isEmpty()) {
		// 最后一个完整包的结束位置
		const HeadInfo& lastHead = headList.at(packageList.size() - 1);
		int lastPackageLen = packageList.last().first.size();
		lastProcessedPos = lastHead.pos + lastPackageLen;
	}
	// 保留未处理的数据（最后一个不完整包头及后续）
	m_recvBuf = (lastProcessedPos < bufSize) ? m_recvBuf.mid(lastProcessedPos) : QByteArray();


	// --------第五步：解析所有完整数据包（可传递包头类型）
	for (auto& packageItem : packageList)
	{
		QByteArray& datagram = packageItem.first;
		PackageHeadType headType = packageItem.second;

		// 可根据包头类型做不同处理，示例：
		LOG_INFO(QString(u8"解析包头类型: 0x%1 数据包长度: %2")
			.arg(QString::number(headType, 16).toUpper())
			.arg(datagram.size()));
		ParsePackageData(datagram, headType); // 如果需要，可修改Parse函数接收headType参数：Parse(datagram, headType)
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



//void ProtocolPrint::ParsePackageData(QByteArray& datagram)
//{
//
//}

void ProtocolPrint::ParsePackageData(QByteArray& datagram, PackageHeadType type /*= PackageHeadType::Head_AABB*/)
{
	switch (type)
	{
		case ProtocolPrint::Head_AABB:
		{
			ParseReqPackageData(datagram, type);
			break;
		}
		case ProtocolPrint::Head_AACC:
		case ProtocolPrint::Head_AADD:
		{
			ParseRespPackageData(datagram, type);
			break;
		}
	default:
		break;
	}
}

void ProtocolPrint::ParseReqPackageData(QByteArray& datagram, PackageHeadType type)
{
	//接收到数据长度
	int recvLength = datagram.length();
	//数据包固定字节长度为10字节，变化长度为数据区，从0到n
	if (recvLength < DATAGRAM_MIN_SIZE)
	{
		LOG_INFO(u8"lrz_motion_sdk print_protocol_moudle cur_recv_req_package_数据长度错误");
		return;
	}

	//数据转变成uchar类型的缓存
	const int size = 512;
	uchar recvBuf[size];
	memset(recvBuf, 0, size);
	int templen = datagram.size() > size ? size : datagram.size();
	memcpy(&recvBuf[0], datagram, templen);

	//比较包头
	if (!(recvBuf[0] == LO_OF_SHORT(Req_Package_Head) && (recvBuf[1] == HI_OF_SHORT(Req_Package_Head))))
	{
		LOG_INFO(u8"lrz_motion_sdk print_protocol_moudle cur_recv_req_package_数据包头错误。");
		return;
	}

	//判断crc校验
	if (!Utils::GetInstance().CheckCRC(recvBuf, recvLength - 10))
	{
		LOG_INFO(QString(u8"lrz_motion_sdk print_protocol_moudle cur_recv_req_package_crc校验错误"));
		LOG_INFO(QString(u8"crc校验错误次数统计：%1").arg(++m_crcErrorNum));
#ifdef TurnOnCRC
		return;
#endif 
	}

	//-------------正确的包，开始解析-------------------//
	//命令类型
	ushort codeType;
	codeType = recvBuf[2] << 8;
	codeType += recvBuf[3];

	//命令码
	ushort code;
	code = recvBuf[4] << 8;
	code += recvBuf[5];

	//长度字段判断
	ushort lenByte;
	lenByte = recvBuf[6] << 8;
	lenByte += recvBuf[7];
	if (lenByte != recvLength - 10)
	{
		LOG_INFO(QString(u8"lrz_motion_sdk print_protocol_moudle cur_recv_req_package_数据区长度字段错误"));
		return;
	}

	// 拼成一个结构体，进行后续逻辑处理
	// req处理逻辑
	emit SigHandleFunOper(codeType, code);


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
		//if (codeType == Period_AllPara)
		//{
		//	HandlePeriodData(&recvBuf[7], recvLength - DATAGRAM_MIN_SIZE);
		//}
	}
}

// 处理操作成功的报文数据
// type设置默认数据类型
void ProtocolPrint::ParseRespPackageData(QByteArray& datagram, PackageHeadType type)
{
	if (type == Head_AACC)
	{
		// 读取命令数据类型，获取数据信息
		// 存坐标、存速度、存

		//接收到数据长度

		int recvLength = datagram.length();
		PackParam packData;

		//数据转变成uchar类型的缓存
		const int size = 512;
		uchar recvBuf[size];
		memset(recvBuf, 0, size);
		int templen = datagram.size() > size ? size : datagram.size();
		memcpy(&recvBuf[0], datagram, templen);

		//-------------正确的包，开始解析-------------------//
		//命令类型
		packData.head = (recvBuf[0] << 8) | recvBuf[1];

		//命令码
		ushort operType;
		operType = (recvBuf[2] << 8) | recvBuf[3];
		packData.operType = operType;

		//长度字段判断
		ushort code;
		code = (recvBuf[4] << 8) | recvBuf[5];
		packData.cmdFun = code;

		//锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷
		ushort dataLen;
		dataLen = (recvBuf[6] << 8) | recvBuf[7];
		packData.dataLen = dataLen;
		if (dataLen != datagram.length() - 10)
		{
			LOG_INFO(QString(u8"lrz_motion_sdk print_protocol_moudle cur_recv_req_package_锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟街段达拷锟斤拷"));
			return;
		}

		// 拼成一个结构体，进行后续逻辑处理

		int copyLen = (dataLen < DATA_LEN_12) ? dataLen : DATA_LEN_12;
		memcpy(&packData.data, &recvBuf[8], dataLen);

		MoveAxisPos posData;
		posData.xPos = (packData.data[0] << 24) | (packData.data[1] << 16) | (packData.data[2] << 8) | packData.data[3];
		posData.yPos = (packData.data[4] << 24) | (packData.data[5] << 16) | (packData.data[6] << 8) | packData.data[7];
		posData.zPos = (packData.data[8] << 24) | (packData.data[9] << 16) | (packData.data[10] << 8) | packData.data[11];

		// req处理逻辑
		emit SigHandleFunOper(operType, code);
		emit SigHandleFunOper1(packData);
		emit SigHandleFunOper2(code, posData);

	}
	else if (type == Head_AADD)
	{
		//emit SigPackFailRetransport(datagram, type);
		emit SigPackFailRetransport(datagram);
	}

}

////处理操作失败的报文数据
//void ProtocolPrint::HandleFailedRespPackageData(QByteArray& datagram, PackageHeadType type)
//{
//
//}

QByteArray ProtocolPrint::GetSendDatagram(ECmdType cmdType, FunCode cmd, QByteArray data)
{
	const int size = 1024;
	uchar sendBuf[size];
	memset(sendBuf, 0, size);

	//包头+命令类型+命令+数据区长度+CRC
	//包头
	sendBuf[0] = LO_OF_SHORT(Req_Package_Head);
	sendBuf[1] = HI_OF_SHORT(Req_Package_Head);

	// 命令类型
	sendBuf[2] = HI_OF_SHORT(cmdType);
	sendBuf[3] = LO_OF_SHORT(cmdType);

	//命令字
	sendBuf[4] = HI_OF_SHORT(cmd);
	sendBuf[5] = LO_OF_SHORT(cmd);

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
	ushort crc = Utils::GetInstance().MakeCRCCheck(sendBuf, length + 8);
	sendBuf[length + 8] = HI_OF_SHORT(crc);
	sendBuf[length + 9] = LO_OF_SHORT(crc);

	QByteArray senddata;
	senddata.resize(length + 8 + 2);
	memcpy(senddata.data(), sendBuf, length + 8 + 2);
	return senddata;
}

QList<QByteArray> ProtocolPrint::GetSendImgDatagram(quint16 w, quint16 h, quint8 Imgtype, const QByteArray &hexData)
{
	QList<QByteArray> packets;
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
	{

	}
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

	//CRC
	ushort crc = Utils::GetInstance().MakeCRCCheck(sendBuf, length+8);
	sendBuf[length] = LO_OF_SHORT(crc);
	sendBuf[length + 1] = HI_OF_SHORT(crc);

	//包头
	//sendBuf[length + 2] = HI_OF_SHORT(Package_Tail);
	//sendBuf[length + 3] = LO_OF_SHORT(Package_Tail);

	QByteArray senddata;
	senddata.resize(length + 2);
	memcpy(senddata.data(), sendBuf, length + 8 + 2);
	return senddata;
}




void ProtocolPrint::HandlePeriodData(uchar* data, ushort length)
{
	return;
}



void ProtocolPrint::HandleResponseData(ushort code, uchar* data, ushort length, QByteArray arr)
{
	//如果是心跳包
	if (code == ProtocolPrint::Get_Breath)
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
	return ProtocolPrint::Get_Breath;
}



DataFieldInfo1::DataFieldInfo1()
{

}

//MoveAxisPos::MoveAxisPos()
//{
//	xPos = 0;
//	yPos = 0;
//	zPos = 0;
//}
//
