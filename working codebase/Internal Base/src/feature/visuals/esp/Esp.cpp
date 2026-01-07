#include "Esp.h"
#include "../../../sdk/entity/EntityManager.h"
#include "../../../sdk/utils/Utils.h"
#include "../../../sdk/utils/Globals.h"
#include "../../../../ext/imgui/imgui.h"
#include <algorithm>

void ESP::Render()
{
    if (!Globals::esp_enabled)
        return;

    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    const float sw = ImGui::GetIO().DisplaySize.x;
    const float sh = ImGui::GetIO().DisplaySize.y;

    const auto& entities = EntityManager::Get().GetEntities();
    C_CSPlayerPawn* localPawn = EntityManager::Get().GetLocalPawn();
    if (!localPawn)
        return;

    const ImU32 boxCol = ImGui::ColorConvertFloat4ToU32(ImVec4(
        Globals::esp_box_color[0], Globals::esp_box_color[1],
        Globals::esp_box_color[2], Globals::esp_box_color[3]
    ));
    const ImU32 skelCol = ImGui::ColorConvertFloat4ToU32(ImVec4(
        Globals::esp_skeleton_color[0], Globals::esp_skeleton_color[1],
        Globals::esp_skeleton_color[2], Globals::esp_skeleton_color[3]
    ));
    const ImU32 nameCol = ImGui::ColorConvertFloat4ToU32(ImVec4(
        Globals::esp_name_color[0], Globals::esp_name_color[1],
        Globals::esp_name_color[2], Globals::esp_name_color[3]
    ));

    for (const auto& ent : entities)
    {
        C_CSPlayerPawn* pawn = ent.pawn;
        if (!pawn || !pawn->IsAlive())
            continue;

        if (pawn->m_iTeamNum() == localPawn->m_iTeamNum())
            continue;

        Vector feet = pawn->m_vOldOrigin();
        Vector head = Utils::GetBonePos(pawn, BoneID::Head);
        if (head.IsZero())
            continue;

        head.z += 8.2f;

        Vector sFeet, sHead;
        if (!Utils::WorldToScreen(feet, sFeet, (float*)Globals::ViewMatrix, sw, sh) ||
            !Utils::WorldToScreen(head, sHead, (float*)Globals::ViewMatrix, sw, sh))
            continue;

        float h = sFeet.y - sHead.y;
        if (h < 5.f)
            continue;

        float w = h * 0.5f;
        float x = sHead.x - w * 0.5f;
        float y = sHead.y;

        if (Globals::esp_box)
        {
            dl->AddRect({ x, y }, { x + w, y + h }, boxCol, 0.f, 0, Globals::esp_box_thickness);
            dl->AddRect({ x - 1, y - 1 }, { x + w + 1, y + h + 1 }, IM_COL32(0, 0, 0, 220));
        }

        if (Globals::esp_health)
        {
            int hp = pawn->m_iHealth();
            float hpFrac = std::clamp(hp / 100.f, 0.f, 1.f);
            float hpH = h * hpFrac;

            dl->AddRectFilled({ x - 6, y - 1 }, { x - 2, y + h + 1 }, IM_COL32(0, 0, 0, 150));
            dl->AddRectFilled({ x - 5, y + h - hpH }, { x - 3, y + h }, IM_COL32(0, 255, 0, 255));
        }

        if (Globals::esp_name && ent.controller)
        {
            char nameBuf[64]{};
            uintptr_t namePtr = reinterpret_cast<uintptr_t>(ent.controller->m_iszPlayerName());
            if (Utils::SafeReadString(namePtr, nameBuf))
            {
                ImVec2 ts = ImGui::CalcTextSize(nameBuf);
                dl->AddText({ x + (w - ts.x) * 0.5f, y - ts.y - 2 }, nameCol, nameBuf);
            }
        }

        if (Globals::esp_skeleton)
        {
            const float thick = Globals::esp_skeleton_thickness;

            for (const auto& conn : Bones::connections)
            {
                Vector b1 = Utils::GetBonePos(pawn, conn.bone1);
                Vector b2 = Utils::GetBonePos(pawn, conn.bone2);
                if (b1.IsZero() || b2.IsZero())
                    continue;

                Vector sb1, sb2;
                if (Utils::WorldToScreen(b1, sb1, (float*)Globals::ViewMatrix, sw, sh) &&
                    Utils::WorldToScreen(b2, sb2, (float*)Globals::ViewMatrix, sw, sh))
                {
                    dl->AddLine({ sb1.x, sb1.y }, { sb2.x, sb2.y }, skelCol, thick);
                }
            }
        }
    }
}
