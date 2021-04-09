/*!
	@file RuEntityBase.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITYBASE_H_
#define _RUENTITYBASE_H_

// Rune system core
#include "../../Engine/Base/RuCore.h"
#include "../../Engine/Base/RuShaderBase.h"
#include "../../Engine/Base/RuMaterialBase.h"
#include "../../Engine/Manager/RuEngine_InstanceManager_Base.h"
#include "../../Engine/Manager/RuEngine_ResourceManager_Base.h"
#include "../../Engine/Manager/RuEngine_ShaderManager.h"

// Animation support system
#include "../../Engine/Animation/RuAnimTrack.h"

// Rune geometry system
#include "../../Engine/Geometry/RuMeshBase.h"
#include "../../Engine/Geometry/RuMeshCollision.h"
#include "../../Engine/Geometry/RuPolySoup.h"

// Renderer base types
#include "../../Renderer/Type/RuRenderer_Type.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

#define ruMAXFLOATRANGE							100000.0f

// ************************************************************************************************************************************************************

#define ruSAFE_DETACH(x) if(x) { x->DetachFromParent(); }
#define ruENTITY_SAFE_RELEASE_CHILD(x) if(x) { x->SetParent(NULL); x->Release(); x = NULL; }

// ************************************************************************************************************************************************************

/*!

	Backdrops

	______________ <-- Backdrop far plane
	\            /
	 \     2    /
	  \________/ <-- Far Plane
	   \      /
	    \  1 /
		 \__/  <-- Near Plane
		  \/

	1 - Normal view frustum area
	2 - Backdrop frustum area -- used for background object rendering (z-write off)

*/

// ************************************************************************************************************************************************************

enum RuEntityStatusFlag
{
	ruENTITYSTATUS_VISIBLE					= 0,							//!< Is the entity visible during last render flush?

