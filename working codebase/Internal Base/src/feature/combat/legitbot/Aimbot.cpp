#include "Aimbot.h"
#include "../../../sdk/entity/EntityManager.h"
#include "../../../sdk/utils/Utils.h"
#include "../../../sdk/memory/PatternScan.h"
#include "../../../sdk/memory/Offsets.h"
#include "../../../sdk/utils/Globals.h"

#include <Windows.h>

void Aimbot::run() 
{

	if (!Globals::aimbot_enabled) return;

    C_CSPlayerPawn* local = EntityManager::Get().GetLocalPawn();
    if (!local || !local->IsAlive()) return;

    if (!(GetAsyncKeyState(Globals::aimbot_key) & 0x8000)) return;


	// select best player via a FoV premise/x,y based
    C_CSPlayerPawn* bestTarget = getBestTarget(local);
    if (!bestTarget) return;

    aimAtTarget(local, bestTarget);
}

void Aimbot::aimAtTarget(C_CSPlayerPawn* local, C_CSPlayerPawn* target)
{
    if (!local || !target) return;


	BoneID targetBone = findNearestBoneId(local, target);
	std::cout << "this is target bone: " << (int)targetBone << '\n';
	// tchange bone pos
	Vector targetPos = Utils::GetBonePos(target, targetBone);
    if (targetPos.IsZero()) return;

	Vector localPos = local->m_vOldOrigin() + local->m_vecViewOffset();
    Vector aimAngles = Utils::CalcAngle(localPos, targetPos);

    uintptr_t client = Memory::GetModuleBase("client.dll");
    if (!client) return;

	// from client we want to get the current angle
    Vector* currentAngles = reinterpret_cast<Vector*>(client + Offsets::dwViewAngles);
	
	if (!currentAngles) return;

    if (Globals::aimbot_smooth)
    {
        Vector delta = aimAngles - *currentAngles;
		Utils::NormalizeAngles(delta);

		// this is going to be how we traverse through certian FOVs
        *currentAngles += delta * (1.f-Globals::aimbot_smoothness);
    }
    else 
    {
			
        *currentAngles = aimAngles;
    }

}

BoneID Aimbot::findNearestBoneId(C_CSPlayerPawn* local, C_CSPlayerPawn* target)
{
	if (!local || !target) return BoneID::Head;

	constexpr BoneID iterateBones[] = {
		BoneID::Head,
		BoneID::Neck,
		BoneID::Spine,
		BoneID::Stomach,
		BoneID::LeftShoulder,
		BoneID::RightShoulder,
	};

	
	bool validBaim = Globals::aimbot_force_baim && target->m_iHealth() <= Globals::aimbot_baim_min;

	int start = validBaim ? 2 : 0; // 0 is head, 1 is neck, 2 is spine ...

	uintptr_t client = Memory::GetModuleBase("client.dll");
	if (!client) return iterateBones[start];

	Vector* currentAngles = reinterpret_cast<Vector*>(client + Offsets::dwViewAngles);
	if (!currentAngles) return iterateBones[start];

	Vector localPos = local->m_vOldOrigin() + local->m_vecViewOffset();

	BoneID bestBone = iterateBones[start];
	float bestFov = FLT_MAX;

	for (int i = start; i < sizeof(iterateBones) / sizeof(BoneID); i++)
	{
		Vector bonePos = Utils::GetBonePos(target, iterateBones[i]);
		if (bonePos.IsZero()) continue;

		Vector aimAngles = Utils::CalcAngle(localPos, bonePos);
		float fov = Utils::GetFoV(*currentAngles, aimAngles);

		// penalize lower bone structs
		float penalty = i * 0.15f;
		float adjustedFov = fov + penalty;

		if (adjustedFov < bestFov)
		{
			bestFov = adjustedFov;
			bestBone = iterateBones[i];
		}
	}

	return bestBone;
}

C_CSPlayerPawn* Aimbot::getBestTarget(C_CSPlayerPawn* local)
{
	// grab all the entities
    const auto& entities = EntityManager::Get().GetEntities();
    C_CSPlayerPawn* bestTarget = nullptr;
	float bestDistance = Globals::aimbot_fov;

    uintptr_t client = Memory::GetModuleBase("client.dll");
    if (!client) return nullptr;

	// get current angles
    Vector* currentAngles = reinterpret_cast<Vector*>(client + Offsets::dwViewAngles);
    if (!currentAngles) return nullptr;

    for (const auto& ent : entities) 
    {
		// base case
		bool isTeammate = !ent.isEnemy;
		if (isTeammate && !Globals::aimbot_friendly_fire) continue;


		// find the pos
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

