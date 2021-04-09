#ifndef _RUHIERARCHY_GR2_H_
#define _RUHIERARCHY_GR2_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityContainer.h"
#include "../../Scene/Base/RuEntityModel.h"

#include "../../ExtLink/RuExtLink_Granny.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class RuHierarchyEvent_BonesUpdated_Args : public RuEventArgs
{
public:
	CRuEntity*								m_hierarchy;					//!< Hierarchy being updated

	RuHierarchyEvent_BonesUpdated_Args(CRuEntity *hierarchy)
	:	m_hierarchy(hierarchy)
	{
	}
};

// ************************************************************************************************************************************************************

class CRuHierarchy_GR2 : public CRuEntity_Container_Impl
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct SubNodeDesc
	{
		UINT32								m_id;											//!< Bone ID
		UINT32								m_groupID;										//!< Group ID bitfield
		CRuSphere							m_localBounds;									//!< Bone local bounding sphere
		CRuOBB								m_localOBB;										//!< Bone local bounding OBB
	};

	struct LinkFrameDesc
	{
		INT32								m_linkIndex;									//!< Index of the bone to which the frame is linked
		BOOL								m_useWorldSpaceTransform;						//!< Link frame uses world space transform instead of composite skin pose
		CRuLinkFrame*						m_linkFrame;									//!< Frame for the link node

		CRuMatrix4x4						m_refTransform;
	};

	struct MotionChannelDesc
	{
		UINT32								m_channelMask;									//!< Channel mask
		granny_track_mask*					m_modelMask;
		CRuAnimation_GR2*					m_animation;
		REAL								m_loopDelay;
		REAL								m_weightMultiplier;								//!< Blend weight multiplier (Maximum effect this channel can have)

		REAL								m_timeMultiplier;								//!< Current time multiplier

		granny_control*						m_control;
		granny_model_instance*				m_controlModelInstance;
		granny_blend_dag_node*				m_controlNode;
		INT32								m_controlTTL;

		REAL								m_fadeTransitionSpeed;
		REAL								m_fadeWeight;
		granny_local_pose*					m_fadePose;
		granny_blend_dag_node*				m_fadePoseNode;

		granny_blend_dag_node*				m_dagCrossFadeNode;
		granny_local_pose*					m_blendedPose;

		granny_blend_dag_node*				m_dagBlendNode;
	};

	struct SubNodeDeltaKeyDesc
	{
		PTRVALUE							m_deltaKeyID;
		INT32								m_subNodeIndex;									//!< Delta key target bone index
		CRuVector3							m_scale;										//!< Delta scale
		CRuQuaternion						m_rotation;										//!< Delta rotation
		CRuVector3							m_position;										//!< Delta position

		CRuArrayList<INT32>*				m_subNodeStopIndices;							//!< List of target bone indices where the delta must be set back to identity
	};

	struct MotionCompensatorDesc
	{
		UINT32								m_motionChannel;								//!< Motion channel to compensate for
		INT32								m_subNodeIndex;									//!< Source bone to compensate for
		REAL								m_compensatorWeight;							//!< Weight of motion compensation

		CRuVector3							m_lastRotation;
	};

	struct BoneModifierDesc
	{
		char*								m_boneName;										//!< Target bone name
		CRuVector3							m_scale;										//!< Modifier scale

		char**								m_terminatorBoneNames;							//!< NULL-terminated array of terminator bone names
	};

	CRuEvent								m_event_BonesUpdated;

	BOOL									m_requiresInitialUpdate;
	BOOL									m_ignoreVisibilityCheck;

	BOOL									m_useAlternateSkinnedMeshColPath;
	CRuSphere								m_alternateSkinnedMeshColSphere;

	BOOL									m_gr2SkeletonIsDeprecated;
	BOOL									m_gr2SkeletonInstanceDirty;
	granny_model*							m_gr2ModelTemplate;								//!< Granny model used as template
	granny_model*							m_grannyModel;									//!< Granny model (skeleton)
	granny_model_instance*					m_grannyModelInstance;							//!< Granny model instance (pose matrices)
	CRuMemoryStream*						m_grRestAnimationStream;						//!< Granny rest-state animation source stream
	granny_file*							m_grRestAnimationFile;							//!< Granny rest-state animation source file
	granny_animation*						m_grRestAnimation;								//!< Granny rest-state animation

	BOOL									m_gr2SparseBoneArrayRequiresUpdate;
	INT32									m_gr2SparseBoneArraySize;
	granny_int32x*							m_gr2SparseBoneArray;
	granny_int32x*							m_gr2SparseBoneArrayReverse;

	granny_local_pose*						m_grannyLocalPose;								//!< Granny local pose
	granny_world_pose*						m_grannyWorldPose;								//!< Granny world pose

	INT32									m_numSubNodes;									//!< Number of sub nodes in the hierarchy
	SubNodeDesc*							m_subNodes;										//!< Sub node array
	CRuMatrix4x4*							m_subNodeTransforms;							//!< Current sub node transforms in skin composite space
	CRuMatrix4x4*							m_subNodeTransforms_World;						//!< Current sub node transforms in world space
	CRuMatrix4x4*							m_restPoseTransforms_World;						//!< Current sub node rest pose transforms in world space

	CRuArrayList<LinkFrameDesc>				m_linkFrames;									//!< List of link frames belonging to the hierarchy

	granny_blend_dag_node*					m_rootBlendNode;								//!< Master blend DAG node
	CRuArrayList<MotionChannelDesc>			m_motionChannels;								//!< List of motion channels supported by the hierarchy

	CRuArrayList<SubNodeDeltaKeyDesc>		m_subNodeDeltaKeys;								//!< List of active subnode delta keys

	BOOL									m_enableMotionCompensators;
	CRuArrayList<MotionCompensatorDesc>		m_motionCompensators;							//!< List of motion compensators

	CRuArrayList<BoneModifierDesc>			m_boneModifiers;

	// Profiling probes
	INT32									m_probe_HierarchyUpdate;

