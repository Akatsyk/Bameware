#include "includes.h"

#include "UTILS\interfaces.h"
#include "UTILS\hooks.h"
#include "UTILS\offsets.h"
#include "UTILS\NetvarManager.h"
#include "FEATURES\EventListeners.h"
#include "FEATURES\Misc.h"

#include "SDK\IEngine.h"

#include "UTILS\Debugger.h"

#include "UTILS\hooks.h"

void Start()
{
	INTERFACES::InitInterfaces();
	OFFSETS::InitOffsets();

	UTILS::INPUT::input_handler.Init();
	FONTS::InitFonts();

	HOOKS::InitHooks();
	HOOKS::InitNetvarHooks();
	FEATURES::MISC::InitializeEventListeners();

	FEATURES::MISC::LoadUserConfig();

	GLOBAL::cheat_start_time = GetTickCount() * 0.001f;

	std::cout << std::hex << GetProcAddress(GetModuleHandle("tier0.dll"), "Msg") << std::endl;
}

BOOL WINAPI DllMain(HINSTANCE Instance, DWORD Reason, LPVOID Reserved)
{
	switch (Reason)
	{
		case DLL_PROCESS_ATTACH:
		{
			CreateDirectoryA(enc_char("C:\\Bameware"), NULL);
			CreateDirectoryA(enc_char("C:\\Bameware\\Logs"), NULL);
			CreateDirectoryA(enc_char("C:\\Bameware\\Configs"), NULL);
			CreateDirectoryA(enc_char("C:\\Bameware\\Resources"), NULL);
			CreateDirectoryA(enc_char("C:\\Bameware\\Resources\\Fonts"), NULL);

			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Start, NULL, NULL, NULL);
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			LOG("DETACHING");
#ifdef DEV_MODE
			{
				fclose(stdin);
				fclose(stdout);
				FreeConsole();
			}
#endif

			HOOKS::UnHook();

			break;
		}
	}

	return true;
}
