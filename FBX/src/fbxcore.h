#pragma once
#include <fbxsdk.h>
#include <GL/glew.h>
#include <vec2f.h>
#include <vec3f.h>
#include <vec4f.h>
#include <vector>
#include <string>
#include <map>
#include <face.h>
#include <point.h>
#include <node.h>

using namespace fbxsdk;

class FBXCore
{
public:
	FBXCore(const std::string &filename);
	~FBXCore();
	
	std::shared_ptr<Node> mNode;
	FbxImporter* mImporter;

	bool		ProcessNodes(FbxNode* pNode, BoneNode* parentBoneNode, MeshNode* parentMeshNode);

	BoneNode*	ProcessBoneNode(FbxNode* pNode, BoneNode* parent);
	MeshNode*	ProcessMeshNode(FbxNode* pNode, MeshNode* parent);

	bool		ProcessSkins(FbxNode* pNode, MeshNode* meshNode);
	bool		ProcessSkin(const FbxGeometry *pGeo, MeshNode* meshNode);
	
	void		BuildMeshNode(FbxMesh* pMesh, MeshNode* pMeshNode);

	void		LoadVertexIndices(FbxMesh* pMesh, int faceIndex, Face &face);

	void		LoadNormals(FbxMesh* pMesh, int faceIndex, Face &face);
	void		LoadSTs(FbxMesh* pMesh, int faceIndex, Face &face);

	void		ConvertVector3FromElement(
					FbxLayerElementTemplate<FbxVector4> &element,
					vec3f								&data,
					int									FacePointIndex,
					int									VertexIndex);

	void		ConvertVector2FromElement(
					FbxLayerElementTemplate<FbxVector2> &element,
					vec2f								&data,
					int									triangleIndex,
					int									triangleCornerId,
					int									vertexIndex);

	const unsigned int GetUVVertexIndex(
		const unsigned int triangleIndex,
		const unsigned int triangleCornerId) const;

	void		ProcessPoses(FbxNode* pNode, FbxImporter* importer);

};

//// Get the matrix of the given pose
//FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
//{
//	FbxAMatrix lPoseMatrix;
//	FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);
//
//	memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));
//
//	return lPoseMatrix;
//}
//
//// Get the geometry offset to a node. It is never inherited by the children.
//FbxAMatrix GetGeometry(FbxNode* pNode)
//{
//	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
//	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
//	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
//
//	return FbxAMatrix(lT, lR, lS);
//}

//const FbxArray<FbxPose *> & lPoseArray = gSceneContext->GetPoseArray();
//for (int lPoseIndex = 0; lPoseIndex < lPoseArray.GetCount(); ++lPoseIndex)
//{
//	if (lPoseArray[lPoseIndex]->IsBindPose())
//	{
//		glutAddMenuEntry(lPoseArray[lPoseIndex]->GetName(), lPoseIndex);
//		lBindPoseCount++;
//	}
//}