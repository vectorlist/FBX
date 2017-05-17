#include "fbxtool.h"
#include <log.h>
#include <bonenode.h>
#include <logger.h>

void FBXTool::buildMesh(std::vector<Vertex>& vertices, std::vector<Face>& faces, FBXMesh &mesh)
{
	LOG << "vertice size : " << vertices.size() << ENDN;
	LOG << "faces size : " << faces.size() << ENDN;
	
	std::vector<int> debugIndex;
	for (auto faceIndex = 0; faceIndex < faces.size(); ++faceIndex)
	{
		for (int facePointIndex = 0; facePointIndex < FACE_POINT_MAX; ++facePointIndex)
		{
			Face &face = faces[faceIndex];
			int vertexIndex = face.getVertexIndex(facePointIndex);
			FBXVertex vertex;
			vertex.position = vertices[vertexIndex].getPosition();
			vertex.normal = face.getNormal(facePointIndex);
			vertex.st = face.getST(facePointIndex);
			
			//TODO : bone IDs 4s
			for (int i = 0; i < BONE_COMPONENT_NUM; ++i)
			{
				vertex.boneID[i] = vertices[vertexIndex].getBoneID(i);
				//TODO : bone weight 4s
				vertex.boneWeight[i] = vertices[vertexIndex].getBoneWeight(i);
			}
			
			mesh.vertices.push_back(vertex);
			mesh.indices.push_back(faceIndex * 3 + facePointIndex);
			debugIndex.push_back(vertexIndex);
		}
	}


	mesh.buildBuffer();

	if (0) return;
	for (int i = 0; i < mesh.vertices.size(); ++i)
	{
		FBXVertex& vertex = mesh.vertices[i];
		/*LOG_W <<
			"pos : " << vertex.position <<
			" normal : " << vertex.normal <<
			" st : " << vertex.st <<
			ENDN;*/
		int index = debugIndex[i];
		LOG_W << "vertexID : " << index
			<< " boneID : " << vertex.boneID[0] << ", " << vertex.boneID[1] << ", "
			<< vertex.boneID[2] << ", " << vertex.boneID[3] << ", "
			<< " boneWeights : " << vertex.boneWeight[0] << ", " << vertex.boneWeight[1] << ", "
			<< vertex.boneWeight[2] << ", " << vertex.boneWeight[3] << ENDN;
		
	}

}


bool FBXTool::loadBoneNodes(FbxNode* pNode, BoneNode* parentNode, FBXBoneMesh &boneMesh)
{
	//we dont need bonemesh here (pass it)
	//prepare new node for bone
	BoneNode* newBoneNode = NULL;

	//get attribute FBX node
	FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
	if (pNodeAttr)
	{
		//get attrib type
		auto type = pNodeAttr->GetAttributeType();

		switch (type)
		{
		case fbxsdk::FbxNodeAttribute::eSkeleton:
			//TODO : set custom bone node tree() if we founded skeletal attr
			newBoneNode = loadBoneNode(pNode, parentNode, boneMesh);
			//LOG << "founded skeletion attr" << ENDN;
			break;
		case fbxsdk::FbxNodeAttribute::eMesh:
			//LOG << "founded mesh attr" << ENDN;
			//TODO : build mesh
			break;
		
		default:
			//LOG << "failed to find attr" << ENDN;
			//pass it other attrib
			break;
		}
	}
	//recursive
	for (int childIndex = 0; childIndex < pNode->GetChildCount(); ++childIndex)
	{
		FbxNode* pChildNode = pNode->GetChild(childIndex);
		//Recursive newboneNode is as a parent node
		loadBoneNodes(pChildNode, newBoneNode, boneMesh);
	}

	return true;
}

BoneNode* FBXTool::loadBoneNode(FbxNode* pNode, BoneNode* parent, FBXBoneMesh &boneMesh)
{
	//we got skeleton attr already convert to skeleton 
	const FbxSkeleton* pSkeleton = (FbxSkeleton*)pNode->GetNodeAttribute();

	BoneNode* boneNode = NULL;
	if (!pSkeleton) {
		LOG_ERROR("failed to convert skeleton node");
		return  NULL;
	}

	//allocate new bone node(current)
	boneNode = new BoneNode();

	//@PARAM (parent, child)
	boneMesh.addChildBoneNode(parent, boneNode);

	//set name
	auto name = pNode->GetName();
	boneNode->setName(name);

	//Transforms
	const FbxAMatrix globalTransform = pNode->EvaluateGlobalTransform(0, FbxNode::eDestinationPivot);
	boneNode->mGlobalTransform = globalTransform;

	//TODO : animation stack : pose ..etc

	//sacale , rot ..etc

	return boneNode;
}