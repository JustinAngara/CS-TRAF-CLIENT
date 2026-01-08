#pragma once
#include "../../../sdk/entity/EntityManager.h"
#include "../../../sdk/utils/Utils.h"

namespace Aimbot
{
	// settings
	void Run();
	void aimAtTarget(C_CSPlayerPawn* local, C_CSPlayerPawn* target);
	C_CSPlayerPawn* getBestTarget(C_CSPlayerPawn* local);
}
