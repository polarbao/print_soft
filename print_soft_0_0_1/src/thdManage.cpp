#include "thdManage.h"
#include "CLogManager.h"
#include <iostream>
#include <stdlib.h>


ThreadMgr::ThreadMgr(QObject *parent)

{
	Init();
}

ThreadMgr::~ThreadMgr()
{
	//if (m_pTcpThread->isRunning()) 
	//{
	//	m_pTcpThread->quit();
	//	m_pTcpThread->wait();
	//}

	//if (m_pScanLogicThread->isRunning()) 
	//{
	//	m_pScanLogicThread->quit();
	//	m_pScanLogicThread->wait();
	//}
}


void ThreadMgr::Init()
{
	////日志：显示当前线程ID
	auto threadID = QStringLiteral("cur_thread_main_thread_moudle, thread_id = @0x%1").arg(quintptr(QThread::currentThreadId()), 16, 16, QLatin1Char('0'));
	LOG_DEBUG(threadID);

	////kill_scan_thd
	//QProcess::startDetached("taskkill -t  -f /IM " + QString("scanservice.exe"));
	//{
	//	//线程控制
	//	{
	//		m_pTcpThread = std::make_unique<QThread>(this);
	//		m_pScanLogicThread = std::make_unique<QThread>(this);
	//	}

	//	//逻辑处理
	//	{
			//m_pTcpWorker = std::make_unique<TcpClient>();
			//m_pTcpWorker->moveToThread(m_pTcpThread.get());

	//		m_pScanLogicWorker = std::make_unique<ScanLogic>();
	//		m_pScanLogicWorker->moveToThread(m_pScanLogicThread.get());
	//	}


	//	//Init_connect	
	//	connect(this, &ThreadMgr::SigScanInit, m_pScanLogicWorker.get(), &ScanLogic::OnHandleScanInit, Qt::QueuedConnection);


	//	connect(m_pScanLogicWorker.get(), &ScanLogic::SigScanFinsihed, m_pScanLogicThread.get(), &QThread::quit);
	//	connect(m_pScanLogicWorker.get(), &ScanLogic::SigScanFinsihed, m_pScanLogicWorker.get(), &ScanLogic::deleteLater);
	//	connect(m_pScanLogicThread.get(), &QThread::finished, m_pScanLogicThread.get(), &QThread::deleteLater);

	//	//thread_start
	//	m_pTcpThread->start();
	//	m_pScanLogicThread->start();
	//	emit SigScanInit();
	//}
	
}
