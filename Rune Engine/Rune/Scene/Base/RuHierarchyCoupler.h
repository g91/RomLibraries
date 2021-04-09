#pragma once

#include "../../Scene/Base/RuHierarchy_GR2.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuHierarchyCoupler : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity*								m_targetEntity;
	CRuEntity*								m_coupledHierarchy;
	INT32									m_boneIndex;
	CRuMatrix4x4							m_refTransform;

public:
											CRuHierarchyCoupler();
	virtual									~CRuHierarchyCoupler();

	BOOL									EstablishCoupling(CRuEntity *targetEntity, CRuEntity *coupledHierarchy, INT32 boneIdx);

protected:
	BOOL									BonesUpdatedHandler(RuEventArgs *eventArgs);
	BOOL									DisposeHandler(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
