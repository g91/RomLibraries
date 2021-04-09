#ifndef _RUFUSION_MINIMAP_H_
#define _RUFUSION_MINIMAP_H_

// Base types
#include "../Fusion/RuFusion_UIElement.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuFusion_Minimap : public CRuFusion_UIElement
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	char*							m_worldName;						//!< Name of the world
	CRuAABB							m_worldBounds;						//!< Bounds of the world map

	INT32							m_minimapLayer;						//!< Current layer to display

	CRuRectangle					m_minimapViewport;					//!< Minimap viewport
	CRuVector2						m_minimapCentroid;					//!< Centroid of the minimap relative to the viewport
	CRuVector3						m_playerPosition;					//!< Current player origin

	REAL							m_zoom;
	REAL							m_texelScale;						//!< Minimap texel scale
	REAL							m_worldScale;						//!< Scale to convert from minimap coordinates to world coordinates

	IRuTexture*						m_minimapMask;						//!< Mask used for the minimap

public:
									CRuFusion_Minimap();
	virtual							~CRuFusion_Minimap();

	void							SetWorldProperties(const char *worldName, const CRuAABB &worldBounds);
	void							SetMinimapLayer(INT32 layer);
	void							SetMinimapViewport(const CRuRectangle &viewport);
	void							SetMinimapCentroid(const CRuVector2 &centroid);
	void							SetPlayerPosition(const CRuVector3 &position);
	void							SetPlayerDir(const CRuVector3 &dir);
	void							SetZoom(REAL zoom);
	void							SetArrow(IRuTexture *minimapMask);
	void							SetMinimapMask(IRuTexture *minimapMask);


	// Deprecated
	void							SetMinimapOrigin(const CRuVector2 &origin);


	// Conversion routines
	CRuVector3						WorldToMinimap(const CRuVector3 &position);
	CRuVector3						MinimapToWorld(const CRuVector3 &position);
	REAL							MinimapRadiusToWorldRadius(  REAL Radius );

	virtual void					Display();
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
