#include <fbxmodel.h>
#include <log.h>


FBXModel::FBXModel(const char *filename, GLuint shader)
	: m_filename(filename), m_shader(shader)
{
	m_manager = FbxManager::Create();
	FbxIOSettings* ioSetting = FbxIOSettings::Create(m_manager, IOSROOT);
	m_manager->SetIOSettings(ioSetting);

	m_importer = FbxImporter::Create(m_manager, "");
	m_importer->Initialize(m_filename.c_str(), -1, m_manager->GetIOSettings());
	if (!m_importer) LOG_ASSERT(std::string("failed to import : ")+m_filename.c_str());

	m_scene = FbxScene::Create(m_manager, "scene");
	
	m_importer->Import(m_scene);
	setSceneSystem(m_scene);
	m_importer->Destroy();

	m_rootNode = m_scene->GetRootNode();

	LOG << "Load FBX : " <<filename << ENDL;
	LOG << "Node Child Nums : " << m_rootNode->GetChildCount() << ENDL;

	m_scene->FillAnimStackNameArray(m_animStatclNameArrays);

	for (int i = 0; i < m_animStatclNameArrays.Size(); ++i) {
		LOG << "Animation Statck : " << m_animStatclNameArrays[i]->Buffer() << ENDL;
	}

	//convert triangles
	FbxGeometryConverter converter(m_manager);
	converter.Triangulate(m_scene, true);

	buildModel(m_rootNode);

	FbxSkeleton* skeleton = (FbxSkeleton*)m_rootNode->GetNodeAttribute();

	//auto name = skeleton->GetAttributeType();
	buildSkin(m_rootNode);
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

		//buildMesh(pMesh, m_mesh);
		//buildBone(pMesh, m_mesh);
		//buildMeshCP(pMesh, m_mesh);
		buildMeshTri(pMesh);
		//buildSkin(pRootNode);
	}
}

