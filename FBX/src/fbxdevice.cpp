#include <fbxdevice.h>
#include <log.h>
#include <animationlayer.h>

FBXDevice::FBXDevice(const FBXDeviceCreateInfo &deviceInfo)
	: mFilename(deviceInfo.filename), mSceneFps(0.f)
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

	processProcedural(deviceInfo);
}

FBXDevice::~FBXDevice()
{
	release();
}

void FBXDevice::processProcedural(const FBXDeviceCreateInfo& info)
{
	if (info.appInfo == Maya)
	{
		FbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
		FbxAxisSystem mayaSystem = FbxAxisSystem::MayaYUp;
		if (SceneAxisSystem != mayaSystem)
		{
			mayaSystem.ConvertScene(mScene);
		}
	}

	mIsTriangleMesh = isTriangleMesh(mScene->GetRootNode());

	if (!mIsTriangleMesh) {
		FbxGeometryConverter lGeomConverter(mManager);
		lGeomConverter.Triangulate(mScene, true);
	}

	//PROCESS ANIMATION INFO

	mScene->FillAnimStackNameArray(mAnimStackNameArray);
	mLayers = std::make_shared<AnimationLayers>();
	mLayers->createLayers(mImporter);

	


	/*FbxTime::EMode timeMode;
	if (mImporter->GetFrameRate(timeMode))
	{
		mSceneFrameRate = FbxTime::GetFrameRate(timeMode);
	}*/
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

float FBXDevice::getSceneFrameRate() const
{
	return mSceneFps;
}

AnimationLayersPtr FBXDevice::getAnimationLayers()
{
	return std::move(mLayers);
}


void FBXDevice::bakeTransforms(FbxNode* pNode) const
// We set up what we want to bake via ConvertPivotAnimationRecursive.
// When the destination is set to 0, baking will occur.
// When the destination value is set to the source¡¯s value, the source values will be retained and not baked.
{
	FbxVector4 zeroVector(0, 0, 0);

	pNode->SetPivotState(FbxNode::eSourcePivot, FbxNode::ePivotActive);
	pNode->SetPivotState(FbxNode::eDestinationPivot, FbxNode::ePivotActive);

	pNode->SetPostRotation(FbxNode::eDestinationPivot, zeroVector);
	pNode->SetPreRotation(FbxNode::eDestinationPivot, zeroVector);
	pNode->SetRotationOffset(FbxNode::eDestinationPivot, zeroVector);
	pNode->SetScalingOffset(FbxNode::eDestinationPivot, zeroVector);
	pNode->SetRotationPivot(FbxNode::eDestinationPivot, zeroVector);
	pNode->SetScalingPivot(FbxNode::eDestinationPivot, zeroVector);

	pNode->SetRotationOrder(FbxNode::eDestinationPivot, eEulerXYZ);

	pNode->SetGeometricTranslation(FbxNode::eDestinationPivot,
		pNode->GetGeometricTranslation(FbxNode::eSourcePivot));
	pNode->SetGeometricRotation(FbxNode::eDestinationPivot,
		pNode->GetGeometricRotation(FbxNode::eSourcePivot));
	pNode->SetGeometricScaling(FbxNode::eDestinationPivot,
		pNode->GetGeometricScaling(FbxNode::eSourcePivot));

	pNode->SetQuaternionInterpolation(FbxNode::eDestinationPivot, pNode->GetQuaternionInterpolation(FbxNode::eSourcePivot));

	const int childCount = pNode->GetChildCount();
	for (int childNum = 0; childNum < childCount; childNum++)
	{
		bakeTransforms(pNode->GetChild(childNum));
	}

}

bool FBXDevice::isTriangleMesh(FbxNode *pNode) const
{
	const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
	if (pNodeAttr)
	{
		const auto type = pNodeAttr->GetAttributeType();
		if (type == FbxNodeAttribute::eMesh)
		{
			auto* pMesh = static_cast<FbxMesh*>(pNode->GetGeometry());
			return pMesh->IsTriangleMesh();
		}
		
	}
	for (int childIndex = 0; childIndex < pNode->GetChildCount(); ++childIndex)
	{
		FbxNode* pChildNode = pNode->GetChild(childIndex);
		isTriangleMesh(pChildNode);
	}
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
