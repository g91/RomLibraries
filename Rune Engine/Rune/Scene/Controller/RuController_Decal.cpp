#include "../../Scene/Controller/RuController_Decal.h"
#include "../../Scene/FX/RuEntity_Decal.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Decal, IRuEntity_KeyableController)
ruHEAP_DEFINE_SUBCLASS(CRuController_Decal, "CRuController_Decal", "IRuEntity_KeyableController")
ruCLASSGUID_DEFINE(CRuController_Decal, 0x36FC66F6, 0x676D4bc7, 0x9AD99272, 0xEF02B2F4)

// ************************************************************************************************************************************************************

CRuController_Decal::CRuController_Decal()
:	m_targetEntity(NULL),
	m_uvBillboardType(ruBILLBOARDTYPE_NONE)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Decal::HandleEvent_Dispose, this, _1), NULL);

	m_track_DecalDiffuseTexture.SetTrackName("Diffuse Texture");

	m_track_DecalRotation.SetTrackName("Rotation");

	m_track_DecalScale.SetTrackName("Scale");
}

CRuController_Decal::~CRuController_Decal()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

RuBillboardType CRuController_Decal::GetBillboardType()
{
	return m_uvBillboardType;
}

void CRuController_Decal::SetBillboardType(RuBillboardType billboardType)
{
	m_uvBillboardType = billboardType;
}

IRuObject *CRuController_Decal::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_Decal();
	CRuController_Decal *controller = static_cast<CRuController_Decal *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_KeyableController::Clone(clonedObject);

	controller->m_uvBillboardType = m_uvBillboardType;

	// Clone tracks
	CloneTracks(controller);

	return clonedObject;
}

BOOL CRuController_Decal::SerializeFrom(IRuStream *stream)
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
				stream->Read(&m_uvBillboardType, sizeof(RuBillboardType));

				// Serialize tracks
				m_track_DecalDiffuseTexture.SerializeFrom(stream);
				m_track_DecalRotation.SerializeFrom(stream);
				m_track_DecalScale.SerializeFrom(stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_Decal::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeTo(stream);

	stream->Write(&m_uvBillboardType, sizeof(RuBillboardType));

	// Serialize tracks
	m_track_DecalDiffuseTexture.SerializeTo(stream);
	m_track_DecalRotation.SerializeTo(stream);
	m_track_DecalScale.SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_Decal::Update(REAL elapsedTime)
{
	if(m_targetEntity && m_enabled)
	{
		REAL relativeT = m_age - m_startTime;

		CRuVector3 basisVectors[3] = { CRuVector3(-1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f) };

		if(relativeT < 0.0f)
		{
			static_cast<CRuEntity_Decal *>(m_targetEntity)->SetDecalRadius(0.0f);
		}
		else
		{
			// If looping is enabled, clamp time range to the duration
			if(relativeT > m_duration && m_duration > 0.0f && m_loopTime >= 0.0f)
			{
				while(relativeT > m_duration)
				{
					relativeT = fmod(relativeT, m_duration);
				}
			}

			CRuQuaternion quat;

			quat.FromAngleAxis(CRuVector3(0.0f, 1.0f, 0.0f), m_track_DecalRotation.GetInterpolatedKey(relativeT));

			static_cast<CRuEntity_Decal *>(m_targetEntity)->SetBillboardType(m_uvBillboardType);
			static_cast<CRuEntity_Decal *>(m_targetEntity)->SetDecalRadius(m_track_DecalScale.GetInterpolatedKey(relativeT));
			static_cast<CRuEntity_Decal *>(m_targetEntity)->SetDecalTransform(quat.GetMatrix());
			static_cast<CRuEntity_Decal *>(m_targetEntity)->SetDecalUVBasis(basisVectors, 0.5f, 0.5f);

			char *textureName = m_track_DecalDiffuseTexture.GetDiscreteKey(relativeT);
			IRuTexture *texture = g_ruResourceManager->LoadTexture(textureName, ruSURFACEFMT_DEFAULT, 0, TRUE);
			if(texture)
			{
				static_cast<CRuEntity_Decal *>(m_targetEntity)->SetDecalTexture(texture);
				ruSAFE_RELEASE(texture);
			}
		}
	}

	// Chain update call to base controller class
	return IRuEntity_KeyableController::Update(elapsedTime);
}

IRuObject *CRuController_Decal::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_Decal::SetTarget(IRuObject *target)
{
	if(target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		// If we had a previous target, unregister our event handler
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		m_targetEntity = static_cast<CRuEntity *>(target);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Decal::HandleEvent_Dispose, this, _1), NULL);

		return TRUE;
	}

	return FALSE;
}

INT32 CRuController_Decal::GetNumTracks() const
{
	return 3;
}

BOOL CRuController_Decal::GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut)
{
	switch(trackIndex)
	{
		case 0:
			trackOut = &m_track_DecalDiffuseTexture;
			return TRUE;

		case 1:
			trackOut = &m_track_DecalRotation;
			return TRUE;

		case 2:
			trackOut = &m_track_DecalScale;
			return TRUE;
	}

	return FALSE;
}

BOOL CRuController_Decal::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
