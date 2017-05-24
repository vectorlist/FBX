#include <fbxcore.h>
#include <log.h>
#include <fbxtool.h>
#include <meshnode.h>
#include <animationsample.h>

FBXCore::FBXCore(const std::string &filename)
{
	FBXDeviceCreateInfo deviceInfo{};
	deviceInfo.filename = filename;
	deviceInfo.enableConvertTriangles = true;
	deviceInfo.enablePose = true;
	deviceInfo.appInfo = Maya;

	mDevice = new FBXDevice(deviceInfo);

	mRootNode = mDevice->getRootNode();

	//ANIMATION TASK 0 1 2
	//0 = animation infomation (fps, fbx time) scene
	createAnimationSamples(mNode);
	//1 = bake transform all node
	bakeNodeTransform(mRootNode);
	//2 = get AnimationSample pointer and convert pivot animations and frame rates
	AnimationSample* sample = mNode.getAnimationSample();
	mRootNode->ConvertPivotAnimationRecursive(NULL,
		FbxNode::eDestinationPivot, sample->getFps());
	//m_rootNode->ConvertPivotAnimationRecursive(,)

	

	//NODE TASK 0 1 2
	//0 = build bone, mesh nodes
	processNodes(mRootNode, mNode.getBoneNodeRoot(), mNode.getMeshNodeRoot());
	//1 = skin for bone id and weight
	MeshNode* currentMeshNode = mNode.getCurrentMeshNode();
	//processSkins(mRootNode, currentMeshNode);
	processSkinNode();
	//2 = build renderable mesh from available mesh node

	sample->setSampleEnd(sample->getSampleEnd() - sample->getSampleStart());
	sample->setSampleStart(0);

}

bool FBXCore::processSkinNode()
{
	bool loadedMesh = false;
	auto meshNodeNum = tempMeshNode.fbxNode.size();

	for (int meshIndex = 0; meshIndex < meshNodeNum; ++meshIndex)
	{
		FbxGeometry* const geo = tempMeshNode.fbxNode[meshIndex]->GetGeometry();
		if (!geo) {
			LOG_ERROR("failed to load geometry");
			continue;
		}
		const FbxSkin* skin = static_cast<const FbxSkin*>(geo->GetDeformer(0, FbxDeformer::eSkin));
		if (!skin) {
			LOG_ERROR("failed to load skin");
			continue;
		}

		const FbxSkin::EType  skinType = skin->GetSkinningType();
		switch (skinType)
		{
		case FbxSkin::eRigid:
		case FbxSkin::eLinear:
		case FbxSkin::eDualQuaternion:
			if (processSkin(geo, tempMeshNode.meshNode[meshIndex]))
			{
				loadedMesh = true;
			}
			continue;
		default:
			LOG_ERROR("unknown skinning type");
		}
	}
	
	return loadedMesh;
}

bool FBXCore::processSkins(FbxNode* pNode, MeshNode* meshNode)
{
	bool loadedSkin = false;

	int childNum = pNode->GetChildCount();
	for (int i = 0; i < childNum; ++i)
	{
		auto* child = pNode->GetChild(i);
		if (!child) continue;
		FbxGeometry* const pGeometry = child->GetGeometry();
		if (!pGeometry) {
			continue;
		}

		const FbxSkin* const pSkin = static_cast<FbxSkin*>(pGeometry->GetDeformer(0, FbxDeformer::eSkin));
		if (!pSkin) {
			continue;
		}

		const FbxSkin::EType skinType = pSkin->GetSkinningType();
		switch (skinType)
		{
		case FbxSkin::eLinear:
		case FbxSkin::eRigid:
		case FbxSkin::eDualQuaternion:
			if (processSkin(pGeometry, meshNode)) {
				loadedSkin = true;
			}
			continue;
		default:
			break;
		}
	}
	
	return loadedSkin;
}

