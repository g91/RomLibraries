#ifndef _RUCONTROLLER_GHOSTTRAIL_H_
#define _RUCONTROLLER_GHOSTTRAIL_H_

#include "../../Scene/Base/RuEntityController.h"
#include "../../Scene/Controller/RuController_MaterialTexture.h"
#include "../../Scene/FX/RuEntity_GhostTrail.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuController_GhostTrail : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity_Container_Impl*				m_targetEntity;									//!< Entity that the controller affects

	// Ghost trail setting
	RuFrameType								m_frameType;									//!< Ghost trail frame-of-reference
	REAL									m_tInterval;									//!< Sampling interval for the trail
	REAL									m_segmentInterval;								//!< Segment interval
	REAL									m_maxAge;										//!< Maximum duration for which a segment is visible
	REAL									m_fadeCoefficients[3];							//!< Fade coefficients [linear, quadratic, cubic]
	INT32									m_maxControlPoints;								//!< Maximum number of control points
	INT32									m_maxTrailSegments;								//!< Maximum number of segments
	REAL									m_activeDuration;								//!< Time during which segment generation is active
	REAL									m_fadeOutDuration;								//!< Fade out duration

	char*									m_anchor0;										//!< Anchor entity 0
	char*									m_anchor1;										//!< Anchor entity 1

	char*									m_trailTextureName;
	INT32									m_trailTextureFrameCount;
	REAL									m_trailTextureFrameRate;

	BOOL									m_distortFXEnable;
	char*									m_distortTextureName;
	INT32									m_distortTextureFrameCount;
	REAL									m_distortTextureFrameRate;

	BOOL									m_attached;
	BOOL									m_active;
	CRuEntity_GhostTrail*					m_ghostTrail;									//!< Ghost trail entity
	CRuController_MaterialTexture*			m_distortFXMatController;

public:
											CRuController_GhostTrail();
	virtual									~CRuController_GhostTrail();

	RuFrameType								GetFrameType() const;
	void									SetFrameType(RuFrameType frameType);

	REAL									GetInterval() const;
	void									SetInterval(REAL interval);

	REAL									GetSegmentInterval() const;
	void									SetSegmentInterval(REAL interval);

	REAL									GetMaxAge() const;
	void									SetMaxAge(REAL maxAge);

	REAL									GetFadeCoefficient(INT32 coefficientIndex) const;
	void									SetFadeCoefficient(INT32 coefficientIndex, REAL coefficient);

	INT32									GetMaxControlPoints() const;	
	void									SetMaxControlPoints(INT32 maxControlPoints);

	INT32									GetMaxSegments() const;	
	void									SetMaxSegments(INT32 maxSegments);

	REAL									GetActiveDuration() const;
	void									SetActiveDuration(REAL duration);

	REAL									GetFadeOutDuration() const;
	void									SetFadeOutDuration(REAL duration);

	const char*								GetTrailAnchor(INT32 anchorIndex) const;
	void									SetTrailAnchor(INT32 anchorIndex, const char *anchor);

	const char*								GetTrailTextureName() const;
	void									SetTrailTextureName(const char *trailTextureName);

	INT32									GetTrailFrameCount() const;
	void									SetTrailFrameCount(INT32 frameCount);

	REAL									GetTrailFrameRate() const;
	void									SetTrailFrameRate(REAL frameRate);

	BOOL									GetDistortFXEnable() const;
	void									SetDistortFXEnable(BOOL enable);

	const char*								GetDistortTextureName() const;
	void									SetDistortTextureName(const char *distortTextureName);

	INT32									GetDistortFrameCount() const;
	void									SetDistortFrameCount(INT32 frameCount);

	REAL									GetDistortFrameRate() const;
	void									SetDistortFrameRate(REAL frameRate);

	void									SetFadeCoefficients(REAL linear, REAL quadratic, REAL cubic);
	void									SetTrailAnchors(const char *anchor0, const char *anchor1);

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_GhostTrail, CRuController_GhostTrail)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
