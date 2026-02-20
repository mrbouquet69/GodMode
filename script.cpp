#include "inc\natives.h"
#include "inc\main.h"
namespace
{
	constexpr DWORD kToggleCooldownMs = 750;
	bool g_godModeEnabled = false;
	bool g_wasF4Down = false;
	DWORD g_lastToggleTime = 0;

	void ShowNotification(const char* message)
	{
		UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
		UI::_ADD_TEXT_COMPONENT_STRING(const_cast<char*>(message));
		UI::_DRAW_NOTIFICATION(false, false);
	}

	void ToggleGodMode()
	{
		g_godModeEnabled = !g_godModeEnabled;

		const Ped playerPed = PLAYER::PLAYER_PED_ID();
		if (ENTITY::DOES_ENTITY_EXIST(playerPed))
		{
			ENTITY::SET_ENTITY_INVINCIBLE(playerPed, g_godModeEnabled);
		}

		ShowNotification(g_godModeEnabled ? "God mode enabled" : "God mode disabled");
	}
}

void ScriptMain()
{
	while (true)
	{
		const bool isF4Down = (GetAsyncKeyState(VK_F4) & 0x8000) != 0;
		const DWORD now = GetTickCount();

		if (isF4Down && !g_wasF4Down && (now - g_lastToggleTime) >= kToggleCooldownMs)
		{
			g_lastToggleTime = now;
			ToggleGodMode();
		}

		g_wasF4Down = isF4Down;
		WAIT(0);
	}
}