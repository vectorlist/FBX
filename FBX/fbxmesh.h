#pragma once

#include <GL/glew.h>
#include <vec2f.h>
#include <vec3f.h>
#include <vector>

#define VBO_VERTEX		0
#define VBO_NORMAL		1
#define VBO_ST			2
#define VBO_NUM			3

struct FBXVertex
{
	vec3f position;
	vec3f normal;
	vec2f st;
	float boneWeight[4];
	int boneID[4];
};

struct FBXMesh
{
	std::vector<uint32_t> indices;
	std::vector<FBXVertex> vertices;
	std::vector<uint32_t> controlIndices;

	void buildBuffer();
	void render(GLuint shader);
	void renderBoundingBox();
	uint32_t indices_num;
private:
	GLuint vao = NULL;
	//GLuint vbo[VBO_NUM];
	GLuint vbo;
	GLuint ibo;
};

