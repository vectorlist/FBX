#include "FBXImport.h"
#include "fmesh.h"
#include "MeshNode.h"
#include "BoneNode.h"
#include "Face.h"
#include "Vertex.h"
#include "AnimationInfo.h"
#include "MathsUtils.h"
#include "VectorKey.h"
#include "QuaternionKey.h"
#include <log.h>
//#include <boost/filesystem.hpp>

#include <assert.h>

#include <log.h>


const std::string FBXImport::m_dummyTexture("Blank.tga");

FBXImport::FBXImport()
: m_fbxManager(NULL),
m_fbxScene(NULL),
m_mesh(NULL)
{
}

FBXImport::~FBXImport()
{
	m_mesh = NULL;
	m_meshNodeInfo.m_meshNode.clear();
	m_meshNodeInfo.m_fbxMeshNode.clear();
}

MeshPtr FBXImport::Import(
	const std::string &fbxFilename
	)
{
	m_fbxManager = FbxManager::Create();
	if(!m_fbxManager)
	{
		LOG_ERROR("Can't create fbx manager.\n");
		return NULL;
	}

	// Set up the import settings
	m_ioSettings = FbxIOSettings::Create(m_fbxManager, IOSROOT);
	m_fbxManager->SetIOSettings(m_ioSettings);

	//Create an FBX scene which holds the imported objects
	m_fbxScene = FbxScene::Create(m_fbxManager, "My Scene");
	if(!m_fbxScene)
	{
		LOG_ERROR("Can't create fbx scene.\n");
		return NULL;
	}

	m_fbxImporter = FbxImporter::Create(m_fbxManager,"");

	if(!m_fbxImporter->Initialize(fbxFilename.c_str(), -1, m_ioSettings))
	{
		FbxString error = m_fbxImporter->GetStatus().GetErrorString();
		LOG_ERROR("Call to FbxImporter::Initialize() failed.\n");
		LOG_ERROR("FBX Import failed with error: %s\n", error.Buffer());

		if (m_fbxImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
		{
			LOG_ERROR("FBX file format version for file '%s' is not valid for this SDK version\n", fbxFilename);
		}

		DestroyFBXManagers();

		return NULL;
	}

	if(!m_fbxImporter->IsFBX())
	{
		LOG_ERROR("File %s is not an FBX file. \n", fbxFilename);
		DestroyFBXManagers();
		return NULL;
	}

	FbxAxisSystem SceneAxisSystem = m_fbxScene->GetGlobalSettings().GetAxisSystem();
	//FbxAxisSystem OurAxisSystem(FbxAxisSystem::eZAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
	//FbxAxisSystem OurAxisSystem(FbxAxisSystem::eMax);
	if (SceneAxisSystem != FbxAxisSystem::MayaZUp)
	{
		LOG << "change scene" << ENDN;
		FbxAxisSystem::MayaZUp.ConvertScene(m_fbxScene);
	}

	// Import the scene.
	if(!m_fbxImporter->Import(m_fbxScene))
	{
		LOG_ERROR("Import failed for file: %s \n", fbxFilename);
		DestroyFBXManagers();
		return NULL;
	}


	// Fill the mesh with the imported data
	m_mesh = MeshPtr(new Mesh());
	FbxNode &fbxRootNode = *m_fbxScene->GetRootNode();

	if (!LoadAnimationLayerInfo())
	{
		LOG_ERROR("Import failed for file: %s \n", fbxFilename);
		DestroyFBXManagers();
		return NULL;
	}
	AnimationInfoPtr animationInfo = m_mesh->GetAnimationInfo();

	// Bake all FBX transforms ie pivots and offsets into standard scale, rotation etc
	// See http://docs.autodesk.com/FBX/2014/ENU/FBX-SDK-Documentation/index.html?url=files/GUID-C35D98CB-5148-4B46-82D1-51077D8970EE.htm,topicNumber=d30e8813
	BakeNodeTransforms(fbxRootNode);
	fbxRootNode.ConvertPivotAnimationRecursive(NULL, 
		FbxNode::eDestinationPivot, animationInfo->GetFPS());	// FPS fixed at 30 atm

	LoadNodes(fbxRootNode, m_mesh->GetBoneNodeHierarchy(), m_mesh->GetMeshNodeHierarchy());

	// TODO proper error reporting.
	if (!LoadSkin())
	{
		LOG_ERROR("Import failed for file: %s \n", fbxFilename);
		DestroyFBXManagers();
		return NULL;
	}

	// Now the all the animation is loaded adjust the time to make sure that it starts at zero
	animationInfo->SetEndSample(animationInfo->GetEndSample() - animationInfo->GetStartSample());
	animationInfo->SetStartSample(0);

	LOG << "sample length : " << animationInfo->GetNumFrames() << ENDN;
	// TODO remove duplicate keys

	DestroyFBXManagers();

	return m_mesh;
}

void FBXImport::DestroyFBXManagers()
{
	m_fbxImporter->Destroy();
	m_fbxScene->Destroy();
	m_ioSettings->Destroy();
	m_fbxManager->Destroy();

	m_fbxImporter = NULL;
	m_fbxScene = NULL;
	m_ioSettings = NULL;
	m_fbxManager = NULL;
}

bool FBXImport::LoadNodes(
	FbxNode& fbxNode,
	BoneNode *parentBoneNode,
	MeshNode *parentMeshNode
	)
{
	MeshNode *newMeshNode = NULL;
	BoneNode *newBoneNode = NULL;

	// Find ut the type of node ie Skeleton, Mesh, Camera, Light - currently only Bones and Mesh nodes are supported
	FbxNodeAttribute* const fbxNodeAttribute = fbxNode.GetNodeAttribute();
	if(fbxNodeAttribute)
	{
		const FbxNodeAttribute::EType fbxAttributeType = fbxNodeAttribute->GetAttributeType();

		switch(fbxAttributeType)
		{
		case FbxNodeAttribute::eMesh:
			newMeshNode = LoadMeshNode(fbxNode, parentMeshNode);
			break;
		case FbxNodeAttribute::eSkeleton:
			newBoneNode = LoadBoneNode(fbxNode, parentBoneNode);
			break;
		default:
			//LOG_ERROR("Node %s type is %d. Only node of type eMesh (4) or eSkeleton (3) can be loaded\n", fbxNode.GetName(), fbxAttributeType);

			break;

		}

	}

	for(int childIndex = 0; childIndex < fbxNode.GetChildCount(); childIndex++)
	{
		FbxNode &fbxChildNode = *fbxNode.GetChild(childIndex);

		LoadNodes(fbxChildNode, newBoneNode != NULL ? newBoneNode : parentBoneNode, newMeshNode != NULL ? newMeshNode : parentMeshNode); // If we didnt load a node just pass through the parent. Otherwise pass the new node as the parent
	}

	return true;
}

MeshNode *FBXImport::LoadMeshNode(
	FbxNode& fbxNode,
	MeshNode *parent
	)
{
	FbxMesh* fbxMesh = fbxNode.GetMesh();
	MeshNode *meshNode = NULL;
	if(fbxMesh)
	{
		//if(m_meshNodeInfo.m_meshNode != NULL)
		//{
		//	LOG_ERROR("Ignoring attempt to load mesh - more than one mesh is not supported.\n");
		//	return NULL; // We have already loaded a mesh so ignore this one
		//}

		if(!fbxMesh->IsTriangleMesh())
		{
			// Make sure the mesh is triangulated
			FbxGeometryConverter fbxGeometryConverter(m_fbxManager);	
			FbxNodeAttribute* fbxNodeAttribute = fbxGeometryConverter.Triangulate(fbxNode.GetMesh(), true);
			if(!fbxNodeAttribute)
			{
				LOG_ERROR("Mesh Triangulation failed. Node Import aborted.\n");
				return NULL;
			}
			fbxMesh = (FbxMesh *)fbxNodeAttribute;
		}

		meshNode = new MeshNode();
		m_mesh->AddChildMeshNode(parent, meshNode);

		m_meshNodeInfo.m_meshNode.push_back(meshNode);	// We are only supporting one mesh, so store it to add loading the skin
		m_meshNodeInfo.m_fbxMeshNode.push_back(&fbxNode);

		std::string name = fbxNode.GetName();
		meshNode->SetName(name);

		const FbxAMatrix fbxGlobalTransform = fbxNode.EvaluateGlobalTransform();
		FbxAMatrix fbxLocalTransform;
		FbxVector4 position = fbxNode.LclTranslation.Get();
		FbxVector4 rotation = fbxNode.LclRotation.Get();
		FbxVector4 scale = fbxNode.LclScaling.Get();

		fbxLocalTransform.SetTRS(position, rotation, scale);
		meshNode->m_globalTransform = fbxLocalTransform;

		// Extract and store vertices
		const unsigned int numVertices = fbxMesh->GetControlPointsCount();
		meshNode->AllocateVertices(numVertices);
		MeshVertexArrayPtr vertexArray = meshNode->GetVertices();

		const FbxVector4* const fbxVertices = fbxMesh->GetControlPoints();
		for(unsigned int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
		{
			vertexArray[vertexIndex].SetPosition(fbxVertices[vertexIndex]);
		}

		// Extract and store triangles
		int numTriangles = fbxMesh->GetPolygonCount();

		meshNode->AllocateTriangles(numTriangles);
		MeshTriangleArrayPtr triangleArray = meshNode->GetTriangles();

			// Get the per triangle material index
		const int materialLayerCount = fbxMesh->GetElementMaterialCount();
		if(materialLayerCount > 1)
		{
			LOG_ERROR("Only one material layer is supported. Only the first layer will be loaded.\n");
		}

		// Load per triangle data
		for(int triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
		{
			int triangleSize = fbxMesh->GetPolygonSize(triangleIndex);
			assert(triangleSize == 3); // The mesh was triangulated above. Maybe sure it really is triangulated.

			Face &triangle = triangleArray[triangleIndex];

			if(materialLayerCount != 0)
			{
				//LoadMaterials(*fbxMesh, triangleIndex, *meshNode, meshNode->GetAppearanceTable(), meshNode->GetNumVerticesPerMaterialArray());
			}
			else
			{
				// There is no material on the mesh. Create and assign a dummy material for this triangle.
				//AssignDummyMaterial(triangleIndex, *meshNode, meshNode->GetAppearanceTable(), meshNode->GetNumVerticesPerMaterialArray());
			}
			LoadVertexIndices(*fbxMesh, triangleIndex, triangle);

			LoadColours(*fbxMesh, triangleIndex, triangle);
			LoadUVs(*fbxMesh, triangleIndex, triangle);

			LoadNormals(*fbxMesh, triangleIndex, triangle);
			LoadBinormals(*fbxMesh, triangleIndex, triangle);		
			LoadTangents(*fbxMesh, triangleIndex, triangle);
		}
	}

	return meshNode;
}

BoneNode *FBXImport::LoadBoneNode(
	FbxNode& fbxNode, // The FBX mesh to extract data from and add to m_mesh bone node list
	BoneNode *parent
	)
{
	const FbxSkeleton* const fbxSkeleton = static_cast<const FbxSkeleton*>
		(fbxNode.GetNodeAttribute());
	BoneNode *boneNode = NULL;
	if (!fbxSkeleton)
	{
		LOG_ERROR("Could not extract skeleton from node");
		return NULL;
	}

	// Add the bone to the mesh as a parent->child list
	boneNode = new BoneNode();

	m_mesh->AddChildBoneNode(parent, boneNode);

	std::string name = fbxNode.GetName();
	boneNode->SetName(name);
	LOG << boneNode->GetName() << ENDN;
	// Store the initial global pose values. The rest will be calulated from the local transforms //TODO do we even need to load the global transforms at all?
	const FbxAMatrix fbxGlobalTransform = fbxNode.EvaluateGlobalTransform(0, FbxNode::eDestinationPivot);
	boneNode->m_globalTransform = fbxGlobalTransform;

	//TODO temp to debug
	//glm::mat4x4 initialLocalTransform;
	//const FbxAMatrix fbxInitialLocalTransform = fbxNode.EvaluateLocalTransform(0, FbxNode::eDestinationPivot);
	//boneNode->m_localTransform = fbxInitialLocalTransform;

	AnimationInfoPtr animationInfo = m_mesh->GetAnimationInfo();
	// Load in the local keys transoforms for each key
	int numSamples = animationInfo->GetNumFrameSamples();
	//double frameRate = animationInfo->GetFPS();

	if(numSamples)
	{
		boneNode->AllocateAnimationTracks(numSamples);
	}

	int startFrame = animationInfo->GetStartSample();
	long startTime = animationInfo->ConvertFrameToMilliseconds(startFrame);
	
	FbxTime fbxTime;

	for(int sample = 0; sample <= numSamples; sample++)
	{
		long sampleTime = animationInfo->ConvertFrameToMilliseconds(sample); // Get the number of milliseconds into the animation for this sample frame (starting at 0)
		fbxTime.SetMilliSeconds(startTime + sampleTime); // Find the current time in the FBX file (starting at anim start time)

		const FbxAMatrix fbxLocalTransform = fbxNode.EvaluateLocalTransform(
			fbxTime, FbxNode::eDestinationPivot);

		// Store the keys with the adjusted time (ie the time starting at 0 regardless or where it started in the FBX file
		VectorKey scaleKey(fbxLocalTransform.GetS(), sampleTime);
		VectorKey positionKey(fbxLocalTransform.GetT(), sampleTime);

		// glm quat constructor expects w, x, y, z. FBX is x, y, z, w. glm nontheless stores x, y, z, w internally
		QuaternionKey rotationKey(fbxLocalTransform.GetQ(), sampleTime);

		boneNode->AddPositionKey(positionKey);
		boneNode->AddRotationKey(rotationKey);
		boneNode->AddScaleKey(scaleKey);
	}

	// Record node scale inheritance
	if (parent)
	{
		FbxEnum inheritType = fbxNode.InheritType.Get();
		switch(inheritType)
		{
		case FbxTransform::eInheritRrs:
			boneNode->SetInheritScale(false);
			break;
		case FbxTransform::eInheritRrSs:
			LOG_ERROR("Unsupported scale type used");
			break;
		case FbxTransform::eInheritRSrs:
			boneNode->SetInheritScale(true);
			break;
		}
	}
	else
	{
		boneNode->SetInheritScale(false);// No parent, cant inherit scale
	}

	return boneNode;
}

bool FBXImport::LoadAnimationLayerInfo()
{
	// Get the fps and calculate the number of frames that should be loaded
	AnimationInfoPtr animationInfo = AnimationInfoPtr(new AnimationInfo());

	const FbxTakeInfo *takeInfo = m_fbxImporter->GetTakeInfo(0);
	if (!takeInfo)
	{
		LOG_ERROR("Unsupported animation format. Animation does not contain take info. Aborting.");
		return false;
	}

	// Set the frame rate as millisecond to frame conversion is based on this
	double frameRate = 30.0f;
	FbxTime::EMode fbxTimeMode;
	if (m_fbxImporter->GetFrameRate(fbxTimeMode))
	{
		frameRate = FbxTime::GetFrameRate(fbxTimeMode);
	}

	animationInfo->SetFPS(frameRate);

	// Set the start and end frame. Use frame and not time to prevent rounding errors
	const long startTime = takeInfo->mLocalTimeSpan.GetStart().GetMilliSeconds();
	const long endTime = takeInfo->mLocalTimeSpan.GetStop().GetMilliSeconds();
	animationInfo->SetStartSample(animationInfo->ConvertMillisecondsToFrame(startTime));
	animationInfo->SetEndSample(animationInfo->ConvertMillisecondsToFrame(endTime));

	int numFrames = animationInfo->GetEndSample() - animationInfo->GetStartSample() - 1; // This will return the number of frame samples ie |-|-|-|-| (where | is a sample). So -1 for the number of frames (where - is a frame)

	animationInfo->SetNumFrames(numFrames);

	m_mesh->SetAnimationInfo(animationInfo);

	return true;
}

bool FBXImport::LoadSkin()
{
	bool loadedSkin = false;
	int numMeshNodes = m_meshNodeInfo.m_fbxMeshNode.size();
	if(numMeshNodes == 0)
	{
		LOG_ERROR("Loading skin failed - there are no mesh loaded mesh nodes");
		return false;
	}

	for(int meshIndex = 0; meshIndex < numMeshNodes; meshIndex++)
	{
		FbxGeometry* const fbxGeometry = m_meshNodeInfo.m_fbxMeshNode[meshIndex]->GetGeometry();
		if (!fbxGeometry)
		{
			LOG_ERROR("Unsupported skin type used");
			continue;
		}

		const FbxSkin* const fbxskin = static_cast<const FbxSkin*>
			(fbxGeometry->GetDeformer(0, FbxDeformer::eSkin));
		if (!fbxskin)
		{
			LOG_ERROR("Skin deformer is null");
			continue;
		}

		const FbxSkin::EType fbxSkinningType = fbxskin->GetSkinningType();
		switch(fbxSkinningType)
		{
		case FbxSkin::eLinear:
		case FbxSkin::eRigid:
		case FbxSkin::eDualQuaternion: //TODO dual quaterion? does this work???!
			if (LoadSkin(*fbxGeometry, *m_meshNodeInfo.m_meshNode[meshIndex]))
			{
				loadedSkin = true;
			}

			continue;

		default:
			LOG_ERROR("Unsupported skin type used");
		}
	}

	return loadedSkin;
}

bool FBXImport::LoadSkin(
	const FbxGeometry &fbxGeometry,// The FBX mesh geometry node to extract data from
	MeshNode &meshNode //Mesh node to store this skin data
	)
{
	const int numVertices = fbxGeometry.GetControlPointsCount();

	// For all skins and all clusters, accumulate their deformation and weight on each vertex and store them in 'clusterDeformation' and 'clusterWeight'
	const int numSkins = fbxGeometry.GetDeformerCount(FbxDeformer::eSkin);

	for (int skinIndex = 0; skinIndex < numSkins; skinIndex++)
	{
		FbxSkin *skin = static_cast<FbxSkin*>(fbxGeometry.GetDeformer(skinIndex, FbxDeformer::eSkin));
        
		// Cluster = group of vertices affected by this bone
		const int numClusters = skin->GetClusterCount();
		for(int clusterNum = 0; clusterNum < numClusters; clusterNum++)
		{
			FbxCluster *cluster = skin->GetCluster(clusterNum);

			// Check this cluster's deformation mode
			const FbxCluster::ELinkMode fbxClusterMode = cluster->GetLinkMode();
			switch(fbxClusterMode)
			{
			case FbxCluster::eNormalize:	// Weights have been normalized
			case FbxCluster::eTotalOne:		// Weights add up to one
				break;

			default: // Currently this must be eAdditive - weights could add up to more than 1 - not currently supported
				LOG_ERROR("Unsupported skin cluster type used. Please ensure that the skin weights total one or have been normalized.");
			}

			// The bone node that this cluster is influenced by
			FbxNode* const fbxLinkedBoneNode = cluster->GetLink();
			if (!fbxLinkedBoneNode)
			{
				LOG_ERROR("Failed to find linked bone node importing skin");
				continue;
			}

			std::string temp(fbxLinkedBoneNode->GetName());
			BoneNode* linkedBone = m_mesh->GetBoneByName(fbxLinkedBoneNode->GetName());
			assert(linkedBone);
			// Store the inverse reference matrix for the bone this cluster is connected to
			FbxAMatrix boneReferenceMatrix;
			cluster->GetTransformLinkMatrix(boneReferenceMatrix);

			FbxAMatrix inverseBoneReferenceMatrix;
			inverseBoneReferenceMatrix = boneReferenceMatrix.Inverse();

			linkedBone->SetInverseReferenceMatrix(inverseBoneReferenceMatrix);

			meshNode.FlagAsSkinned(true);

			const int numClusterIndices = cluster->GetControlPointIndicesCount();
			const int *controlPointIndices = cluster->GetControlPointIndices();
			const double *controlPointWeights = cluster->GetControlPointWeights();

			// Store the weights for each control point
			MeshVertexArrayPtr vertexArray = meshNode.GetVertices();
			for(int clusterIndex = 0; clusterIndex < numClusterIndices; clusterIndex++)
			{
				const int controlPointIndex = controlPointIndices[clusterIndex];
				assert(controlPointIndex < numVertices);
				/*LOG << controlPointIndex << ENDN;*/
				const float controlPointWeight = static_cast<float>(controlPointWeights[clusterIndex]);
				if (controlPointWeight < 0.00001f) // Ignore really small weights
					continue;

				if (!vertexArray[controlPointIndex].AddWeight(linkedBone->m_id, controlPointWeight))
				{
					return false;
				}

				//LOG << controlPointIndex << " w : " << controlPointWeight << ENDN;
			}
		}
	}

	return true;
}

/**
	\brief Bake all possible FBX transforms into the standard rotation, translate and scale variables so the can be extracted easily later
*/

void FBXImport::BakeNodeTransforms(
	FbxNode &node		//!< Node to bake transforms for
	) const
{
	FbxVector4 zeroVector(0,0,0);
 
	// Activate pivot converting 
	node.SetPivotState(FbxNode::eSourcePivot, FbxNode::ePivotActive); 
	node.SetPivotState(FbxNode::eDestinationPivot, FbxNode::ePivotActive); 
 
	// We want to set all these to 0 and bake them into the transforms. 
	node.SetPostRotation(FbxNode::eDestinationPivot, zeroVector); 
	node.SetPreRotation(FbxNode::eDestinationPivot, zeroVector); 
	node.SetRotationOffset(FbxNode::eDestinationPivot, zeroVector); 
	node.SetScalingOffset(FbxNode::eDestinationPivot, zeroVector); 
	node.SetRotationPivot(FbxNode::eDestinationPivot, zeroVector); 
	node.SetScalingPivot(FbxNode::eDestinationPivot, zeroVector); 
 
	// This is to import in a system that supports rotation order. 
	// If rotation order is not supported, do this instead: 
	node.SetRotationOrder(FbxNode::eDestinationPivot, eEulerXYZ); 
 
	// Similarly, this is the case where geometric transforms are supported by the system. 
	// If geometric transforms are not supported, set them to zero instead of the source’s geometric transforms. 
	// Geometric transform = local transform, not inherited by children. 
	node.SetGeometricTranslation(FbxNode::eDestinationPivot,
		node.GetGeometricTranslation(FbxNode::eSourcePivot)); 
	node.SetGeometricRotation(FbxNode::eDestinationPivot, 
		node.GetGeometricRotation(FbxNode::eSourcePivot)); 
	node.SetGeometricScaling(FbxNode::eDestinationPivot,
		node.GetGeometricScaling(FbxNode::eSourcePivot)); 
 
	// Idem for quaternions. 
	node.SetQuaternionInterpolation(FbxNode::eDestinationPivot, node.GetQuaternionInterpolation(FbxNode::eSourcePivot));

	// Recurse on children
	const int childCount = node.GetChildCount();
	for(int childNum = 0; childNum < childCount; childNum++)
		BakeNodeTransforms(*node.GetChild(childNum));
}

/**
	@brief Loads a material id for the given triangle
*/
//void FBXImport::LoadMaterials(
//	FbxMesh &fbxMesh,			// FBX mesh to import data from
//	int triangleIndex,			// Index of the current triangle being loaded
//	MeshNode &meshNode,	// The mesh node which will store the imported material and texture data
//	render::AppearanceTable &appearanceTable,
//	std::vector<unsigned int> &numVerticesPerMaterial
//	)
//{
//	const FbxGeometryElementMaterial &materialElement = *fbxMesh.GetElementMaterial(0); // Get the first material layer element. Only one is supported.
//	const int materialId = materialElement.GetIndexArray().GetAt(triangleIndex);
//	if (materialId >= 0)
//	{
//		const FbxSurfaceMaterial& surfaceMaterial = *fbxMesh.GetNode()->GetMaterial(materialId);
//		meshNode.m_triangleArray[triangleIndex].SetMaterialId(materialId);
//
//		if(appearanceTable.count(materialId) == 0)
//		{
//			numVerticesPerMaterial.push_back(0);// Add another int initialised at 0 for this material
//			render::AppearancePtr appearance;
//
//			if (surfaceMaterial.GetClassId().Is(FbxSurfacePhong::ClassId))
//			{
//				appearance = render::AppearancePtr(new render::PhongAppearance());
//					
//				const FbxSurfacePhong& phongMaterial = (const FbxSurfacePhong &)surfaceMaterial;
//				AddPhongMaterial(appearance, phongMaterial);
//			}
//			else if(surfaceMaterial.GetClassId().Is(FbxSurfaceLambert::ClassId))
//			{
//				appearance = render::AppearancePtr(new render::LambertAppearance());
//
//				const FbxSurfaceLambert& lambertMaterial = (const FbxSurfaceLambert &)surfaceMaterial;
//
//				AddLambertMaterial(appearance, lambertMaterial);
//			}
//			else
//			{
//				LOG_ERROR("Material Id %d, name &s is not supported", materialId, surfaceMaterial.GetName());
//				//TODO assign default material
//			}
//
//			// Just get the diffuse for now. Will load normal/bump and other textures here in future.
//			FbxProperty materialProperty = surfaceMaterial.FindProperty(FbxSurfaceMaterial::sDiffuse);
//			unsigned int textureCount = materialProperty.GetSrcObjectCount<FbxTexture>();
//			if(textureCount > 1)
//			{
//				LOG_ERROR("Currently only 1 diffuse texture is supported. Others have not been loaded", materialId, surfaceMaterial.GetName());
//			}
//			for(int textureIndex = 0; textureIndex < 1; textureIndex++)
//			{
//				FbxFileTexture* fbxFileTexture = materialProperty.GetSrcObject<FbxFileTexture>(textureIndex);
//				if(fbxFileTexture != NULL && boost::filesystem::exists(fbxFileTexture->GetFileName()))
//				{
//					appearance->SetDiffuseTexturePath(fbxFileTexture->GetFileName());
//				}
//				else
//				{
//					appearance->SetDiffuseTexturePath(m_dummyTexture);
//				}
//			}
//
//			render::AppearanceTableEntry materialInfo;
//			materialInfo.first = materialId;
//			materialInfo.second = appearance;
//			appearanceTable.insert(materialInfo);
//		}
//		numVerticesPerMaterial[materialId]+=3;
//	}
//	else
//	{
//		LOG_ERROR("Face %d has no associated material.\n", triangleIndex);
//	}
//}
//
///**
//	@brief Creates a dummy material and assigns it to the mesh appearance table
//*/
//void FBXImport::AssignDummyMaterial(
//	int triangleIndex,			// Index of the current triangle being loaded
//	MeshNode &meshNode,	// The mesh node which will store the imported material and texture data
//	render::AppearanceTable &appearanceTable,
//	std::vector<unsigned int> &numVerticesPerMaterial
//	)
//{
//	render::AppearanceTableEntry materialInfo;
//
//	const int materialId = 0;
//	meshNode.m_triangleArray[triangleIndex].SetMaterialId(materialId);
//
//	numVerticesPerMaterial.push_back(0);// Add another int initialised at 0 for this material
//	render::AppearancePtr appearance = render::AppearancePtr(new render::LambertAppearance());
//
//	const FbxSurfaceLambert& lambertMaterial = *FbxSurfaceLambert::Create(m_fbxManager, "DummyMaterial");
//	AddLambertMaterial(appearance, lambertMaterial);
//
//	materialInfo.first = materialId;
//	materialInfo.second = appearance;
//	appearanceTable.insert(materialInfo);
//}
//
///**
//	@brief Extracts info from an fbx phong material and stores it in the appearance table
//*/
//void FBXImport::AddPhongMaterial(
//	render::AppearancePtr appearance,// Appeararnce to store the data in
//	const FbxSurfacePhong& phongMaterial// FBX phong material to load info from
//)
//{
//	vec3f ambientColour(phongMaterial.Ambient.Get()[0], phongMaterial.Ambient.Get()[1], phongMaterial.Ambient.Get()[3]);
//	vec3f diffuseColour(phongMaterial.Diffuse.Get()[0], phongMaterial.Diffuse.Get()[1], phongMaterial.Diffuse.Get()[3]);
//	vec3f specularColour(phongMaterial.Specular.Get()[0], phongMaterial.Specular.Get()[1], phongMaterial.Specular.Get()[3]);
//	vec3f emmissiveColour(phongMaterial.Emissive.Get()[0], phongMaterial.Emissive.Get()[1], phongMaterial.Emissive.Get()[3]);
//
//	double transparency = phongMaterial.TransparencyFactor.Get();
//	double shininess = phongMaterial.Shininess.Get();
//	double reflectivity = phongMaterial.ReflectionFactor.Get();
//	double diffuseFactor = phongMaterial.DiffuseFactor.Get();
//
//	boost::shared_ptr<render::PhongAppearance> phongAppearancePtr = boost::static_pointer_cast<render::PhongAppearance>(appearance);
//
//	phongAppearancePtr->SetAmbient(ambientColour);
//	phongAppearancePtr->SetDiffuse(diffuseColour);
//	phongAppearancePtr->SetSpecular(specularColour);
//	phongAppearancePtr->SetEmissive(emmissiveColour);
//
//	phongAppearancePtr->SetTransparency(transparency);
//	phongAppearancePtr->SetShininess(shininess);
//	phongAppearancePtr->SetReflectivity(reflectivity);
//	phongAppearancePtr->SetDiffuseFactor(diffuseFactor);
//}
//
///**
//	@brief Extracts info from an fbx lambert material and stores it in the appearance table
//*/
//void FBXImport::AddLambertMaterial(
//	render::AppearancePtr appearance, // Appeararnce to store the data in
//	const FbxSurfaceLambert& lambertMaterial // FBX lambert material to load info from
//	)
//{
//	vec3f ambientColour(lambertMaterial.Ambient.Get()[0], lambertMaterial.Ambient.Get()[1], lambertMaterial.Ambient.Get()[3]);
//	vec3f diffuseColour(lambertMaterial.Diffuse.Get()[0], lambertMaterial.Diffuse.Get()[1], lambertMaterial.Diffuse.Get()[3]);
//	vec3f emmissiveColour(lambertMaterial.Emissive.Get()[0], lambertMaterial.Emissive.Get()[1], lambertMaterial.Emissive.Get()[3]);
//
//	double transparency = lambertMaterial.TransparencyFactor.Get();
//	double diffuseFactor = lambertMaterial.DiffuseFactor.Get();
//	boost::shared_ptr<render::LambertAppearance> lambertAppearancePtr = boost::static_pointer_cast<render::LambertAppearance>(appearance);
//
//	lambertAppearancePtr->SetAmbient(ambientColour);
//	lambertAppearancePtr->SetDiffuse(diffuseColour);
//	lambertAppearancePtr->SetEmissive(emmissiveColour);
//
//	lambertAppearancePtr->SetTransparency(transparency);
//	lambertAppearancePtr->SetDiffuseFactor(diffuseFactor);
//}

/**
	@brief Loads the vertex Indices for the given triangle
*/
void FBXImport::LoadVertexIndices(
	FbxMesh &fbxMesh,			// FBX mesh to import data from
	int triangleIndex,			// Index of the current triangle being loaded
	Face &triangle	// The current triangle to store the imported data
	)
{
	// Get the index into the vertex array for this vertex on this polygon
	for (int triangleCornerId = 0; triangleCornerId < 3; triangleCornerId++)
	{
		int vertexIndex = fbxMesh.GetPolygonVertex(triangleIndex, triangleCornerId);
		triangle.SetVertexIndex(triangleCornerId, vertexIndex);
	}
}

/**
	@brief Loads the colours at each corner of the given triangle
*/
void FBXImport::LoadColours(
	FbxMesh &fbxMesh,			// FBX mesh to import data from
	int triangleIndex,			// Index of the current triangle being loaded
	Face &triangle	// The current triangle to store the imported data
	)
{
	if(fbxMesh.GetElementVertexColorCount() > 1)
	{
		LOG_ERROR("Only one set of vertex colours supported\n");
	}

	FbxGeometryElementVertexColor* vertexColourElement = fbxMesh.GetElementVertexColor();
	if(vertexColourElement)
	{
		for (int triangleCornerId = 0; triangleCornerId < 3; triangleCornerId++)
		{
			int vertexIndex = fbxMesh.GetPolygonVertex(triangleIndex, triangleCornerId);
			vec3f colour;
			LoadColourVertexElement(*vertexColourElement, colour, triangleCornerId, vertexIndex);
			triangle.SetColour(colour, triangleCornerId);
		}
	}
}

/**
	@brief Loads the uvs at each corner of the given triangle
*/
void FBXImport::LoadUVs(
	FbxMesh &fbxMesh,			// FBX mesh to import data from
	int triangleIndex,			// Index of the current triangle being loaded
	Face &triangle	// The current triangle to store the imported data
	)
{
	if(fbxMesh.GetElementUVCount() != 1)
	{
		LOG_ERROR("Mesh must have one sest of uvs. This mesh has %d uv sets.\n", fbxMesh.GetElementUVCount()); // May support more uv sets later
	}

	FbxGeometryElementUV* uvElement = fbxMesh.GetElementUV(/*uvIndex*/);
	if(uvElement)
	{
		for (int triangleCornerId = 0; triangleCornerId < 3; triangleCornerId++)
		{
			int vertexIndex = fbxMesh.GetPolygonVertex(triangleIndex, triangleCornerId);
			vec2f uv;
			LoadVector2VertexElement(*uvElement, uv, triangleIndex, triangleCornerId, vertexIndex);
			triangle.SetUV(uv, triangleCornerId);
		}
	}
}

/**
	@brief Loads the normals at each corner of the given triangle
*/
void FBXImport::LoadNormals(
	FbxMesh &fbxMesh,			// FBX mesh to import data from
	int triangleIndex,			// Index of the current triangle being loaded
	Face &triangle	// The current triangle to store the imported data
	)
{
	if(fbxMesh.GetElementNormalCount() > 1)
	{
		LOG_ERROR("Only one set of normals currently supported\n");
	}

	FbxGeometryElementNormal* normalElement = fbxMesh.GetElementNormal();
	if(normalElement)
	{
		for (int triangleCornerId = 0; triangleCornerId < 3; triangleCornerId++)
		{
			int vertexIndex = fbxMesh.GetPolygonVertex(triangleIndex, triangleCornerId);
			vec4f normal;
			LoadVector4VertexElement(*normalElement, normal, (triangleIndex * 3) + triangleCornerId, vertexIndex);
			vec3f fixNormal = vec3f(normal.x, normal.y, normal.z);
			triangle.SetNormal(fixNormal, triangleCornerId);
		}
	}
}

/**
	@brief Loads the binormals at each corner of the given triangle
*/
void FBXImport::LoadBinormals(
	FbxMesh &fbxMesh,			// FBX mesh to import data from
	int triangleIndex,			// Index of the current triangle being loaded
	Face &triangle	// The current triangle to store the imported data
	)
{
	if(fbxMesh.GetElementBinormalCount() > 1)
	{
		LOG_ERROR("Only one set of binormals currently supported\n");
	}

	FbxGeometryElementBinormal* binormalElement = fbxMesh.GetElementBinormal();
	if(binormalElement)
	{
		for (int triangleCornerId = 0; triangleCornerId < 3; triangleCornerId++)
		{
			int vertexIndex = fbxMesh.GetPolygonVertex(triangleIndex, triangleCornerId);
			vec4f binormal;
			LoadVector4VertexElement(*binormalElement, binormal, triangleCornerId, vertexIndex);
			triangle.SetBinormal(binormal, triangleCornerId);
		}
	}	
}

/**
	@brief Loads the tangents at each corner of the given triangle
*/
void FBXImport::LoadTangents(
	FbxMesh &fbxMesh,			// FBX mesh to import data from
	int triangleIndex,			// Index of the current triangle being loaded
	Face &triangle	// The current triangle to store the imported data
	)
{
	if(fbxMesh.GetElementTangentCount() > 1)
	{
		LOG_ERROR("Only one set of tangents currently supported\n");
	}

	FbxGeometryElementTangent* tangentElement = fbxMesh.GetElementTangent();
	if(tangentElement)
	{
		for (int triangleCornerId = 0; triangleCornerId < 3; triangleCornerId++)
		{
			int vertexIndex = fbxMesh.GetPolygonVertex(triangleIndex, triangleCornerId);
			vec4f tangent;
			LoadVector4VertexElement(*tangentElement, tangent, triangleCornerId, vertexIndex);
			triangle.SetTangent(tangent, triangleCornerId);
		}
	}
}


void FBXImport::LoadVector4VertexElement(
	FbxLayerElementTemplate<FbxVector4> &element,
	vec4f &data, // The current nodes vertex array which will store the imported data
	int triangleCornerId,
	int vertexIndex
	)
{
	switch (element.GetMappingMode())
	{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (element.GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
				{
					FbxVector4 fbxData = element.GetDirectArray().GetAt(vertexIndex);
					data.x = static_cast<float>(fbxData[0]);
					data.y = static_cast<float>(fbxData[1]);
					data.z = static_cast<float>(fbxData[2]);
					data.w = static_cast<float>(fbxData[3]);
				}
				break;
				case FbxGeometryElement::eIndexToDirect:
				{
					int id = element.GetIndexArray().GetAt(vertexIndex);
					FbxVector4 fbxData = element.GetDirectArray().GetAt(id);
					data.x = static_cast<float>(fbxData[0]);
					data.y = static_cast<float>(fbxData[1]);
					data.z = static_cast<float>(fbxData[2]);
					data.w = static_cast<float>(fbxData[3]);
				}
				break;
			default:
				LOG_ERROR("Trying to load %s with an unsupported reference mode\n", element.GetName());
			break;
			}
		}
		break;
		case FbxGeometryElement::eByPolygonVertex:
		{
			switch(element.GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
				{
					FbxVector4 fbxData = element.GetDirectArray().GetAt(triangleCornerId);
					data.x = static_cast<float>(fbxData[0]);
					data.y = static_cast<float>(fbxData[1]);
					data.z = static_cast<float>(fbxData[2]);
					data.w = static_cast<float>(fbxData[3]);
				}
				break;

				case FbxGeometryElement::eIndexToDirect:
				{
					int indexId = element.GetIndexArray().GetAt(triangleCornerId);
					FbxVector4 fbxData = element.GetDirectArray().GetAt(indexId);	
					data.x = static_cast<float>(fbxData[0]);
					data.y = static_cast<float>(fbxData[1]);
					data.z = static_cast<float>(fbxData[2]);
					data.w = static_cast<float>(fbxData[3]);
				}
				break;

				default:
					LOG_ERROR("Trying to load %s with an unsupported reference mode\n", element.GetName());
					break;
			}
		}
		default:
			// TODO eDirect is currently not supported - should be easy enough to add this in?
			//LOG_ERROR("Trying to load %s with an unsupported mapping mode\n", element.GetName());
		break;
	}
}

const unsigned int FBXImport::GetUVVertexIndex(
	const unsigned int triangleIndex, 
	const unsigned int triangleCornerId
	) const
{
	return triangleIndex * 3 + triangleCornerId; // Face index * num verts in a triangle + current vertex corner in the triangle
}

void FBXImport::LoadVector2VertexElement(
	FbxLayerElementTemplate<FbxVector2> &element,
	vec2f &data,
	int triangleIndex,									// The index of this triangle in the triangle array
	int triangleCornerId,								// The corner(vertex) of this triangle being refered to
	int vertexIndex										// The index of the vertex in the vertex array
	)
{
	switch (element.GetMappingMode())
	{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (element.GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
				{
					FbxVector2 fbxData = element.GetDirectArray().GetAt(vertexIndex);
					data.x = static_cast<float>(fbxData[0]);
					data.y = static_cast<float>(fbxData[1]);
				}
				break;
				case FbxGeometryElement::eIndexToDirect:
				{
					int id = element.GetIndexArray().GetAt(vertexIndex);
					FbxVector2 fbxUvs = element.GetDirectArray().GetAt(id);
					data.x = static_cast<float>(fbxUvs[0]);
					data.y = static_cast<float>(fbxUvs[1]);
				}
				break;
				default:
					LOG_ERROR("Trying to load %s with an unsupported reference mode\n", element.GetName());
					break;
			}
		}
		break;
		case FbxGeometryElement::eByPolygonVertex:
		{
			switch (element.GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
				case FbxGeometryElement::eIndexToDirect:
				{
					unsigned int uvIndex = element.GetIndexArray().GetAt(GetUVVertexIndex(triangleIndex, triangleCornerId));
					FbxVector2 fbxData = element.GetDirectArray().GetAt(uvIndex);
					data.x = static_cast<float>(fbxData[0]);
					data.y = static_cast<float>(fbxData[1]);
				}
				break;
				default:
					LOG_ERROR("Trying to load %s with an unsupported reference mode\n", element.GetName());
				break;
			}
		}
		break;
		default:
			LOG_ERROR("Trying to load %s with an unsupported mapping mode\n", element.GetName());
		break;
	}
}

void FBXImport::LoadColourVertexElement(
	FbxLayerElementTemplate<FbxColor> &element,
	vec3f &colour, // The colour to import data into
	int triangleCornerId,
	int vertexIndex
	)
{
	switch (element.GetMappingMode())
	{
		case FbxGeometryElement::eByControlPoint:
			switch (element.GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
				{
					FbxColor fbxColour = element.GetDirectArray().GetAt(vertexIndex);
					colour[0] = static_cast<float>(fbxColour[0]);
					colour[1] = static_cast<float>(fbxColour[1]);
					colour[2] = static_cast<float>(fbxColour[2]);
				}
				break;
				case FbxGeometryElement::eIndexToDirect:
				{
					int id = element.GetIndexArray().GetAt(vertexIndex);
					FbxColor fbxColour = element.GetDirectArray().GetAt(id);
					colour[0] = static_cast<float>(fbxColour[0]);
					colour[1] = static_cast<float>(fbxColour[1]);
					colour[2] = static_cast<float>(fbxColour[2]);
				}
				break;
			default:
				LOG_ERROR("Trying to load %s with an unsupported reference mode\n", element.GetName());
				break;
			}
			break;

		case FbxGeometryElement::eByPolygonVertex:
		{
			switch(element.GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
				{
					FbxColor fbxColour = element.GetDirectArray().GetAt(triangleCornerId);				
					colour[0] = static_cast<float>(fbxColour[0]);
					colour[1] = static_cast<float>(fbxColour[1]);
					colour[2] = static_cast<float>(fbxColour[2]);
				}
					break;
				case FbxGeometryElement::eIndexToDirect:
				{
					int id = element.GetIndexArray().GetAt(triangleCornerId);
					FbxColor fbxColour = element.GetDirectArray().GetAt(id);
					colour[0] = static_cast<float>(fbxColour[0]);
					colour[1] = static_cast<float>(fbxColour[1]);
					colour[2] = static_cast<float>(fbxColour[2]);
				}
				break;
			default:
				LOG_ERROR("Trying to load %s with an unsupported reference mode\n", element.GetName());
				break;
			}
		}
		break;
		default:
			LOG_ERROR("Trying to load %s with an unsupported mapping mode\n", element.GetName());
			break;
	}

}

