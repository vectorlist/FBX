#include <fbxmodel.h>
#include <log.h>
#include <fbxtool.h>

bool FBXModel::loadBoneNodes(FbxNode* pNode, BoneNode* parentBoneNode, MeshNode* parentMeshNode)
{
	//we dont need bonemesh here (pass it)
	//prepare new node for bone
	BoneNode* newBoneNode = NULL;
	MeshNode* newMeshNode = NULL;

	//get attribute FBX node
	FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
	if (pNodeAttr)
	{
		//get attrib type
		auto type = pNodeAttr->GetAttributeType();

		switch (type)
		{
		case fbxsdk::FbxNodeAttribute::eSkeleton:
			//TODO : set custom bone node tree() if we founded skeletal attr
			newBoneNode = loadBoneNode(pNode, parentBoneNode);
			if (newBoneNode) {
				LOG << "create bone node" << ENDN;
			}
			//LOG << "founded skeletion attr" << newBoneNode <<ENDN;
			break;
		case fbxsdk::FbxNodeAttribute::eMesh:
			//TODO create mesh node here
			newMeshNode = loadMeshNode(pNode, parentMeshNode);
			if (newMeshNode) {
				LOG << "create mesh node" << ENDN;
			}			
			break;

		default:
			//LOG << "failed to find attr" << ENDN;
			//pass it other attrib
			break;
		}
	}
	//recursive
	for (int childIndex = 0; childIndex < pNode->GetChildCount(); ++childIndex)
	{
		FbxNode* pChildNode = pNode->GetChild(childIndex);
		//Recursive newboneNode is as a parent node
		loadBoneNodes(pChildNode, newBoneNode, newMeshNode);
	}

	return true;
}

BoneNode* FBXModel::loadBoneNode(FbxNode* pNode, BoneNode* parent)
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

	//TODO : animation stack : pose ..etc

	//sacale , rot ..etc

	return boneNode;
}

MeshNode* FBXModel::loadMeshNode(FbxNode *pNode, MeshNode *parent)
{
	FbxMesh* pMesh = pNode->GetMesh();
	MeshNode* meshNode = NULL;

	//check triangles and pointer
	if (!pMesh && !pMesh->IsTriangleMesh()) {
		LOG << "FBX Mesh is not supported" << ENDN;
		return NULL;
	}

	meshNode = new MeshNode;
	//it should be (parent ---- child) at recursive
	mNode.addChildMeshNode(parent, meshNode);

	//test
	MeshNodeInfo.mMeshNodes.push_back(meshNode);
	MeshNodeInfo.mFbxNodes.push_back(pNode);
	
	std::string nodeName = pNode->GetName();
	meshNode->setName(nodeName);

	LOG << "set node name : " << nodeName << ENDN;

	const FbxAMatrix globalMatrix = pNode->EvaluateGlobalTransform();
	FbxAMatrix localMatrix;
	FbxVector4 position = pNode->LclRotation.Get();
	FbxVector4 rotation = pNode->LclRotation.Get();
	FbxVector4 scale = pNode->LclScaling.Get();

	localMatrix.SetTRS(position, rotation, scale);
	meshNode->mGlobalMarix = localMatrix;
	
	//build faces and vertices for mesh node
	buildMeshNode(pMesh, meshNode);
	//build renderable mesh
	FBXTool::buildMesh(meshNode, m_mesh);
	return nullptr;
}


void FBXModel::buildMeshNode(FbxMesh* pMesh, MeshNode* pMeshNode)
{
	int vertexNum = pMesh->GetControlPointsCount();
	//vertices.resize(vertexNum);
	VertexArray& vertices = pMeshNode->getVertices();
	vertices.resize(vertexNum);

	const FbxVector4* const verticeArrays = pMesh->GetControlPoints();
	for (unsigned int vertexIndex = 0; vertexIndex < vertexNum; ++vertexIndex)
	{
		vertices[vertexIndex].setPosition(verticeArrays[vertexIndex]);
	}
	LOG << "vertex num : " << vertexNum << ENDL;
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
	if (1) return;
	for (int faceIndex = 0; faceIndex < faceNums; ++faceIndex)
	{
		for (int i = 0; i < 3; ++i)
			LOG << faces[faceIndex].getVertexIndex(i) << " " << faces[faceIndex].getNormal(i)
			<< " " << faces[faceIndex].getST(i) << ENDN;
	}
}

