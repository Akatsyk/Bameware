#include "..\includes.h"

#include "hooks.h"
#include "interfaces.h"
#include "offsets.h"

#include "../UTILS/render.h"
#include "../UTILS/NetvarManager.h"
#include "../UTILS/Debugger.h"

#include "../SDK/CInput.h"
#include "../SDK/IClient.h"
#include "../SDK/CPanel.h"
#include "../SDK/IEngine.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/RecvData.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/ModelRender.h"
#include "../SDK/RenderView.h"
#include "../SDK/CTrace.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CViewSetup.h"
#include "../SDK/NetChannel.h"
#include "../SDK/CInputSystem.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/ConVar.h"

#include "../BACKEND\CFG\Configurations.h"

#include "../BACKEND\MAIN\LEGITBOT\Legitbot.h"

#include "../BACKEND\MAIN\PREDICTION\EnginePrediction.h"
#include "../BACKEND\MAIN\RAGEBOT\AIMBOT\Aimbot.h"
#include "../BACKEND\MAIN\RAGEBOT\ANTIAIM\Antiaim.h"
#include "../BACKEND\MAIN\RAGEBOT\AUTOWALL\Autowall.h"
#include "../BACKEND\MAIN\RAGEBOT\BACKTRACK\Backtracking.h"
#include "../BACKEND\MAIN\RAGEBOT\RESOLVER\Resolver.h"

#include "../BACKEND\MISC\FEATURES\EVENT\EventListeners.h"
#include "../BACKEND\MISC\FEATURES\EVENT\InGameLogger.h"
#include "../BACKEND\MISC\FEATURES\MOVEMENT\Autostrafer.h"
#include "../BACKEND\MISC\FEATURES\MOVEMENT\CircleStrafer.h"
#include "../BACKEND\MISC\FEATURES\MOVEMENT\Fakewalk.h"
#include "../BACKEND\MISC\FEATURES\SKINCHANGER\SkinChanger.h"
#include "../BACKEND\MISC\Misc.h"

#include "../BACKEND\VISUALS\CHAMS\Chams.h"
#include "../BACKEND\VISUALS\MAIN\BoxESP.h"
#include "../BACKEND\VISUALS\MAIN\Visuals.h"
#include "../BACKEND\VISUALS\OTHER\Glow.h"
#include "../BACKEND\VISUALS\OTHER\SkeletonESP.h"
#include "../BACKEND\VISUALS\OTHER\VisualizeBacktrack.h"
#include "../BACKEND\VISUALS\OTHER\VisualsMisc.h"
#include "../BACKEND\VISUALS\WORLD\World Textures.h"

#include "../FRONTEND/Menu.h"
#include <utility>

static auto linegoesthrusmoke = UTILS::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x83\xEC\x08\x8B\x15\x00\x00\x00\x00\x0F\x57\xC0", "xxxxxxxx????xxx");

namespace HOOKS
{
	CreateMoveFn original_create_move;
	PaintTraverseFn original_paint_traverse;
	FrameStageNotifyFn original_frame_stage_notify;
	LevelInitPostEntity_t original_LevelInitPostEntity;
	DrawModelExecuteFn original_draw_model_execute;
	SceneEndFn original_scene_end;
	TraceRayFn original_trace_ray;
	SendDatagramFn original_send_datagram;
	OverrideViewFn original_override_view;
	SetupBonesFn original_setup_bones;
	GetBoolFn original_net_showfragments;

	VMT::VMTHookManager iclient_hook_manager;
	VMT::VMTHookManager panel_hook_manager;
	VMT::VMTHookManager model_render_hook_manager;
	VMT::VMTHookManager render_view_hook_manager;
	VMT::VMTHookManager trace_hook_manager;
	VMT::VMTHookManager net_channel_hook_manager;
	VMT::VMTHookManager client_mode_hook_manager;
	VMT::VMTHookManager setup_bones_hook_manager;
	VMT::VMTHookManager net_showfragments;

