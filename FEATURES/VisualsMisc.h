#pragma once

namespace FEATURES
{
	namespace VISUALS
	{
		class VisualsMisc
		{
		public:
			void Do();

			void DisableFlashDuration();

			void NoSmoke();

			SDK::IMaterial* smoke1;
			SDK::IMaterial* smoke2;
			SDK::IMaterial* smoke3;
			SDK::IMaterial* smoke4;

		private:
			void DrawCrosshair();
			void DrawGrenadePrediction();
			void DrawSnaplines();
			void DrawSpreadCircle();
			void DrawAutowallCrosshair();
		};

		extern VisualsMisc visuals_misc;
	}
}
