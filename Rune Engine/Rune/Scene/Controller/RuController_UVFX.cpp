#include "../../Scene/Controller/RuController_UVFX.h"

#include "../../Helper/RuHelper_IRuEntity_Renderable.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_UVFX, IRuEntity_KeyableController)
ruHEAP_DEFINE_SUBCLASS(CRuController_UVFX, "CRuController_UVFX", "IRuEntity_KeyableController")
ruCLASSGUID_DEFINE(CRuController_UVFX, 0x2C8D5FCC, 0x94DD4563, 0x888E35DA, 0x50635079)

// ************************************************************************************************************************************************************

CRuController_UVFX::CRuController_UVFX()
:	m_targetEntity(NULL)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_UVFX::HandleEvent_Dispose, this, _1), NULL);

	// Reset UV transform to identity
	m_uvTransform.SetToIdentity();

	// Setup tracks
	m_track_U.SetTrackName("U");
	m_track_V.SetTrackName("V");
}

CRuController_UVFX::~CRuController_UVFX()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

IRuObject *CRuController_UVFX::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_UVFX();
	CRuController_UVFX *controller = static_cast<CRuController_UVFX *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_KeyableController::Clone(clonedObject);

	// Clone properties

	// Clone tracks
	CloneTracks(controller);

	return clonedObject;
}

BOOL CRuController_UVFX::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	CRuGUID classGUID;

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeFrom(stream);

	switch(version)
	{
		case 1:
			{
				// Read properties

				// Serialize tracks
				m_track_U.SerializeFrom(stream);
				m_track_V.SerializeFrom(stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_UVFX::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeTo(stream);

	// Write properties

	// Serialize tracks
	m_track_U.SerializeTo(stream);
	m_track_V.SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_UVFX::Update(REAL elapsedTime)
{
	if(m_targetEntity && m_enabled)
	{
		REAL relativeT = m_age - m_startTime;

		if(relativeT < 0.0f)
		{
		}
		else
		{
			// If looping is enabled, clamp time range to the duration
			if(relativeT > m_duration && m_duration > 0.0f && m_loopTime >= 0.0f)
			{
				relativeT = fmod(relativeT, m_duration);
			}

			// Get current u and v delta
			REAL uDelta = m_track_U.GetInterpolatedKey(relativeT);
			REAL vDelta = m_track_V.GetInterpolatedKey(relativeT);

			// Update transform
			m_uvTransform.Translate(CRuVector3(uDelta * elapsedTime, vDelta * elapsedTime, 0.0f));

			m_uvTransform._41 = fmod(m_uvTransform._41, 1.0f);
			m_uvTransform._42 = fmod(m_uvTransform._42, 1.0f);
			m_uvTransform._43 = fmod(m_uvTransform._43, 1.0f);

			// Clear all material UV transforms
			for(INT32 i = 0; i < m_materialList.Count(); ++i)
			{
				// Clear UV transform setting
				m_materialList[i]->GetParameterBlock()->ClearParameter(ruPARAMTYPE_TEXTURE_TRANSFORM_0);

				// Unregister dispose handler
				m_materialList[i]->Event_Dispose().UnregisterHandler(this->GetGUID());
			}

			m_materialList.Clear();

			// Enumerate all materials in the target
			RuEntity_EnumerateMaterials(m_targetEntity, m_materialList);

			// Iterate through all target materials and set texture transforms
			for(INT32 i = 0; i < m_materialList.Count(); ++i)
			{
				// Set transform
				m_materialList[i]->GetParameterBlock()->SetParameter(ruPARAMTYPE_TEXTURE_TRANSFORM_0, reinterpret_cast<UINT64>(&m_uvTransform));

				// Set dispose callback for the material
				m_materialList[i]->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_UVFX::HandleEvent_MaterialDispose, this, _1), NULL);
			}
		}
	}

	// Chain update call to base controller class
	return IRuEntity_KeyableController::Update(elapsedTime);
}

IRuObject *CRuController_UVFX::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_UVFX::SetTarget(IRuObject *target)
{
	if(target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		m_targetEntity = static_cast<CRuEntity *>(target);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_UVFX::HandleEvent_Dispose, this, _1), NULL);

		return TRUE;
	}

	return FALSE;
}

INT32 CRuController_UVFX::GetNumTracks() const
{
	return 2;
}

BOOL CRuController_UVFX::GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut)
{
	switch(trackIndex)
	{
		case 0:
			trackOut = &m_track_U;
			return TRUE;

		case 1:
			trackOut = &m_track_V;
			return TRUE;
	}

	return FALSE;
}

BOOL CRuController_UVFX::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		// Clear all material UV transforms
		for(INT32 i = 0; i < m_materialList.Count(); ++i)
		{
			// Clear UV transform setting
			m_materialList[i]->GetParameterBlock()->ClearParameter(ruPARAMTYPE_TEXTURE_TRANSFORM_0);

			// Unregister dispose handler
			m_materialList[i]->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		m_materialList.Clear();

		// Unregister listener from target entity
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	return TRUE;
}

BOOL CRuController_UVFX::HandleEvent_MaterialDispose(RuEventArgs *eventArgs)
{
	RuObjectEvent_Dispose_Args *disposeArgs = static_cast<RuObjectEvent_Dispose_Args *>(eventArgs);

	if(disposeArgs->m_object)
	{
		IRuMaterial *disposedMaterial = static_cast<IRuMaterial *>(disposeArgs->m_object);

		for(INT32 i = 0; i < m_materialList.Count(); ++i)
		{
			if(m_materialList[i] == disposedMaterial)
			{
				m_materialList.RemoveAt(i);
			}
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
