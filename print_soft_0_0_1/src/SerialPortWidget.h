#pragma once

#include "QtWidgets/QtWidgets"
#include "SerialPortData.h"

/** 
*  @author      
*  @class       SerialPortWidget 
*  @brief       串口配置界面
*/
class SerialPortWidget : public QDialog
{
	Q_OBJECT
public:
	SerialPortWidget(SerialPortData* data, QWidget* parent = 0);
	~SerialPortWidget();
	
public slots:
	void onSetComBtn();
	void onDataComing(QByteArray msg);
	void onError(QSerialPort::SerialPortError error);

private:
	void initUI();


	/** 串口名称选择框  */
	QComboBox*      m_portNameBox;

	/** 波特率  */
	QComboBox*      m_baudRateBox;

	/** 校验位  */
	QComboBox*      m_parityBitBox;

	/** 数据位  */
	QComboBox*      m_dataBitBox;

	/** 停止位  */
	QComboBox*      m_stopBitBox;

	/** 设置串口参数按钮  */
	QPushButton*    m_setComBtn;

	/** 串口打开状态 */
	QLabel*         m_pstatusLab;
	
	QTextEdit*      m_inTextEdit;
	QTextEdit*      m_outTextEdit;
	
	//data
	SerialPortData*  m_pSerialPortObj;
	QButtonGroup*	m_btnGroup;








};
