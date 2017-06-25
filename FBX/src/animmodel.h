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


typedef void(*cbf)(Node* node);

class SkinMesh;
class Node;
class AnimModel
{
public:
	AnimModel(const std::string &filename);
	~AnimModel();

	void render(GLuint shader);
	
	//test animation sample stack
	void Update(float delta);

	void updateBoneTransform(Node* pNode);
	void processBoneNode(BoneNode* rootBoneNode);
	void setNodePtr(node_ptr &node);
	Node* getNode();
	AnimHandle* getHandle();
		

	animhandle_ptr m_handle;
	BoneBufferData m_boneData;
	std::shared_ptr<UBO<BoneBufferData>> m_boneUbo;

	void evalCallback();
	cbf callback;
	bool hasChanged;
private:
	
	int hasSampleIndex = 0;
	bool isRunning;
	bool isAnimatedModel;
	bool isFirstLoop;

	node_ptr mNodePtr;
	Node* mNode;
	SkinMesh* mMesh;
};


