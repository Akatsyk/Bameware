#include "../includes.h"

#include "Aimbot.h"

#include "../UTILS/interfaces.h"
#include "../UTILS/render.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/IEngine.h"
#include "../SDK/CTrace.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/NetChannel.h"
#include "../SDK/CGlobalVars.h"

#include "../FEATURES/Backtracking.h"
#include "../FEATURES/Resolver.h"
#include "../FEATURES/Autowall.h"

#include "../UTILS/Debugger.h"
#include "../FEATURES/InGameLogger.h"

#include "../FEATURES/Configurations.h"

namespace FEATURES
{
	namespace RAGEBOT
	{
		Aimbot aimbot;

		void Aimbot::Do(SDK::CUserCmd* cmd)
		{
			GLOBAL::can_shoot_someone = false;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0 || !SETTINGS::ragebot_configs.aimbot_enabled ||
				UTILS::IsWeaponGrenade(UTILS::GetWeapon(local_player)) || UTILS::GetCurtime() <= local_player->GetNextAttack())
				return;

			const auto BoundingBoxCheck = [this, local_player](SDK::CBaseEntity* entity, const Backtracking_Record& record) -> bool
			{
				const auto bbmin = record.bbmin + record.vec_origin;
				const auto bbmax = record.bbmax + record.vec_origin;

				/// the 4 corners on the top, 1 for the head, 1 for the middle of the body, 1 for the feet
				Vector points[7];

				points[0] = GetHitboxPosition(entity, record, 0);
				points[1] = (bbmin + bbmax) * 0.5f;
				points[2] = Vector((bbmax.x + bbmin.x) * 0.5f, (bbmax.y + bbmin.y) * 0.5f, bbmin.z);

				points[3] = bbmax;
				points[4] = Vector(bbmax.x, bbmin.y, bbmax.z);
				points[5] = Vector(bbmin.x, bbmin.y, bbmax.z);
				points[6] = Vector(bbmin.x, bbmax.y, bbmax.z);

				for (const auto& point : points)
				{
					if (autowall.CalculateDamage(local_player->GetEyePosition(), point, local_player, entity).damage > 0)
						return true;
				}

				return false;
			};

			const bool is_weapon_knife = UTILS::IsWeaponKnife(UTILS::GetWeapon(local_player));

			/// loop through every entity to find valid ones to aimbot
			for (int i = 0; i < 64; i++)
			{
				/// reset player aimbot info
				player_aimbot_info[i].head_damage = -1, player_aimbot_info[i].hitscan_damage = -1;

				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
				if (!entity || entity->GetIsDormant() || entity->GetImmunity() || entity->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam())
					continue;

				int most_damage = 0;
				for (auto& rec : backtracking.GetPriorityRecords(entity))
				{
					const int tick = CBacktracking::GetTickCount(rec);

					/// optimization
					if (!BoundingBoxCheck(entity, rec))
						continue;

					int random_ticks = 0;
					if (rec.is_exploiting)
						random_ticks = TIME_TO_TICKS(UTILS::GetLatency());

					backtracking.Restore(entity, rec);
					backtracking.ApplyRestore(entity, TICKS_TO_TIME(tick)); //don't need this cause it literally has nothing in it

					/// knifeboat
					if (is_weapon_knife)
					{
						if (DoKnifebot(entity, rec, player_aimbot_info[i].hitscan_position))
							player_aimbot_info[i].hitscan_damage = 1000;
						else
							player_aimbot_info[i].hitscan_damage = 0;

						rec.pre_computed_damage = player_aimbot_info[i].hitscan_damage;

						player_aimbot_info[i].m_is_foot = false;
						player_aimbot_info[i].head_damage = -1;

						player_aimbot_info[i].tick = tick;
						player_aimbot_info[i].backtrack_record = rec;
						player_aimbot_info[i].extrapolted_ticks = 0;
					}
					else
					{
						int cur_head_damage, cur_hitscan_damage;
						Vector cur_head_pos, cur_hitscan_pos;

						DoHeadAim(entity, rec, cur_head_pos, cur_head_damage);
						DoHitscan(entity, rec, cur_hitscan_pos, cur_hitscan_damage);

						rec.pre_computed_damage = UTILS::Max<int>(cur_head_damage, cur_hitscan_damage);

						if (cur_head_damage > most_damage || cur_hitscan_damage > most_damage)
						{
							most_damage = UTILS::Max<int>(cur_head_damage, cur_hitscan_damage);

							player_aimbot_info[i].head_damage = cur_head_damage;
							player_aimbot_info[i].hitscan_damage = cur_hitscan_damage;

							player_aimbot_info[i].head_position = cur_head_pos;
							player_aimbot_info[i].hitscan_position = cur_hitscan_pos;

							player_aimbot_info[i].tick = tick;
							player_aimbot_info[i].backtrack_record = rec;
							player_aimbot_info[i].extrapolted_ticks = 0;
						}

						RENDER::render_queue.AddDrawCircle(cur_head_pos, 2, 50, WHITE);
						RENDER::render_queue.AddDrawCircle(cur_hitscan_pos, 2, 50, WHITE);
					}
				}
			}

