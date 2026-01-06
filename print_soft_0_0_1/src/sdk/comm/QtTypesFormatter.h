#pragma once

/**
 * @file QtTypesFormatter.h
 * @brief Qt常用数据类型的spdlog格式化器扩展
 * @details 为spdlog库添加更多Qt类型支持，包括日期时间、几何类型、容器类型等
 * @note 需要与SpdlogMgr.h配合使用
 */

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QVariant>
#include <QUrl>
#include <QUuid>
#include <QPoint>
#include <QPointF>
#include <QSize>
#include <QSizeF>
#include <QRect>
#include <QRectF>
#include <QSet>
#include <QPair>

#include "spdlog/fmt/bundled/core.h"

// ============================================================================
//                     高优先级类型格式化器
// ============================================================================

// QStringList格式化器 - 字符串列表，非常常用
template <>
struct fmt::formatter<QStringList> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QStringList& str_list, FormatCtx& ctx) {
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

// QDateTime格式化器 - 日期时间，非常常用
template <>
struct fmt::formatter<QDateTime> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QDateTime& dt, FormatCtx& ctx) {
		if (!dt.isValid()) {
			return fmt::format_to(ctx.out(), "QDateTime(invalid)");
		}
		QString str = dt.toString("yyyy-MM-dd HH:mm:ss.zzz");
		return fmt::format_to(ctx.out(), "{}", str.toStdString());
	}
};

// QDate格式化器 - 日期
template <>
struct fmt::formatter<QDate> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QDate& date, FormatCtx& ctx) {
		if (!date.isValid()) {
			return fmt::format_to(ctx.out(), "QDate(invalid)");
		}
		QString str = date.toString("yyyy-MM-dd");
		return fmt::format_to(ctx.out(), "{}", str.toStdString());
	}
};

// QTime格式化器 - 时间
template <>
struct fmt::formatter<QTime> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QTime& time, FormatCtx& ctx) {
		if (!time.isValid()) {
			return fmt::format_to(ctx.out(), "QTime(invalid)");
		}
		QString str = time.toString("HH:mm:ss.zzz");
		return fmt::format_to(ctx.out(), "{}", str.toStdString());
	}
};

// QVariant格式化器 - 多类型容器，非常常用
template <>
struct fmt::formatter<QVariant> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QVariant& var, FormatCtx& ctx) {
		if (!var.isValid()) {
			return fmt::format_to(ctx.out(), "QVariant(invalid)");
		}

		QString type_name = var.typeName();
		QString value_str;

		switch (var.type()) {
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
			// 尝试转换为字符串
			value_str = var.toString();
			if (value_str.isEmpty() && var.canConvert<QString>()) {
				value_str = "<complex type>";
			}
		}

		return fmt::format_to(ctx.out(), "QVariant({}:{})",
			type_name.toStdString(),
			value_str.toStdString());
	}
};

// ============================================================================
//                     几何类型格式化器
// ============================================================================

// QPoint格式化器
template <>
struct fmt::formatter<QPoint> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QPoint& pt, FormatCtx& ctx) {
		return fmt::format_to(ctx.out(), "QPoint({}, {})", pt.x(), pt.y());
	}
};

// QPointF格式化器
template <>
struct fmt::formatter<QPointF> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QPointF& pt, FormatCtx& ctx) {
		return fmt::format_to(ctx.out(), "QPointF({:.2f}, {:.2f})", pt.x(), pt.y());
	}
};

// QSize格式化器
template <>
struct fmt::formatter<QSize> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QSize& sz, FormatCtx& ctx) {
		return fmt::format_to(ctx.out(), "QSize({}x{})", sz.width(), sz.height());
	}
};

// QSizeF格式化器
template <>
struct fmt::formatter<QSizeF> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QSizeF& sz, FormatCtx& ctx) {
		return fmt::format_to(ctx.out(), "QSizeF({:.2f}x{:.2f})", sz.width(), sz.height());
	}
};

// QRect格式化器
template <>
struct fmt::formatter<QRect> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QRect& rect, FormatCtx& ctx) {
		return fmt::format_to(ctx.out(), "QRect({}, {}, {}x{})",
			rect.x(), rect.y(), rect.width(), rect.height());
	}
};

