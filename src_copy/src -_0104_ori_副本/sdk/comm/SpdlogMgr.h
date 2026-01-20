#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <regex>


#include <memory>
#include <atomic>
#include <mutex>
#include <string>

#include "comm/CSingleton.h"

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/fmt/bundled/ranges.h" // 支持容器打印 (QList, QVector等)

#include <QString>
#include <QByteArray>
#include <QVariant>
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QList>
#include <QVector>
#include <QStringList>
#include <QMap>
#include <QHash>

// ============================================================================
//                       Qt 类型 fmt 特化支持
// ============================================================================

// 支持 QString
template <> struct fmt::formatter<QString> : fmt::formatter<std::string> {
	auto format(const QString& qs, format_context& ctx) const {
		return fmt::formatter<std::string>::format(qs.toStdString(), ctx);
	}
};

// 支持 QByteArray
template <> struct fmt::formatter<QByteArray> : fmt::formatter<std::string> {
	auto format(const QByteArray& qb, format_context& ctx) const {
		return fmt::formatter<std::string>::format(std::string(qb.constData(), qb.length()), ctx);
	}
};

// 支持 QVariant
template <> struct fmt::formatter<QVariant> : fmt::formatter<std::string> {
	auto format(const QVariant& qv, format_context& ctx) const {
		return fmt::formatter<std::string>::format(qv.toString().toStdString(), ctx);
	}
};

// 支持 QDate
template <> struct fmt::formatter<QDate> : fmt::formatter<std::string> {
	auto format(const QDate& d, format_context& ctx) const {
		return fmt::formatter<std::string>::format(d.toString("yyyy-MM-dd").toStdString(), ctx);
	}
};

// 支持 QTime
template <> struct fmt::formatter<QTime> : fmt::formatter<std::string> {
	auto format(const QTime& t, format_context& ctx) const {
		return fmt::formatter<std::string>::format(t.toString("HH:mm:ss.zzz").toStdString(), ctx);
	}
};

// 支持 QDateTime
template <> struct fmt::formatter<QDateTime> : fmt::formatter<std::string> {
	auto format(const QDateTime& dt, format_context& ctx) const {
		return fmt::formatter<std::string>::format(dt.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString(), ctx);
	}
};

// 支持 QPoint
template <> struct fmt::formatter<QPoint> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
	auto format(const QPoint& p, format_context& ctx) const {
		return fmt::format_to(ctx.out(), "({}, {})", p.x(), p.y());
	}
};

// 支持 QPointF
template <> struct fmt::formatter<QPointF> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
	auto format(const QPointF& p, format_context& ctx) const {
		return fmt::format_to(ctx.out(), "({}, {})", p.x(), p.y());
	}
};

// 支持 QSize
template <> struct fmt::formatter<QSize> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
	auto format(const QSize& s, format_context& ctx) const {
		return fmt::format_to(ctx.out(), "({} x {})", s.width(), s.height());
	}
};

// 支持 QSizeF
template <> struct fmt::formatter<QSizeF> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
	auto format(const QSizeF& s, format_context& ctx) const {
		return fmt::format_to(ctx.out(), "({} x {})", s.width(), s.height());
	}
};

// 支持 QRect
template <> struct fmt::formatter<QRect> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
	auto format(const QRect& r, format_context& ctx) const {
		return fmt::format_to(ctx.out(), "[({}, {}) {} x {}]", r.x(), r.y(), r.width(), r.height());
	}
};

// 支持 QRectF
template <> struct fmt::formatter<QRectF> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
	auto format(const QRectF& r, format_context& ctx) const {
		return fmt::format_to(ctx.out(), "[({}, {}) {} x {}]", r.x(), r.y(), r.width(), r.height());
	}
};

// 支持 QList
template <typename T>
struct fmt::formatter<QList<T>> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
	auto format(const QList<T>& list, format_context& ctx) const {
		auto out = ctx.out();
		out = fmt::format_to(out, "[");
		for (int i = 0; i < list.size(); ++i) {
			if (i > 0) out = fmt::format_to(out, ", ");
			out = fmt::format_to(out, "{}", list[i]);
		}
		return fmt::format_to(out, "]");
	}
};

// 支持 QVector
template <typename T>
struct fmt::formatter<QVector<T>> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
	auto format(const QVector<T>& vec, format_context& ctx) const {
		auto out = ctx.out();
		out = fmt::format_to(out, "[");
		for (int i = 0; i < vec.size(); ++i) {
			if (i > 0) out = fmt::format_to(out, ", ");
			out = fmt::format_to(out, "{}", vec[i]);
		}
		return fmt::format_to(out, "]");
	}
};

