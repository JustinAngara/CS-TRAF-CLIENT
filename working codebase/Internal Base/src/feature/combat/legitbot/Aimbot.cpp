#include "Aimbot.h"
#include "../../../sdk/entity/EntityManager.h"
#include "../../../sdk/utils/Utils.h"
#include "../../../sdk/memory/PatternScan.h"
#include "../../../sdk/memory/Offsets.h"
#include "../../../sdk/utils/Globals.h"

#include <Windows.h>

namespace Aimbot {

	namespace AIM = Globals;

    void Run() 
    {
		if (!AIM::aimbot_enabled)
			return;

        C_CSPlayerPawn* local = EntityManager::Get().GetLocalPawn();
        if (!local || !local->IsAlive()) return;

        if (!(GetAsyncKeyState(AIM::aimbot_key) & 0x8000))
			return;

        C_CSPlayerPawn* bestTarget = GetBestTarget(local);
        if (!bestTarget) return;

        AimAtTarget(local, bestTarget);
    }

    void AimAtTarget(C_CSPlayerPawn* local, C_CSPlayerPawn* target) 
    {
        if (!local || !target) return;

        Vector targetPos = Utils::GetBonePos(target, BoneID::Head);
        if (targetPos.IsZero()) return;

        Vector localPos = local->m_vOldOrigin() + local->m_vecViewOffset();

        Vector aimAngles = Utils::CalcAngle(localPos, targetPos);

        uintptr_t client = Memory::GetModuleBase("client.dll");
        if (!client) return;

        Vector* currentAngles = reinterpret_cast<Vector*>(client + Offsets::dwViewAngles);
        if (!currentAngles) return;

        if (AIM::aimbot_smooth)
        {
            Vector delta = aimAngles - *currentAngles;
            Utils::NormalizeAngles(delta);

            *currentAngles += delta * AIM::aimbot_smoothness;
        }
        else 
        {
            *currentAngles = aimAngles;
        }

        Utils::NormalizeAngles(*currentAngles);
    }

    C_CSPlayerPawn* GetBestTarget(C_CSPlayerPawn* local) 
    {
        const auto& entities = EntityManager::Get().GetEntities();
        C_CSPlayerPawn* bestTarget = nullptr;
		float bestDistance = Globals::aimbot_fov;

        uintptr_t client = Memory::GetModuleBase("client.dll");
        if (!client) return nullptr;

        Vector* currentAngles = reinterpret_cast<Vector*>(client + Offsets::dwViewAngles);
        if (!currentAngles) return nullptr;

        for (const auto& ent : entities) 
        {
            if (!ent.pawn || !ent.pawn->IsAlive()) continue;
            if (!ent.isEnemy) continue;

            Vector headPos = Utils::GetBonePos(ent.pawn, BoneID::Head);
            if (headPos.IsZero()) continue;

            Vector localPos = local->m_vOldOrigin() + local->m_vecViewOffset();

            Vector aimAngles = Utils::CalcAngle(localPos, headPos);
            float fov = Utils::GetFoV(*currentAngles, aimAngles);

            if (fov < bestDistance) 
            {
                bestDistance = fov;
                bestTarget = ent.pawn;
            }
        }

        return bestTarget;
    }
}
