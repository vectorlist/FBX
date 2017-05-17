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

	//Build BoneNodes Mesh(replace custom node to fbxnode)
	loadBoneNodes(m_rootNode, mNode.getBoneNodeRoot(), mNode.getMeshNodeRoot());

	//BUILD Mesh(for ID[4], weight[4] per vertex)

	//buildModel(m_rootNode);
	////build Skin after BoneNode
	//buildSkin(m_rootNode);

	//FBXTool::buildMesh(vertices, faces, m_mesh);


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

//void FBXModel::buildModel(FbxNode* pRootNode)
//{
//	
//	int childNum = pRootNode->GetChildCount();
//	for (int i = 0; i < childNum; ++i)
//	{
//		auto* child = pRootNode->GetChild(i);
//		auto* pMesh = child->GetMesh();
//		if (!pMesh) return;
//
//		
//		buildFaces(pMesh);
//		
//	}
//}

//bool FBXModel::buildSkin(FbxNode* pNode)
//{
//	bool loadedSkin = false;
//
//	int childNum = pNode->GetChildCount();
//	for (int i = 0; i < childNum; ++i)
//	{
//		auto* child = pNode->GetChild(i);
//		//auto* pMesh = child->GetMesh();
//		//if (!pMesh) continue;
//		if (!child) continue;
//		FbxGeometry* const pGeometry = child->GetGeometry();
//		if (!pGeometry) {
//			//LOG << "Unssuport skin type used"<< ENDN;
//			continue;
//		}
//
//		const FbxSkin* const pSkin = static_cast<FbxSkin*>(pGeometry->GetDeformer(0, FbxDeformer::eSkin));
//		if (!pSkin) {
//			//LOG << "skin deformer is null" << ENDN;
//			continue;
//		}
//
//		const FbxSkin::EType skinType = pSkin->GetSkinningType();
//		switch (skinType)
//		{
//		case FbxSkin::eLinear:
//			LOG << "linear" << ENDL;
//		case FbxSkin::eRigid:
//			LOG << "rgid" << ENDN;
//		case FbxSkin::eDualQuaternion:
//			LOG << "dual" << ENDN;
//			if (loadSkin(pGeometry, vertices)) {
//				loadedSkin = true;
//			}
//			
//			continue;
//		default:
//			LOG << "default break" << ENDL;
//			break;
//		}
//	}
//	
//	return loadedSkin;
//}
//
//bool FBXModel::loadSkin(const FbxGeometry *pGeo, std::vector<Vertex> &vertices)
//{
//	const int vertexNum = pGeo->GetControlPointsCount();
//	//LOG << vertexNum << ENDN;
//
//	const int skinNum = pGeo->GetDeformerCount(FbxDeformer::eSkin);
//	//LOG << skinNum << ENDN;
//
//	for (int skinIndex = 0; skinIndex < skinNum; ++skinIndex)
//	{
//		FbxSkin* pSkin = static_cast<FbxSkin*>(pGeo->GetDeformer(skinIndex, FbxDeformer::eSkin));
//		const int clusterNum = pSkin->GetClusterCount();
//		LOG << "clusters : " << clusterNum << ENDN;
//		for (int clusterIndex = 0; clusterIndex < clusterNum; ++clusterIndex)
//		{
//			FbxCluster* pCluster = pSkin->GetCluster(clusterIndex);
//			const auto clusterMode = pCluster->GetLinkMode();
//			switch (clusterMode)
//			{
//			case FbxCluster::eNormalize:
//				LOG << "normalize mode" << ENDL;
//				/*break;*/
//			case FbxCluster::eTotalOne:
//				LOG << "total one mode" << ENDL;
//				break;
//			default:
//				LOG << "additive mode" << ENDL;
//				//break;
//			}
//
//			FbxNode* const pLinkBoneNode = pCluster->GetLink();
//			if (!pLinkBoneNode) {
//				LOG << "failed to find linked bone node" << ENDN;
//				continue;
//			}
//
//			//Importance get BoneNode by name
//			std::string temp(pLinkBoneNode->GetName());
//			LOG << temp << ENDN;
//			//must be boneNodes created
//			BoneNode* boneNode = mNode.getBoneNodeByName(temp);
//
//			//transform
//			FbxAMatrix boneRefMatrix;
//			pCluster->GetTransformLinkMatrix(boneRefMatrix);
//
//			FbxAMatrix boneRefMatrixInverse;
//			boneRefMatrixInverse = boneRefMatrix.Inverse();
//			//set global matrix (outside)
//
//			//set ref inverse transform
//			boneNode->mInverseTransform = boneRefMatrixInverse;
//			
//
//			const int clusterIndicesNum = pCluster->GetControlPointIndicesCount();
//			const int *controlPointIndices = pCluster->GetControlPointIndices();
//			const double *controlPointWeights = pCluster->GetControlPointWeights();
//
//			for (int clusterIndex = 0; clusterIndex < clusterIndicesNum; ++clusterIndex)
//			{
//				int controlIndex = controlPointIndices[clusterIndex];
//
//				//Error out of index control points nums
//				if (controlIndex >= vertexNum) {
//					controlIndex = vertexNum - 1;
//				}
//
//				const float controlWeight = (float)controlPointWeights[clusterIndex];
//				if (controlWeight < 0.00001f) continue;
//
//				
//				//into BoneNode ID , control point weight
//				int boneID = boneNode->getID();
//
//				LOG << "vertices : " << controlIndex <<
//					" bone id" << boneID << " weight : " << controlWeight << ENDN;
//				vertices[controlIndex].addWeight(boneID, controlWeight);
//
//			}
//		}
//	}
//	return false;
//
//}
//
//

