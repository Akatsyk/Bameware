#pragma once

namespace SDK
{
	class CMoveData
	{
	public:
		bool m_bFirstRunOfFunctions : 1;
		bool m_bGameCodeMovedPlayer : 1;
		bool m_bNoAirControl : 1;

		unsigned long m_nPlayerHandle;
		int m_nImpulseCommand;
		Vector m_vecViewAngles;
		Vector m_vecAbsViewAngles;
		int m_nButtons;
		int m_nOldButtons;
		float m_flForwardMove;
		float m_flSideMove;
		float m_flUpMove;

		float m_flMaxSpeed;
		float m_flClientMaxSpeed;

		Vector m_vecVelocity;
		Vector m_vecOldVelocity;
		float somefloat;
		Vector m_vecAngles;
		Vector m_vecOldAngles;

		float m_outStepHeight;
		Vector m_outWishVel;
		Vector m_outJumpVel;

		Vector m_vecConstraintCenter;
		float m_flConstraintRadius;
		float m_flConstraintWidth;
		float m_flConstraintSpeedFactor;
		bool m_bConstraintPastRadius;

		void SetAbsOrigin(const Vector& vec);
		const Vector& GetAbsOrigin() const;

	private:
		Vector m_vecAbsOrigin; // edict::origin
	};
}
