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


#include "global.h"

#include "motioncontrolsdk_define.h"
#include "motionControlSDK.h"


namespace UI { class PrintDeviceUI; }

class motionControlSDK;

class PrintDeviceUI : public QDialog
{
    Q_OBJECT

public:
	enum NetFun
	{
		ENF_Begin,
		ENF_StartConn,
		ENF_DisConn,
		ENF_ChangePort,
		ENF_ClearAllText,
		ENF_End
	};

	enum EPrintParamType
	{
		EPPT_Begin = ENF_End + 1,
		EPPT_SetStartPos,
		EPPT_SetStopPos,
		EPPT_SetStepX,
		EPPT_SetStepY,
		EPPT_SetStepZ,
		EPPT_SetClenaPos,
		EPPT_SetLimitXPos,
		EPPT_SetLimitYPos,
		EPPT_SetLimitZPos,
		EPPT_SetSpdX,
		EPPT_SetSpdY,
		EPPT_SetSpdZ,
		EPPT_SetAccX,
		EPPT_SetAccY,
		EPPT_SetAccZ,
		EPPT_SetOffsetX,
		EPPT_SetOffsetY,
		EPPT_SetOffsetZ,
		EPPT_SetLayerStepX,
		EPPT_SetLayerStepY,
		EPPT_SetLayerStepZ,
		EPPT_End
	};


	enum EPrintOperFun
	{
		EPOF_Begin = EPPT_End + 1,
		EPOF_StartPrint,
		EPOF_StopPrint,
		EPOF_PausePrint,
		EPOF_ContinuePrint,
		EPOF_CleanPos,
		EPOF_GoHomnePos,
		EPOF_End
	};


	enum EPrintMoveOperFun
	{
		EPMOF_Begin = EPOF_End + 1,
		EPMOF_XAxisLMove,	//L为正方向，R为负方向
		EPMOF_XAxisRMove,
		EPMOF_YAxisLMove,
		EPMOF_YAxisRMove,
		EPMOF_ZAxisLMove,
		EPMOF_ZAxisRMove,
		EPMOF_End
	};


	enum EPrintSimulateOperFun
	{
		EPSOF_Begin = EPMOF_End + 1,
		EPSOF_StartPrt,	//L为正方向，R为负方向
		EPSOF_End
	};

	enum ShowEditType
	{
		ESET_OperComm = 0x01,		// 操作命令
		ESET_Sendomm = 0x02,		// 发送命令
		ESET_RecvComm = 0x03		// 接收命令
	};

	PrintDeviceUI(QWidget *parent = nullptr);


    ~PrintDeviceUI();


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

	/**
	*  @brief       模拟自动化运动流程测试
	*  @param[in]   cnt 打印层数
	*  @param[out]
	*  @return
	*/
	void SimulateLogic(int layerCnt = 2, int passCnt = 4);
	 
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

	/**
	*  @brief       模拟流程槽函数
	*  @param[in]   cnt 打印层数
	*  @param[out]
	*  @return
	*/
	void OnSimulateLogic(int idx);



signals:

	void SigAddShowLog(const QString& logStr);

	void SigShowOperComm(const QString& msg, const ShowEditType& type, QByteArray arr = QByteArray());
	
	void SigAddShowOperCmd(const QString& msg, QByteArray arr = QByteArray(), const ShowEditType& type = ShowEditType::ESET_Sendomm);


private:
	//sdk
	motionControlSDK* m_motionSDK;
	int m_layerIdx;
	int m_passIdx;


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
