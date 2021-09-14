#define NOCOLOR CColor(0, 0, 0, 0)
#define WHITE CColor(255, 255, 255, 255)
#define BLACK CColor(0, 0, 0, 255)
#define RED CColor(255, 0, 0, 255)
#define LIGHTRED CColor(255, 100, 100, 255)
#define WHITERED CColor(255, 65, 65, 255);
#define DARKRED CColor(120, 0, 0, 255)
#define GREEN CColor(0, 255, 0, 255)
#define BLUE CColor(0, 0, 255, 255)
#define LIGHTBLUE CColor(0, 140, 255, 255)
#define DARKGREY CColor(55, 55, 55, 255)
#define GREY CColor(70, 70, 70, 255)
#define LIGHTGREY CColor(150, 150, 150, 255)
#define HOTPINK CColor(255, 20, 147, 255)
#define CYAN CColor(0, 255, 255, 255)
#define YELLOW CColor(255, 255, 0, 255)


class CColor
{
public:
	unsigned char RGBA[4];

	CColor()
	{
		RGBA[0] = 255;
		RGBA[1] = 255;
		RGBA[2] = 255;
		RGBA[3] = 255;
	}
	CColor(int r, int g, int b, int a = 255)
	{
		RGBA[0] = r;
		RGBA[1] = g;
		RGBA[2] = b;
		RGBA[3] = a;
	}

	inline int r() const
	{
		return RGBA[0];
	}

	inline int g() const
	{
		return RGBA[1];
	}

	inline int b() const
	{
		return RGBA[2];
	}

	inline int a() const
	{
		return RGBA[3];
	}


	bool operator!=(CColor color) { return RGBA[0] != color.RGBA[0] || RGBA[1] != color.RGBA[1] || RGBA[2] != color.RGBA[2] || RGBA[3] != color.RGBA[3]; }
	bool operator==(CColor color) { return RGBA[0] == color.RGBA[0] && RGBA[1] == color.RGBA[1] && RGBA[2] == color.RGBA[2] && RGBA[3] == color.RGBA[3]; }


	// returns the color from 0.f - 1.f
	static float Base(const unsigned char col)
	{
		return col / 255.f;
	}
	static CColor Inverse(const CColor color)
	{
		return CColor(255 - color.RGBA[0], 255 - color.RGBA[1], 255 - color.RGBA[2]);
	}
	float Difference(const CColor color) const // from 0.f - 1.f with 1.f being the most different
	{
		float red_diff = fabs(Base(RGBA[0]) - Base(color.RGBA[0]));
		float green_diff = fabs(Base(RGBA[1]) - Base(color.RGBA[1]));
		float blue_diff = fabs(Base(RGBA[2]) - Base(color.RGBA[2]));
		float alpha_diff = fabs(Base(RGBA[3]) - Base(color.RGBA[3]));

		return (red_diff + green_diff + blue_diff + alpha_diff) * 0.25f;
	}

	// RGB -> HSB
	static float Hue(const CColor color)
	{
		float R = Base(color.RGBA[0]);
		float G = Base(color.RGBA[1]);
		float B = Base(color.RGBA[2]);

		float mx = max(R, max(G, B));
		float mn = min(R, min(G, B));
		if (mx == mn)
			return 0.f;

		float delta = mx - mn;

		float hue = 0.f;
		if (mx == R)
			hue = (G - B) / delta;
		else if (mx == G)
			hue = 2.f + (B - R) / delta;
		else
			hue = 4.f + (R - G) / delta;

		hue *= 60.f;
		if (hue < 0.f)
			hue += 360.f;

		return hue / 360.f;
	}
	static float Saturation(const CColor color)
	{
		float R = Base(color.RGBA[0]);
		float G = Base(color.RGBA[1]);
		float B = Base(color.RGBA[2]);

		float mx = max(R, max(G, B));
		float mn = min(R, min(G, B));

		float delta = mx - mn;

		if (mx == 0.f)
			return delta;

		return delta / mx;
	}
	static float Brightness(const CColor color)
	{
		float R = Base(color.RGBA[0]);
		float G = Base(color.RGBA[1]);
		float B = Base(color.RGBA[2]);

		return max(R, max(G, B));
	}

	float Hue() const
	{
		return Hue(*this);
	}
	float Saturation() const
	{
		return Saturation(*this);
	}
	float Brightness() const
	{
		return Brightness(*this);
	}
};
