#pragma once

namespace SDK
{
	class IClient;
	class CInput;
	class CPanel;
	class ISurface;
	class IEngine;
	class CClientEntityList;
	class CGlobalVars;
	class IVDebugOverlay;
	class CTrace;
	class CModelInfo;
	class ICVar;
	class CInputSystem;
	class CModelRender;
	class CRenderView;
	class IMaterialSystem;
	class IPhysicsProps;
	class CClientMode;
	class IGameEventManager;
	class CGlowObjectManager;
	class CPrediction;
	class IGameMovement;
	class IMoveHelper;
}

namespace INTERFACES
{
	extern SDK::IClient* Client;
	extern SDK::CInput* Input;
	extern SDK::CPanel* Panel;
	extern SDK::ISurface* Surface;
	extern SDK::IEngine* Engine;
	extern SDK::CClientEntityList* ClientEntityList;
	extern SDK::CGlobalVars* Globals;
	extern SDK::IVDebugOverlay* DebugOverlay;
	extern SDK::CTrace* Trace;
	extern SDK::CModelInfo* ModelInfo;
	extern SDK::ICVar* cvar;
	extern SDK::CInputSystem* InputSystem;
	extern SDK::CModelRender* ModelRender;
	extern SDK::CRenderView* RenderView;
	extern SDK::IMaterialSystem* MaterialSystem;
	extern SDK::IPhysicsProps* PhysicsProps;
	extern SDK::CClientMode* ClientMode;
	extern SDK::IGameEventManager* GameEventManager;
	extern SDK::CGlowObjectManager* GlowObjManager;

	extern SDK::IGameMovement* GameMovement;
	extern SDK::CPrediction* Prediction;
	extern SDK::IMoveHelper* MoveHelper;

	void InitInterfaces();

	typedef void* (*CreateInterfaceFn)(const char* Name, int* ReturnCode);
	void* FindInterface(const char* Module, const char* InterfaceName);
	void* FindInterfaceEx(const char* Module, const char* InterfaceName);
}
