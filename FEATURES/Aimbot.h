#pragma once

#include "Backtracking.h"

namespace SDK
{
	class CUserCmd;
	class CBaseEntity;

	struct mstudiobbox_t;
}

namespace FEATURES
{
	namespace RAGEBOT
	{
		class Aimbot
		{
		public:
			void Do(SDK::CUserCmd* cmd);

			void DrawShotHitboxes(SDK::CBaseEntity* entity, CColor color, float duration) const;
		private:
			struct PlayerAimbotInfo
			{
				int head_damage = 0, hitscan_damage = 0, tick, extrapolted_ticks;
				bool m_is_foot;
				Vector head_position, hitscan_position;
				FEATURES::RAGEBOT::Backtracking_Record backtrack_record;
			};

		private:
			PlayerAimbotInfo player_aimbot_info[64];

		public:
			PlayerAimbotInfo GetPlayerAimbotInfo(SDK::CBaseEntity* entity)
			{
				return player_aimbot_info[entity->GetIndex()];
			}

		private:
			bool DoHitscan(SDK::CBaseEntity* entity, const FEATURES::RAGEBOT::Backtracking_Record& bt_rec, Vector& final_position, int& final_damage);
			bool DoHeadAim(SDK::CBaseEntity* entity, const FEATURES::RAGEBOT::Backtracking_Record& bt_rec, Vector& final_position, int& final_damage);

			bool IsAccurate(SDK::CBaseEntity* entity, Vector position) const;

			bool DoKnifebot(SDK::CBaseEntity* entity, const FEATURES::RAGEBOT::Backtracking_Record& bt_rec, Vector& final_position);

			/// helper functions
			bool GetMultipointPositions(SDK::CBaseEntity* entity, const FEATURES::RAGEBOT::Backtracking_Record& bt_rec, Vector* positions, int hitbox_index, float pointscale);
			Vector GetHitboxPosition(SDK::CBaseEntity* entity, const FEATURES::RAGEBOT::Backtracking_Record& bt_rec, int hitbox_index) const;
			static SDK::mstudiobbox_t* GetHitbox(SDK::CBaseEntity* entity, int hitbox_index);
		};

		extern Aimbot aimbot;
	}
}
