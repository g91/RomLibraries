#include "../Fusion/RuFusion_Minimap.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFusion_Minimap, CRuFusion_UIElement)
ruHEAP_DEFINE_SUBCLASS(CRuFusion_Minimap, "CRuFusion_Minimap", "CRuFusion_UIElement")
ruCLASSGUID_DEFINE(CRuFusion_Minimap, 0x17F75534, 0xAE1540ea, 0xBB660D81, 0x21BEF9AB)

// ************************************************************************************************************************************************************

CRuFusion_Minimap::CRuFusion_Minimap()
:	m_worldName(NULL),
	m_minimapLayer(0),
	m_zoom(1.0f),
	m_minimapMask(NULL)
{
}

CRuFusion_Minimap::~CRuFusion_Minimap()
{
	delete[] m_worldName;

	ruSAFE_RELEASE(m_minimapMask);
}

void CRuFusion_Minimap::SetWorldProperties(const char *worldName, const CRuAABB &worldBounds)
{
	// Truncate path preceeding the world name
	CRuString localWorldName(worldName);
	localWorldName.TruncateBefore(max(localWorldName.LastIndexOf("\\"), localWorldName.LastIndexOf("/")) + 1);

	// Truncate file extension from the world name
	localWorldName.Replace(".wdb", "");

	// Copy world name
	delete[] m_worldName;
	m_worldName = ruNEW char [localWorldName.GetLength() + 1];
	strcpy(m_worldName, localWorldName);

	// Copy world bounds
	m_worldBounds = worldBounds;
}

void CRuFusion_Minimap::SetMinimapLayer(INT32 layer)
{
	m_minimapLayer = layer;
}

void CRuFusion_Minimap::SetMinimapViewport(const CRuRectangle &viewport)
{
	m_minimapViewport = viewport;
}

void CRuFusion_Minimap::SetMinimapCentroid(const CRuVector2 &centroid)
{
	m_minimapCentroid = centroid;
}

void CRuFusion_Minimap::SetMinimapOrigin(const CRuVector2 &origin)
{
	m_minimapCentroid = origin;
}

void CRuFusion_Minimap::SetPlayerPosition(const CRuVector3 &position)
{
	m_playerPosition = position;
}

void CRuFusion_Minimap::SetPlayerDir(const CRuVector3 &dir)
{
}

void CRuFusion_Minimap::SetZoom(REAL zoom)
{
	m_zoom = zoom;
}

void CRuFusion_Minimap::SetMinimapMask(IRuTexture *minimapMask)
{
	if(minimapMask)
	{
		minimapMask->AddRef();
		ruSAFE_RELEASE(m_minimapMask);
		m_minimapMask = minimapMask;
	}
}

void CRuFusion_Minimap::SetArrow(IRuTexture *minimapMask)
{
}

CRuVector3 CRuFusion_Minimap::WorldToMinimap(const CRuVector3 &position)
{
	//REAL texelsPerWorldUnit = m_texelScale / m_worldScale;

	//CRuVector2 relativePosition_WorldUnits(position.m_x - m_playerPosition.m_x, position.m_z - m_playerPosition.m_z);
	//CRuVector2 relativePosition_TexelUnits(relativePosition_WorldUnits.m_x * texelsPerWorldUnit, relativePosition_WorldUnits.m_y * texelsPerWorldUnit);
	//CRuVector2 relativePosition_ViewportUnits(m_minimapCentroid.m_x + relativePosition_TexelUnits.m_x * m_zoom, m_minimapCentroid.m_y + -relativePosition_TexelUnits.m_y * m_zoom);

	//CRuVector2 relativePosition_ScreenUnits(relativePosition_ViewportUnits.m_x + m_minimapViewport.m_left, relativePosition_ViewportUnits.m_y + m_minimapViewport.m_top);

	//return CRuVector3(relativePosition_ScreenUnits.m_x, relativePosition_ScreenUnits.m_y, 0.0f);

	//04:39 2007/8/27 by Xun
	//因為後續處理使用相對位置會較為方便 所以改成下面做法

	REAL texelsPerWorldUnit = m_texelScale * m_zoom / m_worldScale;

	return CRuVector3(
		( position.m_x - m_playerPosition.m_x ) * texelsPerWorldUnit ,
		-( position.m_z - m_playerPosition.m_z ) * texelsPerWorldUnit ,
		0.0f );
}

