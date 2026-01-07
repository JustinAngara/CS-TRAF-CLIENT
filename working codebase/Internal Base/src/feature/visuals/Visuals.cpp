#include "Visuals.h"
#include "esp/Esp.h"
#include "enemycounter/EnemyCounter.h"

void Visuals::Render()
{
    ESP::Render();
	EnemyCounter::Render();
}