#pragma once

namespace SDK
{
	class CUserCmd;
}

namespace FEATURES
{
	namespace MISC
	{
		class Autostrafer
		{
		public:
			void Do(SDK::CUserCmd* cmd);
			void Strafe(SDK::CUserCmd* cmd);

			static inline float GetIdealRotation(float speed)
			{
				return UTILS::Clamp<float>(RAD2DEG(std::atan2(15.f, speed)), 0.f, 45.f) * INTERFACES::Globals->interval_per_tick;
			} /// per tick

		public:
			float  m_speed;
			float  m_ideal;
			float  m_ideal2;
			Vector m_mins;
			Vector m_maxs;
			Vector m_origin;
			float  m_switch_value = 1.f;
			int    m_strafe_index;
			float  m_old_yaw;
			float  m_circle_yaw;
			bool   m_invert;

		private:
		};

		extern Autostrafer autostrafer;
	}
}
