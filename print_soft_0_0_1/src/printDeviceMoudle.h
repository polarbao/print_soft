#pragma once

#include <QtWidgets/QtWidgets>
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
	PrintDeviceUI m_printUI;
};
