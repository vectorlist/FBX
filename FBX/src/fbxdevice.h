#pragma once

#include <fbxsdk.h>
#include <string>
#include <vector>

struct FBXDeviceCreateInfo
{
	std::string filename;
	bool enablePose;
	bool enableAnimationStack;
	bool enableConvertTriangles;
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