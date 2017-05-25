#include "animationrenderer.h"
#include <skinmesh.h>
#include <fbxcore.h>
#include <node.h>
#include <fbxtool.h>
#include <animationlayer.h>

AnimationRenderer::AnimationRenderer(FBXCore* core)
	: isRunning(false), mModel(NULL), mCore(core), hasChanged(false)
{
	mScene = AnimationScenePtr(new AnimationScene());
	//UNIFORM LAYOUT BLOCK 0, 1, 2
	mBoneUbo = new UBO<MatrixUBO>(0);

	mNode = &mCore->mNode;

	mModel = new SkinMesh();
	mModel->createMesh(mCore->mNode.getCurrentMeshNode());

	proxys.resize(mCore->mNode.mBoneNodesNum);
}


AnimationRenderer::~AnimationRenderer()
{
	
}

void AnimationRenderer::render(GLuint shader)
{
	if (!mModel) return;
	mModel->render(shader);
}

void AnimationRenderer::processAnimation(int sampleIndex)
{
	if (!mNode) return;
	//AnimationSample* sample = mNode->getAnimationSample();
	if (hasSampleIndex != sampleIndex) {

		hasChanged = true;
	}
	if (!isRunning || hasChanged)
	{
		//TODO : single sample to layer sample
		//AnimationSample* sample = mNode->getAnimationSample();

		AnimationSample* sample = NULL;
		AnimationLayers* layers = mNode->getAnimationLayers();
		if (sampleIndex == 0) {
			sample = layers->getSample(sampleIndex);
		}
		else {
			sample = layers->getSample(sampleIndex);
		}
		mNode->currentSample = sample;
		LOG << "change sample layer : " << sample->getName() << ENDN;
		/*SET SAMPLE*/
		long start = sample->convertFrameToMilli(sample->getSampleStart());
		long end = sample->convertFrameToMilli(sample->getSampleEnd());
		
		LOG << "start : " << start << " end : " << end  << ENDN;
		//set to current (Global time system)
		mScene->startAnimation(GET_TIME(), start, end);
		mScene->setLoop(true);
		isRunning = true;
		hasSampleIndex = sampleIndex;
		hasChanged = false;
	}
	else
	{
		/*if (!sample) {
			LOG << "no sample" << ENDN;
		};*/
		mScene->updateNode(mNode,GET_TIME());
		updateBoneTransform(mNode);
	}
}

void AnimationRenderer::updateBoneTransform(Node *pNode)
{
	BoneNode* rootBoneNode = pNode->getBoneNodeRoot();

	processBoneNode(rootBoneNode);

	for (int i = 0; i < proxys.size(); ++i)
	{
		uboData.bones[i] = proxys[i];
	}

	mBoneUbo->updateBuffer(&uboData);
	frame++;
}

void AnimationRenderer::processBoneNode(BoneNode * rootBoneNode)
{
	for (BoneNode* node = rootBoneNode; node != NULL; node = node->mNext)
	{

		auto global = mNode->getCurrentMeshNode()->getGlobalTransform();
		auto local = node->getGlobalTransfrom();
		auto inv = node->getInveseLocalTransfrom();
		int boneID = node->getID();

		proxys[boneID] = Matrix4x4(local * inv * global);
		if (node->mFirstChild)
		{
			processBoneNode(node->mFirstChild);
		}
	}
}

AnimationSample* AnimationRenderer::getSample()
{
	return mNode->getAnimationSample();
}
