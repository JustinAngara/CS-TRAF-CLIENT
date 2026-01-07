#pragma once
#include "../../../sdk/entity/EntityManager.h"
#include "../../../sdk/utils/Utils.h"

namespace Aimbot
{
	// settings
	inline bool aimbot_enabled = true;
	inline int aimbot_key = VK_LBUTTON; 
	inline float aimbot_fov = 15.0f;
	inline bool aimbot_smooth = true;
	inline float aimbot_smoothness = 0.1f;

	void Run();
	void AimAtTarget(C_CSPlayerPawn* local, C_CSPlayerPawn* target);
	C_CSPlayerPawn* GetBestTarget(C_CSPlayerPawn* local);
}