bool FBXCore::processSkin(const FbxGeometry *pGeo, MeshNode* meshNode)
{
	//get vertice from mesh node
	const int vertexNum = pGeo->GetControlPointsCount();

	const int skinNum = pGeo->GetDeformerCount(FbxDeformer::eSkin);

	for (int skinIndex = 0; skinIndex < skinNum; ++skinIndex)
	{
		FbxSkin* pSkin = static_cast<FbxSkin*>(pGeo->GetDeformer(skinIndex, FbxDeformer::eSkin));
		const int clusterNum = pSkin->GetClusterCount();
	
		for (int clusterIndex = 0; clusterIndex < clusterNum; ++clusterIndex)
		{
			FbxCluster* pCluster = pSkin->GetCluster(clusterIndex);

			const auto clusterMode = pCluster->GetLinkMode();
			switch (clusterMode)
			{
			case FbxCluster::eNormalize:
			case FbxCluster::eTotalOne:
				break;
			default:
				LOG_ASSERT("failed to find link bone node");
			}

			FbxNode* const pLinkBoneNode = pCluster->GetLink();
			if (!pLinkBoneNode) {
				LOG << "failed to find linked bone node" << ENDN;
				continue;
			}

			//Importance get BoneNode by name
			std::string temp(pLinkBoneNode->GetName());

			//must be boneNodes created
			BoneNode* boneNode = mNode.getBoneNodeByName(temp);
			assert(boneNode);

			FbxAMatrix boneRefMatrix;
			pCluster->GetTransformLinkMatrix(boneRefMatrix);

			FbxAMatrix boneRefMatrixInverse;
			boneRefMatrixInverse = boneRefMatrix.Inverse();
			
			//LOG_FBX_MATRIX(boneRefMatrixInverse);
			boneNode->setInverseLocalTransfrom(boneRefMatrixInverse);
			
			const int clusterIndicesNum = pCluster->GetControlPointIndicesCount();
			const int *controlPointIndices = pCluster->GetControlPointIndices();
			const double *controlPointWeights = pCluster->GetControlPointWeights();

			VertexArray& vertexArray = meshNode->getVertices();
			for (int clusterIndex = 0; clusterIndex < clusterIndicesNum; ++clusterIndex)
			{
				int controlIndex = controlPointIndices[clusterIndex];
				assert(controlIndex < vertexNum);

				float controlWeight = (float)controlPointWeights[clusterIndex];
				if (controlWeight < 0.00001f) 
					continue;

				int boneID = boneNode->getID();
				
				//LOG << "id : " << controlIndex << " weight : " << controlWeight << ENDN;
				if (!vertexArray[controlIndex].addWeight(boneID, controlWeight))
				{
					return false;
				}

			}
		}
	}
	return true;

}


bool FBXCore::createAnimationSamples(Node &node)
{
	AnimationSamplePtr sample = AnimationSamplePtr(new AnimationSample);
	const FbxTakeInfo* takeInfo = mDevice->getTakeInfos()[0];

	if (!takeInfo) {
		LOG_ERROR("failed to find animation take info");
		return false;
	}
	float frameRate = ANIMATION_CURRENT_FRAME_RATE;
	FbxTime::EMode fbxTimeMode;
	if (mDevice->getImporter()->GetFrameRate(fbxTimeMode))
	{
		frameRate = FbxTime::GetFrameRate(fbxTimeMode);
	}

	sample->setName(takeInfo->mName.Buffer());
	sample->setFps(frameRate);

	const long startTime = takeInfo->mLocalTimeSpan.GetStart().GetMilliSeconds();
	const long endTime = takeInfo->mLocalTimeSpan.GetStop().GetMilliSeconds();
	int sampleStart = sample->convertMilliToFrame(startTime);
	int sampleEnd = sample->convertMilliToFrame(endTime);
	sample->setSampleStart(sampleStart);
	sample->setSampleEnd(sampleEnd);

	//get numFrame
	int frameNum = sample->getSampleEnd() - sample->getSampleStart() - 1;
	sample->setFrameNums(frameNum);

	bool debugAnimationInfo = true;
	if (debugAnimationInfo)
	{
		LOG << "created sample name : " << sample->getName() << " frame per second : " << sample->getFps() << ENDN;
		LOG << "start : " << sample->getSampleStart() << " end : " << sample->getSampleEnd() << ENDN;
		LOG << "sameple num : " << sample->getSamplesFrameNum() << ENDN;
	}

	//sample move to node
	node.setAnimationSample(sample);
	return true;
}

