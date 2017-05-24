#pragma once

#include <fbxsdk.h>
#include <vertex.h>
#include <face.h>
#include <node.h>
#include <key.h>
#include <sstream>

#define LOG_FBX_MATRIX(x)		FBXTool::logFbxMatrix(x)

class BoneNode;
class MeshNode;
class Node;
class FBXTool
{
public:
	//meshnode to renderable mesh
	
	static float clamp(float val, float fmin, float fmax);

	static void lerp(
		const KeyVec3& current,
		const KeyVec3& next,
		KeyVec3& result,
		float normalizedTime);

	static void lerp(
		const KeyQuaternion& current,
		const KeyQuaternion& next,
		KeyQuaternion& result,
		float normalizedTime);

	static void slerp(
		const KeyQuaternion& last,
		const KeyQuaternion& nextKey,
		KeyQuaternion& result,
		float normalizedTime);


	//output
	static void logFbxMatrix(const FbxAMatrix &mat);
};

