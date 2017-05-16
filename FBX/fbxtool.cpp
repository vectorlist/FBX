#include "fbxtool.h"
#include <log.h>
#include <bonenode.h>

void FBXTool::buildMesh(std::vector<Vertex>& vertices, std::vector<Face>& faces, FBXMesh &mesh)
{
	LOG << "vertice size : " << vertices.size() << ENDN;
	LOG << "faces size : " << faces.size() << ENDN;
	

	for (auto faceIndex = 0; faceIndex < faces.size(); ++faceIndex)
	{
		for (int faceID = 0; faceID < FACE_ELEMENT_MAX; ++faceID)
		{
			Face &face = faces[faceIndex];
			int vertexIndex = face.getVertexIndex(faceID);
			FBXVertex vertex;
			vertex.position = vertices[vertexIndex].getPosition();
			vertex.normal = face.getNormal(faceID);
			vertex.st = face.getST(faceID);
			mesh.vertices.push_back(vertex);
			mesh.indices.push_back(faceIndex * 3 + faceID);
		}
	}

	mesh.buildBuffer();
}

bool FBXTool::loadNodes(FbxNode* pNode, BoneNode* parentBoneNode)
{
	BoneNode* newBoneNode = NULL;

	FbxNodeAttribute* const pNodeAttribute = pNode->GetNodeAttribute();

	LOG << "NODE CALLS" << ENDN;

	for (int childIndex = 0; childIndex < pNode->GetChildCount(); ++childIndex)
	{
		FbxNode* childNode = pNode->GetChild(childIndex);
		loadNodes(childNode, NULL);
	}
	return true;
}
