

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
#include <vertex.h>
#include <node.h>
#include <fbxdevice.h>

using namespace fbxsdk;

class FBXCore
{
public:
	FBXCore(const std::string &filename);

	GLuint mSahder;
	FbxNode* mRootNode;
	FBXDevice* mDevice;
	
	Node mNode;

	bool processSkinNode();
	bool processSkins(FbxNode* pNode, MeshNode* meshNode);
	bool processSkin(const FbxGeometry *pGeo, MeshNode* meshNode);
	
	/*--------------- ANIMATION --------------*/
	//AnimationSample mAnimation;
	bool createAnimationSamples(Node &node);

	/*--------------- EXT FUNC --------------*/

	bool processNodes(
		FbxNode* pNode,
		BoneNode* parentBoneNode,
		MeshNode* parentMeshNode);

	BoneNode* processBoneNode(FbxNode* pNode, BoneNode* parent);
	MeshNode* processMeshNode(FbxNode* pNode, MeshNode* parent);

	//Temportarty
	struct 
	{
		std::vector<MeshNode*> meshNode;
		std::vector<FbxNode*> fbxNode;
	}tempMeshNode;


	/*--------- BUILT IN MESH FUNC --------*/
	//build Meshnode faces and vetices(we need resize)
	void buildMeshNode(FbxMesh* pMesh, MeshNode* pMeshNode);

	void loadVertexIndices(FbxMesh* pMesh, int faceIndex,
		Face &triangle);

	void loadNormals(FbxMesh* pMesh, int faceIndex, Face &face);
	void loadSTs(FbxMesh* pMesh, int faceIndex, Face &face);

	void convert3fDataFromElement(
		FbxLayerElementTemplate<FbxVector4> &element,
		vec3f &data,
		int triangleCornerId,
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

	void bakeNodeTransform(FbxNode* pNode) const;
};