	ruENTITYSTATUS_FORCE_DWORD				= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuEntityTransformMode
{
	ruENTITYTRANSFORMMODE_NORMAL			= 0,							//!< Normal mode
	ruENTITYTRANSFORMMODE_OVERRIDE			= 1,							//!< Override world transform. If enabled, the entity's local transform always converts to world space regardless of its parent's transforms.
	ruENTITYTRANSFORMMODE_NOROTATE			= 2,							//!< Rotations disabled

	ruENTITYTRANSFORMMODE_FORCE_DWORD		= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuEntityCullMode
{
	ruENTITYCULLMODE_NORMAL					= 0,							//!< Normal entity culling.
	ruENTITYCULLMODE_NEVER					= 1,							//!< Never perform culling on this entity.
	ruENTITYCULLMODE_ATTACHED				= 2,							//!< Defer culling to parent. If parent is visible, this entity is always visible.

	ruENTITYCULLMODE_FORCE_DWORD			= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuEntityOption
{
	ruENTITYOPTION_SKINNED					= 0,							//!< Is the entity skinned?
	ruENTITYOPTION_COLLISION				= 1,							//!< Is the entity collision-enabled?
	ruENTITYOPTION_DISTANCECLASS			= 2,							//!< Distance class of the entity
	ruENTITYOPTION_REFLECTCLASS				= 3,							//!< Reflection class of the entity
	ruENTITYOPTION_CASTERCLASS				= 4,							//!< Shadow caster class of the entity
	ruENTITYOPTION_RECEIVERCLASS			= 5,							//!< Shadow receiver class of the entity
	ruENTITYOPTION_BILLBOARD				= 6,							//!< Billboard type of the entity
	ruENTITYOPTION_HIDDEN					= 7,							//!< Is the entity hidden?

	ruENTITYOPTION_NOCLONE					= 8,							//!< Disable cloning?
	ruENTITYOPTION_NOSERIALIZE				= 9,							//!< Disable serialization?

	// Hierarchy options
	ruENTITYOPTION_TRANSFORMMODE			= 10,							//!< Entity transform mode
	ruENTITYOPTION_HRDISABLE				= 11,							//!< Disable the entity's role in the scene hierarchy. The entity's transform and bounding volume always follows its parent exactly.
	ruENTITYOPTION_CULLMODE					= 12,							//!< Entity culling mode
	ruENTITYOPTION_EXCLUDEBOUNDS			= 14,							//!< Entity (not including its descendants) does not factor into bounding calculations

	// Other options
	ruENTITYOPTION_NOCLIP					= 20,							//!< Disable all clip planes?
	ruENTITYOPTION_USEPARENTLIGHTING		= 21,							//!< Use lighting info from parent entity

	// Run-time options
	ruENTITYOPTION_BACKDROP					= 1001,							//!< Entity is a backdrop object
	ruENTITYOPTION_IGNOREVISIBILITY			= 1002,							//!< Entity is executed and drawn regardless of frustum visibility
	ruENTITYOPTION_DECALTARGET				= 1003,							//!< Entity is a valid decal target
	ruENTITYOPTION_ENABLEDISTANCEFADE		= 1004,							//!< Enable distance fade
	ruENTITYOPTION_HIDDENBYDISTANCE			= 1005,							//!< Has the entity been hidden because it is beyond visible distance?
	ruENTITYOPTION_NOMOTIONPROPAGATE		= 1006,							//!< Do not propagate parent motion beyond this entity?

	ruENTITYOPTION_FORCE_DWORD				= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuEntityEvent
{
	ruENTITYEVENT_NONE						= 0,							//!< Default event

	// User events
	ruENTITYEVENT_HIT						= 1,							//!< Hit event, parameter = sound ID?
	ruENTITYEVENT_FOOTSTEP					= 2,							//!< Footstep event, parameter = sound ID?
	ruENTITYEVENT_GHOSTTRAIL				= 3,							//!< Ghost trail event, parameter = ??
	ruENTITYEVENT_HOLSTER					= 4,							//!< Holster weapon event
	ruENTITYEVENT_UNHOLSTER					= 5,							//!< Unholster weapon event
	ruENTITYEVENT_TARGET_HIT				= 6,							//!< Target is hit event, parameter = sound ID?
	ruENTITYEVENT_INTERRUPTIBLE				= 7,							//!< Motion is interruptible from this point

	ruENTITYEVENT_CAST_TRIGGER_01			= 21,							//!< Casting trigger event 1
	ruENTITYEVENT_CAST_TRIGGER_02			= 22,							//!< Casting trigger event 2
	ruENTITYEVENT_CAST_TRIGGER_03			= 23,							//!< Casting trigger event 3
	ruENTITYEVENT_CAST_TRIGGER_04			= 24,							//!< Casting trigger event 4

	ruENTITYEVENT_FX_TRIGGER_01				= 25,
	ruENTITYEVENT_FX_TRIGGER_02				= 26,
	ruENTITYEVENT_FX_TRIGGER_03				= 27,
	ruENTITYEVENT_FX_TRIGGER_04				= 28,

	ruENTITYEVENT_RANGED_LOAD				= 31,							//!< Ranged projectile load event
	ruENTITYEVENT_RANGED_RELEASE			= 32,							//!< Ranged projectile release event
	ruENTITYEVENT_RANGED_FLIGHT				= 33,							//!< Ranged projectile flight begin event

	ruENTITYEVENT_EQUIP_MELEE				= 36,							//!< Melee equip point-in-time
	ruENTITYEVENT_EQUIP_RANGED				= 37,							//!< Ranged equip point-in-time

	ruENTITYEVENT_AUDIO_DIRECT				= 41,							//!< Direct audio trigger, event string is used as the audio resource name
	ruENTITYEVENT_AUDIO_TAGGED				= 42,							//!< Tagged audio trigger, event string is used as the audio ID
	ruENTITYEVENT_AUDIO_3D_DIRECT			= 43,							//!< Direct 3D audio trigger, event string is used as the audio resource name
	ruENTITYEVENT_AUDIO_3D_TAGGED			= 44,							//!< Tagged 3D audio trigger, event string is used as the audio ID
	ruENTITYEVENT_AUDIO_TERMINATE			= 50,							//!< Audio termination trigger. No parameters.

	ruENTITYEVENT_CAMERA_JIGGLE				= 51,							//!< Camera jiggle trigger, parameter0 = max power distance, parameter1 = effective distance

	ruENTITYEVENT_EFFECT_FINISHED			= 61,							//!< Effect is finished marker. No parameters.

	// System broadcast events (Broadcast events are passed along to all entities regardless of event handler conditions)
	ruENTITYEVENT_BROADCAST_CAMERA			= 2001,							//!< Active camera is being broadcast
	ruENTITYEVENT_BROADCAST_WORLD			= 2002,							//!< World transform used by the renderer is being broadcast
	ruENTITYEVENT_BROADCAST_VIEW			= 2003,							//!< View transform used by the renderer is being broadcast
	ruENTITYEVENT_BROADCAST_DECALTARGET		= 2004,							//!< Valid decal target list is being broadcast, parameter0 = (POINTER) CRuArrayList<CRuEntity *> *
	ruENTITYEVENT_BROADCAST_SUBMIT			= 2005,							//!< Entity is being submitted for rendering, parameter0 = (POINTER) IRuSceneRenderer *
	ruENTITYEVENT_BROADCAST_SUBMITTED		= 2006,							//!< Entity has been submitted for rendering and has passed culling, parameter0 = (POINTER) IRuSceneRenderer *
	ruENTITYEVENT_BROADCAST_BEGIN			= 2007,							//!< Scene render has begun
	ruENTITYEVENT_BROADCAST_FLUSH_3D		= 2008,							//!< 3D pipeline is about to be flushed
	ruENTITYEVENT_BROADCAST_FLUSHED_3D		= 2009,							//!< 3D pipeline has just been flushed
	ruENTITYEVENT_BROADCAST_END				= 2010,							//!< Scene render has ended

	ruENTITYEVENT_FORCE_DWORD				= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuEntityTagID
{
	// System tags
	ruENTITYTAG_SYSTEM						= 0,							//!< System tag (used by scene renderer)

	// Game FSM tags
	ruENTITYTAG_FSMSTATE_MOVEMENT			= 1,							//!<

	// User tags
	ruENTITYTAG_USER_0						= 10,							//!<
	ruENTITYTAG_USER_1						= 11,							//!<
	ruENTITYTAG_USER_2						= 12,							//!<
	ruENTITYTAG_USER_3						= 13,							//!<

	ruENTITYTAG_FORCE_DWORD					= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuFrameType
{
	ruFRAMETYPE_WORLD						= 0,							//!< Data in world frame
	ruFRAMETYPE_LOCAL						= 1,							//!< Data in local frame
	ruFRAMETYPE_COMPOSITE					= 2,							//!< Data in skin composite frame
	ruFRAMETYPE_FORCE_DWORD					= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuTransformOp
{
	ruTRANSFORMOP_SET						= 0,							//!< Set scale
	ruTRANSFORMOP_PRECONCAT					= 1,							//!< Pre-concatenate scale
	ruTRANSFORMOP_POSTCONCAT				= 2,							//!< Post-concatenate scale
	ruTRANSFORMOP_FORCE_DWORD				= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuParticleFrameType
{
	ruPRTFRAMETYPE_WORLD					= 0,							//!< Particles are generated in world coordinates
	ruPRTFRAMETYPE_LOCAL					= 1,							//!< Particles are generated in local coordinates
	ruPRTFRAMETYPE_FORCE_DWORD				= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuParticleEmitterType
{
	ruPRTEMITTERTYPE_POINT					= 0,							//!< Point emitter
	ruPRTEMITTERTYPE_VOLUME					= 1,							//!< Volume emitter
	ruPRTEMITTERTYPE_SPHERE					= 2,							//!< Sphere emitter
	ruPRTEMITTERTYPE_SHELL					= 3,							//!< Spherical shell emitter
	ruPRTEMITTERTYPE_DISC					= 4,							//!< Disc emitter
	ruPRTEMITTERTYPE_RING					= 5,							//!< Ring emitter
	ruPRTEMITTERTYPE_FORCE_DWORD			= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuParticleEmitterMotionType
{
	ruPRTEMITTERMOTIONTYPE_STATIC			= 0,							//!< Stationary emitter
	ruPRTEMITTERMOTIONTYPE_ATTRACTOR		= 1,							//!< Attracting emitter
	ruPRTEMITTERMOTIONTYPE_FORCE_DWORD		= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

typedef		boost::variant<	CRuAnimTrack<INT32> *,
							CRuAnimTrack<REAL> *,
							CRuAnimTrack<BYTE> *,
							CRuAnimTrack<BOOL> *,
							CRuAnimTrack<char *> *,
							CRuAnimTrack<CRuColor4> *,
							CRuAnimTrack<CRuVector3> *,
							CRuAnimTrack<RuAlphaMode> *,
							CRuAnimTrack<RuAlphaTestMode> *,
							CRuAnimTrack<RuParticleEmitterType> * > RuAnimTrackVariant;

inline CRuAnimTrackBase *RuAnimTrackVariant_GetBase(RuAnimTrackVariant track)
{
	CRuAnimTrack<INT32> **track_INT32 = boost::get<CRuAnimTrack<INT32> *>(&track);
	CRuAnimTrack<REAL> **track_REAL = boost::get<CRuAnimTrack<REAL> *>(&track);
	CRuAnimTrack<BYTE> **track_BYTE = boost::get<CRuAnimTrack<BYTE> *>(&track);
	CRuAnimTrack<BOOL> **track_BOOL = boost::get<CRuAnimTrack<BOOL> *>(&track);
	CRuAnimTrack<char *> **track_char = boost::get<CRuAnimTrack<char *> *>(&track);
	CRuAnimTrack<CRuColor4> **track_CRuColor4 = boost::get<CRuAnimTrack<CRuColor4> *>(&track);
	CRuAnimTrack<CRuVector3> **track_CRuVector3 = boost::get<CRuAnimTrack<CRuVector3> *>(&track);
	CRuAnimTrack<RuParticleEmitterType> **track_RuParticleEmitterType = boost::get<CRuAnimTrack<RuParticleEmitterType> *>(&track);
	CRuAnimTrack<RuAlphaMode> **track_RuAlphaMode = boost::get<CRuAnimTrack<RuAlphaMode> *>(&track);
	CRuAnimTrack<RuAlphaTestMode> **track_RuAlphaTestMode = boost::get<CRuAnimTrack<RuAlphaTestMode> *>(&track);

	if(track_INT32)
	{
		return (*track_INT32);
	}
	else if(track_REAL)
	{
		return (*track_REAL);
	}
	else if(track_BYTE)
	{
		return (*track_BYTE);
	}
	else if(track_BOOL)
	{
		return (*track_BOOL);
	}
	else if(track_char)
	{
		return (*track_char);
	}
	else if(track_CRuColor4)
	{
		return (*track_CRuColor4);
	}
	else if(track_CRuVector3)
	{
		return (*track_CRuVector3);
	}
	else if(track_RuParticleEmitterType)
	{
		return (*track_RuParticleEmitterType);
	}
	else if(track_RuAlphaMode)
	{
		return (*track_RuAlphaMode);
	}
	else if(track_RuAlphaTestMode)
	{
		return (*track_RuAlphaTestMode);
	}

	return NULL;
}

// ************************************************************************************************************************************************************

class RuEntityEvent_Update_Args : public RuEventArgs
{
public:
	CRuEntity*								m_entity;						//!< Entity being updated
	REAL									m_elapsedTime;					//!< Elapsed time for this update

	RuEntityEvent_Update_Args(CRuEntity *entity, REAL elapsedTime)
	:	m_entity(entity),
		m_elapsedTime(elapsedTime)
	{
	}
};

// ************************************************************************************************************************************************************

class RuEntityEvent_UpdateStep_Args : public RuEventArgs
{
public:
	CRuEntity*								m_entity;						//!< Entity being updated
	REAL									m_elapsedTime;					//!< Elapsed time for this update step

	RuEntityEvent_UpdateStep_Args(CRuEntity *entity, REAL elapsedTime)
	:	m_entity(entity),
		m_elapsedTime(elapsedTime)
	{
	}
};

// ************************************************************************************************************************************************************

class RuEntityEvent_Submitted_Args : public RuEventArgs
{
public:
	CRuRendererNode*						m_renderContext;				//!< Render context
	CRuCamera*								m_camera;						//!< Camera used for submission
	CRuEntity*								m_entity;						//!< Entity that was submitted

	BOOL									m_continueSubmission;			//!< Return value: Continue with render submission for this entry?

	RuEntityEvent_Submitted_Args(CRuRendererNode *renderContext, CRuCamera *camera, CRuEntity *entity, BOOL continueSubmission)
	:	m_renderContext(renderContext),
		m_camera(camera),
		m_entity(entity),
		m_continueSubmission(continueSubmission)
	{
	}
};

// ************************************************************************************************************************************************************

class RuEntityEvent_Camera_Args : public RuEventArgs
{
public:
	CRuCamera*								m_camera;						//!< Camera object

	RuEntityEvent_Camera_Args(CRuCamera *camera)
	:	m_camera(camera)
	{
	}
};

// ************************************************************************************************************************************************************

class RuEntityEvent_Transform_Args : public RuEventArgs
{
public:
	const CRuMatrix4x4*						m_worldTransform;				//!< World transform
	const CRuMatrix4x4*						m_viewTransform;				//!< View transform

	RuEntityEvent_Transform_Args(const CRuMatrix4x4 *worldTransform, const CRuMatrix4x4 *viewTransform)
	:	m_worldTransform(worldTransform),
		m_viewTransform(viewTransform)
	{
	}
};

// ************************************************************************************************************************************************************

class RuEntityEvent_DecalTarget_Args : public RuEventArgs
{
public:
	CRuArrayList<CRuEntity *>*				m_validDecalTargets;			//!< Array of valid decal targets

	RuEntityEvent_DecalTarget_Args(CRuArrayList<CRuEntity *> *validDecalTargets)
	:	m_validDecalTargets(validDecalTargets)
	{
	}
};

// ************************************************************************************************************************************************************

class RuEntityEvent_Trigger_Args : public RuEventArgs
{
public:
	CRuEntity*								m_entity;						//!< Entity involved in this trigger
	CRuEntity*								m_proxy;						//!< Proxy entity which triggered this event
	INT32									m_triggerID;
	PTRVALUE								m_parameter0;
	PTRVALUE								m_parameter1;
	const char*								m_triggerString;
	REAL									m_duration;

	RuEntityEvent_Trigger_Args(CRuEntity *entity, CRuEntity *proxy, INT32 triggerID, PTRVALUE parameter0, PTRVALUE parameter1, const char *triggerString, REAL duration)
	:	m_entity(entity),
		m_proxy(proxy),
		m_triggerID(triggerID),
		m_parameter0(parameter0),
		m_parameter1(parameter1),
		m_triggerString(triggerString),
		m_duration(duration)
	{
	}
};

// ************************************************************************************************************************************************************

class RuEntityEvent_OptionChanged_Args : public RuEventArgs
{
public:
	CRuEntity*								m_entity;

	RuEntityEvent_OptionChanged_Args(CRuEntity *entity)
	:	m_entity(entity)
	{
	}
};

// ************************************************************************************************************************************************************

/*!
	Base interface for entity event listeners. Upon handling a raised event, the handler should return
	TRUE if it does not wish for the event to be propagated further to other event listeners, or FALSE
	if the event should continue to be propagated to other event listeners.
*/
class IRuEntityEventListener
{
public:
	virtual BOOL							HandleEvent(CRuEntity *entity, IRuObject *eventProxy, INT32 eventID, PTRVALUE eventParameter0, PTRVALUE eventParameter1, const char *eventString, REAL eventDuration, void *userData) = 0;
};

// ************************************************************************************************************************************************************

/*!
	Base class for all objects in the scene system. This provides basic services required
	of all scene objects, including the ability to retrieve the object's world transform,
	cloning, updating, and serialization.

	Derived classes typically will override the GetWorldTransform, Clone, and Update
	functions.

	Option Flags Bitfield
	---------------------

	[  0 -  0] - Skinned
	[  1 -  1] - Collision Enabled
	[  2 -  5] - Display Distance Class (0 = Always visible, 4 = Fog edge, 8 = Large decorative items, 12 = Small decorative items)
	[  6 -  7] - Reflection Class (0 = Full Reflect, 1 = Refract Only, 2 = Reflect Only, 3 = No Reflect)
	[  8 -  9] - Shadow Caster Class (0 = No Shadow Interaction, 3 = Full Shadow Interaction)
	[ 10 - 11] - Shadow Receiver Class (0 = No Shadow Interaction, 3 = Full Shadow Interaction)
	[ 12 - 14] - Billboard type of the entity (0 = None, 1 = Unconstrained, ...)
	[ 31 - 31] - Backdrop (0 = normal, 1 = backdrop)

	@author John Tang
	@version 2004/12/21
*/
class CRuEntity : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct EventListenerDesc
	{
		IRuEntityEventListener*				m_listener;										//!< Event listener
		void*								m_userData;										//!< User-specified listener data
		BOOL								m_persistOnClone;								//!< Persist listener on clones?
		EventListenerDesc*					m_next;											//!< Pointer to the next event listener descriptor
	};

	struct TagDesc
	{
		INT32								m_tagID;										//!< Tag ID
		POINTER								m_tag;											//!< Tag data
		TagDesc*							m_next;											//!< Pointer to the next tag
	};

private:
	// Entity hierarchy data
	CRuEntity*								m_entityParent;									//!< Parent of the entity, NULL = root
	CRuEntity*								m_firstChild;									//!< First child of the entity, NULL = no children
	CRuEntity*								m_prevSibling;									//!< Previous sibling
	CRuEntity*								m_nextSibling;									//!< Next sibling

	// Event system data
	CRuEvent								m_event_Update;									//!< Entity update event
	CRuEvent								m_event_UpdateStep;								//!< Entity update step event
	CRuEvent								m_event_Submitted;								//!< Entity submitted event
	CRuEvent								m_event_Camera;									//!< Camera broadcast event
	CRuEvent								m_event_Transform;								//!< Transform broadcast event
	CRuEvent								m_event_DecalTarget;							//!< Valid decal target broadcast event
	CRuEvent								m_event_Trigger;								//!< Trigger event
	CRuEvent								m_event_OptionChanged;							//!< Option changed event

	// Transformation data
	BOOL									m_localTransform_Dirty;							//!< Local transform is dirty
	BOOL									m_transform_Dirty;								//!< World transform is dirty
	BOOL									m_inverseTransform_Dirty;
	REAL									m_transform_Local_Scale;						//!< Local scale
	CRuQuaternion							m_transform_Local_Rotation;						//!< Local rotation
	CRuVector3								m_transform_Local_Translation;					//!< Local translation
	REAL									m_transform_World_Scale;						//!< World scale
	CRuQuaternion							m_transform_World_Rotation;						//!< World rotation
	CRuVector3								m_transform_World_Translation;					//!< World translation

	// Bounding data
	BOOL									m_bounds_EntityDirty;							//!< Bounds dirty
	CRuSphere								m_bounds_Entity;								//!< Local bounding sphere for the entity

	BOOL									m_bounds_Dirty;									//!< Bounds dirty
	CRuSphere								m_bounds_Local;									//!< Local bounding sphere for the subtree
	CRuSphere								m_bounds_World;									//!< World bounding sphere for the subtree

	// Run-time values, not directly modifiable
	CRuMatrix4x4							m_localTransform;								//!< Local transformation
	CRuMatrix4x4							m_worldTransform;								//!< Composed local-to-root transformation
	CRuMatrix4x4							m_inverseWorldTransform;

	REAL									m_accumElapsedTime;
	REAL									m_minUpdateInterval;
	REAL									m_timeToNextLocalBoundUpdate;					//!< Time until next local bound update

	static INT32							s_updateInvokeCount;
	static INT32							s_eventRaiseCount;
	static INT32							s_listenerInvokeCount;

protected:
	// Collision information
	IRuCollisionObject*						m_collisionObject;								//!< Optional collision object for the subtree

	// Flags
	UINT32									m_statusFlags;									//!< Entity status flags
	UINT32									m_optionFlags0;
	UINT32									m_optionFlags1;

	// System data
	TagDesc*								m_tags;											//!< Data tags
	CRuParameterBlock						m_parameterBlock;								//!< The entity's default parameter block

public:
											CRuEntity();
	virtual									~CRuEntity();

	// Memory reporting
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	// Base provided interface
	CRuEntity*								GetParent();
	void									SetParent(CRuEntity *parent);					// Sets the parent of this entity
	void									SetCollisionObject(IRuCollisionObject *colObj);
	IRuCollisionObject*						GetCollisionObject();
	INT32									GetStatusFlag(RuEntityStatusFlag flag);
	INT32									SetStatusFlag(RuEntityStatusFlag flag, INT32 value);
	INT32									GetOption(RuEntityOption option);												// Gets the specified option's value
	INT32									SetOption(RuEntityOption option, INT32 value, BOOL applyToChildren = FALSE);	// Sets the specified option's value
	POINTER									GetTag(INT32 tagID);															// Gets the tag for this entity
	void									SetTag(INT32 tagID, POINTER tag);												// Sets the tag for this entity

	// Container and stuff
	virtual BOOL							AddChild(CRuEntity *entity);
	virtual BOOL							RemoveChild(CRuEntity *entity);
	virtual BOOL							RemoveAllChildren();
	virtual BOOL							DetachFromParent();
	virtual CRuEntity*						GetFirstChild();
	virtual CRuEntity*						GetNextSibling();
	virtual CRuEntity*						GetPreviousSibling();

	// Event system
	CRuEvent&								Event_Update();
	CRuEvent&								Event_UpdateStep();
	CRuEvent&								Event_Submitted();
	CRuEvent&								Event_Camera();
	CRuEvent&								Event_Transform();
	CRuEvent&								Event_DecalTarget();
	CRuEvent&								Event_Trigger();
	CRuEvent&								Event_OptionChanged();

	REAL									GetMinUpdateInterval() const;
	void									SetMinUpdateInterval(REAL minUpdateInterval);

	static void								ResetUpdateInvokeCount();
	static INT32							GetUpdateInvokeCount();

	static void								ResetEventRaiseCount();
	static INT32							GetEventRaiseCount();

	static void								ResetListenerInvokeCount();
	static INT32							GetListenerInvokeCount();

	// Additional properties
	CRuParameterBlock*						GetParameterBlock();

	// IRuReferenceCounted interface
	virtual void							Dispose();

	// IRuObject interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Base interface
	virtual BOOL							ResetStates();									// Resets entity states to initial, if any
	virtual BOOL							Update(REAL elapsedTime);						// Updates entity state

	// Transform interface
	const CRuMatrix4x4&						GetWorldTransform();							// Gets the world transform at the entity
	const CRuMatrix4x4&						GetInverseWorldTransform();						// Gets the inverse world transform at the entity
	const CRuMatrix4x4&						GetLocalTransform();
	void									ResetAllTransforms();
	void									ResetLocalTransform();
	void									ResetLocal3x3Transform();
	void									SetLocalTransform(const CRuMatrix4x4 &transform);
	void									SetLocal3x3Transform(const CRuMatrix4x4 &transform);
	void									Scale(const CRuVector3 &scale, RuTransformOp transformOp = ruTRANSFORMOP_POSTCONCAT);
	void									Scale(REAL scale, RuTransformOp transformOp = ruTRANSFORMOP_POSTCONCAT);
	void									Rotate(const CRuQuaternion &quat, RuTransformOp transformOp = ruTRANSFORMOP_POSTCONCAT);
	void									Translate(const CRuVector3 &translation, RuTransformOp transformOp = ruTRANSFORMOP_POSTCONCAT);
	void									Transform(const CRuMatrix4x4 &transform, RuTransformOp transformOp = ruTRANSFORMOP_POSTCONCAT);
	void									SetTranslation(const CRuVector3 &translation);

	REAL									GetScale(RuFrameType frameType = ruFRAMETYPE_LOCAL);
	const CRuQuaternion&					GetRotation(RuFrameType frameType = ruFRAMETYPE_LOCAL);
	const CRuVector3&						GetTranslation(RuFrameType frameType = ruFRAMETYPE_LOCAL);

	// Bounds
	void									MarkLocalEntityBoundsDirty();
	void									SetLocalEntityBounds(const CRuSphere &localEntityBounds);
	const CRuSphere&						GetLocalEntityBounds();
	const CRuSphere&						GetLocalBounds();
	const CRuSphere&						GetWorldBounds();

	void									UpdateTransformation();
	void									UpdateBoundingInformation();
	void									UpdateEntityBoundingInformation();

	virtual void							CalculateEntityBounds();

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);	// Make a collision query

protected:
	BOOL									HandleEvent_Trigger_Propagate(RuEventArgs *eventArgs);

	void									InvalidateTransformChain(BOOL invalidationRoot = TRUE);
	void									InvalidateBoundingChain();
/*
	void									InvalidateTransformChain(BOOL transform, BOOL entityBounds, BOOL bounds);
	void									PropagateInvalidationUp(BOOL transform, BOOL entityBounds, BOOL bounds);
	void									PropagateInvalidationDown(BOOL transform, BOOL entityBounds, BOOL bounds);
*/
};

// ************************************************************************************************************************************************************

class CRuEntity_Light : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

	RuLightSettings								m_light;										// Light settings

public:
											CRuEntity_Light();
	virtual									~CRuEntity_Light();

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	const RuLightSettings&							GetLight() const;
	void									SetLight(const RuLightSettings &light);

	RuLightType								GetLightType() const;
	void									SetLightType(RuLightType lightType);
	const CRuColor4&						GetDiffuse() const;
	void									SetDiffuse(const CRuColor4 &diffuse);
	const CRuColor4&						GetSpecular() const;
	void									SetSpecular(const CRuColor4 &specular);
	const CRuVector3&						GetPosition() const;
	void									SetPosition(const CRuVector3 &position);
	const CRuVector3&						GetDirection() const;
	void									SetDirection(const CRuVector3 &direction);
	REAL									GetRange() const;
	void									SetRange(REAL range);
	REAL									GetFalloff() const;
	void									SetFalloff(REAL falloff);
	REAL									GetAttenuation0() const;
	void									SetAttenuation0(REAL attenuation0);
	REAL									GetAttenuation1() const;
	void									SetAttenuation1(REAL attenuation1);
	REAL									GetAttenuation2() const;
	void									SetAttenuation2(REAL attenuation2);
	REAL									GetTheta() const;
	void									SetTheta(REAL theta);
	REAL									GetPhi() const;
	void									SetPhi(REAL phi);

protected:
	void									UpdateLightBounds();
};

// ************************************************************************************************************************************************************

class IRuEntity_Container : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
											IRuEntity_Container();
	virtual									~IRuEntity_Container();

	// Container interface
	virtual INT32							GetNumEntities();
	virtual CRuEntity*						GetEntity(INT32 index);
};

// ************************************************************************************************************************************************************

class IRuEntity_Container_Mutable : public IRuEntity_Container
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
											IRuEntity_Container_Mutable();
	virtual									~IRuEntity_Container_Mutable();

	// Mutable container interface
	virtual void							AddEntity(CRuEntity *entity);
	virtual BOOL							RemoveEntity(CRuEntity *entity);
	virtual BOOL							RemoveEntityAt(INT32 index);
	virtual BOOL							ClearEntities();
};

// ************************************************************************************************************************************************************

class CRuEntity_Container_Impl : public IRuEntity_Container_Mutable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct EntityDesc
	{
		CRuEntity*							m_entity;										//!< The entity object
		INT32								m_iterationNumber;								//!< Last known iteration number
	};

	CRuArrayList<EntityDesc>				m_entities;										//!< Entities held by the container

public:
											CRuEntity_Container_Impl();
	virtual									~CRuEntity_Container_Impl();

	// IRuObject interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Container interface
	virtual INT32							GetNumEntities();
	virtual CRuEntity*						GetEntity(INT32 index);

	// Mutable container interface
	virtual void							AddEntity(CRuEntity *entity);
	virtual BOOL							RemoveEntity(CRuEntity *entity);
	virtual BOOL							RemoveEntityAt(INT32 index);
	virtual BOOL							ClearEntities();
};

// ************************************************************************************************************************************************************

class IRuEntity_Renderable : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
											IRuEntity_Renderable();
	virtual									~IRuEntity_Renderable();

	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);
};

// ************************************************************************************************************************************************************

class IRuEntity_Controller : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	char*									m_description;									//!< Description of the controller
	REAL									m_lifetime;										//!< Maximum lifetime of the controller, negative values disable the lifetime limit
	REAL									m_age;											//!< Age of the controller
	REAL									m_startTime;									//!< Start time
	REAL									m_duration;										//!< Duration during which the controller is active
	REAL									m_loopTime;										//!< If greater than or equal to zero, designates that the controller is looping
	BOOL									m_enabled;										//!< Has the controller been enabled? A disabled controller continues to update its timer, however it must not generate any other effects.
	BOOL									m_overrideClockUpdate;							//!< Disable the default clock update?

public:
											IRuEntity_Controller();
	virtual									~IRuEntity_Controller();

	const char*								GetDescription();								// Gets the controller's description
	void									SetDescription(const char *description);		// Sets the controller's description
	void									SetSelfDestructTimer(REAL lifetime);			// Sets the controller's self-destruct timer
	void									Enable(BOOL enable);
	void									SelfDestruct();

	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(REAL elapsedTime);						// Base update function - all derived classes must chain to this function

	virtual REAL							GetStartTime();									// Gets the time at which the controller first begins effect
	virtual REAL							SetStartTime(REAL startTime);
	virtual REAL							GetDuration();									// Gets the length of time, in seconds, for which the controller is active
	virtual REAL							SetDuration(REAL duration);
	virtual REAL							GetLoopTime();
	virtual REAL							SetLoopTime(REAL loopTime);

	virtual REAL							GetCurrentTime() const;
	virtual BOOL							HasExpired() const;								//!< Has the controller duration expired?
	virtual BOOL							IsLooping() const;								// Is the controller a looping controller?

	virtual IRuObject*						GetTarget();									// Gets the target for the controller
	virtual BOOL							SetTarget(IRuObject *target);					// Sets the target for the controller
};

// ************************************************************************************************************************************************************

class IRuEntity_KeyableController : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
											IRuEntity_KeyableController();
	virtual									~IRuEntity_KeyableController();

	virtual INT32							GetNumTracks() const = 0;
	virtual BOOL							GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut) = 0;
	virtual BOOL							GetTrackByName(const char *trackName, RuAnimTrackVariant &trackOut)
											{
												for(INT32 i = 0; i < GetNumTracks(); ++i)
												{
													RuAnimTrackVariant track;
													if(GetTrackByIndex(i, track))
													{
														CRuAnimTrackBase *baseTrack = RuAnimTrackVariant_GetBase(track);
														if(baseTrack)
														{
															if(strcmp(baseTrack->GetTrackName(), trackName) == 0)
															{
																trackOut = track;
																return TRUE;
															}
														}
													}
												}

												return FALSE;
											}

