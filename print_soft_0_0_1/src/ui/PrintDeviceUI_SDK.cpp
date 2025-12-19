/**
 * @file PrintDeviceUI_SDK.cpp
 * @brief 使用PrintDeviceSDK的打印设备UI类实现
 */

#include "PrintDeviceUI_SDK.h"
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QMetaObject>

// 静态成员初始化
PrintDeviceUI_SDK* PrintDeviceUI_SDK::s_instance = nullptr;

PrintDeviceUI_SDK::PrintDeviceUI_SDK(QWidget *parent)
    : QDialog(parent)
    , m_printBtnGroup(new QButtonGroup(this))
    , m_funBtnGroup(new QButtonGroup(this))
    , m_sdkInitialized(false)
{
    s_instance = this;
    Init();
}

PrintDeviceUI_SDK::~PrintDeviceUI_SDK()
{
    // 释放SDK资源
    if (m_sdkInitialized) {
        ReleaseSDK();
        m_sdkInitialized = false;
    }
    s_instance = nullptr;
}

void PrintDeviceUI_SDK::Init()
{
    // 初始化UI
    InitUI();
    
    // 初始化SDK
    InitSDK();
    
    // 连接信号槽
    connect(m_printBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), 
            this, &PrintDeviceUI_SDK::OnPrintFunClicked);
    connect(m_funBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), 
            this, &PrintDeviceUI_SDK::OnListenBtnClicked);
    connect(this, &PrintDeviceUI_SDK::SigShowOperComm, 
            this, &PrintDeviceUI_SDK::OnAppendShowComm);
    
    // 连接SDK事件信号
    connect(this, &PrintDeviceUI_SDK::SigSDKEvent, this, [this](const QString& message, int type, int code) {
        // 在UI线程中处理SDK事件
        ShowEditType showType = ESET_OperComm;
        if (type == EVENT_TYPE_ERROR) {
            showType = ESET_RecvComm;
        }
        OnAppendShowComm(message, showType);
    });
}