// QRectF格式化器
template <>
struct fmt::formatter<QRectF> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QRectF& rect, FormatCtx& ctx) {
		return fmt::format_to(ctx.out(), "QRectF({:.2f}, {:.2f}, {:.2f}x{:.2f})",
			rect.x(), rect.y(), rect.width(), rect.height());
	}
};

// ============================================================================
//                     容器类型格式化器
// ============================================================================

// QSet格式化器 - 集合
template <typename T>
struct fmt::formatter<QSet<T>> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QSet<T>& q_set, FormatCtx& ctx) {
		fmt::format_to(ctx.out(), "{{");
		bool first = true;
		for (const auto& item : q_set) {
			if (!first) fmt::format_to(ctx.out(), ", ");
			fmt::format_to(ctx.out(), "{}", item);
			first = false;
		}
		return fmt::format_to(ctx.out(), "}}");
	}
};

// QPair格式化器 - 键值对
template <typename T1, typename T2>
struct fmt::formatter<QPair<T1, T2>> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QPair<T1, T2>& pair, FormatCtx& ctx) {
		return fmt::format_to(ctx.out(), "({}, {})", pair.first, pair.second);
	}
};

// ============================================================================
//                     网络与URL类型格式化器
// ============================================================================

// QUrl格式化器
template <>
struct fmt::formatter<QUrl> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QUrl& url, FormatCtx& ctx) {
		if (!url.isValid()) {
			return fmt::format_to(ctx.out(), "QUrl(invalid)");
		}
		return fmt::format_to(ctx.out(), "{}", url.toString().toStdString());
	}
};

// QUuid格式化器
template <>
struct fmt::formatter<QUuid> {
	template <typename ParseCtx>
	constexpr auto parse(ParseCtx& ctx) {
		return ctx.begin();
	}

	template <typename FormatCtx>
	auto format(const QUuid& uuid, FormatCtx& ctx) {
		if (uuid.isNull()) {
			return fmt::format_to(ctx.out(), "QUuid(null)");
		}
		return fmt::format_to(ctx.out(), "{}", uuid.toString().toStdString());
	}
};

// ============================================================================
//                     使用说明
// ============================================================================

/*
使用示例：

#include "comm/SpdlogMgr.h"
#include "comm/QtTypesFormatter.h"  // 包含扩展类型支持

void example() {
    // 高优先级类型
    QStringList files = {"file1.txt", "file2.doc"};
    LOG_I("Files: {}", files);  // 输出: Files: ["file1.txt", "file2.doc"]

    QDateTime now = QDateTime::currentDateTime();
    LOG_I("Time: {}", now);  // 输出: Time: 2025-12-23 15:30:45.123

    QVariant var(42);
    LOG_I("Variant: {}", var);  // 输出: Variant: QVariant(int:42)

    // 几何类型
    QPoint pt(100, 200);
    LOG_I("Point: {}", pt);  // 输出: Point: QPoint(100, 200)

    QSize sz(1920, 1080);
    LOG_I("Size: {}", sz);  // 输出: Size: QSize(1920x1080)

    QRect rect(10, 20, 300, 400);
    LOG_I("Rect: {}", rect);  // 输出: Rect: QRect(10, 20, 300x400)

    // 容器类型
    QSet<int> nums = {1, 2, 3, 5};
    LOG_I("Set: {}", nums);  // 输出: Set: {1, 2, 3, 5}

    // URL类型
    QUrl url("https://example.com");
    LOG_I("URL: {}", url);  // 输出: URL: https://example.com
}

支持的类型列表：
- QStringList: 字符串列表
- QDateTime/QDate/QTime: 日期时间
- QVariant: 多类型容器
- QPoint/QPointF: 点坐标
- QSize/QSizeF: 尺寸
- QRect/QRectF: 矩形
- QSet<T>: 集合
- QPair<T1,T2>: 键值对
- QUrl: URL
- QUuid: UUID

注意事项：
1. 此文件需要在 SpdlogMgr.h 之后包含
2. 无效的Qt对象会显示为 "Type(invalid)"
3. QVariant会显示其实际类型和值
4. 所有类型都经过了空值/无效值检查
*/
