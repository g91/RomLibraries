/*!
	@file RuFX2D_UIFX.cpp

	Copyright (c) 2004-2006 All rights reserved
*/

#include "../../Scene/FX2D/RuFX2D_UIFX.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFX2D_UIFX_Base, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuFX2D_UIFX_Base, "CRuFX2D_UIFX_Base", "IRuObject")
ruCLASSGUID_DEFINE(CRuFX2D_UIFX_Base, 0xEBFF93EA, 0x770A45bf, 0x99709033, 0x45A57B3A)

ruRTTI_DEFINE_SUBCLASS(CRuFX2D_ImageTrail, CRuFX2D_UIFX_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFX2D_ImageTrail, "CRuFX2D_ImageTrail", "CRuFX2D_UIFX_Base")
ruCLASSGUID_DEFINE(CRuFX2D_ImageTrail, 0xA6F225A6, 0x4B1B4235, 0x9AB056A4, 0xF319C91B)

ruRTTI_DEFINE_SUBCLASS(CRuFX2D_IconHighlight, CRuFX2D_UIFX_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFX2D_IconHighlight, "CRuFX2D_IconHighlight", "CRuFX2D_UIFX_Base")
ruCLASSGUID_DEFINE(CRuFX2D_IconHighlight, 0x16C88184, 0x26A34455, 0x9D1F17D9, 0x80B0C4EE)

ruRTTI_DEFINE_SUBCLASS(CRuFX2D_IconCooldown, CRuFX2D_UIFX_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFX2D_IconCooldown, "CRuFX2D_IconCooldown", "CRuFX2D_UIFX_Base")
ruCLASSGUID_DEFINE(CRuFX2D_IconCooldown, 0xB0AF7F77, 0x9D004e1a, 0xA6EDCC5B, 0xB4F6B489)

// ************************************************************************************************************************************************************

CRuFX2D_UIFX_Base::CRuFX2D_UIFX_Base()
{
}

CRuFX2D_UIFX_Base::~CRuFX2D_UIFX_Base()
{
}

// ************************************************************************************************************************************************************

CRuFX2D_ImageTrail::CRuFX2D_ImageTrail()
:	m_imageTexture(NULL),
	m_imageTextureRect(0.0f, 0.0f, 32.0f, 32.0f),
	m_imageTrailLength(6),
	m_imageTrailOffset(2.0f),
	m_imageTrailBaseRect(0.0f, 0.0f, 5.0f, 5.0f),
	m_imageTrailColor(0xFFFFFFA0),
	m_imageTrailBaseAlpha(0.80f),
	m_imageTrailAlphaFactor(0.75f),
	m_imageTrailScaleFactor(0.80f),
	m_imageTrailTOffset(0.0f),
	m_imageTrailLooping(TRUE),
	m_pathTraversalSpeed(64.0f),
	m_t(0.0f)
{
}

CRuFX2D_ImageTrail::~CRuFX2D_ImageTrail()
{
	ruSAFE_RELEASE(m_imageTexture);
}

void CRuFX2D_ImageTrail::Update(REAL elapsedTime)
{
	// Perform update to current image trail time
	m_t += elapsedTime * m_pathTraversalSpeed;
}

