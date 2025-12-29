#include "printDeviceMoudle.h"
#include <QtWidgets/QApplication>
#include "global.h"


int main(int argc, char *argv[])
{
	LOG_INFO("print_device_moudle_start");
	CLogManager::getInstance()->startLog("./");
	LOG_DEBUG(u8"软件启动");
    QApplication a(argc, argv);
    printDeviceMoudle w;
    w.show();
    return a.exec();
}
