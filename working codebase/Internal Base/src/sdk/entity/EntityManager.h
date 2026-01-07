#pragma once
#include <vector>
#include <shared_mutex>
#include "Classes.h"

struct Entity_t
{
    C_CSPlayerController* controller = nullptr;
    C_CSPlayerPawn* pawn = nullptr;
    int index = -1;
    bool isEnemy = false;
};

class EntityManager
{
private:
    std::vector<Entity_t> entities;
    std::shared_mutex mutex;
    uintptr_t entityListAddress = 0;
    C_CSPlayerPawn* localPawn = nullptr;

    EntityManager();

public:
    static EntityManager& Get();

    void Update();

    C_CSPlayerPawn* GetLocalPawn();
    const std::vector<Entity_t>& GetEntities();
    C_CSPlayerPawn* GetPawnFromHandle(uint32_t handle);
};
