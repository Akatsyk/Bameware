#include "../includes.h"

#include "../UTILS/interfaces.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CUserCmd.h"

#include "Configurations.h"

#include "Autostrafer.h"

constexpr float pi = 3.1415926535897932384f; // pi
__forceinline constexpr float rad_to_deg(float val) {
	return val * (180.f / pi);
}

namespace FEATURES
{
	namespace MISC
	{
		Autostrafer autostrafer;

		void Autostrafer::Do(SDK::CUserCmd* cmd) // Jono 
		{
			if (!SETTINGS::main_configs.autostrafer_enabled)
				return;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0 || (local_player->GetFlags() & FL_ONGROUND && !(cmd->buttons & IN_JUMP)))
				return;

			Vector viewangles;
			INTERFACES::Engine->GetViewAngles(viewangles);

			static bool side_switch = false;
			side_switch = !side_switch;

			cmd->forwardmove = 0.f;
			cmd->sidemove = side_switch ? 450.f : -450.f;

			const float velocity_yaw = MATH::CalcAngle(Vector(0, 0, 0), local_player->GetVelocity()).y;
			const float ideal_rotation = GetIdealRotation(local_player->GetVelocity().Length2D()) / INTERFACES::Globals->interval_per_tick;
			const float ideal_yaw_rotation = (side_switch ? ideal_rotation : -ideal_rotation) + (fabs(MATH::NormalizeYaw(velocity_yaw - viewangles.y)) < 5.f ? velocity_yaw : viewangles.y);

			UTILS::RotateMovement(cmd, ideal_yaw_rotation);
		}

		void Autostrafer::Strafe(SDK::CUserCmd* cmd) { // Shit
			Vector velocity;
			float  delta, abs_delta, velocity_delta, correct;

			auto local = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local || local->GetHealth() <= 0 || (local->GetFlags() & FL_ONGROUND && !(cmd->buttons & IN_JUMP)))
				return;

			// don't strafe while noclipping or on ladders..
			if (local->GetMovetype() == MOVETYPE_NOCLIP || local->GetMovetype() == MOVETYPE_LADDER)
				return;

			// get networked velocity ( maybe absvelocity better here? ).
			// meh, should be predicted anyway? ill see.
			velocity = local->GetVelocity();

			// get the velocity len2d ( speed ).
			m_speed = velocity.Length2D();

			// compute the ideal strafe angle for our velocity.
			m_ideal = (m_speed > 0.f) ? rad_to_deg(std::asin(15.f / m_speed)) : 90.f;
			m_ideal2 = (m_speed > 0.f) ? rad_to_deg(std::asin(30.f / m_speed)) : 90.f;

			// some additional sanity.
			std::clamp(m_ideal, 0.f, 90.f);
			std::clamp(m_ideal2, 0.f, 90.f);

			// save our origin
			m_origin = local->GetVecOrigin();

			// for changing direction.
			// we want to change strafe direction every call.
			m_switch_value *= -1.f;

			// for allign strafer.
			++m_strafe_index;

			if (GLOBAL::pressing_move && SETTINGS::main_configs.autostrafer_enabled) {
				// took this idea from stacker, thank u !!!!
				enum EDirections {
					FORWARDS = 0,
					BACKWARDS = 180,
					LEFT = 90,
					RIGHT = -90,
					BACK_LEFT = 135,
					BACK_RIGHT = -135
				};

				float wish_dir{ };

				// get our key presses.
				bool holding_w = cmd->buttons & IN_FORWARD;
				bool holding_a = cmd->buttons & IN_MOVELEFT;
				bool holding_s = cmd->buttons & IN_BACK;
				bool holding_d = cmd->buttons & IN_MOVERIGHT;

				// move in the appropriate direction.
				if (holding_w) {
					//	forward left
					if (holding_a) {
						wish_dir += (EDirections::LEFT / 2);
					}
					//	forward right
					else if (holding_d) {
						wish_dir += (EDirections::RIGHT / 2);
					}
					//	forward
					else {
						wish_dir += EDirections::FORWARDS;
					}
				}
				else if (holding_s) {
					//	back left
					if (holding_a) {
						wish_dir += EDirections::BACK_LEFT;
					}
					//	back right
					else if (holding_d) {
						wish_dir += EDirections::BACK_RIGHT;
					}
					//	back
					else {
						wish_dir += EDirections::BACKWARDS;
					}

					cmd->forwardmove = 0;
				}
				else if (holding_a) {
					//	left
					wish_dir += EDirections::LEFT;
				}
				else if (holding_d) {
					//	right
					wish_dir += EDirections::RIGHT;
				}

				GLOBAL::strafe_angles.y += MATH::NormalizeYaw(wish_dir);
			}

			// cancel out any forwardmove values.
			cmd->forwardmove = 0.f;

			if (!SETTINGS::main_configs.autostrafer_enabled)
				return;

			// get our viewangle change.
			delta = MATH::NormalizedAngle(GLOBAL::strafe_angles.y - m_old_yaw);

			// convert to absolute change.
			abs_delta = std::abs(delta);

			// save old yaw for next call.
			m_circle_yaw = m_old_yaw = GLOBAL::strafe_angles.y;

			// set strafe direction based on mouse direction change.
			if (delta > 0.f)
				cmd->sidemove = -450.f;

			else if (delta < 0.f)
				cmd->sidemove = 450.f;

			// we can accelerate more, because we strafed less then needed
			// or we got of track and need to be retracked.
			if (abs_delta <= m_ideal || abs_delta >= 30.f) {
				// compute angle of the direction we are traveling in.
				Vector velocity_angle;
				MATH::VectorAngles(velocity, velocity_angle);

				// get the delta between our direction and where we are looking at.
				velocity_delta = MATH::NormalizeYaw(GLOBAL::strafe_angles.y - velocity_angle.y);

				// correct our strafe amongst the path of a circle.
				correct = m_ideal;

				if (velocity_delta <= correct || m_speed <= 15.f) {
					// not moving mouse, switch strafe every tick.
					if (-correct <= velocity_delta || m_speed <= 15.f) {
						GLOBAL::strafe_angles.y += (m_ideal * m_switch_value);
						cmd->sidemove = 450.f * m_switch_value;
					}

					else {
						GLOBAL::strafe_angles.y = velocity_angle.y - correct;
						cmd->sidemove = 450.f;
					}
				}

				else {
					GLOBAL::strafe_angles.y = velocity_angle.y + correct;
					cmd->sidemove = -450.f;
				}
			}
		}
	}
}