	int GetLerpTicks()
	{
		auto cl_interp_ratio = INTERFACES::cvar->FindVar("cl_interp_ratio");
		auto cl_updaterate = INTERFACES::cvar->FindVar("cl_updaterate");
		auto cl_interp = INTERFACES::cvar->FindVar("cl_interp");

		return GLOBAL::lerp = max(cl_interp->GetFloat(), cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat());
	}

	bool __fastcall HookedCreateMove(void* thisptr, void* edx, float frametime, SDK::CUserCmd* cmd)
	{
		original_create_move(thisptr, frametime, cmd);

		if (!cmd->command_number)
			return true;

		int buttons = cmd->buttons;
		GLOBAL::pressing_move = (buttons & (IN_LEFT) || buttons & (IN_FORWARD) || buttons & (IN_BACK) ||
			buttons & (IN_RIGHT) || buttons & (IN_MOVELEFT) || buttons & (IN_MOVERIGHT) ||
			buttons & (IN_JUMP));

		GetLerpTicks();

		GLOBAL::strafe_angles = cmd->viewangles;

		GLOBAL::last_cmd = cmd;

		RENDER::render_queue.Clear();
		FEATURES::RAGEBOT::backtracking.UpdateIncomingSequences();

		FEATURES::RAGEBOT::fakewalk.Do(cmd);
		FEATURES::MISC::Fakelag(cmd);

		FEATURES::MISC::autostrafer.Strafe(cmd);
		FEATURES::MISC::circle_strafer.Do(cmd);
		FEATURES::MISC::Bhop(cmd);
		FEATURES::MISC::AutoPistol(cmd);
		FEATURES::MISC::ClantagChanger();
		FEATURES::MISC::Misc();

		FEATURES::MISC::engine_prediction.StartPrediction(cmd);
		FEATURES::LEGITBOT::legit_aimbot.Do(cmd);
		FEATURES::RAGEBOT::antiaim.UpdateLBY();
		FEATURES::RAGEBOT::antiaim.Do(cmd);
		FEATURES::RAGEBOT::aimbot.Do(cmd);
		FEATURES::MISC::engine_prediction.EndPrediction();

		FEATURES::MISC::UpdateAngles(cmd);
		FEATURES::MISC::FixMove(cmd, GLOBAL::strafe_angles);
		//FEATURES::MISC::FixMovement(cmd);

		FEATURES::MISC::AutoRevolver(cmd);
		FEATURES::RAGEBOT::resolver.DoCM();

		cmd->viewangles = MATH::NormalizeAngle(cmd->viewangles);

		uintptr_t* pebp;
		__asm mov pebp, ebp
		* reinterpret_cast<bool*>(*reinterpret_cast<uintptr_t*>(pebp) - 0x1C) = GLOBAL::should_send_packet;

		return false;
	}

	void __stdcall HookedPaintTraverse(int VGUIPanel, bool ForceRepaint, bool AllowForce)
	{
		std::string panel_name = INTERFACES::Panel->GetName(VGUIPanel);
		if (panel_name == "MatSystemTopPanel")
		{
			if (FONTS::ShouldReloadFonts())
				FONTS::InitFonts();

			FEATURES::VISUALS::Do();
			FEATURES::MISC::DrawMisc();
			FEATURES::MISC::in_game_logger.Do();
			FEATURES::MISC::hitmarkers.Do();
			FEATURES::MISC::bullet_tracers.Do();

			RENDER::render_queue.Draw();

			PPGUI::MENU::Do();

			UTILS::INPUT::input_handler.Update();
		}

		// no scope overlay
		if (panel_name == "HudZoom" && SETTINGS::main_configs.no_scope_overlay_enabled)
			return;

		original_paint_traverse(INTERFACES::Panel, VGUIPanel, ForceRepaint, AllowForce);
	}

