#include "../../Scene/Terrain/RuWorld_WaterBodyDescriptor.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuWorld_WaterBodyDescriptor, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_WaterBodyDescriptor, "CRuWorld_WaterBodyDescriptor", "IRuObject")
ruCLASSGUID_DEFINE(CRuWorld_WaterBodyDescriptor, 0xF7BB4692, 0x215443a8, 0xAAD2AF46, 0x20D6F03B)

// ***********************************************************************************************************************************************************

CRuWorld_WaterBodyDescriptor::CRuWorld_WaterBodyDescriptor()
{
	m_bodyType = RuWorld::ruWATERBODYTYPE_NORMAL;

	m_effectType = ruWATERFXTYPE_DIFFUSE;

	m_submergedColor = RUCOLOR_ARGB(255, 30, 60, 130);
	m_submergedConstantFog = 0.5f;
	m_submergedFogStart = 0.5f;
	m_submergedFogEnd = 1.0f;

	m_isDirty = TRUE;
	m_waterMaterial = NULL;
	m_baseController = NULL;
	m_causticsController = NULL;

	m_bumpController = NULL;

	this->SetProperty("waterDesc:full:bump frame rate", 30);
	this->SetProperty("waterDesc:full:bump frame count", 1);

	this->SetProperty("waterDesc:common:bump factor", 0.85f);

	this->SetProperty("waterDesc:full:bump scale 0", 0.005f);
	this->SetProperty("waterDesc:full:bump weight 0", 0.500f);

	this->SetProperty("waterDesc:full:bump scale 1", 0.010f);
	this->SetProperty("waterDesc:full:bump weight 1", 0.250f);

	this->SetProperty("waterDesc:full:bump scale 2", 0.020f);
	this->SetProperty("waterDesc:full:bump weight 2", 0.150f);

	this->SetProperty("waterDesc:full:bump scale 3", 0.040f);
	this->SetProperty("waterDesc:full:bump weight 3", 0.100f);

	// Build default water body material
	BuildWaterBodyMaterial();
}

CRuWorld_WaterBodyDescriptor::~CRuWorld_WaterBodyDescriptor()
{
	// Release water body material
	ruSAFE_RELEASE(m_waterMaterial);
	ruSAFE_RELEASE(m_baseController);
	ruSAFE_RELEASE(m_causticsController);

	ruSAFE_RELEASE(m_bumpController);
}

