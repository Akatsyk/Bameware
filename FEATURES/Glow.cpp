#include "../includes.h"

#include "../UTILS/offsets.h"
#include "../UTILS/interfaces.h"
#include "../SDK/CGlowObjectManager.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/player_info_t.h"
#include "../SDK/IEngine.h"
#include "../SDK/CClientEntityList.h"

#include "Glow.h"

#include "Configurations.h"

namespace FEATURES
{
	namespace VISUALS
	{
		CGlow glow;

		void CGlow::DoSceneEnd()
		{
			if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
			{
				if (!SETTINGS::main_configs.glow_local_enabled)
					return; 

				for (auto i = 0; i < INTERFACES::GlowObjManager->GetSize(); i++)
				{
					auto& glowObject = INTERFACES::GlowObjManager->m_GlowObjectDefinitions[i];
					auto entity = reinterpret_cast<SDK::CBaseEntity*>(glowObject.m_pEntity);
					auto m_pLocalPlayer = reinterpret_cast<SDK::CBaseEntity*>(INTERFACES::ClientEntityList->GetClientEntityFromHandle((void*)INTERFACES::Engine->GetLocalPlayer()));

					if (!entity)
						continue;

					if (!m_pLocalPlayer)
						continue;

					if (glowObject.IsUnused())
						continue;

					if (entity->GetClientClass()->m_ClassID != 35)
						continue;


					bool is_local_player = entity == m_pLocalPlayer;
					bool is_teammate = m_pLocalPlayer->GetTeam() == entity->GetTeam() && !is_local_player;
					bool is_enemy = m_pLocalPlayer->GetTeam() != entity->GetTeam();

					CColor color = NOCOLOR;
					bool full_bloom = false;
					int style = 1;

					bool should_draw_glow = false;
					/*if (entity->GetClientClass()->m_ClassID == 1 && entity->GetClientClass()->m_ClassID == 39 && strstr(entity->GetClientClass()->m_pNetworkName, "Projectile") || strstr(entity->GetClientClass()->m_pNetworkName, "Grenade") || strstr(entity->GetClientClass()->m_pNetworkName, "Flashbang") || (strstr(entity->GetClientClass()->m_pNetworkName, "CPlantedC4")) || entity->GetClientClass()->m_ClassID == 29 || strstr(entity->GetClientClass()->m_pNetworkName, "CWeapon"))
					{
						if (SETTINGS::ragebot_configs.glowweapon)
						{
							should_draw_glow = true;
							CColor weaponcolor = SETTINGS::ragebot_configs.glowweaponcolor;
							glowObject.m_flRed = weaponcolor.RGBA[0] / 255.0f;
							glowObject.m_flGreen = weaponcolor.RGBA[1] / 255.0f;
							glowObject.m_flBlue = weaponcolor.RGBA[2] / 255.0f;
							glowObject.m_flAlpha = 255 / 255.0f;

							glowObject.m_bRenderWhenOccluded = true;
							glowObject.m_bRenderWhenUnoccluded = false;
						}
					}*/
					if (is_local_player && SETTINGS::main_configs.glow_local_enabled)
					{
						should_draw_glow = true;
						color = SETTINGS::main_configs.glow_local_color;
						style = SETTINGS::main_configs.glow_local_style;
						full_bloom = SETTINGS::main_configs.glow_local_fullbloom_enabled;
					}

					if (is_teammate && SETTINGS::main_configs.glow_team_enabled)
					{
						should_draw_glow = true;
						color = SETTINGS::main_configs.glow_team_color;
						style = SETTINGS::main_configs.glow_team_style;
						full_bloom = SETTINGS::main_configs.glow_team_fullbloom_enabled;
					}

					if (is_enemy && SETTINGS::main_configs.glow_enemy_enabled)
					{
						should_draw_glow = true;
						color = SETTINGS::main_configs.glow_enemy_color;
						style = SETTINGS::main_configs.glow_enemy_style;
						full_bloom = SETTINGS::main_configs.glow_enemy_fullbloom_enabled;
					}

					if (strstr(glowObject.getEnt()->GetClientClass()->m_pNetworkName, ("CWeapon")))
					{
						should_draw_glow = true;
						color = SETTINGS::main_configs.glow_local_color;
						style = SETTINGS::main_configs.glow_local_style;
						full_bloom = SETTINGS::main_configs.glow_local_fullbloom_enabled;
					}	

					if (!should_draw_glow)

						continue;

					glowObject.m_nGlowStyle = style;
					glowObject.m_bFullBloomRender = full_bloom;
					glowObject.m_flRed = color.RGBA[0] / 255.0f;
					glowObject.m_flGreen = color.RGBA[1] / 255.0f;
					glowObject.m_flBlue = color.RGBA[2] / 255.0f;
					glowObject.m_flAlpha = color.RGBA[3] / 255.0f;
					glowObject.m_bRenderWhenOccluded = true;
					glowObject.m_bRenderWhenUnoccluded = false;
				}
			}
		}
	}
}
