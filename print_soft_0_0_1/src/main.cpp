#include "printDeviceMoudle.h"
#include <QtWidgets/QApplication>
#include "SpdlogMgr.h"

int main(int argc, char *argv[])
{
	// spdlog初始化
	SpdlogWrapper::GetInstance()->Init("./logs", "print_soft", 10 * 1024 * 1024, 5, true, true);
	NAMED_LOG_I("print_soft", "print_soft_start log_moudle_init_finish log_start");

    QApplication a(argc, argv);
    printDeviceMoudle w;
    w.show();
	NAMED_LOG_I("print_soft", "print_soft_end");
    return a.exec();
}
