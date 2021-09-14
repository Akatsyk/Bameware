#pragma once

//this has all common includes that most (every?) cpp file should include

#include <Windows.h>
#include <vector>
#include <stack>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <assert.h>
#include <Psapi.h>
#include <thread>
#include <chrono>
#include <array>
#include <comdef.h>
#include <sstream>
#include <queue>
#include <random>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <intrin.h>
#include <urlmon.h>

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment( lib, "Iphlpapi.lib" )


#define INRANGE(x,a,b)  (x >= a && x <= b)
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

#define IN_ATTACK					(1 << 0)
#define IN_JUMP						(1 << 1)
#define IN_DUCK						(1 << 2)
#define IN_FORWARD					(1 << 3)
#define IN_BACK						(1 << 4)
#define IN_USE						(1 << 5)
#define IN_CANCEL					(1 << 6)
#define IN_LEFT						(1 << 7)
#define IN_RIGHT					(1 << 8)
#define IN_MOVELEFT					(1 << 9)
#define IN_MOVERIGHT				(1 << 10)
#define IN_ATTACK2					(1 << 11)
#define IN_RUN						(1 << 12)
#define IN_RELOAD					(1 << 13)
#define IN_ALT1						(1 << 14)
#define IN_ALT2						(1 << 15)
#define IN_SCORE					(1 << 16)
#define IN_SPEED					(1 << 17)
#define IN_WALK						(1 << 18)
#define IN_ZOOM						(1 << 19)
#define IN_WEAPON1					(1 << 20)
#define IN_WEAPON2					(1 << 21)
#define IN_BULLRUSH					(1 << 22)
#define IN_GRENADE1					(1 << 23)
#define IN_GRENADE2					(1 << 24)
#define	IN_ATTACK3					(1 << 25)

#define	FL_ONGROUND					(1 << 0)
#define FL_DUCKING					(1 << 1)
#define	FL_WATERJUMP				(1 << 2)
#define FL_ONTRAIN					(1 << 3)
#define FL_INRAIN					(1 << 4)
#define FL_FROZEN					(1 << 5)
#define FL_ATCONTROLS				(1 << 6)
#define	FL_CLIENT					(1 << 7)
#define FL_FAKECLIENT				(1 << 8)
#define	FL_INWATER					(1 << 9)
#define	FL_FLY						(1 << 10)
#define	FL_SWIM						(1 << 11)
#define	FL_CONVEYOR					(1 << 12)
#define	FL_NPC						(1 << 13)
#define	FL_GODMODE					(1 << 14)
#define	FL_NOTARGET					(1 << 15)
#define	FL_AIMTARGET				(1 << 16)
#define	FL_PARTIALGROUND			(1 << 17)
#define FL_STATICPROP				(1 << 18)
#define FL_GRAPHED					(1 << 19)
#define FL_GRENADE					(1 << 20)
#define FL_STEPMOVEMENT				(1 << 21)
#define FL_DONTTOUCH				(1 << 22)
#define FL_BASEVELOCITY				(1 << 23)
#define FL_WORLDBRUSH				(1 << 24)
#define FL_OBJECT					(1 << 25)
#define FL_KILLME					(1 << 26)
#define FL_ONFIRE					(1 << 27)
#define FL_DISSOLVING				(1 << 28)
#define FL_TRANSRAGDOLL				(1 << 29)
#define FL_UNBLOCKABLE_BY_PLAYER	(1 << 30)

#define TEXTURE_GROUP_LIGHTMAP						"Lightmaps"
#define TEXTURE_GROUP_WORLD							"World textures"
#define TEXTURE_GROUP_MODEL							"Model textures"
#define TEXTURE_GROUP_VGUI							"VGUI textures"
#define TEXTURE_GROUP_PARTICLE						"Particle textures"
#define TEXTURE_GROUP_DECAL							"Decal textures"
#define TEXTURE_GROUP_SKYBOX						"SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				"ClientEffect textures"
#define TEXTURE_GROUP_OTHER							"Other textures"
#define TEXTURE_GROUP_PRECACHED						"Precached"				// TODO: assign texture groups to the precached materials
#define TEXTURE_GROUP_CUBE_MAP						"CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					"RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					"Unaccounted textures"	// Textures that weren't assigned a texture group.
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			"Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			"Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			"Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					"DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					"ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					"Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				"Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			"RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					"Morph Targets"

#define PLAYER_FLAG_BITS			10
#define DISPSURF_FLAG_SURFACE		(1<<0)
#define DISPSURF_FLAG_WALKABLE		(1<<1)
#define DISPSURF_FLAG_BUILDABLE		(1<<2)
#define DISPSURF_FLAG_SURFPROP1		(1<<3)
#define DISPSURF_FLAG_SURFPROP2		(1<<4)

#define HITGROUP_GENERIC	  0
#define HITGROUP_HEAD         1
#define HITGROUP_CHEST        2
#define HITGROUP_STOMACH      3
#define HITGROUP_LEFTARM      4
#define HITGROUP_RIGHTARM     5
#define HITGROUP_LEFTLEG      6
#define HITGROUP_RIGHTLEG     7
#define HITGROUP_GEAR         10

