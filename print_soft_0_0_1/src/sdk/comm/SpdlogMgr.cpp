#include "SpdlogMgr.h"

#include <iostream>
#include <filesystem>
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#ifdef _WIN32
	#include "spdlog/sinks/msvc_sink.h"
#endif

namespace fs = std::filesystem;

SpdlogWrapper::SpdlogWrapper()
	: m_defaultLogger(nullptr)
	, m_bInit(false)
{

}

SpdlogWrapper::~SpdlogWrapper()
{
	Shutdown();
}

//SpdlogWrapper* SpdlogWrapper::GetInstance()
//{
//	static SpdlogWrapper manager;
//	return &manager;
//}



bool SpdlogWrapper::Init(const std::string& logDir, 
	const std::string& logFileName /*= "app_log"*/, 
	size_t maxFileSize /*= 10 * 1024 * 1024*/, 
	size_t maxFiles /*= 10*/, 
	bool enableConsole /*= true*/, 
	bool enableVSOutput /*= true*/)
{
	if (m_bInit)
	{
		std::cout << "spdlogWrapper already initialized" << std::endl;
		return true;
	}

	try
	{
		m_logDir = logDir;
		if (!fs::exists(m_logDir))
		{
			fs::create_directories(m_logDir);
		}

		// 初始化异步日志线程池
		// 参数：队列大小，工作线程数
		spdlog::init_thread_pool(8192, 1);

		std::vector<spdlog::sink_ptr> sinks;
		// 文件sink
		std::string logFilePath = m_logDir + "/" + logFileName + ".log";
		auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFilePath, maxFileSize, maxFiles);
		fileSink->set_level(spdlog::level::trace);
		sinks.push_back(fileSink);

		// 控制台sink
		if (enableConsole)
		{
			auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			consoleSink->set_level(spdlog::level::debug);
			sinks.push_back(consoleSink);
		}

		// VS sink
#ifdef _WIN32
		if (enableVSOutput)
		{
			auto vsSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
			vsSink->set_level(spdlog::level::debug);
			sinks.push_back(vsSink);
		}
#endif

		// 创建日志器
		m_defaultLogger = std::make_shared<spdlog::async_logger>(
			"async_logger",
			sinks.begin(),
			sinks.end(),
			spdlog::thread_pool(),
			spdlog::async_overflow_policy::block);

		m_defaultLogger->set_level(spdlog::level::trace);
		// 日志格式: [时间] [线程ID] [级别] [文件:行号] 消息
		m_defaultLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%^%l%$] %v");
		spdlog::set_default_logger(m_defaultLogger);	
		
		// 设置自动刷新（每3秒或遇到warn及以上级别立即刷新）
		m_defaultLogger->flush_on(spdlog::level::warn);
		spdlog::flush_every(std::chrono::seconds(3));

		m_bInit = true;
		m_defaultLogger->info("================================================================================");
		m_defaultLogger->info("SpdlogWrapper initialized successfully!");
		m_defaultLogger->info("Log directory: {}", m_logDir);
		m_defaultLogger->info("Log file: {}", logFilePath);
		m_defaultLogger->info("Max file size: {} bytes", maxFileSize);
		m_defaultLogger->info("Max files: {}", maxFiles);
		m_defaultLogger->info("================================================================================");

		return true;
	}
	catch (const spdlog::spdlog_ex& e)
	{
		std::cerr << "Log initialization failed" << e.what() << std::endl;
		return false;
	}
}



bool SpdlogWrapper::InitDailyLogger(const std::string& logDir, 
	const std::string& logFileName /*= "daily_log"*/, 
	int hour /*= 0*/, 
	int minute /*= 0*/, 
	bool enableConsole /*= true*/, 
	bool enableVSOutput /*= true*/)
{
	if (m_bInit)
	{
		std::cout << "spdlogWrapper already initialized!" << std::endl;
		return true;
	}

	try
	{
		m_logDir = logDir;
		if (!fs::exists(m_logDir))
		{
			fs::create_directories(m_logDir);
		}

		// 异步线程池
		spdlog::init_thread_pool(8192, 1);
		std::vector<spdlog::sink_ptr> sinks;

		// 每日文件sink
		std::string logFilePath = m_logDir + "/" + logFileName + ".log";
		auto dailySink = std::make_shared<spdlog::sinks::daily_file_format_sink_mt>(logFilePath, hour, minute);
		dailySink->set_level(spdlog::level::trace);
		sinks.push_back(dailySink);

		// console sink
		if (enableConsole)
		{
			auto conSoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			conSoleSink->set_level(spdlog::level::debug);
			sinks.push_back(conSoleSink);
		}
		
		// VS Output sink 
#ifdef _WIN32
		if (enableVSOutput)
		{
			auto vsSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
			vsSink->set_level(spdlog::level::debug);
			sinks.push_back(vsSink);
		}
#endif
		// 创建日志器
		m_defaultLogger = std::make_shared<spdlog::async_logger>("daily_async_logger",
			sinks.begin(),
			sinks.end(),
			spdlog::thread_pool(),
			spdlog::async_overflow_policy::block);
		m_defaultLogger->set_level(spdlog::level::trace);
		m_defaultLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%^%l%$] %v");
		spdlog::set_default_logger(m_defaultLogger);
		m_defaultLogger->flush_on(spdlog::level::warn);
		spdlog::flush_every(std::chrono::seconds(3));
		m_bInit = true;

		m_defaultLogger->info("================================================================================");
		m_defaultLogger->info("SpdlogWrapper (Daily Mode) initialized successfully!");
		m_defaultLogger->info("Log directory: {}", m_logDir);
		m_defaultLogger->info("Daily rotation time: {:02d}:{:02d}", hour, minute);
		m_defaultLogger->info("================================================================================");

		return true;
	}
	catch (const spdlog::spdlog_ex& e)
	{
		std::cerr << "Log initialization failed: " << e.what() << std::endl;
		return false;
	}
}


