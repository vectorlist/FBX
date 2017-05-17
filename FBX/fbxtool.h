#pragma once

#include <fbxsdk.h>
#include <fbxmesh.h>
#include <vertex.h>
#include <face.h>
#include <bonemesh.h>

class BoneNode;
class FBXTool
{
public:
	static void buildMesh(
		std::vector<Vertex> &vertices,
		std::vector<Face>	&faces,
		FBXMesh &mesh);

	//Bone Mesh (FbxNode, parentnode) recursive(outside class)
	static bool loadBoneNodes(FbxNode* pNode, BoneNode* parentNode,
	FBXBoneMesh &boneMesh);

	//for add BooneNode(build in)
	//TODO : replace to built in func
	static BoneNode* loadBoneNode(FbxNode* pNode, BoneNode* parent, FBXBoneMesh &boneMesh);

	
};

