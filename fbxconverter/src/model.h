#pragma once

#include <vec3f.h>
#include <vec2f.h>
#include <vector>
#include <MeshFwdDecl.h>
#include <GL/glew.h>

struct VBOVertex
{
	vec3f position;
	vec3f normal;
	vec2f st;
	float boneWeight[MAX_INFLUENCES];
	int boneID[MAX_INFLUENCES];

	void setBoneInfo(
		const float* weights,
		const unsigned int* ids)
	{
		for (int i = 0; i < MAX_INFLUENCES; ++i)
		{
			boneWeight[i] = weights[i];
			boneID[i] = ids[i];
		}
	}
};

class Model
{
public:
	Model();
	~Model();

	std::vector<VBOVertex> vertices;
	std::vector<unsigned int> indices;

	void processMesh(MeshNode* pNode);


	void buildBuffer();
	void render(GLuint shader);
private:
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
	unsigned int indices_num;
};

