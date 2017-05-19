#include <fbxdevice.h>
#include <log.h>

FBXDevice::FBXDevice(const FBXDeviceCreateInfo &deviceInfo)
	: mFilename(deviceInfo.filename), mSceneFrameRate(0.f)
{
	mManager = FbxManager::Create();
	if (!mManager)
	{
		LOG_ASSERT("Error: Unable to create FBX Manager!\n");
	}

	LOG << "Autodesk FBX SDK version : " << mManager->GetVersion() << ENDN;

	FbxIOSettings* ioSetting = FbxIOSettings::Create(mManager, IOSROOT);
	mManager->SetIOSettings(ioSetting);

	mScene = FbxScene::Create(mManager, "FBX Animation");
	if (!mScene)
	{
		LOG_ASSERT("failed to create FBX scene");
	}

	mImporter = FbxImporter::Create(mManager, "");

	if (mImporter->Initialize(mFilename.c_str(), -1, ioSetting) == true)
	{
		std::string loading;
		loading = "Importing File ";
		loading += mFilename;
		loading += " Please Wait!";
	}
	else
	{
		std::string err("Unable to open file ");
		err += mImporter->GetStatus().GetErrorString();
		LOG_ASSERT(err);
	}

	if (!mImporter->Import(mScene))
	{
		LOG_ASSERT("Import failed for file: " + mFilename);
	}

	setSceneSetting(deviceInfo);
}

FBXDevice::~FBXDevice()
{
	release();
}

void FBXDevice::setSceneSetting(const FBXDeviceCreateInfo& info)
{
	FbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
	if (SceneAxisSystem != OurAxisSystem)
	{
		OurAxisSystem.ConvertScene(mScene);
	}

	//// Convert Unit System to what is used in this example, if needed
	//FbxSystemUnit SceneSystemUnit = mScene->GetGlobalSettings().GetSystemUnit();
	//if (SceneSystemUnit.GetScaleFactor() != 1.0)
	//{
	//	//The unit in this example is centimeter.
	//	FbxSystemUnit::cm.ConvertScene(mScene);
	//}

	if (info.enableAnimationStack) {
		mScene->FillAnimStackNameArray(mAnimStackNameArray);
	}

	if (info.enableConvertTriangles) {
		FbxGeometryConverter lGeomConverter(mManager);
		lGeomConverter.Triangulate(mScene, true);
	}

	int animationStackNum = mImporter->GetAnimStackCount();
	for(int i = 0; i < animationStackNum; ++i)
	{
		FbxTakeInfo* takeInfo = mImporter->GetTakeInfo(i);
		mTakeInfos.push_back(takeInfo);
	}

	FbxTime::EMode timeMode;
	if (mImporter->GetFrameRate(timeMode))
	{
		mSceneFrameRate = FbxTime::GetFrameRate(timeMode);
	}
}

FbxManager* FBXDevice::getManager()
{
	return mManager;
}

FbxImporter* FBXDevice::getImporter()
{
	return mImporter;
}

FbxScene* FBXDevice::getScene()
{
	return mScene;
}

FbxNode* FBXDevice::getRootNode()
{
	return mScene->GetRootNode();
}

const std::vector<FbxTakeInfo*>& FBXDevice::getTakeInfos() const
{
	return mTakeInfos;
}

float FBXDevice::getSceneFrameRate() const
{
	return mSceneFrameRate;
}



void FBXDevice::release()
{
	mImporter->Destroy();
	mManager->Destroy();
	mScene->Destroy();

	mManager = NULL;
	mImporter = NULL;
	mScene = NULL;
}
