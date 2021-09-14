#pragma once

namespace SDK
{
	class IVDebugOverlay
	{
	public:
		int ScreenPosition(Vector& point, Vector& screen)
		{
			typedef int (__thiscall *Fn)(void*, Vector&, Vector&);
			return VMT::GetVFunction<Fn>(this, 13)(this, point, screen);
		}

		void DrawPill(Vector& mins, Vector& maxs, float pillradius, int r, int g, int b, int a, float duration) 
		{
			VMT::GetVFunction<void(__thiscall*)(void*, Vector&, Vector&, float&, int, int, int, int, float)>(this, 24)(this, mins, maxs, pillradius, r, g, b, a, duration);
		}
	};
}
