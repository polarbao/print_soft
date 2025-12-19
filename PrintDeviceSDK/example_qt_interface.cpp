/**
 * @file example_qt_interface.cpp
 * @brief PrintDeviceSDK Qt接口使用示例
 * 
 * 这是一个完整的示例，展示如何使用Qt接口控制打印设备
 */

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QTextEdit>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>

#include "PrintDeviceController.h"

/**
 * @class PrintDevicePanel
 * @brief 打印设备控制面板
 */
class PrintDevicePanel : public QWidget
{
    Q_OBJECT

public:
    explicit PrintDevicePanel(QWidget *parent = nullptr) 
        : QWidget(parent)
    {
        setupUI();
        setupController();
        
        // 初始化SDK
        if (!m_controller->initialize("./logs")) {
            QMessageBox::critical(this, "错误", "SDK初始化失败");
        }
    }

private:
    void setupUI()
    {
        setWindowTitle("PrintDevice控制面板 - Qt接口示例");
        resize(800, 600);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        
        // ========== 连接区域 ==========
        QGroupBox* connGroup = new QGroupBox("连接设置");
        QGridLayout* connLayout = new QGridLayout(connGroup);
        
        m_statusLabel = new QLabel("❌ 未连接");
        m_statusLabel->setStyleSheet("font-size: 14pt; font-weight: bold; color: red;");
        
        m_ipEdit = new QLineEdit("192.168.100.57");
        m_portEdit = new QLineEdit("5555");
        m_connectBtn = new QPushButton("连接设备");
        
        connLayout->addWidget(new QLabel("状态:"), 0, 0);
        connLayout->addWidget(m_statusLabel, 0, 1, 1, 2);
        connLayout->addWidget(new QLabel("IP地址:"), 1, 0);
        connLayout->addWidget(m_ipEdit, 1, 1);
        connLayout->addWidget(new QLabel("端口:"), 1, 2);
        connLayout->addWidget(m_portEdit, 1, 3);
        connLayout->addWidget(m_connectBtn, 2, 0, 1, 4);
        
        mainLayout->addWidget(connGroup);
        
        // ========== 打印控制区域 ==========
        QGroupBox* printGroup = new QGroupBox("打印控制");
        QGridLayout* printLayout = new QGridLayout(printGroup);
        
        m_loadImageBtn = new QPushButton("📁 加载图像");
        m_startPrintBtn = new QPushButton("▶️ 开始打印");
        m_pausePrintBtn = new QPushButton("⏸️ 暂停打印");
        m_stopPrintBtn = new QPushButton("⏹️ 停止打印");
        
        m_progressBar = new QProgressBar();
        m_progressBar->setFormat("进度: %p% - 准备中");
        
        printLayout->addWidget(m_loadImageBtn, 0, 0);
        printLayout->addWidget(m_startPrintBtn, 0, 1);
        printLayout->addWidget(m_pausePrintBtn, 0, 2);
        printLayout->addWidget(m_stopPrintBtn, 0, 3);
        printLayout->addWidget(new QLabel("打印进度:"), 1, 0);
        printLayout->addWidget(m_progressBar, 1, 1, 1, 3);
        
        // 初始状态：打印按钮禁用
        m_loadImageBtn->setEnabled(false);
        m_startPrintBtn->setEnabled(false);
        m_pausePrintBtn->setEnabled(false);
        m_stopPrintBtn->setEnabled(false);
        
        mainLayout->addWidget(printGroup);
        
        // ========== 运动控制区域 ==========
        QGroupBox* moveGroup = new QGroupBox("运动控制");
        QGridLayout* moveLayout = new QGridLayout(moveGroup);
        
        m_homeBtn = new QPushButton("🏠 回原点");
        m_xPlusBtn = new QPushButton("X+ (10mm)");
        m_xMinusBtn = new QPushButton("X- (10mm)");
        m_yPlusBtn = new QPushButton("Y+ (10mm)");
        m_yMinusBtn = new QPushButton("Y- (10mm)");
        m_zPlusBtn = new QPushButton("Z+ (5mm)");
        m_zMinusBtn = new QPushButton("Z- (5mm)");
        
        moveLayout->addWidget(m_homeBtn, 0, 0, 1, 3);
        moveLayout->addWidget(new QLabel("X轴:"), 1, 0);
        moveLayout->addWidget(m_xPlusBtn, 1, 1);
        moveLayout->addWidget(m_xMinusBtn, 1, 2);
        moveLayout->addWidget(new QLabel("Y轴:"), 2, 0);
        moveLayout->addWidget(m_yPlusBtn, 2, 1);
        moveLayout->addWidget(m_yMinusBtn, 2, 2);
        moveLayout->addWidget(new QLabel("Z轴:"), 3, 0);
        moveLayout->addWidget(m_zPlusBtn, 3, 1);
        moveLayout->addWidget(m_zMinusBtn, 3, 2);
        
        // 初始状态：运动按钮禁用
        m_homeBtn->setEnabled(false);
        m_xPlusBtn->setEnabled(false);
        m_xMinusBtn->setEnabled(false);
        m_yPlusBtn->setEnabled(false);
        m_yMinusBtn->setEnabled(false);
        m_zPlusBtn->setEnabled(false);
        m_zMinusBtn->setEnabled(false);
        
        mainLayout->addWidget(moveGroup);
        
        // ========== 日志区域 ==========
        QGroupBox* logGroup = new QGroupBox("运行日志");
        QVBoxLayout* logLayout = new QVBoxLayout(logGroup);
        
        m_logText = new QTextEdit();
        m_logText->setReadOnly(true);
        m_logText->setMaximumHeight(150);
        
        QPushButton* clearLogBtn = new QPushButton("清除日志");
        connect(clearLogBtn, &QPushButton::clicked, m_logText, &QTextEdit::clear);
        
        logLayout->addWidget(m_logText);
        logLayout->addWidget(clearLogBtn);
        
        mainLayout->addWidget(logGroup);
        
        // ========== 连接按钮槽函数 ==========
        connect(m_connectBtn, &QPushButton::clicked, this, &PrintDevicePanel::onConnect);
        connect(m_loadImageBtn, &QPushButton::clicked, this, &PrintDevicePanel::onLoadImage);
        connect(m_startPrintBtn, &QPushButton::clicked, this, &PrintDevicePanel::onStartPrint);
        connect(m_pausePrintBtn, &QPushButton::clicked, this, &PrintDevicePanel::onPausePrint);
        connect(m_stopPrintBtn, &QPushButton::clicked, this, &PrintDevicePanel::onStopPrint);
        connect(m_homeBtn, &QPushButton::clicked, this, &PrintDevicePanel::onHome);
        connect(m_xPlusBtn, &QPushButton::clicked, [this]() { m_controller->moveXAxis(10); });
        connect(m_xMinusBtn, &QPushButton::clicked, [this]() { m_controller->moveXAxis(-10); });
        connect(m_yPlusBtn, &QPushButton::clicked, [this]() { m_controller->moveYAxis(10); });
        connect(m_yMinusBtn, &QPushButton::clicked, [this]() { m_controller->moveYAxis(-10); });
        connect(m_zPlusBtn, &QPushButton::clicked, [this]() { m_controller->moveZAxis(5); });
        connect(m_zMinusBtn, &QPushButton::clicked, [this]() { m_controller->moveZAxis(-5); });
    }
    
