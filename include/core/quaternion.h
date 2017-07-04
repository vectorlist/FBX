#pragma once

#include <vml.h>
#include <vec3f.h>
#include <matrix4x4.h>

class Quaternion
{
public:
	Quaternion();
	Quaternion(float w, float x, float y, float z);
	Quaternion(float w, const vec3f &vv);
	Quaternion(const vec3f &axis, float angle);
	Quaternion(const Matrix4x4 &m);
	~Quaternion(){};

	Quaternion			operator*(const Quaternion &other);
	vec3f				operator*(const vec3f &p) const;
	Quaternion			operator*(float f) const;
	Quaternion&			operator*=(float f);


	float				Norm() const;
	Quaternion			Conjugate() const;
	Quaternion&			Noramlize();
	Quaternion			Normalized() const;
	static Quaternion	FromAxisAngle(const vec3f &axis, float angle);
	static Quaternion	RotationYawPitchRoll(float yaw, float pitch, float rool);
	Matrix4x4			GetMatrix() const;

	static Quaternion	Euler(float x, float y, float z);
	static vec3f		ToEulerAngle(const Quaternion &q);
	static void			Interpolate(Quaternion& result, const Quaternion& start, const Quaternion& end, float factor);
	vec3f v;
	float w;
};

inline Quaternion::Quaternion() : v(0.0f), w(1.0f) {}

inline Quaternion::Quaternion(float w, float x, float y, float z) : v(x, y, z), w(w) {}

inline Quaternion::Quaternion(float w, const vec3f &v) : v(v), w(w) {}

inline Quaternion::Quaternion(const vec3f &axis, float angle)
{
	//must be deggree to radians
	float t2 = angle / 360.f * M_PI * 2.f;
	float t = t2 / 2.0f;
	vec3f tmpAxis = axis.normalized();

	float sinT = std::sin(t);

	v = tmpAxis * sinT;
	w = std::cos(t);
}

inline Quaternion::Quaternion(const Matrix4x4 &m)
{
	float trace = m[0][0] + m[1][1] + m[2][2];
	if (trace > 0.0f)
	{
		float s = sqrtf(trace + 1.0f);
		w = s / 2.0f;
		s = 0.5f / s;
		v.x = (m[2][1] - m[1][2]) * s;
		v.y = (m[0][2] - m[2][0]) * s;
		v.z = (m[1][0] - m[0][1]) * s;
	}
	else
	{
		const int nxt[3] = { 1, 2, 0 };
		float q[3];
		int i = 0;
		if (m[1][1] > m[0][0]) i = 1;
		if (m[2][2] > m[i][i]) i = 2;
		int j = nxt[i];
		int k = nxt[j];
		float s = sqrtf((m[i][i] - (m[j][j] + m[k][k])) + 1.0);
		q[i] = s * 0.5f;
		if (s != 0.f) s = 0.5f / s;
		w = (m[k][j] - m[j][k]) * s;
		q[j] = (m[j][i] + m[i][j]) * s;
		q[k] = (m[k][i] + m[i][k]) * s;
		v.x = q[0];
		v.y = q[1];
		v.z = q[2];
	}
}

inline Quaternion Quaternion::operator*(const Quaternion &other)
{
	//http://math.ucr.edu/~huerta/introquaternions.pdf
	float scalar = (w * other.w) - vec3f::dot(v, other.v);
	vec3f vv =
		w *			other.v +
		other.w			* v +
		vec3f::cross(v, other.v);	

	return Quaternion(scalar, vv);
}

inline vec3f Quaternion::operator*(const vec3f &p) const
{
	vec3f uv = vec3f::cross(v, p);
	vec3f uuv = vec3f::cross(v, uv);
	uv *= (2.f * w);
	uuv *= 2.f;
	return p + uv + uuv;
}

inline Quaternion& Quaternion::operator*=(float f)
{
	v *= f;
	w *= f;
	return *this;
}

inline Quaternion Quaternion::operator*(float f) const
{
	Quaternion Q = *this;
	Q.v *= f;
	Q.w *= f;
	return Q;
}


inline Quaternion Quaternion::Conjugate() const
{
	//Qr =  ( Qr.scalar, -Qr.vector )
	return Quaternion(w, -v);
}

inline float Quaternion::Norm() const
{
	//The square root of the product of a Quaternion with 
	//its conjugate is called its norm and is denoted ||q|| 
	//sqrt w2 + x2 + y2  + z2
	return (w * w) + v.length2();
}

inline Quaternion& Quaternion::Noramlize()
{
	const float mag = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + w * w);
	if (mag)
	{
		const float inv = 1.0f / mag;
		v *= inv;
		w *= inv;
	}
	return *this;
}

inline Quaternion Quaternion::Normalized() const
{
	Quaternion Q = *this;
	float len = sqrt(this->Norm());
	float inv_len = 1.f / len;

	Q.v *= inv_len;
	Q.w *= inv_len;
	return Q;
}