	virtual BOOL							CloneTracks(IRuEntity_KeyableController *targetController)
	{
											for(INT32 i = 0; i < GetNumTracks(); ++i)
											{
												RuAnimTrackVariant track, targetTrack;
												if(GetTrackByIndex(i, track) && targetController->GetTrackByIndex(i, targetTrack))
												{
													CRuAnimTrack<INT32> **track_INT32 = boost::get<CRuAnimTrack<INT32> *>(&track);
													CRuAnimTrack<REAL> **track_REAL = boost::get<CRuAnimTrack<REAL> *>(&track);
													CRuAnimTrack<BOOL> **track_BOOL = boost::get<CRuAnimTrack<BOOL> *>(&track);
													CRuAnimTrack<BYTE> **track_BYTE = boost::get<CRuAnimTrack<BYTE> *>(&track);
													CRuAnimTrack<char *> **track_char = boost::get<CRuAnimTrack<char *> *>(&track);
													CRuAnimTrack<CRuColor4> **track_CRuColor4 = boost::get<CRuAnimTrack<CRuColor4> *>(&track);
													CRuAnimTrack<CRuVector3> **track_CRuVector3 = boost::get<CRuAnimTrack<CRuVector3> *>(&track);
													CRuAnimTrack<RuParticleEmitterType> **track_RuParticleEmitterType = boost::get<CRuAnimTrack<RuParticleEmitterType> *>(&track);
													CRuAnimTrack<RuAlphaMode> **track_RuAlphaMode = boost::get<CRuAnimTrack<RuAlphaMode> *>(&track);
													CRuAnimTrack<RuAlphaTestMode> **track_RuAlphaTestMode = boost::get<CRuAnimTrack<RuAlphaTestMode> *>(&track);

													if(track_INT32)
													{
														(*track_INT32)->CopyTo(*boost::get<CRuAnimTrack<INT32> *>(&targetTrack));
													}
													else if(track_REAL)
													{
														(*track_REAL)->CopyTo(*boost::get<CRuAnimTrack<REAL> *>(&targetTrack));
													}
													else if(track_BOOL)
													{
														(*track_BOOL)->CopyTo(*boost::get<CRuAnimTrack<BOOL> *>(&targetTrack));
													}
													else if(track_BYTE)
													{
														(*track_BYTE)->CopyTo(*boost::get<CRuAnimTrack<BYTE> *>(&targetTrack));
													}
													else if(track_char)
													{
														(*track_char)->CopyTo(*boost::get<CRuAnimTrack<char *> *>(&targetTrack));
													}
													else if(track_CRuColor4)
													{
														(*track_CRuColor4)->CopyTo(*boost::get<CRuAnimTrack<CRuColor4> *>(&targetTrack));
													}
													else if(track_CRuVector3)
													{
														(*track_CRuVector3)->CopyTo(*boost::get<CRuAnimTrack<CRuVector3> *>(&targetTrack));
													}
													else if(track_RuParticleEmitterType)
													{
														(*track_RuParticleEmitterType)->CopyTo(*boost::get<CRuAnimTrack<RuParticleEmitterType> *>(&targetTrack));
													}
													else if(track_RuAlphaMode)
													{
														(*track_RuAlphaMode)->CopyTo(*boost::get<CRuAnimTrack<RuAlphaMode> *>(&targetTrack));
													}
													else if(track_RuAlphaTestMode)
													{
														(*track_RuAlphaTestMode)->CopyTo(*boost::get<CRuAnimTrack<RuAlphaTestMode> *>(&targetTrack));
													}
												}
		}

		return TRUE;
	}

