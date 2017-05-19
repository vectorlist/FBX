#include "animationrenderer.h"

AnimationRenderer::AnimationRenderer()
	: isRunning(false)
{
	mScene = AnimationScenePtr(new AnimationScene());
}


AnimationRenderer::~AnimationRenderer()
{
}

void AnimationRenderer::render(GLuint shader)
{
	mMesh->render(shader);
}

void AnimationRenderer::setRootNode(Node *node)
{
	mNode = node;
}

void AnimationRenderer::setRenderableMesh(FBXMesh *mesh)
{
	mMesh = mesh;
}

void AnimationRenderer::startAnimation()
{
	if (!mNode) return;
	AnimationSample* sample = mNode->getAnimationSample();
	
	if (!isRunning)
	{
		//set global start and end
		long start = sample->convertFrameToMilli(sample->getSampleStart());
		long end = sample->convertFrameToMilli(sample->getSampleEnd());
		
		LOG << "start : " << start << " end : " << end  << ENDN;
		//set to current (Global time system)
		mScene->startAnimation(GET_TIME(), start, end);
		mScene->setLoop(true);
		isRunning = true;
	}
	else
	{
		//is runninsg
		//add globalTime
		long now = GET_TIME();
		mScene->updateNode(mNode, now);
	}
}

AnimationSample* AnimationRenderer::getSample()
{
	return mNode->getAnimationSample();
}
