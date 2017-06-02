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
	bool processNodes(FbxNode* pNode, BoneNode* parentBoneNode, MeshNode* parentMeshNode);
	BoneNode* processBoneNode(FbxNode* pNode, BoneNode* parent);
	MeshNode* processMeshNode(FbxNode* pNode, MeshNode* parent);

	/** Info : create skin vertex data to mehs node (bone id, bone wieght)*/
	bool processSkins(FbxNode* pNode, MeshNode* meshNode);
	
	bool processSkin(const FbxGeometry *pGeo, MeshNode* meshNode);
	
	void buildMeshNode(FbxMesh* pMesh, MeshNode* pMeshNode);

	void loadVertexIndices(FbxMesh* pMesh, int faceIndex,
		Face &triangle);

	void loadNormals(FbxMesh* pMesh, int faceIndex, Face &face);
	void loadSTs(FbxMesh* pMesh, int faceIndex, Face &face);

	void convert3fDataFromElement(
		FbxLayerElementTemplate<FbxVector4> &element,
		vec3f &data,
		int facePointIndex,
		int vertexIndex);

	void convert2fFromElement(
		FbxLayerElementTemplate<FbxVector2> &element,
		vec2f &data,
		int triangleIndex,
		int triangleCornerId,
		int vertexIndex);

	const unsigned int GetUVVertexIndex(
		const unsigned int triangleIndex,
		const unsigned int triangleCornerId) const;

};

