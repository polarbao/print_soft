#include "ProtocolPrint.h"
//#include "globalHeader.h"
//#include "SerialPortData.h"
#include "CLogManager.h"
#include <QDataStream>
#include <QtEndian>
#include "utils.h"

//����CRC���
//#define TurnOnCRC 

//获取short类型的高字节
#define HI_OF_SHORT(X) (X >> 8)
//获取short类型的低字节
#define LO_OF_SHORT(X) (X & 0xFF)
//包头�?2字节
#define Req_Package_Head 0xAABB
#define Resp_Package_Head_Succ 0xAACC
#define Resp_Package_Head_Err 0xAADD

//取消--包尾�?2字节
#define Package_Tail 0xDDEE

#define SER_HIGH_OFFSET 0x10
#define ZERO 0x00

//协�??数据包最小固定长�?
#define DATAGRAM_MIN_SIZE 10


ProtocolPrint::ProtocolPrint(QObject* parent /*= 0*/)
	:QObject(parent)
{
	qRegisterMetaType<DataFieldInfo1>("DataFieldInfo1");

}



void ProtocolPrint::HandleRecvDatagramData(QByteArray recvdata)
{
	//�жϵ�ǰrecv��req����resp
	//���߼�����3����ͬ���͵ı�������
	QString str = recvdata.toHex(' ');
	LOG_INFO(QString(u8"��������: %1").arg(str));

	//�����ݼ��뻺����
	m_recvBuf.append(recvdata);

	if (m_recvBuf.size() < DATAGRAM_MIN_SIZE)
	{
		return;
	}

	//--------�������������ҵ�ÿһ�����ݰ�βDD EE��λ��
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

	//�����Ҳ಻��һ��������������
	m_recvBuf = m_recvBuf.right(m_recvBuf.size() - preIndex - 1);

	//����������
	for (auto datagram : packageDatas)
	{
		ParsePackageData(datagram);
	}
}

