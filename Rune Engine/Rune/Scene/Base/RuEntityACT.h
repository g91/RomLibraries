/*!
	@project Rune
	@file RuEntity_ACT.h

	Copyright (c) 2004-2009 All rights reserved

	@author John Tang
	@date 2008/10/29
*/

#pragma once

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityContainer.h"
#include "../../Scene/Base/RuEntityModel.h"
#include "../../Scene/Base/RuEntityController.h"
#include "../../Scene/Controller/RuController_EventTrigger.h"
#include "../../Scene/Controller/RuController_Orientation.h"
#include "../../Scene/Paperdoll/RuEntityCompositeModel.h"
#include "../../Scene/Paperdoll/RuEntityPaperdoll.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuACTTemplateNode;
class CRuACTTemplateModifier;
class CRuACTTemplate;
class CRuACTEntity;
class CRuACTLoaderModule;

// ************************************************************************************************************************************************************

// Internal defines
#define ruACTHIERARCHY_POP		0x00000001
#define ruACTHIERARCHY_PUSH		0x00000002

// ************************************************************************************************************************************************************

/*

	ACT Template Notes
	------------------
	All entries in the entity are added to a single linear array. The tree structure is constructed from
	two parameters in each template node. First, each node has m_entityParent, which specifies the top-level node
	to which the node is connected. If this value is less than 0, then the node is a top-level node. Next,
	each node specifies an entity. This entity is parsed to determine its structure, and the structure is stored depth-first
	as a linear array. m_connectivityIndex specifies an index into this linear array, which is the actual interior node
	to which the node is connected.

	Ex:
			Entity 0		Entity 1
			--------        --------
				A              A
			   / \            / \
		      B   G          B   C
		     / \                / \
		    C   D              D   E
			   / \
			  E   F

	Entity 0 is stored as a linear array [ABCDEFG]. AKA in-order traversal.
	                                PUSH:[0110100]
	                                 POP:[0010111]

	Entity 1 is stored as a linear array [ABCDE]. 

	Presuming an entity wants to connect to the interior node C of entity 0, then its m_entityParent would be 0,
	and m_connectivityIndex would be 2. If it wanted to connect to the interior node E of entity 1, then its
	m_entityParent would be 0, and m_connectivityIndex would be 4.

*/

// NOTE TO SELF: Watch closely at the clone and ref counts -- If we clone an entity tree, and then add a different entity
//               to the tree, then delete the tree, what happens?

enum RuACTTemplateNodeType
{
	ruACTTEMPLATENODETYPE_DEFAULT			= 0,											//!< Node specifies an attached entity loaded from a stream
	ruACTTEMPLATENODETYPE_FRAME				= 1,											//!< Node specifies a CRuFrame
	ruACTTEMPLATENODETYPE_EMBEDDEDENTITY	= 2,											//!< Node specifies an embedded entity
	ruACTTEMPLATENODETYPE_EMBEDDEDACT		= 3,											//!< Node specifies an embedded ACT template

	ruACTTEMPLATENODETYPE_FORCE_DWORD		= 0x7FFFFFFF
};

enum RuACTTemplateModifierType
{
	ruACTTEMPLATEMODIFIERTYPE_DEFAULT		= 0,											//!< Specifies a default modifier
	ruACTTEMPLATEMODIFIERTYPE_TEXTURE		= 1,											//!< Specifies a texture override modifier
	ruACTTEMPLATEMODIFIERTYPE_SHADER		= 2,											//!< Specifies a shader override modifier
	ruACTTEMPLATEMODIFIERTYPE_ENTITY		= 3,											//!< Specifies an entity override modifier
	ruACTTEMPLATEMODIFIERTYPE_MATPRIORITY	= 4,											//!< Specifies a material priority override modifier
	ruACTTEMPLATEMODIFIERTYPE_BILLBOARD		= 5,											//!< Specifies a billboard type override modifier
	ruACTTEMPLATEMODIFIERTYPE_UNUSED_0		= 6,
	ruACTTEMPLATEMODIFIERTYPE_ORIENTATION	= 7,											//!< Specifies an orientation modifier
	ruACTTEMPLATEMODIFIERTYPE_PROPERTIES	= 8,											//!< Specifies a property list
	ruACTTEMPLATEMODIFIERTYPE_MATPROPERTIES	= 9,											//!< Specifies a material property list

