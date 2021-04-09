#ifndef _RUENTITYCONTAINER_H_
#define _RUENTITYCONTAINER_H_

// Animation system
#include "../../Engine/Animation/RuAnimBase.h"

// Entity base
#include "../../Scene/Base/RuEntityBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

// Internal defines
#define ruFRAMEHIERARCHY_POP		0x00000001
#define ruFRAMEHIERARCHY_PUSH		0x00000002

// ************************************************************************************************************************************************************

class CRuFrameHierarchy;

// ************************************************************************************************************************************************************

class CRuFrame : public CRuEntity_Container_Impl
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
											CRuFrame();
	virtual									~CRuFrame();

	// IRuObject interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	friend									CRuFrameHierarchy;
};

// ************************************************************************************************************************************************************

class CRuLinkFrame : public CRuEntity_Container_Impl
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	char*									m_linkName;

public:
											CRuLinkFrame();
	virtual									~CRuLinkFrame();

	// IRuObject interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	const char*								GetLinkName() const;
	void									SetLinkName(const char *linkName);
};

// ************************************************************************************************************************************************************

/*!
	Frame hierarchies are containers for hierarchical animation as well as skinning animation entities.
	Internally, a frame hierarchy contains a set of sub nodes, which correspond to nodes in an animation
	hierarchy. Skinned meshes can use this hierarchy for skinning, and hierarchical meshes can use it
	for hierarchical animation.
	
	A hierarchy contains two types of nodes: sub nodes, and links. Sub nodes are the nodes in the
	transform hierarchy, and can be used directly for skinning. Links are frames which are attached
	to specific sub nodes, and are used for attaching entities to the hierarchy.

	TODO: In terms of this inheriting from the container class, it's not quite "right." That is,
	it actually overrides some methods implemented in the container because links count as
	entities too. Also, bounding sphere calculation is completely wrong since it does not take
	into account the entities at the links.

	@author John Tang
	@version 2004/12/13
*/
class CRuFrameHierarchy : public CRuEntity_Container_Impl
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:

	/*
		NOTES
		-----
			1. Transforms are composed by scale first, followed by rotation and then translation
	*/

	/*
		HIERARCHY STRUCTURE FLAGS
		-------------------------
		[ 0 -  0] POP Flag
		[ 1 -  1] PUSH Flag
		[ 2 -  9] Parent sub node index (Calculated at run time)
	*/

	struct SubNode
	{
		// Sub node identifier and hierarchy traversal flags
		UINT32								m_id;											//!< Bone ID
		UINT32								m_groupID;										//!< Group ID bitfield
		UINT32								m_flags;										//!< Hierarchy structure flags

		// Initial sub node transformation
		CRuVector3							m_initialScale;									//!< Non-uniform scale
		CRuQuaternion						m_initialRotation;								//!< Rotation at the frame
		CRuVector3							m_initialTranslation;							//!< Translation at the frame

		inline INT32						GetParentIndex() const
											{
												INT32 parentIndex = (m_flags & 0x000003FC) >> 2;
												return (parentIndex == 0xFF) ? -1 : parentIndex;
											}

		inline void							SetParentIndex(INT32 parentIndex)
											{
												m_flags = (m_flags & ~0x000003FC) | ((parentIndex << 2) & 0x000003FC);
											}
	};

	struct LinkNode
	{
		// Link information
		INT32								m_linkIndex;									//!< Index of the subnode to which the node is linked
		INT32								m_linkID;										//!< ID of the link node
		INT32								m_linkGroupID;									//!< Group ID bitfield
		CRuString							m_linkName;										//!< Name of the link node

		// Initial link node transformation
		CRuVector3							m_initialScale;									//!< Non-uniform scale
		CRuQuaternion						m_initialRotation;								//!< Rotation at the frame
		CRuVector3							m_initialTranslation;							//!< Translation at the frame

		// Link frame
		CRuEntity*							m_linkFrame;									//!< Frame for the link node
	};

	INT32									m_numSubNodes;									//!< Number of sub nodes in the hierarchy
	SubNode*								m_subNodes;										//!< Sub node array

	INT32									m_numLinks;										//!< Number of links in the hierarchy
	LinkNode*								m_links;										//!< Link array

	CRuMatrix4x4*							m_skinToBoneTransforms;							//!< Skin-to-bone transforms (initial orientation transforms)
	CRuMatrix4x4*							m_subNodeTransforms;							//!< Current sub node transforms

	REAL									m_animationUpdateInterval;						//!< Interval, in seconds, at which the animation updates
	REAL									m_timeSinceLastAnimationUpdate;					//!< Time elapsed since last animation update
	REAL									m_animationAccumulatedT;
	CRuAnimationInterpolator*				m_interpolator;									//!< Animation interpolator

	// Workspace
	CRuStack<INT32>							m_scratch_ParentStack;							//!< Scratch pad

public:
											CRuFrameHierarchy();
	virtual									~CRuFrameHierarchy();

	BOOL									Initialize(INT32 numSubNodes, INT32 numLinks, UINT32 *nodeIDs, UINT32 *groupIDs, UINT32 *nodeFlags);
	BOOL									InitializeDefaultPose(CRuVector3 *scaleList, CRuQuaternion *rotationList, CRuVector3 *translationList);

	// IRuObject interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							ResetStates();
	virtual BOOL							Update(REAL elapsedTime);

	// Animation
	CRuAnimationInterpolator*				GetInterpolator();
	void									ClearAnimations(REAL transitionDuration);
	BOOL									PlayAnimation(CRuAnimation *animation, REAL startTime, REAL blendInTime, REAL blendOutTime, REAL loopBlendDuration);
	BOOL									SetAnimationTime(REAL animTime);

	// Links
	CRuEntity*								GetLinkFrameByID(INT32 linkID);
	CRuEntity*								GetLinkFrameByIndex(INT32 linkIndex);
	CRuEntity*								GetLinkFrameByName(const char *linkName);
	INT32									GetNumLinks() const;
	INT32									GetLinkIDByIndex(INT32 linkIndex) const;
	const char*								GetLinkNameByIndex(INT32 linkIndex) const;

	// Sub node ID search
	INT32									GetNumSubNodes();
	INT32									GetSubNodeIndex(INT32 subNodeID);
	INT32									GetSubNodeParentIndex(INT32 subNodeIndex);
	INT32									GetSubTreeSize(INT32 subNodeIndex);

	// Property access
	CRuMatrix4x4*							GetSubNodeTransforms();

	UINT32									GetSubNodeFlags(INT32 subNodeIndex);
	const CRuMatrix4x4&						GetSubNodeTransform(INT32 subNodeIndex);
	const CRuMatrix4x4&						GetSubNodeSkinToBoneTransform(INT32 subNodeIndex);

	BOOL									SupportsMotionChannel(UINT32 channel);

protected:
	void									Reset();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Frame, CRuFrame)
ruOBJECTMODULE_DEFINE(CRuObjectModule_LinkFrame, CRuLinkFrame)
ruOBJECTMODULE_DEFINE(CRuObjectModule_FrameHierarchy, CRuFrameHierarchy)

// ************************************************************************************************************************************************************

inline CRuMatrix4x4 *CRuFrameHierarchy::GetSubNodeTransforms()
{
	return m_subNodeTransforms;
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
