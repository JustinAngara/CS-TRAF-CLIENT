#pragma once

namespace Aimbot
{
	// Settings
	inline bool aimbot_enabled = true;
	inline int aimbot_key = VK_F2; // Right mouse button
	inline float aimbot_fov = 15.0f;
	inline bool aimbot_smooth = true;
	inline float aimbot_smoothness = 0.1f;

	void Run();
	void AimAtTarget(C_CSPlayerPawn* local, C_CSPlayerPawn* target);
	C_CSPlayerPawn* GetBestTarget(C_CSPlayerPawn* local);
}
