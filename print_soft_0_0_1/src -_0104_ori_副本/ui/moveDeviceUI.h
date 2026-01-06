#pragma once

#include <QWidget>
#include <QButtonGroup>
#include <QLineEdit>
#include <QTextEdit>
#include <QSerialPort>
#include <QSerialPortInfo>


#include "global.h"

namespace UI { class MoveDeviceUI; }

class MoveDeviceUI : public QWidget
{
    Q_OBJECT

public:
    
	
	MoveDeviceUI(QWidget *parent = nullptr);


    ~MoveDeviceUI();


private:

	void Init();

	void InitUI();

public slots:

	void OnBtnClicked(int id);

	void OnBtnToggled(int id, bool bToggle);

	void OnAddOperLog(const QString& logStr);

signals:

	void SigBtnClicked(const ELogic::EDMDT& btnType);

	void SigAddShowLog(const QString& std);

	void SigSendSerialComm(const QString& std);


private:
	QButtonGroup m_btnGroup;
	QLineEdit m_inputComm;
	QTextEdit m_logShow;


	//定义串口指针
	QSerialPort *serialPort;


    //Ui::printDeviceMoudleClass ui;
	//QWidget ui;
};
