#include "../../Scene/Controller/RuController_GlobalColor.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_GlobalColor, IRuEntity_KeyableController)
ruHEAP_DEFINE_SUBCLASS(CRuController_GlobalColor, "CRuController_GlobalColor", "IRuEntity_KeyableController")
ruCLASSGUID_DEFINE(CRuController_GlobalColor, 0x085B959A, 0x84404038, 0xA44B4907, 0x04477E8A)

// ************************************************************************************************************************************************************

CRuController_GlobalColor::CRuController_GlobalColor()
:	m_targetEntity(NULL)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_GlobalColor::HandleEvent_Dispose, this, _1), NULL);

	// Insert two dummy keys for start & end color
	m_track_Color.InsertKey(0.0f, CRuColor4(1.0f, 1.0f, 1.0f, 1.0f));
	m_track_Color.InsertKey(0.0f, CRuColor4(1.0f, 1.0f, 1.0f, 1.0f));
	m_track_Color.SetTrackName("Color");
}

CRuController_GlobalColor::~CRuController_GlobalColor()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

IRuObject *CRuController_GlobalColor::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_GlobalColor();
	CRuController_GlobalColor *controller = static_cast<CRuController_GlobalColor *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_KeyableController::Clone(clonedObject);

	// Clone tracks
	CloneTracks(controller);

	return clonedObject;
}

BOOL CRuController_GlobalColor::SerializeFrom(IRuStream *stream)
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
	IRuEntity_KeyableController::SerializeFrom(stream);

	switch(version)
	{
		case 1:
			{
				// Serialize tracks
				m_track_Color.SerializeFrom(stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_GlobalColor::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeTo(stream);

	// Serialize tracks
	m_track_Color.SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_GlobalColor::Update(REAL elapsedTime)
{
	if(m_targetEntity && m_enabled)
	{
		REAL relativeT = m_age - m_startTime;

		if(relativeT < 0.0f)
		{
			CRuParameterBlock *parameterBlock = m_targetEntity->GetParameterBlock();
			parameterBlock->ClearParameter(ruPARAMTYPE_MATERIAL_DIFFUSE);
		}
		else
		{
			// If looping is enabled, clamp time range to the duration
			if(relativeT > m_duration && m_duration > 0.0f && m_loopTime >= 0.0f)
			{
				relativeT = fmod(relativeT, m_duration);
			}

			m_currentColor = m_track_Color.GetInterpolatedKey(relativeT);

			// Update the internal shader parameter block
			CRuParameterBlock *parameterBlock = m_targetEntity->GetParameterBlock();
			parameterBlock->SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, m_currentColor.GetARGB());
		}
	}

	// Chain update call to base controller class
	return IRuEntity_KeyableController::Update(elapsedTime);
}

IRuObject *CRuController_GlobalColor::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_GlobalColor::SetTarget(IRuObject *target)
{
	if(target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		m_targetEntity = static_cast<CRuEntity *>(target);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_GlobalColor::HandleEvent_Dispose, this, _1), NULL);

		return TRUE;
	}

	return FALSE;
}

INT32 CRuController_GlobalColor::GetNumTracks() const
{
	return 1;
}

BOOL CRuController_GlobalColor::GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut)
{
	switch(trackIndex)
	{
		case 0:
			trackOut = &m_track_Color;
			return TRUE;
	}

	return FALSE;
}

BOOL CRuController_GlobalColor::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	return TRUE;
}

void CRuController_GlobalColor::SetStartColor(const CRuColor4 &color)
{
	m_track_Color.SetKeyByIndex(0, color);
}

void CRuController_GlobalColor::SetEndColor(const CRuColor4 &color)
{
	m_track_Color.SetKeyByIndex(1, color);
	m_track_Color.MoveKeyByIndex(1, m_duration);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
