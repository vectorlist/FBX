#include <fbxcore.h>
#include <log.h>
#include <fbxtool.h>
#include <meshnode.h>
#include <animsample.h>
#include <animlayer.h>
#include <fbxdevice.h>

FBXCore::FBXCore(const std::string &filename)
{
	//create fbxscene importer manager animation layers
	FBXDevice device(filename);

	auto fbxroot = device.getRootNode();
	auto importer = device.getImporter();
	auto scene = device.getScene();
	

	mNode = std::make_shared<Node>();
	mNode->setSceneName(scene->GetName());
	mNode->setAnimationLayerPtr(device.getAnimationLayer());

	processNodes(fbxroot, mNode->getBoneNodeRoot(), mNode->getMeshNodeRoot());

	processSkins(fbxroot, mNode->getCurrentMeshNode());
}

FBXCore::~FBXCore()
{
	
}

bool FBXCore::processNodes(FbxNode* pNode, BoneNode* parentBoneNode, MeshNode* parentMeshNode)
{
	BoneNode* newBoneNode = NULL;
	MeshNode* newMeshNode = NULL;

	const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
	if (pNodeAttr)
	{
		const auto type = pNodeAttr->GetAttributeType();
		switch (type)
		{
		case fbxsdk::FbxNodeAttribute::eMesh:
			newMeshNode = processMeshNode(pNode, parentMeshNode);
			break;
		case fbxsdk::FbxNodeAttribute::eSkeleton:
			newBoneNode = processBoneNode(pNode, parentBoneNode);
			break;

		default:
			break;
		}
	}
	for (int childIndex = 0; childIndex < pNode->GetChildCount(); ++childIndex)
	{
		FbxNode* pChildNode = pNode->GetChild(childIndex);
		processNodes(pChildNode,
			newBoneNode != NULL ? newBoneNode : parentBoneNode,
			newMeshNode != NULL ? newMeshNode : parentMeshNode);
	}

	return true;
}

BoneNode* FBXCore::processBoneNode(FbxNode* pNode, BoneNode* parent)
{
	const FbxSkeleton* const pSkeleton = static_cast<const FbxSkeleton*>(pNode->GetNodeAttribute());

	BoneNode* boneNode = NULL;
	if (!pSkeleton) {
		LOG_ERROR("failed to convert skeleton node");
		return  NULL;
	}

	boneNode = new BoneNode();

	mNode->addChildBoneNode(parent, boneNode);

	//set name
	auto name = pNode->GetName();
	boneNode->setName(name);
	LOG << "id : "  << " " << boneNode->id() << " create bone node : " << boneNode->getName() << ENDN;;

	//Transforms
	const FbxAMatrix globalTransform = pNode->EvaluateGlobalTransform(0, FbxNode::eDestinationPivot);
	boneNode->mGlobalTransform = globalTransform;

	//TODO : ANIMATION LAYER SAMPLE
	/*------------------------ ANIMATION LAYER SAMPLE ------------------------------*/
	auto layer = mNode->getAnimationLayer();
	AnimSample* animation = layer->getBaseSample();
	int sampleNum = animation->getSamplesFrameNum();

	if (sampleNum)
	{
		boneNode->allocateTracks(sampleNum);
	}

	int startFrame = animation->getSampleStart();
	long startTime = animation->convertFrameToMilli(startFrame);
	FbxTime fbxTime;
	for (int sample = 0; sample <= sampleNum; ++sample)
	{
		long sampleTime = animation->convertFrameToMilli(sample);
		fbxTime.SetMilliSeconds(startTime + sampleTime);
		
		const FbxAMatrix localMatrix =
			pNode->EvaluateLocalTransform(fbxTime, FbxNode::eDestinationPivot);

		//TODO : scale key rotation key
		KeyQuaternion rotationKey(localMatrix.GetQ(), sampleTime);
		KeyVec3 positionKey(localMatrix.GetT(), sampleTime);
		KeyVec3 scaleKey(localMatrix.GetS(), sampleTime);

		//LOG << rotationKey << ENDN;
		boneNode->addRotationKey(rotationKey);
		boneNode->addPositionKey(positionKey);
		boneNode->addScaleKey(scaleKey);
		//LOG << sample <<" " << boneNode->getPositionKey(sample) << ENDN;
	}
	/*------------------------- SAMPLE -----------------------------*/
	if (parent)
	{
		//pNode->
		auto type = pNode->InheritType.Get();
		switch (type)
		{
		case FbxTransform::eInheritRrs:
			boneNode->setInheritScale(false);
			break;
		case FbxTransform::eInheritRrSs:
			LOG_ERROR("RrSs dosent supported");
			break;
		case FbxTransform::eInheritRSrs:
			boneNode->setInheritScale(true);
			break;
		}
	}
	else
	{
		boneNode->setInheritScale(false);
	}

	return boneNode;
}

