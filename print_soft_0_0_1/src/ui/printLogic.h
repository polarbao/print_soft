#pragma once

#include "QtWidgets/QtWidgets"
#include <QMutex>
#include <QTimer>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QButtonGroup>
#include <memory>





class PrintSimulateHandle : public QObject
{
    Q_OBJECT

public:

	PrintSimulateHandle(QWidget *parent = nullptr);


    ~PrintSimulateHandle();


private:
	/** 
	*  @brief       初始化函数 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	void Init();

	/** 
	*  @brief       UI界面初始化函数 
	*  @param[in]    
	*  @param[out]   
	*  @return                    
	*/
	void InitUI();

	/** 
	*  @brief       读取运动控制配置文件至UI界面
	*  @param[in]   data - 运动控制模块结构体数据
	*  @param[out]   
	*  @return                    
	*/
	void SetMCCfgFile2UI(const MotionConfig& data);
	 
public slots:
	/** 
	*  @brief       网络控制按钮槽函数 
	*  @param[in]   idx - 按钮枚举值（NetFun）
	*  @param[out]   
	*  @return                    
	*/
	void OnListenBtnClicked(int idx);
	
	//add_UI操作界面处理逻辑
	/** 
	*  @brief       操作日志显示处理逻辑 
	*  @param[in]   msg - 操作日志
	*  @param[in]	arr - 16进制数据
	*  @param[in]	type - 显示框类型
	*  @param[out]   
	*  @return                    
	*/
	void OnShowOperCmd(const QString& msg, QByteArray arr, const ShowEditType& type);


	//Btn
	/** 
	*  @brief       打印逻辑处理槽函数 
	*  @param[in]   idx - 按钮类型（EPrintOperFun
	*  @param[out]   
	*  @return                    
	*/
	void OnHandlePrintOperFun(int idx);

	/**
	*  @brief       打印参数处理槽函数
	*  @param[in]   idx - 按钮类型（EPrintOperFun
	*  @param[out]
	*  @return
	*/
	void OnHandlePrintParamFun(int idx);

	/**
	*  @brief       打印移动处理槽函数
	*  @param[in]   idx - 按钮类型（EPrintMoveOperFun
	*  @param[out]
	*  @return
	*/
	void OnHandlePrintMoveFun(int idx);



signals:

	void SigAddShowLog(const QString& logStr);

	void SigShowOperComm(const QString& msg, const ShowEditType& type, QByteArray arr = QByteArray());
	
	void SigAddShowOperCmd(const QString& msg, QByteArray arr = QByteArray(), const ShowEditType& type = ShowEditType::ESET_Sendomm);


private:
	//sdk
	motionControlSDK* m_motionSDK;

	//btn
	QButtonGroup* m_netBtnGroup;
	QButtonGroup* m_printOperBtnGroup;
	QButtonGroup* m_printParamBtnGroup;
	QButtonGroup* m_printMoveBtnGroup;
	QButtonGroup* m_simulatePrtBtnGroup;


	//UI
	QLineEdit* m_ipLine;
	QLineEdit* m_portLine;
	QLabel* m_connStateLab;
	QTextEdit*	m_operLogText;
	QTextEdit*	m_sendCommText;
	QTextEdit*	m_recvCommText;

};
