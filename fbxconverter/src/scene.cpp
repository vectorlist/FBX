#include <Scene.h>
#include <FBXImport.h>
#include <model.h>
#include "fmesh.h"
#include "BoneNode.h"
#include "MeshNode.h"
#include <AnimationController.h>
#include <AnimationInfo.h>
#include <log.h>
#include <shadertool.h>

Scene::Scene(const std::string &filename)
	: isRunning(false)
{
	auto importer(new FBXImport());
	mMesh = importer->Import(filename);

	model = new Model();
	model->processMesh(mMesh->GetMeshNodeHierarchy());
	//model.processMesh(mMesh->GetMeshNodeHierarchy());
	ubo = new UBO(0, sizeof(Matrix4x4) * MAX_BONES);
	mAnimator = boost::shared_ptr<AnimationController>(new AnimationController());
	trans.resize(mMesh->GetNumBoneNodes());

	modelmat = mMesh->GetMeshNodeHierarchy()->GetGlobalTransform();
}


Scene::~Scene()
{
	if (model) {
		delete model;
		delete ubo;
		model = NULL;
		ubo = NULL;
	}
}

void Scene::render(GLuint shader)
{
	glUseProgram(shader);
	//Matrix4x4 m(modelmat);
	//ShaderTool::setUniformMatrix4f(shader, m, "model");
	model->render(shader);
	glUseProgram(0);
}

void Scene::processFrame()
{
	if (!isRunning)
	{

		auto sample = mMesh->GetAnimationInfo();
		long start = sample->ConvertFrameToMilliseconds(sample->GetStartSample());
		long end = sample->ConvertFrameToMilliseconds(sample->GetEndSample());
		mAnimator->StartAnimation(GET_TIME(), start, end);
		isRunning = true;
	}

	else
	{

		mAnimator->Update(mMesh, GET_TIME());
		processUpdate();
	}
}

void Scene::processUpdate()
{
	

	auto root = mMesh->GetBoneNodeHierarchy();
	nodeRecursive(root);

	for (int i = 0; i < trans.size(); ++i)
	{
		
		boneTransforms[i] = trans[i];
	}
	ubo->updateBuffer(boneTransforms);
	//LOG << frame << ENDN;
	frame++;
}

void Scene::nodeRecursive(BoneNode * rootBoneNode)
{
	for (BoneNode* node = rootBoneNode; node != NULL; node = node->m_next)
	{
		auto inv = node->GetInverseReferenceMatrix();
		auto local = node->GetGlobalTransform();

		trans[node->GetId()] = Matrix4x4(local * inv * modelmat);
		//trans[node->GetId()] = Matrix4x4(modelmat * inv * local);
		//LOG << node->GetId() << ENDN;

		if (node->m_firstChild)
		{
			nodeRecursive(node->m_firstChild);
		}
	}
}
