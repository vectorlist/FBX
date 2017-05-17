#pragma once

#include <nodetree.h>
#include <face.h>
#include <vertex.h>
#include <vector>
#include <fbxsdk/core/math/fbxaffinematrix.h>

using namespace fbxsdk;
typedef std::vector<Face> FaceArray;
typedef std::vector<Vertex> VertexArray;

class MeshNode : public NodeTreeItem<MeshNode>
{
public:
	MeshNode();
	~MeshNode();

	void setName(const std::string &name);
	const std::string& getName() const;
	const uint32_t getID() const;

	/*std::vector<Face> faces;
	std::vector<Vertex> vertices;*/
	FaceArray& getFaces();
	VertexArray& getVertices();

	FbxAMatrix mGlobalMarix;
	bool isBuilt;
private:
	static void resetGlobalID();

	std::string m_name;
	uint32_t m_id;
	static uint32_t global_id;

	FaceArray m_faces;
	VertexArray m_vertices;

};
