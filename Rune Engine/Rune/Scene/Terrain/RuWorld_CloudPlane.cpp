#include "../../Scene/Terrain/RuWorld_CloudPlane.h"

#include "../../Engine/Texture/RuTexture_Proxy.h"
#include "../../Core/Noise/RuNoise_Perlin.h"

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_RenderContext.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuWorld_CloudPlane, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_CloudPlane, "CRuWorld_CloudPlane", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuWorld_CloudPlane, 0x6FF7BB3C, 0xC0584f50, 0xBB31C2D8, 0xD8C4DF22)

// ************************************************************************************************************************************************************

CRuWorld_CloudPlane::CRuWorld_CloudPlane()
:	m_numOctaves(0),
	m_baseOctaveSize(1024),
	m_octaveSize(NULL),
	m_octaveScale(NULL),
	m_octaveWeight(NULL),
	m_octaveScrollSpeed(NULL),
	m_octaveScrollMultiplier(NULL),
	m_sunDirection(1.0f, 1.0f, 0.0f),
	m_cloudPlaneMesh(NULL),
	m_cloudPlaneMaterial(NULL),
	m_cloudTexture(NULL),
	m_cloudGenMesh(NULL),
	m_cloudGenMaterial(NULL),
	m_cloudGenModel(NULL),
	m_noiseTextureCount(0),
	m_noiseTextures(NULL),
	m_noiseTextureWeights(NULL),
	m_noiseTextureSizes(NULL)
{
	m_sunDirection.Normalize();

	m_psConstants[0] = 0.40f;		// Cloud cover
	m_psConstants[1] = 0.96f;		// Cloud sharpness

	// Set total octaves to eight
	SetNumOctaves(8);

	// Initialize cloud plane mesh
	InitializeCloudPlaneMesh(300.0f, 60.0f, 0.0f, 40, RuFalloff_Linear);

	m_cloudPlaneMaterial = ruNEW CRuMaterial();
	m_cloudPlaneMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_CLOUD_DOME"));
	m_cloudPlaneMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 0);

	m_cloudPlaneMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_RAW_OFFSET, 27);
	m_cloudPlaneMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_RAW_COUNT, 8);
	m_cloudPlaneMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_RAW_DATA, reinterpret_cast<UINT64>(m_vsConstants));

	m_cloudPlaneMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_RAW_PS_OFFSET, 3);
	m_cloudPlaneMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_RAW_PS_COUNT, 3);
	m_cloudPlaneMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_RAW_PS_DATA, reinterpret_cast<UINT64>(m_psConstants));

	m_cloudPlaneMesh->CalculateBounds();

	this->SetLocalEntityBounds(m_cloudPlaneMesh->GetBounds());

	for(INT32 i = 0; i < 16; ++i)
	{
		m_vsConstants[i] = 0.0f;
	}

	m_vsConstants[16] = 1.0f;
	m_vsConstants[17] = 2.0f;
	m_vsConstants[18] = 8.0f;
	m_vsConstants[19] = 8.0f;
	m_vsConstants[20] = 8.0f;
	m_vsConstants[21] = 8.0f;
	m_vsConstants[22] = 8.0f;
	m_vsConstants[23] = 8.0f;

	// Register event handlers
	Event_Submitted().RegisterHandler(this->GetGUID(), boost::bind(&CRuWorld_CloudPlane::HandleEvent_Submitted, this, _1));
}

CRuWorld_CloudPlane::~CRuWorld_CloudPlane()
{
	delete[] m_octaveSize;
	delete[] m_octaveScale;
	delete[] m_octaveWeight;
	delete[] m_octaveScrollSpeed;
	delete[] m_octaveScrollMultiplier;

	ruSAFE_RELEASE(m_cloudPlaneMesh);
	ruSAFE_RELEASE(m_cloudPlaneMaterial);
	ruSAFE_RELEASE(m_cloudTexture);

	ruSAFE_RELEASE(m_cloudGenMesh);
	ruSAFE_RELEASE(m_cloudGenMaterial);
	ruSAFE_RELEASE(m_cloudGenModel);

	ReleaseNoiseTextures();
}