BOOL CRuWorld_WaterBodyDescriptor::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version;
	stream->Read(&version, sizeof(INT32));

	if(version >= 2)
	{
		// Chain serialization to base class
		IRuObject::SerializeFrom(stream);
	}

	// Read water body GUID
	if(RuReadGUID(stream, m_guid) == FALSE)
	{
		return FALSE;
	}

	REAL dummyREAL;
	DWORD dummyDWORD;
	INT32 dummyINT32;

	switch(version)
	{
		case 1:
		case 2:
			{
				INT32 length;

				// Read reflection, refraction, and alpha factors
				stream->Read(&m_effectType, sizeof(RuWaterEffectType));
				stream->Read(&dummyREAL, sizeof(float));
				stream->Read(&dummyREAL, sizeof(float));
				stream->Read(&dummyREAL, sizeof(float));
				stream->Read(&dummyREAL, sizeof(float));
				stream->Read(&dummyREAL, sizeof(float));
				stream->Read(&dummyREAL, sizeof(float));

				stream->Read(&dummyDWORD, sizeof(DWORD));

				// Read number of base map frames
				stream->Read(&dummyINT32, sizeof(INT32));

				// Read number of normal map frames
				stream->Read(&dummyINT32, sizeof(INT32));

				// Read number of cube map frames
				stream->Read(&dummyINT32, sizeof(INT32));

				// Read base map name length including NULL, then the base map name including NULL
				stream->Read(&length, sizeof(INT32));
				stream->Seek(sizeof(char) * length, ruSSM_Current);

				// Read normal map name length including NULL, then the normal map name including NULL
				stream->Read(&length, sizeof(INT32));
				stream->Seek(sizeof(char) * length, ruSSM_Current);

				// Read cube map name length including NULL, then the cube map name including NULL
				stream->Read(&length, sizeof(INT32));
				stream->Seek(sizeof(char) * length, ruSSM_Current);

				// Other shit
				m_isDirty = FALSE;
				BuildWaterBodyMaterial();
			}

			break;

		case 3:
		case 4:
			{
				INT32 length;

				// Read water body type
				stream->Read(&m_bodyType, sizeof(RuWorld::WaterBodyType));

				// Read reflection, refraction, and alpha factors
				stream->Read(&m_effectType, sizeof(RuWaterEffectType));
				stream->Read(&dummyREAL, sizeof(float));
				stream->Read(&dummyREAL, sizeof(float));
				stream->Read(&dummyREAL, sizeof(float));
				stream->Read(&dummyREAL, sizeof(float));
				stream->Read(&dummyREAL, sizeof(float));
				stream->Read(&dummyREAL, sizeof(float));

				stream->Read(&dummyDWORD, sizeof(DWORD));

				// Read number of base map frames
				stream->Read(&dummyINT32, sizeof(INT32));

				// Read number of normal map frames
				stream->Read(&dummyINT32, sizeof(INT32));

				// Read number of cube map frames
				stream->Read(&dummyINT32, sizeof(INT32));

				// Read base map name length including NULL, then the base map name including NULL
				stream->Read(&length, sizeof(INT32));
				stream->Seek(sizeof(char) * length, ruSSM_Current);

				// Read normal map name length including NULL, then the normal map name including NULL
				stream->Read(&length, sizeof(INT32));
				stream->Seek(sizeof(char) * length, ruSSM_Current);

				// Read cube map name length including NULL, then the cube map name including NULL
				stream->Read(&length, sizeof(INT32));
				stream->Seek(sizeof(char) * length, ruSSM_Current);

				if(version > 3)
				{
					// Read submerged settings
					stream->Read(&m_submergedColor, sizeof(RUCOLOR));
					stream->Read(&m_submergedConstantFog, sizeof(REAL));
					stream->Read(&m_submergedFogStart, sizeof(REAL));
					stream->Read(&m_submergedFogEnd, sizeof(REAL));
				}

				// Other shit
				m_isDirty = FALSE;
				BuildWaterBodyMaterial();
			}

			break;

		case 5:
			{
				// Read water body type
				stream->Read(&m_bodyType, sizeof(RuWorld::WaterBodyType));

				// Read reflection, refraction, and alpha factors
				stream->Read(&m_effectType, sizeof(RuWaterEffectType));

				// Read submerged settings
				stream->Read(&m_submergedColor, sizeof(RUCOLOR));
				stream->Read(&m_submergedConstantFog, sizeof(REAL));
				stream->Read(&m_submergedFogStart, sizeof(REAL));
				stream->Read(&m_submergedFogEnd, sizeof(REAL));

				// Other shit
				m_isDirty = FALSE;
				BuildWaterBodyMaterial();
			}

			break;
	}

	return TRUE;
}

BOOL CRuWorld_WaterBodyDescriptor::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 5;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeTo(stream);

	// Write water body GUID
	stream->Write(m_guid.Data(), m_guid.Size());

	// Write water body type
	stream->Write(&m_bodyType, sizeof(RuWorld::WaterBodyType));

	// Write reflection, refraction, and alpha factors
	stream->Write(&m_effectType, sizeof(RuWaterEffectType));

	// Write submerged settings
	stream->Write(&m_submergedColor, sizeof(RUCOLOR));
	stream->Write(&m_submergedConstantFog, sizeof(REAL));
	stream->Write(&m_submergedFogStart, sizeof(REAL));
	stream->Write(&m_submergedFogEnd, sizeof(REAL));

	return TRUE;
}

