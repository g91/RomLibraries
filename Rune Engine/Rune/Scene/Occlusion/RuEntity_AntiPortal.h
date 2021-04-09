/*!
	@file RuEntity_AntiPortal.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RUENTITY_ANTIPORTAL_H_
#define _RUENTITY_ANTIPORTAL_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Anti-portal.

	@version 2007.07.06
*/
class CRuEntity_AntiPortal : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuVector3								m_vertices[4];					//!< Portal vertices in local space

	CRuVector3								m_vertices_World[4];
	CRuPlane4								m_portalPlanes[5];

public:
											CRuEntity_AntiPortal();
	virtual									~CRuEntity_AntiPortal();

	// Anti-portal setup and query
	BOOL									SetPortalVertices(const CRuVector3 *vertices, INT32 vertexCount);
	BOOL									IsOccluded(const CRuSphere &sphere);

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	// Event handlers
	BOOL									HandleEvent_Camera(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_AntiPortal, CRuEntity_AntiPortal)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
