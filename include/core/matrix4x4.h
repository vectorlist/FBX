
#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include <vml.h>
#include <assert.h>
#include <vec3f.h>
#include <vec4f.h>
#include <iostream>
#include <iomanip>
#include <memory>

enum class AXIS : unsigned int{
	X = 0, Y, Z
};

//row major rule 0 1 2 3 4

class Matrix4x4
{
public:
	Matrix4x4();
	Matrix4x4(const Matrix4x4 &other);
	Matrix4x4(float other[16]);
	Matrix4x4(double other[16]);
	Matrix4x4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);
	~Matrix4x4(){};

	//static const Matrix4x4 zero;

	/*OPERATOR*/

	Matrix4x4 operator*(const Matrix4x4 &other) const;
	Matrix4x4& operator*=(const Matrix4x4 &other);
	vec3f operator*(const vec3f &v) const;
	Matrix4x4 operator*(float f) const;
	Matrix4x4& operator*=(float f);

	const float* operator[](int i) const;
	float* operator[](int i);

	void setRow(int index, const vec4f &v);
	void setColumn(int index, const vec4f &v);
	/*FOR GENERIC NORMAL*/
	vec3f normal(const vec3f &n);

	/*FUNCTION*/
	void setToIdentity();
	Matrix4x4& transpose();
	Matrix4x4 transposed() const;

	/*TRANSFORM*/
	void translate(vec3f t);
	void rotate(AXIS axis, float angle);
	void scale(vec3f s);
	void scale(float s);


	float m[4][4];

	Matrix4x4 rotatedX(float angle) const;
	Matrix4x4 rotatedY(float angle) const;
	Matrix4x4 rotatedZ(float angle) const;

	Matrix4x4 inverted() const;
	static Matrix4x4 vulkandClip();

	inline float* data() { return *m; }
	inline const float* constData() const { return *m; }

	friend std::ostream& operator<<(std::ostream &o, const Matrix4x4 &mat);
};

inline Matrix4x4::Matrix4x4()
{
	setToIdentity();
}

inline Matrix4x4::Matrix4x4(const Matrix4x4 &other)
{
	std::memcpy(m, other.m, sizeof(m));
}

inline Matrix4x4::Matrix4x4(float other[16])
{
	std::memcpy(m, other, sizeof(m));
}

inline Matrix4x4::Matrix4x4(double other[16])
{
	m[0][0] = static_cast<float>(other[0]);
	m[0][1] = static_cast<float>(other[1]);
	m[0][2] = static_cast<float>(other[2]);
	m[0][3] = static_cast<float>(other[3]);

	m[1][0] = static_cast<float>(other[4]);
	m[1][1] = static_cast<float>(other[5]);
	m[1][2] = static_cast<float>(other[6]);
	m[1][3] = static_cast<float>(other[7]);

	m[2][0] = static_cast<float>(other[8]);
	m[2][1] = static_cast<float>(other[9]);
	m[2][2] = static_cast<float>(other[10]);
	m[2][3] = static_cast<float>(other[11]);

	m[3][0] = static_cast<float>(other[12]);
	m[3][1] = static_cast<float>(other[13]);
	m[3][2] = static_cast<float>(other[14]);
	m[3][3] = static_cast<float>(other[15]);
}

inline Matrix4x4::Matrix4x4(float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[0][3] = m03;

	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[1][3] = m13;

	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
	m[2][3] = m23;

	m[3][0] = m30;
	m[3][1] = m31;
	m[3][2] = m32;
	m[3][3] = m33;
}

inline Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &other) const
{
	Matrix4x4 M;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			M.m[i][j] =
				m[i][0] * other.m[0][j] +
				m[i][1] * other.m[1][j] +
				m[i][2] * other.m[2][j] +
				m[i][3] * other.m[3][j];
		}
	}
	return M;
}

inline Matrix4x4& Matrix4x4::operator*=(const Matrix4x4 &other)
{
	*this = operator*(other);
	return *this;
}


inline vec3f Matrix4x4::operator*(const vec3f &v) const
{
	float x, y, z;
	x = v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + m[0][3];
	y = v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + m[1][3];
	z = v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + m[2][3];

	return vec3f(x, y, z);
}