void CRuFX2D_ImageTrail::Render(CRuRendererNode_UI *uiRenderer, REAL elapsedTime)
{
	if(m_pathPoints.Count() > 1)
	{
		REAL pathLength = m_pathPointTs[m_pathPointTs.Count() - 1];

		// Perform update to current image trail data
		m_t += elapsedTime * m_pathTraversalSpeed;

		if(m_imageTrailLooping)
			m_t = fmod(m_t, pathLength);

		for(INT32 i = m_imageTrailLength - 1; i >= 0; --i)
		{
			REAL alphaFactor = pow(m_imageTrailAlphaFactor, i);
			REAL scaleFactor = pow(m_imageTrailScaleFactor, i);

			REAL t = m_t - (i * m_imageTrailOffset) + m_imageTrailTOffset;

			if(m_imageTrailLooping)
				t = fmod(t + pathLength, pathLength);

			if(t > pathLength)
				continue;

			CRuVector3 imagePosition = GetImagePosition(t);

			REAL alphaValue = m_imageTrailBaseAlpha * alphaFactor;

			CRuRectangle scaledImageRect = m_imageTrailBaseRect;
			scaledImageRect *= scaleFactor;

			// do some offset or something

			CRuRectangle rect;
			rect.m_left = scaledImageRect.m_left + imagePosition.m_x - (scaledImageRect.m_right - scaledImageRect.m_left) / 2.0f + m_renderingOffset.m_x;
			rect.m_top = scaledImageRect.m_top + imagePosition.m_y - (scaledImageRect.m_bottom - scaledImageRect.m_top) / 2.0f + m_renderingOffset.m_y;
			rect.m_right = scaledImageRect.m_right + imagePosition.m_x - (scaledImageRect.m_right - scaledImageRect.m_left) / 2.0f + m_renderingOffset.m_x;
			rect.m_bottom = scaledImageRect.m_bottom + imagePosition.m_y - (scaledImageRect.m_bottom - scaledImageRect.m_top) / 2.0f + m_renderingOffset.m_y;

			BYTE alphaValue8 = static_cast<BYTE>(alphaValue * 255.0f);
			DWORD color = RUCOLOR_ARGB(alphaValue8, (m_imageTrailColor >> 16) & 0xFF, (m_imageTrailColor >> 8) & 0xFF, m_imageTrailColor & 0xFF);

			// Draw
			uiRenderer->SetEffectType(ru2DFXTYPE_BLIT);
			uiRenderer->SetTexture(m_imageTexture);
			uiRenderer->Submit(1, &m_imageTextureRect, &rect, &color);
		}
	}
}

void CRuFX2D_ImageTrail::SetImageTexture(IRuTexture *texture)
{
	if(texture)
	{
		// Store reference to new image texture
		texture->AddRef();
		ruSAFE_RELEASE(m_imageTexture);
		m_imageTexture = texture;

		// Store image texture dimensions
		m_imageTextureRect.m_left = 0.0f;
		m_imageTextureRect.m_top = 0.0f;
		m_imageTextureRect.m_right = static_cast<REAL>(m_imageTexture->GetLevelWidth(0));
		m_imageTextureRect.m_bottom = static_cast<REAL>(m_imageTexture->GetLevelHeight(0));
	}
}

void CRuFX2D_ImageTrail::SetImageTrailLength(INT32 length)
{
	m_imageTrailLength = length;
}

void CRuFX2D_ImageTrail::SetImageTrailBaseRect(const CRuRectangle &baseRect)
{
	m_imageTrailBaseRect = baseRect;
}

void CRuFX2D_ImageTrail::SetImageTrailOffset(REAL offset)
{
	m_imageTrailOffset = offset;
}

void CRuFX2D_ImageTrail::SetImageTrailColor(RUCOLOR color)
{
	m_imageTrailColor = color;
}

void CRuFX2D_ImageTrail::SetImageTrailBaseAlpha(REAL baseAlpha)
{
	m_imageTrailBaseAlpha = baseAlpha;
}

void CRuFX2D_ImageTrail::SetImageTrailAlphaFactor(REAL alphaFactor)
{
	m_imageTrailAlphaFactor = alphaFactor;
}

void CRuFX2D_ImageTrail::SetImageTrailScaleFactor(REAL scaleFactor)
{
	m_imageTrailScaleFactor = scaleFactor;
}

void CRuFX2D_ImageTrail::SetImageTrailTOffset(REAL tOffset)
{
	m_imageTrailTOffset = tOffset;
}

void CRuFX2D_ImageTrail::SetImageTrailTime(REAL time)
{
	m_t = time;
}

void CRuFX2D_ImageTrail::SetImageTrailLooping(BOOL loop)
{
	m_imageTrailLooping = loop;
}

void CRuFX2D_ImageTrail::SetPathPoints(const CRuArrayList<CRuVector3> &pathPoints)
{
	if(pathPoints.Count() > 1)
	{
		// Copy path points
		m_pathPoints.CloneFrom(pathPoints);

		// Initialize path point t values
		m_pathPointTs.Clear();
		m_pathPointTs.Add(0.0f);

		for(INT32 i = 1; i < m_pathPoints.Count(); ++i)
		{
			// Set t at path point i
			m_pathPointTs.Add(m_pathPointTs[i - 1] + (m_pathPoints[i] - m_pathPoints[i - 1]).Magnitude());
		}
	}
}