	ruACTTEMPLATENODEMODTYPE_FORCE_DWORD	= 0x7FFFFFFF
};

enum RuACTMotionControllerType
{
	ruACTMOTIONCONTROLLER_DEFAULT			= 0,											//!< Default controller

	ruACTMOTIONCONTROLLER_HIERARCHY			= 1,											//!< Hierarchy animation controller

	ruACTMOTIONCONTROLLER_ALPHA				= 1000,											//!< Alpha controller
	ruACTMOTIONCONTROLLER_COLOR				= 2,											//!< Color controller

	ruACTMOTIONCONTROLLER_LINEAR			= 3,											//!< Linear controller

	ruACTMOTIONCONTROLLER_MATERIAL			= 4,											//!< Material controller
	ruACTMOTIONCONTROLLER_EMBEDDEDHIERARCHY	= 5,											//!< Embedded hierarchy animation controller
	ruACTMOTIONCONTROLLER_MORPH				= 6,											//!< Morphing controller
	ruACTMOTIONCONTROLLER_EVENT				= 7,											//!< Event trigger controller
	ruACTMOTIONCONTROLLER_PARTICLESYSTEM	= 8,											//!< Particle system controller
	ruACTMOTIONCONTROLLER_ENTITY			= 9,											//!< Entity controller

	ruACTMOTIONCONTROLLER_FORCE_DWORD		= 0x7FFFFFFF
};

enum RuACTMotionChannel
{
	ruACTMOTIONCHANNEL_BODY_FULL			= 1,											//!< Full body
	ruACTMOTIONCHANNEL_BODY_LOWER			= 2,											//!< Lower body
	ruACTMOTIONCHANNEL_BODY_UPPER			= 4,											//!< Upper body
	ruACTMOTIONCHANNEL_BODY_HEAD			= 8,											//!< Head
	ruACTMOTIONCHANNEL_BODY_AUXILIARY		= 16,											//!< Auxiliary full body

	ruACTMOTIONCHANNEL_DEFAULT				= 0xFFFFFFFF									//!< Default unspecified channel
};

// ************************************************************************************************************************************************************

class CRuACT_Anchorable : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Anchor connectivity
	char*									m_anchorTarget;									//!< Name of node to which this node is anchored to

public:
											CRuACT_Anchorable();
	virtual									~CRuACT_Anchorable();

	// IRuObject serialization
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Anchoring
	const char*								GetAnchorTarget() const;
	void									SetAnchorTarget(const char *anchorTarget);
};

// ************************************************************************************************************************************************************

class CRuACTTemplateNode_Runtime
{
protected:
	CRuACTTemplateNode*						m_sourceTemplateNode;							//!< Template node that constructed this runtime node

	// Non-persistent run-time data
	INT32									m_numEntities;									//!< Size of entity hierarchy array
	CRuEntity**								m_entities;										//!< Entity hierarchy array
	char**									m_entityAnchorPaths;

	CRuACTTemplateNode_Runtime*				m_anchorNode;

public:
											CRuACTTemplateNode_Runtime(CRuACTTemplateNode *srcTemplateNode, CRuArrayList<CRuEntity *> &interiorEntities);
											~CRuACTTemplateNode_Runtime();

	// Template queries
	RuACTTemplateNodeType					GetNodeType() const;							// Gets the type of the template node
	const char*								GetName() const;								// Gets the name of the entity
	CRuACTTemplateNode*						GetTemplateNode();

	// Attachment
	BOOL									AnchorTo(CRuACTTemplateNode_Runtime *anchorNode);

	// Run-time data queries
	INT32									IndexOf(CRuEntity *entity) const;				// Gets the index of the entity in the entity array
	INT32									GetNumEntities();								// Gets the total number of entities at this node
	CRuEntity*								GetRootEntity();								// Gets the root entity of this template node
	const char*								GetRootAnchorName();
	CRuEntity*								GetInteriorEntity(const char *anchorPath);		// Gets the interior entity matching the specified anchor path
	const char*								GetInteriorAnchorName(CRuEntity *entity);

	CRuEntity*								GetInteriorEntity(INT32 interiorIndex);			// Gets the interior entity at the specified index
	const char*								GetAnchorPath(INT32 interiorIndex);

