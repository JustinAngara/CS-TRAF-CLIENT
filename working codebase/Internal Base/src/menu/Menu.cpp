#include "Menu.h"
#include "../../ext/imgui/imgui.h"
#include "../sdk/utils/Globals.h"
#include <Windows.h>

static bool wait_for_bind = false;
static ImVec2 bind_popup_pos = {};

static bool PollKey(int& out)
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

static void CustomCheckbox(const char* label, bool* v)
{
    ImGui::PushID(label);

    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float sz = 18.f;

    ImGui::InvisibleButton("checkbox", { sz, sz });
    if (ImGui::IsItemClicked())
        *v = !*v;

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

static void CustomColor(const char* label, float col[4])
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

void Menu::Render()
{
    ImGui::SetNextWindowSize({ 320, 260 }, ImGuiCond_Once);
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
    }

    ImGui::Separator();
    ImGui::Spacing();

    CustomCheckbox("Box", &Globals::esp_box);
    CustomColor("Box color", Globals::esp_box_color);

    ImGui::Spacing();

    CustomCheckbox("Skeleton", &Globals::esp_skeleton);
    CustomColor("Skeleton color", Globals::esp_skeleton_color);

    ImGui::Spacing();

    CustomCheckbox("Name", &Globals::esp_name);
    CustomCheckbox("Health bar", &Globals::esp_health);

    ImGui::End();
}
