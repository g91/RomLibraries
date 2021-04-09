#ifndef _RUCONTROLLER_DECAL_H_
#define _RUCONTROLLER_DECAL_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!

	@author John Tang
	@version 2006.03.27
*/
class CRuController_Decal : public IRuEntity_KeyableController
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity*								m_targetEntity;									//!< Entity that the controller affects

	// Controller properties
	RuBillboardType							m_uvBillboardType;

	// Controller tracks
	CRuAnimTrack<char *>					m_track_DecalDiffuseTexture;
	CRuAnimTrack<REAL>						m_track_DecalRotation;
	CRuAnimTrack<REAL>						m_track_DecalScale;
//	CRuAnimTrack<CRuColor4>					m_track_Color;									//!< Color track

public:
											CRuController_Decal();
	virtual									~CRuController_Decal();

	// Properties
	RuBillboardType							GetBillboardType();
	void									SetBillboardType(RuBillboardType billboardType);

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	// IRuEntity_KeyableController implementation
	virtual INT32							GetNumTracks() const;
	virtual BOOL							GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_Decal, CRuController_Decal)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
