
#pragma once

#include <GL/glew.h>
#include <AnimationFwdDecl.h>
#include <MeshFwdDecl.h>
#include <string>
#include <Windows.h>
#include <matrix4x4.h>
#include <ubo.h>
#include <vector>

#define GET_TIME()		timeGetTime()

class Model;
class Scene
{
public:
	Scene(const std::string &filename);
	~Scene();

	MeshPtr mMesh;
	AnimationControllerPtr mAnimator;

	Model* model;

	Matrix4x4 boneTransforms[MAX_BONES];
	FbxAMatrix modelmat;
	std::vector<Matrix4x4> trans;
	UBO* ubo;

	void render(GLuint shader);
	void processFrame();
	void processUpdate();
	bool isRunning;
	float frame = 0;

	void nodeRecursive(BoneNode* rootBoneNode);
};

class FMesh;
class FBXAnimation;
class FBXAnimationCore;
class FBXAnimationInfo;