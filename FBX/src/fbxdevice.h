#pragma once

#include <fbxsdk.h>
#include <string>
#include <vector>

enum ApplicationInfo
{
	Maya,Max
};

struct FBXDeviceCreateInfo
{
	FBXDeviceCreateInfo()
		: filename(""),
		enablePose(false),
		enableAnimationStack(false),
		enableConvertTriangles(false),
		appInfo(Maya)
	{}
	std::string filename;
	bool enablePose;
	bool enableAnimationStack;
	bool enableConvertTriangles;
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
	const std::vector<FbxTakeInfo*>& getTakeInfos() const;
	float getSceneFrameRate() const;

	void setSceneSetting(const FBXDeviceCreateInfo& info);
private:
	FbxManager* mManager;
	FbxScene* mScene;
	FbxImporter* mImporter;
	std::string mFilename;
	std::vector<FbxTakeInfo*> mTakeInfos;

	FbxArray<FbxString*> mAnimStackNameArray;
	FbxArray<FbxPose*> mPoseArray;

	double mSceneFrameRate;
	void release();
};