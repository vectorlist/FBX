#pragma once

#include <fbxsdk.h>
#include <fbxmesh.h>
#include <vertex.h>
#include <face.h>


class FBXTool
{
public:
	static void buildMesh(
		std::vector<Vertex> &vertices,
		std::vector<Face>	&faces,
		FBXMesh &mesh);

};

