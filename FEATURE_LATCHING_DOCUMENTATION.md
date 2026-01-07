# Feature Latching and Game Tick Architecture Documentation

## Overview

This documentation explains how the Asphyxia CS2 cheat framework handles feature execution within game ticks and how to properly latch onto new features. The architecture is designed around a central hook system that provides consistent game state updates and feature execution timing.

## Core Architecture

### Game Tick Hook System

The entire cheat operates through a single primary hook: **`hkPresent`** (DirectX Present hook). This hook is called every frame when the game renders, making it the ideal location for:

- Entity list updates
- Feature execution
- Menu rendering
- State synchronization

**Location**: `src/core/Hooks.cpp` (lines 39-98)

```cpp
HRESULT __stdcall Hooks::hkPresent(IDXGISwapChain* swapChain, UINT sync, UINT flags)
{
    // Entity updates happen first
    EntityManager::Get().Update();
    
    // View matrix update for world-to-screen calculations
    uintptr_t client = Memory::GetModuleBase("client.dll");
    memcpy(&Globals::ViewMatrix, (void*)(client + Offsets::dwViewMatrix), sizeof(Globals::ViewMatrix));
    
    // Feature execution
    Visuals::Render();
    Misc::Render();
    
    // Menu rendering
    if (Menu::IsOpen)
        Menu::Render();
}
```

### Feature Execution Order

Features are executed in a specific order each tick:

1. **Entity System Update** - `EntityManager::Get().Update()`
2. **Visual Features** - `Visuals::Render()` (ESP, EnemyCounter, etc.)
3. **Misc Features** - `Misc::Render()` (Bhop, etc.)
4. **Menu System** - `Menu::Render()` (if open)

## Feature Categories and Structure

### 1. Visual Features (`src/feature/visuals/`)

Visual features are rendering-based and execute every frame in the render loop.

**Structure**:
```
src/feature/visuals/
├── Visuals.h/.cpp          // Main visual feature coordinator
├── esp/
│   ├── Esp.h/.cpp          // ESP rendering
├── enemycounter/
│   ├── EnemyCounter.h/.cpp // Enemy counter display
```

**Execution Pattern**:
```cpp
// In Visuals::Render()
void Visuals::Render()
{
    ESP::Render();
    EnemyCounter::Render();
}
```

### 2. Misc Features (`src/feature/misc/`)

Misc features include gameplay modifications like bunny hop.

**Structure**:
```
src/feature/misc/
├── Misc.h/.cpp            // Main misc feature coordinator
├── bhop/
│   ├── Bhop.h/.cpp        // Bunny hop implementation
```

**Current Implementation**:
```cpp
// In Misc::Render() - currently placeholder
void Misc::Render()
{
    // Placeholder for miscellaneous rendering features
}

void Misc::Run() {
    // This is where we want to run bhop
    std::cout << "i am in misc\n";
}
```

## Entity Management System

### EntityManager Class

The `EntityManager` is a singleton that maintains the current game state.

**Location**: `src/sdk/entity/EntityManager.h/.cpp`

**Key Methods**:
- `Update()` - Refreshes entity list each tick
- `GetLocalPawn()` - Returns your player entity
- `GetEntities()` - Returns vector of all valid entities
- `GetPawnFromHandle()` - Converts entity handles to pointers

**Entity Structure**:
```cpp
struct Entity_t
{
    C_CSPlayerController* controller = nullptr;  // Player controller
    C_CSPlayerPawn* pawn = nullptr;             // Player pawn (actual character)
    int index = -1;                              // Entity index
    bool isEnemy = false;                        // Team relationship
};
```

### Entity Update Process

Each tick, the EntityManager:
1. Reads the entity list from game memory
2. Iterates through all 64 possible player slots
3. Validates controllers and pawns
4. Filters out dead players and local player
5. Updates the thread-safe entity vector

## Adding New Features

### Step 1: Create Feature Structure

For a new feature (example: "Aimbot"):

1. Create directory: `src/feature/combat/aimbot/`
2. Create header: `src/feature/combat/aimbot/Aimbot.h`
3. Create implementation: `src/feature/combat/aimbot/Aimbot.cpp`

### Step 2: Implement Feature Interface

**Header File** (`Aimbot.h`):
```cpp
#pragma once

namespace Aimbot {
    void Render();  // For visual elements
    void Run();     // For logic execution
}
```