void CRuWorld_CloudPlane::SetNumOctaves(INT32 numOctaves)
{
	if(numOctaves != 8)
	{
		return;
	}

	if(m_numOctaves != numOctaves)
	{
		// Delete old octave parameter arrays
		delete[] m_octaveSize;
		delete[] m_octaveScale;
		delete[] m_octaveWeight;
		delete[] m_octaveScrollSpeed;
		delete[] m_octaveScrollMultiplier;

		// Store number of octaves
		m_numOctaves = numOctaves;

		// Create new octave parameter arrays
		m_octaveSize = ruNEW INT32 [m_numOctaves];
		m_octaveScale = ruNEW REAL [m_numOctaves];
		m_octaveWeight = ruNEW REAL [m_numOctaves];
		m_octaveScrollSpeed = ruNEW REAL [m_numOctaves];
		m_octaveScrollMultiplier = ruNEW REAL [m_numOctaves * 2];

		// Setup reasonable default values for the octave parameters
		INT32 octaveSize = m_baseOctaveSize;
		REAL octaveScale = 0.010f;
		REAL octaveWeight = 0.5f;

		for(INT32 i = 0, j = 0; i < numOctaves; ++i)
		{
			m_octaveSize[i] = octaveSize;
			m_octaveScale[i] = octaveScale;
			m_octaveWeight[i] = octaveWeight;
			m_octaveScrollSpeed[i] = 0.0f;

			m_octaveScrollMultiplier[j++] = (RuGlobalMTRand().GenerateFloat() < 0.5f) ? 1.0f : -1.0f;
			m_octaveScrollMultiplier[j++] = (RuGlobalMTRand().GenerateFloat() < 0.5f) ? 1.0f : -1.0f;

			if(i <= 2)
			{
				octaveSize = octaveSize / 2;
			}

			octaveScale = octaveScale * 2.0f;
			octaveWeight = octaveWeight / 2.0f;
		}

		// Release old noise textures
		ReleaseNoiseTextures();
	}
}

void CRuWorld_CloudPlane::SetBaseOctaveSize(INT32 baseOctaveSize)
{
	m_baseOctaveSize = baseOctaveSize;
}

void CRuWorld_CloudPlane::SetOctaveSize(INT32 *octaveSize)
{
	BOOL reinitializeRequired = FALSE;

	for(INT32 i = 0; i < m_numOctaves; ++i)
	{
		if(m_octaveSize[i] != octaveSize[i])
		{
			m_octaveSize[i] = octaveSize[i];
			reinitializeRequired = TRUE;
		}
	}

	if(reinitializeRequired)
	{
		// Release old noise textures
		ReleaseNoiseTextures();
	}
}

void CRuWorld_CloudPlane::SetOctaveScale(REAL *octaveScale)
{
	BOOL reinitializeRequired = FALSE;

	for(INT32 i = 0; i < m_numOctaves; ++i)
	{
		if(m_octaveScale[i] != octaveScale[i])
		{
			m_octaveScale[i] = octaveScale[i];
			reinitializeRequired = TRUE;
		}
	}

	if(reinitializeRequired)
	{
		// Release old noise textures
		ReleaseNoiseTextures();
	}
}

void CRuWorld_CloudPlane::SetOctaveWeight(REAL *octaveWeight)
{
	for(INT32 i = 0; i < m_numOctaves; ++i)
	{
		m_octaveWeight[i] = octaveWeight[i];

		// Store octave weight into PS constants array
		m_psConstants[4 + i] = m_octaveWeight[i];
	}
}

void CRuWorld_CloudPlane::SetOctaveScrollSpeed(REAL *octaveScrollSpeed)
{
	for(INT32 i = 0; i < m_numOctaves; ++i)
	{
		m_octaveScrollSpeed[i] = octaveScrollSpeed[i];
	}
}

void CRuWorld_CloudPlane::SetCloudCover(REAL cloudCover)
{
	if(cloudCover >= 0.0f && cloudCover <= 1.0f)
	{
		m_psConstants[0] = cloudCover;
	}
}

void CRuWorld_CloudPlane::SetCloudSharpness(REAL cloudSharpness)
{
	if(cloudSharpness >= 0.0f && cloudSharpness <= 1.0f)
	{
		m_psConstants[1] = cloudSharpness;
	}
}

void CRuWorld_CloudPlane::SetCloudColor0(RUCOLOR cloudColor)
{
	m_vsConstants[24] = ((cloudColor >> 16) & 0xFF) / 255.0f;
	m_vsConstants[25] = ((cloudColor >>  8) & 0xFF) / 255.0f;
	m_vsConstants[26] = ((cloudColor >>  0) & 0xFF) / 255.0f;
	m_vsConstants[27] = ((cloudColor >> 24) & 0xFF) / 255.0f;
}