    void setupController()
    {
        m_controller = new PrintDeviceController(this);
        
        // ========== 连接SDK信号 ==========
        
        // 连接成功
        connect(m_controller, &PrintDeviceController::connected, this, [this]() {
            m_statusLabel->setText("✅ 已连接");
            m_statusLabel->setStyleSheet("font-size: 14pt; font-weight: bold; color: green;");
            m_connectBtn->setText("断开连接");
            
            // 启用功能按钮
            m_loadImageBtn->setEnabled(true);
            m_homeBtn->setEnabled(true);
            enableMoveButtons(true);
            
            appendLog("✅ 设备连接成功", Qt::darkGreen);
        });
        
        // 连接断开
        connect(m_controller, &PrintDeviceController::disconnected, this, [this]() {
            m_statusLabel->setText("❌ 未连接");
            m_statusLabel->setStyleSheet("font-size: 14pt; font-weight: bold; color: red;");
            m_connectBtn->setText("连接设备");
            
            // 禁用功能按钮
            m_loadImageBtn->setEnabled(false);
            m_startPrintBtn->setEnabled(false);
            m_pausePrintBtn->setEnabled(false);
            m_stopPrintBtn->setEnabled(false);
            m_homeBtn->setEnabled(false);
            enableMoveButtons(false);
            
            appendLog("❌ 设备连接断开", Qt::red);
        });
        
        // 错误处理
        connect(m_controller, &PrintDeviceController::errorOccurred, 
                this, [this](int code, const QString& msg) {
            QString errorMsg = QString("❌ 错误 [%1]: %2").arg(code).arg(msg);
            appendLog(errorMsg, Qt::red);
            QMessageBox::warning(this, "错误", msg);
        });
        
        // 一般信息
        connect(m_controller, &PrintDeviceController::infoMessage, 
                this, [this](const QString& msg) {
            appendLog("ℹ️ " + msg, Qt::blue);
        });
        
        // 打印进度更新
        connect(m_controller, &PrintDeviceController::printProgressUpdated,
                this, [this](int progress, int current, int total) {
            m_progressBar->setValue(progress);
            m_progressBar->setFormat(QString("进度: %1% - 第%2/%3层")
                .arg(progress).arg(current).arg(total));
            
            appendLog(QString("📊 打印进度: %1% (%2/%3层)")
                .arg(progress).arg(current).arg(total), Qt::darkCyan);
        });
        
        // 打印状态改变
        connect(m_controller, &PrintDeviceController::printStatusChanged,
                this, [this](const QString& status) {
            appendLog("🖨️ 打印状态: " + status, Qt::darkMagenta);
        });
        
        // 运动状态改变
        connect(m_controller, &PrintDeviceController::moveStatusChanged,
                this, [this](const QString& status) {
            appendLog("🔄 运动状态: " + status, Qt::darkYellow);
        });
        
        // 位置更新
        connect(m_controller, &PrintDeviceController::positionUpdated,
                this, [this](double x, double y, double z) {
            appendLog(QString("📍 位置: X=%.2f, Y=%.2f, Z=%.2f").arg(x).arg(y).arg(z), 
                     Qt::darkGray);
        });
        
        // 日志消息
        connect(m_controller, &PrintDeviceController::logMessage,
                this, [this](const QString& msg) {
            appendLog("📝 " + msg, Qt::gray);
        });
    }
    