// 支持 QStringList
template <>
struct fmt::formatter<QStringList> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
	auto format(const QStringList& list, format_context& ctx) const {
		auto out = ctx.out();
		out = fmt::format_to(out, "[");
		for (int i = 0; i < list.size(); ++i) {
			if (i > 0) out = fmt::format_to(out, ", ");
			out = fmt::format_to(out, "\"{}\"", list[i].toStdString());
		}
		return fmt::format_to(out, "]");
	}
};

// 支持 QMap
template <typename K, typename V>
struct fmt::formatter<QMap<K, V>> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
	auto format(const QMap<K, V>& map, format_context& ctx) const {
		auto out = ctx.out();
		out = fmt::format_to(out, "{{");
		auto it = map.constBegin();
		while (it != map.constEnd()) {
			if (it != map.constBegin()) out = fmt::format_to(out, ", ");
			out = fmt::format_to(out, "{}: {}", it.key(), it.value());
			++it;
		}
		return fmt::format_to(out, "}}");
	}
};

// 支持 QHash
template <typename K, typename V>
struct fmt::formatter<QHash<K, V>> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
	auto format(const QHash<K, V>& hash, format_context& ctx) const {
		auto out = ctx.out();
		out = fmt::format_to(out, "{{");
		auto it = hash.constBegin();
		while (it != hash.constEnd()) {
			if (it != hash.constBegin()) out = fmt::format_to(out, ", ");
			out = fmt::format_to(out, "{}: {}", it.key(), it.value());
			++it;
		}
		return fmt::format_to(out, "}}");
	}
};

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

// 格式化字符串

#include <sstream>
#include <iomanip>

template<typename... Args>
inline std::string FormatString(Args&&... args)
{
	std::ostringstream oss;
	(oss << ... << args);
	return oss.str();
}


// 日志宏 - 支持流式输出
#define SLOG_TRACE(...)	SpdlogWrapper::GetInstance()->Trace(__FILENAME__, __LINE__, __FUNCTION__, FormatString(__VA_ARGS__))
#define SLOG_DEBUG(...)	SpdlogWrapper::GetInstance()->Debug(__FILENAME__, __LINE__, __FUNCTION__, FormatString(__VA_ARGS__))
#define SLOG_INFO(...)	SpdlogWrapper::GetInstance()->Info(__FILENAME__, __LINE__, __FUNCTION__, FormatString(__VA_ARGS__))
#define SLOG_WARN(...)	SpdlogWrapper::GetInstance()->Warn(__FILENAME__, __LINE__, __FUNCTION__, FormatString(__VA_ARGS__))
#define SLOG_ERROR(...)	SpdlogWrapper::GetInstance()->Error(__FILENAME__, __LINE__, __FUNCTION__, FormatString(__VA_ARGS__))
#define SLOG_CRITICAL(...)	SpdlogWrapper::GetInstance()->Critical(__FILENAME__, __LINE__, __FUNCTION__, FormatString(__VA_ARGS__))

// 日志宏 - 支持流式输出
// 修复：使用 __VA_ARGS__ 传递参数，并补充 main.cpp 中使用的 LOGP 宏
#define LOGP		SpdlogWrapper::GetInstance()->GetLogger()
#define LOGPN(name) SpdlogWrapper::GetInstance()->GetLogger(name)

#define LOG_T(...)	SpdlogWrapper::GetInstance()->GetLogger()->trace(__VA_ARGS__)
#define LOG_D(...)	SpdlogWrapper::GetInstance()->GetLogger()->debug(__VA_ARGS__)
#define LOG_I(...)	SpdlogWrapper::GetInstance()->GetLogger()->info(__VA_ARGS__)
#define LOG_W(...)	SpdlogWrapper::GetInstance()->GetLogger()->warn(__VA_ARGS__)
#define LOG_E(...)	SpdlogWrapper::GetInstance()->GetLogger()->error(__VA_ARGS__)
#define LOG_C(...)	SpdlogWrapper::GetInstance()->GetLogger()->critical(__VA_ARGS__)


// 命名日志器宏
// 修复：支持变参，以匹配 main.cpp 中的 NAMED_LOG_I("AAA", "fmt {}", arg) 用法
#define NAMED_LOG_T(name, ...)  SpdlogWrapper::GetInstance()->GetLogger(name)->trace(__VA_ARGS__)
#define NAMED_LOG_D(name, ...)  SpdlogWrapper::GetInstance()->GetLogger(name)->debug(__VA_ARGS__)
#define NAMED_LOG_I(name, ...)  SpdlogWrapper::GetInstance()->GetLogger(name)->info(__VA_ARGS__)
#define NAMED_LOG_W(name, ...)  SpdlogWrapper::GetInstance()->GetLogger(name)->warn(__VA_ARGS__)
#define NAMED_LOG_E(name, ...)  SpdlogWrapper::GetInstance()->GetLogger(name)->error(__VA_ARGS__)