#pragma once

#include <memory>
#include "global.h"
#include "comm/CSingleton.h"
#include <QObject>
#include "TcpClient.h"

/*
*  All rights reserved.
*
*  @file        CThdManage.h	线程管理模块
*  @author		lrz
*/


/** 
*  @author      lrz
*  @class       CThdManage 
*  @brief       线程管理类 
*/
class ThreadMgr : public QObject, public CSingleton<ThreadMgr>
{
	friend class CSingleton<ThreadMgr>;
	Q_OBJECT

public:
	/** 
	*  @brief       构造函数 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	ThreadMgr(QObject *parent = nullptr);

	/** 
	*  @brief       析构函数 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
    ~ThreadMgr();


private:
	/** 
	*  @brief       初始化函数 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	void Init();

signals:
	/** 
	*  @brief       开始扫描初始化信号 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	void SigScanInit();


private:
	////m_data
	//std::unique_ptr<TcpClient> m_pTcpWorker;
	//std::unique_ptr<ScanLogic> m_pSerialWorker;

	////m_thread
	//std::unique_ptr <QThread> m_pTcpThread;
	//std::unique_ptr<QThread> m_pSerialThread;
};
