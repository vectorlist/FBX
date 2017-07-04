#include <animmodel.h>
#include <skinmesh.h>
#include <fbxcore.h>
#include <node.h>
#include <animlayer.h>

AnimModel::AnimModel(const std::string &filename)
	: isRunning(false), mNode(NULL), mMesh(NULL), hasChanged(false), isFirstLoop(true)
{
	FBXCore core(filename);
	setNodePtr(core.mNode);

	mHandle = animhandle_ptr(new AnimHandle());
	m_boneUbo = std::make_shared<UBO<BoneBufferData>>(0);

	mMesh = new SkinMesh();
	mMesh->createMesh(mNode->GetCurrentMeshNode());

	isAnimatedModel = mNode->HasAnimation();
	//mBoneData.isAnimation = isAnimatedModel;
	mBoneData.IsAnimation = isAnimatedModel;
	
}


AnimModel::~AnimModel()
{
	SAFE_DELETE(mMesh);
}

void AnimModel::render(GLuint shader)
{
	if (!mMesh) return;
	mMesh->render(shader);
}

void AnimModel::Update(float delta)
{
	if (!isAnimatedModel) return;

	if (!isRunning || hasChanged)
	{
		//Set Current
		if (mNode->GetAnimationLayer()->mNumSamples > 1) {
			auto* Layer = mNode->GetAnimationLayer();
			AnimSample* Sample = Layer->getSample(1);		//for idle
			mNode->SetCurrentSample(Sample);
		}
		else {
			mNode->SetCurrentSample(mNode->GetBaseSample());
		}

		//TODO : More simple
		if (hasChanged) {
			auto* Layer  = mNode->GetAnimationLayer();
			AnimSample* Sample = Layer->getSample(Layer->mCurrentSampleIndex);
			mNode->SetCurrentSample(Sample);
		}
		isRunning = true;
		hasChanged = false;
	}
	else
	{
		moveEvent();

		mHandle->UpdateNodes(mNode, delta);
		UpdateBoneTransform(mNode);
	
		//mHandle->UpdateNodes(mNode, delta);
		//UpdateBoneTransform(mNode);
	}
}

void AnimModel::UpdateBoneTransform(Node *pNode)
{
	BoneNode* rootBoneNode = pNode->GetBoneNodeRoot();
	ProcessBoneNode(rootBoneNode);
	ProcessNextBoneNode(rootBoneNode);

	m_boneUbo->updateBuffer(&mBoneData);
}

void AnimModel::ProcessBoneNode(BoneNode * rootBoneNode)
{
	//Global
	Matrix4x4 G = mNode->GetCurrentMeshNode()->GetGlobalTransform();
	for (BoneNode* node = rootBoneNode; node != NULL; node = node->mNext)
	{
		const auto local = node->GetGlobalTransform();
		//const auto tlocal = node->GetNextGlobalTransform();
		


		const auto localInv = node->GetInveseLocalTransfrom();

		int boneID = node->GetId();

		mBoneData.CurrentBones[boneID] = (local * localInv).transposed();
		//mBoneData.CurrentBones[boneID] = (local).transposed();
		
		if (node->mFirstChild)
		{
			ProcessBoneNode(node->mFirstChild);
		}
	}

	//for (BoneNode* node = rootBoneNode; node != NULL; node = node->mNext)
	//{
	//	//Matrix4x4 local;
	//	//local.rotate(AXIS::Y, 90);
	//	//local.translate(vec3f(0, 0, 3));
	//	auto local = node->GetNextGlobalTransform();
	//	//auto local = node->GetGlobalTransform();
	//	
	//	auto invLocal = node->GetInveseLocalTransfrom();
	//	mBoneData.NextBones[node->GetId()] = (local * invLocal).transposed();
	//	if (node->mFirstChild)
	//	{
	//		ProcessBoneNode(node->mFirstChild);
	//	}
	//}
	mBoneData.Weight = mHandle->mWeight;
}

void AnimModel::ProcessNextBoneNode(BoneNode * rootBoneNode)
{
	for (BoneNode* node = rootBoneNode; node != NULL; node = node->mNext)
	{
		//Matrix4x4 local;
		//local.rotate(AXIS::Y, 90);
		//local.translate(vec3f(0, 0, 3));
		const auto tlocal = node->GetNextGlobalTransform();
		//const auto local = node->GetGlobalTransform();

		/*LOG << "t : " << tlocal << ENDN;
		LOG << "l : " << local << ENDN;*/
		
		const auto invLocal = node->GetInveseLocalTransfrom();
		int boneID = node->GetId();
		
		mBoneData.NextBones[boneID] = (tlocal * invLocal).transposed();
		if (node->mFirstChild)
		{
			ProcessNextBoneNode(node->mFirstChild);
		}
	}
}

void AnimModel::setNodePtr(node_ptr &node)
{
	mNodePtr = node;
	mNode = mNodePtr.get();
}

Node* AnimModel::GetNode()
{
	return mNode;
}

AnimHandle* AnimModel::GetHandle()
{
	return mHandle.get();
}

void AnimModel::moveEvent()
{
	if(KeyHandler::isKeyPress(KEY_W))
	{
		AnimLayer* layer = mNode->GetAnimationLayer();
		AnimSample* sample = layer->getSample(2);
		mNode->SetNextSample(sample);
	}
	if (KeyHandler::isKeyUp(KEY_W))
	{
		mNode->SetNextSample(NULL);
	}

	if (KeyHandler::isKeyPress(KEY_R))
	{
		AnimLayer* layer = mNode->GetAnimationLayer();
		AnimSample* sample = layer->getSample(3);
		mNode->SetNextSample(sample);
	}
	if (KeyHandler::isKeyUp(KEY_R))
	{
		mNode->SetNextSample(NULL);
	}
}
