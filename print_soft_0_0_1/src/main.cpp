#include "printDeviceMoudle.h"
#include <QtWidgets/QApplication>
#include "global.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>


int main(int argc, char *argv[])
{
	// 参数1 日志标识符, 参数2 日志文件名
	std::shared_ptr<spdlog::logger> mylogger = spdlog::basic_logger_mt("spdlog", "spdlog.log");
	// 设置日志格式. 参数含义: [日志标识符] [日期] [日志级别] [线程号] [数据]
	mylogger->set_pattern("[%n][%Y-%m-%d %H:%M:%S.%e] [%l] [%t]  %v");
	mylogger->set_level(spdlog::level::debug);
	spdlog::flush_every(std::chrono::seconds(5)); // 定期刷新日志缓冲区

	mylogger->trace("Welcome to info spdlog!");
	mylogger->debug("Welcome to info spdlog!");
	mylogger->info("Welcome to info spdlog!");
	mylogger->warn("Welcome to info spdlog!");
	mylogger->error("Welcome to info spdlog!");
	mylogger->critical("Welcome to info spdlog!");


	LOG_INFO("print_device_moudle_start");
	CLogManager::getInstance()->startLog("./");
	LOG_DEBUG(u8"软件启动");
    QApplication a(argc, argv);
    printDeviceMoudle w;
    w.show();
    return a.exec();
}