	char*									GetReAnchoredRootAnchorName(const char *newAnchorName);

	CRuACTTemplateNode_Runtime*				GetAnchorNode();
	BOOL									IsPartOfSubTree(CRuEntity *entity);

	// Run-time updating
	BOOL									UpdateTemplateNode(BOOL replaceTemplateNodes);	// Updates the source template node with the runtime node's persistence information

	void									BuildAnchorPaths();
};

// ************************************************************************************************************************************************************

class CRuACTTemplateNode : public CRuACT_Anchorable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	RuACTTemplateNodeType					m_nodeType;										//!< Type of the template node

	// Common data
	CRuVector3								m_frame_Scale;									//!< Scale factor
	CRuQuaternion							m_frame_Rotation;								//!< Rotation
	CRuVector3								m_frame_Translation;							//!< Translation

	// Type-specific data (m_nodeType = ruACTTEMPLATENODETYPE_DEFAULT)
	char*									m_default_EntityName;							//!< Name of the entity
	CRuEntity*								m_default_PreloadedEntity;						//!< Reference to a preloaded clone of the entity

	// Type-specific data (m_nodeType = ruACTTEMPLATENODETYPE_EMBEDDEDENTITY)
	CRuEntity*								m_embedded_Entity;								//!< Embedded entity

	// Type-specific data (m_nodeType = ruACTTEMPLATENODETYPE_EMBEDDEDACT)
	CRuACTTemplate*							m_embedded_ACT;									//!< Embedded ACT template

public:
											CRuACTTemplateNode();
	virtual									~CRuACTTemplateNode();

	// IRuObject serialization
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Template setup
	void									Set(char *entityName, const char *anchorName);
	void									Set(CRuEntity *entity, const char *anchorName);

	void									SetOrientation(const CRuVector3 &scale, const CRuQuaternion &rotation, const CRuVector3 &translation);
	void									SetOrientation(REAL scale, const CRuQuaternion &rotation, const CRuVector3 &translation);

	// Template queries
	const char*								GetNodeName() const;
	BOOL									SetNodeName(const char *nodeName);
	BOOL									IsNodeNameMutable();
	RuACTTemplateNodeType					GetNodeType() const;							// Gets the type of the template node
	const char*								GetName() const;								// Gets the name of the entity

	// Run-time data construction
	BOOL									PreloadDependencies(BOOL enableAsyncLoad);
	CRuACTTemplateNode_Runtime*				Construct();									// Constructs the template node's run-time data

	// Run-time updating
	BOOL									UpdateEmbeddedEntity(CRuEntity *embeddedEntity);
#ifdef KALYDO
	// Get all dependencies so they can be downloaded too. Returns whether they need to be loaded.
	virtual BOOL							GetDependencies(std::vector<CRuString>& dependencies);
#endif // KALYDO
protected:
	void									Clear();
};

// ************************************************************************************************************************************************************

class CRuACTTemplateModifier : public CRuACT_Anchorable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	RuACTTemplateModifierType				m_modifier;										//!< Type of the modifier

	// Common stuff
	INT32									m_meshOverride_Index;							//!< Index of the mesh to override

	// Type-specific data (m_nodeType = ruACTTEMPLATENODEMODTYPE_TEXTURE)
	RuTextureChannel						m_texOverride_Channel;							//!< Texture channel to override
	char*									m_texOverride_TextureName;						//!< Name of the texture to use
	IRuTexture*								m_texOverride_PreloadedTexture;

	// Type-specific data (m_nodeType = ruACTTEMPLATENODEMODTYPE_SHADER)
	char*									m_shaderOverride_ShaderName;					//!< Name of the shader to use

	// Type-specific data (m_nodeType = ruACTTEMPLATEMODIFIERTYPE_MATPRIORITY)
	INT32									m_matPriorityOverride_Priority;					//!< New material priority

	// Type-specific data (m_nodeType = ruACTTEMPLATEMODIFIERTYPE_BILLBOARD)
	RuBillboardType							m_billboardTypeOverride_BillboardType;			//!< Billboard type

