#pragma once

#include <fbxsdk.h>
#include <fbxmesh.h>
#include <vertex.h>
#include <face.h>
#include <node.h>

class BoneNode;
class MeshNode;
class FBXTool
{
public:
	//meshnode to renderable mesh
	static void buildMesh(
		std::vector<Vertex> &vertices,
		std::vector<Face>	&faces,
		FBXMesh &mesh);

	static void buildMesh(
		MeshNode *meshNode,
		FBXMesh &mesh);
};

