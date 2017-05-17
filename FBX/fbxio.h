#pragma once

#include <log.h>
#include <fbxsdk.h>
#include <ostream>

//struct VertexBoneData
//{
//	std::array<int, BONE_MAX_PER_VERTEX> IDs;
//	std::array<float, BONE_MAX_PER_VERTEX> weights;
//
//	void add(int boneID, float weight)
//	{
//		for (int i = 0; i < BONE_MAX_PER_VERTEX; ++i)
//		{
//			if (weights[i] == 0.0f)
//			{
//				IDs[i] = boneID;
//				weights[i] = weight;
//				return;
//			}
//		}
//	}
//};
//
//struct BoneData
//{
//	int boneIndex;
//	std::map<int, float> boneMap;
//};

class FBXIO
{
public:
	static void getAttributeString(FbxNodeAttribute* attrib);
};


std::ostream& operator<<(std::ostream &os, FbxAMatrix &mat)
{
	os <<
		(float)mat[0][0] << " ," << (float)mat[0][1] << " ," << mat[0][2] << " ," << mat[0][3] << "\n";


	return os;
}