void CRuWorld_WaterBodyDescriptor::Update(REAL elapsedTime)
{
	if(m_bumpController)
	{
		m_bumpController->Update(elapsedTime);
	}

	// Update base controller if available
	if(m_baseController)
	{
		m_baseController->Update(elapsedTime);
	}

	// Update caustics controller if available
	if(m_causticsController)
	{
		m_causticsController->Update(elapsedTime);
	}

	if(m_waterMaterial)
	{
		// Update bump T
		REAL t = 0.0f;
		UINT64 parameter;
		if(m_waterMaterial->GetParameterBlock()->GetParameter(ruPARAMTYPE_CUSTOM_0, parameter))
		{
			t = RuCastINT64ToFloat(parameter);
		}

		t += elapsedTime;

		m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_CUSTOM_0, RuCastFloatToINT64(t));

		RuPropertyType propertyValue;

		// Set bump parameters
		m_bumpParams_0[0] = this->GetProperty("waterDesc:full:bump x scroll 0", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		m_bumpParams_0[1] = this->GetProperty("waterDesc:full:bump y scroll 0", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		m_bumpParams_0[2] = this->GetProperty("waterDesc:full:bump scale 0", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		m_bumpParams_0[3] = 0.0f;

		m_bumpParams_1[0] = this->GetProperty("waterDesc:full:bump x scroll 1", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		m_bumpParams_1[1] = this->GetProperty("waterDesc:full:bump y scroll 1", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		m_bumpParams_1[2] = this->GetProperty("waterDesc:full:bump scale 1", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		m_bumpParams_1[3] = 0.0f;

		m_bumpParams_2[0] = this->GetProperty("waterDesc:full:bump x scroll 2", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		m_bumpParams_2[1] = this->GetProperty("waterDesc:full:bump y scroll 2", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		m_bumpParams_2[2] = this->GetProperty("waterDesc:full:bump scale 2", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		m_bumpParams_2[3] = 0.0f;

		m_bumpParams_3[0] = this->GetProperty("waterDesc:full:bump x scroll 3", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		m_bumpParams_3[1] = this->GetProperty("waterDesc:full:bump y scroll 3", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		m_bumpParams_3[2] = this->GetProperty("waterDesc:full:bump scale 3", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		m_bumpParams_3[3] = 0.0f;

		m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_CUSTOM_1, reinterpret_cast<UINT64>(m_bumpParams_0));
		m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_CUSTOM_2, reinterpret_cast<UINT64>(m_bumpParams_1));
		m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_CUSTOM_3, reinterpret_cast<UINT64>(m_bumpParams_2));
		m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_CUSTOM_4, reinterpret_cast<UINT64>(m_bumpParams_3));

		// Set bump weights
		REAL bumpWeight_0 = this->GetProperty("waterDesc:full:bump weight 0", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		REAL bumpWeight_1 = this->GetProperty("waterDesc:full:bump weight 1", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		REAL bumpWeight_2 = this->GetProperty("waterDesc:full:bump weight 2", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
		REAL bumpWeight_3 = this->GetProperty("waterDesc:full:bump weight 3", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;

		DWORD packedWeights = RUCOLOR_ARGB((BYTE) (bumpWeight_0 * 255), (BYTE) (bumpWeight_1 * 255), (BYTE) (bumpWeight_2 * 255), (BYTE) (bumpWeight_3 * 255));
		m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_CUSTOM_5, packedWeights);
	}
}

REAL CRuWorld_WaterBodyDescriptor::GetSubmergedBumpStrength()
{
	RuPropertyType propertyValue;
	REAL submergedBumpStrength = this->GetProperty("waterDesc:submerged:bump strength", propertyValue) ? boost::get<REAL>(propertyValue) : 0.05f;

	return submergedBumpStrength;
}

CRuMaterial *CRuWorld_WaterBodyDescriptor::GetWaterBodyMaterial()
{
	return m_waterMaterial;
}

void CRuWorld_WaterBodyDescriptor::BuildWaterBodyMaterial()
{
	char tempStr[1024];

	// Create new water material
	ruSAFE_RELEASE(m_waterMaterial);
	m_waterMaterial = ruNEW CRuMaterial();

	// Set water material to be 2-sided
	m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_2SIDED, TRUE);

	// Setup material based on body type
	switch(m_bodyType)
	{
		case RuWorld::ruWATERBODYTYPE_NORMAL:
		case RuWorld::ruWATERBODYTYPE_SOLID:
			{
				// Get engine settings
				RuPropertyType propertyValue;

				BOOL reflectionEnabled = g_ruEngineSettings->GetSetting("gfx:water reflection", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;
				BOOL refractionEnabled = g_ruEngineSettings->GetSetting("gfx:water refraction", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;

				BOOL fullWaterModelEnabled = this->GetProperty("waterDesc:full:enable", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;

				if(m_effectType == ruWATERFXTYPE_DIRECT)
				{
					m_waterMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_WATER_LAVA"));
				}
				else
				{
					// Use full water reflection/refraction model
					if(reflectionEnabled && refractionEnabled && fullWaterModelEnabled)
					{
						m_waterMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_WATER_FULL"));
					}
					else
					{
						// default crap (remove later)
						m_waterMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_WATER_MINIMAL"));
					}
				}

				// Set self-illumination
				REAL selfIllumination = this->GetProperty("waterDesc:self illumination", propertyValue) ? boost::get<REAL>(propertyValue) : 0.0f;
				m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, RUCOLOR_ARGB(0, 255, 255, 255));
				m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(selfIllumination));

				// Create and configure new bump controller
				ruSAFE_RELEASE(m_bumpController);
				m_bumpController = ruNEW CRuController_MaterialTexture();

				INT32 bumpFrameRate = this->GetProperty("waterDesc:full:bump frame rate", propertyValue) ? boost::get<INT32>(propertyValue) : 30;
				INT32 bumpFrameCount = this->GetProperty("waterDesc:full:bump frame count", propertyValue) ? boost::get<INT32>(propertyValue) : 1;
				const char *bumpName = this->GetProperty("waterDesc:full:bump name", propertyValue) ? boost::get<const char *>(propertyValue) : NULL;
				if(bumpName)
				{
					RuAnimTrackVariant trackVariant;
					m_bumpController->GetTrackByName("texture name", trackVariant);
					CRuAnimTrack<char *> *textureTrack = *boost::get<CRuAnimTrack<char *> *>(&trackVariant);

					REAL frameInterval = 1.0f / bumpFrameRate;

					for(INT32 i = 0; i < bumpFrameCount; ++i)
					{
						// Build texture name
						sprintf(tempStr, "%s_%d", bumpName, i);

						// Insert keyframe
						INT32 keyIdx = textureTrack->InsertKey(i * frameInterval);
						textureTrack->SetKeyByIndex(keyIdx, tempStr);
					}
				}

				m_bumpController->SetLoopTime(0.0f);
				m_bumpController->SetTarget(m_waterMaterial);
				m_bumpController->SetTargetChannel(ruTEXCHANNEL_NORMALMAP);
				m_bumpController->AdjustDurationToKeys();

				// Setup shader parameters
				CRuColor4 waveColor = this->GetProperty("waterDesc:common:wave color", propertyValue) ? boost::get<CRuColor4>(propertyValue) : CRuColor4(1.0f, 1.0f, 1.0f, 1.0f);
				m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_WATER_WAVECOLOR, waveColor.GetARGB());

				REAL bumpFactor = this->GetProperty("waterDesc:common:bump factor", propertyValue) ? boost::get<REAL>(propertyValue) : 0.85f;
				m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_WATER_BUMPFACTOR, RuCastFloatToINT64(bumpFactor));

				// Delete old keys from the submerged bump texture name track
				while(m_track_SubmergedBumpName.GetNumKeys() > 0)
				{
					m_track_SubmergedBumpName.DeleteKeyByIndex(0);
				}

				// Create new keys on the submerged bump texture name track
				INT32 submergedBumpFrameRate = this->GetProperty("waterDesc:submerged:bump frame rate", propertyValue) ? boost::get<INT32>(propertyValue) : 30;
				INT32 submergedBumpFrameCount = this->GetProperty("waterDesc:submerged:bump frame count", propertyValue) ? boost::get<INT32>(propertyValue) : 1;
				const char *submergedBumpName = this->GetProperty("waterDesc:submerged:bump name", propertyValue) ? boost::get<const char *>(propertyValue) : NULL;
				if(submergedBumpName)
				{
					REAL frameInterval = 1.0f / submergedBumpFrameRate;

					for(INT32 i = 0; i < submergedBumpFrameCount; ++i)
					{
						// Build texture name
						sprintf(tempStr, "%s_%d", submergedBumpName, i);

						// Insert keyframe
						INT32 keyIdx = m_track_SubmergedBumpName.InsertKey(i * frameInterval);
						m_track_SubmergedBumpName.SetKeyByIndex(keyIdx, tempStr);
					}
				}
			}

			break;

		case RuWorld::ruWATERBODYTYPE_EXTENTMARKER:
			{
				m_waterMaterial->SetProperty("material:materialType", "Standard Material");

				m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_NONE);
				m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 0);

				m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, RUCOLOR_ARGB(0, 128, 128, 64));
				m_waterMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(1.0f));
			}

			break;
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
