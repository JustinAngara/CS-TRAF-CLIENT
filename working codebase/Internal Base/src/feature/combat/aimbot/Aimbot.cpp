#include "Aimbot.h"
#include "../../../../sdk/entity/EntityManager.h"
#include "../../../../sdk/utils/Utils.h"
#include "../../../../sdk/utils/Globals.h"
#include "../../../../sdk/memory/Offsets.h"
#include <Windows.h>

namespace Aimbot {
    
    void Run() {
        if (!aimbot_enabled) return;

        C_CSPlayerPawn* local = EntityManager::Get().GetLocalPawn();
        if (!local || !local->IsAlive()) return;

        // Check if aimbot key is pressed
        if (!(GetAsyncKeyState(aimbot_key) & 0x8000)) return;

        // Find best target
        C_CSPlayerPawn* bestTarget = GetBestTarget(local);
        if (!bestTarget) return;

        // Aim at target
        AimAtTarget(local, bestTarget);
    }

    void AimAtTarget(C_CSPlayerPawn* local, C_CSPlayerPawn* target) {
        if (!local || !target) return;

        // Get target head position
        Vector targetPos = Utils::GetBonePos(target, BoneID::Head);
        if (targetPos.IsZero()) return;

        // Get local player eye position
        Vector localPos = local->m_vOldOrigin() + local->m_vecViewOffset();

        // Calculate aim angles
        Vector aimAngles = Utils::CalcAngle(localPos, targetPos);
        
        // Get current view angles (you need to find this offset)
        uintptr_t client = Memory::GetModuleBase("client.dll");
        if (!client) return;

        // This offset needs to be found - it's typically in client.dll
        constexpr uintptr_t dwViewAngles = 0xXXXXXXXX; // Replace with actual offset
        Vector* currentAngles = reinterpret_cast<Vector*>(client + dwViewAngles);
        if (!currentAngles) return;

        // Smooth aiming
        if (aimbot_smooth) {
            Vector delta = aimAngles - *currentAngles;
            Utils::NormalizeAngles(delta);
            
            // Apply smoothing
            *currentAngles += delta * aimbot_smoothness;
        } else {
            *currentAngles = aimAngles;
        }

        // Clamp angles to prevent untrusted
        Utils::NormalizeAngles(*currentAngles);
    }

    C_CSPlayerPawn* GetBestTarget(C_CSPlayerPawn* local) {
        const auto& entities = EntityManager::Get().GetEntities();
        C_CSPlayerPawn* bestTarget = nullptr;
        float bestDistance = aimbot_fov;

        // Get current view angles for FOV calculation
        uintptr_t client = Memory::GetModuleBase("client.dll");
        if (!client) return nullptr;
        
        constexpr uintptr_t dwViewAngles = 0xXXXXXXXX; // Replace with actual offset
        Vector* currentAngles = reinterpret_cast<Vector*>(client + dwViewAngles);
        if (!currentAngles) return nullptr;

        for (const auto& ent : entities) {
            if (!ent.pawn || !ent.pawn->IsAlive()) continue;
            if (!ent.isEnemy) continue;

            // Get head position
            Vector headPos = Utils::GetBonePos(ent.pawn, BoneID::Head);
            if (headPos.IsZero()) continue;

            // Get local eye position
            Vector localPos = local->m_vOldOrigin() + local->m_vecViewOffset();

            // Calculate FOV
            Vector aimAngles = Utils::CalcAngle(localPos, headPos);
            float fov = Utils::GetFoV(*currentAngles, aimAngles);

            if (fov < bestDistance) {
                bestDistance = fov;
                bestTarget = ent.pawn;
            }
        }

        return bestTarget;
    }
}