public:
											CRuHierarchy_GR2();
	virtual									~CRuHierarchy_GR2();

	// Events
	CRuEvent&								Event_BonesUpdated();

	// Hierarchy construction (typically used by exporter only)
	BOOL									SetSkeleton(granny_skeleton *skeleton);
	BOOL									SetRestAnimationStream(IRuStream *stream);
	CRuLinkFrame*							CreateLinkFrameOnBone(const char *boneName, BOOL useWorldSpaceTransform);

	BOOL									ResetBoneModifiers();
	BOOL									SetBoneModifier(const char *boneName, const CRuVector3 &scale, char **terminatorBoneNames = NULL);

	// Runtime construction
	BOOL									SetSubNodeBoundsFromMesh(IRuMesh *mesh, BOOL resetExistingBounds);
	INT32									SetSubNodeChannelMaskFromBone(const char *boneName, DWORD channel, BOOL exclusive);

	// Granny-specific
	granny_model_instance*					GetGR2ModelInstance();

	// Settings
	void									SetIgnoreVisibilityCheck(BOOL ignoreVisibilityCheck);

	// Sub node ID search
	INT32									GetNumSubNodes();
	INT32									GetSubNodeIndex(INT32 subNodeID);
	INT32									GetSubNodeIndexByName(const char *subNodeName);
	CRuSphere								GetSubNodeBounds(INT32 subNodeIndex, RuFrameType frameType = ruFRAMETYPE_WORLD);

	// Property access
	CRuMatrix4x4*							GetSubNodeTransforms(RuFrameType frameType = ruFRAMETYPE_COMPOSITE);
	CRuMatrix4x4*							GetSubNodeRestPoseTransforms();
	BOOL									ForceLinkFrameUpdate(CRuEntity *linkFrame);

	// Playback interface
	BOOL									PlayAnimation(UINT32 motionChannel, CRuAnimation_GR2 *animation, REAL loopDelay, REAL timeMultiplier);
	BOOL									UpdateAnimationTimeMultiplier(UINT32 motionChannel, REAL timeMultiplier);
	BOOL									ClearAnimation(UINT32 motionChannel, REAL transitionTime);
	BOOL									SynchronizeMotionChannel(UINT32 motionChannel);

	// Other motion controls
	BOOL									SetSubNodeDeltaKey(PTRVALUE deltaKeyID, INT32 subNodeIndex, const CRuVector3 &scale, const CRuQuaternion &rotation, const CRuVector3 &position);
	BOOL									AddSubNodeDeltaKeyStopIndex(PTRVALUE deltaKeyID, INT32 subNodeIndex, INT32 stopIndex);
	BOOL									EnableMotionCompensators(BOOL enable);
	BOOL									AddMotionCompensator(UINT32 motionChannel, INT32 subNodeIndex, REAL compensatorWeight);
	BOOL									SetChannelWeightMultiplier(UINT32 motionChannel, REAL weightMultiplier);

	BOOL									SupportsMotionChannel(UINT32 channel);

	// Container and stuff
	virtual BOOL							AddChild(CRuEntity *entity);
	virtual BOOL							RemoveChild(CRuEntity *entity);
	virtual BOOL							RemoveAllChildren();

	// IRuObject interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							ResetStates();
	virtual BOOL							Update(REAL elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

protected:
	BOOL									InstantiateGR2Skeleton();
	BOOL									FreeGR2SkeletonInstance();
	BOOL									InstantiateGR2Model();
	BOOL									FreeGR2ModelInstance();

	BOOL									InstantiateSubNodeStructure();
	BOOL									FreeSubNodeStructure();

	BOOL									ConstructMotionChannels();
	BOOL									FreeMotionChannels(CRuArrayList<MotionChannelDesc> &motionChannels);

	BOOL									UpdateSparseBoneArray();

	MotionChannelDesc*						FindChannelDescriptor(UINT32 motionChannel);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Hierarchy_GR2, CRuHierarchy_GR2)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