**Implementation File** (`Aimbot.cpp`):
```cpp
#include "Aimbot.h"
#include "../../../sdk/entity/EntityManager.h"
#include "../../../sdk/utils/Globals.h"
#include "../../../sdk/utils/Utils.h"

void Aimbot::Render()
{
    // Draw aimbot visual elements (FOV circle, etc.)
}

void Aimbot::Run()
{
    // Main aimbot logic
    C_CSPlayerPawn* localPawn = EntityManager::Get().GetLocalPawn();
    if (!localPawn) return;
    
    const auto& entities = EntityManager::Get().GetEntities();
    // ... aimbot logic here
}
```

### Step 3: Register Feature in Coordinator

**For Combat Features**:
Add to `src/feature/combat/Combat.cpp` (create if needed):
```cpp
#include "Combat.h"
#include "aimbot/Aimbot.h"

void Combat::Render()
{
    Aimbot::Render();
}

void Combat::Run()
{
    Aimbot::Run();
}
```

### Step 4: Hook into Main Loop

**In `Hooks.cpp`**:
```cpp
// Add include
#include "../../src/feature/combat/Combat.h"

// In hkPresent function:
HRESULT __stdcall Hooks::hkPresent(IDXGISwapChain* swapChain, UINT sync, UINT flags)
{
    // ... existing code ...
    
    EntityManager::Get().Update();
    
    // Add feature execution
    Combat::Run();        // For logic-based features
    Visuals::Render();    // For visual features
    Misc::Run();          // For misc features
    
    // ... rest of function ...
}
```

## Key Variables and Their Meanings

### Globals (`src/sdk/utils/Globals.h`)

**Visual Settings**:
```cpp
// ESP Settings
inline bool esp_enabled = true;           // Master ESP toggle
inline int esp_bind = VK_F1;              // ESP toggle keybind
inline bool esp_box = true;               // Show bounding boxes
inline float esp_box_color[4] = {1.f, 0.f, 0.f, 1.f};  // Box color (RGBA)
inline float esp_box_thickness = 1.5f;    // Box outline thickness
inline bool esp_skeleton = true;          // Show skeleton
inline float esp_skeleton_color[4] = {1.f, 1.f, 1.f, 0.9f};  // Skeleton color
inline float esp_skeleton_thickness = 1.8f; // Skeleton line thickness
inline bool esp_name = true;              // Show player names
inline float esp_name_color[4] = {1.f, 1.f, 1.f, 1.f};  // Name color
inline bool esp_health = true;            // Show health bars
```

**Display Settings**:
```cpp
inline float ViewMatrix[16] = {0.f};      // Game's view-projection matrix
inline int ScreenWidth = 0;               // Current screen width
inline int ScreenHeight = 0;               // Current screen height
```

### Offsets (`src/sdk/memory/Offsets.h`)

**Entity List Offsets**:
```cpp
constexpr uintptr_t dwEntityList = 0x1D13CE8;      // Base entity list address
constexpr uintptr_t dwLocalPlayerPawn = 0x1BEEF28; // Local player pawn pointer
constexpr uintptr_t dwViewMatrix = 0x1E323D0;     // View matrix address
```

**Entity Class Offsets**:
```cpp
// C_BaseEntity
constexpr uintptr_t m_iHealth = 0x34C;           // Player health
constexpr uintptr_t m_iTeamNum = 0x3EB;          // Team number
constexpr uintptr_t m_vOldOrigin = 0x15A0;       // Player position
constexpr uintptr_t m_pGameSceneNode = 0x330;    // Scene node pointer

// C_CSPlayerPawn  
constexpr uintptr_t m_vecViewOffset = 0xD80;     // Camera view offset
constexpr uintptr_t m_iShotsFired = 0x272C;      // Shots fired counter
constexpr uintptr_t m_aimPunchAngle = 0x16E4;    // Aim punch/recoil

// C_CSPlayerController
constexpr uintptr_t m_iszPlayerName = 0x6E8;     // Player name string
constexpr uintptr_t m_hPlayerPawn = 0x8FC;       // Pawn handle
constexpr uintptr_t m_bPawnIsAlive = 0x7EC;      // Pawn alive status
```

### Utility Classes

**Vector** (`src/sdk/utils/Vector.h`):
```cpp
struct Vector {
    float x, y, z;  // 3D position/rotation
    
    // Operations
    Vector operator+(const Vector& o) const;
    Vector operator-(const Vector& o) const;
    float Length() const;                    // Distance from origin
    bool IsZero() const;                      // Check if all components are 0
};
```

**QAngle** (`src/sdk/utils/Utils.h`):
```cpp
struct QAngle {
    float pitch, yaw, roll;  // Camera angles
    
    void Normalize();        // Clamp angles to valid ranges
};
```

