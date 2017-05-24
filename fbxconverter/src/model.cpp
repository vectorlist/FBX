#include "model.h"
#include <log.h>
#include <MeshNode.h>
#include <Vertex.h>
#include <Face.h>

Model::Model()
{
}


Model::~Model()
{
}

void Model::processMesh(MeshNode *pNode)
{
	for (MeshNode* node = pNode; node != NULL; node = node->m_next)
	{

		MeshTriangleArrayPtr triArray = node->GetTriangles();
		MeshVertexArrayPtr vertexArray = node->GetVertices();

		int numTri = node->GetNumTriangles();
		int numVertex = node->GetNumVertices();

		LOG << "num tri : " << numTri << ENDN;

		for (int triIndex = 0; triIndex < numTri; ++triIndex)
		{
			for (int triConerIndex = 0; triConerIndex < 3; ++triConerIndex)
			{
				VBOVertex vertex;
				unsigned int vertexIndex = triArray[triIndex].GetVertexIndex(triConerIndex);
				FbxVector4 pos = vertexArray[vertexIndex].GetPosition();
				vertex.position = vec3f(pos[0], pos[1], pos[2]);
				vertex.normal = vec3f(triArray[triIndex].GetNormal(triConerIndex));
				vertex.st = triArray[triIndex].GetUV(triConerIndex);

				vertex.setBoneInfo(vertexArray[vertexIndex].GetBoneWeights(),
					vertexArray[vertexIndex].GetBoneInfluences());

				/*LOG << "---------- " << vertexIndex << " ----------"<< ENDN;
				for (int i = 0; i < 4; ++i)
				{
					LOG << "id : " << vertex.boneID[i] << " weight : " << vertex.boneWeight[i] << ENDN;
				}*/

				vertices.push_back(vertex);
				indices.push_back(triIndex * 3 + triConerIndex);
			}
		}
		buildBuffer();



		if (node->m_firstChild)
		{
			processMesh(node->m_firstChild);
		}
	}
}

void Model::buildBuffer()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VBOVertex), vertices.data(),
		GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex),
		(GLvoid*)offsetof(VBOVertex, VBOVertex::position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex),
		(GLvoid*)offsetof(VBOVertex, VBOVertex::normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VBOVertex),
		(GLvoid*)offsetof(VBOVertex, VBOVertex::st));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(VBOVertex),
		(GLvoid*)offsetof(VBOVertex, VBOVertex::boneWeight));

	// this is integer "IPointer" we done need normalize param
	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(4, 4, GL_INT, sizeof(VBOVertex),
		(GLvoid*)offsetof(VBOVertex, VBOVertex::boneID));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(),
		GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	indices_num = indices.size();
}

void Model::render(GLuint shader)
{
	if (vao == NULL) return;
	glUseProgram(shader);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices_num, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	glUseProgram(0);
}
