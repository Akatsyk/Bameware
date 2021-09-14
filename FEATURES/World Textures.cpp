#include "../includes.h"

#include "../UTILS/interfaces.h"

#include "../SDK/IMaterial.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/ModelRender.h"
#include "../SDK/IEngine.h"
#include "../SDK/ConVar.h"

#include "World Textures.h"

#include "Configurations.h"

namespace FEATURES
{
	namespace VISUALS
	{
		WorldTextures world_textures;
		bool prepared = false;

		CColor world_color;
		CColor prop_color;
		CColor skybox_color;

		float world_opacity;
		float props_opacity;

		int skybox;

		void WorldTextures::DoFSN()
		{
			if (!INTERFACES::Engine->IsInGame() || !INTERFACES::Engine->IsConnected())
				FEATURES::VISUALS::world_textures.disconnect = true;

			if (!INTERFACES::Engine->IsInGame() || !INTERFACES::Engine->IsConnected())
				return;

			static auto skybox_var = INTERFACES::cvar->FindVar("sv_skyname");
			skybox_var->nFlags &= ~(1 << 14);

			if (!prepared || disconnect) {
				prepared = true;
				auto c_var = INTERFACES::cvar->FindVar("r_drawspecificstaticprop");
				c_var->SetValue("0");
			}

			if (SETTINGS::main_configs.world_textures_world_color != world_color || disconnect) {

				for (SDK::MaterialHandle_t i = INTERFACES::MaterialSystem->FirstMaterial(); i != INTERFACES::MaterialSystem->InvalidMaterial(); i = INTERFACES::MaterialSystem->NextMaterial(i)) {
					SDK::IMaterial* pMaterial = INTERFACES::MaterialSystem->GetMaterial(i);

					if (!pMaterial)
						continue;

					if (strstr(pMaterial->GetTextureGroupName(), "World textures")) {
						pMaterial->ColorModulate(world_color);
					}
				}
				world_color = SETTINGS::main_configs.world_textures_world_color;
			}

			if (SETTINGS::main_configs.world_textures_static_props_color != prop_color || disconnect) {
				for (SDK::MaterialHandle_t i = INTERFACES::MaterialSystem->FirstMaterial(); i != INTERFACES::MaterialSystem->InvalidMaterial(); i = INTERFACES::MaterialSystem->NextMaterial(i)) {
					SDK::IMaterial* pMaterial = INTERFACES::MaterialSystem->GetMaterial(i);

					if (!pMaterial)
						continue;

					if (strstr(pMaterial->GetTextureGroupName(), "StaticProp textures")) {
						pMaterial->ColorModulate(prop_color);
						pMaterial->AlphaModulate(prop_color.RGBA[3] / 255.0f);
					}
				}
				prop_color = SETTINGS::main_configs.world_textures_static_props_color;
			}

			if (SETTINGS::main_configs.world_textures_skybox_color != skybox_color || disconnect) {
				for (SDK::MaterialHandle_t i = INTERFACES::MaterialSystem->FirstMaterial(); i != INTERFACES::MaterialSystem->InvalidMaterial(); i = INTERFACES::MaterialSystem->NextMaterial(i)) {
					SDK::IMaterial* pMaterial = INTERFACES::MaterialSystem->GetMaterial(i);

					if (!pMaterial)
						continue;

					if (strstr(pMaterial->GetTextureGroupName(), "SkyBox"))
					{
						pMaterial->ColorModulate(skybox_color);
					}
				}
				skybox_color = SETTINGS::main_configs.world_textures_skybox_color;
			}

			if (SETTINGS::main_configs.world_textures_skybox != skybox || disconnect) {
				skybox_var->SetValue(skybox_list[SETTINGS::main_configs.world_textures_skybox]);
				skybox = SETTINGS::main_configs.world_textures_skybox;
			}


			disconnect = false;
		}
	}
}