public:
											CRuACTTemplateModifier();
	virtual									~CRuACTTemplateModifier();

	// IRuObject serialization
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Template queries
	RuACTTemplateModifierType				GetModifierType() const;
	BOOL									IsEquivalentModifier(CRuACTTemplateModifier *modifier);

	// Template setup
	void									Set(RuACTTemplateModifierType modifierType);

	INT32									GetMeshIndex();
	void									SetMeshIndex(INT32 meshIndex);

	// Type-specific config
	const char*								Texture_GetName() const;
	void									Texture_SetName(const char *textureName);

	RuTextureChannel						Texture_GetChannel() const;
	void									Texture_SetChannel(RuTextureChannel channel);

	const char*								Shader_GetName() const;
	void									Shader_SetName(const char *shaderName);

	INT32									MatPriority_GetPriority() const;
	void									MatPriority_SetPriority(INT32 priority);

	RuBillboardType							BillboardType_GetBillboardType() const;
	void									BillboardType_SetBillboardType(RuBillboardType billboardType);

	// Run-time construction
	BOOL									PreloadDependencies(BOOL enableAsyncLoad);
	BOOL									Construct(CRuACTTemplateNode_Runtime *target);

#ifdef KALYDO
	// Get all dependencies so they can be downloaded too.
	virtual BOOL							GetDependencies(std::vector<CRuString>& dependencies);
#endif // KALYDO
};

// ************************************************************************************************************************************************************

class CRuACTMotionNode : public CRuACT_Anchorable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEntity_Controller*					m_controller;									//!< Template controller

public:
											CRuACTMotionNode();
	virtual 								~CRuACTMotionNode();

	// IRuObject serialization
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	void									Set(IRuEntity_Controller *controller);

	// Common functions
	IRuEntity_Controller*					Template_GetController();
	IRuEntity_Controller*					GetController();
};

// ************************************************************************************************************************************************************

class CRuACTMotion : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_motionID;									//!< Motion ID
	CRuString								m_motionName;								//!< Motion name
	REAL									m_motionScale;								//!< Motion scale

	CRuArrayList<CRuACTMotionNode *>		m_motionNodes;

	// Run-time stuff
	BOOL									m_isPlaceHolderMotion;						//!< Is the motion a placeholder motion?

public:
											CRuACTMotion();
	virtual 								~CRuACTMotion();

	// IRuObject serialization
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	void									Set(INT32 motionID, const char *motionName, BOOL isPlaceHolder = FALSE);

	BOOL									AddMotionNode(IRuEntity_Controller *controller, const char *anchorTarget);
	BOOL									RemoveMotionNodeAt(INT32 motionNodeIndex);

	BOOL									Merge(CRuACTMotion *srcMotion);

	INT32									GetMotionID() const;
	const CRuString&						GetMotionName() const;
	REAL									GetMotionScale() const;

	void									SetMotionID(INT32 motionID);
	void									SetMotionName(const char *motionName);
	void									SetMotionScale(REAL motionScale);

	INT32									GetNumMotionNodes() const;
	CRuACTMotionNode*						GetMotionNodeByIndex(INT32 index);

	BOOL									IsPlaceHolderMotion() const;

	// Controller query
	BOOL									EnumerateControllers(const CRuRTTI *controllerType, CRuArrayList<IRuEntity_Controller *> &controllersOut);
};

// ************************************************************************************************************************************************************

class CRuACTTemplate : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuArrayList<CRuACTTemplateNode *>		m_templateNodes;								// Template nodes defining the structure of the ACT entity
	CRuArrayList<CRuACTTemplateModifier *>	m_templateModifiers;							// Template modifiers defining post-build operations on template nodes
	CRuArrayList<CRuACTMotion *>			m_motions;										// Motions available to this ACT entity

	// Properties
	char*									m_classification;								//!< ACT classification
	RuEntityTransformMode					m_transformMode;								//!< ACT entity transform mode
	RuEntityCullMode						m_cullMode;
	BOOL									m_alignToWalkingSurface;						//!< Align entity to walking surface?

	friend									CRuACTLoaderModule;

