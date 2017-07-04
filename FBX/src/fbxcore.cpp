#include <fbxcore.h>
#include <log.h>
#include <meshnode.h>
#include <animsample.h>
#include <animlayer.h>
#include <fbxdevice.h>
#include <matrix4x4.h>
#include <quaternion.h>

FBXCore::FBXCore(const std::string &filename)
{
	//create fbxscene importer manager animation layers
	FBXDevice Device(filename);

	auto FbxRoot = Device.GetRootNode();
	mImporter = Device.GetImporter();
	auto Scene = Device.GetScene();
	

	mNode = std::make_shared<Node>();
	mNode->SetAnimationLayerPtr(Device.GetAnimationLayer());

	ProcessNodes(FbxRoot, mNode->GetBoneNodeRoot(), mNode->GetMeshNodeRoot());
	ProcessSkins(FbxRoot, mNode->GetCurrentMeshNode());
}

FBXCore::~FBXCore()
{
	
}

bool FBXCore::ProcessNodes(FbxNode* pNode, BoneNode* parentBoneNode, MeshNode* parentMeshNode)
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
			newMeshNode = ProcessMeshNode(pNode, parentMeshNode);
			break;
		case fbxsdk::FbxNodeAttribute::eSkeleton:
			newBoneNode = ProcessBoneNode(pNode, parentBoneNode);
			break;

		default:
			break;
		}
	}
	for (int childIndex = 0; childIndex < pNode->GetChildCount(); ++childIndex)
	{
		FbxNode* pChildNode = pNode->GetChild(childIndex);
		ProcessNodes(pChildNode,
			newBoneNode != NULL ? newBoneNode : parentBoneNode,
			newMeshNode != NULL ? newMeshNode : parentMeshNode);
	}

	return true;
}

BoneNode* FBXCore::ProcessBoneNode(FbxNode* pNode, BoneNode* parent)
{
	const FbxSkeleton* const pSkeleton = static_cast<const FbxSkeleton*>(pNode->GetNodeAttribute());

	BoneNode* boneNode = NULL;
	if (!pSkeleton) {
		LOG_ERROR("failed to convert skeleton node");
		return  NULL;
	}

	boneNode = new BoneNode();

	mNode->AddChildBoneNode(parent, boneNode);

	//set name
	auto name = pNode->GetName();
	boneNode->SetName(name);
	LOG << "id : " << boneNode->GetId() << " create bone node : " << boneNode->GetName() << ENDN;;

	//Transforms
	const FbxAMatrix FBXGlobal = pNode->EvaluateGlobalTransform(0, FbxNode::eDestinationPivot);
	boneNode->SetGlobalTransform(Matrix4x4(FBXGlobal.Transpose()));
	boneNode->SetNextGlobalTransform(Matrix4x4(FBXGlobal.Transpose()));

	//TODO : REPLACE TAKE ANIMATION
	/*------------------------ ANIMATION LAYER SAMPLE ------------------------------*/
	//Base Animation Take
	FbxTakeInfo* Take = mImporter->GetTakeInfo(0);
	FbxTime Start = Take->mLocalTimeSpan.GetStart();
	FbxTime End = Take->mLocalTimeSpan.GetStop();

	FbxTime Length = End.GetFrameCount(FRAME_24) -
		Start.GetFrameCount(FRAME_24) + 1;

	if (Length.Get()) {
		//Set Tack Key Nums
		boneNode->AllocateTracks(Length.Get());
	}

	for (FbxLongLong i = Start.GetFrameCount(FRAME_24);
		i <= End.GetFrameCount(FRAME_24); ++i)
	{
		
		FbxTime currentFbxTime;
		currentFbxTime.SetFrame(i, FRAME_24);
		
		const FbxAMatrix FBXLocal = pNode->EvaluateLocalTransform(currentFbxTime, FbxNode::eDestinationPivot);
		//convert Fbx Matrix to Matrix4x4
		Matrix4x4 LocalM(FBXLocal.Transpose());

		auto Prositon = LocalM.getTransform();
		auto Scale = LocalM.getScale();
		auto Rotation = Quaternion(LocalM);
	
		float currentTime = (float)currentFbxTime.GetMilliSeconds() / 1000.f;

		VectorKey PositionKey(Prositon, currentTime);
		VectorKey ScaleKey(Scale, currentTime);
		QuatKey RotationKey(Rotation, currentTime);

		boneNode->AddPositionKey(PositionKey);
		boneNode->AddScaleKey(ScaleKey);
		boneNode->AddRotationKey(RotationKey);
	}
	if (parent)
	{
		//pNode->
		auto type = pNode->InheritType.Get();
		switch (type)
		{
		case FbxTransform::eInheritRrs:
			boneNode->SetInheritScale(false);
			break;
		case FbxTransform::eInheritRrSs:
			LOG_ERROR("RrSs dosent supported");
			break;
		case FbxTransform::eInheritRSrs:
			boneNode->SetInheritScale(true);
			break;
		}
	}
	else
	{
		boneNode->SetInheritScale(false);
	}

	return boneNode;
}

MeshNode* FBXCore::ProcessMeshNode(FbxNode *pNode, MeshNode *parent)
{
	FbxMesh* pMesh = pNode->GetMesh();
	MeshNode* meshNode = NULL;

	if (!pMesh) {
		LOG << "this node is not supported a mesh" << ENDN;
		return meshNode;
	}

	meshNode = new MeshNode;
	
	mNode->AddChildMeshNode(parent, meshNode);

	std::string nodeName = pNode->GetName();
	meshNode->SetName(nodeName);

	LOG << "ID : " << meshNode->GetId() << " Create Mesh Node : " << meshNode->GetName() << ENDN;

	const FbxAMatrix globalMatrix = pNode->EvaluateGlobalTransform();
	FbxAMatrix localTRS;
	FbxVector4 T = pNode->LclTranslation.Get();
	FbxVector4 R = pNode->LclRotation.Get();
	FbxVector4 S = pNode->LclScaling.Get();
	localTRS.SetTRS(T, R, S);

	Matrix4x4 LocalM(localTRS.Transpose());
	meshNode->SetGlobalTransform(LocalM);

	BuildMeshNode(pMesh, meshNode);

	return meshNode;
}