CRuVector3 CRuFusion_Minimap::MinimapToWorld(const CRuVector3 &position)
{
	REAL WorldPerTexelUnit = m_worldScale / ( m_texelScale * m_zoom );

	return CRuVector3( 
		position.m_x * WorldPerTexelUnit + m_playerPosition.m_x , 
		0.0 , 
		( -position.m_y * WorldPerTexelUnit ) + m_playerPosition.m_z );
}

REAL CRuFusion_Minimap::MinimapRadiusToWorldRadius( REAL Radius )
{
	//REAL WorldPerTexelUnit = m_worldScale / ( m_texelScale * m_zoom );
	//return ( m_minimapViewport.m_left - m_minimapViewport.m_right ) / WorldPerTexelUnit;
	return Radius * m_worldScale / ( m_texelScale * m_zoom );
}


void CRuFusion_Minimap::Display()
{
	Ru2D_EffectType displayEffect = ru2DFXTYPE_BLIT_FILTER;

	if(m_minimapMask)
	{
		displayEffect = ru2DFXTYPE_BLIT_MASKED;

		// Draw minimap mask
		g_ruUIRenderer->SetEffectType(ru2DFXTYPE_BLIT_MASK);
		g_ruUIRenderer->SetEffectParameter(ru2DFXPARAM_ROTATE, ru2DROTATE_NONE);
		g_ruUIRenderer->SetTexture(m_minimapMask);

		CRuRectangle srcRect;

		srcRect.m_left = 0.0f;
		srcRect.m_top = 0.0f;
		srcRect.m_right = static_cast<REAL>(m_minimapMask->GetLevelWidth(0));
		srcRect.m_bottom = static_cast<REAL>(m_minimapMask->GetLevelHeight(0));

		g_ruUIRenderer->Submit(srcRect, m_minimapViewport);
	}

	{
		// Render a black patch on top of the minimap area
		g_ruUIRenderer->SetEffectType(displayEffect == ru2DFXTYPE_BLIT_MASKED ? ru2DFXTYPE_FLAT_MASKED : ru2DFXTYPE_FLAT);
		g_ruUIRenderer->SetEffectParameter(ru2DFXPARAM_ROTATE, ru2DROTATE_NONE);
		g_ruUIRenderer->SetEffectParameter(ru2DFXPARAM_COLOR, 0xFF000000);

		CRuRectangle srcRect;
		g_ruUIRenderer->Submit(srcRect, m_minimapViewport);
	}

	char *m_minimapPath = "texture\\minimap";

	/*
		NOTES
		-----
		There is much zen in this coordinate conversion code. So much that even I cannot
		understand exactly what it does to accomplish the task. But it does work, so
		that's all that really matters.
	*/

	m_texelScale = 256.0f;			// Number of texels per section (section defined as the bitmap size of each minimap texture)
	m_worldScale = 480.0f * 8.0f;	// Size of each section in world units

	CRuVector2 viewportDimension(m_minimapViewport.m_right - m_minimapViewport.m_left, m_minimapViewport.m_bottom - m_minimapViewport.m_top);
	CRuVector2 minimapOrigin(m_minimapCentroid);
	CRuVector2 minimapCenter(viewportDimension.x / 2.0f, viewportDimension.y / 2.0f);
	CRuVector2 minimapOriginOffset = minimapCenter - minimapOrigin;

	// Scale viewport dimension by zoom factor
	viewportDimension = viewportDimension / m_zoom;
	minimapOrigin = minimapOrigin / m_zoom;
	minimapCenter = minimapCenter / m_zoom;
	minimapOriginOffset = minimapOriginOffset / m_zoom;

	// Convert viewport dimensions to minimap coordinates
	CRuVector2 viewportDimensionMM = viewportDimension / m_texelScale;
	CRuVector2 viewportDimensionW = viewportDimensionMM * m_worldScale;

	CRuVector2 minimapOriginMM = minimapOrigin / m_texelScale;
	CRuVector2 minimapOriginW = minimapOriginMM * m_worldScale;

	CRuVector2 minimapCenterW = (minimapCenter / m_texelScale) * m_worldScale;
	CRuVector2 minimapOriginOffsetW = (minimapOriginOffset / m_texelScale) * m_worldScale;

	CRuRectangle viewportW;
	viewportW.m_left = m_playerPosition.x - minimapCenterW.x + minimapOriginOffsetW.x;
	viewportW.m_top = m_playerPosition.z - minimapCenterW.y - minimapOriginOffsetW.y;
	viewportW.m_right = viewportW.m_left + viewportDimensionW.x;
	viewportW.m_bottom = viewportW.m_top + viewportDimensionW.y;

	CRuRectangle viewportM;
	viewportM.m_left = (viewportW.m_left - m_worldBounds.Minima().x) / m_worldScale;
	viewportM.m_right = (viewportW.m_right - m_worldBounds.Minima().x) / m_worldScale;
	viewportM.m_top = (viewportW.m_top - m_worldBounds.Minima().z) / m_worldScale;
	viewportM.m_bottom = (viewportW.m_bottom - m_worldBounds.Minima().z) / m_worldScale;

	char textureName[2048];

	for(INT32 z = static_cast<INT32>(floor(viewportM.m_top)); z <= ceil(viewportM.m_bottom); ++z)
	{
		for(INT32 x = static_cast<INT32>(floor(viewportM.m_left)); x <= ceil(viewportM.m_right); ++x)
		{
			// Convert from minimap coordinates to viewport coordinates
			CRuRectangle targetRect;
			targetRect.m_left = (((x * m_worldScale) + m_worldBounds.Minima().x) - viewportW.m_left) / m_worldScale * (m_texelScale * m_zoom) + m_minimapViewport.m_left;
			targetRect.m_left = floor(targetRect.m_left);

			targetRect.m_right = targetRect.m_left + (m_texelScale * m_zoom) + 1.0f;

			targetRect.m_top = -(((z * m_worldScale) + m_worldBounds.Minima().z) - viewportW.m_bottom) / m_worldScale * (m_texelScale * m_zoom) - (m_texelScale * m_zoom) + m_minimapViewport.m_top;
			targetRect.m_top = floor(targetRect.m_top);

			targetRect.m_bottom = targetRect.m_top + (m_texelScale * m_zoom) + 1.0f;

			sprintf(textureName, "%s\\%s\\%d\\minimap_%d_%d", m_minimapPath, m_worldName, m_minimapLayer, x, z);

			IRuTexture *testTexture = g_ruResourceManager->LoadTexture(textureName);

			if(testTexture)
			{
				CRuRectangle srcRect, destRect;

				srcRect.m_left = 0.0f;
				srcRect.m_top = 0.0f;
				srcRect.m_right = static_cast<REAL>(testTexture->GetLevelWidth(0));
				srcRect.m_bottom = static_cast<REAL>(testTexture->GetLevelHeight(0));

				destRect = targetRect;

				g_ruUIRenderer->SetEffectType(displayEffect);
				g_ruUIRenderer->SetTexture(testTexture, FALSE);
				g_ruUIRenderer->Submit(srcRect, destRect);

				ruSAFE_RELEASE(testTexture);
			}
		}
	}
}

// ************************************************************************************************************************************************************

#pragma managed
