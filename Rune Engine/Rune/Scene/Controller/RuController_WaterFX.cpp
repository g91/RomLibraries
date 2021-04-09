#include "../../Scene/Controller/RuController_WaterFX.h"

#include "../../Helper/RuHelper_IRuEntity_Renderable.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_WaterFX, IRuEntity_KeyableController)
ruHEAP_DEFINE_SUBCLASS(CRuController_WaterFX, "CRuController_WaterFX", "IRuEntity_KeyableController")
ruCLASSGUID_DEFINE(CRuController_WaterFX, 0xF6170CC8, 0xA8AC4690, 0x972163A1, 0x7F025337)

// ************************************************************************************************************************************************************

CRuController_WaterFX::CRuController_WaterFX()
:	m_targetEntity(NULL),
	m_effectType(ruWATERFXTYPE_DIFFUSE),
	m_alphaFactor(1.0f),
	m_baseFactor(1.0f),
	m_reflectionFactor(0.5f),
	m_refractionFactor(0.5f),
	m_specularStrength(0.0f),
	m_baseColor(0xFFFFFFFF),
	m_uvScale(1.0f),
	m_baseMapFrames(1),
	m_baseMapName(NULL),
	m_normalMapFrames(1),
	m_normalMapName(NULL),
	m_cubeMapFrames(1),
	m_cubeMapName(NULL),
	m_bumpMapFrames(1),
	m_bumpMapName(NULL),
	m_isApplied(FALSE),
	m_isMaterialDirty(TRUE),
	m_material(NULL),
	m_bumpController(NULL)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_WaterFX::HandleEvent_Dispose, this, _1), NULL);

	m_bumpScale[0] = 0.03f;
	m_bumpScale[1] = 0.06f;
	m_bumpScale[2] = 0.12f;
	m_bumpScale[3] = 0.24f;

	m_bumpWeight[0] = 0.50f;
	m_bumpWeight[1] = 0.25f;
	m_bumpWeight[2] = 0.15f;
	m_bumpWeight[3] = 0.10f;

	// Setup tracks
	m_track_Motion.SetTrackName("Motion");
}

CRuController_WaterFX::~CRuController_WaterFX()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

RuWaterEffectType CRuController_WaterFX::GetEffectType()
{
	return m_effectType;
}

void CRuController_WaterFX::SetEffectType(RuWaterEffectType effectType)
{
	m_effectType = effectType;
}

REAL CRuController_WaterFX::GetAlphaFactor()
{
	return m_alphaFactor;
}

void CRuController_WaterFX::SetAlphaFactor(REAL factor)
{
	m_alphaFactor = factor;
}

REAL CRuController_WaterFX::GetBaseFactor()
{
	return m_baseFactor;
}

void CRuController_WaterFX::SetBaseFactor(REAL factor)
{
	m_baseFactor = factor;
}

REAL CRuController_WaterFX::GetReflectionFactor()
{
	return m_reflectionFactor;
}

void CRuController_WaterFX::SetReflectionFactor(REAL factor)
{
	m_reflectionFactor = factor;
}

REAL CRuController_WaterFX::GetRefractionFactor()
{
	return m_refractionFactor;
}

void CRuController_WaterFX::SetRefractionFactor(REAL factor)
{
	m_refractionFactor = factor;
}

REAL CRuController_WaterFX::GetSpecularStrength()
{
	return m_specularStrength;
}

void CRuController_WaterFX::SetSpecularStrength(REAL strength)
{
	m_specularStrength = strength;
}

RUCOLOR CRuController_WaterFX::GetBaseColor()
{
	return m_baseColor;
}

void CRuController_WaterFX::SetBaseColor(RUCOLOR baseColor)
{
	m_baseColor = baseColor;
}

REAL CRuController_WaterFX::GetUVScale()
{
	return m_uvScale;
}

void CRuController_WaterFX::SetUVScale(REAL uvScale)
{
	m_uvScale = uvScale;
}

INT32 CRuController_WaterFX::GetBaseMapFrames()
{
	return m_baseMapFrames;
}

void CRuController_WaterFX::SetBaseMapFrames(INT32 frames)
{
	m_baseMapFrames = frames;
}

const char *CRuController_WaterFX::GetBaseMapName()
{
	return m_baseMapName ? m_baseMapName : "";
}

void CRuController_WaterFX::SetBaseMapName(const char *name)
{
	delete[] m_baseMapName;

	m_baseMapName = ruNEW char [strlen(name) + 1];
	strcpy(m_baseMapName, name);
}

INT32 CRuController_WaterFX::GetNormalMapFrames()
{
	return m_normalMapFrames;
}

void CRuController_WaterFX::SetNormalMapFrames(INT32 frames)
{
	m_normalMapFrames = frames;
}