    void enableMoveButtons(bool enable)
    {
        m_xPlusBtn->setEnabled(enable);
        m_xMinusBtn->setEnabled(enable);
        m_yPlusBtn->setEnabled(enable);
        m_yMinusBtn->setEnabled(enable);
        m_zPlusBtn->setEnabled(enable);
        m_zMinusBtn->setEnabled(enable);
    }
    
    void appendLog(const QString& message, const QColor& color = Qt::black)
    {
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        QString colorName = color.name();
        
        m_logText->append(QString("<font color='%1'>[%2] %3</font>")
            .arg(colorName).arg(timestamp).arg(message));
        
        // 自动滚动到底部
        QTextCursor cursor = m_logText->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_logText->setTextCursor(cursor);
    }

private slots:
    void onConnect()
    {
        if (m_controller->isConnected()) {
            // 断开连接
            m_controller->disconnectFromDevice();
        } else {
            // 连接设备
            QString ip = m_ipEdit->text();
            quint16 port = m_portEdit->text().toUShort();
            
            if (ip.isEmpty()) {
                QMessageBox::warning(this, "提示", "请输入IP地址");
                return;
            }
            
            appendLog(QString("🔌 尝试连接到 %1:%2...").arg(ip).arg(port), Qt::blue);
            m_controller->connectToDevice(ip, port);
        }
    }
    
