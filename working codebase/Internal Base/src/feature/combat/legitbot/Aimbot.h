#pragma once
#include "../../../sdk/entity/EntityManager.h"
#include "../../../sdk/utils/Utils.h"

namespace Aimbot
{
	// settings

	void Run();
	void AimAtTarget(C_CSPlayerPawn* local, C_CSPlayerPawn* target);
	C_CSPlayerPawn* GetBestTarget(C_CSPlayerPawn* local);
}
