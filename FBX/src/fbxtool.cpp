#include "fbxtool.h"
#include <log.h>
#include <bonenode.h>
#include <meshnode.h>
#include <logger.h>

void FBXTool::buildMesh(MeshNode *meshNode, FBXMesh &mesh)
{
	if (meshNode->getID() > 0) {
		LOG_ERROR("dosent supported 2more meshes in this version");
	}
	std::vector<int> debugIndex;
	auto& faces = meshNode->getFaces();
	auto& vertices = meshNode->getVertices();
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
	bool enableWrite = false;
	if (!enableWrite) return;
	for (int i = 0; i < mesh.vertices.size(); ++i)
	{
		FBXVertex& vertex = mesh.vertices[i];
		int index = debugIndex[i];
		LOG_W << "vertexID : " << index
			<< " boneID : " << vertex.boneID[0] << ", " << vertex.boneID[1] << ", "
			<< vertex.boneID[2] << ", " << vertex.boneID[3] << ", "
			<< " boneWeights : " << vertex.boneWeight[0] << ", " << vertex.boneWeight[1] << ", "
			<< vertex.boneWeight[2] << ", " << vertex.boneWeight[3] << ENDN;

	}

}

float FBXTool::clamp(float val, float fmin, float fmax)
{
	if (val < fmin) {
		val = fmin;
	}
	else if (val > fmax) {
		val = fmax;
	}
	return val;
}

KeyVec3 FBXTool::lerp(
	const KeyVec3& key,
	const KeyVec3& nextKey,
	float normalizedTime)
{
	//A * T +  B(1.f - T)
	KeyVec3 result;
	result.mVector = key.mVector * normalizedTime + (nextKey.mTime * (1.0f - normalizedTime));
	return result;
}

void FBXTool::lerp(
	const KeyVec3& current,
	const KeyVec3& next,
	KeyVec3& result,
	float normalizedTime)
{
	//A + (B - A) * T
	result.mVector = current.data() + (next.data() - current.data()) * normalizedTime;
	return;
}

void FBXTool::lerp(
	const KeyQuaternion& current,
	const KeyQuaternion& next,
	KeyQuaternion& result,
	float normalizedTime)
{
	result.mQuaternion = current.data() + (next.data() - current.data()) * normalizedTime;
	return;
}

void FBXTool::slerp(
	const KeyQuaternion& last,
	const KeyQuaternion& next,
	KeyQuaternion& result,
	float normalizedTime)
{
	//http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/
	FbxQuaternion nextQ = next.mQuaternion;
	
	double cosTheta =	last[0] * nextQ[0] + last[1] * nextQ[1] +
						last[2] * nextQ[2] + last[3] * nextQ[3];

	//no difference
	if (cosTheta == 1.0)
	{
		result.mQuaternion = last.mQuaternion;
		return;
	}

	if (cosTheta < 0)
	{
		//if more than 90deg then flip
		cosTheta *= -1;
		nextQ = -next.mQuaternion;
	}
	
	if (cosTheta < 0.99999f)
	{
		//difference

		//double ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
		//double ratioB = sin(t * halfTheta) / sinHalfTheta;
		////calculate Quaternion.
		//qm.w = (qa.w * ratioA + qb.w * ratioB);
		//qm.x = (qa.x * ratioA + qb.x * ratioB);
		//qm.y = (qa.y * ratioA + qb.y * ratioB);
		//qm.z = (qa.z * ratioA + qb.z * ratioB);
		
		//to half costhea
		double halfTheta = acos(cosTheta);
		double weightA = sin((1 - normalizedTime) * halfTheta) / sin(halfTheta);
		double weightB = sin(normalizedTime * halfTheta) / sin(halfTheta);

		FbxQuaternion interpolatedQ =
			(last.mQuaternion  * weightA) + (nextQ * weightB);

		result = KeyQuaternion(interpolatedQ, 0);
		/*LOG << "interpolated" <<result << ENDN;*/
		return;
	}

	return lerp(last, next, result, normalizedTime);
}


void FBXTool::logFbxMatrix(const FbxAMatrix &mat)
{
	std::stringstream st;
#define FM std::setw(12)
		//row major order
		st << "FbxAMatrix[" << "\n" <<
			FM << mat[0][0] << FM << mat[0][1] << FM << mat[0][2] << FM << mat[0][3] << "\n" <<
			FM << mat[1][0] << FM << mat[1][1] << FM << mat[1][2] << FM << mat[1][3] << "\n" <<
			FM << mat[2][0] << FM << mat[2][1] << FM << mat[2][2] << FM << mat[2][3] << "\n" <<
			FM << mat[3][0] << FM << mat[3][1] << FM << mat[3][2] << FM << mat[3][3] << " ]" << "\n";
#undef FM
		
	LOG << st.str() << ENDN;
}