void FBXModel::loadVertexIndices(FbxMesh *pMesh, int faceIndex, Face &triangle)
{
	for (int i = 0; i < 3; ++i)
	{
		int vertexIndex = pMesh->GetPolygonVertex(faceIndex, i);
		triangle.setVertexIndex(i, vertexIndex);
	}
}

void FBXModel::loadNormals(FbxMesh * pMesh, int faceIndex, Face & face)
{
	if (pMesh->GetElementNormalCount() > 1) {
		LOG_ERROR("only one set of normal currenty supported");
	}
	FbxGeometryElementNormal* eNormal = pMesh->GetElementNormal();
	if (!eNormal) LOG_ERROR("element normal dosent supported");

	for (int faceElement = 0; faceElement < FACE_POINT_MAX; ++faceElement)
	{
		int vertexIndex = pMesh->GetPolygonVertex(faceIndex, faceElement);
		vec3f normal;
		convert3fDataFromElement(*eNormal, normal, (faceIndex * 3) + faceElement, vertexIndex);
		face.setNormal(faceElement, normal);
	}
}

void FBXModel::loadSTs(FbxMesh * pMesh, int faceIndex, Face & face)
{
	if (pMesh->GetElementUVCount() != 0) {
		//LOG_ERROR("mesh must have one sets of uvs, this mesh has %d uv sets.", pMesh->GetElementUVCount());
	}

	FbxGeometryElementUV* eST = pMesh->GetElementUV();
	if (!eST) LOG_ERROR("failed to load uv elements");

	for (int faceElement = 0; faceElement < FACE_POINT_MAX; ++faceElement)
	{
		int vertexIndex = pMesh->GetPolygonVertex(faceIndex, faceElement);
		vec2f st;
		convert2fFromElement(*eST, st, faceIndex, faceElement, vertexIndex);
		face.setST(faceElement, st);
	}
}

void FBXModel::convert3fDataFromElement(
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
			//data[3] = static_cast<float>(fbxData[3]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = element.GetIndexArray().GetAt(vertexIndex);
			FbxVector4 fbxData = element.GetDirectArray().GetAt(id);
			data[0] = static_cast<float>(fbxData[0]);
			data[1] = static_cast<float>(fbxData[1]);
			data[2] = static_cast<float>(fbxData[2]);
			//data[3] = static_cast<float>(fbxData[3]);
		}
		break;
		default:
			LOG_ERROR("Trying to load %s with an unsupported reference mode\n", element.GetName());
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
			LOG_ERROR("Trying to load %s with an unsupported reference mode\n", element.GetName());
			break;
		}
	}
	default:
		// TODO eDirect is currently not supported - should be easy enough to add this in?
		//wxLogDebug("Trying to load %s with an unsupported mapping mode\n", element.GetName());
		break;
	}
}

void FBXModel::convert2fFromElement(
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
			LOG_ERROR("Trying to load %s with an unsupported reference mode\n", element.GetName());
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
			LOG_ERROR("Trying to load %s with an unsupported reference mode\n", element.GetName());
			break;
		}
	}
	break;
	default:
		LOG_ERROR("Trying to load %s with an unsupported mapping mode\n", element.GetName());
		break;
	}
}


const unsigned int FBXModel::GetUVVertexIndex(
	const unsigned int triangleIndex,
	const unsigned int triangleCornerId) const
{
	return triangleIndex * 3 + triangleCornerId; // Triangle index * num verts in a triangle + current vertex corner in the triangle
}