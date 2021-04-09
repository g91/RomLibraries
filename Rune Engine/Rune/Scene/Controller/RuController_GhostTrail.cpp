#include "../../Scene/Controller/RuController_GhostTrail.h"

#include "../../Helper/RuHelper_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_GhostTrail, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(CRuController_GhostTrail, "CRuController_GhostTrail", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(CRuController_GhostTrail, 0xDAFCF084, 0xF0324ea6, 0x8A1AB291, 0xC3480CC2)

// ************************************************************************************************************************************************************

CRuController_GhostTrail::CRuController_GhostTrail()
:	m_targetEntity(NULL),
	m_frameType(ruFRAMETYPE_LOCAL),
	m_tInterval(0.033f),
	m_segmentInterval(0.016f),
	m_maxAge(1.0f),
	m_maxControlPoints(32),
	m_maxTrailSegments(512),
	m_activeDuration(1.0f),
	m_fadeOutDuration(0.0f),
	m_anchor0(NULL),
	m_anchor1(NULL),
	m_trailTextureName(NULL),
	m_trailTextureFrameCount(1),
	m_trailTextureFrameRate(30.0f),
	m_distortFXEnable(FALSE),
	m_distortTextureName(NULL),
	m_distortTextureFrameCount(1),
	m_distortTextureFrameRate(30.0f),
	m_attached(FALSE),
	m_active(FALSE),
	m_ghostTrail(NULL),
	m_distortFXMatController(NULL)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_GhostTrail::HandleEvent_Dispose, this, _1), NULL);
}

CRuController_GhostTrail::~CRuController_GhostTrail()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

RuFrameType CRuController_GhostTrail::GetFrameType() const
{
	return m_frameType;
}

void CRuController_GhostTrail::SetFrameType(RuFrameType frameType)
{
	m_frameType = frameType;
}

REAL CRuController_GhostTrail::GetInterval() const
{
	return m_tInterval;
}

void CRuController_GhostTrail::SetInterval(REAL interval)
{
	m_tInterval = interval;
}

REAL CRuController_GhostTrail::GetSegmentInterval() const
{
	return m_segmentInterval;
}

void CRuController_GhostTrail::SetSegmentInterval(REAL interval)
{
	m_segmentInterval = interval;
}

REAL CRuController_GhostTrail::GetMaxAge() const
{
	return m_maxAge;
}

void CRuController_GhostTrail::SetMaxAge(REAL maxAge)
{
	m_maxAge = maxAge;
}

REAL CRuController_GhostTrail::GetFadeCoefficient(INT32 coefficientIndex) const
{
	return m_fadeCoefficients[coefficientIndex];
}

void CRuController_GhostTrail::SetFadeCoefficient(INT32 coefficientIndex, REAL coefficient)
{
	m_fadeCoefficients[coefficientIndex] = coefficient;
}

INT32 CRuController_GhostTrail::GetMaxControlPoints() const
{
	return m_maxControlPoints;
}

void CRuController_GhostTrail::SetMaxControlPoints(INT32 maxControlPoints)
{
	m_maxControlPoints = maxControlPoints;
}

INT32 CRuController_GhostTrail::GetMaxSegments() const
{
	return m_maxTrailSegments;
}

void CRuController_GhostTrail::SetMaxSegments(INT32 maxSegments)
{
	m_maxTrailSegments = maxSegments;
}

REAL CRuController_GhostTrail::GetActiveDuration() const
{
	return m_activeDuration;
}

void CRuController_GhostTrail::SetActiveDuration(REAL duration)
{
	m_activeDuration = duration;
}

REAL CRuController_GhostTrail::GetFadeOutDuration() const
{
	return m_fadeOutDuration;
}

void CRuController_GhostTrail::SetFadeOutDuration(REAL duration)
{
	m_fadeOutDuration = duration;
}

const char *CRuController_GhostTrail::GetTrailAnchor(INT32 anchorIndex) const
{
	switch(anchorIndex)
	{
		case 0:	return m_anchor0;
		case 1: return m_anchor1;
	}

	return "";
}

