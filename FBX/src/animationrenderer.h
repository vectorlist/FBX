#pragma once

#include <animationscene.h>
#include <matrix4x4.h>
#include <ubo.h>
#include <vector>

#define MAX_BONES			64

struct MatrixUBO
{
	Matrix4x4 bones[MAX_BONES];
};

class SkinMesh;
class Node;
class FBXCore;
class AnimationRenderer
{
public:
	AnimationRenderer(FBXCore* core);
	~AnimationRenderer();

	void render(GLuint shader);
	
	//void setAnimationStack(FbxA)
	void processAnimation();

	void updateBoneTransform(Node* pNode);
	void processBoneNode(BoneNode* rootBoneNode);
	AnimationSample* getSample();
	
	SkinMesh* mModel;
	Node* mNode;
	FBXCore* mCore;
	float frame;
	bool isRunning;
	AnimationScenePtr mScene;
	/*Matrix4x4 mBoneTransforms[MAX_BONES];*/
	MatrixUBO uboData;
	std::vector<Matrix4x4> proxys;

	UBO<MatrixUBO> *mBoneUbo;

};