void ProtocolPrint::HandleRecvDatagramData1(QByteArray recvdata)
{
	//�жϵ�ǰrecv��req����resp
	//���߼�����3����ͬ���͵ı�������
	QString str = recvdata.toHex(' ');
	LOG_INFO(QString(u8"lrz_motion_sdk print_protocol_moudle cur_recv_data: %1").arg(str));

	//�����ݼ��뻺����
	m_recvBuf.append(recvdata);

	if (m_recvBuf.size() < DATAGRAM_MIN_SIZE)
	{
		return;
	}

	// �洢���а�ͷ����ʼ��������ͷռ2�ֽڣ�����Ϊ��һ���ֽ�λ�ã�
	// --------��һ�����������������ռ����кϷ���ͷ��λ�ú�����
	struct HeadInfo
	{
		int pos;                // ��ͷ��ʼλ�ã���һ���ֽڵ�������
		PackageHeadType type;   // ��ͷ����
	};
	QList<HeadInfo> headList;   // �洢���кϷ���ͷ����Ϣ

	// ������Χ��0 ~ size-2������pos+1Խ�磩
	for (int i = 0; i < m_recvBuf.size() - 1; i++)
	{
		PackageHeadType headType = static_cast<PackageHeadType>(HandleCheckPackageHead(m_recvBuf, i));
		if (headType != 0)
		{ // ƥ�䵽�Ϸ���ͷ
			headList.append({ i, headType });
			// ������һ���ֽڣ���ͷռ2�ֽڣ������ظ�ƥ�䣬����AABB�ĵڶ����ֽڲ��ᱻ���У�
			i++;
		}
	}

	// δ�ҵ��κκϷ���ͷ�����أ���ѡ��������ݣ�
	if (headList.isEmpty())
	{
		// m_recvBuf.clear(); // �������ã�����޺Ϸ���ͷ����Ч����
		return;
	}

	// --------�ڶ�����������һ����ͷǰ��������
	int firstHeadPos = headList.first().pos;
	if (firstHeadPos > 0)
	{
		m_recvBuf.remove(0, firstHeadPos); // �Ƴ�ǰ��������
		// �������а�ͷ��λ�ã��������Ѳü���
		for (auto& head : headList)
		{
			head.pos -= firstHeadPos;
		}
	}
	int bufSize = m_recvBuf.size(); // ���»���������

	  // --------���������ָ��������ݰ�
	QList<QPair<QByteArray, PackageHeadType>> packageList; // ���ݰ� + ��Ӧ��ͷ����
	for (int i = 0; i < headList.size(); i++)
	{
		const HeadInfo& currHead = headList.at(i);
		// ��һ����ͷ��λ�ã����һ����ͷ��ȡ������ĩβ��
		int nextHeadPos = (i + 1 < headList.size()) ? headList.at(i + 1).pos : bufSize;
		int packageLen = nextHeadPos - currHead.pos;

		// ���������Ƿ�������СҪ�󣬲��������ڻ�����
		if (packageLen >= 11) {
			QByteArray packageData = m_recvBuf.mid(currHead.pos, packageLen);
			packageList.append({ packageData, currHead.type });
		}
		else {
			break; // ���һ������������ֹͣ����
		}
	}

	// --------���Ĳ������»�����������δ������ʣ������
	int lastProcessedPos = 0;
	if (!packageList.isEmpty()) {
		// ���һ���������Ľ���λ��
		const HeadInfo& lastHead = headList.at(packageList.size() - 1);
		int lastPackageLen = packageList.last().first.size();
		lastProcessedPos = lastHead.pos + lastPackageLen;
	}
	// ����δ���������ݣ����һ����������ͷ��������
	m_recvBuf = (lastProcessedPos < bufSize) ? m_recvBuf.mid(lastProcessedPos) : QByteArray();


	// --------���岽�����������������ݰ����ɴ��ݰ�ͷ���ͣ�
	for (auto& packageItem : packageList)
	{
		QByteArray& datagram = packageItem.first;
		PackageHeadType headType = packageItem.second;

		// �ɸ��ݰ�ͷ��������ͬ������ʾ����
		LOG_INFO(QString(u8"������ͷ����: 0x%1 ���ݰ�����: %2")
			.arg(QString::number(headType, 16).toUpper())
			.arg(datagram.size()));
		ParsePackageData(datagram, headType); // �����Ҫ�����޸�Parse��������headType������Parse(datagram, headType)
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
	quint16 headValue = (hi << 8) | lo; // ƴ�ӳ�16λ��ͷֵ

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
	//���յ����ݳ���
	int recvLength = datagram.length();
	//���ݰ��̶��ֽڳ���Ϊ10�ֽڣ��仯����Ϊ����������0��n
	if (recvLength < DATAGRAM_MIN_SIZE)
	{
		LOG_INFO(u8"lrz_motion_sdk print_protocol_moudle cur_recv_req_package_���ݳ��ȴ���");
		return;
	}

	//����ת���uchar���͵Ļ���
	const int size = 512;
	uchar recvBuf[size];
	memset(recvBuf, 0, size);
	int templen = datagram.size() > size ? size : datagram.size();
	memcpy(&recvBuf[0], datagram, templen);

	//�Ƚϰ�ͷ
	if (!(recvBuf[0] == LO_OF_SHORT(Req_Package_Head) && (recvBuf[1] == HI_OF_SHORT(Req_Package_Head))))
	{
		LOG_INFO(u8"lrz_motion_sdk print_protocol_moudle cur_recv_req_package_���ݰ�ͷ����");
		return;
	}

	//�ж�crcУ��
	if (!Utils::GetInstance().CheckCRC(recvBuf, recvLength - 10))
	{
		LOG_INFO(QString(u8"lrz_motion_sdk print_protocol_moudle cur_recv_req_package_crcУ�����"));
		LOG_INFO(QString(u8"crcУ��������ͳ�ƣ�%1").arg(++m_crcErrorNum));
#ifdef TurnOnCRC
		return;
#endif 
	}

	//-------------正确的包，开始解析-------------------//
	// 命令类型 (小端字节序)
	ushort codeType;
	codeType = (recvBuf[3] << 8) | recvBuf[2];

	// 命令字 (小端字节序)
	ushort code;
	code = (recvBuf[5] << 8) | recvBuf[4];

	// 长度字段判断 (小端字节序)
	ushort lenByte;
	lenByte = (recvBuf[7] << 8) | recvBuf[6];
	if (lenByte != recvLength - 10)
	{
		LOG_INFO(QString(u8"lrz_motion_sdk print_protocol_moudle cur_recv_req_package_�����������ֶδ���"));
		return;
	}

	// ƴ��һ���ṹ�壬���к����߼�����
	// req�����߼�
	emit SigHandleFunOper(codeType, code);


	//�������λ����Ӧ��λ���İ�
	if (recvBuf[1] == LO_OF_SHORT(Req_Package_Head))
	{
		//Note:�����ظ�����
		//�ظ����������λ-0x1000
		code = (recvBuf[4] - SER_HIGH_OFFSET) << 8;
		code += recvBuf[5];
		HandleResponseData(code, &recvBuf[7], recvLength - DATAGRAM_MIN_SIZE, datagram);
		//
		//auto a = GetResponseCommData(datagram);
		//return;
	}
	//�������λ���������͵���λ���İ�
	else
	{
		//��λ���������͵����ݰ�
		//if (codeType == Period_AllPara)
		//{
		//	HandlePeriodData(&recvBuf[7], recvLength - DATAGRAM_MIN_SIZE);
		//}
	}
}

// ���������ɹ��ı�������
// type����Ĭ����������
void ProtocolPrint::ParseRespPackageData(QByteArray& datagram, PackageHeadType type)
{
	if (type == Head_AACC)
	{
		// ��ȡ�����������ͣ���ȡ������Ϣ
		// �����ꡢ���ٶȡ���

		//���յ����ݳ���

		int recvLength = datagram.length();
		PackParam packData;

		//����ת���uchar���͵Ļ���
		const int size = 512;
		uchar recvBuf[size];
		memset(recvBuf, 0, size);
		int templen = datagram.size() > size ? size : datagram.size();
		memcpy(&recvBuf[0], datagram, templen);

		//-------------正确的包，开始解析-------------------//
		// 包头 (小端字节序)
		packData.head = (recvBuf[1] << 8) | recvBuf[0];

		// 命令类型 (小端字节序)
		ushort operType;
		operType = (recvBuf[3] << 8) | recvBuf[2];
		packData.operType = operType;

		// 命令字段判断 (小端字节序)
		ushort code;
		code = (recvBuf[5] << 8) | recvBuf[4];
		packData.cmdFun = code;

		// 数据长度 (小端字节序)
		ushort dataLen;
		dataLen = (recvBuf[7] << 8) | recvBuf[6];
		packData.dataLen = dataLen;
		if (dataLen != datagram.length() - 10)
		{
			LOG_INFO(QString(u8"lrz_motion_sdk print_protocol_moudle cur_recv_req_package_�����������ֶδ���"));
			return;
		}

		// ƴ��һ���ṹ�壬���к����߼�����

		int copyLen = (dataLen < DATA_LEN_12) ? dataLen : DATA_LEN_12;
		memcpy(&packData.data, &recvBuf[8], dataLen);

		MoveAxisPos posData;
		// 数据区使用小端字节序解析
		posData.xPos = (packData.data[3] << 24) | (packData.data[2] << 16) | (packData.data[1] << 8) | packData.data[0];
		posData.yPos = (packData.data[7] << 24) | (packData.data[6] << 16) | (packData.data[5] << 8) | packData.data[4];
		posData.zPos = (packData.data[11] << 24) | (packData.data[10] << 16) | (packData.data[9] << 8) | packData.data[8];

		// req�����߼�
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

////��������ʧ�ܵı�������
//void ProtocolPrint::HandleFailedRespPackageData(QByteArray& datagram, PackageHeadType type)
//{
//
//}

QByteArray ProtocolPrint::GetSendDatagram(ECmdType cmdType, FunCode cmd, QByteArray data)
{
	const int size = 1024;
	uchar sendBuf[size];
	memset(sendBuf, 0, size);

	//��ͷ+��������+����+����������+CRC
	//��ͷ
	sendBuf[0] = LO_OF_SHORT(Req_Package_Head);
	sendBuf[1] = HI_OF_SHORT(Req_Package_Head);

	// ��������
	sendBuf[2] = HI_OF_SHORT(cmdType);
	sendBuf[3] = LO_OF_SHORT(cmdType);

	//������
	sendBuf[4] = HI_OF_SHORT(cmd);
	sendBuf[5] = LO_OF_SHORT(cmd);

	//������
	// ����������
	ushort length = data.size();
	sendBuf[6] = LO_OF_SHORT(length);
	sendBuf[7] = HI_OF_SHORT(length);
	
	//��������
	if (data.size() > 0)
	{
		memcpy(&sendBuf[8], data.data(), data.size());
	}

	//У��
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

	//��ͷ
	sendBuf[0] = LO_OF_SHORT(Resp_Package_Head_Succ);
	sendBuf[1] = HI_OF_SHORT(Resp_Package_Head_Succ);

	//��������
	ProtocolPrint::ECmdType cmd = ProtocolPrint::ECmdType::SetParamCmd;
	sendBuf[2] = LO_OF_SHORT(cmd);
	sendBuf[3] = HI_OF_SHORT(cmd);

	//������
	sendBuf[4] = LO_OF_SHORT(code);
	sendBuf[5] = HI_OF_SHORT(code);

	//������ ����������
	ushort length = data.size();
	sendBuf[6] = LO_OF_SHORT(length);
	sendBuf[7] = HI_OF_SHORT(length);

	//��������
	if (data.size() > 0)
	{
		memcpy(&sendBuf[8], data.data(), 102);
	}

	//CRC
	ushort crc = Utils::GetInstance().MakeCRCCheck(sendBuf, length+8);
	sendBuf[length] = LO_OF_SHORT(crc);
	sendBuf[length + 1] = HI_OF_SHORT(crc);

	//��ͷ
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
	//�����������
	if (code == ProtocolPrint::Get_Breath)
	{
		emit SigHeartBeat();
		//return;
	}
	//Note: �ظ��������߼�����
	////�����������ش�����룬ԭ������+0x8000
	//else if (code >= 0x8000)
	//{
	//	uchar errcode = data[0];
	//	LOG_INFO(u8"��λ�����ش�����,����Ϊ: " + getErrString(errcode));
	//	LOG_INFO(QString(u8"��λ�����ش��������ͳ�ƣ�%1").arg(++m_codeErrorNum));
	//	emit SigCmdReply(code - 0x8000, errcode);
	//	return;
	//}
	////����Ƕ�ȡ�������󷵻ص�Ӧ�����ݰ�
	//else if (code >= Read_Switch && code < Period_AllPara)
	//{
	//	//����������
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
	//	//case Read_AllSetPara:    //��ȡ�������ò���
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
		return QString(u8"���������");
	case ProtocolPrint::DataField_Err:
		return QString(u8"���������");
	case ProtocolPrint::Crc_Err:
		return QString(u8"У�����");
	case ProtocolPrint::Status_Err:
		return QString(u8"״̬����");
	default:
		return QString(u8"δ֪����");
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
