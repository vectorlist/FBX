#pragma once

#include <GL/glew.h>
#include <vec2f.h>
#include <vec3f.h>
#include <vector>

#define MAX_BONE_CONPONENT		4

class MeshNode;
struct SkinVertex
{
	vec3f position;
	vec3f normal;
	vec2f st;
	float boneWeight[MAX_BONE_CONPONENT];
	int boneID[MAX_BONE_CONPONENT];

	void setBoneWeights(
		const float* weights,
		const unsigned int* ids)
	{
		for (int i = 0; i < MAX_BONE_CONPONENT; ++i)
		{
			boneWeight[i] = weights[i];
			boneID[i] = ids[i];
		}
	}
};

class SkinMesh
{
public:
	SkinMesh();
	~SkinMesh();
	
	void createMesh(MeshNode *meshNode);
	void createBuffer();
	void releaseBuffer();
	void render(GLuint shader);
	uint32_t indices_num;
private:
	GLuint vao = NULL;
	GLuint vbo;
	GLuint ibo;

	std::vector<uint32_t> indices;
	std::vector<SkinVertex> vertices;
	bool mIsBuffer;
};