public:
											CRuACTTemplate();
	virtual									~CRuACTTemplate();

	// IRuObject serialization
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Template modification
	BOOL									AddTemplateNode(char *entityName, const char *anchorName);
	BOOL									AddTemplateNode(CRuEntity *entity, const char *anchorName);
	BOOL									DeleteTemplateNode(CRuACTEntity *actEntity, CRuACTTemplateNode *node);
	BOOL									MoveTemplateNode(CRuACTEntity *actEntity, CRuACTTemplateNode *node, CRuEntity *anchorEntity);
	BOOL									RenameTemplateNode(CRuACTEntity *actEntity, CRuACTTemplateNode *node, const char *name);

	INT32									AddTemplateModifier(CRuACTTemplateModifier *modifier);
	BOOL									DeleteTemplateModifier(CRuACTTemplateModifier *modifier);
	BOOL									DeleteTemplateModifier(INT32 modifierIndex);

	CRuACTTemplateModifier*					CreateTemplateModifier(const char *anchorTarget, INT32 meshIndex, RuACTTemplateModifierType modifierType);
	CRuACTTemplateModifier*					FindTemplateModifier(const char *anchorTarget, INT32 meshIndex, RuACTTemplateModifierType modifierType);
	void									FindTemplateModifiers(const char *anchorTarget, INT32 meshIndex, RuACTTemplateModifierType modifierType, CRuArrayList<CRuACTTemplateModifier *> &modifiersOut);

	BOOL									Merge(CRuACTTemplate *srcTemplate);
	BOOL									Sort();

	// Template query
	INT32									GetNumTemplateNodes() const;
	CRuACTTemplateNode*						GetTemplateNode(INT32 index);
	INT32									GetTemplateNodeIndex(CRuACTTemplateNode *templateNode);

	INT32									GetNumTemplateModifiers() const;
	CRuACTTemplateModifier*					GetTemplateModifier(INT32 index);

	BOOL									NodeNameExists(const char *name);

	// Motion modification
	BOOL									AddMotion(INT32 motionID, const char *motionName);
	BOOL									AddPlaceHolderMotion(INT32 motionID, const char *motionName);
	BOOL									DeleteMotion(INT32 index);

	// Motion query
	INT32									GetNumMotions() const;
	CRuACTMotion*							GetMotion(INT32 index);
	CRuACTMotion*							GetMotionByID(INT32 id);
	CRuACTMotion*							GetMotionByName(const char *motionName);
	INT32									GetNextMotionID() const;

	INT32									GetMotionIDByName(const char *motionName) const;
	const char*								GetMotionNameByID(INT32 id) const;
	INT32									GetMotionIndexByID(INT32 id) const;
	INT32									GetMotionIndexByName(const char *motionName) const;

	void									SetMotion(INT32 index, CRuACTMotion *motion);

	// ACT properties
	const char*								GetClassification() const;
	void									SetClassification(const char *classification);

	RuEntityTransformMode					GetTransformMode() const;
	void									SetTransformMode(RuEntityTransformMode transformMode);

	RuEntityCullMode						GetCullMode() const;
	void									SetCullMode(RuEntityCullMode cullMode);

	BOOL									GetAlignToWalkingSurface() const;
	void									SetAlignToWalkingSurface(BOOL align);

	// ACT entity construction
	BOOL									PreloadDependencies(BOOL enableAsyncLoad);
	CRuACTEntity*							Construct();									// Constructs the ACT entity from the definition template

#ifdef KALYDO
	// Get all dependencies so they can be downloaded too.
	virtual BOOL							GetDependencies(std::vector<CRuString>& dependencies);
#endif // KALYDO

protected:
	const char*								FindAnchorTarget(INT32 numRuntimeNodes, CRuACTTemplateNode_Runtime **nodeRuntimeArray, CRuEntity *anchorEntity);
	CRuACTTemplateNode_Runtime*				FindRuntimeNode(INT32 numRuntimeNodes, CRuACTTemplateNode_Runtime **nodeRuntimeArray, const char *anchorPath);
	CRuACTTemplateNode_Runtime*				FindRuntimeNode(INT32 numRuntimeNodes, CRuACTTemplateNode_Runtime **nodeRuntimeArray, CRuACTTemplateNode *node);
	BOOL									HasMatchingPrefix(const char *prefix, const char *fullString);
};

// ************************************************************************************************************************************************************

class CRuACT_ActionEvents : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	//!< Defined: void EventTriggerCallback();
	typedef boost::function<void (CRuController_EventTrigger *controller, PTRVALUE userData)> EventTriggerCallback;

