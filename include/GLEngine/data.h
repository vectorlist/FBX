
#pragma once

#include <mesh.h>

namespace Data
{
	static std::vector<Vertex> cubeVertices = {
		//front    
		{ vec3f( 0.5f,  0.5f,  0.5f),	vec3f(0.0f, 0.0f, 1.0f),	vec2f(1.0f, 0.0f) },
		{ vec3f(-0.5f,  0.5f,  0.5f),	vec3f(0.0f, 0.0f, 1.0f),	vec2f(0.0f, 0.0f) },
		{ vec3f(-0.5f, -0.5f,  0.5f),	vec3f(0.0f, 0.0f, 1.0f),	vec2f(0.0f, 1.0f) },
		{ vec3f( 0.5f, -0.5f,  0.5f),	vec3f(0.0f, 0.0f, 1.0f),	vec2f(1.0f, 1.0f) },

		{ vec3f( 0.5f,  0.5f,  0.5f),	vec3f(1.0f, 0.0f, 0.0f),	vec2f(1.0f, 0.0f) },
		{ vec3f( 0.5f, -0.5f,  0.5f), 	vec3f(1.0f, 0.0f, 0.0f),	vec2f(0.0f, 0.0f) },
		{ vec3f( 0.5f, -0.5f, -0.5f),	vec3f(1.0f, 0.0f, 0.0f),	vec2f(0.0f, 1.0f) },
		{ vec3f( 0.5f,  0.5f, -0.5f),	vec3f(1.0f, 0.0f, 0.0f),	vec2f(1.0f, 1.0f) },

		{ vec3f( 0.5f,  0.5f,  0.5f),	vec3f(0.0f, 1.0f, 0.0f),	vec2f(1.0f, 0.0f) },
		{ vec3f( 0.5f,  0.5f, -0.5f),	vec3f(0.0f, 1.0f, 0.0f),	vec2f(0.0f, 0.0f) },
		{ vec3f(-0.5f,  0.5f, -0.5f),	vec3f(0.0f, 1.0f, 0.0f),	vec2f(0.0f, 1.0f) },
		{ vec3f(-0.5f,  0.5f,  0.5f),	vec3f(0.0f, 1.0f, 0.0f),	vec2f(1.0f, 1.0f) },

		{ vec3f(-0.5f,  0.5f,  0.5f),	vec3f(-1.0f, 0.0f, 0.0f),	vec2f(1.0f, 0.0f) },
		{ vec3f(-0.5f,  0.5f, -0.5f),	vec3f(-1.0f, 0.0f, 0.0f),	vec2f(0.0f, 0.0f) },
		{ vec3f(-0.5f, -0.5f, -0.5f),	vec3f(-1.0f, 0.0f, 0.0f),	vec2f(0.0f, 1.0f) },
		{ vec3f(-0.5f, -0.5f,  0.5f),	vec3f(-1.0f, 0.0f, 0.0f),	vec2f(1.0f, 1.0f) },

		{ vec3f(-0.5f, -0.5f, -0.5f),	vec3f(0.0f, -1.0f, 0.0f),	vec2f(1.0f, 0.0f) },
		{ vec3f( 0.5f, -0.5f, -0.5f),	vec3f(0.0f, -1.0f, 0.0f),	vec2f(0.0f, 0.0f) },
		{ vec3f( 0.5f, -0.5f,  0.5f),	vec3f(0.0f, -1.0f, 0.0f),	vec2f(0.0f, 1.0f) },
		{ vec3f(-0.5f, -0.5f,  0.5f),	vec3f(0.0f, -1.0f, 0.0f),	vec2f(1.0f, 1.0f) },

		{ vec3f( 0.5f, -0.5f, -0.5f),	vec3f(0.0f, 0.0f, -1.0f),	vec2f(1.0f, 0.0f) },
		{ vec3f(-0.5f, -0.5f, -0.5f),	vec3f(0.0f, 0.0f, -1.0f),	vec2f(0.0f, 0.0f) },
		{ vec3f(-0.5f,  0.5f, -0.5f),	vec3f(0.0f, 0.0f, -1.0f),	vec2f(0.0f, 1.0f) },
		{ vec3f( 0.5f,  0.5f, -0.5f),	vec3f(0.0f, 0.0f, -1.0f),	vec2f(1.0f, 1.0f) },
	};

	static std::vector<uint32_t> cubeIndices = {
		0,1,2 ,2 ,3 ,0,
		4,5,6, 6,7,4,
		8,9,10,10,11,8,
		12,13,14,14,15,12,
		16,17,18,18,19,16,
		20,21,22,22,23,20
	};

	static float size = 1;
	static std::vector<Vertex> floorVertices{

		// Positions          // Colors           // Texture Coords
		{ {  size,  0.0f,  size },{ 0.0f, 1.0f, 0.0f } ,{ 1.0f, 1.0f } },   // Top Right
		{ {  size,  0.0f, -size },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },   // Bottom Right
		{ { -size,  0.0f, -size },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },   // Bottom Left
		{ { -size,  0.0f,  size },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f } }    // Top Left 
	};

	static std::vector<uint32_t> floorIndices
	{
		0,1,3,
		1,2,3
	};

	static std::vector<Vertex> quadVertices{
		{ vec3f( 0.5, 0.5, 0.0f), vec3f(0), vec2f(1.0f,1.0f)},		//T R 0
		{ vec3f( 0.5,-0.5, 0.0f),  vec3f(0), vec2f(1.0f,0.0f) },		//B R 1
		{ vec3f(-0.5,-0.5, 0.0f),  vec3f(0), vec2f(0.0f,0.0f) },		//B L 2
		{ vec3f(-0.5, 0.5, 0.0f),  vec3f(0), vec2f(0.0f,1.0f) }		//T L 3
	};

	static std::vector<uint32_t> quadindices{
		//0,3,1, 2,1,3
		0,2,3, 1,2,0
	};
}