			int highest_damage = 0, best_tick = 0;
			FEATURES::RAGEBOT::Backtracking_Record last_backtrack_record;
			SDK::CBaseEntity* best_entity = nullptr;
			bool is_hitscan = false, is_ignore_head = false;
			Vector best_position;

			/// sort valid entities by most damage
			for (int i = 0; i < 32; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
				if (!entity || entity->GetIsDormant() || entity->GetImmunity() || entity->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam())
					continue;

				auto FillInfo = [&is_hitscan, &best_position, &best_entity, &last_backtrack_record, &highest_damage, &best_tick, entity, i, this](bool hitscan) -> void
				{
					is_hitscan = hitscan && !player_aimbot_info[i].m_is_foot, best_tick = player_aimbot_info[i].tick;
					best_entity = entity, last_backtrack_record = player_aimbot_info[i].backtrack_record;

					if (hitscan)
					{
						best_position = player_aimbot_info[i].hitscan_position;
						highest_damage = player_aimbot_info[i].hitscan_damage;
					}
					else
					{
						best_position = player_aimbot_info[i].head_position;
						highest_damage = player_aimbot_info[i].head_damage;
					}
				};

				const int hitscan_dmg = player_aimbot_info[i].hitscan_damage, head_dmg = player_aimbot_info[i].head_damage;

				if (is_weapon_knife)
				{
					if (hitscan_dmg > highest_damage)
					{
						FillInfo(true);
						break;
					}

					continue;
				}

				/// can't shoot this entity, continue
				if (hitscan_dmg <= 0 && head_dmg <= 0)
					continue;

				auto ShouldIgnoreHead = [](const Resolver::PlayerResolveRecord& resolve_record) -> bool
				{
					switch (SETTINGS::ragebot_configs.aimbot_ignore_head_type)
					{
					case 1: /// enabled
						return true;
					case 2: /// on key
						return SETTINGS::ragebot_configs.aimbot_ignore_head_keybind != -1 && GetAsyncKeyState(SETTINGS::ragebot_configs.aimbot_ignore_head_keybind);
					case 3: /// on fake soft
						return !(resolver.IsResolved(resolve_record.resolve_type) ||
							resolve_record.resolve_type & Resolver::RESOLVE_TYPE_ANTI_FREESTANDING ||
							resolve_record.resolve_type & Resolver::RESOLVE_TYPE_LBY);
					case 4: /// on fake hard
						return !resolver.IsResolved(resolve_record.resolve_type);

					default:
						return false;
					}
				};

				if (ShouldIgnoreHead(player_aimbot_info[i].backtrack_record.resolve_record))
				{
					if (hitscan_dmg > highest_damage && !player_aimbot_info[i].m_is_foot)
					{
						FillInfo(true);
						is_ignore_head = true;
					}

					continue;
				}

				/// one of these is not valid, choose the one that is valid
				if (hitscan_dmg <= 0 || head_dmg <= 0)
				{
					FillInfo(hitscan_dmg > head_dmg);
					continue;
				}

				/// hitscan if lethal or if more than head damage
				if (hitscan_dmg > entity->GetHealth() || hitscan_dmg > head_dmg)
				{
					FillInfo(true);
					if (hitscan_dmg > entity->GetHealth())
						break;

					continue;
				}

				const bool prefer_baim = SETTINGS::ragebot_configs.aimbot_prefer_baim_enabled ||
					(SETTINGS::ragebot_configs.aimbot_bodyaim_if_x_damage < player_aimbot_info[i].hitscan_damage);

				if (prefer_baim)
				{
					FillInfo(true);
					continue;
				}

				FillInfo(false);
			}

			static float duration_spent_aiming = 0.f;