    void onLoadImage()
    {
        QString filePath = QFileDialog::getOpenFileName(
            this, 
            "选择打印图像", 
            QDir::homePath(),
            "图像文件 (*.jpg *.png *.bmp);;所有文件 (*.*)"
        );
        
        if (filePath.isEmpty()) {
            return;
        }
        
        appendLog("📁 加载图像: " + filePath, Qt::blue);
        
        if (m_controller->loadPrintData(filePath)) {
            m_currentImagePath = filePath;
            m_startPrintBtn->setEnabled(true);
            QMessageBox::information(this, "成功", "图像数据已加载");
        } else {
            QMessageBox::warning(this, "错误", "加载图像数据失败");
        }
    }
    
    void onStartPrint()
    {
        if (m_currentImagePath.isEmpty()) {
            QMessageBox::warning(this, "提示", "请先加载图像数据");
            return;
        }
        
        if (m_controller->startPrint()) {
            m_startPrintBtn->setEnabled(false);
            m_pausePrintBtn->setEnabled(true);
            m_stopPrintBtn->setEnabled(true);
            m_progressBar->setValue(0);
            appendLog("▶️ 开始打印...", Qt::darkGreen);
        }
    }
    
    void onPausePrint()
    {
        if (m_pausePrintBtn->text() == "⏸️ 暂停打印") {
            if (m_controller->pausePrint()) {
                m_pausePrintBtn->setText("▶️ 继续打印");
                appendLog("⏸️ 打印已暂停", Qt::darkYellow);
            }
        } else {
            if (m_controller->resumePrint()) {
                m_pausePrintBtn->setText("⏸️ 暂停打印");
                appendLog("▶️ 打印已继续", Qt::darkGreen);
            }
        }
    }
    
    void onStopPrint()
    {
        if (m_controller->stopPrint()) {
            m_startPrintBtn->setEnabled(true);
            m_pausePrintBtn->setEnabled(false);
            m_pausePrintBtn->setText("⏸️ 暂停打印");
            m_stopPrintBtn->setEnabled(false);
            m_progressBar->setValue(0);
            m_progressBar->setFormat("进度: %p% - 已停止");
            appendLog("⏹️ 打印已停止", Qt::red);
        }
    }
    
    void onHome()
    {
        if (m_controller->goHome()) {
            appendLog("🏠 正在回原点...", Qt::darkCyan);
        }
    }

private:
    PrintDeviceController* m_controller;
    
    // UI控件
    QLabel* m_statusLabel;
    QLineEdit* m_ipEdit;
    QLineEdit* m_portEdit;
    QPushButton* m_connectBtn;
    
    QPushButton* m_loadImageBtn;
    QPushButton* m_startPrintBtn;
    QPushButton* m_pausePrintBtn;
    QPushButton* m_stopPrintBtn;
    QProgressBar* m_progressBar;
    
    QPushButton* m_homeBtn;
    QPushButton* m_xPlusBtn;
    QPushButton* m_xMinusBtn;
    QPushButton* m_yPlusBtn;
    QPushButton* m_yMinusBtn;
    QPushButton* m_zPlusBtn;
    QPushButton* m_zMinusBtn;
    
    QTextEdit* m_logText;
    
    QString m_currentImagePath;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用信息
    app.setApplicationName("PrintDevice Controller");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Your Company");
    
    // 创建并显示主窗口
    PrintDevicePanel panel;
    panel.show();
    
    return app.exec();
}

#include "example_qt_interface.moc"

