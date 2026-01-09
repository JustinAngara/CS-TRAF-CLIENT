#include "Menu.h"
#include "../../ext/imgui/imgui.h"
#include "../sdk/utils/Globals.h"
#include <Windows.h>
#include <algorithm>

static bool Menu::PollKey(int& out)
{
	for (int i = 1; i < 256; i++)
	{
		if (GetAsyncKeyState(i) & 1)
		{
			out = i;
			return true;
		}
	}
	return false;
}

static void Menu::CustomCheckbox(const char* label, bool* v)
{
	ImGui::PushID(label);

	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* dl = ImGui::GetWindowDrawList();
	float sz = 18.f;

	ImGui::InvisibleButton("checkbox", { sz, sz });
	bool hovered = ImGui::IsItemHovered();
	if (ImGui::IsItemClicked())
		*v = !*v;

	ImU32 bg_col = *v ? IM_COL32(130, 90, 255, 255) : (hovered ? IM_COL32(55, 55, 55, 255) : IM_COL32(40, 40, 40, 255));

	// Shadow
	dl->AddRectFilled({ p.x + 1, p.y + 1 }, { p.x + sz + 1, p.y + sz + 1 }, IM_COL32(0, 0, 0, 80), 4.f);
	// Main box
	dl->AddRectFilled(p, { p.x + sz, p.y + sz }, bg_col, 4.f);
	// Border
	dl->AddRect(p, { p.x + sz, p.y + sz }, IM_COL32(0, 0, 0, 150), 4.f, 0, 1.5f);

	if (*v)
	{
		// Inner checkmark box
		dl->AddRectFilled(
		{ p.x + 5, p.y + 5 },
		{ p.x + sz - 5, p.y + sz - 5 },
		IM_COL32(255, 255, 255, 255),
		2.f);
	}

	ImGui::SameLine();
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
	ImGui::TextUnformatted(label);

	ImGui::PopID();
}

static void Menu::CustomColor(const char* label, float col[4])
{
	ImGui::PushID(label);

	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* dl = ImGui::GetWindowDrawList();

	ImU32 c = ImGui::ColorConvertFloat4ToU32(
	ImVec4(col[0], col[1], col[2], col[3]));

	ImGui::InvisibleButton("color", { 24, 16 });
	bool hovered = ImGui::IsItemHovered();

	if (ImGui::IsItemClicked())
		ImGui::OpenPopup("picker");

	// Shadow
	dl->AddRectFilled({ p.x + 1, p.y + 1 }, { p.x + 25, p.y + 17 }, IM_COL32(0, 0, 0, 80), 3.f);
	// Color box
	dl->AddRectFilled(p, { p.x + 24, p.y + 16 }, c, 3.f);
	// Border
	ImU32 border_col = hovered ? IM_COL32(130, 90, 255, 255) : IM_COL32(0, 0, 0, 200);
	dl->AddRect(p, { p.x + 24, p.y + 16 }, border_col, 3.f, 0, 1.5f);

	if (ImGui::BeginPopup("picker"))
	{
		ImGui::ColorPicker4("picker", col,
		ImGuiColorEditFlags_NoInputs |
		ImGuiColorEditFlags_NoSidePreview |
		ImGuiColorEditFlags_NoSmallPreview);
		ImGui::EndPopup();
	}

	ImGui::SameLine();
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1);
	ImGui::TextUnformatted(label);

	ImGui::PopID();
}

