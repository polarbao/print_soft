#include "printDeviceMoudle.h"
#include <QPushButton>
#include <QSplitter>
#include <qapplication.h>
//#include "motioncontrolsdk_global.h"


printDeviceMoudle::printDeviceMoudle(QWidget *parent)
    : QWidget(parent)
	, m_printUI(PrintDeviceUI())
	//, m_motion(parent)
	//, m_serialUI(SerialPortWidget(new SerialPortData()))
{
	Init();
}

printDeviceMoudle::~printDeviceMoudle()
{
	
		
}

void printDeviceMoudle::Init()
{
	InitUI();
	//auto ret = m_motion.initialize("./");
	//auto ret1 = m_motion.isInitialized();
	//m_motion.connectToDevice("127.0.0.1");
	//auto ret2 = m_motion.isConnected();
	//auto ret3 = m_motion.moveTo(123, 123, 123);

}

void printDeviceMoudle::InitUI()
{
	QGridLayout* mainlay = new QGridLayout(this);
	mainlay->setContentsMargins(5, 5, 5, 5);
	mainlay->setSpacing(10);
	this->setLayout(mainlay);

	QWidget* contentWidget = new QWidget;
	QGridLayout* layout = new QGridLayout();
	layout->setContentsMargins(5, 5, 5, 5);
	layout->setSpacing(5);
	contentWidget->setLayout(layout);
	mainlay->addWidget(contentWidget, 0, 0);

	QRect rect = QApplication::desktop()->screenGeometry();
	m_printUI.setMaximumSize(rect.width(), rect.height() - 100);
	m_printUI.setMinimumWidth(340);

// 	m_serialUI.setMaximumSize(rect.width() / 2, rect.height() - 150);
// 	m_serialUI.setMinimumWidth(340);

	QSplitter* spliter = new QSplitter;
	spliter->addWidget(&m_printUI);
	//spliter->addWidget(&m_serialUI);

	spliter->setStretchFactor(0, 1);
	//spliter->setStretchFactor(1, 1);

	spliter->setMinimumSize(340, 500);
	spliter->setChildrenCollapsible(false);
	spliter->setLineWidth(2);
	layout->addWidget(spliter, 0, 0);
	//this->resize(1000, 1000);

	//this->showMaximized();

	//this->setStyleSheet("QWidget{background:rgb(27,26,31);border:0px;}\
	//					 QTabBar::tab{background:lightgrey;margin-left:10px;padding:10px;}\
	//				     QTabBar::tab:selected{background:blue;color:white;}\
	//					 QTabWidget:pane{background-color:transparent;}\
	//					 QLabel{font-size:25px;color:white;background:transparent;}\
	//					 QLineEdit{background-color:white;font:25px;}\
	//					 QTextEdit{background-color:white;font:20px;}\
	//					 QPushButton{height:50px;font:20px;color:white;border-radius:8px;background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 rgba(61,112,97,1),stop:1 rgba(61,117,143,1));}\
	//					 QPushButton:enabled:hover{background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 rgba(61,112,97,0.8),stop:1 rgba(61,117,143,0.8));}\
	//					 QPushButton:enabled:pressed{background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 rgba(0,252,235,1),stop:1 rgba(0,147,137,1));}\
	//					 QPushButton:!enabled{background:rgb(23,35,46);}\
	//					 QGroupBox::title{subcontrol-origin:margin;subcontrol-position:top left;font-size:10px;color:white;}\
	//					 QGroupBox{border:1px;background:rgb(41,42,47);border-radius:10px;}\
	//					 QSpinBox{background:white;color:black;qproperty-alignment:AlignCenter;}");



}
