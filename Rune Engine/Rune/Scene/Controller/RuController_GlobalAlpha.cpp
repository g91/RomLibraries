#include "../../Scene/Controller/RuController_GlobalAlpha.h"

#include "../../Helper/RuHelper_IRuEntity_Renderable.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_GlobalAlpha, IRuEntity_KeyableController)
ruHEAP_DEFINE_SUBCLASS(CRuController_GlobalAlpha, "CRuController_GlobalAlpha", "IRuEntity_KeyableController")
ruCLASSGUID_DEFINE(CRuController_GlobalAlpha, 0x7415933A, 0x7BA742dd, 0xA8DC52D8, 0x4C4DD02F)

// ************************************************************************************************************************************************************

CRuController_GlobalAlpha::CRuController_GlobalAlpha()
:	m_targetEntity(NULL),
	m_dataFormatIsOld(FALSE),
	m_currentAlphaMode(ruALPHAMODE_NORMAL),
	m_currentAlpha(1.0f),
	m_currentAlphaTestEnable(TRUE),
	m_currentAlphaTestValue(128),
	m_currentAlphaTestMode(ruALPHATESTMODE_GREATER)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_GlobalAlpha::HandleEvent_Dispose, this, _1), NULL);

	// Insert default keys
	m_track_AlphaMode.InsertKey(0.0f, m_currentAlphaMode);
	m_track_AlphaMode.SetTrackName("Alpha Mode");

	m_track_Alpha.InsertKey(0.0f, m_currentAlpha);
	m_track_Alpha.SetTrackName("Alpha");

	m_track_AlphaTestEnable.InsertKey(0.0f, m_currentAlphaTestEnable);
	m_track_AlphaTestEnable.SetTrackName("Alpha Test Enable");

	m_track_AlphaTestValue.InsertKey(0.0f, m_currentAlphaTestValue);
	m_track_AlphaTestValue.SetTrackName("Alpha Test Value");

	m_track_AlphaTestMode.InsertKey(0.0f, m_currentAlphaTestMode);
	m_track_AlphaTestMode.SetTrackName("Alpha Test Mode");
}

CRuController_GlobalAlpha::~CRuController_GlobalAlpha()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

IRuObject *CRuController_GlobalAlpha::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_GlobalAlpha();
	CRuController_GlobalAlpha *controller = static_cast<CRuController_GlobalAlpha *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_KeyableController::Clone(clonedObject);

	controller->m_dataFormatIsOld = m_dataFormatIsOld;

	// Clone tracks
	CloneTracks(controller);

	return clonedObject;
}

BOOL CRuController_GlobalAlpha::SerializeFrom(IRuStream *stream)
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
				m_dataFormatIsOld = TRUE;

				// Serialize tracks
				m_track_AlphaMode.SerializeFrom(stream);
				m_track_Alpha.SerializeFrom(stream);
			}

			break;

		case 2:
			{
				// Serialize tracks
				m_track_AlphaMode.SerializeFrom(stream);
				m_track_Alpha.SerializeFrom(stream);
				m_track_AlphaTestEnable.SerializeFrom(stream);
				m_track_AlphaTestValue.SerializeFrom(stream);
				m_track_AlphaTestMode.SerializeFrom(stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_GlobalAlpha::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeTo(stream);

	// Serialize tracks
	m_track_AlphaMode.SerializeTo(stream);
	m_track_Alpha.SerializeTo(stream);
	m_track_AlphaTestEnable.SerializeTo(stream);
	m_track_AlphaTestValue.SerializeTo(stream);
	m_track_AlphaTestMode.SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_GlobalAlpha::Update(REAL elapsedTime)
{
	// Chain update call to base controller class
	IRuEntity_KeyableController::Update(elapsedTime);

	if(m_targetEntity && m_enabled)
	{
		REAL relativeT = m_age - m_startTime;

		if(relativeT < 0.0f)
		{
		}
		else
		{
			// If looping is enabled, clamp time range to the duration
			if(relativeT > m_duration)
			{
				if(m_loopTime >= 0.0f && m_duration > 0.0f)
				{
					relativeT = fmod(relativeT, m_duration);
				}
				else
				{
					relativeT = m_duration;
				}
			}

			// Get current track values
			m_currentAlphaMode = m_track_AlphaMode.GetDiscreteKey(relativeT);
			m_currentAlpha = m_track_Alpha.GetInterpolatedKey(relativeT);
			m_currentAlphaTestEnable = m_track_AlphaTestEnable.GetDiscreteKey(relativeT);
			m_currentAlphaTestValue = m_track_AlphaTestValue.GetInterpolatedKey(relativeT);
			m_currentAlphaTestMode = m_track_AlphaTestMode.GetDiscreteKey(relativeT);

			{
				// Update the target entity's shader parameter block
				CRuParameterBlock *parameterBlock = m_targetEntity->GetParameterBlock();
				parameterBlock->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(m_currentAlpha));
				parameterBlock->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, static_cast<UINT64>(m_currentAlphaMode));

				if(m_currentAlphaTestEnable)
				{
					parameterBlock->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, m_currentAlphaTestValue);
					parameterBlock->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, m_currentAlphaTestMode);
				}
				else
				{
					parameterBlock->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, m_currentAlphaTestValue);
					parameterBlock->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_NONE);
				}

				// Override alpha test setting if applicable
				if(m_dataFormatIsOld)
				{
					if(m_currentAlpha < 1.0f)
					{
						parameterBlock->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_NONE);
					}
					else
					{
						parameterBlock->ClearParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE);
					}
				}
			}
		}
	}

	return TRUE;
}

