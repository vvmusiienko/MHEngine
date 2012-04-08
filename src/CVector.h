#ifndef CVECTOR_H_
#define CVECTOR_H_

#include <math.h>

#define scalar_t	float // the scalar data type

class CVector
{
public:
	scalar_t x; // x, y, z vector components
	scalar_t y;
	scalar_t z;
public:
	// constructors
	CVector(scalar_t a = 0, scalar_t b = 0, scalar_t c = 0)
	{
		x = a; y = b; z = c;
	}
	
	
	CVector(const CVector &vec)
	{
		x = vec.x; y = vec.y; z = vec.z;
	}
	
	
	// Vector Assignment
	const CVector &operator=(const CVector &vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		return *this;
	}
	
	
	// Vector Equality
	const bool operator==(const CVector &vec) const
	{
		return ((x == vec.x) && (y == vec.y) && (z == vec.z));
	}
	
	
	// Vector Inequality
	const bool operator!=(const CVector &vec) const
	{
		return !(*this == vec);
	}
	
	
	// Vector Addition
	const CVector operator+(const CVector &vec) const
	{
		return CVector(x + vec.x, y + vec.y, z + vec.z);
	}
	
	
	// Vector Add
	const CVector operator+() const
	{
		return CVector(*this);
	}
	
	
	// Vector Increment
	const CVector& operator+=(const CVector& vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}
	
	
	// Vector Subtraction
	const CVector operator-(const CVector& vec) const
	{
		return CVector(x - vec.x, y - vec.y, z - vec.z);
	}
	
	
	// Vector Negation
	const CVector operator-() const
	{
		return CVector(-x, -y, -z);
	}
	
	
	// Vector Decrement
	const CVector &operator-=(const CVector& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	}
	
	
	// Scalar Multiply
	const CVector &operator*=(const scalar_t &s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	
	
	// Scalar Division
	const CVector &operator/=(const scalar_t &s)
	{
		const float recip = 1/s; // for speed
		x *= recip;
		y *= recip;
		z *= recip;
		return *this;
	}
	
	
	// Post Multiply by Scalar
	const CVector operator*(const scalar_t &s) const
	{
		return CVector(x*s, y*s, z*s);
	}
	
	// Post Multiply by Vector
	const CVector operator*(const CVector &v) const
	{
		return CVector(x*v.x, y*v.y, z*v.z);
	}

	
	// Pre Multiply by Scalar
	friend inline const CVector operator*(const scalar_t &s, const CVector &vec)
	{
		return vec*s;
	}
	
	
	// Divide by Scalar
	const CVector operator/(scalar_t s) const
	{
		s = 1/s;
		return CVector(s*x, s*y, s*z);
	}
	
	
	// Cross Product of This Vector and vec
	const CVector CrossProduct(const CVector &vec) const
	{
		return CVector(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
	}
	
	
	// Cross Product (Thanks to Bas Kuenen for symbol idea!)
	const CVector operator^(const CVector &vec) const
	{
		return CVector(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
	}
	
	
	// Dot Product
	const scalar_t DotProduct(const CVector &vec) const
	{
		return x*vec.x + y*vec.x + z*vec.z;
	}
	
	
	// Dot Product
	const scalar_t operator%(const CVector &vec) const
	{
		return x*vec.x + y*vec.x + z*vec.z;
	}
	
	
	// Length of Vector
	const scalar_t Length() const
	{
		return (scalar_t)sqrt((double)(x*x + y*y + z*z));
	}
	
	
	// Return the Unit Vector of this Vector
	const CVector UnitVector() const
	{
		return (*this) / Length();
	}
	
	
	// Normalize this Vector
	void Normalize()
	{
		(*this) /= Length();
	}
	
	
	// Vector Length Operator
	const scalar_t operator!() const
	{
		return sqrtf(x*x + y*y + z*z);
	}
	
	
	// Return this Vector with the Specified length
	const CVector operator | (const scalar_t length) const
	{
		return *this * (length / !(*this));
	}
	
	
	// Set Length of Vector Equal to length
	const CVector& operator |= (const float length)
	{
		return *this = *this | length;
	}
	
	
	// Return Angle Between this Vector and a Normal Vector
	const float inline Angle(const CVector& normal) const
	{
		return acosf(*this % normal); // return arccos(this vector . normal)
	}
	
	
	// Reflect this Vector about a Normal Vector
	const CVector inline Reflection(const CVector& normal) const
	{
		const CVector vec(*this | 1); // get unit vector
		return (vec - normal * 2.0 * (vec % normal)) * !*this;
	}

}; // end CVector

#endif /*VECTOR_H_*/
