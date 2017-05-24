#pragma once

#include <fbxsdk.h>
#include "LinkedTree.h"
#include <memory>
#include "VectorKey.h"
#include "QuaternionKey.h"


class VectorTrack;
class QuaternionTrack;
class FBXImport;

class BoneNode : public LinkedTreeItem<BoneNode>
{
public:

	BoneNode();

	~BoneNode();

	void SetName(
		const std::string& name
		)
	{
		m_name = name;
	};

	std::string&	GetName()
	{		
		return m_name;
	}

	unsigned int GetId()
	{
		return m_id;
	}

	static void ResetIdCounter()
	{
		 m_counter = 0;
	}

	void AllocateAnimationTracks(
		int numFrames
		);

	const boost::shared_ptr<VectorTrack> GetPositionTrack()
	{
		return m_positionTrack;
	}

	const boost::shared_ptr<VectorTrack> GetScaleTrack()
	{
		return m_scaleTrack;
	}

	const boost::shared_ptr<QuaternionTrack> GetRotationTrack()
	{
		return m_rotationTrack;
	}

	void AddPositionKey(
		const VectorKey &position
		);

	void AddRotationKey(
		const QuaternionKey &rotation
		);

	void AddScaleKey(
		const VectorKey &scale
		);

	void SetInheritScale(
		bool inheritScale // When true this node inherits scale from it's parent (FbxTransform::eInheritRSrs). When false scale is not inherited (eInheritRrs)
	)
	{
		m_inheritScale = inheritScale;
	}

	bool InheritsScale()
	{
		return m_inheritScale;
	}

	const FbxAMatrix &GetGlobalTransform() const
	{
		return m_globalTransform;
	};

	FbxAMatrix &GetGlobalTransform()
	{
		return m_globalTransform;
	};

	void SetInverseReferenceMatrix(
		FbxAMatrix &inverseReferenceMatrix
	)
	{
		m_inverseReferenceMatrix = inverseReferenceMatrix;
	}

	const FbxAMatrix &GetInverseReferenceMatrix()
	{
		return m_inverseReferenceMatrix;
	}

private:
	friend class FBXImport;			// Friend as the import class needs direct access to these arrays. All other classes accessing a mesh node should use the access function provided.
	bool m_inheritScale;					// When true this node inherits scale from it's parent (FbxTransform::eInheritRSrs). When false scale is not inherited (eInheritRrs)

	boost::shared_ptr<QuaternionTrack> m_rotationTrack;
	boost::shared_ptr<VectorTrack> m_scaleTrack;
	boost::shared_ptr<VectorTrack> m_positionTrack;

	FbxAMatrix m_inverseReferenceMatrix;	// The inverse reference matrix for this bone - calculated on import

	unsigned int m_id;
	static unsigned int m_counter;

	std::string m_name;

	FbxAMatrix m_globalTransform;			// Transform from this node to the global model transform. This is updated each tick during animation
};