void FBXModel::buildMesh(FbxMesh * pMesh, FBXMesh &mesh)
{
	const FbxGeometryElementNormal* eNormal = pMesh->GetElementNormal();
	const FbxGeometryElementUV* eUV = pMesh->GetElementUV();

	hasNormal = eNormal;
	hasUV = eUV;
	
	if (eNormal->GetMappingMode() == FbxGeometryElement::eNone) {
		hasNormal = false;
	}
	if (hasNormal && eNormal->GetMappingMode() != FbxGeometryElement::eByControlPoint) {
		byControlPoint = false;
	}
	if (eUV->GetMappingMode() == FbxGeometryElement::eNone) {
		hasUV = false;
	}
	if (hasUV && eUV->GetMappingMode() != FbxGeometryElement::eByControlPoint) {
		byControlPoint = false;
	}
	
	//face
	const int faceNum = pMesh->GetPolygonCount();
	int polygonVertexNum = pMesh->GetControlPointsCount();
	if (!byControlPoint) {
		polygonVertexNum = faceNum * 3;
	}
	else {
		LOG_ERROR("by control point mesh incomplete..");
	}

	//UV
	const char* uvName = nullptr;
	if (hasUV) {
		FbxStringList uvNames;
		pMesh->GetUVSetNames(uvNames);
		uvName = uvNames[0];
	}
	//mesh.vertices.resize(polygonVertexNum);
	mesh.indices.resize(faceNum * 3);
	mesh.controlIndices.resize(faceNum * 3);
	//mesh.indices.resize(pMesh->GetControlPointsCount());
	LOG << "by control point : " << (byControlPoint ? "true" : "false") << ENDN;
	LOG << "has UV : " << (hasUV? "true" : "false") << ENDN;
	const FbxVector4* controlVertex = pMesh->GetControlPoints();
	
	uint32_t vertexCount = 0;
	FbxVector4 fVertex;
	FbxVector4 fNormal;
	FbxVector2 fST;
	for (int faceIndex = 0; faceIndex < faceNum; ++faceIndex)
	{
		for (int vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
		{
			FBXVertex vertex;
			const int controlPointIndex = pMesh->GetPolygonVertex(faceIndex, vertexIndex);
			//LOG << controlPointIndex << ENDN;
			
			mesh.indices[faceIndex * 3 + vertexIndex] = vertexCount;
			mesh.controlIndices[faceIndex * 3 + vertexIndex] = controlPointIndex;
			
			fVertex = controlVertex[controlPointIndex];
			vertex.position.x = (float)fVertex[0];
			vertex.position.y = (float)fVertex[1];
			vertex.position.z = (float)fVertex[2];

			if (hasNormal) {
				pMesh->GetPolygonVertexNormal(faceIndex, vertexIndex, fNormal);
				vertex.normal.x = (float)fNormal[0];
				vertex.normal.y = (float)fNormal[1];
				vertex.normal.z = (float)fNormal[2];
			}
			if (hasUV) {
				bool unMappedST;
				pMesh->GetPolygonVertexUV(faceIndex, vertexIndex, uvName, fST, unMappedST);
				vertex.st.x = (float)fST[0];
				vertex.st.y = (float)fST[1];
			}

			mesh.vertices.push_back(vertex);
			++vertexCount;
		}
	}
	mesh.buildBuffer();
}

void FBXModel::buildMeshCP(FbxMesh *pMesh, FBXMesh &mesh)
{
	int polygonVertexCount = pMesh->GetControlPointsCount();
	const FbxVector4* controlPoints = pMesh->GetControlPoints();
	const FbxGeometryElementNormal* eNormal = pMesh->GetElementNormal();
	const FbxGeometryElementUV* eUV = pMesh->GetElementUV();
	
	FbxArray<FbxVector4> normals;
	//pMesh->normal(normals);
	
	LOG << "normals : "<< pMesh->GetElementNormalCount() << ENDL;
	
	mesh.vertices.resize(polygonVertexCount);
	for (int index = 0; index < polygonVertexCount; ++index)
	{
		FbxVector4 pos = controlPoints[index];
		mesh.vertices[index].position.x = (float)pos[0];
		mesh.vertices[index].position.y = (float)pos[1];
		mesh.vertices[index].position.z = (float)pos[2];

		int normalIndex = index;
		if (eNormal->GetReferenceMode() == FbxLayerElement::eIndexToDirect) {
			normalIndex = eNormal->GetIndexArray().GetAt(index);
		}
		if (eNormal->GetReferenceMode() == FbxLayerElement::eDirect)
		{
			LOG << " direct" << eNormal->GetDirectArray().GetAt(index)[0]<<ENDN;
			normalIndex = eNormal->GetIndexArray().GetAt(index);
		}
		FbxVector4 normal = eNormal->GetDirectArray().GetAt(index * 3);
		//FbxVector4 normal =;
		
		//normal = eNormal->GetIndexArray().GetAt(normalIndex);
		mesh.vertices[index].normal.x = (float)normal[0];
		mesh.vertices[index].normal.y = (float)normal[1];
		mesh.vertices[index].normal.z = (float)normal[2];
	}

	FbxVector4 normal;
	FbxVector2 st;
	int polygonNum = pMesh->GetPolygonCount();
	int vertexNum = 3;
	mesh.indices.resize(polygonNum * vertexNum);
	int vertexCount = 0;
	for (int i = 0; i < polygonNum; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			const int index = pMesh->GetPolygonVertex(i, j);
			mesh.indices[i * 3 + j] = index;

			
			/*mesh.vertices[index].position.x = (float)pos[0];
			mesh.vertices[index].position.y = (float)pos[1];
			mesh.vertices[index].position.z = (float)pos[2];*/

			pMesh->GetPolygonVertexNormal(i, j, normal);
			normal = eNormal->GetDirectArray().GetAt(i * 3 +j);
			mesh.vertices[index].normal.x = (float)normal[0];
			mesh.vertices[index].normal.y = (float)normal[1];
			mesh.vertices[index].normal.z = (float)normal[2];
		}
	}
	mesh.buildBuffer();
}

