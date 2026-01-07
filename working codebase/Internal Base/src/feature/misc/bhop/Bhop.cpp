#include <windows.h>
#include "../../../sdk/entity/EntityManager.h"
#include "../../../sdk/memory/PatternScan.h"
#include <iostream>
namespace Bhop {
	void performJump();
	void Run()
	{
		std::cout << " i am in run\n";
		// create a reference to a new entitymanager obj
		auto& em = EntityManager::Get();
		// take the reference of local pawn
		C_CSPlayerPawn* local = em.GetLocalPawn();

		if (!local || !local->IsAlive()) return;
		if (!(GetAsyncKeyState(VK_SPACE) & 0x8000)) return;

		bool onGround = local->IsOnGround();

		if (onGround) {
			performJump();
		}
	}

	void performJump()
	{
		uintptr_t client = Memory::GetModuleBase("client.dll");
		if (client) {
			// constexpr uintptr_t dwForceJump = 0x1435; 
			// *reinterpret_cast<int*>(client + dwForceJump) = 5;
		}
	}
}