static void Menu::CustomSlider(const char* label, float* v, float minVal, float maxVal)
{
	ImGui::PushID(label);
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* dl = ImGui::GetWindowDrawList();

	float slider_width = 200.f;
	float slider_height = 20.f;
	float grab_size = 8.f;

	ImGui::InvisibleButton("slider", { slider_width, slider_height });

	bool is_active = ImGui::IsItemActive();
	bool is_hovered = ImGui::IsItemHovered();

	if (is_active && ImGui::IsMouseDragging(0, 0.f))
	{
		float mouse_x = ImGui::GetMousePos().x - p.x;
		float percentage = std::clamp(mouse_x / slider_width, 0.0f, 1.0f);
		*v = minVal + percentage * (maxVal - minVal);
	}

	float percentage = (*v - minVal) / (maxVal - minVal);
	percentage = std::clamp(percentage, 0.0f, 1.0f);
	float grab_x = p.x + percentage * slider_width;

	// Shadow
	dl->AddRectFilled({ p.x + 1, p.y + 1 }, { p.x + slider_width + 1, p.y + slider_height + 1 }, IM_COL32(0, 0, 0, 60), 5.f);

	// Background
	ImU32 bg_col = is_hovered ? IM_COL32(50, 50, 50, 255) : IM_COL32(35, 35, 35, 255);
	dl->AddRectFilled(p, { p.x + slider_width, p.y + slider_height }, bg_col, 5.f);

	// Filled portion with gradient effect
	dl->AddRectFilledMultiColor(
	p, { grab_x, p.y + slider_height },
	IM_COL32(100, 70, 200, 255),
	IM_COL32(130, 90, 255, 255),
	IM_COL32(130, 90, 255, 255),
	IM_COL32(100, 70, 200, 255));

	// Border
	dl->AddRect(p, { p.x + slider_width, p.y + slider_height }, IM_COL32(0, 0, 0, 150), 5.f, 0, 1.5f);

	// Grab circle shadow
	dl->AddCircleFilled({ grab_x, p.y + slider_height / 2 }, grab_size + 1, IM_COL32(0, 0, 0, 100));

	// Grab circle
	ImU32 grab_col = is_active ? IM_COL32(160, 120, 255, 255) : IM_COL32(255, 255, 255, 255);
	dl->AddCircleFilled({ grab_x, p.y + slider_height / 2 }, grab_size, grab_col);
	dl->AddCircle({ grab_x, p.y + slider_height / 2 }, grab_size, IM_COL32(0, 0, 0, 200), 16, 1.5f);

	ImGui::SameLine();
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
	ImGui::Text("%s: %.1f", label, *v);

	ImGui::PopID();
}