	virtual REAL							AdjustDurationToKeys()
											{
												REAL maxTrackLength = 0.0f;

												for(INT32 i = 0; i < GetNumTracks(); ++i)
												{
													RuAnimTrackVariant track;
													if(GetTrackByIndex(i, track))
													{
														CRuAnimTrackBase *baseTrack = RuAnimTrackVariant_GetBase(track);
														if(baseTrack)
														{
															maxTrackLength = max(maxTrackLength, baseTrack->GetTrackLength());
														}
													}
												}

												return IRuEntity_Controller::SetDuration(maxTrackLength);
											}

	virtual REAL							SetDurationWithScale(REAL duration)
											{
												// Store old duration and chain call to parent
												REAL oldDuration = m_duration;
												REAL newDuration = IRuEntity_Controller::SetDuration(duration);

												// Calculate the duration scale
												REAL durationScale = (oldDuration > 0.0f) ? newDuration / oldDuration : 1.0f;

												for(INT32 i = 0; i < GetNumTracks(); ++i)
												{
													RuAnimTrackVariant track;
													if(GetTrackByIndex(i, track))
													{
														CRuAnimTrackBase *baseTrack = RuAnimTrackVariant_GetBase(track);
														if(baseTrack)
														{
															baseTrack->ScaleKeys(durationScale);
														}
													}
												}

												return newDuration;
											}
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity, CRuEntity)
ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_Light, CRuEntity_Light)
ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_Container, IRuEntity_Container)
ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_Renderable, IRuEntity_Renderable)
ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_Controller, IRuEntity_Controller)

