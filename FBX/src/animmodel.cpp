#include <animmodel.h>
#include <skinmesh.h>
#include <fbxcore.h>
#include <node.h>
#include <fbxtool.h>
#include <animlayer.h>

AnimModel::AnimModel(const std::string &filename)
	: isRunning(false), m_node(NULL), m_mesh(NULL), hasChanged(false), isFirstLoop(true)
{
	FBXCore core(filename);
	setNodePtr(core.mNode);

	m_handle = animhandle_ptr(new AnimHandle());
	m_boneUbo = std::make_shared<UBO<BoneBufferData>>(0);

	m_mesh = new SkinMesh();
	m_mesh->createMesh(m_node->getCurrentMeshNode());

	isAnimatedModel = m_node->hasAnimation();
	//m_boneData.isAnimation = isAnimatedModel;
	m_boneData.isAnimation = isAnimatedModel;
	LOG << "animated ? : " << m_boneData.isAnimation << ENDN;
}


AnimModel::~AnimModel()
{
	delete m_mesh;
}

void AnimModel::render(GLuint shader)
{
	if (!m_mesh) return;
	m_mesh->render(shader);
}

void AnimModel::processAnimation()
{
	if (!isAnimatedModel) return;

	//TODO call back for has change if gui changed(must be trigger has changed)
	if (!isRunning || hasChanged)
	{
		LOG << "has changed" << ENDN;
		AnimSample* sample = NULL;
		//Set base layer
		AnimLayer* layer= NULL;
		if (isFirstLoop) {
			layer = m_node->getAnimationLayer();
			m_node->setCurrentSample(layer->getSample(0));
			isFirstLoop = false;
		}
		//we set current sample from gui
		sample = m_node->getCurrentSample();

		long start = sample->convertFrameToMilli(sample->getSampleStart());
		long end = sample->convertFrameToMilli(sample->getSampleEnd());
		
		LOG << "start : " << start << " end : " << end  << ENDN;
		//set to current (Global time system)
		m_handle->startAnimation(GET_TIME(), start, end);
		m_handle->setLoop(true);
		isRunning = true;
		hasChanged = false;
	}
	else
	{
		m_handle->updateNodes(m_node,GET_TIME());
		updateBoneTransform(m_node);
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
	FbxAMatrix G = m_node->getCurrentMeshNode()->getGlobalTransform();
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
	m_nodePtr = node;
	m_node = m_nodePtr.get();
}

Node* AnimModel::getNode()
{
	return m_node;
}

AnimHandle* AnimModel::getHandle()
{
	return m_handle.get();
}

void AnimModel::evalCallback()
{
	//callback()
}
