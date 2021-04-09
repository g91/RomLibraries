/*!
	@file RuEntity_DecalShadow.h

	Copyright (c) 2004-2006 All rights reserved
*/

#ifndef _RUENTITY_DECALSHADOW_H_
#define _RUENTITY_DECALSHADOW_H_

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_UI.h"

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

enum RuDecalShadowMode
{
	ruDECALSHADOWMODE_NONE					= 0,
	ruDECALSHADOWMODE_SIMPLE				= 1,
	ruDECALSHADOWMODE_NORMAL				= 2,

	ruDECALSHADOWMODE_FORCE_DWORD			= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

/*!
	Decal entity.

	@version 2006.03.16
*/
class CRuEntity_DecalShadow : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_maximumDecalRadius;
	INT32									m_maximumDecalFaces;						//!< Maximum number of faces to enumerate for the decal
	REAL									m_enumerationPadding;						//!< How much extra distance to pad geometry enumeration
	RuDecalShadowMode						m_shadowMode;

	CRuVector3								m_shadowDirection;							//!< Shadow projection direction, specified in world frame
	REAL									m_shadowZoneSize;							//!< Size by which to extend the shadowed zone
																						//!< Enumeration center is moved out along light vector by this value
	REAL									m_dotShadowSize;							//!< Size of the dot shadow

	// Runtime
	CRuArrayList<CRuEntity *>				m_casters;
	INT32									m_primaryCasterIdx;
	IRuObject*								m_receiver;									//!< Receiver object
	INT32									m_triEnumIteration;							//!< Iteration number of last triangle enumeration
	CRuSphere								m_decalEnumerationBounds;					//!< Bounds of last mesh enumeration, specified in world frame
	CRuCamera*								m_shadowCamera;

	CRuMatrix4x4							m_mappingTransform;
	CRuMatrix4x4							m_clampingTransform;

	IRuTextureView*							m_textureView;
	IRuTexture*								m_dotTexture;								//!< Simple dot texture used in simple shadow mode

	CRuStack<CRuEntity *>					m_entityStack;

	CRuRendererNode_RenderContext*			m_renderContext;							//!< Render context used for constructing the shadow projection map

	// Decal mesh
	BOOL									m_decalEnabled;
	CRuDynamicMesh*							m_mesh;										//!< Mesh object
	CRuMaterial*							m_material;									//!< Material object
	CRuMaterial*							m_shadowRenderMaterial_Static;
	CRuMaterial*							m_shadowRenderMaterial_Skin;

	// Run-time stuff
	CRuCollisionQuery_Sphere_TriEnum		m_triEnumQuery;

public:
											CRuEntity_DecalShadow();
	virtual									~CRuEntity_DecalShadow();

	RuDecalShadowMode						GetShadowMode() const;
	void									SetShadowMode(RuDecalShadowMode shadowMode);

	REAL									GetDotShadowSize() const;
	void									SetDotShadowSize(REAL dotShadowSize);

	void									AddCaster(CRuEntity *caster, BOOL primaryCaster);
	void									RemoveCaster(CRuEntity *caster);
	void									RemoveAllCasters();
	void									SetReceiver(IRuObject *receiver);

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	// IRuEntity_Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	// Event handlers
	BOOL									HandleEvent_Submitted(RuEventArgs *eventArgs);

	IRuMaterial*							MaterialOverrideFunctor(IRuMaterial *sourceMaterial);

protected:
	BOOL									CollisionContainerChangedHandler(RuEventArgs *eventArgs);
	BOOL									CollisionContainerDisposeHandler(RuEventArgs *eventArgs);

	CRuSphere								GetCasterBounds();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_DecalShadow, CRuEntity_DecalShadow)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