void CRuController_GhostTrail::SetTrailAnchor(INT32 anchorIndex, const char *anchor)
{
	switch(anchorIndex)
	{
		case 0:
			{
				delete[] m_anchor0;

				if(anchor)
				{
					m_anchor0 = ruNEW char [strlen(anchor) + 1];
					memcpy(m_anchor0, anchor, strlen(anchor) + 1);
				}
				else
				{
					m_anchor0 = NULL;
				}
			}

			break;

		case 1:
			{
				delete[] m_anchor1;

				if(anchor)
				{
					m_anchor1 = ruNEW char [strlen(anchor) + 1];
					memcpy(m_anchor1, anchor, strlen(anchor) + 1);
				}
				else
				{
					m_anchor1 = NULL;
				}
			}

			break;
	}
}

const char *CRuController_GhostTrail::GetTrailTextureName() const
{
	if(m_trailTextureName)
	{
		return m_trailTextureName;
	}

	return "";
}

void CRuController_GhostTrail::SetTrailTextureName(const char *trailTextureName)
{
	delete[] m_trailTextureName;

	if(trailTextureName)
	{
		m_trailTextureName = ruNEW char [strlen(trailTextureName) + 1];
		memcpy(m_trailTextureName, trailTextureName, strlen(trailTextureName) + 1);
	}
	else
	{
		m_trailTextureName = NULL;
	}
}

INT32 CRuController_GhostTrail::GetTrailFrameCount() const
{
	return m_trailTextureFrameCount;
}

void CRuController_GhostTrail::SetTrailFrameCount(INT32 frameCount)
{
	m_trailTextureFrameCount = frameCount;
}

REAL CRuController_GhostTrail::GetTrailFrameRate() const
{
	return m_trailTextureFrameRate;
}

void CRuController_GhostTrail::SetTrailFrameRate(REAL frameRate)
{
	m_trailTextureFrameRate = frameRate;
}

BOOL CRuController_GhostTrail::GetDistortFXEnable() const
{
	return m_distortFXEnable;
}

void CRuController_GhostTrail::SetDistortFXEnable(BOOL enable)
{
	m_distortFXEnable = enable;
}

const char *CRuController_GhostTrail::GetDistortTextureName() const
{
	if(m_distortTextureName)
	{
		return m_distortTextureName;
	}

	return "";
}

void CRuController_GhostTrail::SetDistortTextureName(const char *trailTextureName)
{
	delete[] m_distortTextureName;

	if(trailTextureName)
	{
		m_distortTextureName = ruNEW char [strlen(trailTextureName) + 1];
		memcpy(m_distortTextureName, trailTextureName, strlen(trailTextureName) + 1);
	}
	else
	{
		m_distortTextureName = NULL;
	}
}

INT32 CRuController_GhostTrail::GetDistortFrameCount() const
{
	return m_distortTextureFrameCount;
}

void CRuController_GhostTrail::SetDistortFrameCount(INT32 frameCount)
{
	m_distortTextureFrameCount = frameCount;
}

REAL CRuController_GhostTrail::GetDistortFrameRate() const
{
	return m_distortTextureFrameRate;
}

void CRuController_GhostTrail::SetDistortFrameRate(REAL frameRate)
{
	m_distortTextureFrameRate = frameRate;
}

void CRuController_GhostTrail::SetFadeCoefficients(REAL linear, REAL quadratic, REAL cubic)
{
	m_fadeCoefficients[0] = linear;
	m_fadeCoefficients[0] = quadratic;
	m_fadeCoefficients[0] = cubic;
}

void CRuController_GhostTrail::SetTrailAnchors(const char *anchor0, const char *anchor1)
{
	delete[] m_anchor0;

	if(anchor0)
	{
		m_anchor0 = ruNEW char [strlen(anchor0) + 1];
		memcpy(m_anchor0, anchor0, strlen(anchor0) + 1);
	}
	else
	{
		m_anchor0 = NULL;
	}

	delete[] m_anchor1;

	if(anchor1)
	{
		m_anchor1 = ruNEW char [strlen(anchor1) + 1];
		memcpy(m_anchor1, anchor1, strlen(anchor1) + 1);
	}
	else
	{
		m_anchor1 = NULL;
	}
}

