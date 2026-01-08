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
    if (ImGui::IsItemClicked()) *v = !*v;

    ImU32 col = *v ? IM_COL32(130, 90, 255, 255) : IM_COL32(45, 45, 45, 255);
    dl->AddRectFilled(p, { p.x + sz, p.y + sz }, col, 4.f);

    if (*v)
        dl->AddRectFilled(
            { p.x + 5, p.y + 5 },
            { p.x + sz - 5, p.y + sz - 5 },
            IM_COL32(15, 15, 15, 255),
            2.f
        );

    ImGui::SameLine();
    ImGui::TextUnformatted(label);

    ImGui::PopID();
}

static void Menu::CustomColor(const char* label, float col[4])
{
    ImGui::PushID(label);

    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    ImU32 c = ImGui::ColorConvertFloat4ToU32(
        ImVec4(col[0], col[1], col[2], col[3])
    );

    ImGui::InvisibleButton("color", { 22, 14 });
    if (ImGui::IsItemClicked())
        ImGui::OpenPopup("picker");

    dl->AddRectFilled(p, { p.x + 22, p.y + 14 }, c, 3.f);
    dl->AddRect(p, { p.x + 22, p.y + 14 }, IM_COL32(0, 0, 0, 255), 3.f);

    if (ImGui::BeginPopup("picker"))
    {
        ImGui::ColorPicker4("picker", col,
            ImGuiColorEditFlags_NoInputs |
            ImGuiColorEditFlags_NoSidePreview |
            ImGuiColorEditFlags_NoSmallPreview);
        ImGui::EndPopup();
    }

    ImGui::SameLine();
    ImGui::TextUnformatted(label);

    ImGui::PopID();
}

static void Menu::CustomSlider(const char* label, float* v, float minVal, float maxVal)
{
	ImGui::PushID(label);
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* dl = ImGui::GetWindowDrawList();

	float slider_width = 150.f;
	float slider_height = 18.f;
	float grab_size = 10.f;

	// Create invisible button for interaction
	ImGui::InvisibleButton("slider", { slider_width, slider_height });

	bool is_active = ImGui::IsItemActive();
	bool is_hovered = ImGui::IsItemHovered();

	// Handle dragging
	if (is_active && ImGui::IsMouseDragging(0, 0.f))
	{
		float mouse_x = ImGui::GetMousePos().x - p.x;
		float percentage = std::clamp(mouse_x / slider_width, 0.0f, 1.0f);
		*v = minVal + percentage * (maxVal - minVal);
	}

	// Calculate grab position
	float percentage = (*v - minVal) / (maxVal - minVal);
	percentage = std::clamp(percentage, 0.0f, 1.0f);
	float grab_x = p.x + percentage * slider_width;

	ImU32 bg_col = is_hovered ? IM_COL32(55, 55, 55, 255) : IM_COL32(45, 45, 45, 255);
	dl->AddRectFilled(p, { p.x + slider_width, p.y + slider_height }, bg_col, 4.f);

	dl->AddRectFilled(p, { grab_x, p.y + slider_height },
	IM_COL32(130, 90, 255, 255), 4.f);

	ImU32 grab_col = is_active ? IM_COL32(150, 110, 255, 255) : IM_COL32(130, 90, 255, 255);
	dl->AddCircleFilled({ grab_x, p.y + slider_height / 2 }, grab_size, grab_col);
	dl->AddCircle({ grab_x, p.y + slider_height / 2 }, grab_size, IM_COL32(0, 0, 0, 255), 12, 1.5f);

	ImGui::SameLine();
	ImGui::Text("%s: %.1f", label, *v);

	ImGui::PopID();
}

void Menu::createESP()
{
	CustomCheckbox("Enable ESP", &Globals::esp_enabled);

	ImGui::SameLine(ImGui::GetWindowWidth() - 35.f);
	ImGui::Text("...");
	if (ImGui::IsItemClicked())
	{
		bind_popup_pos = ImGui::GetMousePos();
		wait_for_bind = false;
		ImGui::OpenPopup("ESP Bind");
	}

	ImGui::SetNextWindowPos(bind_popup_pos, ImGuiCond_Appearing);
	if (ImGui::BeginPopupModal("ESP Bind", nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextUnformatted("ESP Toggle Bind");
		ImGui::Separator();

		if (!wait_for_bind)
		{
			if (ImGui::Button("Set bind", { 140, 0 }))
				wait_for_bind = true;

			ImGui::SameLine();
			if (ImGui::Button("Close", { 80, 0 }))
				ImGui::CloseCurrentPopup();
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

				wait_for_bind = false;
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
		createSubESP();
	}
}

void Menu::createSubESP()
{
	CustomCheckbox("Box", &Globals::esp_box);
	CustomColor("Box color", Globals::esp_box_color);

	ImGui::Spacing();

	CustomCheckbox("Skeleton", &Globals::esp_skeleton);
	CustomColor("Skeleton color", Globals::esp_skeleton_color);

	ImGui::Spacing();

	CustomCheckbox("Name", &Globals::esp_name);
	CustomCheckbox("Health bar", &Globals::esp_health);
}

void Menu::createAimbot()
{
	CustomCheckbox("Enable Aimbot", &Globals::aimbot_enabled);
	CustomSlider("FOV", &Globals::aimbot_fov, 0.f, 89.f);
}




// will display visuals here
void Menu::Render()
{
	ImGui::SetNextWindowSize({ SIZE_X, SIZE_Y }, ImGuiCond_Once);
	ImGui::Begin("VISUALS", &IsOpen,
	ImGuiWindowFlags_NoResize |
	ImGuiWindowFlags_NoCollapse |
	ImGuiWindowFlags_NoScrollbar);

	ImDrawList* dl = ImGui::GetWindowDrawList();
	ImVec2 wp = ImGui::GetWindowPos();

	dl->AddRectFilled(
		wp,
		{ wp.x + ImGui::GetWindowWidth(), wp.y + 3 },
		IM_COL32(130, 90, 255, 255)
	);

	ImGui::Spacing();
	ImGui::Spacing();

	createESP();

	ImGui::Separator();
	ImGui::Spacing();

	createSubESP();

	ImGui::Spacing();

	// aimbot stuff
	ImGui::Separator();
	ImGui::Spacing();

	createAimbot();

	ImGui::End();
}
