#include "MathsUtils.h"


void MathsUtils::TranslateRotateScale(
	const vec3f &position,
	const Matrix4x4 &rotation,
	const vec3f &scale,
	Matrix4x4 &result)
{
	result[3][0] = position.x;
	result[3][1] = position.y;
	result[3][2] = position.z;

	result[0][0] = rotation[0][0] * scale.x;
	result[0][1] = rotation[0][1] * scale.x;
	result[0][2] = rotation[0][2] * scale.x;

	result[1][0] = rotation[1][0] * scale.y;
	result[1][1] = rotation[1][1] * scale.y;
	result[1][2] = rotation[1][2] * scale.y;

	result[2][0] = rotation[2][0] * scale.z;
	result[2][1] = rotation[2][1] * scale.z;
	result[2][2] = rotation[2][2] * scale.z;
}

void MathsUtils::ConvertFBXVector4ToGlVec3(
	const FbxVector4 &fbxVec4,
	vec3f &glmVec3
)
{
	glmVec3 = vec3f(static_cast<float>(fbxVec4[0]), static_cast<float>(fbxVec4[1]), static_cast<float>(fbxVec4[2]));
}

void MathsUtils::ConvertFBXVector4ToGlVec4(
	const FbxVector4 &fbxVec4,
	vec4f &glmVec4
)
{
	glmVec4 = vec4f(static_cast<float>(fbxVec4[0]), static_cast<float>(fbxVec4[1]), static_cast<float>(fbxVec4[2]), 1);
}

void MathsUtils::Decompose()
{
	assert("Not implemented");
}

void MathsUtils::ConvertFBXToGLMatrix(
	const FbxAMatrix &fbxMatrix,	// Matrix to convert
	Matrix4x4 &glmMatrix			// Converted matrix
	)
{
	glmMatrix[0][0] = static_cast<float>(fbxMatrix[0][0]);
	glmMatrix[0][1] = static_cast<float>(fbxMatrix[0][1]);
	glmMatrix[0][2] = static_cast<float>(fbxMatrix[0][2]);
	glmMatrix[0][3] = static_cast<float>(fbxMatrix[0][3]);

	glmMatrix[1][0] = static_cast<float>(fbxMatrix[1][0]);
	glmMatrix[1][1] = static_cast<float>(fbxMatrix[1][1]);
	glmMatrix[1][2] = static_cast<float>(fbxMatrix[1][2]);
	glmMatrix[1][3] = static_cast<float>(fbxMatrix[1][3]);

	glmMatrix[2][0] = static_cast<float>(fbxMatrix[2][0]);
	glmMatrix[2][1] = static_cast<float>(fbxMatrix[2][1]);
	glmMatrix[2][2] = static_cast<float>(fbxMatrix[2][2]);
	glmMatrix[2][3] = static_cast<float>(fbxMatrix[2][3]);

	glmMatrix[3][0] = static_cast<float>(fbxMatrix[3][0]);
	glmMatrix[3][1] = static_cast<float>(fbxMatrix[3][1]);
	glmMatrix[3][2] = static_cast<float>(fbxMatrix[3][2]);
	glmMatrix[3][3] = static_cast<float>(fbxMatrix[3][3]);

}

void MathsUtils::ConvertGLToFBXMatrix(
	const Matrix4x4 &glmMatrix,	// Matrix to convert
	FbxAMatrix &fbxMatrix			// Converted matrix
	)
{
	fbxMatrix[0][0] = glmMatrix[0][0];
	fbxMatrix[0][1] = fbxMatrix[0][1];
	fbxMatrix[0][2] = fbxMatrix[0][2];
	fbxMatrix[0][3] = glmMatrix[0][3];

	fbxMatrix[1][0] = glmMatrix[1][0];
	fbxMatrix[1][1] = glmMatrix[1][1];
	fbxMatrix[1][2] = glmMatrix[1][2];
	fbxMatrix[1][3] = glmMatrix[1][3];

	fbxMatrix[2][0] = glmMatrix[2][0];
	fbxMatrix[2][1] = glmMatrix[2][1];
	fbxMatrix[2][2] = glmMatrix[2][2];
	fbxMatrix[2][3] = glmMatrix[2][3];

	fbxMatrix[3][0] = glmMatrix[3][0];
	fbxMatrix[3][1] = glmMatrix[3][1];
	fbxMatrix[3][2] = glmMatrix[3][2];
	fbxMatrix[3][3] = glmMatrix[3][3];
}

float MathsUtils::NormalizeValue(
	const long currentValue, // The current value - must be between start and end values
	const long lastValue,	 // Start value
	const long nextValue	 // End Value
	)
{
	assert(currentValue >= lastValue && currentValue <= nextValue);
	// Find the current time value as a 0 - 1 proporion between the two keys
	float normalizedValue = (currentValue - lastValue) / (float)(nextValue - lastValue);
	assert(normalizedValue > -0.00001f && normalizedValue < 1.000001f);
	return normalizedValue;
}