protected:
	struct EventTriggerCallbackDesc
	{
		INT32								m_eventID;
		EventTriggerCallback				m_callback;
		PTRVALUE							m_userData;

		EventTriggerCallbackDesc*			m_next;
	};

	CRuACTEntity*							m_actEntity;
	CRuArrayList<IRuEntity_Controller *>	m_controllers;

	BOOL									m_disableEventTriggerCallback;
	EventTriggerCallbackDesc*				m_eventTriggerCallbacks;

public:
											CRuACT_ActionEvents();
	virtual									~CRuACT_ActionEvents();

	virtual void							Dispose();

	void									DisableEventTriggerCallback();
	void									Reset();
	void									ResetActionEvents();
	BOOL									InitializeActionEventsFromMotion(CRuACTEntity *actEntity, const char *motionName);
	BOOL									InitializeActionEventsFromMotion(CRuACTEntity *actEntity, CRuACTMotion *motion);

	CRuACTEntity*							GetACTEntity();
	CRuController_EventTrigger*				GetEventController(INT32 eventID);

	CRuArrayList<IRuEntity_Controller *>&	GetControllers();

	BOOL									HandleEvent_Trigger(RuEventArgs *eventArgs);

	// Event callbacks
	BOOL									RegisterEventTriggerCallback(INT32 eventID, EventTriggerCallback callback, PTRVALUE userData);
};

// ************************************************************************************************************************************************************

/*!
	CRuACTEntity
	@author John Tang
	@version 2008.10.29

	The mega object that handles entity templating, motions, etc.

	The object honors the following additional transient properties on its child objects:

	Property Name                                                Semantics                      Description
	------------------------------------------------------------------------------------------------------------------------------------
	act:no enumerate                                             BOOL                           If TRUE, the child object is never enumerated by the editor
*/

class CRuACTEntity : public CRuEntity_Container_Impl
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct InterpolatorDesc
	{
		CRuFrameHierarchy*					m_boundHierarchy;								//!< Entity to which this interpolator is bound
		CRuAnimationInterpolator*			m_interpolator;									//!< The interpolator
	};

	class ActiveMotionDesc
	{
	public:
		INT32								m_motionID;										//!< Currently playing motion ID. -1 = no motion.
		REAL								m_motionTime;									//!< Time marker of the current playing motion.
		UINT32								m_channel;										//!< Channel occupied by this motion descriptor.
		REAL								m_channelWeight;								//!< Blend weight of this channel
		BOOL								m_paused;										//!< Is the motion playback paused?
		CRuArrayList<InterpolatorDesc>		m_interpolators;								//!< Interpolators used by this motion.
		CRuArrayList<IRuEntity_Controller *>m_controllers;									//!< List of all active controllers for this motion.

		REAL								m_leadIn;										//!< Lead-in time
		REAL								m_leadOut;										//!< Lead-out time

		REAL								m_timeMultiplier;

		REAL								m_motionLength;									//!< Length of the motion
		REAL								m_fadeTime;										//!< Current motion fading time.
		REAL								m_fadeDuration;									//!< Motion fading duration.

	public:
											ActiveMotionDesc();
											~ActiveMotionDesc();

		CRuAnimationInterpolator*			GetInterpolator(CRuFrameHierarchy *hierarchy);

		void								Synchronize(ActiveMotionDesc *keyedMotion);
		void								Update(REAL elapsedTime, BOOL surpressControllers);
		void								Mix(REAL elapsedTime);
		void								UpdateMotionLength();

		BOOL								IsMotionComplete(BOOL ignoreLoopingControllers) const;

		void								ClearMotion(REAL transitionDuration, REAL fadeDuration);
		void								Clear();
	};

	struct AnimCompensatorDesc
	{
		UINT32								m_sourceChannel;
		UINT32								m_targetChannel;
		INT32								m_nodeID;
		REAL								m_strength;
	};

	// Constructed entity
	CRuACTTemplate*							m_template;										//!< Template from which this entity was generated

	INT32									m_numRuntimeNodes;								//!< Number of runtime nodes
	CRuACTTemplateNode_Runtime**			m_runtimeNodes;									//!< Runtime template nodes

	REAL									m_motionTime;									//!< Global time marker
	ActiveMotionDesc*						m_persistentMotion;
	CRuArrayList<ActiveMotionDesc *>		m_activeMotions;

	CRuArrayList<AnimCompensatorDesc>		m_pendingAnimCompensators;						//!< List of animation compensators that need to be applied

	// Runtime workspace
	CRuArrayList<INT32>						m_playedMotions;

