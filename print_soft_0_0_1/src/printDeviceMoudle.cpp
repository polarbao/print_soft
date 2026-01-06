#include "printDeviceMoudle.h"
#include <QSplitter>
#include <qapplication.h>

printDeviceMoudle::printDeviceMoudle(QWidget *parent)
    : QWidget(parent)
	, m_printUI(PrintDeviceUI())
{
	Init();
}

printDeviceMoudle::~printDeviceMoudle()
{
	
		
}

void printDeviceMoudle::Init()
{
	InitUI();
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

	QSplitter* spliter = new QSplitter;
	spliter->addWidget(&m_printUI);
	spliter->setStretchFactor(0, 1);
	spliter->setMinimumSize(340, 500);
	spliter->setChildrenCollapsible(false);
	spliter->setLineWidth(2);
	layout->addWidget(spliter, 0, 0);

}
