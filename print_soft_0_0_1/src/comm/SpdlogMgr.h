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

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/msvc_sink.h"

//#include "fmt/bundled/base.h"
//#include "fmt/bundled/core.h"
//#include "fmt/bundled/ranges.h"
#include "comm/CNewSingleton.h"

#include <QString>
#include <QList>
#include <QMap>
#include <QVector>
#include <QHash>
#include <QDate>
#include <QDateTime>
#include <QTime>





// 格式化字符串
template<typename... Args>
inline std::string FormatString(Args&&... args)
{
	std::ostringstream oss;
	(oss << ... << args);
	return oss.str();
}

// Qt类型特化
namespace fmt 
{
	template <>
	struct formatter<QString> {
		constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
			return ctx.begin();
		}

		template <typename FormatContext>
		auto format(const QString& s, FormatContext& ctx) const -> decltype(ctx.out()) {
			return fmt::format_to(ctx.out(), "{}", s.toStdString());
		}
	};


	// QList
	template <typename T>
	struct formatter<QList<T>>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QList<T>& q_list, FormatCtx& ctx) const
		{
			auto out = ctx.out(); // 接收初始迭代器
			out = fmt::format_to(out, "["); // 串联返回值
			bool first = true;
			for (const auto& item : q_list) {
				if (!first) out = fmt::format_to(out, ", ");
				out = fmt::format_to(out, "{}", item);
				first = false;
			}
			out = fmt::format_to(out, "]");
			return out;
		}
	};

	// QVector
	template <typename T>
	struct formatter<QVector<T>>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QVector<T>& q_vec, FormatCtx& ctx) const
		{
			auto out = ctx.out();
			out = fmt::format_to(out, "[");
			bool first = true;
			for (const auto& item : q_vec)
			{
				if (!first) out = fmt::format_to(out, ", ");
				out = fmt::format_to(out, "{}", item);
				first = false;
			}
			out = fmt::format_to(out, "]");
			return out;
		}
	};

	// QMap
	template <typename K, typename V>
	struct formatter<QMap<K, V>>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)  -> decltype(ctx.begin())
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QMap<K, V>& q_map, FormatCtx& ctx) const
		{
			auto out = ctx.out();
			out = fmt::format_to(out, "{{");
			bool first = true;
			for (auto it = q_map.constBegin(); it != q_map.constEnd(); ++it)
			{
				if (!first) out = fmt::format_to(out, ", ");
				out = fmt::format_to(out, "{}: {}", it.key(), it.value()); // 串联
				first = false;
			}
			out = fmt::format_to(out, "}}");
			return out;
		}
	};

	// QHash - 修复版本：独立实现，不继承QMap
	template <typename K, typename V>
	struct formatter<QHash<K, V>> {
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx) {
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QHash<K, V>& q_hash, FormatCtx& ctx) const
		{
			auto out = ctx.out();
			out = fmt::format_to(out, "{{");
			bool first = true;
			for (auto it = q_hash.constBegin(); it != q_hash.constEnd(); ++it) {
				if (!first) out = fmt::format_to(out, ", ");
				out = fmt::format_to(out, "{}: {}", it.key(), it.value()); // 串联
				first = false;
			}
			out = fmt::format_to(out, "}}");
			return out;
		}
	};

	// QByteArray - 新增：十六进制格式输出
	template <>
	struct formatter<QByteArray>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QByteArray& qba, FormatCtx& ctx) const
		{
			auto out = ctx.out();
			out = fmt::format_to(out, "{}", qba.toHex(' ').toStdString());
			return out;
		}
	};

	template <>
	struct formatter<QStringList> {
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx) 
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QStringList& str_list, FormatCtx& ctx) const
		{
			fmt::format_to(ctx.out(), "[");
			bool first = true;
			for (const auto& str : str_list) {
				if (!first) fmt::format_to(ctx.out(), ", ");
				fmt::format_to(ctx.out(), "\"{}\"", str.toStdString());
				first = false;
			}
			return fmt::format_to(ctx.out(), "]");
		}
	};

	// QVariant
	template <>
	struct formatter<QVariant> 
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx) 
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QVariant& var, FormatCtx& ctx) const
		{
			if (!var.isValid()) 
			{
				return fmt::format_to(ctx.out(), "QVariant(invalid)");
			}

			QString type_name = var.typeName();
			QString value_str;

			switch (var.type()) 
			{
			case QVariant::Invalid:
				return fmt::format_to(ctx.out(), "QVariant(invalid)");
			case QVariant::Bool:
				value_str = var.toBool() ? "true" : "false";
				break;
			case QVariant::Int:
				value_str = QString::number(var.toInt());
				break;
			case QVariant::UInt:
				value_str = QString::number(var.toUInt());
				break;
			case QVariant::LongLong:
				value_str = QString::number(var.toLongLong());
				break;
			case QVariant::ULongLong:
				value_str = QString::number(var.toULongLong());
				break;
			case QVariant::Double:
				value_str = QString::number(var.toDouble(), 'f', 6);
				break;
			case QVariant::String:
				value_str = QString("\"%1\"").arg(var.toString());
				break;
			case QVariant::Date:
				value_str = var.toDate().toString("yyyy-MM-dd");
				break;
			case QVariant::Time:
				value_str = var.toTime().toString("HH:mm:ss");
				break;
			case QVariant::DateTime:
				value_str = var.toDateTime().toString("yyyy-MM-dd HH:mm:ss");
				break;
			default:
				value_str = var.toString();
				if (value_str.isEmpty() && var.canConvert<QString>()) 
				{
					value_str = "<complex type>";
				}
			}
			return fmt::format_to(ctx.out(), "QVariant({}:{})",
				type_name.toStdString(),
				value_str.toStdString());
		}
	};

}



class SpdlogWrapper //: public CSingleton<SpdlogWrapper>
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
#define SLOG_DEBUG(...)	SpdlogWrapper::GetInstance()->DEBUG(__FILENAME__, __LINE__, __FUNCTION__, FormatString(__VA_ARGS__))
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



