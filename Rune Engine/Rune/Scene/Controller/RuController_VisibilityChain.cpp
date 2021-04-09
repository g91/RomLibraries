#include "../../Scene/Controller/RuController_VisibilityChain.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_VisibilityChain, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(CRuController_VisibilityChain, "CRuController_VisibilityChain", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(CRuController_VisibilityChain, 0x431FF266, 0xE86A4b3d, 0xAD3A600B, 0x480D46F1)

// ************************************************************************************************************************************************************

CRuController_VisibilityChain::CRuController_VisibilityChain()
:	m_targetEntity(NULL),
	m_chainSource(NULL)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_VisibilityChain::HandleEvent_Dispose, this, _1), NULL);
}

CRuController_VisibilityChain::~CRuController_VisibilityChain()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

IRuObject *CRuController_VisibilityChain::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_VisibilityChain();
	CRuController_VisibilityChain *controller = static_cast<CRuController_VisibilityChain *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_Controller::Clone(clonedObject);

	// Clone properties

	return clonedObject;
}

BOOL CRuController_VisibilityChain::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Controller::SerializeFrom(stream);

	switch(version)
	{
		case 1:
			{
				// Read properties
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_VisibilityChain::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Controller::SerializeTo(stream);

	// Write properties

	return TRUE;
}

BOOL CRuController_VisibilityChain::Update(REAL elapsedTime)
{
	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

IRuObject *CRuController_VisibilityChain::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_VisibilityChain::SetTarget(IRuObject *target)
{
	if(target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		m_targetEntity = static_cast<CRuEntity *>(target);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_VisibilityChain::HandleEvent_Dispose, this, _1), NULL);

		return TRUE;
	}

	return FALSE;
}

CRuEntity *CRuController_VisibilityChain::GetChainSource()
{
	return m_chainSource;
}

BOOL CRuController_VisibilityChain::SetChainSource(CRuEntity *chainSource)
{
	if(chainSource)
	{
		// If we had a previous chain source, unregister it
		if(m_chainSource)
		{
			m_chainSource->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		m_chainSource = chainSource;
		m_chainSource->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_VisibilityChain::HandleEvent_Dispose, this, _1), NULL);
	}

	return TRUE;
}

BOOL CRuController_VisibilityChain::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	if(m_chainSource)
	{
		m_chainSource->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_chainSource = NULL;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