IRuObject *CRuController_GhostTrail::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_GhostTrail();
	CRuController_GhostTrail *controller = static_cast<CRuController_GhostTrail *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_Controller::Clone(clonedObject);

	// Clone properties
	controller->m_frameType = m_frameType;
	controller->m_tInterval = m_tInterval;
	controller->m_segmentInterval = m_segmentInterval;
	controller->m_maxAge = m_maxAge;
	for(INT32 i = 0; i < 3; ++i)
		controller->m_fadeCoefficients[i] = m_fadeCoefficients[i];
	controller->m_maxControlPoints = m_maxControlPoints;
	controller->m_maxTrailSegments = m_maxTrailSegments;
	controller->m_activeDuration = m_activeDuration;
	controller->m_fadeOutDuration = m_fadeOutDuration;

	if(m_anchor0)
	{
		controller->m_anchor0 = ruNEW char [strlen(m_anchor0) + 1];
		strcpy(controller->m_anchor0, m_anchor0);
	}

	if(m_anchor1)
	{
		controller->m_anchor1 = ruNEW char [strlen(m_anchor1) + 1];
		strcpy(controller->m_anchor1, m_anchor1);
	}

	if(m_trailTextureName)
	{
		controller->m_trailTextureName = ruNEW char [strlen(m_trailTextureName) + 1];
		strcpy(controller->m_trailTextureName, m_trailTextureName);
	}

	controller->m_trailTextureFrameCount = m_trailTextureFrameCount;
	controller->m_trailTextureFrameRate = m_trailTextureFrameRate;

	controller->m_distortFXEnable = m_distortFXEnable;

	if(m_distortTextureName)
	{
		controller->m_distortTextureName = ruNEW char [strlen(m_distortTextureName) + 1];
		strcpy(controller->m_distortTextureName, m_distortTextureName);
	}

	controller->m_distortTextureFrameCount = m_distortTextureFrameCount;
	controller->m_distortTextureFrameRate = m_distortTextureFrameRate;

	return clonedObject;
}

