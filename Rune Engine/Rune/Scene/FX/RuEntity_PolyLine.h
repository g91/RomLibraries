/*!
	@file RuEntity_PolyLine.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RUENTITY_POLYLINE_H_
#define _RUENTITY_POLYLINE_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityContainer.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Poly line entity.

	@version 2007.07.06
*/
class CRuEntity_PolyLine : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct SegmentDesc
	{
		CRuVector3							m_p0;
		CRuVector3							m_centerAxis;
	};

	INT32									m_maximumSegmentCount;
	REAL									m_segmentLength;
	REAL									m_segmentWidth;
	REAL									m_deviation;
	REAL									m_uvRatio;
	REAL									m_parabolaHeight;
	REAL									m_vSpeed;

	// Runtime stuff
	CRuEntity*								m_targetEntity;
	CRuFrame*								m_dummyFrame;					//!< Dummy frame used for attaching to the target entity

	CRuArrayList<SegmentDesc>				m_segments;
	REAL									m_currentV;

	CRuMTRandom								m_randGen;

	CRuDynamicMesh*							m_mesh;
	CRuMaterial*							m_material;

public:
											CRuEntity_PolyLine();
	virtual									~CRuEntity_PolyLine();

	// Settings
	REAL									GetSegmentLength();
	void									SetSegmentLength(REAL segmentLength);

	REAL									GetSegmentWidth();
	void									SetSegmentWidth(REAL segmentWidth);

	REAL									GetDeviation();
	void									SetDeviation(REAL deviation);

	REAL									GetUVRatio();
	void									SetUVRatio(REAL uvRatio);

	REAL									GetParabolaHeight();
	void									SetParabolaHeight(REAL parabolaHeight);

	REAL									GetVSpeed();
	void									SetVSpeed(REAL vSpeed);

	// Runtime setup
	void									SetTarget(CRuEntity *target);

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
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Camera(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_PolyLine, CRuEntity_PolyLine)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
