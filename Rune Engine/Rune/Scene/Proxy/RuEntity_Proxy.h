/*!
	@file RuEntity_Proxy.h

	Copyright (c) 2004-2008 All rights reserved
*/

#ifndef _RUENTITY_PROXY_H_
#define _RUENTITY_PROXY_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Paperdoll/RuEntityPaperdoll.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Entity proxy.

	@version 2008.01.10
*/
class CRuEntity_Proxy : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Proxied entity
	CRuEntity*								m_proxiedEntity;

	// Event system data
	CRuEvent								m_event_LoadComplete;									//!< Entity load has been completed

public:
											CRuEntity_Proxy();
	virtual									~CRuEntity_Proxy();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	// Proxy interface
	BOOL									LoadEntity(const char *resourceName);

	// Event system
	CRuEvent&								Event_LoadComplete();

protected:
	void									ExitLoadPhase();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_Proxy, CRuEntity_Proxy)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
