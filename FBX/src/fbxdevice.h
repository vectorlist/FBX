#pragma once

#include <fbxsdk.h>
#include <string>
#include <vector>
#include <config.h>

enum ApplicationInfo
{
	Maya,Max
};

struct FBXDeviceCreateInfo
{
	FBXDeviceCreateInfo()
		: filename(""),
		enablePose(false),
		appInfo(Maya)
	{}
	std::string filename;
	bool enablePose;
	ApplicationInfo appInfo;
};

class FBXDevice
{
public:
	FBXDevice(const FBXDeviceCreateInfo &deviceInfo);
	~FBXDevice();

	FbxManager* getManager();
	FbxImporter* getImporter();
	FbxScene* getScene();
	FbxNode* getRootNode();
	float getSceneFrameRate() const;
	AnimationLayersPtr getAnimationLayers();

	void processProcedural(const FBXDeviceCreateInfo& info);
	void bakeTransforms(FbxNode* pNode) const;
	bool isTriangleMesh(FbxNode* pNode) const;
private:

	FbxManager* mManager;
	FbxScene* mScene;
	FbxImporter* mImporter;
	std::string mFilename;
	
	FbxArray<FbxString*> mAnimStackNameArray;
	FbxArray<FbxPose*> mPoseArray;

	AnimationLayersPtr mLayers;

	double mSceneFps;
	bool mIsTriangleMesh;
	void release();
};