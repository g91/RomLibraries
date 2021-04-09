/*!
	@file RuRenderer.h

	Copyright (c) 2004-2008 All rights reserved
*/

#ifndef _RURENDERER_H_
#define _RURENDERER_H_

#include "../Renderer/Type/RuRenderer_Type.h"
#include "../Renderer/Cache/RuSkinCache.h"

#include "../Engine/Texture/RuTextureAtlas_Aggregate.h"

#include "../Scene/Base/RuEntityBase.h"
#include "../Scene/Base/RuEntityACT.h"
#include "../Scene/Base/RuHierarchy_GR2.h"
#include "../Scene/Container/RuContainer_Portal.h"
#include "../Scene/Container/RuEntityAABBTree.h"
#include "../Scene/Occlusion/RuEntity_AntiPortal.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuRenderer;

// ************************************************************************************************************************************************************

class CRuRenderer : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Renderer commands & stacks
	CRuRendererNode*						m_rootRendererNode;

	// Default parameter block
	CRuParameterBlock						m_defaultParamBlock;							//!< Default parameter block

	// Cache
	CRuSkinCache*							m_skinCache;									//!< Skinning cache

	// Current scene state
	INT32									m_iterationNumber;								//!< Iteration number of the current scene

	// Support textures
	CRuTexture_Proxy*						m_nullTexture;									//!< NULL texture map
	CRuTexture_Proxy*						m_blackTexture;									//!< Black texture map
	CRuTexture_Proxy*						m_clampTexture;									//!< Clamping texture map
	CRuTexture_Proxy*						m_grid1x1Texture;								//!< 1x1 grid texture map

	// Lighting prop
	CRuAABBTree*							m_lightTree;									//!< Light tree
	CRuArrayList<CRuEntity_Light *>			m_recycledLightEntities;						//!< Unused light structures
	DWORD									m_specularColor;
	CRuVector3								m_specularDirection;

//	CRuArrayList<RuLightingInfo *>			m_activeLightingInfo;
	CRuHashMap<CRuGUID, RuLightingInfo *>*	m_lightingInfoHash;

	CRuArrayList<CRuEntity *>				m_enumeratedLights;
	CRuArrayList<REAL>						m_enumeratedLightDistances;

	// Traversal structures
	CRuStack<CRuEntity *>					m_entityStack;									//!< Stack used for traversing an entity tree

	// 3rd-order spherical harmonic constants
	REAL									m_sh9_Const1;
	REAL									m_sh9_Const2;
	REAL									m_sh9_Const3;
	REAL									m_sh9_Const4;
	REAL									m_sh9_Const5;

	// Sorting mechanism
	CRuRadixSort							m_radixSort2;									//!< Sorting mechanism

	// Render node execution map
	CRuKeyHash<PTRVALUE>					m_executionMap;

	// Workspace
	DWORD									m_gc_LastGCTime;
	DWORD									m_gc_ElapsedTTL;
	CRuQueue<CRuGUID>						m_gc_LightingInfoGUID;

public:
											CRuRenderer();
	virtual									~CRuRenderer();

	BOOL									AttachRendererNode(CRuRendererNode *rendererNode);
	BOOL									DetachRendererNode(CRuRendererNode *rendererNode);

	void									Begin();
	void									End();

	// Light prop setup
	void									SetLightingProp(CRuEntity *lightingProp);
	void									SetSpecularColor(DWORD specularColor);
	void									SetSpecularDirection(const CRuVector3 &specularDir);

	void									AddtoSH9(CRuColor4 *shCoefficients, CRuColor4 &lightColor, CRuVector3 &dirN);
	RuLightingInfo*							BuildLightingInfo(const CRuGUID &guid, const CRuMatrix4x4 &invWorldTransform, const CRuSphere &worldBounds);

	CRuParameterBlock&						GetDefaultParameterBlock();
	CRuSkinCache*							GetSkinCache();

	// Metrics
	UINT32									GetNumPrimitivesRendered();
	UINT32									GetNumBatchesIssued();

protected:
	BOOL									ClearLightingInfoHash();
	BOOL									ClearLightingInfoHashCallback(const void *key, void *data);

	BOOL									GarbageCollect(PTRVALUE userData);

	BOOL									GarbageCollectLightingInfoCallback(const void *key, void *data);
};

// ************************************************************************************************************************************************************

extern CRuRenderer*							g_ruRenderer;

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