void CRuFX2D_ImageTrail::SetPathTraversalSpeed(REAL speed)
{
	m_pathTraversalSpeed = speed;
}

void CRuFX2D_ImageTrail::SetRenderingOffset(const CRuVector3 &renderingOffset)
{
	m_renderingOffset = renderingOffset;
}

CRuVector3 CRuFX2D_ImageTrail::GetImagePosition(REAL t)
{
	for(INT32 i = 1; i < m_pathPointTs.Count(); ++i)
	{
		if(m_pathPointTs[i] >= t)
		{
			REAL it1 = (t - m_pathPointTs[i - 1]) / (m_pathPointTs[i] - m_pathPointTs[i - 1]);
			REAL it0 = 1.0f - it1;

			return m_pathPoints[i - 1] * it0 + m_pathPoints[i] * it1;
		}
	}

	return CRuVector3(0.0f, 0.0f, 0.0f);
}

// ************************************************************************************************************************************************************

CRuFX2D_IconHighlight::CRuFX2D_IconHighlight()
{
	CRuArrayList<CRuVector3> pathPoints;
	pathPoints.Add(CRuVector3(0.0f, 0.0f, 0.0f));
	pathPoints.Add(CRuVector3(32.0f, 0.0f, 0.0f));
	pathPoints.Add(CRuVector3(32.0f, 32.0f, 0.0f));
	pathPoints.Add(CRuVector3(0.0f, 32.0f, 0.0f));
	pathPoints.Add(CRuVector3(0.0f, 0.0f, 0.0f));

	INT32 trailObjectCount = 3;
	REAL trailLength = 128.0f;
	REAL segmentLength = trailLength / trailObjectCount;

	IRuTexture *imageTexture = g_ruResourceManager->LoadTexture("texture\\interface\\circle", ruSURFACEFMT_DEFAULT, 0, TRUE);

	for(INT32 i = 0; i < trailObjectCount; ++i)
	{
		CRuFX2D_ImageTrail *imageTrail = ruNEW CRuFX2D_ImageTrail();

		imageTrail->SetImageTexture(imageTexture);
		imageTrail->SetImageTrailTOffset(i * segmentLength);
		imageTrail->SetPathPoints(pathPoints);

		m_trails.Add(imageTrail);
	}

	ruSAFE_RELEASE(imageTexture);
}

CRuFX2D_IconHighlight::~CRuFX2D_IconHighlight()
{
	for(INT32 i = 0; i < m_trails.Count(); ++i)
	{
		ruSAFE_RELEASE(m_trails[i]);
	}
}

void CRuFX2D_IconHighlight::Update(REAL elapsedTime)
{
	for(INT32 i = 0; i < m_trails.Count(); ++i)
	{
		m_trails[i]->Update(elapsedTime);
	}
}

void CRuFX2D_IconHighlight::Render(CRuRendererNode_UI *uiRenderer, REAL elapsedTime)
{
	for(INT32 i = 0; i < m_trails.Count(); ++i)
	{
		m_trails[i]->Render(uiRenderer, elapsedTime);
	}
}

void CRuFX2D_IconHighlight::SetIconSize(REAL width, REAL height)
{
	REAL wScale = width / 32.0f;
	REAL hScale = height / 32.0f;

	CRuArrayList<CRuVector3> pathPoints;
	pathPoints.Add(CRuVector3(0.0f  * wScale, 0.0f  * hScale, 0.0f));
	pathPoints.Add(CRuVector3(32.0f * wScale, 0.0f  * hScale, 0.0f));
	pathPoints.Add(CRuVector3(32.0f * wScale, 32.0f * hScale, 0.0f));
	pathPoints.Add(CRuVector3(0.0f  * wScale, 32.0f * hScale, 0.0f));
	pathPoints.Add(CRuVector3(0.0f  * wScale, 0.0f  * hScale, 0.0f));

	for(INT32 i = 0; i < m_trails.Count(); ++i)
	{
		m_trails[i]->SetPathPoints(pathPoints);
	}
}