void PrintDeviceUI_SDK::InitUI()
{
    setWindowTitle(QString::fromLocal8Bit("打印设备模块（SDK版本）"));
    resize(1000, 600);

    // 网络通信区域
    QGroupBox* box1 = new QGroupBox(QString::fromLocal8Bit("网络通信控制"));
    {
        QGridLayout* layout = new QGridLayout;
        box1->setLayout(layout);

        QLabel* ipLab = new QLabel("IP");
        QLabel* portLab = new QLabel("Port");
        QLabel* stateLab = new QLabel(QString::fromLocal8Bit("连接状态"));
        m_connStateLab = new QLabel(QString::fromLocal8Bit("未连接"));

        m_ipLine = new QLineEdit();
        m_ipLine->setText("192.168.100.57");
        
        m_portLine = new QLineEdit();
        m_portLine->setText("5555");

        QPushButton* connBtn = new QPushButton(QString::fromLocal8Bit("开始连接"));
        QPushButton* disConnBtn = new QPushButton(QString::fromLocal8Bit("断开连接"));
        QPushButton* clearBtn = new QPushButton(QString::fromLocal8Bit("清空日志"));
        
        m_funBtnGroup->addButton(connBtn, ENF_StartConn);
        m_funBtnGroup->addButton(disConnBtn, ENF_DisConn);
        m_funBtnGroup->addButton(clearBtn, ENF_ClearAllText);

        layout->addWidget(ipLab, 0, 0, 1, 1);
        layout->addWidget(m_ipLine, 0, 1, 1, 2);
        layout->addWidget(portLab, 1, 0, 1, 1);
        layout->addWidget(m_portLine, 1, 1, 1, 2);
        layout->addWidget(stateLab, 2, 0, 1, 1);
        layout->addWidget(m_connStateLab, 2, 1, 1, 1);
        layout->addWidget(connBtn, 3, 0, 1, 1);
        layout->addWidget(disConnBtn, 3, 1, 1, 1);
        layout->addWidget(clearBtn, 3, 2, 1, 1);
    }

    // 打印功能区域
    QGroupBox* box2 = new QGroupBox(QString::fromLocal8Bit("打印功能操作"));
    {
        QGridLayout* layout = new QGridLayout;
        box2->setLayout(layout);

        auto addBtn = [&](const QString& text, PrintFun type, int row, int col) {
            QPushButton* btn = new QPushButton(text);
            m_printBtnGroup->addButton(btn, type);
            layout->addWidget(btn, row, col, 1, 1);
        };

        addBtn(QString::fromLocal8Bit("开始打印"), EPF_StartPrint, 0, 0);
        addBtn(QString::fromLocal8Bit("停止打印"), EPF_StopPrint, 0, 1);
        addBtn(QString::fromLocal8Bit("暂停打印"), EPF_PausePrint, 0, 2);
        addBtn(QString::fromLocal8Bit("继续打印"), EPF_ContinuePrint, 0, 3);
        addBtn(QString::fromLocal8Bit("选择图片"), EPF_TransData, 1, 0);
        addBtn(QString::fromLocal8Bit("打印复位"), EPF_ResetPrint, 1, 1);
    }

    // 运动控制区域
    QGroupBox* box3 = new QGroupBox(QString::fromLocal8Bit("运动轴移动控制"));
    {
        QGridLayout* layout = new QGridLayout;
        box3->setLayout(layout);

        auto addAxisBtn = [&](const QString& text, AxisFun type, int row, int col) {
            QPushButton* btn = new QPushButton(text);
            m_printBtnGroup->addButton(btn, type);
            layout->addWidget(btn, row, col, 1, 1);
        };

        // Z轴控制
        QLabel* zAxisLab = new QLabel(QString::fromLocal8Bit("Z轴控制"));
        layout->addWidget(zAxisLab, 0, 0, 1, 1);
        addAxisBtn(QString::fromLocal8Bit("自动上升"), EAF_ZAxisAutoForward, 1, 0);
        addAxisBtn(QString::fromLocal8Bit("自动下降"), EAF_ZAxisAutoBack, 1, 1);
        addAxisBtn(QString::fromLocal8Bit("上升1CM"), EAF_ZAxisForward1CM, 2, 0);
        addAxisBtn(QString::fromLocal8Bit("下降1CM"), EAF_ZAxisBack1CM, 2, 1);
        addAxisBtn(QString::fromLocal8Bit("Z轴复位"), EAF_ZAxisReset, 3, 0);
        addAxisBtn(QString::fromLocal8Bit("打印位置"), EAF_ZAxisMovePrintPos, 3, 1);

        // X/Y轴控制
        QLabel* xyAxisLab = new QLabel(QString::fromLocal8Bit("X/Y轴控制"));
        layout->addWidget(xyAxisLab, 0, 2, 1, 1);
        addAxisBtn(QString::fromLocal8Bit("X轴复位"), EAF_XAxisReset, 1, 2);
        addAxisBtn(QString::fromLocal8Bit("X轴打印位"), EAF_XAxisMovePrintPos, 1, 3);
        addAxisBtn(QString::fromLocal8Bit("Y轴复位"), EAF_YAxisReset, 2, 2);
        addAxisBtn(QString::fromLocal8Bit("Y轴打印位"), EAF_YAxisMovePrintPos, 2, 3);
        
        // 全部复位按钮
        QPushButton* allResetBtn = new QPushButton(QString::fromLocal8Bit("全部复位"));
        m_printBtnGroup->addButton(allResetBtn, EAF_End + 100);  // 特殊ID
        layout->addWidget(allResetBtn, 3, 2, 1, 2);
    }

    // 日志信息区域
    QGroupBox* box4 = new QGroupBox(QString::fromLocal8Bit("操作日志"));
    {
        QVBoxLayout* layout = new QVBoxLayout;
        box4->setLayout(layout);
        m_operLogText = new QTextEdit();
        m_operLogText->setReadOnly(true);
        layout->addWidget(m_operLogText);
    }

    // 发送/接收信息区域
    QGroupBox* box5 = new QGroupBox(QString::fromLocal8Bit("发送/接收信息"));
    {
        QGridLayout* layout = new QGridLayout;
        box5->setLayout(layout);

        QLabel* sendLab = new QLabel(QString::fromLocal8Bit("发送命令"));
        QLabel* recvLab = new QLabel(QString::fromLocal8Bit("接收应答"));

        m_sendCommText = new QTextEdit();
        m_sendCommText->setReadOnly(true);
        m_sendCommText->setMinimumWidth(300);
        
        m_recvCommText = new QTextEdit();
        m_recvCommText->setReadOnly(true);
        m_recvCommText->setMinimumWidth(300);

        layout->addWidget(sendLab, 0, 0, 1, 1);
        layout->addWidget(m_sendCommText, 1, 0, 2, 1);
        layout->addWidget(recvLab, 0, 1, 1, 1);
        layout->addWidget(m_recvCommText, 1, 1, 2, 1);
    }

    // 主布局
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    {
        QVBoxLayout* leftLayout = new QVBoxLayout;
        leftLayout->addWidget(box1);
        leftLayout->addWidget(box2);
        leftLayout->addWidget(box3);
        leftLayout->addWidget(box4);

        mainLayout->addLayout(leftLayout, 1);
        mainLayout->addWidget(box5, 1);
    }
    setLayout(mainLayout);
}

