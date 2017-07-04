#pragma once

#include <fbxsdk.h>
#include <string>
#include <vector>
#include <config.h>

class FBXDevice
{
public:
	FBXDevice(const std::string &filename);
	~FBXDevice();

	FbxManager* GetManager();
	FbxImporter* GetImporter();
	FbxScene* GetScene();
	FbxNode* GetRootNode();
	animlayer_ptr GetAnimationLayer();

	void initialize();
	void bakeNodeTransforms(FbxNode* pNode) const;
	bool isTriangleMesh(FbxNode* pNode) const;
	void SetPoses();
	void LoadPoseNodes(FbxPose* pose, FbxNode* node);
private:
	FbxManager* mManager;
	FbxScene* mScene;
	FbxImporter* mImporter;
	
	FbxArray<FbxString*> mAnimStackNameArray;
	/*FbxArray<FbxPose*> mPoseArray;*/
	std::vector<FbxPose*> mPoses;
	FbxAMatrix mPoseLocalMatrix;

	animlayer_ptr mLayer;

	void release();
};