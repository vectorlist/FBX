#pragma once

#include <GL/glew.h>
#include <vec2f.h>
#include <vec3f.h>
#include <matrix4x4.h>
#include <vector>
#include <memory>
struct Vertex
{
	vec3f pos;
	vec3f normal;
	vec2f st;
};

class Mesh
{
public:
	Mesh();
	Mesh(std::vector<Vertex> vertice, std::vector<uint32_t> indices);
	~Mesh();


	GLuint vao = NULL;
	uint32_t indices_size;
	Matrix4x4 model;
	void render();

	void buildSphere(float radius, float xseg, float yseg);
private:
	void buildBuffers();
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	GLuint vbo = NULL;
	GLuint ibo = NULL;

};

inline Mesh::Mesh()
{
	//dothing
}

inline Mesh::Mesh(std::vector<Vertex> vertice, std::vector<uint32_t> indices)
{
	this->vertices = vertice;
	this->indices = indices;
	buildBuffers();
}

inline Mesh::~Mesh()
{
	/*if (vao)
		glDeleteVertexArrays(1, &vao);
	if (vbo)
		glDeleteBuffers(1, &vbo);
	if (ibo)
		glDeleteBuffers(1, &ibo);*/
}

inline void Mesh::buildBuffers()
{
	// Create buffers/arrays
	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);
	glGenBuffers(1, &this->ibo);

	glBindVertexArray(this->vao);

	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
		&this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
		&this->indices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)0);
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Vertex::normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Vertex::st));
	
	glBindVertexArray(0);

	indices_size = indices.size();
	/*vertices.clear();
	indices.clear();*/
}


inline void Mesh::render()
{
	glBindVertexArray(this->vao);
	glDrawElements(GL_TRIANGLES, this->indices_size, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


inline void Mesh::buildSphere(float radius, float slices, float stacks)
{
	int faces = slices * stacks;
	int ibv = slices * stacks;

	vertices.resize(ibv);
	indices.resize(6 * faces);

	float x, y, z, r, s;
	int idx = 0, tidx = 0;

	float const R = 1.f / (float)(slices - 1);
	float const S = 1.f / (float)(stacks - 1);

	for (r = 0; r < slices; ++r)
	{
		for (s = 0; s < stacks; ++s)
		{
			x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);
			y = sin(-M_PI_2 + (M_PI * r * R));

			Vertex vertex;
			vertex.pos = vec3f(x, y, z) * radius;
			vertex.normal = vec3f(x, y, z).normalized();
			vertex.st = vec2f(s*S, r*R);

			vertices[idx] = vertex;
			idx++;
			tidx++;
		}
	}

	idx = 0;
	int ringStart, nextRingStart, nextSlice;

	for (r = 0; r < slices; ++r)
	{
		ringStart = r * stacks;
		nextRingStart = (r + 1) * stacks;
		for (s = 0; s < stacks; ++s)
		{
			nextSlice = s + 1;

			indices[idx] = ringStart + s;
			indices[idx + 1] = nextRingStart + s;
			indices[idx + 2] = nextRingStart + nextSlice;
			indices[idx + 3] = ringStart + s;
			indices[idx + 4] = nextRingStart + nextSlice;
			indices[idx + 5] = ringStart + nextSlice;
			idx += 6;
		}
	}

	//
	/*::s = s;
	::slices = slices;
	::stacks = stacks;*/
	//    vector <float> positiondata;
	//    vector <float> colordata;

	/*float const R = 1.f / (float)(slices - 1);
	float const S = 1.f / (float)(stacks - 1);

	for (int i = 0; i <= stacks; i++) {
		float theta = i * M_PI / (float)stacks;
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);

		for (int j = 0; j <= slices; j++) {
			float phi = j * 2 * M_PI / (float)slices;
			float sinPhi = sinf(phi);
			float cosPhi = cosf(phi);

			float x = cosPhi * sinTheta;
			float y = cosTheta;
			float z = sinPhi * sinTheta;

			float s = i * R;
			float t = j * S;

			Vertex vertex;
			vertex.pos = vec3f(x, y, z) * radius;
			vertex.normal = vec3f(x, y, z).normalized();
			vertex.st = vec2f(s,t);
			vertices.push_back(vertex);
		}
	}

	for (int i = 0; i < stacks; i++) {
		for (int j = 0; j < slices; j++) {
			unsigned short first = (i * (slices + 1)) + j;
			unsigned short second = first + slices + 1;

			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);

			indices.push_back(second);
			indices.push_back(second + 1);
			indices.push_back(first + 1);
		}
	}*/




	buildBuffers();
	indices_size = indices.size();
		
}
