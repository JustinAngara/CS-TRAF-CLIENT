#pragma once
#include <cstdint>


//achieve dumper via https://github.com/a2x/cs2-dumper
namespace Offsets
{
    // -> offsets.h on a2x dumper
    constexpr uintptr_t dwEntityList = 0x1D13CE8;
    constexpr uintptr_t dwLocalPlayerPawn = 0x1BEEF28;
    constexpr uintptr_t dwViewMatrix = 0x1E323D0;

    
	// -> client.dll.hpp on a2x dumper -> C_BaseEntity 
    constexpr uintptr_t m_iHealth = 0x34C;
    constexpr uintptr_t m_iTeamNum = 0x3EB;
    constexpr uintptr_t m_vOldOrigin = 0x15A0;
    constexpr uintptr_t m_pGameSceneNode = 0x330;

	// -> CbaseModelEntity
    constexpr uintptr_t m_vecViewOffset = 0xD80;

	// -> C_CSPlayerPawn
    constexpr uintptr_t m_iShotsFired = 0x272C;
    constexpr uintptr_t m_aimPunchAngle = 0x16E4;
    constexpr uintptr_t m_fFlags = 0x4C4;  // Player movement flags



	// -> C_CSPlayerController
    constexpr uintptr_t m_iszPlayerName = 0x6E8;
    constexpr uintptr_t m_hPlayerPawn = 0x8FC;
    constexpr uintptr_t m_bPawnIsAlive = 0x7EC;

	// -> CBodyComponentSkeletonInstance
    constexpr uintptr_t m_modelState = 0x190;

	// -> CObserverServices
    constexpr std::ptrdiff_t m_pObserverServices = 0x1408;
    constexpr std::ptrdiff_t m_hObserverTarget = 0x44;
	constexpr std::ptrdiff_t m_iObserverMode = 0x40;

}
