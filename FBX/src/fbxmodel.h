

#pragma once
#include <fbxsdk.h>
#include <GL/glew.h>
#include <vec2f.h>
#include <vec3f.h>
#include <vec4f.h>
#include <vector>
#include <string>
#include <fbxmesh.h>
#include <map>
#include <array>

#include <face.h>
#include <vertex.h>
#include <node.h>
#include <fbxdevice.h>

class FBXModel
{
public:
	FBXModel(const std::string &filename, GLuint shader);

	GLuint m_shader;
	FbxManager* m_manager;
	FbxImporter* m_importer;
	FbxScene* m_scene;
	FbxNode* mRootNode;
	FBXDevice* mDevice;

	void render(GLuint shader) { m_mesh.render(shader); }

	bool buildSkin(FbxNode* pNode, MeshNode* meshNode);
	bool loadSkin(
		const FbxGeometry *pGeo,
		MeshNode* meshNode);
	
	//vbo mesh
	FBXMesh m_mesh;
	//node mesh
	Node mNode;
	/*--------------- ANIMATION --------------*/
	//AnimationSample mAnimation;
	bool createAnimationSamples(Node &node);

	/*--------------- EXT FUNC --------------*/
	struct 
	{
		//matched FbxNode and MeshNode pointers
		std::vector<MeshNode*> mMeshNodes;
		std::vector<FbxNode*> mFbxNodes;
	}MeshNodeInfo;

	bool loadNodes(
		FbxNode* pNode,
		BoneNode* parentBoneNode,
		MeshNode* parentMeshNode);

	BoneNode* loadBoneNode(FbxNode* pNode, BoneNode* parent);
	MeshNode* loadMeshNode(FbxNode* pNode, MeshNode* parent);





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

