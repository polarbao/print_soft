#pragma once
#include "motionControlSDK.h"
#include "spdlog/spdlog.h"

#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QVector>
#include <QHash>
#include <QSet>
#include <QPair>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QVariant>
#include <QUrl>
#include <QUuid>
#include <QLine>
#include <QMargins>

// Qt类型特化
namespace fmt
{
	template <>
	struct formatter<MoveAxisPos> 
	{
		constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) 
		{
			return ctx.begin();
		}

		template <typename FormatContext>
		auto format(const MoveAxisPos& p, FormatContext& ctx) const -> decltype(ctx.out()) 
		{
			double x, y, z;
			p.toMillimeters(x, y, z);

			return fmt::format_to(ctx.out(), "MoveAxisPos(x:{:.3f}, y:{:.3f}, z:{:.3f})  mm", x, y, z);
		}
	};

	template <>
	struct formatter<QString>
	{
		constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
		{
			return ctx.begin();
		}

		template <typename FormatContext>
		auto format(const QString& s, FormatContext& ctx) const -> decltype(ctx.out())
		{
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
	//template <>
	//struct formatter<QByteArray>
	//{
	//	template <typename ParseCtx>
	//	constexpr auto parse(ParseCtx& ctx)
	//	{
	//		return ctx.begin();
	//	}

	//	template <typename FormatCtx>
	//	auto format(const QByteArray& qba, FormatCtx& ctx) const
	//	{
	//		auto out = ctx.out();
	//		out = fmt::format_to(out, "{}", qba.toHex(' ').toStdString());
	//		return out;
	//	}
	//};
	template <>
	struct formatter<QByteArray> : formatter<std::string>
	{
		auto format(const QByteArray& qb, format_context& ctx) const
		{
			return fmt::formatter<std::string>::format(std::string(qb.constData(), qb.length()), ctx);
		}
	};


	template <>
	struct formatter<QStringList>
	{
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
			for (const auto& str : str_list)
			{
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

	// QDateTime格式化器 - 日期时间，非常常用
	template <>
	struct formatter<QDateTime>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QDateTime& dt, FormatCtx& ctx) const
		{
			if (!dt.isValid()) {
				return fmt::format_to(ctx.out(), "QDateTime(invalid)");
			}
			QString str = dt.toString("yyyy-MM-dd HH:mm:ss.zzz");
			return fmt::format_to(ctx.out(), "{}", str.toStdString());
		}
	};

	// QDate格式化器 - 日期
	template <>
	struct formatter<QDate>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QDate& date, FormatCtx& ctx) const
		{
			if (!date.isValid())
			{
				return fmt::format_to(ctx.out(), "QDate(invalid)");
			}
			QString str = date.toString("yyyy-MM-dd");
			return fmt::format_to(ctx.out(), "{}", str.toStdString());
		}
	};

	// QTime格式化器 - 时间
	template <>
	struct formatter<QTime>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QTime& time, FormatCtx& ctx) const
		{
			if (!time.isValid()) {
				return fmt::format_to(ctx.out(), "QTime(invalid)");
			}
			QString str = time.toString("HH:mm:ss.zzz");
			return fmt::format_to(ctx.out(), "{}", str.toStdString());
		}
	};

	// QSet格式化器 - 集合
	template <typename T>
	struct formatter<QSet<T>>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QSet<T>& q_set, FormatCtx& ctx) const
		{
			fmt::format_to(ctx.out(), "{{");
			bool first = true;
			for (const auto& item : q_set)
			{
				if (!first) fmt::format_to(ctx.out(), ", ");
				fmt::format_to(ctx.out(), "{}", item);
				first = false;
			}
			return fmt::format_to(ctx.out(), "}}");
		}
	};

	// QPair格式化器 - 键值对
	template <typename T1, typename T2>
	struct formatter<QPair<T1, T2>>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QPair<T1, T2>& pair, FormatCtx& ctx) const
		{
			return fmt::format_to(ctx.out(), "({}, {})", pair.first, pair.second);
		}
	};

	// 几何数据类型
	// QPoint格式化器
	template <>
	struct formatter<QPoint>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QPoint& pt, FormatCtx& ctx) const
		{
			return fmt::format_to(ctx.out(), "QPoint({}, {})", pt.x(), pt.y());
		}
	};

	// QPointF格式化器
	template <>
	struct formatter<QPointF>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QPointF& pt, FormatCtx& ctx) const
		{
			return fmt::format_to(ctx.out(), "QPointF({:.2f}, {:.2f})", pt.x(), pt.y());
		}
	};

	// QSize格式化器
	template <>
	struct formatter<QSize>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QSize& sz, FormatCtx& ctx) const
		{
			return fmt::format_to(ctx.out(), "QSize({}x{})", sz.width(), sz.height());
		}
	};

	// QSizeF格式化器
	template <>
	struct formatter<QSizeF>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QSizeF& sz, FormatCtx& ctx) const
		{
			return fmt::format_to(ctx.out(), "QSizeF({:.2f}x{:.2f})", sz.width(), sz.height());
		}
	};

	// QRect格式化器
	template <>
	struct formatter<QRect>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QRect& rect, FormatCtx& ctx) const
		{
			return fmt::format_to(ctx.out(), "QRect({}, {}, {}x{})",
				rect.x(), rect.y(), rect.width(), rect.height());
		}
	};

	// QRectF格式化器
	template <>
	struct formatter<QRectF>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QRectF& rect, FormatCtx& ctx) const
		{
			return fmt::format_to(ctx.out(), "QRectF({:.2f}, {:.2f}, {:.2f}x{:.2f})",
				rect.x(), rect.y(), rect.width(), rect.height());
		}
	};

	// QUrl 格式化器
	template <>
	struct formatter<QUrl>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QUrl& url, FormatCtx& ctx) const
		{
			return fmt::format_to(ctx.out(), "QUrl(\"{}\")", url.toString().toStdString());
		}
	};

	// QUuid 格式化器
	template <>
	struct formatter<QUuid>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QUuid& uuid, FormatCtx& ctx) const
		{
			return fmt::format_to(ctx.out(), "QUuid({})", uuid.toString().toStdString());
		}
	};

	// QLine 格式化器
	template <>
	struct formatter<QLine>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QLine& l, FormatCtx& ctx) const
		{
			return fmt::format_to(ctx.out(), "QLine(({}, {}), ({}, {}))", l.x1(), l.y1(), l.x2(), l.y2());
		}
	};

	// QLineF 格式化器
	template <>
	struct formatter<QLineF>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QLineF& l, FormatCtx& ctx) const
		{
			return fmt::format_to(ctx.out(), "QLineF(({:.2f}, {:.2f}), ({:.2f}, {:.2f}))", l.x1(), l.y1(), l.x2(), l.y2());
		}
	};

	// QMargins 格式化器
	template <>
	struct formatter<QMargins>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QMargins& m, FormatCtx& ctx) const
		{
			return fmt::format_to(ctx.out(), "QMargins(L:{}, T:{}, R:{}, B:{})", m.left(), m.top(), m.right(), m.bottom());
		}
	};

	// QMarginsF 格式化器
	template <>
	struct formatter<QMarginsF>
	{
		template <typename ParseCtx>
		constexpr auto parse(ParseCtx& ctx)
		{
			return ctx.begin();
		}

		template <typename FormatCtx>
		auto format(const QMarginsF& m, FormatCtx& ctx) const
		{
			return fmt::format_to(ctx.out(), "QMarginsF(L:{:.2f}, T:{:.2f}, R:{:.2f}, B:{:.2f})", m.left(), m.top(), m.right(), m.bottom());
		}
	};
}

