#pragma once

#include "../../ext/imgui/imgui.h"
namespace MenuColors
{
	// base
	constexpr ImU32 black = IM_COL32(0, 0, 0, 100);
	constexpr ImU32 dark_bg = IM_COL32(8, 8, 12, 160);
	constexpr ImU32 darker_bg = IM_COL32(12, 12, 18, 120);
	constexpr ImU32 darkest_bg = IM_COL32(18, 18, 25, 90);

	// accent colors
	constexpr ImU32 purple = IM_COL32(130, 90, 255, 255);
	constexpr ImU32 dark_purple = IM_COL32(90, 60, 180, 255);
	constexpr ImU32 bright_purple = IM_COL32(140, 100, 255, 255);
	constexpr ImU32 light_purple = IM_COL32(150, 110, 255, 255);
	constexpr ImU32 vibrant_purple = IM_COL32(160, 120, 255, 255);
	constexpr ImU32 checkbox_purple = IM_COL32(120, 80, 230, 255);
	constexpr ImU32 checkbox_bright_purple = IM_COL32(150, 100, 255, 255);

	// gray colors
	constexpr ImU32 gray = IM_COL32(80, 80, 90, 255);
	constexpr ImU32 light_gray = IM_COL32(90, 90, 100, 255);
	constexpr ImU32 dark_gray = IM_COL32(45, 45, 55, 255);
	constexpr ImU32 hover_gray = IM_COL32(65, 65, 75, 255);

	// white
	constexpr ImU32 white = IM_COL32(255, 255, 255, 255);

	// semi transparent overlays
	constexpr ImU32 black_80 = IM_COL32(0, 0, 0, 80);
	constexpr ImU32 purple_glow = IM_COL32(130, 90, 255, 60);
	constexpr ImU32 purple_hover = IM_COL32(130, 90, 255, 40);
	constexpr ImU32 white_highlight = IM_COL32(255, 255, 255, 30);

	// transparent colors
	constexpr ImU32 transparent = IM_COL32(0, 0, 0, 0);
}
