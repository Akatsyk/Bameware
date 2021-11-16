#include "../includes.h"

#include "math.h"

namespace MATH
{
#define  Assert( _exp )										((void)0)
#define  AssertAligned( ptr )								((void)0)
#define  AssertOnce( _exp )									((void)0)
#define  AssertMsg( _exp, _msg )							((void)0)
#define  AssertMsgOnce( _exp, _msg )						((void)0)
#define  AssertFunc( _exp, _f )								((void)0)
#define  AssertEquals( _exp, _expectedValue )				((void)0)
#define  AssertFloatEquals( _exp, _expectedValue, _tol )	((void)0)
#define  Verify( _exp )										(_exp)
#define  VerifyEquals( _exp, _expectedValue )           	(_exp)

	void VectorAngles(const Vector& forward, Vector& angles)
	{
		float tmp, yaw, pitch;

		if (forward[1] == 0 && forward[0] == 0)
		{
			yaw = 0;
			if (forward[2] > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else
		{
			yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
			if (yaw < 0)
				yaw += 360;

			tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
			pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
			if (pitch < 0)
				pitch += 360;
		}

		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0;
	}

	void inline SinCos(float radians, float* sine, float* cosine)
	{
		*sine = sin(radians);
		*cosine = cos(radians);
	}

	void AngleVectors(const Vector& angles, Vector* forward, Vector* right, Vector* up)
	{
		float sr, sp, sy, cr, cp, cy;
		SinCos(DEG2RAD(angles[1]), &sy, &cy);
		SinCos(DEG2RAD(angles[0]), &sp, &cp);
		SinCos(DEG2RAD(angles[2]), &sr, &cr);

		if (forward)
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}
		if (right)
		{
			right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
			right->y = (-1 * sr * sp * sy + -1 * cr * cy);
			right->z = -1 * sr * cp;
		}
		if (up)
		{
			up->x = (cr * sp * cy + -sr * -sy);
			up->y = (cr * sp * sy + -sr * cy);
			up->z = cr * cp;
		}
	}

	inline void CrossProduct(const Vector& a, const Vector& b, Vector& result)
	{
		CHECK_VALID(a);
		CHECK_VALID(b);
		Assert(&a != &result);
		Assert(&b != &result);
		result.x = a.y * b.z - a.z * b.y;
		result.y = a.z * b.x - a.x * b.z;
		result.z = a.x * b.y - a.y * b.x;
	}

	void VectorVectors(const Vector& forward, Vector& right, Vector& up)
	{
		Vector tmp;

		if (fabs(forward[0]) < 1e-6 && fabs(forward[1]) < 1e-6)
		{
			// pitch 90 degrees up/down from identity
			right[0] = 0;
			right[1] = -1;
			right[2] = 0;
			up[0] = -forward[2];
			up[1] = 0;
			up[2] = 0;
		}
		else
		{
			tmp[0] = 0; tmp[1] = 0; tmp[2] = 1.0;
			CrossProduct(forward, tmp, right);
			right.Normalize();
			CrossProduct(right, forward, up);
			up.Normalize();
		}
	}

	void AngleMatrix(const Vector& angles, matrix3x4_t& matrix)
	{
		float sr, sp, sy, cr, cp, cy;

		SinCos(DEG2RAD(angles.y), &sy, &cy);
		SinCos(DEG2RAD(angles.x), &sp, &cp);

		// matrix = (YAW * PITCH) * ROLL
		matrix[0][0] = cp * cy;
		matrix[1][0] = cp * sy;
		matrix[2][0] = -sp;

		// NOTE: Do not optimize this to reduce multiplies! optimizer bug will screw this up.
		matrix[0][1] = sr * sp * cy + cr * -sy;
		matrix[1][1] = sr * sp * sy + cr * cy;
		matrix[2][1] = sr * cp;
		matrix[0][2] = (cr * sp * cy + -sr * -sy);
		matrix[1][2] = (cr * sp * sy + -sr * cy);
		matrix[2][2] = cr * cp;

		matrix[0][3] = 0.0f;
		matrix[1][3] = 0.0f;
		matrix[2][3] = 0.0f;
	}

	void MatrixCopy(const matrix3x4_t& in, matrix3x4_t& out)
	{
		//Assert(s_bMathlibInitialized);
		memcpy(out.Base(), in.Base(), sizeof(float) * 3 * 4);
	}

	void ConcatTransforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out)
	{
		//Assert(s_bMathlibInitialized);
		if (&in1 == &out)
		{
			matrix3x4_t in1b;
			MatrixCopy(in1, in1b);
			ConcatTransforms(in1b, in2, out);
			return;
		}
		if (&in2 == &out)
		{
			matrix3x4_t in2b;
			MatrixCopy(in2, in2b);
			ConcatTransforms(in1, in2b, out);
			return;
		}

		out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
			in1[0][2] * in2[2][0];
		out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
			in1[0][2] * in2[2][1];
		out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
			in1[0][2] * in2[2][2];
		out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
			in1[0][2] * in2[2][3] + in1[0][3];
		out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
			in1[1][2] * in2[2][0];
		out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
			in1[1][2] * in2[2][1];
		out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
			in1[1][2] * in2[2][2];
		out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
			in1[1][2] * in2[2][3] + in1[1][3];
		out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
			in1[2][2] * in2[2][0];
		out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
			in1[2][2] * in2[2][1];
		out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
			in1[2][2] * in2[2][2];
		out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
			in1[2][2] * in2[2][3] + in1[2][3];
	}

