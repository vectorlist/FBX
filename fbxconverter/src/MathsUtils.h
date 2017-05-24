#pragma once

#include <vec2f.h>
#include <vec3f.h>
#include <vec4f.h>
#include <matrix4x4.h>
#include <fbxsdk.h>


class MathsUtils
{
public:

	static void TranslateRotateScale(
		const vec3f &position, 
		const Matrix4x4 &rotation,
		const vec3f &scale,
		Matrix4x4 &result
		);

	static void Decompose();

	static void ConvertFBXVector4ToGlVec3(
		const FbxVector4 & fbxVec4,
		vec3f & glmVec3
		);

	static void ConvertFBXVector4ToGlVec4(
		const FbxVector4 &fbxVec4,
		vec4f &glmVec4
		);

	static void ConvertFBXToGLMatrix(
		const FbxAMatrix &fbxMatrix,
		Matrix4x4 &glmMatrix
		);

	static void ConvertGLToFBXMatrix(
		const Matrix4x4 &glmMatrix, // Matrix to convert
		FbxAMatrix &fbxMatrix // Converted matrix
		);

	/*
		Takes the current time and the time at the keyframe before and after this time.
		Returns a normalised time value between 0.0 and 1.0 where 0.0 is the time at the last
		keyframe and 1.0 is the time at the next keyframe.
	*/
	static float NormalizeValue(
		const long currentValue, // The current value
		const long lastValue, // Start value
		const long nextValue	 // End Value
		);

	static int Clamp(
		int value,
		int minValue,
		int maxValue
		)
	{
		if(value < minValue)
		{
			value = minValue;
		}
		else if(value > maxValue)
		{
			value = maxValue;
		}

		return value;
	}

	static float Clamp(
		float value,
		float minValue,
		float maxValue
		)
	{
		if(value < minValue)
		{
			value = minValue;
		}
		else if(value > maxValue)
		{
			value = maxValue;
		}

		return value;
	}

};
