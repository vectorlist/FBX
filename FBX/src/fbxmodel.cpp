#include <fbxmodel.h>
#include <log.h>
#include <fbxtool.h>
#include <meshnode.h>
#include <animationsample.h>

FBXModel::FBXModel(const std::string &filename, GLuint shader)
	: m_shader(shader)
{
	FBXDeviceCreateInfo deviceInfo{};
	deviceInfo.filename = filename;
	
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
	loadNodes(mRootNode, mNode.getBoneNodeRoot(), mNode.getMeshNodeRoot());
	//1 = skin for bone id and weight
	MeshNode* currentMeshNode = mNode.getCurrentMeshNode();
	buildSkin(mRootNode, currentMeshNode);
	//2 = build renderable mesh from available mesh node
	FBXTool::buildMesh(currentMeshNode, m_mesh);

}

bool FBXModel::createAnimationSamples(Node &node)
{
	AnimationSamplePtr sample = AnimationSamplePtr(new AnimationSample);
	const FbxTakeInfo* takeInfo = mDevice->getTakeInfos()[0];

	if (!takeInfo) {
		LOG_ERROR("failed to find animation take info");
		return false;
	}
	float frameRate = ANIMATION_CURRENT_FRAME_RATE;
	if (mDevice->getSceneFrameRate() > 0.0f) {
		frameRate = mDevice->getSceneFrameRate();
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
		LOG << "created sample name : " << sample->getName() <<" frame per second : " << sample->getFps() << ENDN;
		LOG << "start : " << sample->getSampleStart() << " end : " << sample->getSampleEnd() << ENDN;
		LOG	<< "sameple num : " << sample->getSamplesFrameNum() << ENDN;
	}

	//sample move to node
	node.setAnimationSample(sample);
	return true;
}


bool FBXModel::buildSkin(FbxNode* pNode, MeshNode* meshNode)
{
	bool loadedSkin = false;

	int childNum = pNode->GetChildCount();
	for (int i = 0; i < childNum; ++i)
	{
		auto* child = pNode->GetChild(i);
		//auto* pMesh = child->GetMesh();
		//if (!pMesh) continue;
		if (!child) continue;
		FbxGeometry* const pGeometry = child->GetGeometry();
		if (!pGeometry) {
			//LOG << "Unssuport skin type used"<< ENDN;
			continue;
		}

		const FbxSkin* const pSkin = static_cast<FbxSkin*>(pGeometry->GetDeformer(0, FbxDeformer::eSkin));
		if (!pSkin) {
			//LOG << "skin deformer is null" << ENDN;
			continue;
		}

		const FbxSkin::EType skinType = pSkin->GetSkinningType();
		switch (skinType)
		{
		case FbxSkin::eLinear:
		case FbxSkin::eRigid:
		case FbxSkin::eDualQuaternion:
			if (loadSkin(pGeometry, meshNode)) {
				loadedSkin = true;
				LOG << "loaded skin : " << ENDN;
			}
			continue;
		default:
			LOG << "default break" << ENDL;
			break;
		}
	}
	
	return loadedSkin;
}

bool FBXModel::loadSkin(const FbxGeometry *pGeo, MeshNode* meshNode)
{
	//get vertice from mesh node
	auto& vertices = meshNode->getVertices();
	const int vertexNum = pGeo->GetControlPointsCount();

	const int skinNum = pGeo->GetDeformerCount(FbxDeformer::eSkin);

	for (int skinIndex = 0; skinIndex < skinNum; ++skinIndex)
	{
		FbxSkin* pSkin = static_cast<FbxSkin*>(pGeo->GetDeformer(skinIndex, FbxDeformer::eSkin));
		const int clusterNum = pSkin->GetClusterCount();
		LOG << "clusters : " << clusterNum << ENDN;
		for (int clusterIndex = 0; clusterIndex < clusterNum; ++clusterIndex)
		{
			FbxCluster* pCluster = pSkin->GetCluster(clusterIndex);
			const auto clusterMode = pCluster->GetLinkMode();
			switch (clusterMode)
			{
			case FbxCluster::eNormalize:
				LOG << "normalize mode" << ENDL;
				/*break;*/
			case FbxCluster::eTotalOne:
				LOG << "total one mode" << ENDL;
				break;
			default:
				LOG << "additive mode" << ENDL;
				//break;
			}

			FbxNode* const pLinkBoneNode = pCluster->GetLink();
			if (!pLinkBoneNode) {
				LOG << "failed to find linked bone node" << ENDN;
				continue;
			}

			//Importance get BoneNode by name
			std::string temp(pLinkBoneNode->GetName());
			LOG << temp << ENDN;
			//must be boneNodes created
			BoneNode* boneNode = mNode.getBoneNodeByName(temp);

			//transform
			FbxAMatrix boneRefMatrix;
			pCluster->GetTransformLinkMatrix(boneRefMatrix);

			FbxAMatrix boneRefMatrixInverse;
			boneRefMatrixInverse = boneRefMatrix.Inverse();
			//set global matrix (outside)

			//set ref inverse transform
			boneNode->mInverseTransform = boneRefMatrixInverse;
			

			const int clusterIndicesNum = pCluster->GetControlPointIndicesCount();
			const int *controlPointIndices = pCluster->GetControlPointIndices();
			const double *controlPointWeights = pCluster->GetControlPointWeights();

			for (int clusterIndex = 0; clusterIndex < clusterIndicesNum; ++clusterIndex)
			{
				int controlIndex = controlPointIndices[clusterIndex];

				//Error out of index control points nums
				if (controlIndex >= vertexNum) {
					controlIndex = vertexNum - 1;
				}

				const float controlWeight = (float)controlPointWeights[clusterIndex];
				if (controlWeight < 0.00001f) continue;

				
				//into BoneNode ID , control point weight
				int boneID = boneNode->getID();

				//LOG << "vertices : " << controlIndex <<
				//	" bone id" << boneID << " weight : " << controlWeight << ENDN;
				vertices[controlIndex].addWeight(boneID, controlWeight);

			}
		}
	}
	return false;

}