BOOL CRuController_GhostTrail::SerializeFrom(IRuStream *stream)
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
				// Read properties
				stream->Read(&m_tInterval, sizeof(REAL));
				stream->Read(&m_segmentInterval, sizeof(REAL));
				stream->Read(&m_maxAge, sizeof(REAL));
				stream->Read(&m_fadeCoefficients[0], sizeof(REAL));
				stream->Read(&m_fadeCoefficients[1], sizeof(REAL));
				stream->Read(&m_fadeCoefficients[2], sizeof(REAL));
				stream->Read(&m_maxControlPoints, sizeof(INT32));
				stream->Read(&m_maxTrailSegments, sizeof(INT32));
				stream->Read(&m_activeDuration, sizeof(REAL));
				stream->Read(&m_fadeOutDuration, sizeof(REAL));

				// Read length of anchor 0 name including NULL, followed by the name
				INT32 len = 0;
				stream->Read(&len, sizeof(INT32));
				m_anchor0 = ruNEW char [len];
				stream->Read(m_anchor0, len);

				// Read length of anchor 1 name including NULL, followed by the name
				len = 0;
				stream->Read(&len, sizeof(INT32));
				m_anchor1 = ruNEW char [len];
				stream->Read(m_anchor1, len);

				// Read length of trail texture name including NULL, followed by the name
				len = 0;
				stream->Read(&len, sizeof(INT32));
				m_trailTextureName = ruNEW char [len];
				stream->Read(m_trailTextureName, len);

				stream->Read(&m_trailTextureFrameCount, sizeof(INT32));
				stream->Read(&m_trailTextureFrameRate, sizeof(REAL));
			}

			break;

		case 2:
			{
				// Read properties
				stream->Read(&m_frameType, sizeof(RuFrameType));
				stream->Read(&m_tInterval, sizeof(REAL));
				stream->Read(&m_segmentInterval, sizeof(REAL));
				stream->Read(&m_maxAge, sizeof(REAL));
				stream->Read(&m_fadeCoefficients[0], sizeof(REAL));
				stream->Read(&m_fadeCoefficients[1], sizeof(REAL));
				stream->Read(&m_fadeCoefficients[2], sizeof(REAL));
				stream->Read(&m_maxControlPoints, sizeof(INT32));
				stream->Read(&m_maxTrailSegments, sizeof(INT32));
				stream->Read(&m_activeDuration, sizeof(REAL));
				stream->Read(&m_fadeOutDuration, sizeof(REAL));

				// Read length of anchor 0 name including NULL, followed by the name
				INT32 len = 0;
				stream->Read(&len, sizeof(INT32));
				m_anchor0 = ruNEW char [len];
				stream->Read(m_anchor0, len);

				// Read length of anchor 1 name including NULL, followed by the name
				len = 0;
				stream->Read(&len, sizeof(INT32));
				m_anchor1 = ruNEW char [len];
				stream->Read(m_anchor1, len);

				// Read length of trail texture name including NULL, followed by the name
				len = 0;
				stream->Read(&len, sizeof(INT32));
				m_trailTextureName = ruNEW char [len];
				stream->Read(m_trailTextureName, len);

				stream->Read(&m_trailTextureFrameCount, sizeof(INT32));
				stream->Read(&m_trailTextureFrameRate, sizeof(REAL));
			}

			break;

		case 3:
			{
				// Read properties
				stream->Read(&m_frameType, sizeof(RuFrameType));
				stream->Read(&m_tInterval, sizeof(REAL));
				stream->Read(&m_segmentInterval, sizeof(REAL));
				stream->Read(&m_maxAge, sizeof(REAL));
				stream->Read(&m_fadeCoefficients[0], sizeof(REAL));
				stream->Read(&m_fadeCoefficients[1], sizeof(REAL));
				stream->Read(&m_fadeCoefficients[2], sizeof(REAL));
				stream->Read(&m_maxControlPoints, sizeof(INT32));
				stream->Read(&m_maxTrailSegments, sizeof(INT32));
				stream->Read(&m_activeDuration, sizeof(REAL));
				stream->Read(&m_fadeOutDuration, sizeof(REAL));

				// Read length of anchor 0 name including NULL, followed by the name
				INT32 len = 0;
				stream->Read(&len, sizeof(INT32));
				m_anchor0 = ruNEW char [len];
				stream->Read(m_anchor0, len);

				// Read length of anchor 1 name including NULL, followed by the name
				len = 0;
				stream->Read(&len, sizeof(INT32));
				m_anchor1 = ruNEW char [len];
				stream->Read(m_anchor1, len);

				// Read length of trail texture name including NULL, followed by the name
				len = 0;
				stream->Read(&len, sizeof(INT32));
				m_trailTextureName = ruNEW char [len];
				stream->Read(m_trailTextureName, len);

				stream->Read(&m_trailTextureFrameCount, sizeof(INT32));
				stream->Read(&m_trailTextureFrameRate, sizeof(REAL));

				stream->Read(&m_distortFXEnable, sizeof(BOOL));

				// Read length of distort texture name including NULL, followed by the name
				len = 0;
				stream->Read(&len, sizeof(INT32));
				m_distortTextureName = ruNEW char [len];
				stream->Read(m_distortTextureName, len);

				stream->Read(&m_distortTextureFrameCount, sizeof(INT32));
				stream->Read(&m_distortTextureFrameRate, sizeof(REAL));
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_GhostTrail::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 3;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Controller::SerializeTo(stream);

	// Write properties
	stream->Write(&m_frameType, sizeof(RuFrameType));
	stream->Write(&m_tInterval, sizeof(REAL));
	stream->Write(&m_segmentInterval, sizeof(REAL));
	stream->Write(&m_maxAge, sizeof(REAL));
	stream->Write(&m_fadeCoefficients[0], sizeof(REAL));
	stream->Write(&m_fadeCoefficients[1], sizeof(REAL));
	stream->Write(&m_fadeCoefficients[2], sizeof(REAL));
	stream->Write(&m_maxControlPoints, sizeof(INT32));
	stream->Write(&m_maxTrailSegments, sizeof(INT32));
	stream->Write(&m_activeDuration, sizeof(REAL));
	stream->Write(&m_fadeOutDuration, sizeof(REAL));

	// Write length of anchor 0 name including NULL, followed by the name
	if(m_anchor0)
	{
		INT32 len = (INT32)strlen(m_anchor0) + 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write(m_anchor0, len);
	}
	else
	{
		INT32 len = 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write("", len);
	}

	// Write length of anchor 1 name including NULL, followed by the name
	if(m_anchor1)
	{
		INT32 len = (INT32)strlen(m_anchor1) + 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write(m_anchor1, len);
	}
	else
	{
		INT32 len = 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write("", len);
	}

	// Write length of trail texture name including NULL, followed by the name
	if(m_trailTextureName)
	{
		INT32 len = (INT32)strlen(m_trailTextureName) + 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write(m_trailTextureName, len);
	}
	else
	{
		INT32 len = 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write("", len);
	}

	stream->Write(&m_trailTextureFrameCount, sizeof(INT32));
	stream->Write(&m_trailTextureFrameRate, sizeof(REAL));

	stream->Write(&m_distortFXEnable, sizeof(BOOL));

	// Write length of distort texture name including NULL, followed by the name
	if(m_distortTextureName)
	{
		INT32 len = (INT32)strlen(m_distortTextureName) + 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write(m_distortTextureName, len);
	}
	else
	{
		INT32 len = 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write("", len);
	}

	stream->Write(&m_distortTextureFrameCount, sizeof(INT32));
	stream->Write(&m_distortTextureFrameRate, sizeof(REAL));

	return TRUE;
}