void SpdlogWrapper::SetLogLevel(LogLevel level)
{
	if (m_defaultLogger)
	{
		m_defaultLogger->set_level(ConvertLogLevel(level));
	}
}

void SpdlogWrapper::SetPattern(const std::string& pattern)
{
	if (m_defaultLogger)
	{
		m_defaultLogger->set_pattern(pattern);
	}
}

std::shared_ptr<spdlog::logger> SpdlogWrapper::GetLogger()
{
	return m_defaultLogger;
}

std::shared_ptr<spdlog::logger> SpdlogWrapper::GetLogger(const std::string& name,
	const std::string& logDir /*= ""*/, 
	size_t maxFileSize /*= 10*1024*1024*/, 
	size_t maxFiles /*= 10*/)
{
	auto logger = spdlog::get(name);
	if (logger)
	{
		return logger;
	}

	try
	{
		std::string dir = logDir.empty() ? m_logDir : logDir;
		if (!fs::exists(dir))
		{
			fs::create_directories(dir);
		}

		std::string logFilePath = dir + "/" + name + ".log";
		auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFilePath, maxFileSize, maxFiles);
		auto newLogger = std::make_shared<spdlog::async_logger>(name, 
			fileSink, 
			spdlog::thread_pool(), 
			spdlog::async_overflow_policy::block);

		newLogger->set_level(spdlog::level::trace);
		newLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%^%l%$] %v");
		newLogger->flush_on(spdlog::level::warn);

		spdlog::register_logger(newLogger);
		if (m_defaultLogger)
		{
			m_defaultLogger->info("Create new logger: {}, file", name, logFilePath);
		}
		return newLogger;
	}
	catch (const spdlog::spdlog_ex& e)
	{
		std::cerr << "Failed to create logger: " << e.what() << std::endl;
		return nullptr;
	}

	

}

void SpdlogWrapper::Flush()
{
	if (m_defaultLogger)
	{
		m_defaultLogger->flush();
	}
	spdlog::apply_all([](std::shared_ptr<spdlog::logger> l) 
	{
		l->flush();
	});
}

void SpdlogWrapper::Shutdown()
{
	if (m_bInit)
	{
		if (m_defaultLogger)
		{
			m_defaultLogger->info("spdlogWrapper shutting down...");
		}

		Flush();
		spdlog::shutdown();
		m_bInit = false;
		m_defaultLogger.reset();
	}
}

void SpdlogWrapper::Trace(const char* file, int line, const char* func, const std::string& msg)
{
	if (m_defaultLogger)
	{
		m_defaultLogger->trace("[{}:{}][{}] {}", ExtractFileName(file), line, func, msg);
	}
}

void SpdlogWrapper::Debug(const char* file, int line, const char* func, const std::string& msg)
{
	if (m_defaultLogger)
	{
		m_defaultLogger->debug("[{}:{}][{}] {}", ExtractFileName(file), line, func, msg);
	}
}

void SpdlogWrapper::Info(const char* file, int line, const char* func, const std::string& msg)
{
	if (m_defaultLogger)
	{
		m_defaultLogger->info("[{}:{}][{}] {}", ExtractFileName(file), line, func, msg);
	}
}

void SpdlogWrapper::Warn(const char* file, int line, const char* func, const std::string& msg)
{
	if (m_defaultLogger)
	{
		m_defaultLogger->warn("[{}:{}][{}] {}", ExtractFileName(file), line, func, msg);
	}
}

void SpdlogWrapper::Error(const char* file, int line, const char* func, const std::string& msg)
{
	if (m_defaultLogger)
	{
		m_defaultLogger->error("[{}:{}][{}] {}", ExtractFileName(file), line, func, msg);
	}
}

void SpdlogWrapper::Critical(const char* file, int line, const char* func, const std::string& msg)
{
	if (m_defaultLogger)
	{
		m_defaultLogger->critical("[{}:{}][{}] {}", ExtractFileName(file), line, func, msg);
	}
}



const char* SpdlogWrapper::ExtractFileName(const char* filePath)
{
	if(!filePath)
	{
		return "";
	}
#ifdef _WIN32
	const char* fileName = strrchr(filePath, '\\');
#else
	const char* fileName = strrchr(fileName, '/);
#endif
		
	return fileName ? fileName + 1 : filePath;
}

spdlog::level::level_enum SpdlogWrapper::ConvertLogLevel(LogLevel level)
{
	switch (level)
	{
		case LogLevel::Trace:
		{
			return spdlog::level::trace;
		}
		case LogLevel::Debug:
		{
			return spdlog::level::trace;
		}
		case LogLevel::Info:
		{
			return spdlog::level::trace;
		}
		case LogLevel::Warn:
		{
			return spdlog::level::trace;
		}
		case LogLevel::Err:
		{
			return spdlog::level::trace;
		}
		case LogLevel::Critical:
		{
			return spdlog::level::critical;
		}
		case LogLevel::Off:
		{
			return spdlog::level::off;
		}
		default:
			return spdlog::level::info;
	}
}



