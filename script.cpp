#include "inc\natives.h"
#include "inc\main.h"
#include <fstream>
#include <string>
namespace
{
	constexpr DWORD kToggleCooldownMs = 750;
	constexpr const char* kConfigFile = ".\\godmode_config.txt";
	bool g_godModeEnabled = false;
	bool g_wasF4Down = false;
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

	void ToggleGodMode()
	{
		g_godModeEnabled = !g_godModeEnabled;

		const Ped playerPed = PLAYER::PLAYER_PED_ID();
		if (ENTITY::DOES_ENTITY_EXIST(playerPed))
		{
			ENTITY::SET_ENTITY_INVINCIBLE(playerPed, g_godModeEnabled);
		}

		SaveGodModeState();
		ShowNotification(g_godModeEnabled ? "God mode enabled" : "God mode disabled");
	}
}

void ScriptMain()
{
	LoadGodModeState();

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
			const Ped playerPed = PLAYER::PLAYER_PED_ID();
			if (ENTITY::DOES_ENTITY_EXIST(playerPed))
			{
				ENTITY::SET_ENTITY_INVINCIBLE(playerPed, true);
			}
		}

		g_wasF4Down = isF4Down;
		WAIT(0);
	}
}