public:
											CRuACTEntity();
	virtual									~CRuACTEntity();

	// IRuObject interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// CRuEntity overrides
	virtual BOOL							ResetStates();
	virtual BOOL							Update(REAL elapsedTime);

	// Template query
	CRuACTTemplate*							GetTemplate();
	INT32									GetNumRuntimeNodes() const;
	CRuACTTemplateNode_Runtime*				GetRuntimeNode(INT32 index);
	CRuACTTemplateNode_Runtime**			GetRuntimeNodes();
	const char*								GetAnchorName(CRuEntity *anchorEntity);
	CRuEntity*								FindEntity(const char *anchorTarget);
	CRuEntity*								FindEntity(INT32 parentIndex, INT32 connectivityIndex);
	BOOL									UpdateTemplate(BOOL replaceTemplateNodes);

	// Other stuff
	BOOL									ValidateAnchorPaths();

	// Motion channel operations
	BOOL									PlayPersistentMotion(BOOL forceOverride = TRUE);
	BOOL									PlayMotion(UINT32 channel, INT32 motionID, REAL blendTime, BOOL forceOverride = TRUE, REAL channelWeight = 1.0f, REAL leadIn = 0.0f, REAL leadOut = 0.0f, REAL expectedMotionScale = 0.0f, CRuACT_ActionEvents *actionEventsOut = NULL);
	BOOL									PlayMotionByIndex(UINT32 channel, INT32 motionIndex, REAL blendTime, BOOL forceOverride = TRUE, REAL channelWeight = 1.0f, REAL leadIn = 0.0f, REAL leadOut = 0.0f, REAL expectedMotionScale = 0.0f, CRuACT_ActionEvents *actionEventsOut = NULL);
	BOOL									SwapMotion(UINT32 srcchannel, UINT32 destchannel);
	BOOL									ClearMotion(UINT32 channel, REAL fadeDuration);
	BOOL									SynchronizeMotion(INT32 syncIndex);

	BOOL									PauseChannel(UINT32 channel);
	BOOL									ResumeChannel(UINT32 channel);

	BOOL									CreateAnimationCompensator(UINT32 sourceChannel, UINT32 targetChannel, INT32 nodeID, REAL strength);

	// Motion channel queries
	INT32									GetActiveMotionID(UINT32 channel) const;
	const char*								GetActiveMotionName(UINT32 channel) const;
	REAL									GetActiveMotionTime(UINT32 channel) const;
	CRuArrayList<IRuEntity_Controller *>*	GetActiveMotionControllers(UINT32 channel);
	REAL									GetMotionTime() const;

	BOOL									IsActiveMotion(UINT32 channel, const char *motionName);
	BOOL									IsMotionComplete(UINT32 channel, BOOL ignoreLoopingControllers = FALSE) const;

	// Other queries
	INT32									GetMotionIDByName(const char *motionName);
	INT32									GetMotionIndexByName(const char *motionName);
	REAL									GetMotionLength(const char *motionName);

	BOOL									CheckForController(UINT32 channel, const CRuRTTI &controllerType);
	BOOL									CheckForEvent(UINT32 channel, RuEntityEvent eventType, REAL *startTimeOut = NULL);

protected:
	INT32									GetMotionDescriptorIndex(UINT32 channel);
	void									CreatePendingAnimationCompensators();

	friend									CRuACTTemplate;
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_ACTTemplateNode, CRuACTTemplateNode)
ruOBJECTMODULE_DEFINE(CRuObjectModule_ACTTemplateModifier, CRuACTTemplateModifier)
ruOBJECTMODULE_DEFINE(CRuObjectModule_ACTMotionNode, CRuACTMotionNode)
ruOBJECTMODULE_DEFINE(CRuObjectModule_ACTMotion, CRuACTMotion)
ruOBJECTMODULE_DEFINE(CRuObjectModule_ACTTemplate, CRuACTTemplate)
ruOBJECTMODULE_DEFINE(CRuObjectModule_ACTEntity, CRuACTEntity)

// ************************************************************************************************************************************************************

#pragma managed(pop)
