#include <animmodel.h>
#include <skinmesh.h>
#include <fbxcore.h>
#include <node.h>
#include <fbxtool.h>
#include <animlayer.h>

AnimModel::AnimModel(const std::string &filename)
	: isRunning(false), m_node(NULL), m_mesh(NULL), hasChanged(false)
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

void AnimModel::processAnimation(int sampleIndex)
{
	if (!isAnimatedModel) return;
	//AnimSample* sample = mNode->getAnimationSample();
	if (hasSampleIndex != sampleIndex) {

		hasChanged = true;
	}
	if (!isRunning || hasChanged)
	{
		AnimSample* sample = NULL;
		AnimLayer* layers = m_node->getAnimationLayer();
		
		if (sampleIndex == 0) {
			sample = layers->getSample(sampleIndex);
		}
		else {
			sample = layers->getSample(sampleIndex);
		}
		
		
		m_node->setCurrentSample(sample);
		LOG << "change sample layer : " << sample->getName() << ENDN;
		/*SET SAMPLE*/
		long start = sample->convertFrameToMilli(sample->getSampleStart());
		long end = sample->convertFrameToMilli(sample->getSampleEnd());
		
		LOG << "start : " << start << " end : " << end  << ENDN;
		//set to current (Global time system)
		m_handle->startAnimation(GET_TIME(), start, end);
		m_handle->setLoop(true);
		isRunning = true;
		hasSampleIndex = sampleIndex;
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