			/// we have a valid target to shoot at
			if (best_entity)
			{
				GLOBAL::can_shoot_someone = true;

				duration_spent_aiming += INTERFACES::Globals->interval_per_tick;

				if (SETTINGS::ragebot_configs.aimbot_autostop_type == 1 && local_player->GetFlags() & FL_ONGROUND &&
					(highest_damage > SETTINGS::ragebot_configs.aimbot_autostop_damage_trigger ||
						highest_damage > best_entity->GetHealth() + 10) && !is_weapon_knife && !GLOBAL::is_fakewalking && UTILS::CanShoot())
				{
					cmd->sidemove = 0;
					cmd->forwardmove = local_player->GetVelocity().Length2D() > 20.f ? 450.f : 0.f;
					UTILS::RotateMovement(cmd, MATH::CalcAngle(Vector(0, 0, 0), local_player->GetVelocity()).y + 180.f);
				}

				if (SETTINGS::ragebot_configs.aimbot_autocrouch_enabled)
					cmd->buttons |= IN_DUCK;

				const bool should_right_click = is_weapon_knife && best_entity->GetHealth() > 30;
				if (UTILS::CanShoot(should_right_click))
				{
					/// autoscope
					if (!local_player->IsScopedIn() && UTILS::IsWeaponSniper(UTILS::GetWeapon(local_player)) && local_player->GetFlags() & FL_ONGROUND)
						cmd->buttons |= IN_ATTACK2;

					if (SETTINGS::ragebot_configs.aimbot_autostop_type != 0 && local_player->GetFlags() & FL_ONGROUND &&
						(highest_damage > SETTINGS::ragebot_configs.aimbot_autostop_damage_trigger || highest_damage > best_entity->GetHealth())
						&& !is_weapon_knife
						&& !GLOBAL::is_fakewalking						
						&& UTILS::CanShoot())
					{
						cmd->sidemove = 0;
						cmd->forwardmove = local_player->GetVelocity().Length2D() > 20.f ? 450.f : 0.f;
						UTILS::RotateMovement(cmd, MATH::CalcAngle(Vector(0, 0, 0), local_player->GetVelocity()).y + 180.f);
					}

					/// delay shot
					auto DelayShot = [best_entity, last_backtrack_record, highest_damage]() -> bool
					{
						/// jumping
						if (!(best_entity->GetFlags() & FL_ONGROUND))
							return true;

						if (SETTINGS::ragebot_configs.aimbot_delay_shot_type == 0) /// damage
						{
							if (highest_damage >= best_entity->GetHealth() + 10)
								return true;

							return duration_spent_aiming >= SETTINGS::ragebot_configs.aimbot_delay_shot;
						}
						else /// resolved
						{
							if (resolver.IsResolved(last_backtrack_record.resolve_record.resolve_type))
								return true;

							return duration_spent_aiming >= SETTINGS::ragebot_configs.aimbot_delay_shot;
						}
					};
					if (is_weapon_knife || DelayShot())
					{
						/// need to restore to the player's backtrack record so that hitchance can work properly
						backtracking.Restore(best_entity, last_backtrack_record);
						backtracking.ApplyRestore(best_entity, best_tick);

						/// if is accurate enough to shoot, then shoot... (hitchance, spread limit, etc)

						const auto net_channel = INTERFACES::Engine->GetNetChannel();
						auto weapon = UTILS::GetWeapon(local_player);

						const bool can_shoot = is_weapon_knife || (weapon && weapon->GetItemDefinitionIndex() == SDK::WEAPON_REVOLVER) ||
							(IsAccurate(best_entity, best_position) && (net_channel && net_channel->m_choked_packets));

						if (can_shoot)
						{
							if (should_right_click)
								cmd->buttons |= IN_ATTACK2;
							else
								cmd->buttons |= IN_ATTACK;

							cmd->viewangles = MATH::CalcAngle(local_player->GetEyePosition(), best_position) - (local_player->GetPunchAngles() * 2.f);
							cmd->tick_count = best_tick;

							// RENDER::render_queue.AddDrawCircle(best_position, 2, 50, RED, 3.5);
							//INTERFACES::DebugOverlay->AddBoxOverlay(best_position, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, 0, 0, 127, 3.f);

							/// resolver
							if (!is_weapon_knife)
								resolver.AddShotSnapshot(best_entity, last_backtrack_record.resolve_record);

							/// psilent
							if (!GLOBAL::is_fakewalking)
							{
								GLOBAL::should_send_packet = false;
								GLOBAL::disable_fake_lag = false; //true
							}
						}
					}
				}
				else if (const auto weapon = UTILS::GetWeapon(local_player); weapon && weapon->GetItemDefinitionIndex() == SDK::WEAPON_REVOLVER)
					cmd->buttons |= IN_ATTACK;
			}

