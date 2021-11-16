#pragma once
#include <math.h>

#define CHECK_VALID( _v ) 0
#define M_PI 3.14159265358979323846

class Vector
{
public:
	float x, y, z;

	inline void Init(float ix, float iy, float iz)
	{
		x = ix;
		y = iy;
		z = iz;
	}

	Vector()
	{
		x = 0;
		y = 0;
		z = 0;
	};

	Vector(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	};

	FORCEINLINE void  Set(float x = 0.0f, float y = 0.0f, float z = 0.0f)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	FORCEINLINE Vector(const float* v)
	{
		this->Set(v[0], v[1], v[2]);
	}

	float operator[](int i) const
	{
		if (i == 0)
			return x;
		if (i == 1)
			return y;
		return z;
	};

	float& operator[](int i)
	{
		if (i == 0)
			return x;
		if (i == 1)
			return y;
		return z;
	};

	bool operator==(const Vector& v) const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	bool operator!=(const Vector& v) const
	{
		return x != v.x || y != v.y || z != v.z;
	}

	inline Vector operator-(const Vector& v) const
	{
		return Vector(x - v.x, y - v.y, z - v.z);
	}

	inline Vector operator+(const Vector& v) const
	{
		return Vector(x + v.x, y + v.y, z + v.z);
	}

	inline Vector operator*(const Vector& v) const
	{
		return Vector(x * v.x, y * v.y, z * v.z);
	}

	inline Vector operator/(const Vector& v) const
	{
		return Vector(x / v.x, y / v.y, z / v.z);
	}

	inline Vector operator*(const int n) const
	{
		return Vector(x * n, y * n, z * n);
	}

	inline Vector operator*(const float n) const
	{
		return Vector(x * n, y * n, z * n);
	}

	inline Vector operator/(const int n) const
	{
		return Vector(x / n, y / n, z / n);
	}

	inline Vector operator/(const float n) const
	{
		return Vector(x / n, y / n, z / n);
	}

	inline Vector operator-()
	{
		return Vector(-x, -y, -z);
	}

	inline Vector& operator+=(const Vector& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	inline Vector& operator-=(const Vector& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	inline Vector& operator*=(const Vector& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	inline Vector& operator/=(const Vector& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	inline Vector& operator /= (float fl)
	{
		this->x /= fl;
		this->y /= fl;
		this->z /= fl;

		return (*this);
	}

	inline void operator*=(const float& v)
	{
		x *= v;
		y *= v;
		z *= v;
	}

	float LengthSqr(void)
	{
		return (x * x + y * y + z * z);
	}

	float Length(void) const
	{
		return sqrt(x * x + y * y + z * z);
	}

	FORCEINLINE Vector Cross(const Vector& v)
	{
		return { this->y * v.z - this->z * v.y,
				 this->z * v.x - this->x * v.z,
				 this->x * v.y - this->y * v.x };
	}

	FORCEINLINE Vector  Normalized() const
	{
		Vector vec(*this);

		float radius = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

		// FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
		float iradius = 1.f / (radius + FLT_EPSILON);

		vec.x *= iradius;
		vec.y *= iradius;
		vec.z *= iradius;

		return vec;
	}

	inline float Length2D() const
	{
		return sqrt((x * x) + (y * y));
	}

	//T must be between 0 and 1
	Vector lerp(Vector target, float t)
	{
		return *this * (1 - t) + target * t;
	}

	void lerpme(Vector target, float t)
	{
		*this = *this * (1 - t) + target * t;
	}

	inline float Dot(const Vector& e) const
	{
		return (x * e.x) + (y * e.y) + (z * e.z);
	}

	float NormalizeInPlace()
	{
		Vector& v = *this;

		float iradius = 1.f / (this->LengthSqr() + 1.192092896e-07F); //FLT_EPSILON

		v.x *= iradius;
		v.y *= iradius;
		v.z *= iradius;
		return iradius;
	}

	inline Vector Vector::NormalizeNoClamp()
	{
		CHECK_VALID(*this);

		this->x -= floorf(this->x / 360.0f + 0.5f) * 360.0f;

		this->y -= floorf(this->y / 360.0f + 0.5f) * 360.0f;

		this->z -= floorf(this->z / 360.0f + 0.5f) * 360.0f;

		return *this;
	}

	FORCEINLINE void  ToVectors(Vector& right, Vector& up) { // VectorVectors
		Vector tmp;
		if (x == 0.f && y == 0.f) {
			// pitch 90 degrees up/down from identity
			right[0] = 0.f;
			right[1] = -1.f;
			right[2] = 0.f;
			up[0] = -z;
			up[1] = 0.f;
			up[2] = 0.f;
		}
		else {
			tmp[0] = 0.f;
			tmp[1] = 0.f;
			tmp[2] = 1.0f;
			right = Cross(tmp);
			up = right.Cross(*this);

			right.Normalize();
			up.Normalize();
		}
	}

	FORCEINLINE float  Normalize()
	{
		float len = Length();

		(*this) /= (len + std::numeric_limits< float >::epsilon());

		return len;
	}
};