// ************************************************************************************************************************************************************

inline CRuEntity::CRuEntity()
:	m_entityParent(NULL),
	m_firstChild(NULL),
	m_prevSibling(NULL),
	m_nextSibling(NULL),
	m_localTransform_Dirty(TRUE),
	m_transform_Dirty(TRUE),
	m_inverseTransform_Dirty(TRUE),
	m_transform_Local_Scale(1.0f),
	m_transform_Local_Rotation(0.0f, 0.0f, 0.0f, 1.0f),
	m_transform_Local_Translation(0.0f, 0.0f, 0.0f),
	m_transform_World_Scale(1.0f),
	m_transform_World_Rotation(0.0f, 0.0f, 0.0f, 1.0f),
	m_transform_World_Translation(0.0f, 0.0f, 0.0f),
	m_bounds_EntityDirty(TRUE),
	m_bounds_Dirty(TRUE),
	m_bounds_Local(CRuVector3(0.0f, 0.0f, 0.0f), 0.0f),
	m_accumElapsedTime(0.0f),
	m_timeToNextLocalBoundUpdate(0.0f),
	m_minUpdateInterval(0.0f),
	m_collisionObject(NULL),
	m_statusFlags(0),
	m_tags(NULL)
{
	// Clear all option flags
	m_optionFlags0 = 0;
	m_optionFlags1 = 0;

	// Set default options
	SetOption(ruENTITYOPTION_COLLISION, TRUE);

	// Reset rotation
	m_transform_Local_Rotation.FromAngleAxis(CRuVector3(0.0f, 1.0f, 0.0f), 0.0f);

	// Setup event propagation handlers
	m_event_Trigger.RegisterHandler(this->GetGUID(), 0xFFFFFFFF, boost::bind(&CRuEntity::HandleEvent_Trigger_Propagate, this, _1), NULL);
}

