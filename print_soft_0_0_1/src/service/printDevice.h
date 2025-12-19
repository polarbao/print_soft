#pragma once

#include <QObject>

class PrintDevice : public QObject
{
    Q_OBJECT

public:
	PrintDevice(QWidget *parent = nullptr);
	~PrintDevice();




	//业务逻辑
	/*
	业务逻辑：
	1. 开始打印
	2. 暂停打印
	3. 复位
	4. 打印头冲洗
	*/

private:

};