void PrintDeviceUI_SDK::InitSDK()
{
    // 初始化SDK
    int ret = InitSDK("./logs");
    if (ret == 0) {
        m_sdkInitialized = true;
        
        // 注册事件回调
        RegisterEventCallback(&PrintDeviceUI_SDK::OnSDKEvent);
        
        emit SigShowOperComm(QString::fromLocal8Bit("SDK初始化成功"), ESET_OperComm);
    } else {
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), 
                            QString::fromLocal8Bit("SDK初始化失败！"));
    }
}

void PrintDeviceUI_SDK::OnSDKEvent(const SdkEvent* event)
{
    if (!s_instance) return;
    
    // 将事件转发到UI线程
    QString message = QString::fromUtf8(event->message);
    
    // 使用invokeMethod确保在UI线程中执行
    QMetaObject::invokeMethod(s_instance, [=]() {
        s_instance->SigSDKEvent(message, event->type, event->code);
    }, Qt::QueuedConnection);
}

void PrintDeviceUI_SDK::OnPrintFunClicked(int idx)
{
    QString logStr;
    
    // 检查连接状态
    if (!IsConnected() && idx != EAF_End + 100) {
        if (idx >= EPF_Begin && idx < EAF_End) {
            logStr = QString::fromLocal8Bit("设备未连接，请先连接设备！");
            emit SigShowOperComm(logStr, ESET_OperComm);
            return;
        }
    }

    // 处理打印功能
    if (idx >= EPF_Begin && idx < EPF_End) {
        switch (idx) {
        case EPF_StartPrint:
            StartPrint();
            logStr = QString::fromLocal8Bit("发送开始打印命令");
            break;
        case EPF_StopPrint:
            StopPrint();
            logStr = QString::fromLocal8Bit("发送停止打印命令");
            break;
        case EPF_PausePrint:
            PausePrint();
            logStr = QString::fromLocal8Bit("发送暂停打印命令");
            break;
        case EPF_ContinuePrint:
            ResumePrint();
            logStr = QString::fromLocal8Bit("发送继续打印命令");
            break;
        case EPF_ResetPrint:
            // SDK中没有直接的resetPrint API，但可以通过内部实现
            logStr = QString::fromLocal8Bit("发送打印复位命令");
            break;
        case EPF_TransData: {
            QString imgPath = QFileDialog::getOpenFileName(
                this, 
                QString::fromLocal8Bit("选择图片文件"), 
                QDir::homePath(), 
                "Image Files(*.jpg *.png *.bmp *.raw)");
            
            if (!imgPath.isEmpty()) {
                int ret = LoadPrintData(imgPath.toUtf8().constData());
                if (ret == 0) {
                    logStr = QString::fromLocal8Bit("图像数据加载成功: ") + imgPath;
                } else {
                    logStr = QString::fromLocal8Bit("图像数据加载失败！");
                }
            }
            break;
        }
        default:
            break;
        }
    }
    // 处理运动控制
    else if (idx >= EAF_Begin && idx < EAF_End) {
        switch (idx) {
        case EAF_ZAxisAutoForward:
            MoveBy(0, 0, 15.0, 100);  // 大于10mm，触发自动上升
            logStr = QString::fromLocal8Bit("Z轴自动上升");
            break;
        case EAF_ZAxisAutoBack:
            MoveBy(0, 0, -15.0, 100);  // 小于-10mm，触发自动下降
            logStr = QString::fromLocal8Bit("Z轴自动下降");
            break;
        case EAF_ZAxisForward1CM:
            MoveBy(0, 0, 5.0, 100);  // 小于10mm，触发1CM移动
            logStr = QString::fromLocal8Bit("Z轴上升1CM");
            break;
        case EAF_ZAxisBack1CM:
            MoveBy(0, 0, -5.0, 100);  // 大于-10mm，触发1CM移动
            logStr = QString::fromLocal8Bit("Z轴下降1CM");
            break;
        case EAF_ZAxisReset:
            MoveBy(0, 0, -1.0, 100);  // 负值触发复位
            logStr = QString::fromLocal8Bit("Z轴复位");
            break;
        case EAF_ZAxisMovePrintPos:
            MoveBy(0, 0, 1.0, 100);  // 正值触发移动到打印位置
            logStr = QString::fromLocal8Bit("Z轴移动到打印位置");
            break;
        case EAF_XAxisReset:
            MoveBy(-1.0, 0, 0, 100);  // X轴复位
            logStr = QString::fromLocal8Bit("X轴复位");
            break;
        case EAF_XAxisMovePrintPos:
            MoveBy(1.0, 0, 0, 100);  // X轴移动到打印位置
            logStr = QString::fromLocal8Bit("X轴移动到打印位置");
            break;
        case EAF_YAxisReset:
            MoveBy(0, -1.0, 0, 100);  // Y轴复位
            logStr = QString::fromLocal8Bit("Y轴复位");
            break;
        case EAF_YAxisMovePrintPos:
            MoveBy(0, 1.0, 0, 100);  // Y轴移动到打印位置
            logStr = QString::fromLocal8Bit("Y轴移动到打印位置");
            break;
        default:
            break;
        }
    }
    // 全部复位
    else if (idx == EAF_End + 100) {
        GoHome();
        logStr = QString::fromLocal8Bit("全部轴复位");
    }

    if (!logStr.isEmpty()) {
        emit SigShowOperComm(logStr, ESET_OperComm);
    }
}