inline Matrix4x4 Matrix4x4::operator*(float f) const
{
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result[i][j] = f * m[i][j];
		}
	}
	return result;
}

inline Matrix4x4& Matrix4x4::operator*=(float f)
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			m[i][j] *= f;
		}
	}
	return *this;
}

inline const float* Matrix4x4::operator[](int i) const {
	assert(i >= 0 && i < 4);
	return m[i];
}

inline float* Matrix4x4::operator[](int i) {
	assert(i >= 0 && i < 4);
	return m[i];
}

inline void Matrix4x4::setRow(int index, const vec4f & v)
{
	assert(index >= 0 && index < 4);
	m[index][0] = v.x;
	m[index][1] = v.y;
	m[index][2] = v.z;
	m[index][3] = v.w;
}

inline void Matrix4x4::setColumn(int index, const vec4f & v)
{
	assert(index >= 0 && index < 4);
	m[0][index] = v.x;
	m[1][index] = v.y;
	m[2][index] = v.z;
	m[3][index] = v.w;
}

inline void Matrix4x4::setToIdentity()
{
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = 0.0f;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;

}

inline Matrix4x4& Matrix4x4::transpose()
{
	for (int row = 0; row < 4; ++row)
		for (int col = row + 1; col < 4; ++col)
		{
			float tmp = m[row][col];
			m[row][col] = m[col][row];
			m[col][row] = tmp;
		}

	return *this;
}

inline Matrix4x4 Matrix4x4::transposed() const
{
	//from row major rule data to colum major rule
	Matrix4x4 M;
	for (int row = 0; row < 4; ++row)
		for (int col = 0; col < 4; ++col)
			M.m[row][col] = m[col][row];				//swap datas
	return M;
}

inline Matrix4x4 Matrix4x4::rotatedX(float angle) const
{
	float t = radians(angle);
	float sinT = sin(t);
	float cosT = cos(t);

	Matrix4x4 M = Matrix4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cosT, -sinT, 0.0f,
		0.0f, sinT, cosT, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return M;
}

