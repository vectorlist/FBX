#pragma once

#include <string>
#include <vector>
//#include "../Batch/BatchFwdDecl.h"
#include "MeshFwdDecl.h"
#include <vec2f.h>
#include <vec3f.h>
#include <vec4f.h>
#include <fbxsdk/scene/fbxaxissystem.h>

using namespace fbxsdk;

class FBXImport
{
public:
	FBXImport();
	~FBXImport();

	/**
		@brief Imports an fbx mesh into an import mesh format
		@return Returns an import mesh or NULL if import failed
	*/
	MeshPtr Import(
		const std::string &fbxFilename		// The name of the FBX file to import
	);

private:

	bool LoadNodes(
		FbxNode& fbxNode,	// The FBX mesh to extract data from and add to m_mesh
		BoneNode *parentBoneNode,
		MeshNode *parentMeshNode
	);

	MeshNode *LoadMeshNode(
		FbxNode &fbxNode,				// The FBX mesh to extract data from and add to m_mesh mesh node list
		MeshNode *parent
		);

	BoneNode *LoadBoneNode(
		FbxNode& fbxNode,	// The FBX mesh to extract data from and add to m_mesh bone node list
		BoneNode *parent
	);

	bool LoadAnimationLayerInfo();

	bool LoadSkin();

	bool LoadSkin(
		const FbxGeometry &fbxGeometry,// The FBX mesh geometry node to extract data from
		MeshNode &meshNode //Mesh node to store this skin data
		);

	/**
	@brief Bake all possible FBX transforms into the statndard rotation, trnaslate and scale variables so the can be extracted easily later
	*/
	void BakeNodeTransforms(
		FbxNode &node		//!< Node to bake transforms for
		) const;


	const unsigned int GetUVVertexIndex(
		const unsigned int triangleIndex, 
		const unsigned int triangleCornerId
		) const;

	
	/**
		@brief Loads the vertex Indices for the given triangle
	*/
	void LoadVertexIndices(
		FbxMesh &fbxMesh,			// FBX mesh to import data from
		int triangleIndex,			// Index of the current triangle being loaded
		Face &triangle	// The current triangle to store the imported data
		);

	/**
		@brief Loads the colours at each corner of the given triangle
	*/
	void LoadColours(
		FbxMesh &fbxMesh,			// FBX mesh to import data from
		int triangleIndex,			// Index of the current triangle being loaded
		Face &triangle	// The current triangle to store the imported data
		);

	/**
		@brief Loads the uvs at each corner of the given triangle
	*/
	void LoadUVs(
		FbxMesh &fbxMesh,			// FBX mesh to import data from
		int triangleIndex,			// Index of the current triangle being loaded
		Face &triangle	// The current triangle to store the imported data
		);

	/**
		@brief Loads the normals at each corner of the given triangle
	*/
	void LoadNormals(
		FbxMesh &fbxMesh,			// FBX mesh to import data from
		int triangleIndex,			// Index of the current triangle being loaded
		Face &triangle	// The current triangle to store the imported data
		);

	/**
		@brief Loads the binormals at each corner of the given triangle
	*/
	void LoadBinormals(
		FbxMesh &fbxMesh,			// FBX mesh to import data from
		int triangleIndex,			// Index of the current triangle being loaded
		Face &triangle	// The current triangle to store the imported data
		);

	/**
		@brief Loads the tangents at each corner of the given triangle
	*/
	void LoadTangents(
		FbxMesh &fbxMesh,			// FBX mesh to import data from
		int triangleIndex,			// Index of the current triangle being loaded
		Face &triangle	// The current triangle to store the imported data
		);

	void LoadVector4VertexElement(
		FbxLayerElementTemplate<FbxVector4> &element,
		vec4f &data,
		int triangleCornerId,								// The corner(vertex) of this triangle being refered to
		int vertexIndex										// The index of the vertex in the vertex array
		);

	void LoadVector2VertexElement(
		FbxLayerElementTemplate<FbxVector2> &element,
		vec2f &data,
		int triangleIndex,									// The index of this triangle in the triangle array
		int triangleCornerId,								// The corner(vertex) of this triangle being refered to
		int vertexIndex										// The index of the vertex in the vertex array
		);

	void LoadColourVertexElement(
		FbxLayerElementTemplate<FbxColor> &element,
		vec3f &colour, // The current nodes vertex array which will store the imported data
		int triangleCornerId,								// The corner(vertex) of this triangle being refered to
		int vertexIndex										// The index of the vertex in the vertex array
		);

	void DestroyFBXManagers();

	FbxManager *m_fbxManager;			// FBX SDK manager
	FbxScene *m_fbxScene;				// FBX SDK scene which data will be extracted from
	FbxImporter *m_fbxImporter;			// Scene importer for the FBX file
	FbxIOSettings *m_ioSettings;

	MeshPtr m_mesh;					// The mesh to hold the imported data

	static const std::string m_dummyTexture;

	// Store the mesh nodes temporarily in case they are needed for skinning
	struct MeshNodeInfo
	{
		std::vector<MeshNode *> m_meshNode;
		std::vector<FbxNode*> m_fbxMeshNode;
	};

	MeshNodeInfo m_meshNodeInfo;
};

