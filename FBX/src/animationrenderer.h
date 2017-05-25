#pragma once

#include <animationscene.h>
#include <matrix4x4.h>
#include <ubo.h>
#include <vector>
#include <config.h>

struct MatrixUBO
{
	Matrix4x4 bones[BONE_MAX];
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
	
	//test animation sample stack
	void processAnimation(int sampleIndex);

	void updateBoneTransform(Node* pNode);
	void processBoneNode(BoneNode* rootBoneNode);
	AnimationSample* getSample();
	
	SkinMesh* mModel;
	Node* mNode;
	FBXCore* mCore;
	float frame;
	bool isRunning;
	AnimationScenePtr mScene;
	/*Matrix4x4 mBoneTransforms[BONE_MAX];*/
	MatrixUBO uboData;
	std::vector<Matrix4x4> proxys;

	UBO<MatrixUBO> *mBoneUbo;

private:
	bool hasChanged;
	int hasSampleIndex = 0;
};


