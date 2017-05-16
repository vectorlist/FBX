#include "fbxtool.h"
#include <log.h>


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