MeshNode* FBXCore::processMeshNode(FbxNode *pNode, MeshNode *parent)
{
	FbxMesh* pMesh = pNode->GetMesh();
	MeshNode* meshNode = NULL;

	if (!pMesh) {
		LOG << "this node is not supported a mesh" << ENDN;
		return meshNode;
	}

	meshNode = new MeshNode;
	
	mNode->addChildMeshNode(parent, meshNode);

	std::string nodeName = pNode->GetName();
	meshNode->setName(nodeName);

	LOG << "ID : " << meshNode->id() << " Create Mesh Node : " << meshNode->getName() << ENDN;

	const FbxAMatrix globalMatrix = pNode->EvaluateGlobalTransform();
	FbxAMatrix localTRS;
	FbxVector4 T = pNode->LclTranslation.Get();
	FbxVector4 R = pNode->LclRotation.Get();
	FbxVector4 S = pNode->LclScaling.Get();
	localTRS.SetTRS(T, R, S);

	meshNode->setGlobalTransform(localTRS);
	//meshNode->setGlobalTransform(globalMatrix);

	buildMeshNode(pMesh, meshNode);

	return meshNode;
}


void FBXCore::buildMeshNode(FbxMesh* pMesh, MeshNode* pMeshNode)
{
	int vertexNum = pMesh->GetControlPointsCount();

	PointArray& points = pMeshNode->getPoints();
	points.resize(vertexNum);

	const FbxVector4* const contolPoints = pMesh->GetControlPoints();
	for (unsigned int vertexIndex = 0; vertexIndex < vertexNum; ++vertexIndex)
	{
		points[vertexIndex].setPosition(contolPoints[vertexIndex]);
	}
	int faceNums = pMesh->GetPolygonCount();

	FaceArray& faces = pMeshNode->getFaces();
	faces.resize(faceNums);

	for (int faceIndex = 0; faceIndex < faceNums; ++faceIndex)
	{
		Face &face = faces[faceIndex];
		loadVertexIndices(pMesh, faceIndex, face);
		loadNormals(pMesh, faceIndex, face);
		loadSTs(pMesh, faceIndex, face);
	}
}

bool FBXCore::processSkins(FbxNode* pNode, MeshNode* meshNode)
//*Info : create skin vertex data to mehs node (bone id, bone wieght)
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

			std::string temp(pLinkBoneNode->GetName());

			BoneNode* boneNode = mNode->getBoneNodeByName(temp);
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

			PointArray& pointArray = meshNode->getPoints();
			for (int clusterIndex = 0; clusterIndex < clusterIndicesNum; ++clusterIndex)
			{
				int controlIndex = controlPointIndices[clusterIndex];
				assert(controlIndex < vertexNum);

				float controlWeight = (float)controlPointWeights[clusterIndex];
				if (controlWeight < 0.00001f) 
					continue;

				int boneID = boneNode->id();
				
				//LOG << "id : " << controlIndex << " weight : " << controlWeight << ENDN;
				if (!pointArray[controlIndex].addWeight(boneID, controlWeight))
				{
					return false;
				}

			}
		}
	}
	return true;
}