REAL CRuController_GlobalAlpha::SetDuration(REAL duration)
{
	// Store old duration and chain call to parent
	REAL oldDuration = m_duration;
	REAL newDuration = IRuEntity_Controller::SetDuration(duration);

	// Calculate the duration scale
	REAL durationScale = (oldDuration > 0.0f) ? newDuration / oldDuration : 1.0f;

	// Adjust all tracks to scale to the new duration
	m_track_AlphaMode.ScaleKeys(durationScale);
	m_track_Alpha.ScaleKeys(durationScale);
	m_track_AlphaTestEnable.ScaleKeys(durationScale);
	m_track_AlphaTestValue.ScaleKeys(durationScale);
	m_track_AlphaTestMode.ScaleKeys(durationScale);

	return newDuration;
}

IRuObject *CRuController_GlobalAlpha::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_GlobalAlpha::SetTarget(IRuObject *targetEntity)
{
	if(targetEntity->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		// Store target reference, setup event listener, and setup target parameter block
		m_targetEntity = static_cast<CRuEntity *>(targetEntity);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_GlobalAlpha::HandleEvent_Dispose, this, _1), NULL);

		return TRUE;
	}

	return FALSE;
}

INT32 CRuController_GlobalAlpha::GetNumTracks() const
{
	return 5;
}

BOOL CRuController_GlobalAlpha::GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut)
{
	switch(trackIndex)
	{
		case 0:
			trackOut = &m_track_AlphaMode;
			return TRUE;

		case 1:
			trackOut = &m_track_Alpha;
			return TRUE;

		case 2:
			trackOut = &m_track_AlphaTestEnable;
			return TRUE;

		case 3:
			trackOut = &m_track_AlphaTestValue;
			return TRUE;

		case 4:
			trackOut = &m_track_AlphaTestMode;
			return TRUE;
	}

	return FALSE;
}

BOOL CRuController_GlobalAlpha::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		// Unregister from target entity
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	return TRUE;
}

void CRuController_GlobalAlpha::SetAlphaMode(RuAlphaMode alphaMode)
{
	if(m_track_AlphaMode.GetNumKeys() < 2)
	{
		m_track_AlphaMode.InsertKey(0.0f, m_currentAlphaMode);
	}

	m_track_AlphaMode.SetKeyByIndex(0, alphaMode);
	m_track_AlphaMode.SetKeyByIndex(1, alphaMode);
}

void CRuController_GlobalAlpha::SetStartAlpha(REAL alpha)
{
	m_track_Alpha.SetKeyByIndex(0, alpha);
}

void CRuController_GlobalAlpha::SetEndAlpha(REAL alpha)
{
	if(m_track_Alpha.GetNumKeys() < 2)
	{
		m_track_Alpha.InsertKey(0.0f, m_currentAlpha);
	}

	m_track_Alpha.SetKeyByIndex(1, alpha);
	m_track_Alpha.MoveKeyByIndex(1, m_duration);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
