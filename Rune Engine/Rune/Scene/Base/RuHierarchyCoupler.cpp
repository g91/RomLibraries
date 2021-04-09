#include "../../Scene/Base/RuHierarchyCoupler.h"

#include "../../Helper/RuHelper_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuHierarchyCoupler, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuHierarchyCoupler, "CRuHierarchyCoupler", "IRuObject")
ruCLASSGUID_DEFINE(CRuHierarchyCoupler, 0x460AA187, 0x4FA248a9, 0xB40FFBED, 0x8F5DE9FE)

// ************************************************************************************************************************************************************

CRuHierarchyCoupler::CRuHierarchyCoupler()
:	m_targetEntity(NULL),
	m_coupledHierarchy(NULL),
	m_boneIndex(-1)
{
	// Register dispose event on self
	this->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuHierarchyCoupler::DisposeHandler, this, _1));
}

CRuHierarchyCoupler::~CRuHierarchyCoupler()
{
	// All destruction is handled by the disposal handler
}

BOOL CRuHierarchyCoupler::EstablishCoupling(CRuEntity *targetEntity, CRuEntity *coupledHierarchy, INT32 boneIdx)
{
	// Store reference to targets
	m_targetEntity = targetEntity;
	m_coupledHierarchy = coupledHierarchy;
	m_boneIndex = boneIdx;

	m_refTransform = m_targetEntity->GetLocalTransform();

	if(coupledHierarchy->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
	{
		// Register bone updated handler with the hierarchy
		static_cast<CRuHierarchy_GR2 *>(coupledHierarchy)->Event_BonesUpdated().RegisterHandler(this->GetGUID(), boost::bind(&CRuHierarchyCoupler::BonesUpdatedHandler, this, _1));
	}

	// Register disposal handlers
	targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuHierarchyCoupler::DisposeHandler, this, _1));
	coupledHierarchy->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuHierarchyCoupler::DisposeHandler, this, _1));

	return TRUE;
}

BOOL CRuHierarchyCoupler::BonesUpdatedHandler(RuEventArgs *eventArgs)
{
	if(m_coupledHierarchy->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
	{
		CRuHierarchy_GR2 *hierarchy = static_cast<CRuHierarchy_GR2 *>(m_coupledHierarchy);

		INT32 boneCount = hierarchy->GetNumSubNodes();
		CRuMatrix4x4 *boneTransforms = hierarchy->GetSubNodeTransforms(ruFRAMETYPE_COMPOSITE);

		if(m_boneIndex >= 0 && m_boneIndex < boneCount)
		{
			// NOTE: Target entity's ref transform is presumed to be in skin pose, so here we simply multiply by the composite skin transform
			m_targetEntity->ResetAllTransforms();
			m_targetEntity->Transform(m_refTransform * boneTransforms[m_boneIndex]);
		}
	}

	return TRUE;
}

BOOL CRuHierarchyCoupler::DisposeHandler(RuEventArgs *eventArgs)
{
	// Unregister handlers
	this->Event_Dispose().UnregisterHandler(this->GetGUID());
	m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
	m_coupledHierarchy->Event_Dispose().UnregisterHandler(this->GetGUID());

	if(static_cast<CRuHierarchy_GR2 *>(m_coupledHierarchy)->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
	{
		static_cast<CRuHierarchy_GR2 *>(m_coupledHierarchy)->Event_BonesUpdated().UnregisterHandler(this->GetGUID());
	}

	// Terminate coupling
	m_targetEntity = NULL;
	m_coupledHierarchy = NULL;

	// Dispose self
	this->Release();

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
