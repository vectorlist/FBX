#include "meshnode.h"

uint32_t MeshNode::global_id = 0;

MeshNode::MeshNode()
	: m_id(global_id++), isBuilt(false)
{
}


MeshNode::~MeshNode()
{
	resetGlobalID();
}

void MeshNode::setName(const std::string &name)
{
	m_name = name;
}

const std::string & MeshNode::getName() const
{
	return m_name;
}

const uint32_t MeshNode::getID() const
{
	return m_id;
}

FaceArray& MeshNode::getFaces()
{
	// TODO: insert return statement here
	return m_faces;
}

VertexArray& MeshNode::getVertices()
{
	// TODO: insert return statement here
	return m_vertices;
}

void MeshNode::resetGlobalID()
{
	global_id = 0;
}
