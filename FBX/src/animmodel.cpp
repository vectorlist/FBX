#include <animmodel.h>
#include <skinmesh.h>
#include <fbxcore.h>
#include <node.h>
#include <fbxtool.h>
#include <animlayer.h>

AnimModel::AnimModel(const std::string &filename)
	: isRunning(false), mNode(NULL), mMesh(NULL), hasChanged(false), isFirstLoop(true)
{
	FBXCore core(filename);
	setNodePtr(core.mNode);

	m_handle = animhandle_ptr(new AnimHandle());
	m_boneUbo = std::make_shared<UBO<BoneBufferData>>(0);

	mMesh = new SkinMesh();
	mMesh->createMesh(mNode->getCurrentMeshNode());

	isAnimatedModel = mNode->hasAnimation();
	//m_boneData.isAnimation = isAnimatedModel;
	m_boneData.isAnimation = isAnimatedModel;
	LOG << "animated ? : " << m_boneData.isAnimation << ENDN;
}


AnimModel::~AnimModel()
{
	delete mMesh;
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
		mNode->setCurrentSample(mNode->getBaseSample());
		LOG << "changed" << ENDN;
		if (hasChanged) {
			auto* Layer  = mNode->getAnimationLayer();
			AnimSample* Sample = Layer->getSample(Layer->mIndex);
			mNode->setCurrentSample(Sample);
		}
		isRunning = true;
		hasChanged = false;
	}
	else
	{
		
		m_handle->updateNodes(mNode, delta);
		updateBoneTransform(mNode);
	}
}

void AnimModel::updateBoneTransform(Node *pNode)
{
	BoneNode* rootBoneNode = pNode->getBoneNodeRoot();

	processBoneNode(rootBoneNode);
	m_boneUbo->updateBuffer(&m_boneData);
}

void AnimModel::processBoneNode(BoneNode * rootBoneNode)
{
	//Global
	FbxAMatrix G = mNode->getCurrentMeshNode()->getGlobalTransform();
	for (BoneNode* node = rootBoneNode; node != NULL; node = node->mNext)
	{
		//Lcal
		auto L = node->getGlobalTransfrom();
		//Local Inv
		auto LInv = node->getInveseLocalTransfrom();
		int boneID = node->id();

		m_boneData.bones[boneID] = Matrix4x4(L * LInv * G);
		if (node->mFirstChild)
		{
			processBoneNode(node->mFirstChild);
		}
	}
}

void AnimModel::setNodePtr(node_ptr &node)
{
	mNodePtr = node;
	mNode = mNodePtr.get();
}

Node* AnimModel::getNode()
{
	return mNode;
}

AnimHandle* AnimModel::getHandle()
{
	return m_handle.get();
}

void AnimModel::evalCallback()
{
	//callback()
}
