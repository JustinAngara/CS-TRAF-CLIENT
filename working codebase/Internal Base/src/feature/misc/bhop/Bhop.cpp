#include <windows.h>
#include "../../../sdk/entity/EntityManager.h"
#include "../../../sdk/memory/PatternScan.h"
#include "../../../sdk/memory/Offsets.h"
#include <chrono>
#include "Bhop.h"

namespace Bhop
{
	static bool jumpActive = false;
	static std::chrono::steady_clock::time_point lastActionTime = std::chrono::steady_clock::now();
	static const int jumpDelayMs = 10;

	const int FORCE_JUMP_ACTIVE = 65537;
	const int FORCE_JUMP_INACTIVE = 256;

	void Run()
	{
		auto& em = EntityManager::Get();
		C_CSPlayerPawn* local = em.GetLocalPawn();
		if (!local || !local->IsAlive())
		{
			if (jumpActive)
				jumpActive = false;
			return;
		}

		bool keyPressed = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
		if (!keyPressed)
		{
			if (jumpActive)
				jumpActive = false;
			return;
		}

		uintptr_t client = Memory::GetModuleBase("client.dll");
		if (!client)
			return;

		uintptr_t jumpAddress = client + Offsets::jump;
		uint32_t* jumpButton = reinterpret_cast<uint32_t*>(jumpAddress);

		auto currentTime = std::chrono::steady_clock::now();
		auto timeSinceLastAction = std::chrono::duration_cast<std::chrono::milliseconds>(
		currentTime - lastActionTime);

		if (timeSinceLastAction.count() >= jumpDelayMs)
		{
			if (!jumpActive)
			{
				*jumpButton = FORCE_JUMP_ACTIVE;
				jumpActive = true;
				lastActionTime = currentTime;
			}
			else
			{
				*jumpButton = FORCE_JUMP_INACTIVE;
				jumpActive = false;
				lastActionTime = currentTime;
			}
		}
	}
}
