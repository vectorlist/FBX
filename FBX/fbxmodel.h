

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

//
#include <face.h>
#include <vertex.h>
#define BONE_MAX_PER_VERTEX		4

struct VertexBoneData
{
	std::array<int, BONE_MAX_PER_VERTEX> IDs;
	std::array<float, BONE_MAX_PER_VERTEX> weights;

	void add(int boneID, float weight)
	{
		for (int i = 0; i < BONE_MAX_PER_VERTEX; ++i)
		{
			if (weights[i] == 0.0f)
			{
				IDs[i] = boneID;
				weights[i] = weight;
				return;
			}
		}
	}
};

struct BoneData
{
	int boneIndex;
	std::map<int, float> boneMap;
};

class FBXModel
{
public:
	FBXModel(const char* filename, GLuint shader);

	std::string m_filename;
	GLuint m_shader;
	FbxManager* m_manager;
	FbxImporter* m_importer;
	FbxScene* m_scene;
	FbxNode* m_rootNode;

	mutable FbxTime m_frameTime, m_start, m_stop, m_currentTime;
	mutable FbxTime m_cacheStart, m_cacheStop;
	//built in
	FbxArray<FbxString*> m_animStatclNameArrays;
	//std::vector<FBXMesh> meshes;

	void setSceneSystem(FbxScene* pScene);

	FBXMesh m_mesh;
	void buildModel(FbxNode* pRootNode);
	void buildMesh(FbxMesh* pMesh, FBXMesh &mesh);
	void buildMeshCP(FbxMesh* pMesh, FBXMesh &mesh);

	void buildBone(FbxMesh* pMesh, FBXMesh &dMesh);

	void render(GLuint shader) { m_mesh.render(shader); }

	bool hasNormal;
	bool hasUV;
	bool byControlPoint;


	//bone
	std::map<std::string, int> boneMapping;
	std::vector<VertexBoneData> bones;
	std::vector<BoneData> boneDatas;
	int numBones =0;
	

	//Vertex Tri
	std::vector<Face> faces;
	std::vector<Vertex> vertices;

	void buildMeshTri(FbxMesh* pMesh);
	void loadVertexIndices(FbxMesh* pMesh, int triangleIndex,
		Face &triangle);

	void loadNormals(FbxMesh* pMesh, int faceIndex, Face &face);
	void loadSTs(FbxMesh* pMesh, int faceIndex, Face &face);

	bool buildSkin(FbxNode* pNode);
	bool loadSkin(const FbxGeometry *pGeo, std::vector<Vertex> &vertices);

	void convert3fDataFromElement(
		FbxLayerElementTemplate<FbxVector4> &element,
		vec3f &data,
		int triangleCornerId,
		int vertexIndex);

	void LoadVector2VertexElement(
		FbxLayerElementTemplate<FbxVector2> &element,
		vec2f &data,
		int triangleIndex,
		int triangleCornerId,
		int vertexIndex);

	const unsigned int GetUVVertexIndex(
		const unsigned int triangleIndex,
		const unsigned int triangleCornerId) const;
};

