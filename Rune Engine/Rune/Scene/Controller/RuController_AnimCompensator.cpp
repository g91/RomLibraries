/*!
	@file RuController_AnimCompensator.cpp

	Copyright (c) 2004-2005 All rights reserved
*/


#include "../../Scene/Controller/RuController_AnimCompensator.h"
#include "../../Helper/RuHelper_Entity.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_AnimCompensator, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(CRuController_AnimCompensator, "CRuController_AnimCompensator", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(CRuController_AnimCompensator, 0x9091F5B2, 0xAD0C4bec, 0xBA52C8A7, 0xDF056367)

// ************************************************************************************************************************************************************

CRuController_AnimCompensator::CRuController_AnimCompensator()
:	m_syncNodeID(0),
	m_syncStrength(0.0f),
	m_hierarchy(NULL),
	m_sourceInterpolator(NULL),
	m_targetInterpolator(NULL)
{
	// Override base parameters
	m_duration = FLT_MAX;
}

CRuController_AnimCompensator::~CRuController_AnimCompensator()
{
	ruSAFE_RELEASE(m_sourceInterpolator);
	ruSAFE_RELEASE(m_targetInterpolator);

	if(m_hierarchy)
	{
		m_hierarchy->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_hierarchy->Event_Update().UnregisterHandler(this->GetGUID());
	}
}

BOOL CRuController_AnimCompensator::Update(REAL elapsedTime)
{
	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

IRuObject *CRuController_AnimCompensator::GetTarget()
{
	return m_hierarchy;
}

BOOL CRuController_AnimCompensator::SetTarget(IRuObject *targetEntity)
{
	if(targetEntity->GetType().IsSubClassOf(CRuFrameHierarchy::Type()))
	{
		// Store pointer to target hierarchy
		m_hierarchy = static_cast<CRuFrameHierarchy *>(targetEntity);

		// Setup event listener
		m_hierarchy->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_AnimCompensator::HandleEvent_Dispose, this, _1));
		m_hierarchy->Event_Update().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_AnimCompensator::HandleEvent_Update, this, _1));
	}

	return TRUE;
}

BOOL CRuController_AnimCompensator::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_hierarchy)
	{
		m_hierarchy->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_hierarchy->Event_Update().UnregisterHandler(this->GetGUID());
	}

	ruSAFE_RELEASE(m_sourceInterpolator);
	ruSAFE_RELEASE(m_targetInterpolator);

	return TRUE;
}

BOOL CRuController_AnimCompensator::HandleEvent_Update(RuEventArgs *eventArgs)
{
	if(m_sourceInterpolator && m_hierarchy)
	{
		CRuMatrix4x4 desiredM, actualM, invActualM, parentM, invParentM;

		RuHierarchy_GetSubNodeParentTransform(m_hierarchy, m_targetInterpolator, m_syncNodeIndex, desiredM);

		RuHierarchy_GetSubNodeParentTransform(m_hierarchy, m_sourceInterpolator, m_syncNodeIndex, actualM);
		invActualM = actualM.GetInverse();

		// Break-down transform into components
		CRuMatrix4x4 expectedM = desiredM * invActualM;
		CRuVector3 scale, shear, rotation, translation;
		expectedM.GetDecomposition(scale, shear, rotation, translation);
		rotation = rotation * m_syncStrength;

		// Apply transform to target interpolator
		CRuQuaternion rotQuat;
		rotQuat.MakeEulerRotation(rotation);
		m_hierarchy->GetInterpolator()->OverrideKeyFrame_Rotation(m_syncNodeIndex, rotQuat);
	}

	return TRUE;
}

BOOL CRuController_AnimCompensator::SetCompensatorSource(CRuAnimationInterpolator *source, CRuAnimationInterpolator *target, INT32 syncNodeID, REAL syncStrength)
{
	if(source)
	{
		m_sourceInterpolator = source;
		m_sourceInterpolator->AddRef();

		m_targetInterpolator = target;
		m_targetInterpolator->AddRef();

		m_syncNodeID = syncNodeID;
		m_syncStrength = syncStrength;

		m_syncNodeIndex = m_hierarchy->GetSubNodeIndex(m_syncNodeID);

		if(m_syncNodeIndex < 0)
		{
			ruSAFE_RELEASE(m_sourceInterpolator);
			ruSAFE_RELEASE(m_targetInterpolator);

			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed
