#include <fbxdevice.h>
#include <animlayer.h>
#include <log.h>

FBXDevice::FBXDevice(const std::string &filename)
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

	if (mImporter->Initialize(filename.c_str(), -1, ioSetting) == true)
	{
		std::string loading;
		loading = "importing File ";
		loading += filename;
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
		LOG_ASSERT("failed to import file: " + filename);
	}

	initialize();
}

FBXDevice::~FBXDevice()
{
	release();
}

void FBXDevice::initialize()
{
	//OpenGL and Maya is same axis order
	FbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem mayaSystem = FbxAxisSystem::MayaYUp;
	if (SceneAxisSystem != mayaSystem)
	{
		mayaSystem.ConvertScene(mScene);
	}
	
	//covert triangle mesh
	bool isTri = isTriangleMesh(mScene->GetRootNode());

	if (!isTri) {
		LOG << "convert mesh to trianulate" << ENDN;
		FbxGeometryConverter lGeomConverter(mManager);
		lGeomConverter.Triangulate(mScene, true);
	}

	//create layers for animation sample
	mScene->FillAnimStackNameArray(mAnimStackNameArray);
	mLayer = std::make_shared<AnimLayer>();
	mLayer->createLayers(mImporter);

	//convert pivot animation recursive all node
	bakeNodeTransforms(GetRootNode());
	//TODO : fix FbxTime::eFrame 24
	GetRootNode()->ConvertPivotAnimationRecursive(
		NULL,
		FbxNode::eDestinationPivot,
		24.0f);

	SetPoses();
}

bool FBXDevice::isTriangleMesh(FbxNode *pNode) const
{
	const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
	bool found = false;
	if (pNodeAttr)
	{
		const auto type = pNodeAttr->GetAttributeType();
		if (type == FbxNodeAttribute::eMesh)
		{
			auto* pMesh = pNode->GetMesh();
			found = pMesh->IsTriangleMesh();
			if (found) {
				return found;
			}
			//return pMesh->IsTriangleMesh();
		}

	}
	for (int childIndex = 0; childIndex < pNode->GetChildCount(); ++childIndex)
	{
		FbxNode* pChildNode = pNode->GetChild(childIndex);
		isTriangleMesh(pChildNode);
	}
	return found;
}

void FBXDevice::bakeNodeTransforms(FbxNode* pNode) const
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

	pNode->SetQuaternionInterpolation(FbxNode::eDestinationPivot,
		pNode->GetQuaternionInterpolation(FbxNode::eSourcePivot));

	const int childCount = pNode->GetChildCount();
	for (int childNum = 0; childNum < childCount; childNum++)
	{
		bakeNodeTransforms(pNode->GetChild(childNum));
	}
}

//const FbxArray<FbxPose *> & lPoseArray = gSceneContext->GetPoseArray();
//for (int lPoseIndex = 0; lPoseIndex < lPoseArray.GetCount(); ++lPoseIndex)
//{
//	if (lPoseArray[lPoseIndex]->IsBindPose())
//	{
//		glutAddMenuEntry(lPoseArray[lPoseIndex]->GetName(), lPoseIndex);
//		lBindPoseCount++;
//	}
//}

void FBXDevice::SetPoses()
{
	
	//TODO : it must be 3 Poses
	auto PoseNum = mScene->GetPoseCount();

	for (int i = 0; i < PoseNum; ++i)
	{
		auto* pose = mScene->GetPose(i);
		mPoses.push_back(pose);
	}
}

void FBXDevice::LoadPoseNodes(FbxPose* pose, FbxNode* node)
{
	//TODO : get pose Matrix for blend animation (Weight B) 
	
}

FbxManager* FBXDevice::GetManager()
{
	return mManager;
}

FbxImporter* FBXDevice::GetImporter()
{
	return mImporter;
}

FbxScene* FBXDevice::GetScene()
{
	return mScene;
}

FbxNode* FBXDevice::GetRootNode()
{
	return mScene->GetRootNode();
}

animlayer_ptr FBXDevice::GetAnimationLayer()
{
	return std::move(mLayer);
}

void FBXDevice::release()
{
	mImporter->Destroy();
	mScene->Destroy();
	mManager->Destroy();
	
	mManager = NULL;
	mImporter = NULL;
	mScene = NULL;
}
