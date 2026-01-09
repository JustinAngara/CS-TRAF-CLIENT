#pragma once
#include "../../../sdk/entity/EntityManager.h"
#include "../../../sdk/utils/Utils.h"

class Aimbot
{
public:
	static void run();

private:
	static void aimAtTarget(C_CSPlayerPawn* local, C_CSPlayerPawn* target);
	static C_CSPlayerPawn* getBestTarget(C_CSPlayerPawn* local);
	static BoneID findNearestBoneId(C_CSPlayerPawn* local, C_CSPlayerPawn* target); 
};
