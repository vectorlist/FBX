#pragma once

#include <animationscene.h>
#include <node.h>
#include <fbxmesh.h>

class AnimationRenderer
{
public:
	AnimationRenderer();
	~AnimationRenderer();

	void render(GLuint shader);
	void setRootNode(Node* node);
	void setRenderableMesh(FBXMesh* mesh);
	//void setAnimationStack(FbxA)
	void startAnimation();

	AnimationSample* getSample();
	
	FBXMesh* mMesh;
	Node* mNode;
	
	bool isRunning;
	AnimationScenePtr mScene;
};


