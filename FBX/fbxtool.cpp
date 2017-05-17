#include "fbxtool.h"
#include <log.h>
#include <bonenode.h>
#include <meshnode.h>
#include <logger.h>

void FBXTool::buildMesh(std::vector<Vertex>& vertices, std::vector<Face>& faces, FBXMesh &mesh)
{
	LOG << "vertice size : " << vertices.size() << ENDN;
	LOG << "faces size : " << faces.size() << ENDN;
	
	std::vector<int> debugIndex;
	for (auto faceIndex = 0; faceIndex < faces.size(); ++faceIndex)
	{
		for (int facePointIndex = 0; facePointIndex < FACE_POINT_MAX; ++facePointIndex)
		{
			Face &face = faces[faceIndex];
			int vertexIndex = face.getVertexIndex(facePointIndex);
			FBXVertex vertex;
			vertex.position = vertices[vertexIndex].getPosition();
			vertex.normal = face.getNormal(facePointIndex);
			vertex.st = face.getST(facePointIndex);
			
			//TODO : bone IDs 4s
			for (int i = 0; i < BONE_COMPONENT_NUM; ++i)
			{
				vertex.boneID[i] = vertices[vertexIndex].getBoneID(i);
				//TODO : bone weight 4s
				vertex.boneWeight[i] = vertices[vertexIndex].getBoneWeight(i);
			}
			
			mesh.vertices.push_back(vertex);
			mesh.indices.push_back(faceIndex * 3 + facePointIndex);
			debugIndex.push_back(vertexIndex);
		}
	}


	mesh.buildBuffer();

	if (0) return;
	for (int i = 0; i < mesh.vertices.size(); ++i)
	{
		FBXVertex& vertex = mesh.vertices[i];
		/*LOG_W <<
			"pos : " << vertex.position <<
			" normal : " << vertex.normal <<
			" st : " << vertex.st <<
			ENDN;*/
		int index = debugIndex[i];
		LOG_W << "vertexID : " << index
			<< " boneID : " << vertex.boneID[0] << ", " << vertex.boneID[1] << ", "
			<< vertex.boneID[2] << ", " << vertex.boneID[3] << ", "
			<< " boneWeights : " << vertex.boneWeight[0] << ", " << vertex.boneWeight[1] << ", "
			<< vertex.boneWeight[2] << ", " << vertex.boneWeight[3] << ENDN;
		
	}

}

void FBXTool::buildMesh(MeshNode *meshNode, FBXMesh &mesh)
{
	if (meshNode->getID() > 0) {
		LOG_ERROR("dosent supported 2more meshes in this version");
	}
	std::vector<int> debugIndex;
	auto& faces = meshNode->getFaces();
	auto& vertices = meshNode->getVertices();
	for (auto faceIndex = 0; faceIndex < faces.size(); ++faceIndex)
	{
		for (int facePointIndex = 0; facePointIndex < FACE_POINT_MAX; ++facePointIndex)
		{
			Face &face = faces[faceIndex];
			int vertexIndex = face.getVertexIndex(facePointIndex);
			FBXVertex vertex;
			vertex.position = vertices[vertexIndex].getPosition();
			vertex.normal = face.getNormal(facePointIndex);
			vertex.st = face.getST(facePointIndex);

			//TODO : bone IDs 4s
			for (int i = 0; i < BONE_COMPONENT_NUM; ++i)
			{
				vertex.boneID[i] = vertices[vertexIndex].getBoneID(i);
				//TODO : bone weight 4s
				vertex.boneWeight[i] = vertices[vertexIndex].getBoneWeight(i);
			}

			mesh.vertices.push_back(vertex);
			mesh.indices.push_back(faceIndex * 3 + facePointIndex);
			debugIndex.push_back(vertexIndex);
		}
	}


	mesh.buildBuffer();
	bool enableWrite = false;
	if (!enableWrite) return;
	for (int i = 0; i < mesh.vertices.size(); ++i)
	{
		FBXVertex& vertex = mesh.vertices[i];
		int index = debugIndex[i];
		LOG_W << "vertexID : " << index
			<< " boneID : " << vertex.boneID[0] << ", " << vertex.boneID[1] << ", "
			<< vertex.boneID[2] << ", " << vertex.boneID[3] << ", "
			<< " boneWeights : " << vertex.boneWeight[0] << ", " << vertex.boneWeight[1] << ", "
			<< vertex.boneWeight[2] << ", " << vertex.boneWeight[3] << ENDN;

	}

}