void FBXModel::buildBone(FbxMesh *pMesh, FBXMesh &dMesh)
{
	int deformerNum = pMesh->GetDeformerCount();
	if (!deformerNum) LOG_ERROR("mesh hasnt deformers");

	/*for (int i = 0; i < deformerNum; ++i)
	{
		FbxDeformer* pDeformer = pMesh->GetDeformer(i);
		FbxSkin* pSkin = (FbxSkin*)pDeformer;

	}*/
	int skinNum = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	//LOG << "skins : " << skinNum << ENDL;
	FbxSkin* pSkin = (FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin);

	int boneNum = pSkin->GetClusterCount();

	LOG << dMesh.vertices.size() << " : " << pMesh->GetControlPointsCount() << ENDL;
	int vertexNum = pMesh->GetControlPointsCount();
	LOG << vertexNum << ENDL;
	bones.resize(vertexNum + 2);
	//boneDatas.resize(boneNum);
	for (int i = 0; i < boneNum; ++i)
	{
		FbxCluster* pBone = pSkin->GetCluster(i);
		if (!pBone->GetLink()) {
			continue;
		}

		const char* boneName = pBone->GetLink()->GetName();
		int *boneIncdies = pBone->GetControlPointIndices();
		double *boneWeights = pBone->GetControlPointWeights();
		int boneIndicesNum = pBone->GetControlPointIndicesCount();
		

		int index = 0;

		LOG << boneName << " " << "num of index " << boneIndicesNum << ENDN;
		
		std::string name = boneName;
		if (boneMapping.find(name) == boneMapping.end())
		{
			index = numBones;
			numBones++;
			boneMapping[name] = index;
		}
		else {
			index = boneMapping[name];
		}
		BoneData boneData;

		for (int j = 0; j < boneIndicesNum; ++j)
		{
			int indiceID = boneIncdies[j];
			//LOG << indiceID << ENDL;
			bones[indiceID].add(index, boneWeights[j]);
		}
	}

	
	for (int i = 0; i < bones.size(); ++i)
	{
		LOG << "index : " << i << ENDL;
		
		for (int j = 0; j < 4; ++j)
		{
			LOG << "boneID : " << bones[i].IDs[j] << " weight : " << bones[i].weights[j]<< ENDN;
		}
	}

	for (int i = 0; i < dMesh.indices.size(); ++i)
	{
		//LOG << dMesh.indices[i] << ENDL;
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
	for (int triIndex = 0; triIndex < faceNums; ++triIndex)
	{
		Face &face = faces[triIndex];
		loadVertexIndices(pMesh, triIndex, face);
		loadNormals(pMesh, triIndex, face);
		loadSTs(pMesh, triIndex, face);
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
		LoadVector2VertexElement(*eST, st, faceIndex, faceElement, vertexIndex);
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
		FbxSkin* pSkin = (FbxSkin*)pGeo->GetDeformer(skinIndex, FbxDeformer::eSkin);
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
				break;
			case FbxCluster::eTotalOne:
				LOG << "total one mode" << ENDL;
				break;
			default:
				LOG << "additive mode" << ENDL;
				break;
			}

			FbxNode* const pLinkBoneNode = pCluster->GetLink();
			if (!pLinkBoneNode) {
				LOG << "failed to find linked bone node" << ENDN;
			}
			std::string temp(pLinkBoneNode->GetName());
			LOG << temp << ENDN;
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

void FBXModel::LoadVector2VertexElement(
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
//for (int vertexIndex = 0; vertexIndex < dMesh.vertices.size(); ++vertexIndex)
//{
//	int index = pBone->GetControlPointIndices()[vertexIndex];
//	if (index >= vertexNum) continue;
//	float weight = (float)pBone->GetControlPointWeights()[vertexIndex];
//	if (weight == 0.0f) continue;
//	//LOG << "index : " << index << " weight : " << weight << ENDL;
//
//	bool foundSpot = false;
//	for (int spot = 0; spot < 4; ++spot)
//	{
//		dMesh.vertices[vertexIndex].boneID[spot] = boneID;
//		dMesh.vertices[vertexIndex].boneWeight[spot] = weight;
//		foundSpot = true;
//		break;
//	}
//
//	LOG << "vertex : " << vertexIndex << ENDL;
//	LOG << dMesh.vertices[vertexIndex].boneID[0] << ENDL;
//	LOG << dMesh.vertices[vertexIndex].boneID[1] << ENDL;
//	LOG << dMesh.vertices[vertexIndex].boneID[2] << ENDL;
//	LOG << dMesh.vertices[vertexIndex].boneID[3] << ENDL;
//}
