#pragma once
#include <cstdint>
#include <Windows.h>

namespace Globals
{
    inline float ViewMatrix[16] = { 0.f };
    inline int ScreenWidth = 0;
    inline int ScreenHeight = 0;

    inline bool esp_enabled = true;
    inline int  esp_bind = VK_F1;

    inline bool esp_box = true;
    inline float esp_box_color[4] = { 1.f, 0.f, 0.f, 1.f };
    inline float esp_box_thickness = 1.5f;

    inline bool esp_skeleton = true;
    inline float esp_skeleton_color[4] = { 1.f, 1.f, 1.f, 0.9f };
    inline float esp_skeleton_thickness = 1.8f;

    inline bool esp_name = true;
    inline float esp_name_color[4] = { 1.f, 1.f, 1.f, 1.f };

    inline bool esp_health = true;

}
