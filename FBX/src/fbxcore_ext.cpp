#include <fbxcore.h>
#include <log.h>
#include <fbxtool.h>
#include <animationlayer.h>

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
			if (newMeshNode) {
				LOG << "create mesh node : " << newMeshNode->getName() << ENDN;
			}
			break;
		case fbxsdk::FbxNodeAttribute::eSkeleton:
			newBoneNode = processBoneNode(pNode, parentBoneNode);
			if (newBoneNode) {
				LOG << "create bone node : " << newBoneNode->getName() <<  ENDN;
			}
			//LOG << "founded skeletion attr" << newBoneNode <<ENDN;
			break;

		default:
			break;
		}
	}
	//recursive
	for (int childIndex = 0; childIndex < pNode->GetChildCount(); ++childIndex)
	{
		FbxNode* pChildNode = pNode->GetChild(childIndex);
		//Recursive newboneNode is as a parent node
		processNodes(pChildNode, 
			newBoneNode != NULL ? newBoneNode : parentBoneNode,
			newMeshNode != NULL ? newMeshNode : parentMeshNode);
	}

	return true;
}

//BoneNode* FBXCore::processBoneNode(FbxNode* pNode, BoneNode* parent)
//{
//	//we got skeleton attr already convert to skeleton 
//	const FbxSkeleton* pSkeleton = (FbxSkeleton*)pNode->GetNodeAttribute();
//
//	BoneNode* boneNode = NULL;
//	if (!pSkeleton) {
//		LOG_ERROR("failed to convert skeleton node");
//		return  NULL;
//	}
//
//	//allocate new bone node(current)
//	boneNode = new BoneNode();
//
//	//@PARAM (parent, child)
//	mNode.addChildBoneNode(parent, boneNode);
//
//	//set name
//	auto name = pNode->GetName();
//	boneNode->setName(name);
//
//	//Transforms
//	const FbxAMatrix globalTransform = pNode->EvaluateGlobalTransform(0, FbxNode::eDestinationPivot);
//	boneNode->mGlobalTransform = globalTransform;
//
//	//TODO : ANIMATION LAYER SAMPLE
//	/*------------------------ ANIMATION LAYER SAMPLE ------------------------------*/
//	//replace single sample to layer sample
//	AnimationSample* animation = mNode.getAnimationSample();
//	int sampleNum = animation->getSamplesFrameNum();
//
//	//TASK 0 1 2
//	//0 = allocate track and keys
//	if (sampleNum)
//	{
//		boneNode->allocateTracks(sampleNum);
//	}
//	//1 = sey keys
//	int startFrame = animation->getSampleStart();
//	long startTime = animation->convertFrameToMilli(startFrame);
//	FbxTime fbxTime;
//	for (int sample = 0; sample <= sampleNum; ++sample)
//	{
//		//LOG << "sample : " << sample << ENDN;
//		long sampleTime = animation->convertFrameToMilli(sample);
//		fbxTime.SetMilliSeconds(startTime + sampleTime);
//
//		//get local transform
//		const FbxAMatrix localMatrix =
//			pNode->EvaluateLocalTransform(fbxTime, FbxNode::eDestinationPivot);
//
//		//TODO : scale key rotation key
//		KeyQuaternion rotationKey(localMatrix.GetQ(), sampleTime);
//		KeyVec3 positionKey(localMatrix.GetT(), sampleTime);
//		KeyVec3 scaleKey(localMatrix.GetS(), sampleTime);
//		
//		//LOG << rotationKey << ENDN;
//		boneNode->addRotationKey(rotationKey);
//		boneNode->addPositionKey(positionKey);
//		boneNode->addScaleKey(scaleKey);
//		/*LOG << boneNode->getPositionKey(sample) << ENDN;*/
//	}
//	/*------------------------- SAMPLE -----------------------------*/
//	if (parent)
//	{
//		//pNode->
//		auto type = pNode->InheritType.Get();
//		switch (type)
//		{
//		case FbxTransform::eInheritRrs:
//			boneNode->setInheritScale(false);
//			break;
//		case FbxTransform::eInheritRrSs:
//			LOG_ERROR("RrSs dosent supported");
//			break;
//		case FbxTransform::eInheritRSrs:
//			boneNode->setInheritScale(true);
//			break;
//		}
//	}
//	else
//	{
//		boneNode->setInheritScale(false);
//	}
//
//	return boneNode;
//}

