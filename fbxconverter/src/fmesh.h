
#pragma once

#include "LinkedTree.h"
#include "MeshFwdDecl.h"
#include <string>

class Mesh
{
public:

	Mesh::Mesh();
	Mesh::~Mesh();

	BoneNode* GetBoneNodeHierarchy()
	{
		return m_boneNodes.m_root;
	}

	MeshNode* GetMeshNodeHierarchy()
	{
		return m_meshNodes.m_root;
	}

	void AddChildBoneNode(
		BoneNode *parent,	// Parent node or NULL if no parent
		BoneNode *node		// Child node to add
		)
	{
		m_boneNodes.AddAsChild(parent, node);
		m_numBoneNodes++;
	}

	void AddChildMeshNode(
		MeshNode *parent,	// Parent node or NULL if no parent
		MeshNode *node		// Child node to add
		)
	{
		m_meshNodes.AddAsChild(parent, node);
		m_numMeshNodes++;
	}

	BoneNode* GetBoneByName(
		const std::string &name
	);

	unsigned int GetNumMeshNodes()
	{
		return m_numMeshNodes;
	};

	unsigned int GetNumBoneNodes()
	{
		return m_numBoneNodes;
	};

	AnimationInfoPtr GetAnimationInfo()
	{
		return m_animationInfo;
	}

	void SetAnimationInfo(
		AnimationInfoPtr animationInfo
		)
	{
		m_animationInfo = animationInfo;
	}

private:
	LinkedTree<BoneNode> m_boneNodes;
	LinkedTree<MeshNode> m_meshNodes;
	AnimationInfoPtr m_animationInfo; // Extra info about all animation tracks
	unsigned int m_numMeshNodes;
	unsigned int m_numBoneNodes;
};
