#include "EnemyCounter.h"
#include "../../../sdk/entity/EntityManager.h"
#include "../../../sdk/utils/Utils.h"
#include "../../../sdk/utils/Globals.h"
#include "../../../../ext/imgui/imgui.h"
#include <algorithm>

void EnemyCounter::Render()
{
    auto& em = EntityManager::Get();
    C_CSPlayerPawn* local = em.GetLocalPawn();
    if (!local)
        return;

    int aliveEnemies = 0;

    for (const auto& ent : em.GetEntities())
    {
        if (!ent.pawn)
            continue;

        if (!ent.pawn->IsAlive())
            continue;

        if (ent.pawn->m_iTeamNum() == local->m_iTeamNum())
            continue;

        aliveEnemies++;
    }

    char buf[32];
    sprintf_s(buf, "ENEMIES: %d", aliveEnemies);

    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    dl->AddText(
        { 20.f, 20.f }, // posizione
        IM_COL32(255, 255, 255, 220),
        buf
    );
}