BoneNode* FBXCore::processBoneNode(FbxNode* pNode, BoneNode* parent)
{
	//we got skeleton attr already convert to skeleton 
	const FbxSkeleton* pSkeleton = (FbxSkeleton*)pNode->GetNodeAttribute();

	BoneNode* boneNode = NULL;
	if (!pSkeleton) {
		LOG_ERROR("failed to convert skeleton node");
		return  NULL;
	}

	//allocate new bone node(current)
	boneNode = new BoneNode();

	//@PARAM (parent, child)
	mNode.addChildBoneNode(parent, boneNode);

	//set name
	auto name = pNode->GetName();
	boneNode->setName(name);

	//Transforms
	const FbxAMatrix globalTransform = pNode->EvaluateGlobalTransform(0, FbxNode::eDestinationPivot);
	boneNode->mGlobalTransform = globalTransform;

	//TODO : ANIMATION LAYER SAMPLE
	/*------------------------ ANIMATION LAYER SAMPLE ------------------------------*/
	//replace single sample to layer sample
	/*AnimationSample* animation = mNode.getAnimationSample();
	int sampleNum = animation->getSamplesFrameNum();*/
	AnimationLayers* layers = mNode.getAnimationLayers();
	int totalSampleFrame = layers->getTotalSampleFrameNum();
	LOG << "total allocated : "<< totalSampleFrame << ENDN;
	if(totalSampleFrame)
		boneNode->allocateTracks(totalSampleFrame);

	for (int sampleIndex = 0; sampleIndex < layers->size(); ++sampleIndex)
	{
		AnimationSample* sample = layers->getSample(sampleIndex);
		
		int sampleFrame = sample->getSamplesFrameNum();
		int startFrame = sample->getSampleStart();
		long startTime = sample->convertFrameToMilli(startFrame);
		//LOG << "sample name : " << sample->getName() << " sample frame num : " << sampleFrame << ENDN;
		FbxTime fbxTime;
		for (int frame = startFrame; frame < (sampleFrame + startFrame); ++frame)
		{
			//LOG << frame << ENDN;
			long sampleTime = sample->convertFrameToMilli(frame);
			fbxTime.SetMilliSeconds(startTime + sampleTime);

			//get local transform
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
		}
	}

	//int startFrame = animation->getSampleStart();
	//long startTime = animation->convertFrameToMilli(startFrame);
	//FbxTime fbxTime;
	//for (int sample = 0; sample <= sampleNum; ++sample)
	//{
	//	//LOG << "sample : " << sample << ENDN;
	//	long sampleTime = animation->convertFrameToMilli(sample);
	//	fbxTime.SetMilliSeconds(startTime + sampleTime);

	//	//get local transform
	//	const FbxAMatrix localMatrix =
	//		pNode->EvaluateLocalTransform(fbxTime, FbxNode::eDestinationPivot);

	//	//TODO : scale key rotation key
	//	KeyQuaternion rotationKey(localMatrix.GetQ(), sampleTime);
	//	KeyVec3 positionKey(localMatrix.GetT(), sampleTime);
	//	KeyVec3 scaleKey(localMatrix.GetS(), sampleTime);

	//	//LOG << rotationKey << ENDN;
	//	boneNode->addRotationKey(rotationKey);
	//	boneNode->addPositionKey(positionKey);
	//	boneNode->addScaleKey(scaleKey);
		/*LOG << boneNode->getPositionKey(sample) << ENDN;*/
	//}
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

	if (!pMesh && !pMesh->IsTriangleMesh()) {
		LOG << "FBX Mesh is not supported" << ENDN;
		return NULL;
	}

	meshNode = new MeshNode;
	//it should be (parent ---- child) at recursive
	mNode.addChildMeshNode(parent, meshNode);

	//tempotary
	tempMeshNode.meshNode.push_back(meshNode);
	tempMeshNode.fbxNode.push_back(pNode);

	std::string nodeName = pNode->GetName();
	meshNode->setName(nodeName);

	const FbxAMatrix globalMatrix = pNode->EvaluateGlobalTransform();
	FbxAMatrix localMatrix;
	FbxVector4 position = pNode->LclTranslation.Get();
	FbxVector4 rotation = pNode->LclRotation.Get();
	FbxVector4 scale = pNode->LclScaling.Get();

	localMatrix.SetTRS(position, rotation, scale);
	meshNode->mGlobalMarix = localMatrix;

	buildMeshNode(pMesh, meshNode);
	
	return meshNode;
}