inline Matrix4x4 Matrix4x4::rotatedY(float angle) const
{
	float t = radians(angle);
	float sinT = sin(t);
	float cosT = cos(t);

	Matrix4x4 M = Matrix4x4(
		cosT, 0.0f, sinT, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sinT, 0.0f, cosT, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return M;
}

inline Matrix4x4 Matrix4x4::rotatedZ(float angle) const
{
	float t = radians(angle);
	float sinT = sin(t);
	float cosT = cos(t);

	Matrix4x4 M = Matrix4x4(
		cosT, -sinT, 0.0f, 0.0f,
		sinT, cosT, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	return M;
}

inline void Matrix4x4::translate(vec3f t)
{
	Matrix4x4 M;
	M.m[0][3] = t.x;
	M.m[1][3] = t.y;
	M.m[2][3] = t.z;


	*this = *this * M;
}

inline void Matrix4x4::rotate(AXIS axis, float angle)
{
	Matrix4x4 rotM;
	switch (axis)
	{
	case AXIS::X:
		rotM[0][0] = 1.f;
		rotM[1][1] = std::cos(angle * RADIANS);
		rotM[1][2] = -sin(angle * RADIANS);
		rotM[2][1] = sin(angle * RADIANS);
		rotM[2][2] = cos(angle * RADIANS);
		rotM[3][3] = 1.f;
		break;
	case AXIS::Y:
		rotM[0][0] = cos(angle * RADIANS);
		rotM[0][2] = sin(angle * RADIANS);
		rotM[1][1] = 1.f;
		rotM[2][0] = -sin(angle * RADIANS);
		rotM[2][2] = cos(angle * RADIANS);
		rotM[3][3] = 1.f;
		break;
	case AXIS::Z:
		rotM[0][0] = cos(angle * RADIANS);
		rotM[0][1] = -sin(angle * RADIANS);
		rotM[1][0] = sin(angle * RADIANS);
		rotM[1][1] = cos(angle * RADIANS);
		rotM[2][2] = 1.f;
		break;
	}

	*this = *this * rotM;
}

inline void Matrix4x4::scale(vec3f s)
{
	Matrix4x4 M;
	M.m[0][0] *= s.x;
	M.m[1][1] *= s.y;
	M.m[2][2] *= s.z;

	*this = *this * M;
}

inline void Matrix4x4::scale(float s)
{
	Matrix4x4 M;
	M.m[0][0] *= s;
	M.m[1][1] *= s;
	M.m[2][2] *= s;

	*this = *this * M;
}

inline Matrix4x4 Matrix4x4::inverted() const
{
	Matrix4x4 inv;
	float m00 = m[0][0];
	float m01 = m[0][1];
	float m02 = m[0][2];
	float m03 = m[0][3];
	float m10 = m[1][0];
	float m11 = m[1][1];
	float m12 = m[1][2];
	float m13 = m[1][3];
	float m20 = m[2][0];
	float m21 = m[2][1];
	float m22 = m[2][2];
	float m23 = m[2][3];
	float m30 = m[3][0];
	float m31 = m[3][1];
	float m32 = m[3][2];
	float m33 = m[3][3];

	float R23C23 = m22 * m33 - m23 * m32;
	float R23C13 = m21 * m33 - m23 * m31;
	float R23C12 = m21 * m32 - m22 * m31;
	float R23C03 = m20 * m33 - m23 * m30;
	float R23C02 = m20 * m32 - m22 * m30;
	float R23C01 = m20 * m31 - m21 * m30;

	inv[0][0] = +(m11 * R23C23 - m12 * R23C13 + m13 * R23C12);
	inv[1][0] = -(m10 * R23C23 - m12 * R23C03 + m13 * R23C02);
	inv[2][0] = +(m10 * R23C13 - m11 * R23C03 + m13 * R23C01);
	inv[3][0] = -(m10 * R23C12 - m11 * R23C02 + m12 * R23C01);

	float det = m00 * inv[0][0] + m01 * inv[1][0] +
		m02 * inv[2][0] + m03 * inv[3][0];
	if (fabs(det) < MATRIX_EPSILON) {
		//return;
	}
	float invDet = 1.0f / det;

	inv[0][1] = -(m01 * R23C23 - m02 * R23C13 + m03 * R23C12);
	inv[1][1] = +(m00 * R23C23 - m02 * R23C03 + m03 * R23C02);
	inv[2][1] = -(m00 * R23C13 - m01 * R23C03 + m03 * R23C01);
	inv[3][1] = +(m00 * R23C12 - m01 * R23C02 + m02 * R23C01);

	float R13C23 = m12 * m33 - m13 * m32;
	float R13C13 = m11 * m33 - m13 * m31;
	float R13C12 = m11 * m32 - m12 * m31;
	float R13C03 = m10 * m33 - m13 * m30;
	float R13C02 = m10 * m32 - m12 * m30;
	float R13C01 = m10 * m31 - m11 * m30;

	inv[0][2] = +(m01 * R13C23 - m02 * R13C13 + m03 * R13C12);
	inv[1][2] = -(m00 * R13C23 - m02 * R13C03 + m03 * R13C02);
	inv[2][2] = +(m00 * R13C13 - m01 * R13C03 + m03 * R13C01);
	inv[3][2] = -(m00 * R13C12 - m01 * R13C02 + m02 * R13C01);

	float R12C23 = m12 * m23 - m13 * m22;
	float R12C13 = m11 * m23 - m13 * m21;
	float R12C12 = m11 * m22 - m12 * m21;
	float R12C03 = m10 * m23 - m13 * m20;
	float R12C02 = m10 * m22 - m12 * m20;
	float R12C01 = m10 * m21 - m11 * m20;

	inv[0][3] = -(m01 * R12C23 - m02 * R12C13 + m03 * R12C12);
	inv[1][3] = +(m00 * R12C23 - m02 * R12C03 + m03 * R12C02);
	inv[2][3] = -(m00 * R12C13 - m01 * R12C03 + m03 * R12C01);
	inv[3][3] = +(m00 * R12C12 - m01 * R12C02 + m02 * R12C01);

	inv *= invDet;
	return inv;
}

inline Matrix4x4 Matrix4x4::vulkandClip()
{
	Matrix4x4 clip = {
		1.0f,  0.0f,  0.0f,  0.0f,
		0.0f, -1.0f,  0.0f,  0.0f,
		0.0f,  0.0f,  0.5f,  0.5f,
		0.0f,  0.0f,  0.0f,  1.0f
	};
	return clip;
}



inline vec3f Matrix4x4::normal(const vec3f &n)
{
	float x, y, z;
	x = n.x * m[0][0] + n.y * m[0][1] + n.z * m[0][2];
	y = n.x * m[1][0] + n.y * m[1][1] + n.z * m[1][2];
	z = n.x * m[2][0] + n.y * m[2][1] + n.z * m[2][2];

	return vec3f(x, y, z).normalized();
}



inline std::ostream& operator<<(std::ostream &o, const Matrix4x4 &mat)
{
#define FM std::setw(12)
	//row major order
	o << "Matrix4x4[" << "\n" <<
		FM << mat[0][0] << FM << mat[0][1] << FM << mat[0][2] << FM << mat[0][3] << "\n" <<
		FM << mat[1][0] << FM << mat[1][1] << FM << mat[1][2] << FM << mat[1][3] << "\n" <<
		FM << mat[2][0] << FM << mat[2][1] << FM << mat[2][2] << FM << mat[2][3] << "\n" <<
		FM << mat[3][0] << FM << mat[3][1] << FM << mat[3][2] << FM << mat[3][3] << " ]" << "\n";
#undef FM
	return o;
}


namespace vml
{
	inline Matrix4x4 perspectiveRH(float fovY, float aspect, float znear, float zfar)
	{
		float const tanHalfFov = tan(RADIANS * fovY * 0.5f);

		Matrix4x4 M;
		M[0][0] = 1.f / (aspect* tanHalfFov);
		M[1][1] = 1.f / tanHalfFov;
		M[3][2] = -1.f;

#if VML_DEPTH_CLIP_SPACE == VML_DEPTH_ZERO_TO_ONE

		M[2][2] = zfar / (znear - zfar);
		M[2][3] = -(zfar * znear) / (zfar - znear);
#else
		M[2][2] = -(zfar + znear) / (zfar - znear);
		M[2][3] = -(2.f * zfar * znear) / (zfar - znear);
#endif
		return M;
	}

	inline Matrix4x4 perspectiveLH(float fovY, float aspect, float znear, float zfar)
	{
		float const tanHalfFov = tan(RADIANS * fovY * 0.5f);

		Matrix4x4 M;
		M[0][0] = 1.f / (aspect* tanHalfFov);
		M[1][1] = 1.f / tanHalfFov;
		M[3][2] = 1.f;

#if VML_DEPTH_CLIP_SPACE == VML_DEPTH_ZERO_TO_ONE

		M[2][2] = zfar / (znear - zfar);
		M[2][3] = -(zfar * znear) / (zfar - znear);
#else
		M[2][2] = (zfar + znear) / (zfar - znear);
		M[2][3] = -(2.f * zfar * znear) / (zfar - znear);
#endif
		return M;
	}

	inline Matrix4x4 perspective(float fovY, float aspect, float znear, float zfar)
	{
#	if VML_COORDINATE_SYSTEM == VML_LEFT_HAND
		if(VML_VULKAN_CLIP)
			return Matrix4x4::vulkandClip() * perspectiveLH(fovY, aspect, znear, zfar);
		return perspectiveLH(fovY, aspect, znear, zfar);
#	else
		if (VML_VULKAN_CLIP)
			return Matrix4x4::vulkandClip() * perspectiveRH(fovY, aspect, znear, zfar);
		return perspectiveRH(fovY, aspect, znear, zfar);	//row for ray tracing(d12)
#	endif 
		
	}

	inline Matrix4x4 lookAtRH(const vec3f &eye,const vec3f& center,const vec3f& upvector)
	{
		vec3f const forward = (center - eye).normalized();
		vec3f const right	= vec3f::cross(forward, upvector).normalized();
		vec3f const up		= vec3f::cross(right, forward);

		float x = -vec3f::dot(  right, eye);
		float y = -vec3f::dot(     up, eye);
		float z =  vec3f::dot(forward, eye);

		Matrix4x4 view = {
		       right.x,     right.y,	 right.z,		x,
			      up.x,        up.y,        up.z,		y,
			-forward.x,  -forward.y,  -forward.z,		z,
				 0.0f,      0.0f,       0.0f,    1.0f};
		return view;
	}

	inline Matrix4x4 lookAtLH(const vec3f &eye, const vec3f& center, const vec3f& upvector)
	{
		vec3f const forward = (center - eye).normalized();
		vec3f const right	= vec3f::cross(upvector, forward).normalized();
		vec3f const up		= vec3f::cross(forward, right);

		float x = -vec3f::dot(	 right, eye);
		float y = -vec3f::dot(	    up, eye);
		float z = -vec3f::dot( forward, eye);

		Matrix4x4 view = {
			   right.x,     right.y,	 right.z,		x,
			      up.x,        up.y,        up.z,		y,
			-forward.x,  -forward.y,  -forward.z,		z,
			      0.0f,        0.0f,        0.0f,     1.0f };
		return view;
	}

	inline Matrix4x4 lookAt(const vec3f &eye, const vec3f &center, const vec3f &up)
	{
#		if VML_COORDINATE_SYSTEM == VML_LEFT_HAND
		return lookAtLH(eye, center, up);
#else
		return lookAtRH(eye, center, up);
#endif
	}

	//for vulkan
	inline Matrix4x4 lookAtVK(const vec3f &eye, const vec3f &center, const vec3f &up)
	{
		vec3f forward = (eye - center).normalized();
		vec3f right = vec3f::cross(up, forward).normalized();
		vec3f upvector = vec3f::cross(forward, right).normalized();

		float x = -vec3f::dot(eye, right);
		float y = -vec3f::dot(eye, upvector);
		float z = -vec3f::dot(eye, forward);

		Matrix4x4 view = {
			right.x,upvector.x,forward.x,0.0f,
			right.y,upvector.y,forward.y,0.0f,
			right.z,upvector.z,forward.z,0.0f,
			x,y,z,1.0f
		};
		return view;
	}

	inline Matrix4x4 perspectiveVK(float fovy, float aspect, float znear, float zfar)
	{
		Matrix4x4 proj = perspective(fovy, aspect, znear, zfar);
		Matrix4x4 clip = {
			1.0f,	0.0f,	0.0f,	0.0f,
			0.0f,  -1.0f,	0.0f,	0.0f,
			0.0f,	0.0f,	0.5f,	0.5f,
			0.0f,	0.0f,	0.0f,	1.0f
		};
		return clip * proj;
	}

	//right hand for default
	inline Matrix4x4 ortho(float left, float right,float bottom, float top)
	{
		Matrix4x4 proj;
		proj[0][0] = 2.f / (right - left);
		proj[1][1] = 2.f / (top - bottom);
		proj[2][2] = -1.f;
		proj[0][3] = -(right + left) / (right - left);
		proj[1][3] = -(top + bottom) / (top - bottom);
		return proj;
	}

	inline Matrix4x4 ortho(float left, float right, float bottom, float top,
		float znear, float zfar)
	{
		Matrix4x4 proj;
		proj[0][0] = 2.f / (right - left);
		proj[1][1] = 2.f / (top - bottom);
		proj[2][2] = -2.f / (zfar - znear);
		proj[0][3] = -(right + left) / (right - left);
		proj[1][3] = -(top + bottom) / (top - bottom);
		proj[2][3] = -(zfar + znear) / (zfar - znear);
		return proj;
	}

	//opengl static scale matrix
	inline Matrix4x4 scale(const Matrix4x4 &mat, float s)
	{
		Matrix4x4 M;
#ifndef  VML_USE_VULKAN
		M[0][0] *= s;
		M[1][1] *= s;
		M[2][2] *= s;
#else
		M[0][0] *= s;
		M[1][1] *= s;
		M[2][2] *= s;
#endif
		return mat * M;
	}

	inline Matrix4x4 transform(const vec3f &pos, float rx, float ry, float rz, float scale)
	{
		Matrix4x4 model;
		model.translate(pos);
		model.rotate(AXIS::X, rx);
		model.rotate(AXIS::Y, ry);
		model.rotate(AXIS::Z, rz);
		model.scale(vec3f(scale));
		return model;
	}
}

#endif // MATRIX4X4_H
