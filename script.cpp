#include "inc\natives.h"
#include "inc\main.h"
#include <fstream>
#include <string>

namespace
{
    constexpr DWORD kToggleCooldownMs = 750;
    constexpr const char* kConfigFile = ".\\godmode_config.txt";

    bool  g_godModeEnabled = false;
    bool  g_wasF4Down = false;
    DWORD g_lastToggleTime = 0;

    void ShowNotification(const char* message)
    {
        UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
        UI::_ADD_TEXT_COMPONENT_STRING(const_cast<char*>(message));
        UI::_DRAW_NOTIFICATION(false, false);
    }

    void SaveGodModeState()
    {
        std::ofstream file(kConfigFile);
        if (file.is_open())
        {
            file << (g_godModeEnabled ? "1" : "0");
            file.close();
        }
    }

    void LoadGodModeState()
    {
        std::ifstream file(kConfigFile);
        if (file.is_open())
        {
            std::string line;
            if (std::getline(file, line))
            {
                g_godModeEnabled = (line == "1");
            }
            file.close();
        }
    }

    void ApplyStrongGodmodeTick()
    {
        const Ped playerPed = PLAYER::PLAYER_PED_ID();
        if (!ENTITY::DOES_ENTITY_EXIST(playerPed))
            return;

        // Keep invincibility set (some mission effects can still reduce health, so clamp too)
        ENTITY::SET_ENTITY_INVINCIBLE(playerPed, true);

        // Reduce a couple of edge-case damage outcomes (safe for SP)
        PED::SET_PED_SUFFERS_CRITICAL_HITS(playerPed, false);

        // Clamp health back to max if anything bypasses invincibility (e.g., Grass Roots beam)
        const int maxHp = ENTITY::GET_ENTITY_MAX_HEALTH(playerPed);
        const int curHp = ENTITY::GET_ENTITY_HEALTH(playerPed);
        if (curHp < maxHp)
        {
            ENTITY::SET_ENTITY_HEALTH(playerPed, maxHp);
        }

        // Keep armor topped up (optional but commonly expected with godmode)
        const int curArmor = PED::GET_PED_ARMOUR(playerPed);
        if (curArmor < 100)
        {
            PED::SET_PED_ARMOUR(playerPed, 100);
        }
    }

    void ToggleGodMode()
    {
        g_godModeEnabled = !g_godModeEnabled;

        const Ped playerPed = PLAYER::PLAYER_PED_ID();
        if (ENTITY::DOES_ENTITY_EXIST(playerPed))
        {
            ENTITY::SET_ENTITY_INVINCIBLE(playerPed, g_godModeEnabled);

            // If enabling, immediately apply the stronger protections once
            if (g_godModeEnabled)
            {
                ApplyStrongGodmodeTick();
            }
        }

        SaveGodModeState();
        ShowNotification(g_godModeEnabled ? "God mode enabled" : "God mode disabled");
    }
}

void ScriptMain()
{
    LoadGodModeState();

    // If config loaded as enabled, apply immediately on script start
    if (g_godModeEnabled)
    {
        const Ped playerPed = PLAYER::PLAYER_PED_ID();
        if (ENTITY::DOES_ENTITY_EXIST(playerPed))
        {
            ENTITY::SET_ENTITY_INVINCIBLE(playerPed, true);
        }
    }

    while (true)
    {
        const bool isF4Down = (GetAsyncKeyState(VK_F4) & 0x8000) != 0;
        const DWORD now = GetTickCount();

        if (isF4Down && !g_wasF4Down && (now - g_lastToggleTime) >= kToggleCooldownMs)
        {
            g_lastToggleTime = now;
            ToggleGodMode();
        }

        if (g_godModeEnabled)
        {
            ApplyStrongGodmodeTick();
        }

        g_wasF4Down = isF4Down;
        WAIT(0);
    }
}