void CRuWorld_CloudPlane::SetCloudColor1(RUCOLOR cloudColor)
{
	m_vsConstants[28] = ((cloudColor >> 16) & 0xFF) / 255.0f;
	m_vsConstants[29] = ((cloudColor >>  8) & 0xFF) / 255.0f;
	m_vsConstants[30] = ((cloudColor >>  0) & 0xFF) / 255.0f;
	m_vsConstants[31] = ((cloudColor >> 24) & 0xFF) / 255.0f;
}

BOOL CRuWorld_CloudPlane::SetSunDirection(const CRuVector3 &sunDirection)
{
	if(sunDirection.Magnitude() > ruEPSILON)
	{
		// Store normalized sun direction
		m_sunDirection = sunDirection;
		m_sunDirection.Normalize();

		// Shift textures
		ShiftNoiseTextures();

		return TRUE;
	}

	return FALSE;
}

IRuObject *CRuWorld_CloudPlane::Clone(IRuObject *clonedObject)
{
	return NULL;
}

BOOL CRuWorld_CloudPlane::SerializeFrom(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuWorld_CloudPlane::SerializeTo(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuWorld_CloudPlane::Update(float elapsedTime)
{
	// Chain update call to parent class
	if(CRuEntity::Update(elapsedTime) == FALSE)
	{
		return FALSE;
	}

	// Update UV offsets
	for(INT32 i = 0, j = 0; i < m_numOctaves; ++i)
	{
		m_vsConstants[j] += m_octaveScrollSpeed[i] * m_octaveScrollMultiplier[j] * elapsedTime;
		m_vsConstants[j] = fmod(m_vsConstants[j], 1.0f);
		++j;

		m_vsConstants[j] += m_octaveScrollSpeed[i] * m_octaveScrollMultiplier[j] * elapsedTime;
		m_vsConstants[j] = fmod(m_vsConstants[j], 1.0f);
		++j;
	}

	return TRUE;
}

INT32 CRuWorld_CloudPlane::GetNumMeshes() const
{
	return 1;
}

BOOL CRuWorld_CloudPlane::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_cloudPlaneMesh;
		*materialOut = m_cloudPlaneMaterial;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuWorld_CloudPlane::QueryCollision(CRuCollisionQuery *colQuery)
{
	return FALSE;
}

BOOL CRuWorld_CloudPlane::HandleEvent_Submitted(RuEventArgs *eventArgs)
{
	RuEntityEvent_Submitted_Args *submittedEventArgs = static_cast<RuEntityEvent_Submitted_Args *>(eventArgs);
	CRuCamera *camera = submittedEventArgs->m_camera;

	// Create noise textures if necessary
	if(m_noiseTextureCount == 0 || m_noiseTextures == NULL)
	{
		CRuNoise_Perlin *noiseGenerator = ruNEW CRuNoise_Perlin();
		CreateNoiseTextures(m_numOctaves, m_octaveSize, m_octaveScale, m_octaveWeight, noiseGenerator);
		ruSAFE_RELEASE(noiseGenerator);
	}

	// Update uv scale
	if(m_noiseTextures)
	{
		for(INT32 i = 0; i < m_numOctaves; ++i)
		{
			m_vsConstants[16 + i] = m_baseOctaveSize / static_cast<REAL>(m_noiseTextureSizes[i]);
		}
	}

	return TRUE;
}

BOOL CRuWorld_CloudPlane::CreateNoiseTextures(INT32 numOctaves, INT32 *octaveSize, REAL *octaveScale, REAL *octaveWeights, CRuNoise_Base *noiseGenerator)
{
	ReleaseNoiseTextures();

	m_noiseTextureCount = numOctaves;
	m_noiseTextures = ruNEW CRuTexture_Proxy * [numOctaves];
	m_noiseTextureWeights = ruNEW REAL [numOctaves];
	m_noiseTextureSizes = ruNEW INT32 [numOctaves];

	for(INT32 i = 0; i < numOctaves; ++i)
	{
		if(noiseGenerator->GetType() == CRuNoise_Perlin::Type())
		{
			static_cast<CRuNoise_Perlin *>(noiseGenerator)->SetSeed(i + 51);
		}

		// Get texture size for the current octave
		INT32 textureSize = octaveSize[i];

		// Allocate texture from resource manager
		m_noiseTextures[i] = ruNEW CRuTexture_Proxy();
		m_noiseTextures[i]->Allocate(textureSize, textureSize, 1, ruSURFACEFMT_L8);

		char textureName[128];
		sprintf(textureName, "ruWORLD_CLOUD_NOISE_TEXTURE: ");
		m_noiseTextures[i]->GetGUID().ToString(&textureName[strlen(textureName)]);

		m_noiseTextures[i]->SetTextureName(textureName);
		g_ruResourceManager->AddTexture(m_noiseTextures[i]);

		m_noiseTextureWeights[i] = octaveWeights[i];
		m_noiseTextureSizes[i] = textureSize;

		// Store octave weight into PS constants array
		m_psConstants[4 + i] = octaveWeights[i];

		if(m_noiseTextures[i])
		{
			BYTE *dstBits;
			INT32 dstPitch;

			m_noiseTextures[i]->Lock(0, reinterpret_cast<void **>(&dstBits), &dstPitch);

			for(INT32 y = 0; y < textureSize; ++y)
			{
				for(INT32 x = 0; x < textureSize; ++x)
				{
					REAL sx = x * octaveScale[i];
					REAL sy = y * octaveScale[i];
					REAL scaledTextureSize = textureSize * octaveScale[i];

					REAL v0 = noiseGenerator->Evaluate(sx, sy, 0.0f) * (scaledTextureSize - sx) * (scaledTextureSize - sy);
					REAL v1 = noiseGenerator->Evaluate(sx - scaledTextureSize, sy, 0.0f) * (sx) * (scaledTextureSize - sy);
					REAL v2 = noiseGenerator->Evaluate(sx - scaledTextureSize, sy - scaledTextureSize, 0.0f) * (sx) * (sy);
					REAL v3 = noiseGenerator->Evaluate(sx, sy - scaledTextureSize, 0.0f) * (scaledTextureSize - sx) * (sy);

					REAL noiseValue = min(1.0f, max(0.0f, ((v0 + v1 + v2 + v3) / (scaledTextureSize * scaledTextureSize) + 1.0f) * 0.5f));
					BYTE scaledNoiseValue = static_cast<BYTE>(noiseValue * 255.0f);
/*
					// Evaluate noise using current scale
					REAL noiseValue = noiseGenerator->Evaluate(x * octaveScale[i], y * octaveScale[i], 0.0f);

					// Scale noise value
					BYTE scaledNoiseValue = static_cast<BYTE>((noiseValue + 1.0f) * 0.5f * 255.0f);
*/
					// Store
					dstBits[x] = scaledNoiseValue;
//					dstBits[x] = RUCOLOR_ARGB(scaledNoiseValue, scaledNoiseValue, scaledNoiseValue, scaledNoiseValue);
				}

				dstBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(dstBits) + dstPitch);
			}

			m_noiseTextures[i]->Unlock(0);

			m_cloudPlaneMaterial->SetTexture(static_cast<RuTextureChannel>(ruTEXCHANNEL_CUSTOM_0 + i), m_noiseTextures[i]);
		}
	}

	return ShiftNoiseTextures();
}

BOOL CRuWorld_CloudPlane::ReleaseNoiseTextures()
{
	if(m_noiseTextures)
	{
		for(INT32 i = 0; i < m_noiseTextureCount; ++i)
		{
			ruSAFE_RELEASE(m_noiseTextures[i]);
		}

		delete[] m_noiseTextures;
	}

	delete[] m_noiseTextureWeights;
	delete[] m_noiseTextureSizes;

	m_noiseTextureCount = 0;
	m_noiseTextures = NULL;
	m_noiseTextureWeights = NULL;
	m_noiseTextureSizes = NULL;

	return TRUE;
}

BOOL CRuWorld_CloudPlane::ShiftNoiseTextures()
{
/*
	if(m_noiseTextureCount > 0 && m_noiseTextures)
	{
		for(INT32 i = 0; i < m_noiseTextureCount; ++i)
		{
			INT32 textureSize = m_noiseTextures[i]->GetLevelWidth(0);

			INT32 xShift = static_cast<INT32>(m_sunDirection.m_x * (textureSize * 0.0625f));
			INT32 yShift = static_cast<INT32>(m_sunDirection.m_z * (textureSize * 0.0625f));

			INT32 xShiftStep = xShift / 3;
			INT32 yShiftStep = yShift / 3;

			INT32 xShiftSteps[4] = { 0, xShiftStep * 1, xShiftStep * 2, xShiftStep * 3 };
			INT32 yShiftSteps[4] = { 0, yShiftStep * 1, yShiftStep * 2, yShiftStep * 3 };



			DWORD *dstBits;
			INT32 dstPitch;

			m_noiseTextures[i]->Lock(0, reinterpret_cast<void **>(&dstBits), &dstPitch);

			dstPitch /= 4;

			for(INT32 y = 0; y < textureSize; ++y)
			{
				for(INT32 x = 0; x < textureSize; ++x)
				{
					BYTE noiseValues[4];

					for(INT32 j = 0; j < 4; ++j)
					{
						INT32 xMod = ((x + xShiftSteps[j]) + textureSize) % textureSize;
						INT32 yMod = ((y + yShiftSteps[j]) + textureSize) % textureSize;

						noiseValues[j] = (dstBits[xMod + yMod * dstPitch] >> 24) & 0xFF;
					}

					dstBits[x + y * dstPitch] = RUCOLOR_ARGB(noiseValues[0], noiseValues[1], noiseValues[2], noiseValues[3]);
				}
			}

			m_noiseTextures[i]->Unlock(0);			
		}

		return TRUE;
	}
*/
	return FALSE;
}

BOOL CRuWorld_CloudPlane::InitializeCloudPlaneMesh(REAL size, REAL apexHeight, REAL baseHeight, INT32 numSegments, RuFalloffFunction falloff)
{
	INT32 numVerticesPerRow = numSegments + 1;
	INT32 numVertices = numVerticesPerRow * numVerticesPerRow;
	INT32 numPrimitives = numSegments * numSegments * 2;

	m_cloudPlaneMesh = ruNEW CRuMesh();
	m_cloudPlaneMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, numVertices, numPrimitives, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);
	m_cloudPlaneMesh->AddTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);

	CRuMorphTarget *mt = static_cast<CRuMorphTarget *>(m_cloudPlaneMesh->GetMorphTarget(0));
	CRuVector3 *position = mt->GetPosition();
	DWORD *diffuse = mt->GetDiffuse();
	REAL *uv = mt->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);

	REAL segmentGap = size / numSegments;
	REAL planeOffset = size / 2.0f;

	for(INT32 z = 0, i = 0, j = 0; z < numVerticesPerRow; ++z)
	{
		for(INT32 x = 0; x < numVerticesPerRow; ++x)
		{
			REAL tx = ((REAL) x / numSegments) * 2.0f - 1.0f;
			REAL tz = ((REAL) z / numSegments) * 2.0f - 1.0f;

			REAL absTX = fabs(tx);
			REAL absTZ = fabs(tz);

			REAL piTX = ruPI * (1.0f - falloff(absTX, 1.0f)) * 0.5f;
			REAL piTZ = ruPI * (1.0f - falloff(absTZ, 1.0f)) * 0.5f;

			REAL cosTX = cos(piTX);
			REAL cosTZ = cos(piTZ);

			REAL h = cosTX * cosTZ * (apexHeight - baseHeight) + baseHeight;

			position[i].Set(x * segmentGap - planeOffset, h, z * segmentGap - planeOffset);

//			REAL arf = sqrt(position[i].m_x * position[i].m_x + position[i].m_z * position[i].m_z);
			REAL arf = max(fabs(position[i].m_x), fabs(position[i].m_z));
			arf = 1.0f - min(1.0f, arf / planeOffset);
			REAL arf2 = 1.0f - pow(0.90f, arf * 1024.0f);

			BYTE moo = static_cast<BYTE>(arf2 * 255.0f);

			diffuse[i++] = RUCOLOR_ARGB(moo, 0xFF, 0xFF, 0xFF);;

			uv[j++] = (tx + 1.0f) / 2.0f;
			uv[j++] = (tz + 1.0f) / 2.0f;
		}
	}

	INT32 faceConstruction[]	= { 0, 1, 2, 1, 3, 2 };

	INT32 curIndex = 0;
	UINT16 indexSet[4];

	UINT16 *indices = m_cloudPlaneMesh->GetIndices();

	// Build indices for the mesh body
	for(INT32 z = 0; z < numSegments; ++z)
	{
		// Build indices for two faces at a time
		for(INT32 x = 0; x < numSegments; ++x)
		{
			// Calculate the indices used in these four faces
			indexSet[0] = (z * numVerticesPerRow) + x;
			indexSet[1] = indexSet[0] + 1;
			indexSet[2] = indexSet[0] + numVerticesPerRow;
			indexSet[3] = indexSet[2] + 1;

			for(INT32 j = 0; j < 6; ++j)
			{
				indices[curIndex++] = indexSet[faceConstruction[j]];
			}
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
