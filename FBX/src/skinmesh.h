#pragma once

#include <GL/glew.h>
#include <config.h>
#include <vec2f.h>
#include <vec3f.h>
#include <vector>


class MeshNode;
struct SkinVertex
{
	vec3f position;
	vec3f normal;
	vec2f st;
	float boneWeight[BONE_COMPONENT_NUM];
	int boneID[BONE_COMPONENT_NUM];

	void setBoneWeights(
		const float* weights,
		const unsigned int* ids)
	{
		for (int i = 0; i < BONE_COMPONENT_NUM; ++i)
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
	bool isBuffer();
	uint32_t indices_num;
private:
	GLuint vao;
	GLuint vbo;
	GLuint ibo;

	std::vector<uint32_t> indices;
	std::vector<SkinVertex> vertices;
	bool mIsBuffer;
};