const char *CRuController_WaterFX::GetNormalMapName()
{
	return m_normalMapName ? m_normalMapName : "";
}

void CRuController_WaterFX::SetNormalMapName(const char *name)
{
	delete[] m_normalMapName;

	m_normalMapName = ruNEW char [strlen(name) + 1];
	strcpy(m_normalMapName, name);
}

INT32 CRuController_WaterFX::GetCubeMapFrames()
{
	return m_cubeMapFrames;
}

void CRuController_WaterFX::SetCubeMapFrames(INT32 frames)
{
	m_cubeMapFrames = frames;
}

const char *CRuController_WaterFX::GetCubeMapName()
{
	return m_cubeMapName ? m_cubeMapName : "";
}

void CRuController_WaterFX::SetCubeMapName(const char *name)
{
	delete[] m_cubeMapName;

	m_cubeMapName = ruNEW char [strlen(name) + 1];
	strcpy(m_cubeMapName, name);
}

INT32 CRuController_WaterFX::GetBumpMapFrames()
{
	return m_bumpMapFrames;
}

void CRuController_WaterFX::SetBumpMapFrames(INT32 frames)
{
	m_bumpMapFrames = frames;
}

const char *CRuController_WaterFX::GetBumpMapName()
{
	return m_bumpMapName ? m_bumpMapName : "";
}

void CRuController_WaterFX::SetBumpMapName(const char *name)
{
	delete[] m_bumpMapName;

	m_bumpMapName = ruNEW char [strlen(name) + 1];
	strcpy(m_bumpMapName, name);
}

REAL CRuController_WaterFX::GetBumpScale(INT32 layerIdx)
{
	return m_bumpScale[layerIdx];
}

void CRuController_WaterFX::SetBumpScale(INT32 layerIdx, REAL scale)
{
	m_bumpScale[layerIdx] = scale;
}

REAL CRuController_WaterFX::GetBumpWeight(INT32 layerIdx)
{
	return m_bumpWeight[layerIdx];
}

void CRuController_WaterFX::SetBumpWeight(INT32 layerIdx, REAL weight)
{
	m_bumpWeight[layerIdx] = weight;
}

const CRuVector2 &CRuController_WaterFX::GetBumpScroll(INT32 layerIdx)
{
	return m_bumpScroll[layerIdx];
}

void CRuController_WaterFX::SetBumpScroll(INT32 layerIdx, const CRuVector2 &scroll)
{
	m_bumpScroll[layerIdx] = scroll;
}

IRuObject *CRuController_WaterFX::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_WaterFX();
	CRuController_WaterFX *controller = static_cast<CRuController_WaterFX *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_KeyableController::Clone(clonedObject);

	// Clone properties
	controller->m_effectType = m_effectType;

	controller->m_alphaFactor = m_alphaFactor;
	controller->m_baseFactor = m_baseFactor;
	controller->m_reflectionFactor = m_reflectionFactor;
	controller->m_refractionFactor = m_refractionFactor;
	controller->m_specularStrength = m_specularStrength;

	controller->m_baseColor = m_baseColor;

	controller->m_uvScale = m_uvScale;

	controller->m_baseMapFrames = m_baseMapFrames;
	controller->m_normalMapFrames = m_normalMapFrames;
	controller->m_cubeMapFrames = m_cubeMapFrames;
	controller->m_bumpMapFrames = m_bumpMapFrames;

	if(m_baseMapName)
	{
		controller->SetBaseMapName(m_baseMapName);
	}

	if(m_normalMapName)
	{
		controller->SetNormalMapName(m_normalMapName);
	}

	if(m_cubeMapName)
	{
		controller->SetCubeMapName(m_cubeMapName);
	}

	if(m_bumpMapName)
	{
		controller->SetBumpMapName(m_bumpMapName);
	}

	for(INT32 i = 0; i < 4; ++i)
	{
		controller->m_bumpScale[i] = m_bumpScale[i];
		controller->m_bumpWeight[i] = m_bumpWeight[i];
		controller->m_bumpScroll[i] = m_bumpScroll[i];
	}

	// Clone tracks
	CloneTracks(controller);

	return clonedObject;
}

