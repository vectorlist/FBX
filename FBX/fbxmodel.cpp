#include <fbxmodel.h>
#include <log.h>
#include <fbxtool.h>

FBXModel::FBXModel(const char *filename, GLuint shader)
	: m_filename(filename), m_shader(shader)
{
	m_manager = FbxManager::Create();
	if (m_manager)
	{
		LOG << "Autodesk FBX SDK version : " << m_manager->GetVersion() << ENDN;
	}
	FbxIOSettings* ioSetting = FbxIOSettings::Create(m_manager, IOSROOT);
	m_manager->SetIOSettings(ioSetting);

	m_importer = FbxImporter::Create(m_manager, "");
	LOG << "Loading : " << m_filename << ENDL;
	bool init = m_importer->Initialize(m_filename.c_str(), -1, m_manager->GetIOSettings());
	if (!init)
	{
		std::string err = m_importer->GetStatus().GetErrorString();
		LOG_ERROR(err + " : " + m_filename);
	}

	m_scene = FbxScene::Create(m_manager, "scene");
	if (!m_scene)
	{
		LOG_ASSERT("failed to create scene");
	}
	m_importer->Import(m_scene);
	setSceneSystem(m_scene);
	m_importer->Destroy();

	m_rootNode = m_scene->GetRootNode();

	LOG << "Node Child Nums : " << m_rootNode->GetChildCount() << ENDL;

	m_scene->FillAnimStackNameArray(m_animStatclNameArrays);

	for (int i = 0; i < m_animStatclNameArrays.Size(); ++i)
	{
		LOG << "Animation Statck : " << m_animStatclNameArrays[i]->Buffer() << ENDL;
	}

	//convert triangles
	FbxGeometryConverter converter(m_manager);
	converter.Triangulate(m_scene, true);


	buildModel(m_rootNode);
	buildSkin(m_rootNode);

	FBXTool::buildMesh(vertices, faces, m_mesh);
}

void FBXModel::setSceneSystem(FbxScene *pScene)
{
	FbxAxisSystem sceneAxis = pScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem oglAxis(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd,
		FbxAxisSystem::eRightHanded);

	if (sceneAxis == oglAxis) {

		oglAxis.ConvertScene(pScene);
	}
}

void FBXModel::buildModel(FbxNode* pRootNode)
{
	
	int childNum = pRootNode->GetChildCount();
	for (int i = 0; i < childNum; ++i)
	{
		auto* child = pRootNode->GetChild(i);
		auto* pMesh = child->GetMesh();
		if (!pMesh) return;

		
		buildMeshTri(pMesh);
		
	}
}

void FBXModel::buildMeshTri(FbxMesh *pMesh)
{
	int vertexNum = pMesh->GetControlPointsCount();
	vertices.resize(vertexNum);
	const FbxVector4* const verticeArrays = pMesh->GetControlPoints();
	for (unsigned int vertexIndex = 0; vertexIndex < vertexNum; ++vertexIndex)
	{
		vertices[vertexIndex].setPosition(verticeArrays[vertexIndex]);
	}
	LOG << "vertex num : " << vertexNum << ENDL;
	int faceNums = pMesh->GetPolygonCount();
	faces.resize(faceNums);
	for (int faceIndex = 0; faceIndex < faceNums; ++faceIndex)
	{
		Face &face = faces[faceIndex];
		loadVertexIndices(pMesh, faceIndex, face);
		loadNormals(pMesh, faceIndex, face);
		loadSTs(pMesh, faceIndex, face);
	}

	if (1) return;
	for (int faceIndex = 0; faceIndex < faceNums; ++faceIndex)
	{
		for(int i =0; i < 3; ++i)
			LOG << faces[faceIndex].getVertexIndex(i) << " " << faces[faceIndex].getNormal(i)
			<< " " << faces[faceIndex].getST(i)<< ENDN;
	}
}

void FBXModel::loadVertexIndices(FbxMesh *pMesh, int triangleIndex, Face &triangle)
{
	for (int i = 0; i < 3; ++i)
	{
		int vertexIndex = pMesh->GetPolygonVertex(triangleIndex, i);
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

	for (int faceElement = 0; faceElement < FACE_ELEMENT_MAX; ++faceElement)
	{
		int vertexIndex = pMesh->GetPolygonVertex(faceIndex, faceElement);
		vec3f normal;
		convert3fDataFromElement(*eNormal, normal, (faceIndex * 3 ) + faceElement, vertexIndex);
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

	for (int faceElement = 0; faceElement < FACE_ELEMENT_MAX; ++faceElement)
	{
		int vertexIndex = pMesh->GetPolygonVertex(faceIndex, faceElement);
		vec2f st;
		convert2fFromElement(*eST, st, faceIndex, faceElement, vertexIndex);
		face.setST(faceElement, st);
	}
}

bool FBXModel::buildSkin(FbxNode* pNode)
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
			LOG << "linear" << ENDL;
		case FbxSkin::eRigid:
			LOG << "rgid" << ENDN;
		case FbxSkin::eDualQuaternion:
			LOG << "dual" << ENDN;
			if (loadSkin(pGeometry, vertices)) {
				loadedSkin = true;
			}
			
			continue;
		default:
			LOG << "default break" << ENDL;
			break;
		}
	}
	
	return loadedSkin;
}

bool FBXModel::loadSkin(const FbxGeometry *pGeo, std::vector<Vertex> &vertices)
{
	const int vertexNum = pGeo->GetControlPointsCount();
	LOG << vertexNum << ENDN;

	const int skinNum = pGeo->GetDeformerCount(FbxDeformer::eSkin);
	LOG << skinNum << ENDN;

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
			std::string temp(pLinkBoneNode->GetName());
			LOG << temp << ENDN;

			const int clusterIndicesNum = pCluster->GetControlPointIndicesCount();
			const int *controlPointIndices = pCluster->GetControlPointIndices();
			const double *controlPointWeights = pCluster->GetControlPointWeights();

			for (int clusterIndex = 0; clusterIndex < clusterIndicesNum; ++clusterIndex)
			{
				int controlIndex = controlPointIndices[clusterIndex];

				if (controlIndex >= vertexNum) {
					controlIndex = vertexNum - 1;
				}

				const float controlWeight = (float)controlPointWeights[clusterIndex];
				if (controlWeight < 0.00001f) continue;

				//vertices[controlIndex].addWeight()

				/*LOG << controlIndex << ENDL;*/
			}
		}
	}
	return false;

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
			//data[3] = static_cast<float>(fbxData[3]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int indexId = element.GetIndexArray().GetAt(triangleCornerId);
			FbxVector4 fbxData = element.GetDirectArray().GetAt(indexId);
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

