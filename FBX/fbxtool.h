#pragma once

#include <fbxsdk.h>
#include <fbxmesh.h>
#include <vertex.h>
#include <face.h>

class BoneNode;
class FBXTool
{
public:
	static void buildMesh(
		std::vector<Vertex> &vertices,
		std::vector<Face>	&faces,
		FBXMesh &mesh);

	static bool loadNodes(
		FbxNode* pNode,
		BoneNode* parentBoneNode);
};