			/*	for (int i = 0; i < 32; i++)
				{
					auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
					if (!entity || entity->GetIsDormant() || entity->GetImmunity() || entity->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam())
						continue;

					backtracking.RestoreToCurrentRecord(entity);
				}	*/

			backtracking.RestoreAllToCurrentRecord();

			if (!best_entity)
				duration_spent_aiming = 0.f;
		}

		bool Aimbot::DoHitscan(SDK::CBaseEntity* entity, const FEATURES::RAGEBOT::Backtracking_Record& bt_rec, Vector& final_position, int& final_damage)
		{
			/// default value
			final_damage = 0;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return false;

			auto weapon = UTILS::GetWeapon(local_player);
			if (!weapon)
				return false;

			const auto eye_position = local_player->GetEyePosition();
			const bool is_taser = UTILS::IsWeaponTaser(weapon);
			const bool should_ignore_limbs = SETTINGS::ragebot_configs.aimbot_ignore_limbs_when_moving && entity->GetVelocity().Length2D() > 0.f;
			const float pointscale = SETTINGS::ragebot_configs.aimbot_hitscan_pointscale * 0.01f;

			/// if taser or low on ammo, set minimum damage to their health so that it kills in a single shot
			const int minimum_damage = (is_taser || weapon->GetAmmo() <= 2) ? entity->GetHealth() + 10 : UTILS::Min<int>(SETTINGS::ragebot_configs.aimbot_minimum_damage, entity->GetHealth() + 10);
			const int minimum_autowall_damage = (is_taser || weapon->GetAmmo() <= 2) ? entity->GetHealth() + 10 : UTILS::Min<int>(SETTINGS::ragebot_configs.aimbot_minimum_autowall_damage, entity->GetHealth() + 10);

			/// functions for creating the hitscan positions
			const auto ConstructHitscanMultipointPositions = [this, entity, eye_position, local_player, is_taser, &bt_rec](bool ignore_limbs, bool secondary = false) -> std::vector<std::pair<Vector, int>>
			{
				std::vector<std::pair<Vector, int>> positions;

				if (is_taser)
					return positions;

				const auto CreateAndAddBufferToVec = [this, entity, &bt_rec](std::vector<std::pair<Vector, int>>& vec, int hitbox_index, float pointscale) -> void
				{
					Vector buffer[6];
					if (!GetMultipointPositions(entity, bt_rec, buffer, hitbox_index, pointscale))
						return;

					for (auto& i : buffer)
						vec.emplace_back(i, hitbox_index);
				};
				const auto GetSpreadDistance = [this, local_player](float distance) -> float
				{
					auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
					if (!weapon)
						return 0.f;

					Vector viewangles;
					INTERFACES::Engine->GetViewAngles(viewangles);

					weapon->UpdateAccuracyPenalty();
					const float spread = weapon->GetSpreadCone();
					const float inaccuracy = weapon->GetInnaccuracy();

					Vector forward, right, up;
					MATH::AngleVectors(viewangles, &forward, &right, &up);

					float random_a = 0;
					float random_b = 1.0f - random_a * random_a;

					random_a = M_PI_F * 2.0f;
					random_b *= spread;

					const float spread_x1 = (cos(random_a) * random_b);
					const float spread_y1 = (sin(random_a) * random_b);

					float random_c = 0;
					float random_f = 1.0f - random_c * random_c;

					random_c = M_PI_F * 2.0f;
					random_f *= inaccuracy;

					const float spread_x2 = (cos(random_c) * random_f);
					const float spread_y2 = (sin(random_c) * random_f);

					const float spread_x = spread_x1 + spread_x2;
					const float spread_y = spread_y1 + spread_y2;

					Vector spread_forward;
					spread_forward[0] = forward[0] + (spread_x * right[0]) + (spread_y * up[0]);
					spread_forward[1] = forward[1] + (spread_x * right[1]) + (spread_y * up[1]);
					spread_forward[2] = forward[2] + (spread_x * right[2]) + (spread_y * up[2]);
					spread_forward.NormalizeInPlace();

					Vector new_angle;
					MATH::VectorAngles(spread_forward, new_angle);
					new_angle = MATH::NormalizeAngle(new_angle);

					Vector end;
					MATH::AngleVectors(new_angle, &end);

					return ((end - forward) * distance).Length();
				};
				const auto GetIdealPointscale = [this, entity, eye_position, GetSpreadDistance, secondary](int hitbox_index) -> float
				{
					if (secondary)
						return SETTINGS::ragebot_configs.aimbot_duel_multipoint_pointscale * 0.01f;
					else
						return SETTINGS::ragebot_configs.aimbot_hitscan_pointscale * 0.01f;
				};

				if (SETTINGS::ragebot_configs.aimbot_selected_multipoint_hitgroups[0]) /// chest
				{
					CreateAndAddBufferToVec(positions, 6, GetIdealPointscale(6));
					CreateAndAddBufferToVec(positions, 5, GetIdealPointscale(5));
					CreateAndAddBufferToVec(positions, 4, GetIdealPointscale(4));
				}
				if (SETTINGS::ragebot_configs.aimbot_selected_multipoint_hitgroups[1]) /// stomach
				{
					CreateAndAddBufferToVec(positions, 3, GetIdealPointscale(3));
					CreateAndAddBufferToVec(positions, 2, GetIdealPointscale(2));
				}
				if (SETTINGS::ragebot_configs.aimbot_selected_multipoint_hitgroups[2] && !ignore_limbs) /// arms
				{
					CreateAndAddBufferToVec(positions, 17, GetIdealPointscale(17));
					CreateAndAddBufferToVec(positions, 15, GetIdealPointscale(15));
				}
				if (SETTINGS::ragebot_configs.aimbot_selected_multipoint_hitgroups[3] && !ignore_limbs) /// legs
				{
					CreateAndAddBufferToVec(positions, 8, GetIdealPointscale(8));
					CreateAndAddBufferToVec(positions, 7, GetIdealPointscale(7));
				}
				if (SETTINGS::ragebot_configs.aimbot_selected_multipoint_hitgroups[4] && !ignore_limbs) /// feet
				{
					CreateAndAddBufferToVec(positions, 12, GetIdealPointscale(12));
					CreateAndAddBufferToVec(positions, 11, GetIdealPointscale(11));
				}

				return positions;
			};
			const auto ConstructHitscanPositions = [this, entity, &bt_rec, is_taser](bool ignore_limbs) -> std::vector<std::pair<Vector, int>>
			{
				std::vector<std::pair<Vector, int>> positions;

				if (is_taser)
				{
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 6), 6);
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 5), 5);
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 4), 4);
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 3), 3);
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 2), 2);

					return positions;
				}

				if (SETTINGS::ragebot_configs.aimbot_selected_hitgroups[0]) /// chest
				{
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 6), 6);
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 5), 5);
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 4), 4);
				}
				if (SETTINGS::ragebot_configs.aimbot_selected_hitgroups[1]) /// stomach
				{
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 3), 3);
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 2), 2);
				}
				if (SETTINGS::ragebot_configs.aimbot_selected_hitgroups[2] && !ignore_limbs) /// arms
				{
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 17), 17);
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 15), 15);
				}
				if (SETTINGS::ragebot_configs.aimbot_selected_hitgroups[3] && !ignore_limbs) /// legs
				{
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 8), 8);
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 7), 7);
				}
				if (SETTINGS::ragebot_configs.aimbot_selected_hitgroups[4] && !ignore_limbs) /// feet
				{
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 12), 12);
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 11), 11);
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 10), 10);
					positions.emplace_back(GetHitboxPosition(entity, bt_rec, 9), 9);
				}

				return positions;
			};

			/// get the positions
			const auto hitscan_mp_positions = ConstructHitscanMultipointPositions(should_ignore_limbs);
			const auto hitscan_secondary_mp_positions = ConstructHitscanMultipointPositions(true, true);
			const auto hitscan_positions = ConstructHitscanPositions(should_ignore_limbs);

			/// get the best multipoint hitscan position
			int mp_damage = -1, mp_min_damage = 0; Vector mp_position;
			for (const auto& pair : hitscan_mp_positions)
			{
				const auto info = FEATURES::RAGEBOT::autowall.CalculateDamage(eye_position, pair.first, local_player, entity, is_taser ? 0 : -1);
				const int min_dmg = info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;

				if (info.damage > mp_damage)
				{
					mp_damage = info.damage, mp_position = pair.first;
					mp_min_damage = info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;
				}
			}

			/// do secondary multipoint positions
			if (mp_damage < 20 && SETTINGS::ragebot_configs.aimbot_duel_multipoint_enabled)
			{
				for (const auto& pair : hitscan_secondary_mp_positions)
				{
					const auto info = FEATURES::RAGEBOT::autowall.CalculateDamage(eye_position, pair.first, local_player, entity, is_taser ? 0 : -1);
					const int min_dmg = info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;

					if (info.damage > mp_damage)
					{
						mp_damage = info.damage, mp_position = pair.first;
						mp_min_damage = info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;
					}
				}
			}

			/// get the best hitscan position
			int hitscan_damage = -1, hitscan_min_damage = 0, hitscan_hitbox = 0;
			bool is_hitscan_foot = false; Vector hitscan_position;
			for (const auto& pair : hitscan_positions)
			{
				const auto info = FEATURES::RAGEBOT::autowall.CalculateDamage(eye_position, pair.first, local_player, entity, is_taser ? 0 : -1);
				const bool has_pos_already = hitscan_damage != -1;

				if (info.damage > hitscan_damage || is_hitscan_foot)
				{
					hitscan_hitbox = pair.second;
					hitscan_damage = info.damage, hitscan_position = pair.first;
					hitscan_min_damage = info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;
				}
			}

			/// no positions deal enough damage
			if (hitscan_damage < hitscan_min_damage && mp_damage < mp_min_damage)
				return false;

			/// one of these are not valid, choose the one that is valid
			if (hitscan_damage < hitscan_min_damage || mp_damage < mp_min_damage)
			{
				if (hitscan_damage >= hitscan_min_damage)
				{
					final_position = hitscan_position;
					final_damage = hitscan_damage;
					return true;
				}
				else
				{
					final_position = mp_position;
					final_damage = mp_damage;
					return true;
				}
			}

			if (hitscan_damage > mp_damage)
			{
				final_position = hitscan_position;
				final_damage = hitscan_damage;
			}
			else
			{
				final_position = mp_position;
				final_damage = mp_damage;
			}

			//	RENDER::render_queue.AddDrawCircle(mp_position, 2, 50, RED);

			return true;
		}

		bool Aimbot::DoHeadAim(SDK::CBaseEntity* entity, const FEATURES::RAGEBOT::Backtracking_Record& bt_rec, Vector& final_position, int& final_damage)
		{
			/// default value
			final_damage = 0;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return false;

			auto weapon = UTILS::GetWeapon(local_player);
			if (!weapon)
				return false;

			if (UTILS::IsWeaponTaser(weapon))
				return false;

			const auto eye_position = local_player->GetEyePosition();
			const float pointscale = SETTINGS::ragebot_configs.aimbot_pointscale * 0.01f;

			/// if taser or low on ammo, set minimum damage to their health so that it kills in a single shot
			const int minimum_damage = (weapon->GetAmmo() <= 2) ? entity->GetHealth() : UTILS::Min<int>(SETTINGS::ragebot_configs.aimbot_minimum_damage, entity->GetHealth() + 10);
			const int minimum_autowall_damage = (weapon->GetAmmo() <= 2) ? entity->GetHealth() : UTILS::Min<int>(SETTINGS::ragebot_configs.aimbot_minimum_autowall_damage, entity->GetHealth() + 10);

			/// center of the hitbox
			const auto head_center_position = GetHitboxPosition(entity, bt_rec, 0);
			const auto head_center_info = FEATURES::RAGEBOT::autowall.CalculateDamage(eye_position, head_center_position, local_player, entity);
			const int head_center_damage = head_center_info.damage,
				head_center_min_damage = head_center_info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;

			/// the multipoints around the center of the hitbox
			Vector head_multipoints[6];
			GetMultipointPositions(entity, bt_rec, head_multipoints, 0, pointscale);

			/// get the multipoint position that does the most damage
			Vector best_multipoint_position;
			int best_multipoint_damage = -1, multipoint_min_damage = 0;
			for (const auto& head_multipoint : head_multipoints)
			{
				const auto info = FEATURES::RAGEBOT::autowall.CalculateDamage(eye_position, head_multipoint, local_player, entity);

				if (info.damage > best_multipoint_damage)
				{
					best_multipoint_damage = info.damage, best_multipoint_position = head_multipoint;
					multipoint_min_damage = info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;
				}
			}

			/// none are valid
			if (head_center_damage < minimum_damage && best_multipoint_damage < minimum_damage)
				return false;

			/// one of these aren't valid, choose the valid one
			if (head_center_damage < head_center_min_damage || best_multipoint_damage < multipoint_min_damage)
			{
				if (head_center_damage > best_multipoint_damage)
				{
					final_position = head_center_position;
					final_damage = head_center_damage;
				}
				else
				{
					final_position = best_multipoint_position;
					final_damage = best_multipoint_damage;
				}
			}
			else /// both are valid, do some more decision making 
			{
				/// decide whether to shoot the center of the hitbox or the multipoints of the hitbox
				if (head_center_damage > entity->GetHealth() + 10 || head_center_damage + 10 > best_multipoint_damage)
				{
					final_position = head_center_position;
					final_damage = head_center_damage;
				}
				else
				{
					final_position = best_multipoint_position;
					final_damage = best_multipoint_damage;
				}
			}

			//	RENDER::render_queue.AddDrawCircle(best_multipoint_position, 2, 50, RED);

			return true;
		}

		bool Aimbot::DoKnifebot(SDK::CBaseEntity* entity, const FEATURES::RAGEBOT::Backtracking_Record& bt_rec, Vector& final_position)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return false;

			const auto eye_position = local_player->GetEyePosition();
			const bool right_click = entity->GetHealth() > 30;
			/// 6 - 3

			float shortest_distance = FLT_MAX;
			for (int i = 0; i < 4; i++)
			{
				const auto position = GetHitboxPosition(entity, bt_rec, 3 + i);

				SDK::Ray_t ray;
				ray.Init(eye_position, position);
				SDK::CTraceFilterOneEntity filter;
				filter.pEntity = entity;

				SDK::trace_t trace;
				INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

				if (const float distance = (trace.end - eye_position).Length(); trace.m_pEnt == entity && distance < shortest_distance)
				{
					shortest_distance = distance;
					final_position = trace.end;
				}
			}

			return shortest_distance < (right_click ? 55.f : 74.f);
		}

		bool Aimbot::IsAccurate(SDK::CBaseEntity* entity, Vector position) const
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return false;

			auto weapon = UTILS::GetWeapon(local_player);
			if (!weapon)
				return false;

			if (UTILS::IsWeaponKnife(weapon) || UTILS::IsWeaponTaser(weapon))
				return true;

			switch (SETTINGS::ragebot_configs.aimbot_accuracy_type)
			{
			case 0: /// none
				return true;
			case 1: /// hitchance
			{
				const auto RandomFloat = [](float min, float max) -> float
				{
					typedef float(*RandomFloat_t)(float, float);
					static auto random_float = reinterpret_cast<RandomFloat_t>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat"));
					return random_float(min, max);
				};

				const auto angle = MATH::NormalizeAngle(MATH::CalcAngle(local_player->GetEyePosition(), position));
				const auto spread = weapon->GetSpreadCone();
				const auto inaccuracy = weapon->GetInnaccuracy();
				auto eye_position = local_player->GetEyePosition();

				Vector forward, right, up;
				MATH::AngleVectors(angle, &forward, &right, &up);

				int num_hit = 0;
				weapon->UpdateAccuracyPenalty();

				for (int i = 0; i < 256; i++)
				{
					float random_a = RandomFloat(0.0f, 1.0f);
					float random_b = 1.0f - random_a * random_a;

					random_a = RandomFloat(0.0f, M_PI_F * 2.0f);
					random_b *= weapon->GetSpreadCone();

					const float spread_x1 = (cos(random_a) * random_b);
					const float spread_y1 = (sin(random_a) * random_b);

					float random_c = RandomFloat(0.0f, 1.0f);
					float random_f = 1.0f - random_c * random_c;

					random_c = RandomFloat(0.0f, M_PI_F * 2.0f);
					random_f *= weapon->GetInnaccuracy();

					const float spread_x2 = (cos(random_c) * random_f);
					const float spread_y2 = (sin(random_c) * random_f);

					const float spread_x = spread_x1 + spread_x2;
					const float spread_y = spread_y1 + spread_y2;

					Vector spread_forward;
					spread_forward[0] = forward[0] + (spread_x * right[0]) + (spread_y * up[0]);
					spread_forward[1] = forward[1] + (spread_x * right[1]) + (spread_y * up[1]);
					spread_forward[2] = forward[2] + (spread_x * right[2]) + (spread_y * up[2]);
					spread_forward.NormalizeNoClamp();


					Vector new_angle;
					MATH::VectorAngles(spread_forward, new_angle);
					new_angle = MATH::NormalizeAngle(new_angle);

					Vector end;
					MATH::AngleVectors(new_angle, &end);
					end = eye_position + (end * 8092.f);

					SDK::trace_t trace;
					SDK::Ray_t ray;
					ray.Init(eye_position, end);

					SDK::CTraceFilterOneEntity2 filter;
					filter.pEntity = entity;

					INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace); //MASK_ALL //Side note fatality does MASK_SHOT_HULL | CONTENTS_HITBOX
					if (trace.m_pEnt == entity)
						num_hit++;
				}

				return num_hit / 256.f >= SETTINGS::ragebot_configs.aimbot_accuracy / 100.f;

			}
			case 2: /// spread limit
			{
				return weapon->GetInnaccuracy() <= SETTINGS::ragebot_configs.aimbot_accuracy;
			}
			}

			return false;
		}

		bool Aimbot::GetMultipointPositions(SDK::CBaseEntity* entity, const FEATURES::RAGEBOT::Backtracking_Record& bt_rec, Vector* positions, int hitbox_index, float pointscale)
		{
			const auto hitbox = GetHitbox(entity, hitbox_index);
			if (!hitbox)
				return false;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return false;

			const float hitbox_radius = hitbox->m_flRadius * pointscale;
			const auto bone_matrix = entity->GetBoneMatrix(hitbox->bone);

			Vector bbmin, bbmax;
			MATH::VectorTransform(hitbox->bbmin, bone_matrix, bbmin);
			MATH::VectorTransform(hitbox->bbmax, bone_matrix, bbmax);

			Vector right_direction, left_direction;
			MATH::AngleVectors(Vector(0.f, MATH::CalcAngle(local_player->GetVecOrigin(), entity->GetVecOrigin()).y + 90.f, 0.f), &right_direction);
			MATH::AngleVectors(Vector(0.f, MATH::CalcAngle(local_player->GetVecOrigin(), entity->GetVecOrigin()).y - 90.f, 0.f), &left_direction);

			if (hitbox->m_flRadius < 0.f)
				return false;

			positions[0] = bbmax + Vector(0.f, 0.f, hitbox_radius); /// top
			positions[1] = bbmin - Vector(0.f, 0.f, hitbox_radius); /// bottom

			positions[2] = bbmax + (right_direction * hitbox_radius);
			positions[3] = bbmax + (left_direction * hitbox_radius);

			positions[4] = bbmin + (right_direction * hitbox_radius);
			positions[5] = bbmin + (left_direction * hitbox_radius);

			return true;
		}

		Vector Aimbot::GetHitboxPosition(SDK::CBaseEntity* entity, const FEATURES::RAGEBOT::Backtracking_Record& bt_rec, int hitbox_index) const
		{
			const auto hitbox = GetHitbox(entity, hitbox_index);
			if (!hitbox)
				return Vector(0, 0, 0);

			const auto bone_matrix = entity->GetBoneMatrix(hitbox->bone);

			Vector bbmin, bbmax;
			MATH::VectorTransform(hitbox->bbmin, bone_matrix, bbmin);
			MATH::VectorTransform(hitbox->bbmax, bone_matrix, bbmax);

			const auto center = (bbmin + bbmax) * 0.5f;
			if (hitbox_index == 12 || hitbox_index == 13)
				return center + Vector(0, 0, 3);
			else
				return center;
		}

		SDK::mstudiobbox_t* Aimbot::GetHitbox(SDK::CBaseEntity* entity, int hitbox_index)
		{
			if (!entity || entity->GetIsDormant() || entity->GetHealth() <= 0)
				return nullptr;

			const auto model = entity->GetModel();
			if (!model)
				return nullptr;

			const auto studio_hdr = INTERFACES::ModelInfo->GetStudioModel(model);
			if (!studio_hdr)
				return nullptr;

			const auto set = studio_hdr->GetHitboxSet(0);
			if (!set)
				return nullptr;

			if (hitbox_index >= set->numhitboxes || hitbox_index < 0)
				return nullptr;

			return set->GetHitbox(hitbox_index);
		};

		void Aimbot::DrawShotHitboxes(SDK::CBaseEntity* entity, CColor color, float duration) const
		{
			const auto model = entity->GetModel();
			if (!model)
				return;

			const auto hdr = INTERFACES::ModelInfo->GetStudioModel(model);
			if (!hdr)
				return;

			const auto hitbox_set = hdr->GetHitboxSet(0);
			if (!hitbox_set)
				return;

			for (int i = 0; i < hitbox_set->numhitboxes; i++)
			{
				auto hitbox = hitbox_set->GetHitbox(i);
				if (!hitbox)
					continue;

				const auto matrix = entity->GetBoneMatrix(hitbox->bone);

				Vector min, max;
				MATH::VectorTransform(hitbox->bbmin, matrix, min);
				MATH::VectorTransform(hitbox->bbmax, matrix, max);

				INTERFACES::DebugOverlay->DrawPill(min, max, hitbox->m_flRadius, color.RGBA[0], color.RGBA[1], color.RGBA[2], color.RGBA[3], duration);
			}
		}
	}
}
