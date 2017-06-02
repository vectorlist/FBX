#pragma once

#include <animhandle.h>
#include <matrix4x4.h>
#include <ubo.h>
#include <vector>
#include <config.h>

struct BoneBufferData
{
	BoneBufferData() : isAnimation(true){}
	Matrix4x4 bones[BONE_MAX];
	bool isAnimation;
};

class SkinMesh;
class Node;
class AnimModel
{
public:
	AnimModel(const std::string &filename);
	~AnimModel();

	void render(GLuint shader);
	
	//test animation sample stack
	void processAnimation(int sampleIndex);

	void updateBoneTransform(Node* pNode);
	void processBoneNode(BoneNode* rootBoneNode);
	void setNodePtr(node_ptr &node);
	Node* getNode();
		

	animhandle_ptr m_handle;
	BoneBufferData m_boneData;
	std::shared_ptr<UBO<BoneBufferData>> m_boneUbo;

private:
	bool hasChanged;
	int hasSampleIndex = 0;
	bool isRunning;
	bool isAnimatedModel;
	node_ptr m_nodePtr;
	Node* m_node;
	SkinMesh* m_mesh;
};