#define WEAPON_TYPE_PISTOL 1
#define WEAPON_TYPE_SNIPER 5
#define WEAPON_TYPE_GRENADE 9
#define WEAPON_TYPE_KNIFE 0

#define MAX_STUDIOBONES 128
#define MAXSTUDIOSKINS 32

#define BONE_USED_BY_HITBOX	0x00000100
#define BONE_USED_BY_ANYTHING		0x0007FF00

#define	SURF_LIGHT		0x0001		// value will hold the light strength
#define	SURF_SKY2D		0x0002		// don't draw, indicates we should skylight + draw 2d sky but not draw the 3D skybox
#define	SURF_SKY		0x0004		// don't draw, but add to skybox
#define	SURF_WARP		0x0008		// turbulent water warp
#define	SURF_TRANS		0x0010
#define SURF_NOPORTAL	0x0020	// the surface can not have a portal placed on it
#define	SURF_TRIGGER	0x0040	// FIXME: This is an xbox hack to work around elimination of trigger surfaces, which breaks occluders
#define	SURF_NODRAW		0x0080	// don't bother referencing the texture
#define	SURF_HINT		0x0100	// make a primary bsp splitter
#define	SURF_SKIP		0x0200	// completely ignore, allowing non-closed brushes
#define SURF_NOLIGHT	0x0400	// Don't calculate light
#define SURF_BUMPLIGHT	0x0800	// calculate three lightmaps for the surface for bumpmapping
#define SURF_NOSHADOWS	0x1000	// Don't receive shadows
#define SURF_NODECALS	0x2000	// Don't receive decals
#define SURF_NOPAINT	SURF_NODECALS	// the surface can not have paint placed on it
#define SURF_NOCHOP		0x4000	// Don't subdivide patches on this surface 
#define SURF_HITBOX		0x8000	// surface is part of a hitbox

#define CHAR_TEX_ANTLION		'A'
#define CHAR_TEX_BLOODYFLESH	'B'
#define	CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_EGGSHELL		'E' ///< the egg sacs in the tunnels in ep2.
#define CHAR_TEX_FLESH			'F'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_ALIENFLESH		'H'
#define CHAR_TEX_CLIP			'I'
//#define CHAR_TEX_UNUSED		'J'
#define CHAR_TEX_SNOW			'K'
#define CHAR_TEX_PLASTIC		'L'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_SAND			'N'
#define CHAR_TEX_FOLIAGE		'O'
#define CHAR_TEX_COMPUTER		'P'
//#define CHAR_TEX_UNUSED		'Q'
#define CHAR_TEX_REFLECTIVE		'R'
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_TILE			'T'
#define CHAR_TEX_CARDBOARD		'U'
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_WOOD			'W'
//#define CHAR_TEX_UNUSED		'X' ///< do not use - "fake" materials use this (ladders, wading, clips, etc)
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_WARPSHIELD		'Z' ///< wierd-looking jello effect for advisor shield.

#define CHAR_TEX_STEAM_PIPE		11

// hhh macros
#define TIME_TO_TICKS( dt )		( (int)( 0.5 + (float)(dt) / INTERFACES::Globals->interval_per_tick ) )
#define TICKS_TO_TIME( t )		( INTERFACES::Globals->interval_per_tick *( t ) )



#include "UTILS\vmt.h"
#include "UTILS\vector3D.h"
#include "UTILS\vector2D.h"
#include "UTILS\vmatrix.h"
#include "UTILS\quaternion.h"
#include "UTILS\math.h"
#include "UTILS\logging.h"
//#include "UTILS\general_utils.h"
#include "UTILS\utils.h"
#include "UTILS\color.h"
#include "UTILS\string_encryption.h"

#include "UTILS\input_handler.h"
#include "UTILS\variables.h"

// for fsn
enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1, // (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};

enum MoveType
{
	MOVETYPE_NONE = 0, /**< never moves */
	MOVETYPE_ISOMETRIC, /**< For players */
	MOVETYPE_WALK, /**< Player only - moving on the ground */
	MOVETYPE_STEP, /**< gravity, special edge handling -- monsters use this */
	MOVETYPE_FLY, /**< No gravity, but still collides with stuff */
	MOVETYPE_FLYGRAVITY, /**< flies through the air + is affected by gravity */
	MOVETYPE_VPHYSICS, /**< uses VPHYSICS for simulation */
	MOVETYPE_PUSH, /**< no clip to world, push and crush */
	MOVETYPE_NOCLIP, /**< No gravity, no collisions, still do velocity/avelocity */
	MOVETYPE_LADDER, /**< Used by players only when going onto a ladder */
	MOVETYPE_OBSERVER, /**< Observer movement, depends on player's observer mode */
	MOVETYPE_CUSTOM, /**< Allows the entity to describe its own physics */
};

BOOL WINAPI DllMain(HINSTANCE Instance, DWORD Reason, LPVOID Reserved);
