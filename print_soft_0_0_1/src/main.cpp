#include "printDeviceMoudle.h"
#include <QtWidgets/QApplication>
#include "global.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>


int main(int argc, char *argv[])
{
	//std::shared_ptr<spdlog::logger> my_logger = spdlog::basic_logger_mt("spdlog", "spdlog.txt");
	//// 设置日志格式. 参数含义: [日志标识符] [日期] [日志级别] [线程号] [数据]
	//my_logger->set_pattern("[%n][%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");
	//my_logger->set_level(spdlog::level::debug);
	//spdlog::flush_every(std::chrono::seconds(5)); // 定期刷新日志缓冲区

	//my_logger->trace("Welcome to trace spdlog!");
	//my_logger->debug("Welcome to debug spdlog!");
	//my_logger->info("Welcome to info spdlog!");
	//my_logger->error("Welcome to error spdlog!");

	LOG_INFO("print_device_moudle_start");
	CLogManager::getInstance()->startLog("./");
	LOG_DEBUG(u8"软件启动");
    QApplication a(argc, argv);
    printDeviceMoudle w;
    w.show();
    return a.exec();
}
