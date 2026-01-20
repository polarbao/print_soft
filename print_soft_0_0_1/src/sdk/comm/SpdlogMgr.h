#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <functional>
#include <regex>

#include <memory>
#include <atomic>
#include <mutex>
#include <iomanip>

#include "SpdlogQtDataFormat.h"
#include "comm/CNewSingleton.h"

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/msvc_sink.h"



// 格式化字符串
template<typename... Args>
inline std::string FormatString(Args&&... args)
{
	std::ostringstream oss;
	(oss << ... << args);
	return oss.str();
}

class SpdlogWrapper //: public CNewSingleton<SpdlogWrapper>
{

public:
	enum class LogLevel
	{
		Trace = 0,
		Debug = 1,
		Info = 2,
		Warn = 3,
		Err = 4,
		Critical = 5,
		Off = 6
	};


	static SpdlogWrapper* GetInstance();

	bool Init(const std::string& logDir,
		const std::string& logFileName = "app_log",
		size_t maFileSize = 10 * 1024 * 1024,
		size_t maxFiles = 10,
		bool enableConsole = true,
		bool enableVSOutput = true);

	/**
	 * @brief 初始化每日日志文件模式
	 * @param logDir 日志文件存储目录
	 * @param logFileName 日志文件名前缀
	 * @param hour 每日切换日志的小时（0-23）
	 * @param minute 每日切换日志的分钟（0-59）
	 * @param enableConsole 是否启用控制台输出
	 * @param enableVSOutput 是否启用Visual Studio调试输出
	 * @return true-成功，false-失败
	 */
	bool InitDailyLogger(const std::string& logDir,
		const std::string& logFileName = "daily_log",
		int hour = 0,
		int minute = 0,
		bool enableConsole = true,
		bool enableVSOutput = true);


	void SetLogLevel(LogLevel level);

	void SetPattern(const std::string& pattern);

	std::shared_ptr<spdlog::logger> GetLogger();

	std::shared_ptr<spdlog::logger> GetLogger(
		const std::string& name,
		const std::string& logDir = "",
		size_t maxFileSize = 10*1024*1024,
		size_t maxFiles = 10);

	/**
	 * @brief 手动刷新日志缓冲区
	 */
	void Flush();
	
	/**
	 * @brief 关闭日志系统
	 */
	void Shutdown();

	// 便捷的日志输出宏使用的实际函数
	void Trace(const char* file, int line, const char* func, const std::string& msg);
	void Debug(const char* file, int line, const char* func, const std::string& msg);
	void Info(const char* file, int line, const char* func, const std::string& msg);
	void Warn(const char* file, int line, const char* func, const std::string& msg);
	void Error(const char* file, int line, const char* func, const std::string& msg);
	void Critical(const char* file, int line, const char* func, const std::string& msg);



private:
	//friend class CSingleton<SpdlogWrapper>;
	SpdlogWrapper();
	~SpdlogWrapper();
	SpdlogWrapper(const SpdlogWrapper&) = delete;
	SpdlogWrapper& operator=(const SpdlogWrapper&) = delete;

	const char* ExtractFileName(const char* filePath);

	spdlog::level::level_enum ConvertLogLevel(LogLevel level);

private:
	static SpdlogWrapper* m_instance;
	static std::mutex m_mtx;
	std::shared_ptr<spdlog::logger> m_defaultLogger;
	std::string m_logDir;
	bool m_bInit;

};


// ============================================================================
//								便捷日志宏定义
// ============================================================================

#ifdef _WIN32
#define __FILENAME__ (strchr(__FILE__,'\\') ? strrchr(__FILE__,'\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__,'/') + 1 : __FILE__)
#endif





// 日志宏 - 支持流式输出
#define SLOG_TRACE(...)	SpdlogWrapper::GetInstance()->Trace(__FILENAME__, __LINE__, __FUNCTION__, FormatString(__VA_ARGS__))
#define SLOG_DEBUG(...)	SpdlogWrapper::GetInstance()->Debug(__FILENAME__, __LINE__, __FUNCTION__, FormatString(__VA_ARGS__))
#define SLOG_INFO(...)	SpdlogWrapper::GetInstance()->Info(__FILENAME__, __LINE__, __FUNCTION__, FormatString(__VA_ARGS__))
#define SLOG_WARN(...)	SpdlogWrapper::GetInstance()->Warn(__FILENAME__, __LINE__, __FUNCTION__, FormatString(__VA_ARGS__))
#define SLOG_ERROR(...)	SpdlogWrapper::GetInstance()->Error(__FILENAME__, __LINE__, __FUNCTION__, FormatString(__VA_ARGS__))
#define SLOG_CRITICAL(...)	SpdlogWrapper::GetInstance()->Critical(__FILENAME__, __LINE__, __FUNCTION__, FormatString(__VA_ARGS__))

// 日志宏 - 支持流式输出
#define LOG_T(...)	SpdlogWrapper::GetInstance()->GetLogger()->trace(__VA_ARGS__)
#define LOG_D(...)	SpdlogWrapper::GetInstance()->GetLogger()->debug(__VA_ARGS__)
#define LOG_I(...)	SpdlogWrapper::GetInstance()->GetLogger()->info(__VA_ARGS__)
#define LOG_W(...)	SpdlogWrapper::GetInstance()->GetLogger()->warn(__VA_ARGS__)
#define LOG_E(...)	SpdlogWrapper::GetInstance()->GetLogger()->error(__VA_ARGS__)
#define LOG_C(...)	SpdlogWrapper::GetInstance()->GetLogger()->critical(__VA_ARGS__)


// 命名日志器宏
#define NAMED_LOG_T(name, ...)  SpdlogWrapper::GetInstance()->GetLogger(name)->trace(__VA_ARGS__)
#define NAMED_LOG_D(name, ...)  SpdlogWrapper::GetInstance()->GetLogger(name)->debug(__VA_ARGS__)
#define NAMED_LOG_I(name, ...)  SpdlogWrapper::GetInstance()->GetLogger(name)->info(__VA_ARGS__)
#define NAMED_LOG_W(name, ...)  SpdlogWrapper::GetInstance()->GetLogger(name)->warn(__VA_ARGS__)
#define NAMED_LOG_E(name, ...)  SpdlogWrapper::GetInstance()->GetLogger(name)->error(__VA_ARGS__)


// 原生日志指针, 便于直接使用spdlog自带函数
#define LOGP SpdlogWrapper::GetInstance()->GetLogger()
#define LOGPN(name) SpdlogWrapper::GetInstance()->GetLogger(name)



