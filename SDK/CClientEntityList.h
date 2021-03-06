#pragma once

#include "../UTILS/vmt.h"
#include "CBaseEntity.h"

namespace SDK
{
	class CBaseEntity;

	class CClientEntityList
	{
	public:

		CBaseEntity* GetClientEntity(int index)
		{
			typedef CBaseEntity*(__thiscall* Fn)(void*, int);
			return VMT::GetVFunction<Fn>(this, 3)(this, index);
		}

		int GetHighestEntityIndex()
		{
			typedef int (__thiscall* Fn)(void*);
			return VMT::GetVFunction<Fn>(this, 8)(this);
		}

		CBaseEntity* GetClientEntityFromHandle(void* handle)
		{
			typedef CBaseEntity*(__thiscall* Fn)(void*, void*);
			return VMT::GetVFunction<Fn>(this, 4)(this, handle); //7
		}
	};
}
