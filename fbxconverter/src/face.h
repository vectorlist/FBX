#pragma once

#include <vec4f.h>
#include <vec3f.h>
#include <vec2f.h>

class Face
{
public :

	Face::Face();

	int GetMaterialId()
	{
		return m_materialId;
	}

	void SetMaterialId(
		unsigned int materialId
		)
	{
		m_materialId = materialId;
	}

	int GetVertexIndex(
		int triangleCornerId
		)
	{
		return m_vertexIndices[triangleCornerId];
	};

	void SetVertexIndex(
		int triangleCornerId,
		int vertexIndex
		)
	{
		m_vertexIndices[triangleCornerId] = vertexIndex;
	};

	void SetVertexIndices(
		unsigned int vertexIndices[3]
		)
	{
		m_vertexIndices[0] = vertexIndices[0];
		m_vertexIndices[1] = vertexIndices[1];
		m_vertexIndices[2] = vertexIndices[2];
	};

	void SetNormals(
		vec3f normals[3]
		)
	{
		m_normals[0] = normals[0];
		m_normals[1] = normals[1];
		m_normals[2] = normals[2];
	};

	void SetNormal(
		vec3f normal,
		int triangleCornerId
		)
	{
		m_normals[triangleCornerId] = normal;

	};

	vec3f GetNormal(
		int triangleCornerId
	)
	{
		return m_normals[triangleCornerId];
	}

	void SetBinormals(
		vec4f binormals[3]
		)
	{
		m_binormals[0] = binormals[0];
		m_binormals[1] = binormals[1];
		m_binormals[2] = binormals[2];
	};

	void SetBinormal(
		vec4f binormal,
		int triangleCornerId
		)
	{
		m_binormals[triangleCornerId] = binormal;

	};

	vec4f GetBinormal(
		int triangleCornerId
	)
	{
		return m_binormals[triangleCornerId];
	}

	void SetTangents(
		vec4f tangents[3]
		)
	{
		m_tangents[0] = tangents[0];
		m_tangents[1] = tangents[1];
		m_tangents[2] = tangents[2];
	};

	void SetTangent(
		vec4f tangent,
		int triangleCornerId
		)
	{
		m_tangents[triangleCornerId] = tangent;

	};

	vec4f GetTangent(
		int triangleCornerId
	)
	{
		return m_tangents[triangleCornerId];
	}

	void SetUVs(
		vec2f uvs[3]
		)
	{
		m_uvs[0] = uvs[0];
		m_uvs[1] = uvs[1];
		m_uvs[2] = uvs[2];

	};

  	void SetUV(
		vec2f uv,
		int triangleCornerId
		)
	{
		m_uvs[triangleCornerId] = uv;

	};

	vec2f GetUV(
		int triangleCornerId
	)
	{
		return m_uvs[triangleCornerId];
	}

	vec3f GetColour(
		int triangleCornerId
	)
	{		
		return m_colours[triangleCornerId];
	};

	void SetColour(
		vec3f colour,
		int triangleCornerId
		)
	{		
		m_colours[triangleCornerId] = colour;
	};

	void SetColours(
		vec3f colours[3]
		)
	{		
		m_colours[0] = colours[0];
		m_colours[1] = colours[1];
		m_colours[2] = colours[2];

	};

private:

	unsigned int m_vertexIndices[3];	// Vertex Indices for the 3 triangle corners
	vec3f m_normals[3];				// Normals for the 3 triangle corners (thus multiple normals per vertex)
	vec4f m_binormals[3];			// Binormals for the 3 triangle corners (thus multiple normals per vertex)
	vec4f m_tangents[3];			// Tangents for the 3 triangle corners (thus multiple normals per vertex)
	vec2f m_uvs[3];					// UVs for the 3 triangle corners (thus multiple uvs per vertex)
	vec3f m_colours[3];				// Colours for the 3 triangle corners (thus multiple colours per vertex)
	int m_materialId;					// Material Id. The mesh must have a material - a default one will be assigned if it does not
};