inline Quaternion Quaternion::FromAxisAngle(const vec3f &axis, float angle)
{
	float half = angle * 0.5f * M_PI / 180.f;
	float sinT = sin(half);
	Quaternion Q;
	Q.v = axis.normalized() * sinT;
	Q.w = cos(half);
	return Q.Normalized();
}

inline Quaternion Quaternion::RotationYawPitchRoll(float yaw, float pitch, float roll)
{
	//http://codesuppository.blogspot.kr/2008/06/d3dxquaternionrotationyawpitchroll.html
	float sinY, cosY, sinP, cosP, sinR, cosR;

	sinY = sinf(0.5f * yaw);
	cosY = cosf(0.5f * yaw);

	sinP = sinf(0.5f * pitch);
	cosP = cosf(0.5f * pitch);

	sinR = sinf(0.5f * roll);
	cosR = cosf(0.5f * roll);

	Quaternion Q;
	Q.v.x = sinY * cosP * cosR - cosY * sinP * sinR;
	Q.v.y = cosY * cosP * sinR - sinY * sinP * cosR;
	Q.v.z = cosY * cosP * cosR + sinY * sinP * sinR;
	Q.w = cosY * sinP * cosR + sinY * cosP * sinR;
	return Q;
}

inline Matrix4x4 Quaternion::GetMatrix() const
{
	//http://www.j3d.org/matrix_faq/matrfaq_latest.html#Q56
	float x2 = 2.0f * v.x;
	float y2 = 2.0f * v.y;
	float z2 = 2.0f * v.z;

	float xx2 = x2 * v.x;
	float xy2 = x2 * v.y;
	float xz2 = x2 * v.z;
	float xw2 = x2 * w;

	float yy2 = y2 * v.y;
	float yz2 = y2 * v.z;
	float yw2 = y2 * w;

	float zz2 = z2 * v.z;
	float zw2 = z2 * w;

	Matrix4x4 M(
		1.f - yy2 - zz2, xy2 - zw2, xz2 + yw2, 0.f,			//0 row
		xy2 + zw2, 1.f - xx2 - zz2, yz2 - xw2, 0.f,			//1 row 
		xz2 - yw2, yz2 + xw2, 1.f - xx2 - yy2, 0.f,         //2 row
		0.f, 0.f, 0.f, 1.0f);

	return M;
}

inline Quaternion Quaternion::Euler(float x, float y, float z)
{
	float t0 = sin(z * 0.5f);
	float t1 = cos(z * 0.5f);
	float t2 = sin(x * 0.5f);
	float t3 = cos(x * 0.5f);
	float t4 = sin(y * 0.5f);
	float t5 = cos(y * 0.5f);
	Quaternion Q;
	Q.v.x = t5 * t2 * t1 + t4 * t3 * t0;
	Q.v.y = t4 * t3 * t1 - t5 * t2 * t0;
	Q.v.z = t5 * t3 * t0 - t4 * t2 * t1;
	Q.w = t5 * t3 * t1 + t4 * t2 * t0;
	return Q;
}

inline vec3f Quaternion::ToEulerAngle(const Quaternion & q)
{
	vec3f v;

	/*v.x = atan2(2.f * q.x*q.w - 2 * q.y*q.z, 1 - 2 * q.x*q.x - 2 * q.z*q.z) * Mathf::RadToDeg();
	v.y = atan2(2 * q.y*q.w - 2 * q.x*q.z, 1 - 2 * q.y*q.y - 2 * q.z*q.z) * Mathf::RadToDeg();
	v.z = asin(2 * q.x*q.y + 2 * q.z*q.w) * RadToDeg();*/

	return v;
}

inline void Quaternion::Interpolate(Quaternion& result, const Quaternion& start, const Quaternion& end, float factor)
{
	// calc cosine theta
	float cosom = start.v.x * end.v.x + start.v.y * end.v.y
		+ start.v.z * end.v.z + start.w * end.w;

	// adjust signs (if necessary)
	Quaternion newEnd = end;
	if (cosom < 0.0f)
	{
		cosom = -cosom;
		newEnd.v.x = -newEnd.v.x;
		newEnd.v.y = -newEnd.v.y;
		newEnd.v.z = -newEnd.v.z;
		newEnd.w = -newEnd.w;
	}

	// Calculate coefficients
	float sclp, sclq;
	if ((1.0f - cosom) > 0.0001f) // 0.0001 -> some epsillon
	{
		// Standard case (slerp)
		float omega, sinom;
		omega = std::acos(cosom);
		sinom = std::sin(omega);
		sclp = std::sin((1.0f - factor) * omega) / sinom;
		sclq = std::sin(factor * omega) / sinom;
	}
	else
	{
		// Very close, do linear interp (because it's faster)
		sclp = 1.0f - factor;
		sclq = factor;
	}

	result.v.x = sclp * start.v.x + sclq * newEnd.v.x;
	result.v.y = sclp * start.v.y + sclq * newEnd.v.y;
	result.v.z = sclp * start.v.z + sclq * newEnd.v.z;
	result.w = sclp * start.w + sclq * newEnd.w;
}