	__forceinline float DotProduct(const float* a, const float* b)
	{
		return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	}

	__forceinline float DotProduct(const Vector& a, const Vector& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	void VectorTransform(const float* in1, const matrix3x4_t& in2, float* out)
	{
		out[0] = DotProduct(in1, in2[0]) + in2[0][3];
		out[1] = DotProduct(in1, in2[1]) + in2[1][3];
		out[2] = DotProduct(in1, in2[2]) + in2[2][3];
	}

	void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
	{
		VectorTransform(&in1.x, in2, &out.x);
	}

	float CalcAngle2D(const Vector2D& src, const Vector2D& dst)
	{
		float angle;
		VectorAngle2D(dst - src, angle);
		return angle;
	}

	void VectorAngle2D(const Vector2D& direction, float& angle)
	{
		angle = RAD2DEG(std::atan2(direction.y, direction.x)) + 90.f;
	}

	void AngleVectors2D(float angle, Vector2D& forward)
	{
		angle = DEG2RAD(angle);
		Vector2D slope(sin(angle), -cos(angle));
		forward = slope;
	}

	void NormalizeAngleTest(float& angle) {
		float rot;

		// bad number.
		if (!std::isfinite(angle)) {
			angle = 0.f;
			return;
		}

		// no need to normalize this angle.
		if (angle >= -180.f && angle <= 180.f)
			return;

		// get amount of rotations needed.
		rot = std::round(std::abs(angle / 360.f));

		// normalize.
		angle = (angle < 0.f) ? angle + (360.f * rot) : angle - (360.f * rot);
	}

	/*
	void draw_angle_line2d(const Vector2D center, const float yaw, const float
		line_distance_from_center, const float line_length, const CColor line_color)
	{
		const auto start_position = get_rotated_screen_position(center, yaw, line_distance_from_center);
		const auto end_position = get_rotated_screen_position(center, yaw, line_distance_from_center + line_length);

		IRender->DrawLine(start_position.x, start_position.y, end_position.x, end_position.y, line_color);
	}
	Vector2D get_rotated_screen_position(Vector2D center, float yaw, float distance)
	{
		Vector2D slope = { sin(DEG2RAD(yaw)), -cos(DEG2RAD(yaw)) }; // x = run, y = rise
		Vector2D direction = slope;
		return center + (slope * distance);
	}
	*/
}
