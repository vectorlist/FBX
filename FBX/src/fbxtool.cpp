#include "fbxtool.h"
#include <log.h>
#include <bonenode.h>
#include <meshnode.h>

float FBXTool::clamp(float val, float fmin, float fmax)
{
	if (val < fmin) {
		val = fmin;
	}
	else if (val > fmax) {
		val = fmax;
	}
	return val;
}


void FBXTool::lerp(
	const KeyVec3& current,
	const KeyVec3& next,
	KeyVec3& result,
	float normalizedTime)
{
	//A + (B - A) * T
	result.mVector = current.data() + (next.data() - current.data()) * normalizedTime;
	return;
}

void FBXTool::lerp(
	const KeyQuaternion& current,
	const KeyQuaternion& next,
	KeyQuaternion& result,
	float normalizedTime)
{
	result.mQuaternion = current.data() + (next.data() - current.data()) * normalizedTime;
	return;
}

void FBXTool::slerp(
	const KeyQuaternion& key,
	const KeyQuaternion& next,
	KeyQuaternion& result,
	float normalizedTime)
{
	//http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/
	FbxQuaternion nextQuaternion = next.mQuaternion; //Copy this incase we need to negate it

														 // First calcualte the 4D dot product which gives cos theta
	double cosTheta = key.mQuaternion[0] * nextQuaternion[0] + key.mQuaternion[1] * nextQuaternion[1]
		+ key.mQuaternion[2] * nextQuaternion[2] + key.mQuaternion[3] * nextQuaternion[3];

	if (cosTheta == 1)
	{
		result.mQuaternion = key.mQuaternion; // The two key are the same so just return.
		return;
	}

	if (cosTheta < 0) // q1 and q2 are more than 90 degrees apart so invert one of them to reduce spinning
	{
		cosTheta *= -1;
		nextQuaternion = -next.mQuaternion;
	}

	if (cosTheta < 0.99999f) // If the keys are different
	{
		double theta = acos(cosTheta);
		double firstKeyWeight = sin((1 - normalizedTime) * theta) / sin(theta);
		double nextKeyWeight = sin(normalizedTime * theta) / sin(theta);

		FbxQuaternion interpolatedQuat = (key.mQuaternion * firstKeyWeight) + (nextQuaternion * nextKeyWeight);

		result = KeyQuaternion(interpolatedQuat, 0);
		return;
	}

	return lerp(key, next, result,normalizedTime);
}


void FBXTool::logFbxMatrix(const FbxAMatrix &mat)
{
	std::stringstream st;
#define FM std::setw(12)
		//row major order
		st << "FbxAMatrix[" << "\n" <<
			FM << mat[0][0] << FM << mat[0][1] << FM << mat[0][2] << FM << mat[0][3] << "\n" <<
			FM << mat[1][0] << FM << mat[1][1] << FM << mat[1][2] << FM << mat[1][3] << "\n" <<
			FM << mat[2][0] << FM << mat[2][1] << FM << mat[2][2] << FM << mat[2][3] << "\n" <<
			FM << mat[3][0] << FM << mat[3][1] << FM << mat[3][2] << FM << mat[3][3] << " ]" << "\n";
#undef FM
		
	LOG << st.str() << ENDN;
}
