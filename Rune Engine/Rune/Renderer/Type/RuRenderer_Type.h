/*!
	@file RuRenderer_Type.h

	Copyright (c) 2004-2008 All rights reserved
*/

#ifndef _RURENDERER_TYPE_H_
#define _RURENDERER_TYPE_H_

#include "../../Engine/Base/RuCore.h"
#include "../../Engine/Base/RuShaderBase.h"
#include "../../Engine/Base/RuMaterialBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuClipPlaneSet;
class CRuRendererNode;

// ************************************************************************************************************************************************************

struct RuLightingInfo
{
	DWORD									m_TTL;											//!< TTL of this lighting info structure

	INT32									m_sceneIteration;								//!< Iteration number of the current scene
	INT32									m_entityIteration;								//!< Iteration number of the target entity
	INT32									m_lightingIteration;							//!< Iteration number of the lighting information

	CRuColor4								m_ambientLight;									//!< Ambient light color

	CRuArrayList<RuLightSettings>			m_directionalLights;							//!< Directional lights
	CRuArrayList<RuLightSettings>			m_pointLights;									//!< Point lights
	CRuArrayList<RuLightSettings>			m_spotLights;									//!< Spot lights

	// Deprecated?
	CRuColor4								m_shCoefficients[9];							//!< Object-level SH lighting coefficients
	RUCOLOR									m_specularColor;
	CRuVector3								m_specularDirection;							//!< Object-space specular light source position
};

// ************************************************************************************************************************************************************

struct RuRenderQueueEntry
{
	// Clip plane setting
	CRuClipPlaneSet*						m_clipPlaneSet;									//!< Clip plane set to use, NULL to disable

	// Transformation and parameters
	const CRuSphere*						m_worldBounds;									//!< World bounds
	const CRuMatrix4x4*						m_worldTransform;								//!< World transform
	CRuParameterCache*						m_paramCache;									//!< Entity-specific parameter cache

	// Lighting data
	CRuGUID									m_lightingGUID;									//!< GUID for the entry
	CRuMatrix4x4							m_lightingInvTransform;							//!< World inverse transform

	CRuCamera*								m_camera;
	INT32									m_billboardType;
	BOOL									m_isBackdrop;

	// Entity render information
	IRuMesh*								m_mesh;											//!< Mesh
	IRuMaterial*							m_material;										//!< Material
	IRuShader*								m_shader;										//!< Shader to use
	REAL									m_priority;										//!< Entity priority, calculated based on distance to camera and alpha blending settings

	CRuEntity*								m_entity;
	BOOL									m_isRootEntity;

	// Execute-only stuff
	IRuVertexAssembly*						m_vertexAssembly;
	IRuIndexBuffer*							m_instancedIB;
	IRuBaseTexture*							m_instancedTextures[4][16];						//!< 4 passes, 16 textures max
};

// ************************************************************************************************************************************************************

class CRuRendererEvent_Begin_Args : public RuEventArgs
{
public:
	CRuRendererNode*						m_rendererNode;									//!< Renderer node on which begin was called

	CRuRendererEvent_Begin_Args(CRuRendererNode *rendererNode)
	:	m_rendererNode(rendererNode)
	{
	}
};

// ************************************************************************************************************************************************************

class CRuRendererEvent_Execute_Args : public RuEventArgs
{
public:
	CRuRendererNode*						m_rendererNode;									//!< Renderer node to be executed

	CRuRendererEvent_Execute_Args(CRuRendererNode *rendererNode)
	:	m_rendererNode(rendererNode)
	{
	}
};

// ************************************************************************************************************************************************************

class CRuRendererExecuteState
{
public:
	CRuKeyHash<CRuGUID>						m_triggerHash;
	INT32									m_lastTriggerHashCount;

	CRuKeyHash<PTRVALUE>					m_executionMap;
	CRuArrayList<CRuRendererNode *>			m_executionArray;
};

// ************************************************************************************************************************************************************

class CRuClipPlaneSet : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuPlane4								m_clipPlanes[6];

public:
											CRuClipPlaneSet();
	virtual									~CRuClipPlaneSet();

	const CRuPlane4&						GetClipPlane(INT32 index);
	void									SetClipPlane(INT32 index, const CRuPlane4 &clipPlane);
};

// ************************************************************************************************************************************************************

class CRuRendererNode : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	BOOL									m_persistAcrossFrames;				//!< Persist this node across multiple frames? If not, node will be deleted at the end of execution
	BOOL									m_priority;							//!< Priority, higher = earlier execution

//	INT32									m_iteration;						//!< Structural iteration
	CRuRendererNode*						m_parent;							//!< Node parent
	CRuRendererNode*						m_firstChild;						//!< First child
	CRuRendererNode*						m_prevSibling;						//!< Previous sibling
	CRuRendererNode*						m_nextSibling;						//!< Next sibling

	CRuArrayList<CRuRendererNode *>			m_dependencies;						//!< Permanent dependencies
	CRuArrayList<CRuRendererNode *>			m_transientDependencies;			//!< Temporary dependencies

	CRuEvent								m_event_Begin;						//!< Begin event object
	CRuEvent								m_event_Execute;					//!< Execute event object

	// Runtime
	CRuArrayList<CRuRendererNode *>			m_runtime_ExecutionArray;

public:
											CRuRendererNode();
	virtual									~CRuRendererNode();

	// Settings
	BOOL									PersistAcrossFrames(BOOL enable);
	BOOL									IsPersistentAcrossFrames();
	INT32									GetPriority();
	INT32									SetPriority(INT32 priority);

	// Graph management
	BOOL									AddChild(CRuRendererNode *node);
	BOOL									RemoveChild(CRuRendererNode *node);
	BOOL									RemoveAllChildren();
	BOOL									DetachFromParent();
	BOOL									DetachNonPersistentChildren();

	// Dependency management
	BOOL									AddDependency(CRuRendererNode *node, BOOL transient);
	BOOL									RemoveDependency(CRuRendererNode *node);

	BOOL									ReadyToExecute(CRuKeyHash<PTRVALUE> &executionMap);

	// Events
	CRuEvent&								Event_Begin();
	CRuEvent&								Event_Execute();

	// CRuRendererNode overridable interface
	virtual void							SetParentRenderer(IRuObject *renderer);
	virtual void							Begin();
	virtual void							Execute();
	virtual void							End();
	virtual void							Execute(CRuKeyHash<PTRVALUE> &executionMap);

protected:
	void									RunExecuteTrigger(CRuKeyHash<CRuGUID> &triggerHash, INT32 lastTriggerHashCount);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
