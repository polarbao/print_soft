#pragma once
#include <spdlog/fmt/fmt.h>
#include <motionControlSDK.h>

namespace fmt
{
	template <>
	struct formatter<MoveAxisPos> {
		constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
			return ctx.begin();
		}

		template <typename FormatContext>
		auto format(const MoveAxisPos& p, FormatContext& ctx) const -> decltype(ctx.out()) {
			double x, y, z;
			p.toMillimeters(x, y, z);

			return fmt::format_to(ctx.out(), "MoveAxisPos(x:{:.3f}, y:{:.3f}, z:{:.3f})  mm", x, y, z);
		}
	};
}