inline CRuEntity::~CRuEntity()
{
	// Release reference to collision object
	ruSAFE_RELEASE(m_collisionObject);

	// Delete all tags
	TagDesc *curTag = m_tags;
	while(curTag)
	{
		// Advance to next tag descriptor and delete
		TagDesc *deletedTag = curTag;
		curTag = curTag->m_next;
		delete deletedTag;
	}

	// Clear tags
	m_tags = NULL;

	// Detach all child nodes
	CRuEntity *curChild = m_firstChild;
	while(curChild)
	{
		CRuEntity *nextChild = curChild->m_nextSibling;

		// Reset pointers
		curChild->m_entityParent = NULL;
		curChild->m_prevSibling = NULL;
		curChild->m_nextSibling = NULL;

		// Release
		ruSAFE_RELEASE(curChild);

		// Advance to next sibling
		curChild = nextChild;
	}

	// If the entity is an interior node or leaf, detach it
	if(m_entityParent)
	{
		this->DetachFromParent();
		m_entityParent = NULL;
	}

	// Clear children
	m_firstChild = NULL;
}

inline void CRuEntity::SetCollisionObject(IRuCollisionObject *colObj)
{
	if(colObj)
	{
		// Update references
		colObj->AddRef();
		ruSAFE_RELEASE(m_collisionObject);

		m_collisionObject = colObj;
	}
}

inline IRuCollisionObject *CRuEntity::GetCollisionObject()
{
	return m_collisionObject;
}

// ************************************************************************************************************************************************************

inline IRuEntity_KeyableController::IRuEntity_KeyableController()
{
}

inline IRuEntity_KeyableController::~IRuEntity_KeyableController()
{
}

// ************************************************************************************************************************************************************

extern INT32				g_ruProbe_TransformUpdateCount;
extern INT32				g_ruProbe_TransformUpdateTime;
extern INT32				g_ruProbe_BoundingUpdateCount;
extern INT32				g_ruProbe_BoundingUpdateTime;

// ************************************************************************************************************************************************************

#pragma managed

#endif
