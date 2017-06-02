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

	FbxManager* getManager();
	FbxImporter* getImporter();
	FbxScene* getScene();
	FbxNode* getRootNode();
	animlayer_ptr getAnimationLayer();

	void initialize();
	void bakeNodeTransforms(FbxNode* pNode) const;
	bool isTriangleMesh(FbxNode* pNode) const;
private:
	FbxManager* mManager;
	FbxScene* mScene;
	FbxImporter* mImporter;
	
	FbxArray<FbxString*> mAnimStackNameArray;
	FbxArray<FbxPose*> mPoseArray;

	animlayer_ptr mLayer;

	void release();
};