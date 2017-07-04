#pragma once

#include <algorithm>

const float MATRIX_INVERSE_EPSILON = 1e-14f;
const float MATRIX_EPSILON = 1e-5f;

#define VML_PI 3.14159265358979323846f

#define RADIANS VML_PI / 180.f
#define DEGREES 180.f / VML_PI

#define FLOAT_EPSILON	0.00001f
#define	FLOAT_EQUAL( a, b )		( abs((a) - (b) ) < FLOAT_EPSILON )
#define	FLOAT_ZERO(a)			( abs((a)) < FLOAT_EPSILON )
#define	VECTOR_ZERO(v)			( FLOAT_ZERO((v).x) && FLOAT_ZERO((v).y) && FLOAT_ZERO((v).z) )


#define VML_DEPTH_ZERO_TO_ONE			0x00000001
#define VML_DEPTH_NEGATIVE_ONE_TO_ONE	0x00000002

#ifdef VML_USE_DEPTH_ZERO_TO_ONE
#define VML_DEPTH_CLIP_SPACE VML_DEPTH_ZERO_TO_ONE
#else
#define VML_DEPTH_CLIP_SPACE VML_DEPTH_NEGATIVE_ONE_TO_ONE
#endif

#define VML_LEFT_HAND					0x00000001
#define VML_RIGHT_HAND					0x00000002

#ifdef VML_USE_LEFT_HAND
#define VML_COORDINATE_SYSTEM VML_LEFT_HAND
#else
#define VML_COORDINATE_SYSTEM VML_RIGHT_HAND
#endif

//OpenGL uses Left Hand and Column Major Order
#define VML_ROW_ORDER					0x00000001
#define VML_COLUMN_ORDER				0x00000002

#ifdef VML_USE_COLUMN_MAJOR		
#define VML_MATRIX_ORDER				VML_COLUMN_ORDER
#else
#define VML_MATRIX_ORDER				VML_ROW_ORDER		//defualt row order
#endif 

//note : include once matrix4x4 in header file

#ifdef VML_USE_VULKAN
#define VML_VULKAN_CLIP					true
#else
#define VML_VULKAN_CLIP					false
#endif //VML_USE_VULKA

#ifdef VML_USE_OPENGL
#undef VML_USE_VULKAN
#endif

#define VML_ORTHO						true
#ifdef VML_USE_ORTHO
#undef VML_ORTHO
#endif // VML_USE_ORTHO

namespace vml
{
	const inline float radians(float deg)
	{
		return VML_PI / 180.f * deg;
	}

	const inline float degree(float rad)
	{
		return 180.f / VML_PI * rad;
	}

	const inline float clamp(float value, float _min = 0.0f, float _max = 1.0f)
	{
		if (value < _min)
			return _min;
		else if (value > _max)
			return _max;

		return value;
	}

	const inline float clampMinusOneToPlusOne(float value)
	{
		if (value < -1.0f)
			return -1.0f;
		else if (value > 1.0f)
			return 1.0f;

		return value;
	}

	const inline float normalize(float current, float next, float factor)
	{
		return (factor - current) / (next - current);
	}

}
