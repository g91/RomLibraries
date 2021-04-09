#include "../../Scene/Controller/RuController_MaterialTexture.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_MaterialTexture, IRuEntity_KeyableController)
ruHEAP_DEFINE_SUBCLASS(CRuController_MaterialTexture, "CRuController_MaterialTexture", "IRuEntity_KeyableController")
ruCLASSGUID_DEFINE(CRuController_MaterialTexture, 0xD677EE4E, 0xB03B4e50, 0x8CEE0025, 0xE5BB5B45)

// ************************************************************************************************************************************************************

CRuController_MaterialTexture::CRuController_MaterialTexture()
:	m_target(NULL),
	m_targetChannel(ruTEXCHANNEL_DIFFUSEMAP),
	m_curTextureName(NULL),
	m_curTexture(NULL)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_MaterialTexture::HandleEvent_Dispose, this, _1), NULL);

	m_track_TextureName.SetTrackName("Texture");
}

CRuController_MaterialTexture::~CRuController_MaterialTexture()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

IRuObject *CRuController_MaterialTexture::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_MaterialTexture();
	CRuController_MaterialTexture *controller = static_cast<CRuController_MaterialTexture *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_KeyableController::Clone(clonedObject);

	controller->m_targetChannel = m_targetChannel;

	// Clone tracks
	CloneTracks(controller);

	return clonedObject;
}

BOOL CRuController_MaterialTexture::SerializeFrom(IRuStream *stream)
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
	IRuEntity_KeyableController::SerializeFrom(stream);

	switch(version)
	{
		case 1:
			{
				// Read target channel
				stream->Read(&m_targetChannel, sizeof(RuTextureChannel));

				// Serialize tracks
				m_track_TextureName.SerializeFrom(stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_MaterialTexture::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeTo(stream);

	// Write target channel
	stream->Write(&m_targetChannel, sizeof(RuTextureChannel));

	// Serialize tracks
	m_track_TextureName.SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_MaterialTexture::Update(REAL elapsedTime)
{
	if(m_enabled && m_target)
	{
		REAL relativeT = m_age - m_startTime;

		if(relativeT >= 0.0f)
		{
			// If looping is enabled, clamp time range to the duration
			if(relativeT > m_duration && m_duration > 0.0f && m_loopTime >= 0.0f)
			{
				relativeT = fmod(relativeT, m_duration);
			}

			char *textureName = m_track_TextureName.GetDiscreteKey(relativeT);

			if(textureName && textureName != m_curTextureName && textureName[0] != NULL)
			{
				// Save current texture name
				m_curTextureName = textureName;

				// Look for texture in the cache
				if(m_textureCache.Get(textureName, m_curTexture) == FALSE)
				{
					// Texture is not in cache, load it from resource manager
					m_curTexture = g_ruResourceManager->LoadTexture(textureName, ruSURFACEFMT_DEFAULT, 0, TRUE);

					// Insert texture into cache
					m_textureCache.Add(textureName, m_curTexture);
				}
			}

			if(m_target->GetType().IsSubClassOf(IRuMaterial::Type()))
			{
				static_cast<IRuMaterial *>(m_target)->SetTexture(m_targetChannel, m_curTexture);
			}
			else if(m_target->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
			{
				for(INT32 i = 0; i < static_cast<IRuEntity_Renderable *>(m_target)->GetNumMeshes(); ++i)
				{
					IRuMesh *mesh;
					IRuMaterial *material;

					if(static_cast<IRuEntity_Renderable *>(m_target)->GetMesh(i, &mesh, &material))
					{
						material->SetTexture(m_targetChannel, m_curTexture);
					}
				}
			}
			else
			{
				OutputDebugString("Unsupported\n");
			}
		}
	}

	// Chain update call to base controller class
	return IRuEntity_KeyableController::Update(elapsedTime);
}

IRuObject *CRuController_MaterialTexture::GetTarget()
{
	return m_target;
}

BOOL CRuController_MaterialTexture::SetTarget(IRuObject *target)
{
	// If we had a previous target, unregister it
	if(m_target && m_target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		m_target->Event_Dispose().UnregisterHandler(this->GetGUID());
	}

	// Store target reference, setup event listener, and setup target parameter block
	m_target = target;

	if(m_target && m_target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		m_target->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_MaterialTexture::HandleEvent_Dispose, this, _1), NULL);
	}
	else if(m_target)
	{
		m_target->AddRef();
	}

	return TRUE;
}

RuTextureChannel CRuController_MaterialTexture::GetTargetChannel()
{
	return m_targetChannel;
}

void CRuController_MaterialTexture::SetTargetChannel(RuTextureChannel targetChannel)
{
	m_targetChannel = targetChannel;
}

INT32 CRuController_MaterialTexture::GetNumTracks() const
{
	return 1;
}

BOOL CRuController_MaterialTexture::GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut)
{
	switch(trackIndex)
	{
		case 0:
			trackOut = &m_track_TextureName;
			return TRUE;
	}

	return FALSE;
}

BOOL CRuController_MaterialTexture::GetTrackByName(const char *trackName, RuAnimTrackVariant &trackOut)
{
	if(strcmpi(trackName, "texture name") == 0)
	{
		trackOut = &m_track_TextureName;
		return TRUE;
	}

	return FALSE;
}

BOOL CRuController_MaterialTexture::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_target && m_target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		m_target->Event_Dispose().UnregisterHandler(this->GetGUID());
	}
	else if(m_target)
	{
		ruSAFE_RELEASE(m_target);
	}

	m_target = NULL;

	// Reset current texture
	m_curTexture = NULL;
	m_curTextureName = NULL;

	// Flush texture cache
	m_textureCache.ForAllElements(boost::bind(&CRuController_MaterialTexture::ClearTextureCacheCallback, this, _1, _2));
	m_textureCache.Clear();

	return TRUE;
}

BOOL CRuController_MaterialTexture::ClearTextureCacheCallback(const void *key, void *data)
{
	IRuTexture *texture = *reinterpret_cast<IRuTexture **>(data);
	ruSAFE_RELEASE(texture);
	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