BOOL CRuController_WaterFX::SerializeFrom(IRuStream *stream)
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
				INT32 length;

				// Read reflection, refraction, and alpha factors
				stream->Read(&m_effectType, sizeof(RuWaterEffectType));

				stream->Read(&m_alphaFactor, sizeof(float));
				stream->Read(&m_baseFactor, sizeof(float));
				stream->Read(&m_reflectionFactor, sizeof(float));
				stream->Read(&m_refractionFactor, sizeof(float));
				stream->Read(&m_specularStrength, sizeof(float));

				stream->Read(&m_baseColor, sizeof(DWORD));

				stream->Read(&m_uvScale, sizeof(float));

				// Read number of base map frames
				stream->Read(&m_baseMapFrames, sizeof(INT32));

				// Read number of normal map frames
				stream->Read(&m_normalMapFrames, sizeof(INT32));

				// Read number of cube map frames
				stream->Read(&m_cubeMapFrames, sizeof(INT32));

				// Read number of bump map frames
				stream->Read(&m_bumpMapFrames, sizeof(INT32));

				// Read base map name length including NULL, then the base map name including NULL
				stream->Read(&length, sizeof(INT32));
				if(length > 0)
				{
					m_baseMapName = ruNEW char [length];
					stream->Read(m_baseMapName, sizeof(char) * length);
				}
				else
				{
					m_baseMapName = NULL;
				}

				// Read normal map name length including NULL, then the normal map name including NULL
				stream->Read(&length, sizeof(INT32));
				if(length > 0)
				{
					m_normalMapName = ruNEW char [length];
					stream->Read(m_normalMapName, sizeof(char) * length);
				}
				else
				{
					m_normalMapName = NULL;
				}

				// Read cube map name length including NULL, then the cube map name including NULL
				stream->Read(&length, sizeof(INT32));
				if(length > 0)
				{
					m_cubeMapName = ruNEW char [length];
					stream->Read(m_cubeMapName, sizeof(char) * length);
				}
				else
				{
					m_cubeMapName = NULL;
				}

				// Read bump map name length including NULL, then the bump map name including NULL
				stream->Read(&length, sizeof(INT32));
				if(length > 0)
				{
					m_bumpMapName = ruNEW char [length];
					stream->Read(m_bumpMapName, sizeof(char) * length);
				}
				else
				{
					m_bumpMapName = NULL;
				}

				for(INT32 i = 0; i < 4; ++i)
				{
					stream->Read(&m_bumpScale[i], sizeof(REAL));
					stream->Read(&m_bumpWeight[i], sizeof(REAL));
					stream->Read(&m_bumpScroll[i].m_x, sizeof(REAL));
					stream->Read(&m_bumpScroll[i].m_y, sizeof(REAL));
				}

				// Serialize tracks
//				m_track_Motion.SerializeFrom(stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_WaterFX::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeTo(stream);

	// Write reflection, refraction, and alpha factors
	stream->Write(&m_effectType, sizeof(RuWaterEffectType));

	stream->Write(&m_alphaFactor, sizeof(float));
	stream->Write(&m_baseFactor, sizeof(float));
	stream->Write(&m_reflectionFactor, sizeof(float));
	stream->Write(&m_refractionFactor, sizeof(float));
	stream->Write(&m_specularStrength, sizeof(float));

	stream->Write(&m_baseColor, sizeof(DWORD));

	stream->Write(&m_uvScale, sizeof(float));

	// Write number of base map frames
	stream->Write(&m_baseMapFrames, sizeof(INT32));

	// Write number of normal map frames
	stream->Write(&m_normalMapFrames, sizeof(INT32));

	// Write number of cube map frames
	stream->Write(&m_cubeMapFrames, sizeof(INT32));

	// Write number of bump map frames
	stream->Write(&m_bumpMapFrames, sizeof(INT32));

	// Write base map name length including NULL, then the base map name including NULL
	if(m_baseMapName)
	{
		INT32 length = (INT32)strlen(m_baseMapName) + 1;
		stream->Write(&length, sizeof(INT32));
		stream->Write(m_baseMapName, sizeof(char) * length);
	}
	else
	{
		INT32 length = 1;
		stream->Write(&length, sizeof(INT32));
		stream->Write("", sizeof(char) * length);
	}

	// Write normal map name length including NULL, then the normal map name including NULL
	if(m_normalMapName)
	{
		INT32 length = (INT32)strlen(m_normalMapName) + 1;
		stream->Write(&length, sizeof(INT32));
		stream->Write(m_normalMapName, sizeof(char) * length);
	}
	else
	{
		INT32 length = 1;
		stream->Write(&length, sizeof(INT32));
		stream->Write("", sizeof(char) * length);
	}

	// Write cube map name length including NULL, then the cube map name including NULL
	if(m_cubeMapName)
	{
		INT32 length = (INT32)strlen(m_cubeMapName) + 1;
		stream->Write(&length, sizeof(INT32));
		stream->Write(m_cubeMapName, sizeof(char) * length);
	}
	else
	{
		INT32 length = 1;
		stream->Write(&length, sizeof(INT32));
		stream->Write("", sizeof(char) * length);
	}

	// Write bump map name length including NULL, then the bump map name including NULL
	if(m_bumpMapName)
	{
		INT32 length = (INT32)strlen(m_bumpMapName) + 1;
		stream->Write(&length, sizeof(INT32));
		stream->Write(m_bumpMapName, sizeof(char) * length);
	}
	else
	{
		INT32 length = 1;
		stream->Write(&length, sizeof(INT32));
		stream->Write("", sizeof(char) * length);
	}

	for(INT32 i = 0; i < 4; ++i)
	{
		stream->Write(&m_bumpScale[i], sizeof(REAL));
		stream->Write(&m_bumpWeight[i], sizeof(REAL));
		stream->Write(&m_bumpScroll[i].m_x, sizeof(REAL));
		stream->Write(&m_bumpScroll[i].m_y, sizeof(REAL));
	}

	// Serialize tracks
