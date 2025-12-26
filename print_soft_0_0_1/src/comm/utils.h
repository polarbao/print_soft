#pragma once

#include <atomic>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <regex>

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QDateTime>


//#include "CLogManager.h"
#include "comm/CSingleton.h"



//----------------------------enum-------------------------------------
//----------------------------enum-------------------------------------

class Utils : public QObject, public CSingleton<Utils>
{
	friend class CSingleton<Utils>;
	Q_OBJECT

public:

	explicit Utils();

	~Utils();
	
	// 将输入文本转换为QByteArray(原始字节)
	QByteArray Text2ByteArr(const QString& text);

	// 将QByteArray转换为16进制字符串(用于显示)
	QString ByteArr2HexText(const QByteArray& arr);
	
	// 将16进制字符串转换回QByteArray
	QByteArray HexText2ByteArr(const QString& text);

	// 直接将文本转换为16进制表示的QByteArray
	QByteArray Text2HexByteArr(const QString& text);


	bool readAndConvertImage(const QString &filePath, QByteArray &hexData, quint16 &w, quint16 &h);

	// 组装协议报文
	QList<QByteArray> assemblePackets(quint16 width, quint16 height, quint8 imageType, const QByteArray &hexData);

	bool CheckCRC(uchar* data, int datalen);

	ushort MakeCRCCheck(uchar* data, int datalen);

	// 单个微米数据转换为4字节数据
	QByteArray MicroDisStrTo4BytesHex(const QString& strData, bool isBigEndian = true, bool isMicronDirect = true);

	// 多个微米数据转换为12字节数据
	QByteArray MultiMicroDisStrTo12BytesHex(const QString& commaStr, bool isBigEndian = true, bool isMicronDirect = true);

private:
	/**  crc高字节  **/
	static const uchar gabyCRCHi[256];
	/**  crc低字节  **/
	static const uchar gabyCRCLo[256];


};

