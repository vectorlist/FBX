#include <skinmesh.h>
#include <meshnode.h>
#include <log.h>

SkinMesh::SkinMesh()
	: vao(NULL), vbo(NULL), ibo(NULL),mIsBuffer(false)
{
}

SkinMesh::~SkinMesh()
{
	releaseBuffer();
}

void SkinMesh::createMesh(MeshNode *meshNode)
{
	for (MeshNode* node = meshNode; node != NULL; node = node->mFirstChild)
	{
		if (meshNode->getID() > 0) {
			LOG_ERROR("dosent supported 2more meshes in this version");
		}
		auto& faceArray = meshNode->getFaces();
		auto& vertArray = meshNode->getVertices();

		for (auto faceIndex = 0; faceIndex < faceArray.size(); ++faceIndex)
		{
			for (int facePointIndex = 0; facePointIndex < FACE_COMPONENT_NUM; ++facePointIndex)
			{
				Face &face = faceArray[faceIndex];
				int vertexIndex = face.getVertexIndex(facePointIndex);
				SkinVertex vertex;
				vertex.position = vertArray[vertexIndex].getPosition();
				vertex.normal = face.getNormal(facePointIndex);
				vertex.st = face.getST(facePointIndex);
				vertex.setBoneWeights(
					vertArray[vertexIndex].getBoneWeights(),
					vertArray[vertexIndex].getBoneIDs());

				vertices.push_back(vertex);
				indices.push_back(faceIndex * 3 + facePointIndex);
				
			}
		}
		createBuffer();
	}
	
}

void SkinMesh::createBuffer()
{

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkinVertex), vertices.data(),
		GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinVertex),
		(GLvoid*)offsetof(SkinVertex, SkinVertex::position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkinVertex),
		(GLvoid*)offsetof(SkinVertex, SkinVertex::normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SkinVertex),
		(GLvoid*)offsetof(SkinVertex, SkinVertex::st));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(SkinVertex),
		(GLvoid*)offsetof(SkinVertex, SkinVertex::boneWeight));
	
	// this is integer "IPointer" we done need normalize param
	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(4, 4, GL_INT, sizeof(SkinVertex),
		(GLvoid*)offsetof(SkinVertex, SkinVertex::boneID));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(),
		GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	indices_num = indices.size();
	mIsBuffer = true;
	vertices.clear();
	indices.clear();
}

void SkinMesh::render(GLuint shader)
{
	if (!mIsBuffer) return;
	glUseProgram(shader);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices_num, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	glUseProgram(0);
}

bool SkinMesh::isBuffer()
{
	return mIsBuffer;
}

void SkinMesh::releaseBuffer()
{
	if (mIsBuffer)
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
		vao = NULL;
		vbo = NULL;
		ibo = NULL;
		mIsBuffer = false;
	}
}