void CRuFX2D_IconHighlight::SetRenderingOffset(const CRuVector3 &renderingOffset)
{
	// Set rendering offset into the image trail objects
	for(INT32 i = 0; i < m_trails.Count(); ++i)
	{
		m_trails[i]->SetRenderingOffset(renderingOffset);
	}
}

// ************************************************************************************************************************************************************

CRuFX2D_IconCooldown::CRuFX2D_IconCooldown()
{
	m_iconRect.Set(-16.0f, -16.0f, 16.0f, 16.0f);

	m_cooldownRemaining = 1.00f;
	m_cooldownLength = 1.00f;
	m_cooldownFinished = FALSE;

	m_renderingOffset.Set(32.0f, 32.0f, 0.0f);

	m_cooldownMesh = ruNEW CRuDynamicMesh();
	m_cooldownMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, 11, 9, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

	m_cooldownMaterial = ruNEW CRuMaterial();
	m_cooldownMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_FX2D_UIFX_DARKEN"));


	CRuArrayList<CRuVector3> pathPoints0;
	pathPoints0.Add(CRuVector3(16.0f, 0.0f, 0.0f));
	pathPoints0.Add(CRuVector3(32.0f, 0.0f, 0.0f));
	pathPoints0.Add(CRuVector3(32.0f, 32.0f, 0.0f));
	pathPoints0.Add(CRuVector3(16.0f, 32.0f, 0.0f));

	CRuArrayList<CRuVector3> pathPoints1;
	pathPoints1.Add(CRuVector3(16.0f, 0.0f, 0.0f));
	pathPoints1.Add(CRuVector3(0.0f, 0.0f, 0.0f));
	pathPoints1.Add(CRuVector3(0.0f, 32.0f, 0.0f));
	pathPoints1.Add(CRuVector3(16.0f, 32.0f, 0.0f));

	IRuTexture *imageTexture = g_ruResourceManager->LoadTexture("texture\\interface\\circle", ruSURFACEFMT_DEFAULT, 0, TRUE);

	CRuFX2D_ImageTrail *imageTrail0 = ruNEW CRuFX2D_ImageTrail();
	imageTrail0->SetImageTexture(imageTexture);
	imageTrail0->SetImageTrailLooping(FALSE);
	imageTrail0->SetPathPoints(pathPoints0);
	imageTrail0->SetPathTraversalSpeed(128.0f);
	m_trails.Add(imageTrail0);

	CRuFX2D_ImageTrail *imageTrail1 = ruNEW CRuFX2D_ImageTrail();
	imageTrail1->SetImageTexture(imageTexture);
	imageTrail1->SetImageTrailLooping(FALSE);
	imageTrail1->SetPathPoints(pathPoints1);
	imageTrail1->SetPathTraversalSpeed(128.0f);
	m_trails.Add(imageTrail1);

	ruSAFE_RELEASE(imageTexture);
}

CRuFX2D_IconCooldown::~CRuFX2D_IconCooldown()
{
	ruSAFE_RELEASE(m_cooldownMesh);
	ruSAFE_RELEASE(m_cooldownMaterial);

	for(INT32 i = 0; i < m_trails.Count(); ++i)
	{
		ruSAFE_RELEASE(m_trails[i]);
	}
}

void CRuFX2D_IconCooldown::Update(REAL elapsedTime)
{
	// Update cooldown time
	m_cooldownRemaining -= elapsedTime;
	if(m_cooldownRemaining < 0.0f)
		m_cooldownRemaining = 0.0f;

	// Calculate t in [0, 1]
	REAL t = m_cooldownRemaining / m_cooldownLength;

	// Perform the render only if the cooldown indicator is actually visible (t must be greater than zero)
	if(t > 0.0f)
	{
	}
	else
	{
		for(INT32 i = 0; i < m_trails.Count(); ++i)
		{
			m_trails[i]->Update(elapsedTime);
		}
	}
}