//	m_track_Motion.SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_WaterFX::Update(REAL elapsedTime)
{
	if(m_isMaterialDirty)
	{
		BuildMaterial();
	}

	if(m_bumpController)
	{
		m_bumpController->Update(elapsedTime);
	}

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

			if(!m_isApplied)
			{
				// Retrieve water shader
//				IRuShader *shader_Minimal = g_ruShaderManager->GetShader("ruSHADER_WATER_MINIMAL");

				// Set all material shaders
				CRuArrayList<IRuMaterial *> materialList;
				RuEntity_EnumerateMaterials(m_targetEntity, materialList);

				for(INT32 i = 0; i < materialList.Count(); ++i)
				{
					*static_cast<CRuMaterial *>(materialList[i]) = *m_material;
//					materialList[i]->SetShader(shader_Minimal);
				}

				// Set apply status
//				m_isApplied = TRUE;
			}
		}
	}

	// Chain update call to base controller class
	return IRuEntity_KeyableController::Update(elapsedTime);
}

IRuObject *CRuController_WaterFX::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_WaterFX::SetTarget(IRuObject *target)
{
	if(target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		// Register target entity
		m_targetEntity = static_cast<CRuEntity *>(target);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_WaterFX::HandleEvent_Dispose, this, _1), NULL);

		// Reset apply status
		m_isApplied = FALSE;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuController_WaterFX::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	delete[] m_baseMapName;
	delete[] m_normalMapName;
	delete[] m_cubeMapName;
	delete[] m_bumpMapName;

	ruSAFE_RELEASE(m_material);
	ruSAFE_RELEASE(m_bumpController);

	return TRUE;
}

INT32 CRuController_WaterFX::GetNumTracks() const
{
	return 1;
}

BOOL CRuController_WaterFX::GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut)
{
	switch(trackIndex)
	{
		case 0:
			trackOut = &m_track_Motion;
			return TRUE;
	}

	return FALSE;
}

void CRuController_WaterFX::BuildMaterial()
{
	char tempStr[4096];

	// Crete new material
	ruSAFE_RELEASE(m_material);
	m_material = ruNEW CRuMaterial();

	// Get engine settings
	RuPropertyType propertyValue;

	BOOL reflectionEnabled = g_ruEngineSettings->GetSetting("gfx:water reflection", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;
	BOOL refractionEnabled = g_ruEngineSettings->GetSetting("gfx:water refraction", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;

	switch(m_effectType)
	{
		case ruWATERFXTYPE_FULL:
			{
				if(reflectionEnabled && refractionEnabled)
				{
					m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_WATER_FULL"));
				}
				else
				{
					// default crap (remove later)
					m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_WATER_MINIMAL"));
				}
			}

			break;

		default:
			{
				m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_WATER_MINIMAL"));
			}

			break;
	}

	// Create and configure new bump controller
	ruSAFE_RELEASE(m_bumpController);
	m_bumpController = ruNEW CRuController_MaterialTexture();

	if(m_bumpMapName)
	{
		// Get the texture track from bump controller
		RuAnimTrackVariant trackVariant;
		m_bumpController->GetTrackByName("texture name", trackVariant);
		CRuAnimTrack<char *> *textureTrack = *boost::get<CRuAnimTrack<char *> *>(&trackVariant);

		for(INT32 i = 0; i < m_bumpMapFrames; ++i)
		{
			// Build texture name
			sprintf(tempStr, "%s_%d", m_bumpMapName, i);

			// Insert keyframe
			INT32 keyIdx = textureTrack->InsertKey(i * 0.0555f);
			textureTrack->SetKeyByIndex(keyIdx, tempStr);
		}
	}

	m_bumpController->SetLoopTime(0.0f);
	m_bumpController->SetTarget(m_material);
	m_bumpController->SetTargetChannel(ruTEXCHANNEL_NORMALMAP);
	m_bumpController->AdjustDurationToKeys();

	// Reset apply status
	m_isApplied = FALSE;
	m_isMaterialDirty = FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