void FBXCore::BuildMeshNode(FbxMesh* pMesh, MeshNode* pMeshNode)
{
	int vertexNum = pMesh->GetControlPointsCount();

	auto& points = pMeshNode->GetPoints();
	points.resize(vertexNum);

	const FbxVector4* const contolPoints = pMesh->GetControlPoints();
	for (unsigned int vertexIndex = 0; vertexIndex < vertexNum; ++vertexIndex)
	{
		points[vertexIndex].SetPosition(contolPoints[vertexIndex]);
	}
	int faceNums = pMesh->GetPolygonCount();

	auto& faces = pMeshNode->GetFaces();
	faces.resize(faceNums);

	for (int faceIndex = 0; faceIndex < faceNums; ++faceIndex)
	{
		Face &face = faces[faceIndex];
		LoadVertexIndices(pMesh, faceIndex, face);
		LoadNormals(pMesh, faceIndex, face);
		LoadSTs(pMesh, faceIndex, face);
	}
}

bool FBXCore::ProcessSkins(FbxNode* pNode, MeshNode* meshNode)
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
			if (ProcessSkin(pGeometry, meshNode)) {
				loadedSkin = true;
			}
			continue;
		default:
			break;
		}
	}
	
	return loadedSkin;
}

bool FBXCore::ProcessSkin(const FbxGeometry *pGeo, MeshNode* meshNode)
{
	const int VertexNum = pGeo->GetControlPointsCount();

	const int SkinNum = pGeo->GetDeformerCount(FbxDeformer::eSkin);

	for (int skinIndex = 0; skinIndex < SkinNum; ++skinIndex)
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

			std::string BoneName(pLinkBoneNode->GetName());

			BoneNode* boneNode = mNode->GetBoneNodeByName(BoneName);
			assert(boneNode);

			FbxAMatrix LinkedMatrix;
			pCluster->GetTransformLinkMatrix(LinkedMatrix);

			FbxAMatrix LinkedInverseMatrix;
			LinkedInverseMatrix = LinkedMatrix.Inverse();
			
			boneNode->SetInverseLocalTransfrom(Matrix4x4(LinkedInverseMatrix.Transpose()));
			
			const int ClusterIndicesNum = pCluster->GetControlPointIndicesCount();
			const int *ControlPointIndices = pCluster->GetControlPointIndices();
			const double *ControlPointWeights = pCluster->GetControlPointWeights();

			auto& PointArray = meshNode->GetPoints();
			for (int clusterIndex = 0; clusterIndex < ClusterIndicesNum; ++clusterIndex)
			{
				int ControlIndex = ControlPointIndices[clusterIndex];
				assert(ControlIndex < VertexNum);

				float ControlWeight = (float)ControlPointWeights[clusterIndex];
				if (ControlWeight < 0.00001f)
					continue;

				int boneID = boneNode->GetId();
				
				//LOG << "id : " << controlIndex << " weight : " << controlWeight << ENDN;
				if (!PointArray[ControlIndex].AddWeight(boneID, ControlWeight))
				{
					return false;
				}

			}
		}
	}
	return true;
}

void FBXCore::LoadVertexIndices(FbxMesh *pMesh, int faceIndex, Face &face)
{
	for (int i = 0; i < 3; ++i)
	{
		int vertexIndex = pMesh->GetPolygonVertex(faceIndex, i);
		face.SetVertexIndex(i, vertexIndex);
	}
}

void FBXCore::LoadNormals(FbxMesh * pMesh, int faceIndex, Face &face)
{
	if (pMesh->GetElementNormalCount() > 1) {
		LOG_ERROR("only one set of normal currenty supported");
	}
	FbxGeometryElementNormal* eNormal = pMesh->GetElementNormal();
	if (!eNormal) LOG_ERROR("element normal dosent supported");

	for (int facePointIndex = 0; facePointIndex < FACE_COMPONENT_NUM; ++facePointIndex)
	{
		int vertexIndex = pMesh->GetPolygonVertex(faceIndex, facePointIndex);
		vec3f normal;
		ConvertVector3FromElement(*eNormal, normal, (faceIndex * 3) + facePointIndex, vertexIndex);
		face.SetNormal(facePointIndex, normal);
	}
}

void FBXCore::LoadSTs(FbxMesh * pMesh, int faceIndex, Face & face)
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
		ConvertVector2FromElement(*eST, st, faceIndex, faceElement, vertexIndex);
		face.SetST(faceElement, st);
	}
}

void FBXCore::ConvertVector3FromElement(
	FbxLayerElementTemplate<FbxVector4> &element,
	vec3f &data,
	int facePointIndex,
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
			FbxVector4 fbxData = element.GetDirectArray().GetAt(facePointIndex);
			data[0] = static_cast<float>(fbxData[0]);
			data[1] = static_cast<float>(fbxData[1]);
			data[2] = static_cast<float>(fbxData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int indexId = element.GetIndexArray().GetAt(facePointIndex);
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

void FBXCore::ConvertVector2FromElement(
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

void FBXCore::ProcessPoses(FbxNode *pNode, FbxImporter *importer)
{
	//TODO : add Pose Matrix to each boneNode
}
