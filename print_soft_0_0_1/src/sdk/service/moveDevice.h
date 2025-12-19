#pragma once

#include <QObject>

class MoveDevice : public QObject
{
    Q_OBJECT

public:

	MoveDevice(QWidget *parent = nullptr);
    
	
	~MoveDevice();

private:
	/*
	业务逻辑：
	1. 龙门移动（X Y Z
	2. 复位
	3. 打印头冲洗
	4. 通信协议制定
	*/

private:

};