**BoneID Enum**:
```cpp
enum class BoneID : int {
    Head = 6, Neck = 5, Spine = 4, Pelvis = 0,
    LeftShoulder = 8, LeftArm = 9, LeftHand = 11,
    RightShoulder = 13, RightArm = 14, RightHand = 16,
    LeftHip = 22, LeftKnee = 23, LeftFoot = 24,
    RightHip = 25, RightKnee = 26, RightFoot = 27
};
```

## Game Tick Utilization Patterns

### Pattern 1: Visual Features (Every Frame)

Visual features should run every render frame for smooth display:

```cpp
void MyFeature::Render()
{
    if (!feature_enabled) return;
    
    // Get current game state
    const auto& entities = EntityManager::Get().GetEntities();
    C_CSPlayerPawn* localPawn = EntityManager::Get().GetLocalPawn();
    
    // Render visual elements
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    // ... drawing code ...
}
```

### Pattern 2: Logic Features (Every Frame)

Logic features that need continuous updates:

```cpp
void MyFeature::Run()
{
    if (!feature_enabled) return;
    
    C_CSPlayerPawn* localPawn = EntityManager::Get().GetLocalPawn();
    if (!localPawn) return;
    
    // Check keybinds
    if (!(GetAsyncKeyState(feature_bind) & 0x8000)) return;
    
    // Execute feature logic
    // ... feature code ...
}
```

### Pattern 3: Event-Driven Features

Features that respond to specific game events:

```cpp
void MyFeature::Run()
{
    static int lastHealth = 100;
    C_CSPlayerPawn* localPawn = EntityManager::Get().GetLocalPawn();
    
    if (!localPawn) return;
    
    int currentHealth = localPawn->m_iHealth();
    
    // Detect health change
    if (currentHealth != lastHealth) {
        OnHealthChanged(currentHealth, lastHealth);
        lastHealth = currentHealth;
    }
}
```

## Memory Access Patterns

### Safe Memory Reading

Always validate pointers before accessing:

```cpp
// Safe pattern
C_CSPlayerPawn* pawn = GetPawnFromHandle(handle);
if (!pawn || !pawn->IsAlive()) return;

// Safe string reading
char nameBuf[64];
uintptr_t namePtr = reinterpret_cast<uintptr_t>(controller->m_iszPlayerName());
if (Utils::SafeReadString(namePtr, nameBuf)) {
    // Use name
}
```

### Entity Iteration Pattern

```cpp
const auto& entities = EntityManager::Get().GetEntities();
C_CSPlayerPawn* localPawn = EntityManager::Get().GetLocalPawn();

for (const auto& ent : entities) {
    C_CSPlayerPawn* pawn = ent.pawn;
    if (!pawn || !pawn->IsAlive()) continue;
    
    // Check team relationship
    if (pawn->m_iTeamNum() == localPawn->m_iTeamNum()) continue;
    
    // Process enemy
}
```

## Best Practices

### 1. Thread Safety
- Use `EntityManager` for thread-safe entity access
- Never cache entity pointers across ticks
- Always re-fetch entities each tick

### 2. Performance
- Early return in feature functions when disabled
- Minimize expensive operations per tick
- Use ImGui's background draw list for overlays

### 3. Memory Safety
- Always validate pointers before dereferencing
- Use `Utils::IsValidPtr()` for pointer validation
- Handle exceptions when reading game memory

### 4. Feature Organization
- Group related features in categories
- Use consistent naming conventions
- Separate visual and logic components

## Example: Complete Bunny Hop Implementation

Here's how to properly implement the bhop feature:

**`Bhop.h`**:
```cpp
#pragma once

namespace Bhop {
    void Run();
}
```

**`Bhop.cpp`**:
```cpp
#include "Bhop.h"
#include "../../../sdk/memory/PatternScan.h"
#include "../../../sdk/memory/Offsets.h"
#include <Windows.h>

namespace Bhop {
    void Run() {
        C_CSPlayerPawn* localPawn = EntityManager::Get().GetLocalPawn();
        if (!localPawn) return;

        // Check if player is on ground (you'll need to find the correct offset)
        bool onGround = /* read ground state */;
        
        // Check if jump key is pressed
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            if (onGround) {
                // Force jump
                // Write to jump offset
            }
        }
    }
}
```

**Update `Misc.cpp`**:
```cpp
void Misc::Run() {
    Bhop::Run();
}
```

This architecture ensures all features execute in the same game tick with consistent game state, making it easy to add new features that work together seamlessly.