void CRuFX2D_IconCooldown::Render(CRuRendererNode_UI *uiRenderer, REAL elapsedTime)
{
	// Update cooldown time
	m_cooldownRemaining -= elapsedTime;
	if(m_cooldownRemaining < 0.0f)
		m_cooldownRemaining = 0.0f;

	// Calculate t in [0, 1]
	REAL t = m_cooldownRemaining / m_cooldownLength;

	// Perform the render only if the cooldown indicator is actually visible (t must be greater than zero)
	if(t > 0.0f)
	{
		CRuRectangle actualIconRect = m_iconRect;
		uiRenderer->ConvertCoordinates(actualIconRect);

		CRuVector3 actualRenderingOffset = m_renderingOffset;
		uiRenderer->ConvertCoordinates(actualRenderingOffset);

		REAL fixedSectionInterval = 1.0f / 8.0f;
		INT32 numFixedFanSections = static_cast<INT32>(t / fixedSectionInterval) + 1;

		CRuVector3 iconCenter	(
									(actualIconRect.m_right - actualIconRect.m_left) / 2.0f + actualRenderingOffset.m_x,
									(actualIconRect.m_bottom - actualIconRect.m_top) / 2.0f + actualRenderingOffset.m_y,
									0.0f
								);

		REAL iconBoundingRadius = RuSqrt(pow(max(actualIconRect.m_right - actualIconRect.m_left, actualIconRect.m_bottom - actualIconRect.m_top) / 2.0f, 2));

		// Lock mesh
		INT32 vIdx = 0;
		IRuMorphTarget *morphTarget = m_cooldownMesh->GetMorphTarget(0);
		CRuVector3 *position = morphTarget->GetPosition();
		DWORD *diffuse = morphTarget->GetDiffuse();

		// Place origin point
		position[vIdx] = iconCenter;
		diffuse[vIdx] = 0x77FFFFFF;
		++vIdx;

		// Place fixed fan points
		for(INT32 i = 0; i < numFixedFanSections; ++i)
		{
			position[vIdx] = CalculateFanVertex(i * fixedSectionInterval, actualIconRect, actualRenderingOffset);
			diffuse[vIdx] = 0x77FFFFFF;
			++vIdx;
		}

		// Place final fan point
		position[vIdx] = CalculateFanVertex(t, actualIconRect, actualRenderingOffset);
		diffuse[vIdx] = 0x77FFFFFF;
		++vIdx;

		// Convert coordinates
		for(INT32 i = 0; i < vIdx; ++i)
		{
			uiRenderer->ViewportToHomogeneous(position[i]);
		}

		// Initialize fan indices
		UINT16 *indices = m_cooldownMesh->GetIndices();

		for(INT32 i = 2, iIdx = 0; i < vIdx; ++i)
		{
			indices[iIdx++] = 0;
			indices[iIdx++] = i - 1;
			indices[iIdx++] = i;
		}

		// Set vertex & index count
		m_cooldownMesh->SetNumVertices(vIdx);
		m_cooldownMesh->SetNumPrimitives(vIdx - 2);

		// Increment mesh iteration
		m_cooldownMesh->IncrementMeshIteration();
		m_cooldownMesh->IncrementTopologyIteration();

		// Render
		uiRenderer->Submit(m_cooldownMesh, m_cooldownMaterial);
	}
	else
	{
		if(m_cooldownFinished == FALSE)
		{
			m_cooldownFinished = TRUE;

			for(INT32 i = 0; i < m_trails.Count(); ++i)
			{
				m_trails[i]->SetImageTrailTime(0.0f);
			}
		}

		// Start cooldown finish animation
		for(INT32 i = 0; i < m_trails.Count(); ++i)
		{
			m_trails[i]->Render(uiRenderer, elapsedTime);
		}
	}
}

