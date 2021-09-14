#include "../includes.h"

#include "Chams.h"

#include "../UTILS/interfaces.h"

#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/ModelRender.h"
#include "../SDK/IEngine.h"
#include "../SDK/IMaterial.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/RenderView.h"

#include "Configurations.h"
#include "Backtracking.h"

namespace FEATURES
{
	namespace VISUALS
	{
		CChams chams;

		void CChams::DoSceneEnd()
		{
			if (!(INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame()))
				return;

			UpdateMaterials();

			for (auto i = 1; i < 32; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);

				if (!entity)
					continue;

				if (entity->GetHealth() <= 0 || !entity->IsAlive())
					continue;

				if (entity->GetIsDormant())
					continue;

				DrawPlayer(entity);
			}
		}

		void CChams::DrawBacktrackPlayer(SDK::CBaseEntity* entity)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam() || !SETTINGS::main_configs.bt_vis_chams_enabled)
				return;

			auto records = FEATURES::RAGEBOT::backtracking.GetValidRecords(entity);
			if (!records.size())
				return;

			FEATURES::RAGEBOT::backtracking.Restore(entity, records.back());
			FEATURES::RAGEBOT::backtracking.ApplyRestore(entity, records.back().curtime);

			INTERFACES::ModelRender->ForcedMaterialOverride(backtrack_visualization_mat);
			entity->DrawModel(0x1, 255);

			FEATURES::RAGEBOT::backtracking.RestoreToCurrentRecord(entity);
		}

		void CChams::DrawPlayer(SDK::CBaseEntity* entity)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			bool is_localplayer = local_player == entity;
			bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_localplayer;
			bool is_enemy = local_player->GetTeam() != entity->GetTeam();

			// if doesn't have chams enabled
			bool should_draw_chams = true;
			if ((is_localplayer && !SETTINGS::main_configs.chams_local_enabled) || (is_localplayer && !SETTINGS::main_configs.thirdperson_enabled))
				should_draw_chams = false;
			else if (is_teammate && !SETTINGS::main_configs.chams_team_enabled)
				should_draw_chams = false;
			else if (is_enemy && !SETTINGS::main_configs.chams_enemy_enabled)
				should_draw_chams = false;

			if (!local_player_mat || !teammate_occluded_mat || !teammate_visible_mat || !enemy_occluded_mat || !enemy_visible_mat)
				return;

			if (teammate_visible_mat == NULL)
				return;

			if (teammate_visible_mat == NULL)
				return;

			if (enemy_occluded_mat == NULL)
				return;

			if (enemy_visible_mat == NULL)
				return;

			if (local_player_mat == NULL)
				return;

			SDK::IMaterial* occluded_material = nullptr;
			SDK::IMaterial* visible_material = nullptr;

			if (is_localplayer)
				visible_material = local_player_mat;
			else if (is_teammate)
			{
				occluded_material = teammate_occluded_mat;
				visible_material = teammate_visible_mat;
			}
			else if (is_enemy)
			{
				occluded_material = enemy_occluded_mat;
				visible_material = enemy_visible_mat;
			}

			if (occluded_material && !is_localplayer)
				occluded_material->SetMaterialVarFlag(SDK::MATERIAL_VAR_IGNOREZ, true);

			/// dont draw chams on localplayer if transparency when scoped
			if (is_localplayer && local_player->IsScopedIn() && SETTINGS::main_configs.transparency_when_scoped < 100.f)
				return;

			if ((INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame()))
			{
				DrawBacktrackPlayer(entity);

				if (!is_localplayer && should_draw_chams)
				{
					INTERFACES::ModelRender->ForcedMaterialOverride(occluded_material);
					entity->DrawModel(0x1, 255);
					INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
				}

				if (should_draw_chams)
				{
					INTERFACES::ModelRender->ForcedMaterialOverride(visible_material);
					entity->DrawModel(0x1, 255);
					INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
				}
			}
		}

		bool CChams::ShouldDrawModel(SDK::CBaseEntity* entity)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return false;

			if (!entity)
				return false;

			if (entity->GetHealth() <= 0)
				return false;

			bool is_localplayer = local_player == entity;
			bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_localplayer;
			bool is_enemy = local_player->GetTeam() != entity->GetTeam();

			bool should_draw_player = true;
			if (is_localplayer && !SETTINGS::main_configs.draw_local_player)
				should_draw_player = false;
			else if (is_teammate && !SETTINGS::main_configs.draw_team_player)
				should_draw_player = false;
			else if (is_enemy && !SETTINGS::main_configs.draw_enemy_player)
				should_draw_player = false;

			return should_draw_player;
		}

		void CChams::UpdateMaterials()
		{
			static bool initilized = false;
			if (!initilized)
			{
				initilized = true;

				local_player_mat = CreateMaterial(false, true, false); //INTERFACES::MaterialSystem->FindMaterial("debug/debugambientcube", "Model textures");
				enemy_visible_mat = CreateMaterial(false, true, false);
				enemy_occluded_mat = CreateMaterial(true, true, false);
				teammate_visible_mat = CreateMaterial(false, true, false);
				teammate_occluded_mat = CreateMaterial(true, true, false);
				backtrack_visualization_mat = CreateMaterial(true, true, false);//INTERFACES::MaterialSystem->FindMaterial("debug/debugdrawflat", nullptr);

			}

			static unsigned int material_update_time = 1;
			const bool force_material_update = material_update_time % 100;
			material_update_time++;

			if (SETTINGS::main_configs.chams_local_enabled != force_material_update)
			{
				local_player_mat->ColorModulate(SETTINGS::main_configs.chams_local_color);
				local_player_mat->AlphaModulate(SETTINGS::main_configs.chams_local_color.RGBA[3] / 255.f);
			}

			if (SETTINGS::main_configs.chams_enemy_enabled != force_material_update)
			{
				enemy_visible_mat->ColorModulate(SETTINGS::main_configs.chams_enemy_unoccluded_color);
				enemy_visible_mat->AlphaModulate(SETTINGS::main_configs.chams_enemy_unoccluded_color.RGBA[3] / 255.f);
				enemy_occluded_mat->ColorModulate(SETTINGS::main_configs.chams_enemy_occluded_color);
				enemy_occluded_mat->AlphaModulate(SETTINGS::main_configs.chams_enemy_occluded_color.RGBA[3] / 255.f);
			}

			if (SETTINGS::main_configs.chams_team_enabled != force_material_update)
			{
				teammate_visible_mat->ColorModulate(SETTINGS::main_configs.chams_team_unoccluded_color);
				teammate_visible_mat->AlphaModulate(SETTINGS::main_configs.chams_team_unoccluded_color.RGBA[3] / 255.f);
				teammate_occluded_mat->ColorModulate(SETTINGS::main_configs.chams_team_occluded_color);
				teammate_occluded_mat->AlphaModulate(SETTINGS::main_configs.chams_team_occluded_color.RGBA[3] / 255.f);
			}

			backtrack_visualization_mat->ColorModulate(SETTINGS::main_configs.bt_vis_chams_color);
			backtrack_visualization_mat->AlphaModulate(SETTINGS::main_configs.bt_vis_chams_color.RGBA[3] / 255.f);

			static bool last_local_wireframe = false, last_enemy_wireframe = false, last_team_wireframe = false;

			if (last_local_wireframe != SETTINGS::main_configs.chams_local_wireframe_enabled)
			{
				last_local_wireframe = SETTINGS::main_configs.chams_local_wireframe_enabled;
				local_player_mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, SETTINGS::main_configs.chams_local_wireframe_enabled);
			}
			if (last_enemy_wireframe != SETTINGS::main_configs.chams_enemy_wireframe_enabled)
			{
				enemy_visible_mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, SETTINGS::main_configs.chams_enemy_wireframe_enabled);
				enemy_occluded_mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, SETTINGS::main_configs.chams_enemy_wireframe_enabled);
			}
			if (last_team_wireframe != SETTINGS::main_configs.chams_team_wireframe_enabled)
			{
				teammate_visible_mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, SETTINGS::main_configs.chams_team_wireframe_enabled);
				teammate_occluded_mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, SETTINGS::main_configs.chams_team_wireframe_enabled);
			}
		}

		void CChams::InitKeyValues(SDK::KeyValues* kv_, std::string name_)
		{
			static auto address = UTILS::FindSignature("client.dll", "55 8B EC 51 33 C0 C7 45");
			using Fn = void(__thiscall*)(void* thisptr, const char* name);
			reinterpret_cast<Fn>(address)(kv_, name_.c_str());
		}

		void CChams::LoadFromBuffer(SDK::KeyValues* vk_, std::string name_, std::string buffer_)
		{
			static auto address = UTILS::FindSignature("client.dll", "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04");
			using Fn = void(__thiscall*)(
				void* thisptr, const char* resourceName,
				const char* pBuffer, void* pFileSystem,
				const char* pPathID, void* pfnEvaluateSymbolProc);

			reinterpret_cast<Fn>(address)(
				vk_, name_.c_str(), buffer_.c_str(), nullptr, nullptr, nullptr);
		}


		SDK::IMaterial* CChams::CreateMaterial(bool shouldIgnoreZ, bool isLit, bool isWireframe) 
		{
		/*	static int created = 10;
			static const char tmp[] =
			{
				"\"%s\"\
					\n{\
					\n\t\"$basetexture\" \"vgui/white\"\
					\n\t\"$envmap\" \"env_cubemap\"\
					\n\t\"$reflectiviy\"\"5\"\
					\n\t\"$model\" \"1\"\
					\n\t\"$flat\" \"0\"\
					\n\t\"$nocull\" \"0\"\
					\n\t\"$selfillum\" \"3\"\
					\n\t\"$halflambert\" \"1\"\
					\n\t\"$nofog\" \"1\"\
					\n\t\"$ignorez\" \"%i\"\
					\n\t\"$znearer\" \"0\"\
					\n\t\"$wireframe\" \"%i\"\
					\n}\n"
			};

			char* baseType = (isLit == true ? "VertexLitGeneric" : "UnlitGeneric");
			char material[512];
			sprintf_s(material, sizeof(material), tmp, baseType, (shouldIgnoreZ) ? 1 : 0, (isWireframe) ? 1 : 0);

			char name[512];
			sprintf_s(name, sizeof(name), "#nice_pener_material%i.vmt", created);
			++created;

			SDK::KeyValues* keyValues = new SDK::KeyValues;

			InitKeyValues(keyValues, baseType);
			LoadFromBuffer(keyValues, name, material);

			SDK::IMaterial* created_material = INTERFACES::MaterialSystem->CreateMaterial(name, keyValues);
			created_material->IncrementReferenceCount();

			return created_material; */

			static int created = 10;
			static std::string tmp =
			{
				"\"%s\"\
				\n{\
				\n\t\"$basetexture\" \"vgui/white_additive\"\
				\n\t\"$envmap\" \"env_cubemap\"\
				\n\t\"$normalmapalphaenvmapmask\" \"1\"\
				\n\t\"$envmapcontrast\" \"1\"\
				\n\t\"$model\" \"1\"\
				\n\t\"$flat\" \"1\"\
				\n\t\"$nocull\" \"0\"\
				\n\t\"$selfillum\" \"1\"\
				\n\t\"$halflambert\" \"1\"\
				\n\t\"$nofog\" \"0\"\
				\n\t\"$ignorez\" \"" + std::to_string(shouldIgnoreZ) + "\"\
				\n\t\"$znearer\" \"0\"\
				\n\t\"$wireframe\" \"" + std::to_string(isWireframe) + "\"\
				\n}\n"
			};

			isLit = true;
			char* baseType = (isLit == true ? "VertexLitGeneric" : "UnlitGeneric");
			char material[512];
			sprintf_s(material, sizeof(material), tmp.c_str(), baseType);

			char name[512];
			sprintf_s(name, sizeof(name), "#nice_pener_material%i.vmt", created);
			++created;

			auto keyValues = static_cast<SDK::KeyValues*>(malloc(sizeof(SDK::KeyValues)));

			InitKeyValues(keyValues, baseType);
			LoadFromBuffer(keyValues, name, material);

			SDK::IMaterial* created_material = INTERFACES::MaterialSystem->CreateMaterial(name, keyValues);
			created_material->IncrementReferenceCount();

			return created_material;
		}

		/*SDK::IMaterial* CChams::CreateMaterial(bool shouldIgnoreZ, bool isLit, bool isWireframe)
		{
			static int created = 10;
			static const char tmp[] =
			{
				"\"%s\"\
				\n{\
				\n\t\"$basetexture\" \"vgui/white_additive\"\
				\n\t\"$envmap\" \"env_cubemap\"\
				\n\t\"$model\" \"1\"\
				\n\t\"$flat\" \"0\"\
				\n\t\"$nocull\" \"0\"\
				\n\t\"$selfillum\" \"1\"\
				\n\t\"$halflambert\" \"1\"\
				\n\t\"$nofog\" \"1\"\
				\n\t\"$ignorez\" \"%i\"\
				\n\t\"$znearer\" \"0\"\
				\n\t\"$wireframe\" \"%i\"\
				\n}\n"
			};

			char* baseType = (isLit == true ? "VertexLitGeneric" : "UnlitGeneric");
			char material[512];
			sprintf_s(material, sizeof(material), tmp, baseType, (shouldIgnoreZ) ? 1 : 0, (isWireframe) ? 1 : 0);

			char name[512];
			sprintf_s(name, sizeof(name), "#nice_pener_material%i.vmt", created);
			++created;

			SDK::KeyValues* keyValues = new SDK::KeyValues;

			SDK::IMaterial* created_material = INTERFACES::MaterialSystem->CreateMaterial(name, keyValues);
			created_material->IncrementReferenceCount();

			return created_material;

			/*	static int created = 10;
				static const char tmp[] =
				{
					"\"%s\"\
					\n{\
					\n\t\"$basetexture\" \"vgui/white\"\
					\n\t\"$envmap\" \"env_cubemap\"\
					\n\t\"$reflectiviy\"\"5\"\
					\n\t\"$model\" \"1\"\
					\n\t\"$flat\" \"0\"\
					\n\t\"$nocull\" \"0\"\
					\n\t\"$selfillum\" \"3\"\
					\n\t\"$halflambert\" \"1\"\
					\n\t\"$nofog\" \"1\"\
					\n\t\"$ignorez\" \"%i\"\
					\n\t\"$znearer\" \"0\"\
					\n\t\"$wireframe\" \"%i\"\
					\n}\n"
				};

				char* baseType = (isLit == true ? "VertexLitGeneric" : "UnlitGeneric");
				char material[512];
				sprintf_s(material, sizeof(material), tmp, baseType, (shouldIgnoreZ) ? 1 : 0, (isWireframe) ? 1 : 0);

				char name[512];
				sprintf_s(name, sizeof(name), "#nice_pener_material%i.vmt", created);
				++created;

				SDK::KeyValues* keyValues = new SDK::KeyValues;

				InitKeyValues(keyValues, baseType);
				LoadFromBuffer(keyValues, name, material);

				SDK::IMaterial* created_material = INTERFACES::MaterialSystem->CreateMaterial(name, keyValues);
				created_material->IncrementReferenceCount();

				return created_material;*/
		//}
	}
}
