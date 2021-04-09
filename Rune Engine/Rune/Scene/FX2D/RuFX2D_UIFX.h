/*!
	@file RuFX2D_UIFX.h

	Copyright (c) 2004-2006 All rights reserved
*/

#ifndef _RUFX2D_UIFX_H_
#define _RUFX2D_UIFX_H_

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_UI.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	@version 2006.12.22
*/
class CRuFX2D_UIFX_Base : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:

public:
											CRuFX2D_UIFX_Base();
	virtual									~CRuFX2D_UIFX_Base();

	virtual void							Update(REAL elapsedTime) = 0;
	virtual void							Render(CRuRendererNode_UI *uiRenderer, REAL elapsedTime) = 0;
};

// ************************************************************************************************************************************************************

/*!
	@version 2006.12.22
*/
class CRuFX2D_ImageTrail : public CRuFX2D_UIFX_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Settings
	IRuTexture*								m_imageTexture;								//!< Texture to use for image trail
	CRuRectangle							m_imageTextureRect;

	INT32									m_imageTrailLength;							//!< Number of images on the trail
	REAL									m_imageTrailOffset;							//!< Offset between each image on the trail
	CRuRectangle							m_imageTrailBaseRect;
	RUCOLOR									m_imageTrailColor;
	REAL									m_imageTrailBaseAlpha;
	REAL									m_imageTrailAlphaFactor;
	REAL									m_imageTrailScaleFactor;
	REAL									m_imageTrailTOffset;
	BOOL									m_imageTrailLooping;

	CRuArrayList<CRuVector3>				m_pathPoints;								//!< Path points the image trail travels on
	CRuArrayList<REAL>						m_pathPointTs;
	REAL									m_pathTraversalSpeed;						//!< Speed the image trail moves at, in units per second

	CRuVector3								m_renderingOffset;

	// Run-time
	REAL									m_t;

public:
											CRuFX2D_ImageTrail();
	virtual									~CRuFX2D_ImageTrail();

	virtual void							Update(REAL elapsedTime);
	virtual void							Render(CRuRendererNode_UI *uiRenderer, REAL elapsedTime);

	void									SetImageTexture(IRuTexture *texture);

	void									SetImageTrailLength(INT32 length);
	void									SetImageTrailOffset(REAL offset);
	void									SetImageTrailBaseRect(const CRuRectangle &baseRect);
	void									SetImageTrailColor(RUCOLOR color);
	void									SetImageTrailBaseAlpha(REAL baseAlpha);
	void									SetImageTrailAlphaFactor(REAL alphaFactor);
	void									SetImageTrailScaleFactor(REAL scaleFactor);
	void									SetImageTrailTOffset(REAL tOffset);
	void									SetImageTrailTime(REAL time);
	void									SetImageTrailLooping(BOOL loop);

	void									SetPathPoints(const CRuArrayList<CRuVector3> &pathPoints);
	void									SetPathTraversalSpeed(REAL speed);

	void									SetRenderingOffset(const CRuVector3 &renderingOffset);

protected:
	CRuVector3								GetImagePosition(REAL t);
};

// ************************************************************************************************************************************************************

class CRuFX2D_IconHighlight : public CRuFX2D_UIFX_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuArrayList<CRuFX2D_ImageTrail *>		m_trails;

public:
											CRuFX2D_IconHighlight();
	virtual									~CRuFX2D_IconHighlight();

	virtual void							Update(REAL elapsedTime);
	virtual void							Render(CRuRendererNode_UI *uiRenderer, REAL elapsedTime);

	void									SetIconSize(REAL width, REAL height);
	void									SetRenderingOffset(const CRuVector3 &renderingOffset);
};

// ************************************************************************************************************************************************************

class CRuFX2D_IconCooldown : public CRuFX2D_UIFX_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuRectangle							m_iconRect;									//!< Bounding rect of the cooldown indicator

	REAL									m_cooldownRemaining;						//!< Length of time remaining on cooldown counter
	REAL									m_cooldownLength;
	BOOL									m_cooldownFinished;

	CRuVector3								m_renderingOffset;

	CRuDynamicMesh*							m_cooldownMesh;
	CRuMaterial*							m_cooldownMaterial;

	CRuArrayList<CRuFX2D_ImageTrail *>		m_trails;

public:
											CRuFX2D_IconCooldown();
	virtual									~CRuFX2D_IconCooldown();

	virtual void							Update(REAL elapsedTime);
	virtual void							Render(CRuRendererNode_UI *uiRenderer, REAL elapsedTime);

	void									SetIconSize(REAL width, REAL height);
	void									SetRenderingOffset(const CRuVector3 &renderingOffset);
	void									StartCooldown(REAL cooldownLength, REAL cooldownRemaining);
	REAL									GetCooldownRemaining();

protected:
	CRuVector3								CalculateFanVertex(REAL t, CRuRectangle &iconRect, CRuVector3 &renderingOffset);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
