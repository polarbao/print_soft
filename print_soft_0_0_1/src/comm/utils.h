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


#include "SpdlogMgr.h"
#include "CNewSingleton.h"
#include "motionControlSDK.h"



class Utils : public QObject, public CNewSingleton<Utils>
{
	Q_OBJECT
private:
	friend class CNewSingleton<Utils>;

	explicit Utils();

public:

	~Utils();
	
	// 将输入文本转换为QByteArray(原始字节)
	QByteArray Text2ByteArr(const QString& text);

	// 将QByteArray转换为16进制字符串(用于显示)
	QString ByteArr2HexText(const QByteArray& arr);
	
	// 将16进制字符串转换回QByteArray
	QByteArray HexText2ByteArr(const QString& text);

	// 直接将文本转换为16进制表示的QByteArray
	QByteArray Text2HexByteArr(const QString& text);

	bool CheckCRC(uchar* data, int datalen);

	ushort MakeCRCCheck(uchar* data, int datalen);

	// 单个微米数据转换为4字节数据
	QByteArray MicroDisStrTo4BytesHex(const QString& strData, bool isBigEndian = true, bool isMicronDirect = true);

	// 多个微米数据转换为12字节数据
	QByteArray MultiMicroDisStrTo12BytesHex(const QString& commaStr, bool isBigEndian = true, bool isMicronDirect = true);

	// 将qstring数据类型转换为MoveAxis数据
	MoveAxisPos StrLis2MoveAxisData(const QString& strData);


private:
	/**  crc高字节  **/
	static const uchar gabyCRCHi[256];
	/**  crc低字节  **/
	static const uchar gabyCRCLo[256];


};

