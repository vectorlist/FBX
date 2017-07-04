#pragma once

#include <animhandle.h>
#include <matrix4x4.h>
#include <ubo.h>
#include <vector>
#include <config.h>

class SkinMesh;
class Node;
struct BoneBufferData
{
	BoneBufferData() : Weight(1.0f),IsAnimation(true){}
	Matrix4x4	CurrentBones[BONE_MAX];
	Matrix4x4	NextBones[BONE_MAX];
	float		Weight;
	bool		IsAnimation;
};


class AnimModel
{
public:
	AnimModel(const std::string &filename);
	~AnimModel();

	void render(GLuint shader);
	
	//test animation sample stack
	void Update(float delta);

	void UpdateBoneTransform(Node* pNode);
	void ProcessBoneNode(BoneNode* rootBoneNode);
	void ProcessNextBoneNode(BoneNode* rootBoneNode);
	void setNodePtr(node_ptr &node);
	Node* GetNode();
	AnimHandle* GetHandle();
	void moveEvent();
		
	animhandle_ptr mHandle;
	BoneBufferData mBoneData;
	std::shared_ptr<UBO<BoneBufferData>> m_boneUbo;

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