void Menu::createESP()
{
	CustomCheckbox("Enable ESP", &Globals::esp_enabled);

	ImGui::SameLine(ImGui::GetWindowWidth() - 40.f);
	ImVec2 bind_p = ImGui::GetCursorScreenPos();
	ImDrawList* dl = ImGui::GetWindowDrawList();

	ImGui::InvisibleButton("bind_btn", { 25, 20 });
	bool bind_hovered = ImGui::IsItemHovered();

	if (ImGui::IsItemClicked())
	{
		ESP::bind_popup_pos = ImGui::GetMousePos();
		ESP::wait_for_bind = false;
		ImGui::OpenPopup("ESP Bind");
	}

	// Bind button
	ImU32 bind_col = bind_hovered ? IM_COL32(130, 90, 255, 255) : IM_COL32(60, 60, 60, 255);
	dl->AddRectFilled(bind_p, { bind_p.x + 25, bind_p.y + 20 }, bind_col, 3.f);
	dl->AddRect(bind_p, { bind_p.x + 25, bind_p.y + 20 }, IM_COL32(0, 0, 0, 150), 3.f, 0, 1.5f);

	dl->AddCircleFilled({ bind_p.x + 8, bind_p.y + 10 }, 2.f, IM_COL32(255, 255, 255, 255));
	dl->AddCircleFilled({ bind_p.x + 13, bind_p.y + 10 }, 2.f, IM_COL32(255, 255, 255, 255));
	dl->AddCircleFilled({ bind_p.x + 18, bind_p.y + 10 }, 2.f, IM_COL32(255, 255, 255, 255));

	ImGui::SetNextWindowPos(ESP::bind_popup_pos, ImGuiCond_Appearing);
	if (ImGui::BeginPopupModal("ESP Bind", nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextUnformatted("ESP Toggle Bind");
		ImGui::Separator();
		ImGui::Spacing();

		if (!ESP::wait_for_bind)
		{
			if (ImGui::Button("Set bind", { 140, 0 }))
			{
				ESP::wait_for_bind = true;
			}

			ImGui::SameLine();

			if (ImGui::Button("Close", { 80, 0 }))
			{
				ImGui::CloseCurrentPopup();
			}
		}
		else
		{
			ImGui::TextUnformatted("Press a key...");
			ImGui::TextDisabled("ESC to cancel");

			int key = 0;
			if (PollKey(key))
			{
				if (key != VK_ESCAPE)
					Globals::esp_bind = key;

				ESP::wait_for_bind = false;
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
	}
}

void Menu::createSubESP()
{
	ImGui::Indent(20.f);

	CustomCheckbox("Box", &Globals::esp_box);
	ImGui::SameLine(ImGui::GetWindowWidth() - 140.f);
	CustomColor("Box color", Globals::esp_box_color);

	ImGui::Spacing();

	CustomCheckbox("Skeleton", &Globals::esp_skeleton);
	ImGui::SameLine(ImGui::GetWindowWidth() - 140.f);
	CustomColor("Skeleton color", Globals::esp_skeleton_color);

	ImGui::Spacing();

	CustomCheckbox("Name", &Globals::esp_name);
	CustomCheckbox("Health bar", &Globals::esp_health);

	ImGui::Unindent(20.f);
}

void Menu::createAimbot()
{
	CustomCheckbox("Enable Aimbot", &Globals::aimbot_enabled);
	ImGui::Spacing();

	CustomSlider("FOV", &Globals::aimbot_fov, 0.f, 89.f);
	ImGui::Spacing();

	CustomCheckbox("Enable Smooth", &Globals::aimbot_smooth);
	ImGui::Spacing();

	CustomSlider("Smoothness", &Globals::aimbot_smoothness, 0.f, 1.f);

	CustomCheckbox("Enable Friendly Fire", &Globals::aimbot_friendly_fire);
}

void Menu::Render()
{
	ImGui::SetNextWindowSize({ SIZE_X, SIZE_Y }, ImGuiCond_Once);
	ImGui::Begin("TRAF CLIENT", &IsOpen,
	ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	ImDrawList* dl = ImGui::GetWindowDrawList();
	ImVec2 wp = ImGui::GetWindowPos();

	// Top accent bar with gradient
	dl->AddRectFilledMultiColor(
	wp,
	{ wp.x + ImGui::GetWindowWidth(), wp.y + 3 },
		IM_COL32(100, 70, 200, 255),
		IM_COL32(130, 90, 255, 255),
		IM_COL32(130, 90, 255, 255),
		IM_COL32(100, 70, 200, 255)
	);

	ImGui::Spacing();
	ImGui::Spacing();

	// ESP Section
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(130, 90, 255, 255));
	ImGui::Text("ESP");
	ImGui::PopStyleColor();
	ImGui::Spacing();

	createESP();
	ImGui::Spacing();
	createSubESP();

	ImGui::Spacing();
	ImGui::Spacing();

	// Separator line
	ImVec2 sep_pos = ImGui::GetCursorScreenPos();
	dl->AddLine(
	{ sep_pos.x, sep_pos.y },
	{ sep_pos.x + ImGui::GetWindowWidth() - 30, sep_pos.y },
	IM_COL32(80, 80, 80, 255),
	1.5f);

	ImGui::Spacing();
	ImGui::Spacing();

	// Aimbot Section
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(130, 90, 255, 255));
	ImGui::Text("AIMBOT");
	ImGui::PopStyleColor();
	ImGui::Spacing();

	createAimbot();

	ImGui::End();
}
