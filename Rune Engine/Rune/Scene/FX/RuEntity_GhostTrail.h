/*!
	@file RuEntity_GhostTrail.h

	Copyright (c) 2004-2006 All rights reserved
*/

#ifndef _RUENTITY_GHOSTTRAIL_H_
#define _RUENTITY_GHOSTTRAIL_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Ghost trail entity.

	@version 2006.05.08
*/
class CRuEntity_GhostTrail : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ControlPointDesc
	{
		REAL								m_age;											//!< Age of segment
		REAL								m_u;											//!< u coordinate
		REAL								m_alpha;										//!< Alpha factor
		CRuVector3							m_p0;											//!< Segment anchor 0
		CRuVector3							m_p1;											//!< Segment anchor 1
	};

	// Settings
	RuFrameType								m_frameType;									//!< Ghost trail frame-of-reference
	REAL									m_tInterval;									//!< Trail update interval
	REAL									m_segmentInterval;								//!< Segment interval
	REAL									m_maxAge;										//!< Max segment age
	REAL									m_fadeCoefficients[3];							//!< Fade coefficients
	INT32									m_maxControlPoints;								//!< Maximum number of control points
	INT32									m_maxTrailSegments;								//!< Maximum number of trail segments
	REAL									m_activeDuration;								//!< Time during which segment generation is active
	REAL									m_fadeOutDuration;								//!< Fade out duration
	INT32									m_trailTextureFrameCount;						//!< Trail texture frame count
	REAL									m_trailTextureFrameRate;						//!< Trail texture frame rate
	INT32									m_numSubdivisions;								//!< Number of subdivisions along segment strip

	// Run-time data
	INT32									m_numControlPoints;								//!< Number of active control points
	INT32									m_controlSegmentHead;							//!< Head segment, defined as the oldest active segment
	INT32									m_controlSegmentTail;							//!< Tail segment, defined as the youngest active segment
	ControlPointDesc*						m_controlPoints;								//!< Control segment descriptors

	INT32									m_numSegments;									//!< Number of active segments
	ControlPointDesc*						m_segments;										//!< Segment descriptors

	CRuEntity*								m_anchor0;										//!< Anchor 0
	CRuEntity*								m_anchor1;										//!< Anchor 1

	REAL									m_t;											//!< Current time
	REAL									m_accumT;
	REAL									m_trailAccumT;
	INT32									m_trailTextureFrameIndex;
	REAL									m_alphaFactor;									//!< Current fade-out alpha factor

	BOOL									m_visible;
	CRuDynamicMesh*							m_mesh;
	CRuMaterial*							m_material;

public:
											CRuEntity_GhostTrail();
	virtual									~CRuEntity_GhostTrail();

	void									SetFrameType(RuFrameType frameType);
	void									SetUpdateInterval(REAL updateInterval);
	void									SetSegmentInterval(REAL segmentInterval);
	void									SetMaxAge(REAL maxAge);
	void									SetFadeCoefficients(REAL linear, REAL quadratic, REAL cubic);
	void									SetMaxControlPoints(INT32 maxControlPoints);
	void									SetMaxSegments(INT32 maxSegments);
	void									SetActiveDuration(REAL duration);
	void									SetFadeOutDuration(REAL duration);

	BOOL									SetTrailAnchors(CRuEntity *p0, CRuEntity *p1);
	BOOL									SetTrailTexture(IRuTexture *texture);
	void									SetTrailFrameCount(INT32 frameCount);
	void									SetTrailFrameRate(REAL frameRate);

	// Base entity interface
	virtual CRuEntity*						Clone(CRuEntity *clonedEntity = NULL);

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	// IRuEntity_Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