	int __fastcall HookedSendDatagram(SDK::NetChannel* ecx, void* edx, void* data)
	{
		if (data || !INTERFACES::Engine->IsInGame())
			return original_send_datagram(ecx, data);

		auto nci = INTERFACES::Engine->GetNetChannelInfo();
		if (!nci)
			return original_send_datagram(ecx, data);

		int in_reliable_state = ecx->m_in_rel_state;
		int in_sequence_num = ecx->m_in_seq;

		FEATURES::RAGEBOT::backtracking.AddLatency(ecx);

		int ret = original_send_datagram(ecx, data);

		ecx->m_in_rel_state = in_reliable_state;
		ecx->m_in_seq = in_sequence_num;

		return ret;
	}

	void LevelInitPostEntity()
	{
		FEATURES::RAGEBOT::backtracking.last_incoming_sequence = 0;
		auto m_net = INTERFACES::Engine->GetNetChannelInfo();
		if (m_net) 
		{
			net_channel_hook_manager.Restore();
			net_channel_hook_manager.Init(m_net);
			original_send_datagram = net_channel_hook_manager.HookFunction< SendDatagramFn >(48, HookedSendDatagram);
			LOG(enc_char("Successfully hooked virtual function - SendDatagram"));
		}

		// invoke original method.
		original_LevelInitPostEntity();
	}

	bool __fastcall HookedNetShowFragments(void* pConVar, void* ebx)
	{
		if (!INTERFACES::Engine->IsInGame())
			return original_net_showfragments(pConVar);

		static auto read_sub_channel_data_ret = reinterpret_cast<uintptr_t*>(UTILS::FindSignature(enc_char("engine.dll"), enc_char("85 C0 74 12 53 FF 75 0C 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 0C")));
		static auto check_receiving_list_ret = reinterpret_cast<uintptr_t*>(UTILS::FindSignature(enc_char("engine.dll"), enc_char("8B 1D ? ? ? ? 85 C0 74 16 FF B6")));

		static uint32_t last_fragment = 0;

		if (_ReturnAddress() == reinterpret_cast<void*>(read_sub_channel_data_ret) && last_fragment > 0)
		{
			const auto data = &reinterpret_cast<uint32_t*>(INTERFACES::Engine->GetNetChannel())[0x56];
			const auto bytes_fragments = reinterpret_cast<uint32_t*>(data)[0x43];

			if (bytes_fragments == last_fragment)
			{
				auto& buffer = reinterpret_cast<uint32_t*>(data)[0x42];
				buffer = 0;
			}
		}

		if (_ReturnAddress() == check_receiving_list_ret)
		{
			const auto data = &reinterpret_cast<uint32_t*>(INTERFACES::Engine->GetNetChannel())[0x56];
			const auto bytes_fragments = reinterpret_cast<uint32_t*>(data)[0x43];

			last_fragment = bytes_fragments;
		}

		return original_net_showfragments(pConVar);
	}

	void __fastcall HookedFrameStageNotify(void* ecx, void* edx, int stage)
	{
		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

		switch (stage)
		{	
			case FRAME_NET_UPDATE_POSTDATAUPDATE_START:

				FEATURES::RAGEBOT::resolver.DoFSN();

				FEATURES::VISUALS::world_textures.DoFSN();
				FEATURES::VISUALS::visuals_misc.DisableFlashDuration();

				FEATURES::MISC::Radar();
				FEATURES::MISC::skin_changer.Do();

				break;
			case FRAME_NET_UPDATE_POSTDATAUPDATE_END:

				FEATURES::RAGEBOT::backtracking.Store();

				FEATURES::VISUALS::visuals_misc.NoSmoke();

				break;
			case FRAME_RENDER_START:

				FEATURES::MISC::ThirdpersonFSN();

				break;
		}

		original_frame_stage_notify(ecx, stage);
	}

	void __fastcall HookedDrawModelExecute(void* ecx, void* edx, SDK::IMatRenderContext* context, const SDK::DrawModelState_t& state, const SDK::ModelRenderInfo_t& render_info, matrix3x4_t* matrix)
	{
		auto local_player = INTERFACES::ClientEntityList->GetClientEntityFromHandle((void*)INTERFACES::Engine->GetLocalPlayer());
		auto entity = INTERFACES::ClientEntityList->GetClientEntityFromHandle((void*)render_info.entity_index);

		/// localplayer
		if (entity == local_player)
		{
			if (local_player && local_player->IsScopedIn())
				INTERFACES::RenderView->SetBlend(SETTINGS::main_configs.transparency_when_scoped * 0.01f);
		}

		original_draw_model_execute(ecx, context, state, render_info, matrix);	
	}