void FBXCore::buildMeshNode(FbxMesh* pMesh, MeshNode* pMeshNode)
{
	int vertexNum = pMesh->GetControlPointsCount();

	VertexArray& vertices = pMeshNode->getVertices();
	vertices.resize(vertexNum);

	const FbxVector4* const contolPoints = pMesh->GetControlPoints();
	for (unsigned int vertexIndex = 0; vertexIndex < vertexNum; ++vertexIndex)
	{
		vertices[vertexIndex].setPosition(contolPoints[vertexIndex]);
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

	pMeshNode->isBuilt = true;
}

void FBXCore::loadVertexIndices(FbxMesh *pMesh, int faceIndex, Face &triangle)
{
	for (int i = 0; i < 3; ++i)
	{
		int vertexIndex = pMesh->GetPolygonVertex(faceIndex, i);
		triangle.setVertexIndex(i, vertexIndex);
	}
}

void FBXCore::loadNormals(FbxMesh * pMesh, int faceIndex, Face & face)
{
	if (pMesh->GetElementNormalCount() > 1) {
		LOG_ERROR("only one set of normal currenty supported");
	}
	FbxGeometryElementNormal* eNormal = pMesh->GetElementNormal();
	if (!eNormal) LOG_ERROR("element normal dosent supported");

	for (int faceElement = 0; faceElement < FACE_COMPONENT_NUM; ++faceElement)
	{
		int vertexIndex = pMesh->GetPolygonVertex(faceIndex, faceElement);
		vec3f normal;
		convert3fDataFromElement(*eNormal, normal, (faceIndex * 3) + faceElement, vertexIndex);
		face.setNormal(faceElement, normal);
	}
}

void FBXCore::loadSTs(FbxMesh * pMesh, int faceIndex, Face & face)
{
	if (pMesh->GetElementUVCount() != 0) {
		//LOG_ERROR("uv dosent supported");
	}

	FbxGeometryElementUV* eST = pMesh->GetElementUV();
	if (!eST) LOG_ERROR("failed to load uv elements");

	for (int faceElement = 0; faceElement < FACE_COMPONENT_NUM; ++faceElement)
	{
		int vertexIndex = pMesh->GetPolygonVertex(faceIndex, faceElement);
		vec2f st;
		convert2fFromElement(*eST, st, faceIndex, faceElement, vertexIndex);
		face.setST(faceElement, st);
	}
}

void FBXCore::convert3fDataFromElement(
	FbxLayerElementTemplate<FbxVector4> &element,
	vec3f &data,
	int triangleCornerId,
	int vertexIndex)
{
	switch (element.GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (element.GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			FbxVector4 fbxData = element.GetDirectArray().GetAt(vertexIndex);
			data[0] = static_cast<float>(fbxData[0]);
			data[1] = static_cast<float>(fbxData[1]);
			data[2] = static_cast<float>(fbxData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = element.GetIndexArray().GetAt(vertexIndex);
			FbxVector4 fbxData = element.GetDirectArray().GetAt(id);
			data[0] = static_cast<float>(fbxData[0]);
			data[1] = static_cast<float>(fbxData[1]);
			data[2] = static_cast<float>(fbxData[2]);
		}
		break;
		default:
			LOG_ERROR("unkown element");
			break;
		}
	}
	break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (element.GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			FbxVector4 fbxData = element.GetDirectArray().GetAt(triangleCornerId);
			data[0] = static_cast<float>(fbxData[0]);
			data[1] = static_cast<float>(fbxData[1]);
			data[2] = static_cast<float>(fbxData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int indexId = element.GetIndexArray().GetAt(triangleCornerId);
			FbxVector4 fbxData = element.GetDirectArray().GetAt(indexId);
			data[0] = static_cast<float>(fbxData[0]);
			data[1] = static_cast<float>(fbxData[1]);
			data[2] = static_cast<float>(fbxData[2]);
		}
		break;

		default:
			LOG_ERROR("unknown element");
			break;
		}
	}
	default:
		break;
	}
}

void FBXCore::convert2fFromElement(
	FbxLayerElementTemplate<FbxVector2> &element,
	vec2f &data,
	int triangleIndex,
	int triangleCornerId,
	int vertexIndex)
{
	switch (element.GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (element.GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			FbxVector2 fbxData = element.GetDirectArray().GetAt(vertexIndex);
			data.x = static_cast<float>(fbxData[0]);
			data.y = static_cast<float>(fbxData[1]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = element.GetIndexArray().GetAt(vertexIndex);
			FbxVector2 fbxUvs = element.GetDirectArray().GetAt(id);
			data.x = static_cast<float>(fbxUvs[0]);
			data.y = static_cast<float>(fbxUvs[1]);
		}
		break;
		default:
			LOG_ERROR("unknown element");
			break;
		}
	}
	break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (element.GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			unsigned int uvIndex = element.GetIndexArray().GetAt(GetUVVertexIndex(triangleIndex, triangleCornerId));
			FbxVector2 fbxData = element.GetDirectArray().GetAt(uvIndex);
			data.x = static_cast<float>(fbxData[0]);
			data.y = static_cast<float>(fbxData[1]);
		}
		break;
		default:
			LOG_ERROR("unknown element");
			break;
		}
	}
	break;
	default:
		LOG_ERROR("unknown element");
		break;
	}
}


const unsigned int FBXCore::GetUVVertexIndex(
	const unsigned int triangleIndex,
	const unsigned int triangleCornerId) const
{
	return triangleIndex * 3 + triangleCornerId; 
}

void FBXCore::bakeNodeTransform(FbxNode* pNode) const
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
		bakeNodeTransform(pNode->GetChild(childNum));
	}
		
}