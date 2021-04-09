/*!
	@file RuFX2D_Model.h

	Copyright (c) 2004-2006 All rights reserved
*/

#ifndef _RUFX2D_MODEL_H_
#define _RUFX2D_MODEL_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_UI.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	FX2D model display.

	@version 2007.09.26
*/
class CRuFX2D_Model : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct TargetBoneDesc
	{
		CRuString							m_boneName;
		INT32								m_boneIndex;
	};

	// Stencil masking texture
	IRuTexture*								m_maskingTexture;

	// Base model data
	CRuEntity*								m_model;
	CRuVector3								m_modelOffset;
	REAL									m_modelScale;

	// Model bone targeting data
	CRuHierarchy_GR2*						m_targetHierarchy;
	CRuArrayList<TargetBoneDesc *>			m_targetBones;
	BOOL									m_targetBonesDirty;
	BOOL									m_validTargetBoneExists;

	// Model frame alignment data
	char*									m_alignmentFrame_0;
	char*									m_alignmentFrame_1;
	CRuVector3								m_alignmentBias;
	REAL									m_alignmentScale;

	// Camera settings
	CRuVector3								m_relativeCameraPosition;

	// Run-time generated model data
	BOOL									m_modelBoundsDirty;
	CRuSphere								m_modelBounds;

	// Viewport
	BOOL									m_enableModelAutoPlacement;
	CRuRectangle							m_viewportRect;
	IRuViewport*							m_viewport;

	// Model camera
	CRuCamera*								m_modelCamera;

public:
											CRuFX2D_Model();
	virtual									~CRuFX2D_Model();

	void									SetMaskingTexture(IRuTexture *maskingTexture);

	BOOL									SetModel(CRuEntity *model);
	BOOL									SetModelOffset(const CRuVector3 &modelOffset);
	BOOL									SetModelScale(REAL scale);

	BOOL									AddTargetBone(const char *targetBoneName);
	BOOL									ClearTargetBones();

	BOOL									SetAlignmentFrames(const char *frame0, const char *frame1, REAL bias, REAL scale);
	BOOL									SetAlignmentFrames(const char *frame0, const char *frame1, const CRuVector3 &bias, REAL scale);
	BOOL									ClearAlignmentFrames();

	void									SetRelativeCameraPosition(const CRuVector3 &relativeCamPos);

	void									SetViewRect(const CRuRectangle &viewRect);

	void									Render(CRuRendererNode_UI *uiRenderer);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);

protected:
	void									UpdateTargetBones();
	void									UpdateModelBounds();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_FX2D_Model, CRuFX2D_Model)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
