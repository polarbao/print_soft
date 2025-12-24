#pragma once

//#include "ui_printDeviceMoudle.h"
#include <QtWidgets/QtWidgets>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "printDeviceUI.h"


namespace UI { class printDeviceMoudle; }

class printDeviceMoudle : public QWidget
{
    Q_OBJECT

public:
    
	
	printDeviceMoudle(QWidget *parent = nullptr);


    ~printDeviceMoudle();


private:

	void Init();

	void InitUI();



private:
	//motionControlSDK m_motion;
	PrintDeviceUI m_printUI;
	//SerialPortWidget m_serialUI;
};