void CRuFX2D_IconCooldown::SetIconSize(REAL width, REAL height)
{
	m_iconRect.Set(-width / 2.0f, -height / 2.0f, width / 2.0f, height / 2.0f);

	REAL wScale = width / 32.0f;
	REAL hScale = height / 32.0f;

	CRuArrayList<CRuVector3> pathPoints0;
	pathPoints0.Add(CRuVector3(16.0f * wScale, 0.0f  * hScale, 0.0f));
	pathPoints0.Add(CRuVector3(32.0f * wScale, 0.0f  * hScale, 0.0f));
	pathPoints0.Add(CRuVector3(32.0f * wScale, 32.0f * hScale, 0.0f));
	pathPoints0.Add(CRuVector3(16.0f * wScale, 32.0f * hScale, 0.0f));

	CRuArrayList<CRuVector3> pathPoints1;
	pathPoints1.Add(CRuVector3(16.0f * wScale, 0.0f  * hScale, 0.0f));
	pathPoints1.Add(CRuVector3(0.0f  * wScale, 0.0f  * hScale, 0.0f));
	pathPoints1.Add(CRuVector3(0.0f  * wScale, 32.0f * hScale, 0.0f));
	pathPoints1.Add(CRuVector3(16.0f * wScale, 32.0f * hScale, 0.0f));

	m_trails[0]->SetPathPoints(pathPoints0);
	m_trails[1]->SetPathPoints(pathPoints1);
}

void CRuFX2D_IconCooldown::SetRenderingOffset(const CRuVector3 &renderingOffset)
{
	m_renderingOffset = renderingOffset;

	// Set rendering offset into the image trail objects
	for(INT32 i = 0; i < m_trails.Count(); ++i)
	{
		m_trails[i]->SetRenderingOffset(renderingOffset);
	}
}

void CRuFX2D_IconCooldown::StartCooldown(REAL cooldownLength, REAL cooldownRemaining)
{
	m_cooldownRemaining = cooldownRemaining;
	m_cooldownLength = cooldownLength;
	m_cooldownFinished = FALSE;
}

REAL CRuFX2D_IconCooldown::GetCooldownRemaining()
{
	return m_cooldownRemaining;
}

CRuVector3 CRuFX2D_IconCooldown::CalculateFanVertex(REAL t, CRuRectangle &iconRect, CRuVector3 &renderingOffset)
{
	CRuRectangle iconBoundingRect	(
										iconRect.m_left + renderingOffset.m_x,
										iconRect.m_top + renderingOffset.m_y,
										iconRect.m_right + renderingOffset.m_x,
										iconRect.m_bottom + renderingOffset.m_y
									);

	REAL width = iconRect.m_right - iconRect.m_left;
	REAL height = iconRect.m_bottom - iconRect.m_top;

	CRuVector3 iconCenter	(
								(iconRect.m_right - iconRect.m_left) / 2.0f + renderingOffset.m_x,
								(iconRect.m_bottom - iconRect.m_top) / 2.0f + renderingOffset.m_y,
								0.0f
							);

	REAL iconBoundingRadius = RuSqrt(pow(width * width + height * height, 2));

	CRuVector3 fanVertex(0.0f, 1.0f, 0.0f);

	// Transform fan vertex into correct angle
	CRuQuaternion rotQuat;
	rotQuat.FromAngleAxis(CRuVector3(0.0f, 0.0f, 1.0f), t * ruPI * 2.0f);
	rotQuat.TransformVector(fanVertex);

	// Invert y axis
	fanVertex *= -1.0f;

	// The radial ray will intersect along the vertical axis
	if(fabs(fanVertex.m_x) + ruEPSILON >= fabs(fanVertex.m_y))
	{
		// Calculate slope along x axis
		REAL slope = (fanVertex.m_y * height) / (fanVertex.m_x * width);

		if(fanVertex.m_x > 0.0f)
		{
			fanVertex.m_x = iconRect.m_right + iconCenter.m_x;
			fanVertex.m_y = slope * iconRect.m_right + iconCenter.m_y;
		}
		else
		{
			fanVertex.m_x = iconRect.m_left + iconCenter.m_x;
			fanVertex.m_y = slope * iconRect.m_left + iconCenter.m_y;
		}
	}
	// The radial ray will intersect along the horizontal axis
	else
	{
		// Calculate slope along y axis
		REAL slope = (fanVertex.m_x * width) / (fanVertex.m_y * height);

		if(fanVertex.m_y > 0.0f)
		{
			fanVertex.m_x = slope * iconRect.m_bottom + iconCenter.m_x;
			fanVertex.m_y = iconRect.m_bottom + iconCenter.m_y;
		}
		else
		{
			fanVertex.m_x = slope * iconRect.m_top + iconCenter.m_x;
			fanVertex.m_y = iconRect.m_top + iconCenter.m_y;
		}
	}

	return fanVertex;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