	void __fastcall HookedSceneEnd(void* ecx, void* edx)
	{
		original_scene_end(ecx);

		FEATURES::VISUALS::chams.DoSceneEnd();
	//	FEATURES::VISUALS::glow.DoSceneEnd();

		if (SETTINGS::main_configs.no_smoke_enabled)
		{
			std::vector<const char*> vistasmoke_wireframe =
			{
				"particle/vistasmokev1/vistasmokev1_smokegrenade",
			};

			std::vector<const char*> vistasmoke_nodraw =
			{
				"particle/vistasmokev1/vistasmokev1_fire",
				"particle/vistasmokev1/vistasmokev1_emods",
				"particle/vistasmokev1/vistasmokev1_emods_impactdust",
			};
			for (auto mat_s : vistasmoke_wireframe)
			{
				SDK::IMaterial* mat = INTERFACES::MaterialSystem->FindMaterial(mat_s, TEXTURE_GROUP_OTHER);
				mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, true); //wireframe
			}

			for (auto mat_n : vistasmoke_nodraw)
			{
				SDK::IMaterial* mat = INTERFACES::MaterialSystem->FindMaterial(mat_n, TEXTURE_GROUP_OTHER);
				mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_NO_DRAW, true);
			}

			static auto smokecout = *(DWORD*)(linegoesthrusmoke + 0x8);
			*(int*)(smokecout) = 0;
		}

		FEATURES::VISUALS::visuals_misc.NoSmoke();
	}

	void __fastcall HookedTraceRay(void* thisptr, void*, const SDK::Ray_t& ray, unsigned int fMask, SDK::ITraceFilter* pTraceFilter, SDK::trace_t* pTrace)
	{
		original_trace_ray(thisptr, ray, fMask, pTraceFilter, pTrace);
		pTrace->surface.flags |= SURF_SKY;
	}

	void __fastcall HookedOverrideView(void* thisptr, void* edx, SDK::CViewSetup* setup)
	{
		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

		if (local_player && !local_player->IsScopedIn())
			setup->fov = SETTINGS::main_configs.visual_fov;

		FEATURES::MISC::ThirdpersonOV();

		original_override_view(thisptr, setup);
	}

	void InitHooks()
	{
		iclient_hook_manager.Init(INTERFACES::Client);
		panel_hook_manager.Init(INTERFACES::Panel);
		model_render_hook_manager.Init(INTERFACES::ModelRender);
		render_view_hook_manager.Init(INTERFACES::RenderView);
		trace_hook_manager.Init(INTERFACES::Trace);
		client_mode_hook_manager.Init(INTERFACES::ClientMode);
		net_showfragments.Init(INTERFACES::cvar->FindVar("net_showfragments"));

		original_create_move = client_mode_hook_manager.HookFunction<CreateMoveFn>(24, HookedCreateMove);
		LOG(enc_char("Successfully hooked virtual function - CreateMove"));

		original_frame_stage_notify = iclient_hook_manager.HookFunction<FrameStageNotifyFn>(36, HookedFrameStageNotify);
		LOG(enc_char("Successfully hooked virtual function - FrameStageNotify"));

		original_LevelInitPostEntity = iclient_hook_manager.HookFunction<LevelInitPostEntity_t>(6, LevelInitPostEntity);
		LOG(enc_char("Successfully hooked virtual function - LevelInitPostEntity"));

		original_paint_traverse = panel_hook_manager.HookFunction<PaintTraverseFn>(41, HookedPaintTraverse);
		LOG(enc_char("Successfully hooked virtual function - PaintTraverse"));

		original_draw_model_execute = model_render_hook_manager.HookFunction<DrawModelExecuteFn>(21, HookedDrawModelExecute);
		LOG(enc_char("Successfully hooked virtual function - DrawModelExecute"));

		original_scene_end = render_view_hook_manager.HookFunction<SceneEndFn>(9, HookedSceneEnd); 
		LOG(enc_char("Successfully hooked virtual function - SceneEnd"));

		original_trace_ray = trace_hook_manager.HookFunction<TraceRayFn>(5, HookedTraceRay);
		LOG(enc_char("Successfully hooked virtual function - TraceRay"));

		original_override_view = client_mode_hook_manager.HookFunction<OverrideViewFn>(18, HookedOverrideView);
		LOG(enc_char("Successfully hooked virtual function - OverrideView"));

		original_net_showfragments = net_showfragments.HookFunction<GetBoolFn>(13, HookedNetShowFragments);
		LOG(enc_char("Successfully hooked virtual function - NetShowFragments"));
	}

	void UnHook()
	{
		iclient_hook_manager.Restore();
		panel_hook_manager.Restore();
		model_render_hook_manager.Restore();
		render_view_hook_manager.Restore();
		trace_hook_manager.Restore();
		client_mode_hook_manager.Restore();
		setup_bones_hook_manager.Restore();
		net_showfragments.Restore();

		FEATURES::MISC::RemoveEventListeners();	
	}

	/// netvar hooks
	void* OriginalSequenceFn = nullptr;

	void EyeAnglesPitchHook(const SDK::CRecvProxyData* pData, void* pStruct, void* pOut)
	{
		*reinterpret_cast<float*>(pOut) = pData->m_Value.m_Float;

		auto entity = reinterpret_cast<SDK::CBaseEntity*>(pStruct);
		if (!entity)
			return;

		FEATURES::RAGEBOT::resolver.GetPlayerResolveInfo(entity).networked_angles.x = pData->m_Value.m_Float;
	}

	void EyeAnglesYawHook(const SDK::CRecvProxyData* pData, void* pStruct, void* pOut)
	{
		*reinterpret_cast<float*>(pOut) = pData->m_Value.m_Float;

		auto entity = reinterpret_cast<SDK::CBaseEntity*>(pStruct);
		if (!entity)
			return;

		FEATURES::RAGEBOT::resolver.GetPlayerResolveInfo(entity).networked_angles.y = pData->m_Value.m_Float;
	}

	void SmokeEffectHook(const SDK::CRecvProxyData* pData, void* pStruct, void* pOut)
	{
		*(int*)(uintptr_t(pOut)) = pData->m_Value.m_Int;

		if (!SETTINGS::main_configs.no_smoke_enabled)
			return;
	}

	void SequenceHook(const SDK::CRecvProxyData* pData, void* pStruct, void* pOut)
	{
		const auto original_func = reinterpret_cast<SDK::RecvVarProxyFn>(OriginalSequenceFn);

		auto viewmodel = reinterpret_cast<SDK::CBaseViewmodel*>(pStruct);
		if (!viewmodel)
			return original_func(pData, pStruct, pOut);

		auto viewmodel_owner = INTERFACES::ClientEntityList->GetClientEntity(viewmodel->GetOwnerIndex());
		if (!viewmodel_owner || viewmodel_owner->GetIndex() != INTERFACES::Engine->GetLocalPlayer())
			return original_func(pData, pStruct, pOut);

		const char* model = INTERFACES::ModelInfo->GetModelName(viewmodel->GetModel());
		int sequence = pData->m_Value.m_Int;

		return original_func(pData, pStruct, pOut);
	}

	void InitNetvarHooks()
	{
		UTILS::netvar_manager.Hook("DT_CSPlayer", "m_angEyeAngles[0]", EyeAnglesPitchHook);
		UTILS::netvar_manager.Hook("DT_CSPlayer", "m_angEyeAngles[1]", EyeAnglesYawHook);
		UTILS::netvar_manager.Hook("DT_SmokeGrenadeProjectile", "m_bDidSmokeEffect", SmokeEffectHook);
		//OriginalSequenceFn = reinterpret_cast<void*>(UTILS::netvar_manager.Hook("DT_BaseViewModel", "m_nSequence", SequenceHook));
	}
}