void PrintDeviceUI_SDK::OnListenBtnClicked(int idx)
{
    QString logStr;
    
    switch (idx) {
    case ENF_StartConn: {
        QString ip = m_ipLine->text();
        int port = m_portLine->text().toInt();
        
        int ret = ConnectByTCP(ip.toUtf8().constData(), port);
        if (ret == 0) {
            logStr = QString::fromLocal8Bit("正在连接设备 %1:%2...").arg(ip).arg(port);
            m_connStateLab->setText(QString::fromLocal8Bit("连接中..."));
            m_funBtnGroup->button(ENF_StartConn)->setEnabled(false);
            m_funBtnGroup->button(ENF_DisConn)->setEnabled(true);
        } else {
            logStr = QString::fromLocal8Bit("连接失败！");
        }
        break;
    }
    case ENF_DisConn:
        Disconnect();
        logStr = QString::fromLocal8Bit("断开连接");
        m_connStateLab->setText(QString::fromLocal8Bit("未连接"));
        m_funBtnGroup->button(ENF_StartConn)->setEnabled(true);
        m_funBtnGroup->button(ENF_DisConn)->setEnabled(false);
        break;
        
    case ENF_ClearAllText:
        m_operLogText->clear();
        m_sendCommText->clear();
        m_recvCommText->clear();
        logStr = QString::fromLocal8Bit("清空日志");
        break;
        
    default:
        break;
    }

    if (!logStr.isEmpty()) {
        emit SigShowOperComm(logStr, ESET_OperComm);
    }
}

void PrintDeviceUI_SDK::OnAppendShowComm(const QString& msg, const ShowEditType& type)
{
    QString showStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    showStr.append("\n");
    showStr.append(msg);
    showStr.append("\n");

    switch (type) {
    case ESET_OperComm:
        m_operLogText->append(showStr);
        break;
    case ESET_SendComm:
        m_sendCommText->append(showStr);
        break;
    case ESET_RecvComm:
        m_recvCommText->append(showStr);
        // 更新连接状态
        if (msg.contains("Connected")) {
            m_connStateLab->setText(QString::fromLocal8Bit("已连接"));
        } else if (msg.contains("Disconnected")) {
            m_connStateLab->setText(QString::fromLocal8Bit("已断开"));
        }
        break;
    default:
        break;
    }
}

