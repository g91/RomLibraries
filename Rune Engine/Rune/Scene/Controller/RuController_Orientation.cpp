/*!
	@file RuController_Orientation.cpp

	Copyright (c) 2004-2007 All rights reserved
*/

#include "../../Scene/Controller/RuController_Orientation.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Orientation, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(CRuController_Orientation, "CRuController_Orientation", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(CRuController_Orientation, 0xA353D662, 0xDE754a82, 0xB193134D, 0xCE6678C6)

// ************************************************************************************************************************************************************

CRuController_Orientation::CRuController_Orientation()
:	m_targetEntity(NULL),
	m_referenceEntity(NULL)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Orientation::HandleEvent_Dispose, this, _1), NULL);
}

CRuController_Orientation::~CRuController_Orientation()
{
	// NOTE: Destruction of resources is done during the ruENTITYEVENT_ONDESTROY event.
}

IRuObject *CRuController_Orientation::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_Orientation();
	CRuController_Orientation *controller = static_cast<CRuController_Orientation *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_Controller::Clone(clonedObject);

	// not done

	return clonedObject;
}

BOOL CRuController_Orientation::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
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
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_Orientation::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Controller::SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_Orientation::Update(REAL elapsedTime)
{
	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

IRuObject *CRuController_Orientation::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_Orientation::SetTarget(IRuObject *target)
{
	if(target && target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		// Store new target and register as a listener on it
		m_targetEntity = static_cast<CRuEntity *>(target);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Orientation::HandleEvent_Dispose, this, _1), NULL);

		return TRUE;
	}

	return FALSE;
}

CRuEntity *CRuController_Orientation::GetReferenceEntity()
{
	return m_referenceEntity;
}

BOOL CRuController_Orientation::SetReferenceEntity(CRuEntity *referenceEntity)
{
	if(referenceEntity)
	{
		// If we had a previous reference entity, unregister it
		if(m_referenceEntity)
		{
			m_referenceEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		// Store new reference entity and register as a listener on it
		m_referenceEntity = referenceEntity;
		m_referenceEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Orientation::HandleEvent_Dispose, this, _1), NULL);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuController_Orientation::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	if(m_referenceEntity)
	{
		m_referenceEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_referenceEntity = NULL;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
