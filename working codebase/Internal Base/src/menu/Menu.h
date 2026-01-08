#pragma once

namespace Menu {
    inline bool IsOpen = false;
    void Render();
	static void CustomSlider(const char* label, float* v, float minVal, float maxVal);
	static void CustomColor(const char* label, float col[4]);
	static void CustomCheckbox(const char* label, bool* v);
	static bool PollKey(int& out);
}