BOOL CRuController_GhostTrail::Update(REAL elapsedTime)
{
	if(m_enabled)
	{
		if(m_targetEntity)
		{
			if(m_ghostTrail == NULL)
			{
				m_ghostTrail = ruNEW CRuEntity_GhostTrail();
				m_ghostTrail->SetFrameType(m_frameType);
				m_ghostTrail->SetUpdateInterval(m_tInterval);
				m_ghostTrail->SetSegmentInterval(m_segmentInterval);
				m_ghostTrail->SetMaxControlPoints(m_maxControlPoints);
				m_ghostTrail->SetMaxSegments(m_maxTrailSegments);
				m_ghostTrail->SetMaxAge(m_maxAge);
				m_ghostTrail->SetFadeCoefficients(m_fadeCoefficients[0], m_fadeCoefficients[1], m_fadeCoefficients[2]);
				m_ghostTrail->SetActiveDuration(m_activeDuration);
				m_ghostTrail->SetFadeOutDuration(m_fadeOutDuration);

				if(m_trailTextureName)
				{
					IRuTexture *texture = g_ruResourceManager->LoadTexture(m_trailTextureName, ruSURFACEFMT_DEFAULT, 0, TRUE);
					m_ghostTrail->SetTrailTexture(texture);
					ruSAFE_RELEASE(texture);
				}

				m_ghostTrail->SetTrailFrameCount(m_trailTextureFrameCount);
				m_ghostTrail->SetTrailFrameRate(m_trailTextureFrameRate);

			}

			IRuMesh *mesh;
			IRuMaterial *material;
			if(m_ghostTrail->GetMesh(0, &mesh, &material))
			{
				material->SetProperty("material:distortFX2Pass", m_distortFXEnable);
			}

			if(m_distortFXMatController)
			{
				if(m_distortFXEnable)
				{
					m_distortFXMatController->SetTarget(m_ghostTrail);
				}

				m_distortFXMatController->Update(elapsedTime);
			}

			REAL t = m_age;
			REAL elapsedTimeRemaining = elapsedTime;
			while(elapsedTimeRemaining > 0.0f && m_duration > 0.0f)
			{
				// Split time update duration into bite-sized segments
				elapsedTime = min(m_duration, elapsedTimeRemaining);
				elapsedTimeRemaining -= elapsedTime;

				REAL relativeT = t;
				REAL endTime = m_startTime + m_duration;

				if(m_loopTime >= 0.0f)
				{
					relativeT = fmod(t, m_loopTime);
					endTime = fmod(endTime, m_loopTime);
				}

				// Increment t
				t += elapsedTime;

				if(m_attached)
				{
					if(relativeT <= endTime)
					{
						relativeT += elapsedTime;

						// If we have hit the end time, detach the ghost trail
						if(relativeT > endTime)
						{
							if(m_ghostTrail)
							{
								m_ghostTrail->DetachFromParent();
								m_attached = FALSE;
							}
						}
					}
				}
				else
				{
					if(relativeT <= m_startTime)
					{
						relativeT += elapsedTime;

						// If we have hit the start time, detach the ghost trail
						if(relativeT > m_startTime)
						{
							if(m_ghostTrail)
							{
								CRuEntity *anchor0 = RuEntity_FindLinkFrame(m_targetEntity, m_anchor0);
								CRuEntity *anchor1 = RuEntity_FindLinkFrame(m_targetEntity, m_anchor1);

								if(anchor0 && anchor1)
								{
									m_targetEntity->AddEntity(m_ghostTrail);
									m_ghostTrail->SetTrailAnchors(anchor0, anchor1);
									m_attached = TRUE;
								}
							}
						}
					}
				}
			}
		}
	}

	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

IRuObject *CRuController_GhostTrail::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_GhostTrail::SetTarget(IRuObject *targetEntity)
{
	if(targetEntity->GetType().IsSubClassOf(CRuEntity_Container_Impl::Type()))
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		// Store target reference and setup event listener
		m_targetEntity = static_cast<CRuEntity_Container_Impl *>(targetEntity);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_GhostTrail::HandleEvent_Dispose, this, _1), NULL);

		if(m_distortFXEnable)
		{
			ruSAFE_RELEASE(m_distortFXMatController);
			m_distortFXMatController = ruNEW CRuController_MaterialTexture();
			m_distortFXMatController->SetTargetChannel(ruTEXCHANNEL_DISTORTMAP);
			m_distortFXMatController->SetLoopTime(0.0f);

			if(m_distortTextureName)
			{
				char tempStr[4096];

				RuAnimTrackVariant trackVariant;
				m_distortFXMatController->GetTrackByName("texture name", trackVariant);
				CRuAnimTrack<char *> *textureTrack = *boost::get<CRuAnimTrack<char *> *>(&trackVariant);

				if(m_distortTextureFrameCount <= 1)
				{
					// Build texture name
					sprintf(tempStr, "%s", m_distortTextureName);

					// Insert keyframe
					INT32 keyIdx = textureTrack->InsertKey(0.0f);
					textureTrack->SetKeyByIndex(keyIdx, tempStr);
				}
				else
				{
					for(INT32 i = 0; i < m_distortTextureFrameCount; ++i)
					{
						// Build texture name
						sprintf(tempStr, "%s_%d", m_distortTextureName, i);

						// Insert keyframe
						INT32 keyIdx = textureTrack->InsertKey(i * (1.0f / m_distortTextureFrameRate));
						textureTrack->SetKeyByIndex(keyIdx, tempStr);
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL CRuController_GhostTrail::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	delete[] m_anchor0;
	delete[] m_anchor1;
	delete[] m_trailTextureName;

	m_anchor0 = NULL;
	m_anchor1 = NULL;
	m_trailTextureName = NULL;

	if(m_ghostTrail)
	{
		// Detach from the target entity
		m_ghostTrail->DetachFromParent();

		// Release reference to the ghost trail
		ruSAFE_RELEASE(m_ghostTrail);
	}

	ruSAFE_RELEASE(m_distortFXMatController);

	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
