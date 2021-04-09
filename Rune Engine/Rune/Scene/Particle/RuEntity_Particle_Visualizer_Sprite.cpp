/*!
	@file RuEntity_Particle_Visualizer_Sprite.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Particle/RuEntity_Particle_Visualizer_Sprite.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Particle_Visualizer_Sprite, IRuEntity_Particle_Visualizer)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Particle_Visualizer_Sprite, "CRuEntity_Particle_Visualizer_Sprite", "IRuEntity_Particle_Visualizer")
ruCLASSGUID_DEFINE(CRuEntity_Particle_Visualizer_Sprite, 0xA0EE1154, 0xB6364ba0, 0xAD532783, 0x1B88EA31)

// ************************************************************************************************************************************************************

CRuEntity_Particle_Visualizer_Sprite::CRuEntity_Particle_Visualizer_Sprite()
:	m_allowWorldRotation(FALSE),
	m_orientation(ruAXIS_Z_POSITIVE),
	m_billboardType(ruBILLBOARDTYPE_UNCONSTRAINED),
	m_blendType(ruALPHAMODE_NORMAL),
	m_spriteTextureName(NULL),
	m_spriteTexture(NULL),
	m_spriteSubTextureWidth(16),
	m_spriteSubTextureHeight(16),
	m_frameRate(1.0f),
	m_numFrames(1),
	m_randomRotation(FALSE),
	m_quadScale(1.0f, 1.0f, 0.0f),
	m_emitter(NULL),
	m_maxParticlesPerBatch(70),
	m_timeSinceLastUpdate(1000000.0f),
	m_updateInterval(0.010f),
	m_mesh(NULL),
	m_iterationCount(0)
{
	// Create model
	m_model = ruNEW CRuModel();

	// Disable automatic cloning and serialization
	m_model->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
	m_model->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);

	// Disable scene hierarchy role
	m_model->SetOption(ruENTITYOPTION_HRDISABLE, TRUE);

	m_model->SetProperty("act:no enumerate", TRUE);

	this->AddChild(m_model);

	// Create mesh to use for rendering particle batches
	BuildMesh();

	CRuColor4 color(1.0f, 1.0f, 1.0f, 1.0f);
	m_track_Color.InsertKey(0.0f, color);
	m_track_Color.InsertKey(1.0f, color);

	REAL alpha = 1.0f;
	m_track_Alpha.InsertKey(0.0f, alpha);
	m_track_Alpha.InsertKey(1.0f, alpha);

	REAL scale = 1.0f;
	m_track_Scale.InsertKey(0.0f, scale);
	m_track_Scale.InsertKey(1.0f, scale);

	REAL speed = 0.0f;
	m_track_RotationSpeed.InsertKey(0.0f, speed);
	m_track_RotationSpeed.InsertKey(1.0f, speed);

	// Disable collision detection by default
	this->SetOption(ruENTITYOPTION_COLLISION, FALSE);

	// Register event handlers
	Event_Transform().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_Particle_Visualizer_Sprite::HandleEvent_Transform, this, _1));

	// Create performance probes
	m_probe_UpdateCount = g_ruProfiler->CreateProbe("PrtSysSpr Upd", "Misc");
	m_probe_ValidUpdateCount = g_ruProfiler->CreateProbe("PrtSysSpr Upd OK", "Misc");
}

CRuEntity_Particle_Visualizer_Sprite::~CRuEntity_Particle_Visualizer_Sprite()
{
	// Clear all batches
	ClearBatches();

	// Release particle emitter
	if(m_emitter && m_emitter->IsEmbeddedEmitter())
	{
		ruSAFE_RELEASE(m_emitter);
	}

	// Release model
	ruSAFE_RELEASE(m_model);

	// Release mesh
	ruSAFE_RELEASE(m_mesh);

	// Release texture name string
	delete[] m_spriteTextureName;

	// Release sprite texture
	ruSAFE_RELEASE(m_spriteTexture);

	// Release resources used by batch structures
	while(m_freeBatchDescStack.IsEmpty() == FALSE)
	{
		delete m_freeBatchDescStack.Pop();
	}
}

IRuObject *CRuEntity_Particle_Visualizer_Sprite::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_Particle_Visualizer_Sprite();

	// Chain cloning call to parent class
	IRuEntity_Particle_Visualizer::Clone(clonedObject);

	// Shortcut reference to the cloned entity
	CRuEntity_Particle_Visualizer_Sprite *clonedPSys = static_cast<CRuEntity_Particle_Visualizer_Sprite *>(clonedObject);

	// Copy visualizer settings
	clonedPSys->m_allowWorldRotation = m_allowWorldRotation;
	clonedPSys->m_orientation = m_orientation;
	clonedPSys->m_billboardType = m_billboardType;
	clonedPSys->m_blendType = m_blendType;

	if(m_spriteTextureName)
	{
		clonedPSys->m_spriteTextureName = ruNEW char [strlen(m_spriteTextureName) + 1];
		strcpy(clonedPSys->m_spriteTextureName, m_spriteTextureName);
	}

	if(m_spriteTexture)
	{
		clonedPSys->m_spriteTexture = m_spriteTexture;
		clonedPSys->m_spriteTexture->AddRef();
	}

	clonedPSys->m_spriteSubTextureWidth = m_spriteSubTextureWidth;
	clonedPSys->m_spriteSubTextureHeight = m_spriteSubTextureHeight;
	clonedPSys->m_frameRate = m_frameRate;
	clonedPSys->m_numFrames = m_numFrames;
	clonedPSys->m_orientToDirectionOfTravel = m_orientToDirectionOfTravel;
	clonedPSys->m_randomRotation = m_randomRotation;
	clonedPSys->m_quadOffset = m_quadOffset;
	clonedPSys->m_quadScale = m_quadScale;

	m_track_Color.CopyTo(&clonedPSys->m_track_Color);
	m_track_Alpha.CopyTo(&clonedPSys->m_track_Alpha);
	m_track_Scale.CopyTo(&clonedPSys->m_track_Scale);
	m_track_RotationSpeed.CopyTo(&clonedPSys->m_track_RotationSpeed);

	clonedPSys->m_maxParticlesPerBatch = m_maxParticlesPerBatch;

	// Clone the emitter
	if(m_emitter && m_emitter->IsEmbeddedEmitter())
	{
		IRuObject *clonedEmitter = m_emitter->Clone();
		if(clonedEmitter && clonedEmitter->GetType().IsSubClassOf(IRuEntity_Particle_Emitter::Type()))
		{
			clonedPSys->m_emitter = static_cast<IRuEntity_Particle_Emitter *>(clonedEmitter);

			// Disable automatic cloning and serialization
			clonedPSys->m_emitter->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
			clonedPSys->m_emitter->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
			clonedPSys->m_emitter->SetProperty("act:no enumerate", TRUE);

			clonedPSys->AddChild(clonedPSys->m_emitter);
		}
	}

	return clonedPSys;
}

BOOL CRuEntity_Particle_Visualizer_Sprite::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;
	BOOL emitterAvailable = FALSE;

	// Legacy stuff
	DWORD startColor;
	DWORD endColor;
	REAL startScale;
	REAL endScale;
	REAL minRotationSpeed = 0.0f;
	REAL maxRotationSpeed = 0.0f;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	if(version >= 6)
	{
		// Chain serialization to base class
		IRuEntity_Particle_Visualizer::SerializeFrom(stream);
	}

	switch(version)
	{
		case 1:
			{
				// Read blend settings
				stream->Read(&m_blendType, sizeof(INT32));

				// Read length of texture name including NULL, followed by the name
				INT32 len = 0;
				stream->Read(&len, sizeof(INT32));
				m_spriteTextureName = ruNEW char [len];
				stream->Read(m_spriteTextureName, len);

				// Read texture settings
				stream->Read(&m_spriteSubTextureWidth, sizeof(INT32));
				stream->Read(&m_spriteSubTextureHeight, sizeof(INT32));
				stream->Read(&m_frameRate, sizeof(REAL));
				stream->Read(&m_numFrames, sizeof(INT32));

				// Read color settings
				stream->Read(&startColor, sizeof(DWORD));
				stream->Read(&endColor, sizeof(DWORD));
				stream->Read(&startScale, sizeof(REAL));
				stream->Read(&endScale, sizeof(REAL));

				// If particle emitter is available, serialize particle emitter
				stream->Read(&emitterAvailable, sizeof(BOOL));
			}

			break;

		case 2:
			{
				// Read blend settings
				stream->Read(&m_blendType, sizeof(INT32));

				// Read length of texture name including NULL, followed by the name
				INT32 len = 0;
				stream->Read(&len, sizeof(INT32));
				m_spriteTextureName = ruNEW char [len];
				stream->Read(m_spriteTextureName, len);

				// Read texture settings
				stream->Read(&m_spriteSubTextureWidth, sizeof(INT32));
				stream->Read(&m_spriteSubTextureHeight, sizeof(INT32));
				stream->Read(&m_frameRate, sizeof(REAL));
				stream->Read(&m_numFrames, sizeof(INT32));

				// Read color settings
				stream->Read(&startColor, sizeof(DWORD));
				stream->Read(&endColor, sizeof(DWORD));
				stream->Read(&startScale, sizeof(REAL));
				stream->Read(&endScale, sizeof(REAL));
				stream->Read(&minRotationSpeed, sizeof(REAL));
				stream->Read(&maxRotationSpeed, sizeof(REAL));

				// If particle emitter is available, serialize particle emitter
				stream->Read(&emitterAvailable, sizeof(BOOL));
			}

			break;

		case 3:
			{
				// Read orientation
				stream->Read(&m_orientation, sizeof(RuBillboardType));

				// Read billboard settings
				stream->Read(&m_billboardType, sizeof(RuBillboardType));

				// Read blend settings
				stream->Read(&m_blendType, sizeof(INT32));

				// Read length of texture name including NULL, followed by the name
				INT32 len = 0;
				stream->Read(&len, sizeof(INT32));
				m_spriteTextureName = ruNEW char [len];
				stream->Read(m_spriteTextureName, len);

				// Read texture settings
				stream->Read(&m_spriteSubTextureWidth, sizeof(INT32));
				stream->Read(&m_spriteSubTextureHeight, sizeof(INT32));
				stream->Read(&m_frameRate, sizeof(REAL));
				stream->Read(&m_numFrames, sizeof(INT32));

				// Read color settings
				stream->Read(&startColor, sizeof(DWORD));
				stream->Read(&endColor, sizeof(DWORD));
				stream->Read(&startScale, sizeof(REAL));
				stream->Read(&endScale, sizeof(REAL));
				stream->Read(&minRotationSpeed, sizeof(REAL));
				stream->Read(&maxRotationSpeed, sizeof(REAL));

				// If particle emitter is available, serialize particle emitter
				stream->Read(&emitterAvailable, sizeof(BOOL));
			}

			break;

		case 4:
		case 5:	// NOTE: Versions 4 and 5 are identical except rotation speed is now interpreted as degrees instead of radians
		case 6:	// NOTE: Version 6 is identical to 5 except for object serialization chaining
			{
				// Read orientation
				stream->Read(&m_orientation, sizeof(RuBillboardType));

				// Read billboard settings
				stream->Read(&m_billboardType, sizeof(RuBillboardType));

				// Read blend settings
				stream->Read(&m_blendType, sizeof(INT32));

				// Read length of texture name including NULL, followed by the name
				INT32 len = 0;
				stream->Read(&len, sizeof(INT32));
				m_spriteTextureName = ruNEW char [len];
				stream->Read(m_spriteTextureName, len);

				// Read texture settings
				stream->Read(&m_spriteSubTextureWidth, sizeof(INT32));
				stream->Read(&m_spriteSubTextureHeight, sizeof(INT32));
				stream->Read(&m_frameRate, sizeof(REAL));
				stream->Read(&m_numFrames, sizeof(INT32));

				// Read other settings
				stream->Read(&m_randomRotation, sizeof(BOOL));

				// Serialize tracks
				m_track_Color.SerializeFrom(stream);
				m_track_Alpha.SerializeFrom(stream);
				m_track_Scale.SerializeFrom(stream);
				m_track_RotationSpeed.SerializeFrom(stream);

				// If particle emitter is available, serialize particle emitter
				stream->Read(&emitterAvailable, sizeof(BOOL));
			}

			break;
	}

	if(version < 4)
	{
		m_track_Color.SetKeyByIndex(0, CRuColor4(startColor));
		m_track_Color.SetKeyByIndex(1, CRuColor4(endColor));

		m_track_Alpha.SetKeyByIndex(0, CRuColor4(startColor).m_a);
		m_track_Alpha.SetKeyByIndex(1, CRuColor4(endColor).m_a);

		m_track_Scale.SetKeyByIndex(0, startScale);
		m_track_Scale.SetKeyByIndex(1, endScale);

		m_track_RotationSpeed.SetKeyByIndex(0, minRotationSpeed);
		m_track_RotationSpeed.SetKeyByIndex(1, minRotationSpeed);
	}

	if(version < 5)
	{
		// Convert PI to degrees
		for(INT32 i = 0; i < m_track_RotationSpeed.GetNumKeys(); ++i)
		{
			REAL rotation;
			m_track_RotationSpeed.GetKeyByIndex(i, &rotation);
			rotation = rotation * 360.0f / (2.0f * ruPI);
			m_track_RotationSpeed.SetKeyByIndex(i, rotation);
		}
	}

	if(emitterAvailable)
	{
		IRuObject *object = RuObjectFactory().CreateObject(stream);
		if(object && object->GetType().IsSubClassOf(IRuEntity_Particle_Emitter::Type()))
		{
			m_emitter = static_cast<IRuEntity_Particle_Emitter *>(object);

			// Disable automatic cloning and serialization
			m_emitter->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
			m_emitter->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
			m_emitter->SetProperty("act:no enumerate", TRUE);

			this->AddChild(m_emitter);
		}
		else
		{
			ruSAFE_RELEASE(object);
		}
	}

	// Reload texture
	ruSAFE_RELEASE(m_spriteTexture);
	m_spriteTexture = g_ruResourceManager->LoadTexture(m_spriteTextureName, ruSURFACEFMT_DEFAULT, 0, TRUE);

	// Reset orientation
	SetOrientation(m_orientation);

	return TRUE;
}

BOOL CRuEntity_Particle_Visualizer_Sprite::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 6;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Particle_Visualizer::SerializeTo(stream);

	// Write orientation
	stream->Write(&m_orientation, sizeof(RuBillboardType));

	// Write billboard settings
	stream->Write(&m_billboardType, sizeof(RuBillboardType));

	// Write blend settings
	stream->Write(&m_blendType, sizeof(INT32));

	// Write length of texture name including NULL, followed by the name
	if(m_spriteTextureName)
	{
		INT32 len = (INT32)strlen(m_spriteTextureName) + 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write(m_spriteTextureName, len);
	}
	else
	{
		INT32 len = 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write("", len);
	}

	// Write texture settings
	stream->Write(&m_spriteSubTextureWidth, sizeof(INT32));
	stream->Write(&m_spriteSubTextureHeight, sizeof(INT32));
	stream->Write(&m_frameRate, sizeof(REAL));
	stream->Write(&m_numFrames, sizeof(INT32));

	// Write other settings
	stream->Write(&m_randomRotation, sizeof(BOOL));

	// Serialize tracks
	m_track_Color.SerializeTo(stream);
	m_track_Alpha.SerializeTo(stream);
	m_track_Scale.SerializeTo(stream);
	m_track_RotationSpeed.SerializeTo(stream);

	// If particle emitter is available, serialize particle emitter
	if(m_emitter && m_emitter->IsEmbeddedEmitter())
	{
		BOOL emitterAvailable = TRUE;
		stream->Write(&emitterAvailable, sizeof(BOOL));
		m_emitter->SerializeTo(stream);
	}
	else
	{
		BOOL emitterAvailable = FALSE;
		stream->Write(&emitterAvailable, sizeof(BOOL));
	}

	return TRUE;
}

BOOL CRuEntity_Particle_Visualizer_Sprite::ResetStates()
{
	return IRuEntity_Particle_Visualizer::ResetStates();
}

BOOL CRuEntity_Particle_Visualizer_Sprite::Update(REAL elapsedTime)
{
	RUPROFILE_SAMPLE_ACCUMULATE(m_probe_UpdateCount, 6, 1);

	// If the particle system was not visible during last render flush, or if it is marked invisible, simply return
	if(m_iterationCount && (this->GetStatusFlag(ruENTITYSTATUS_VISIBLE) == FALSE || this->GetOption(ruENTITYOPTION_HIDDEN) != false))
	{
		return TRUE;
	}

	RUPROFILE_SAMPLE_ACCUMULATE(m_probe_ValidUpdateCount, 6, 1);

	// Rebuild particle batches at specified intervals
	m_timeSinceLastUpdate += elapsedTime;
	if(m_timeSinceLastUpdate >= m_updateInterval)
	{
		// Reset update counter
		m_timeSinceLastUpdate = 0.0f;

		// Chain update to base class
		BOOL retVal = IRuEntity_Particle_Visualizer::Update(elapsedTime);

		// Build the batch
		BuildBatches();

		if(m_emitter)
		{
			if(m_emitter->GetFrameType() == ruPRTFRAMETYPE_WORLD)
			{
				m_model->SetOption(ruENTITYOPTION_TRANSFORMMODE, ruENTITYTRANSFORMMODE_OVERRIDE);
			}
			else
			{
				m_model->SetOption(ruENTITYOPTION_TRANSFORMMODE, ruENTITYTRANSFORMMODE_NORMAL);
			}
		}

		// Increment iteration count
		++m_iterationCount;

		return retVal;
	}

	return TRUE;
}

BOOL CRuEntity_Particle_Visualizer_Sprite::QueryCollision(CRuCollisionQuery *colQuery)
{
	// Chain collision query to the emitter
	if(m_emitter && m_emitter->IsEmbeddedEmitter())
	{
		return m_emitter->QueryCollision(colQuery);
	}

	return TRUE;
}

INT32 CRuEntity_Particle_Visualizer_Sprite::GetNumEntities()
{
	return 1;
}

CRuEntity *CRuEntity_Particle_Visualizer_Sprite::GetEntity(INT32 index)
{
	return m_model;
}

IRuEntity_Particle_Emitter *CRuEntity_Particle_Visualizer_Sprite::GetEmitter()
{
	return m_emitter;
}

BOOL CRuEntity_Particle_Visualizer_Sprite::SetEmitter(IRuEntity_Particle_Emitter *emitter)
{
	if(emitter)
	{
		if(emitter->IsEmbeddedEmitter())
		{
			this->RemoveChild(m_emitter);

			// Release original emitter and store reference to new emitter
			ruSAFE_RELEASE(m_emitter);
			m_emitter = emitter;
			m_emitter->AddRef();

			// Disable automatic cloning and serialization
			m_emitter->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
			m_emitter->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
			m_emitter->SetProperty("act:no enumerate", TRUE);

			this->AddChild(m_emitter);
		}
		else
		{
			m_emitter = emitter;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_Particle_Visualizer_Sprite::HandleEvent_Transform(RuEventArgs *eventArgs)
{
	RuEntityEvent_Transform_Args *transformArgs = static_cast<RuEntityEvent_Transform_Args *>(eventArgs);

	// Store broadcast world transform matrix
	m_broadcastWorldMatrix = *(transformArgs->m_worldTransform);

	// Store broadcast view transform matrix
	m_broadcastViewMatrix = *(transformArgs->m_viewTransform);

	return TRUE;
}

void CRuEntity_Particle_Visualizer_Sprite::SetUpdateInterval(REAL updateInterval)
{
	m_updateInterval = updateInterval;
}

void CRuEntity_Particle_Visualizer_Sprite::ResetUpdateTimer()
{
	m_timeSinceLastUpdate = 100000.0f;
}

void CRuEntity_Particle_Visualizer_Sprite::SetAllowWorldRotation(BOOL allowWorldRotation)
{
	m_allowWorldRotation = allowWorldRotation;
}

RuOrthogonalAxis CRuEntity_Particle_Visualizer_Sprite::GetOrientation() const
{
	return m_orientation;
}

void CRuEntity_Particle_Visualizer_Sprite::SetOrientation(RuOrthogonalAxis orientation)
{
	m_orientation = orientation;

	BuildMesh();
}

RuBillboardType CRuEntity_Particle_Visualizer_Sprite::GetBillboardType() const
{
	return m_billboardType;
}

void CRuEntity_Particle_Visualizer_Sprite::SetBillboardType(RuBillboardType billboardType)
{
	m_billboardType = billboardType;
}

RuOrthogonalAxis CRuEntity_Particle_Visualizer_Sprite::GetBillboardAxis() const
{
	return m_billboardAxis;
}

void CRuEntity_Particle_Visualizer_Sprite::SetBillboardAxis(RuOrthogonalAxis billboardAxis)
{
	m_billboardAxis = billboardAxis;
}

RuAlphaMode CRuEntity_Particle_Visualizer_Sprite::GetBlendType() const
{
	return m_blendType;
}

void CRuEntity_Particle_Visualizer_Sprite::SetBlendType(RuAlphaMode blendType)
{
	m_blendType = blendType;
}

const char *CRuEntity_Particle_Visualizer_Sprite::GetTextureName()
{
	if(m_spriteTextureName)
	{
		return m_spriteTextureName;
	}

	return "";
}

void CRuEntity_Particle_Visualizer_Sprite::SetTextureName(const char *textureName)
{
	if(textureName)
	{
		if(m_spriteTextureName)
		{
			delete[] m_spriteTextureName;
		}

		m_spriteTextureName = ruNEW char [strlen(textureName) + 1];
		strcpy(m_spriteTextureName, textureName);

		// Reload texture
		ruSAFE_RELEASE(m_spriteTexture);
		m_spriteTexture = g_ruResourceManager->LoadTexture(m_spriteTextureName, ruSURFACEFMT_DEFAULT, 0, TRUE);
	}
}

INT32 CRuEntity_Particle_Visualizer_Sprite::GetSubTextureWidth() const
{
	return m_spriteSubTextureWidth;
}

void CRuEntity_Particle_Visualizer_Sprite::SetSubTextureWidth(INT32 width)
{
	if(width > 0)
	{
		m_spriteSubTextureWidth = width;

		// Force rebuild of the mesh
		BuildMesh();
	}
}

INT32 CRuEntity_Particle_Visualizer_Sprite::GetSubTextureHeight() const
{
	return m_spriteSubTextureHeight;
}

void CRuEntity_Particle_Visualizer_Sprite::SetSubTextureHeight(INT32 height)
{
	if(height > 0)
	{
		m_spriteSubTextureHeight = height;

		// Force rebuild of the mesh
		BuildMesh();
	}
}

REAL CRuEntity_Particle_Visualizer_Sprite::GetFrameRate() const
{
	return m_frameRate;
}

void CRuEntity_Particle_Visualizer_Sprite::SetFrameRate(REAL frameRate)
{
	if(frameRate > 0.0f)
	{
		m_frameRate = frameRate;

		// Force rebuild of the mesh
		BuildMesh();
	}
}

INT32 CRuEntity_Particle_Visualizer_Sprite::GetNumFrames() const
{
	return m_numFrames;
}

void CRuEntity_Particle_Visualizer_Sprite::SetNumFrames(INT32 numFrames)
{
	if(numFrames > 0)
	{
		m_numFrames = numFrames;
	}
}

BOOL CRuEntity_Particle_Visualizer_Sprite::GetOrientToDirectionOfTravel() const
{
	return m_orientToDirectionOfTravel;
}

void CRuEntity_Particle_Visualizer_Sprite::SetOrientToDirectionOfTravel(BOOL orientToDirectionOfTravel)
{
	m_orientToDirectionOfTravel = orientToDirectionOfTravel;
}

BOOL CRuEntity_Particle_Visualizer_Sprite::GetRandomRotation() const
{
	return m_randomRotation;
}

void CRuEntity_Particle_Visualizer_Sprite::SetRandomRotation(BOOL randomRotation)
{
	m_randomRotation = randomRotation;
}

const CRuVector3 &CRuEntity_Particle_Visualizer_Sprite::GetQuadOffset() const
{
	return m_quadOffset;
}

void CRuEntity_Particle_Visualizer_Sprite::SetQuadOffset(const CRuVector3 &quadOffset)
{
	m_quadOffset = quadOffset;

	// Force rebuild of mesh
	BuildMesh();
}

const CRuVector3 &CRuEntity_Particle_Visualizer_Sprite::GetQuadScale() const
{
	return m_quadScale;
}

void CRuEntity_Particle_Visualizer_Sprite::SetQuadScale(const CRuVector3 &quadScale)
{
	m_quadScale = quadScale;

	// Force rebuild of mesh
	BuildMesh();
}

CRuAnimTrack<CRuColor4> &CRuEntity_Particle_Visualizer_Sprite::GetTrack_Color()
{
	return m_track_Color;
}

CRuAnimTrack<REAL> &CRuEntity_Particle_Visualizer_Sprite::GetTrack_Alpha()
{
	return m_track_Alpha;
}

CRuAnimTrack<REAL> &CRuEntity_Particle_Visualizer_Sprite::GetTrack_Scale()
{
	return m_track_Scale;
}

CRuAnimTrack<REAL> &CRuEntity_Particle_Visualizer_Sprite::GetTrack_RotationSpeed()
{
	return m_track_RotationSpeed;
}

DWORD CRuEntity_Particle_Visualizer_Sprite::GetStartColor() const
{
	CRuColor4 color;
	m_track_Color.GetKeyByIndex(0, &color);
	return color.GetARGB();
}

void CRuEntity_Particle_Visualizer_Sprite::SetStartColor(DWORD color)
{
	m_track_Color.SetKeyByIndex(0, CRuColor4(color));
}

DWORD CRuEntity_Particle_Visualizer_Sprite::GetEndColor() const
{
	CRuColor4 color;
	m_track_Color.GetKeyByIndex(m_track_Color.GetNumKeys() - 1, &color);
	return color.GetARGB();
}

void CRuEntity_Particle_Visualizer_Sprite::SetEndColor(DWORD color)
{
	m_track_Color.SetKeyByIndex(m_track_Color.GetNumKeys() - 1, CRuColor4(color));
}

REAL CRuEntity_Particle_Visualizer_Sprite::GetStartAlpha() const
{
	REAL alpha;
	m_track_Alpha.GetKeyByIndex(0, &alpha);
	return alpha;
}

void CRuEntity_Particle_Visualizer_Sprite::SetStartAlpha(REAL alpha)
{
	m_track_Alpha.SetKeyByIndex(0, alpha);
}

REAL CRuEntity_Particle_Visualizer_Sprite::GetEndAlpha() const
{
	REAL alpha;
	m_track_Alpha.GetKeyByIndex(m_track_Alpha.GetNumKeys() - 1, &alpha);
	return alpha;
}

void CRuEntity_Particle_Visualizer_Sprite::SetEndAlpha(REAL alpha)
{
	m_track_Alpha.SetKeyByIndex(m_track_Alpha.GetNumKeys() - 1, alpha);
}

REAL CRuEntity_Particle_Visualizer_Sprite::GetStartScale() const
{
	REAL scale;
	m_track_Scale.GetKeyByIndex(0, &scale);
	return scale;
}

void CRuEntity_Particle_Visualizer_Sprite::SetStartScale(REAL scale)
{
	m_track_Scale.SetKeyByIndex(0, scale);
}

REAL CRuEntity_Particle_Visualizer_Sprite::GetEndScale() const
{
	REAL scale;
	m_track_Scale.GetKeyByIndex(m_track_Scale.GetNumKeys() - 1, &scale);
	return scale;
}

void CRuEntity_Particle_Visualizer_Sprite::SetEndScale(REAL scale)
{
	m_track_Scale.SetKeyByIndex(m_track_Scale.GetNumKeys() - 1, scale);
}

REAL CRuEntity_Particle_Visualizer_Sprite::GetRotationSpeed() const
{
	REAL speed;
	m_track_RotationSpeed.GetKeyByIndex(0, &speed);
	return speed;
}

void CRuEntity_Particle_Visualizer_Sprite::SetRotationSpeed(REAL speed)
{
	m_track_RotationSpeed.SetKeyByIndex(0, speed);
	m_track_RotationSpeed.SetKeyByIndex(m_track_RotationSpeed.GetNumKeys() - 1, speed);
}

void CRuEntity_Particle_Visualizer_Sprite::BuildMesh()
{
	// Build a descriptor string for the required mesh
	char meshDescriptorStr[1024];
	sprintf(meshDescriptorStr, "0xA0EE1154, 0xB6364ba0, 0xAD532783, 0x1B88EA31");

	// Attempt to get an existing mesh from the resource manager
	ruSAFE_RELEASE(m_mesh);
	m_mesh = static_cast<CRuMesh *>(g_ruResourceManager->GetGenericObject(meshDescriptorStr));

	// Create a new mesh if there are no compatible meshes available
	if(m_mesh == NULL)
	{
		// Create mesh object
		m_mesh = ruNEW CRuMesh();
		m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, 4 * m_maxParticlesPerBatch, 2 * m_maxParticlesPerBatch, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

		// Insert mesh object into generic dictionary
		g_ruResourceManager->AddGenericObject(meshDescriptorStr, m_mesh);

		// Construct particle mesh geometry
		CRuVector3 *position = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
		DWORD *diffuse = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetDiffuse();
		UINT16 *indices = m_mesh->GetIndices();

		for(INT32 i = 0, vIdx = 0, fIdx = 0; i < m_maxParticlesPerBatch; ++i)
		{
			// v0
			diffuse[vIdx] = (i << 24) | (0x00 << 16) | (0x00 << 8);
			position[vIdx].Set(0.0f, 1.0f, -1.0f);
			++vIdx;

			// v1
			diffuse[vIdx] = (i << 24) | (0x00 << 16) | (0xFF << 8);
			position[vIdx].Set(0.0f, 1.0f, 1.0f);
			++vIdx;

			// v2
			diffuse[vIdx] = (i << 24) | (0xFF << 16) | (0xFF << 8);
			position[vIdx].Set(0.0f, -1.0f, 1.0f);
			++vIdx;

			// v3
			diffuse[vIdx] = (i << 24) | (0xFF << 16) | (0x00 << 8);
			position[vIdx].Set(0.0f, -1.0f, -1.0f);
			++vIdx;

			INT32 vertexOffset = i * 4;

			indices[fIdx++] = vertexOffset + 0;
			indices[fIdx++] = vertexOffset + 2;
			indices[fIdx++] = vertexOffset + 1;

			indices[fIdx++] = vertexOffset + 0;
			indices[fIdx++] = vertexOffset + 3;
			indices[fIdx++] = vertexOffset + 2;
		}
	}
}

void CRuEntity_Particle_Visualizer_Sprite::BuildBatches()
{
	// Abort if emitter is not available
	if(m_emitter == NULL)
		return;

	RuOrthogonalAxis emitterOrientation = m_emitter->GetEmitterOrientation();

	// Update aggregate matrix
	m_aggregateMatrix = m_broadcastWorldMatrix * m_broadcastViewMatrix;

	// Clear existing batches
	ClearBatches();

	// If texture exists but has not yet been loaded, abort
	if(m_spriteTexture && m_spriteTexture->IsInvalidated())
	{
		return;
	}

	// Decompose orientation transform
	const CRuMatrix4x4 &worldTransform = this->GetWorldTransform();
	CRuVector3 wScale, wShear, wRotation, wTranslation;
	worldTransform.GetDecomposition(wScale, wShear, wRotation, wTranslation);

	// Calculate frame period
	REAL framePeriod = 1.0f / m_frameRate;

	// Get particle list from the emitter
	CRuArrayList<CRuParticleDesc> *particles = m_emitter->GetParticleDescriptors();

	CRuSphere meshBounds;;

	// Iterate through all particles and build batches
	BatchDesc *batch = NULL;
	INT32 particlesThisBatch = 0;
	INT32 currentTextureWidth = 0;
	INT32 currentTextureHeight = 0;
	for(INT32 i = 0, j = 24, firstParticle = 0; i < particles->Count(); ++i)
	{
		// Store reference to the particle
		const CRuParticleDesc &particle = (*particles)[i];

		// Skip inactive particles
		if(particle.m_active == FALSE)
			continue;

		// Make sure run-time particles list is at least as large as the emitter particle list
		while(i >= m_particles.Count())
		{
			m_particles.Add(ParticleDesc());
		}

		// Detect if there is a need for a new batch
		if(batch == NULL || particlesThisBatch >= m_maxParticlesPerBatch)
		{
			// Set number of particles for the previous batch if applicable
			if(batch)
			{
				// Set rendering parameters
				batch->m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, particlesThisBatch);
				batch->m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, m_blendType);

				// Setup rendering batch
				batch->m_mesh->SetProxiedMesh(m_mesh, 0, particlesThisBatch * 2);
				batch->m_mesh->SetBounds(meshBounds);
			}

			// Get a free batch descriptor
			if(m_freeBatchDescStack.IsEmpty())
			{
				batch = ruNEW BatchDesc(m_maxParticlesPerBatch);
			}
			else
			{
				batch = m_freeBatchDescStack.Pop();
			}

			// Select texture
			if(m_spriteTexture)
			{
				batch->m_material->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, m_spriteTexture);

				currentTextureWidth = m_spriteTexture->GetLevelWidth(0);
				currentTextureHeight = m_spriteTexture->GetLevelHeight(0);


				// Setup header constants
				batch->m_vsConstants[0] = (REAL) m_spriteSubTextureWidth / m_spriteTexture->GetLevelWidth(0);
				batch->m_vsConstants[1] = (REAL) m_spriteSubTextureHeight / m_spriteTexture->GetLevelHeight(0);
			}
			else
			{
				batch->m_material->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, NULL);

				currentTextureWidth = 16;
				currentTextureHeight = 16;

				// Setup header constants
				batch->m_vsConstants[0] = 0.0f;
				batch->m_vsConstants[1] = 0.0f;
			}

			REAL aspectRatio = m_spriteSubTextureWidth > 0.0f ? (REAL) m_spriteSubTextureHeight / m_spriteSubTextureWidth : 1.0f;
			batch->m_vsConstants[2] = aspectRatio;
			batch->m_vsConstants[3] = m_allowWorldRotation ? 1.0f : 0.0f;

			batch->m_vsConstants[4] = m_quadScale.m_x * 0.5f;
			batch->m_vsConstants[5] = m_quadScale.m_y * 0.5f;
			batch->m_vsConstants[6] = m_quadOffset.m_x;
			batch->m_vsConstants[7] = m_quadOffset.m_y;

			// Orientation transform
			CRuMatrix4x4 orientationTransform;
			orientationTransform.SetToIdentity();

			switch(m_orientation)
			{
				case ruAXIS_X_POSITIVE:
					// Do nothing
					break;

				case ruAXIS_X_NEGATIVE:
					orientationTransform.Rotate(CRuVector3(0.0f, 1.0f, 0.0f), ruPI);
					break;

				case ruAXIS_Y_POSITIVE:
					orientationTransform.Rotate(CRuVector3(0.0f, 0.0f, 1.0f), ruPI * -0.5f);
					break;

				case ruAXIS_Y_NEGATIVE:
					orientationTransform.Rotate(CRuVector3(0.0f, 0.0f, 1.0f), ruPI * 0.5f);
					break;

				case ruAXIS_Z_POSITIVE:
					orientationTransform.Rotate(CRuVector3(0.0f, 1.0f, 0.0f), ruPI * -0.5f);
					orientationTransform.Rotate(CRuVector3(0.0f, 0.0f, 1.0f), ruPI * 0.5f);

					// HACK!!! We invert the offset for the Z orientation mode only... it's different from the original implementation because we use rotations to get the quad into place now
					batch->m_vsConstants[7] = m_quadOffset.m_y * -1.0f;
					break;

				case ruAXIS_Z_NEGATIVE:
					orientationTransform.Rotate(CRuVector3(0.0f, 1.0f, 0.0f), ruPI * 0.5f);
					orientationTransform.Rotate(CRuVector3(0.0f, 0.0f, 1.0f), ruPI * -0.5f);

					// HACK!!! We invert the offset for the Z orientation mode only... it's different from the original implementation because we use rotations to get the quad into place now
					batch->m_vsConstants[7] = m_quadOffset.m_y * -1.0f;
					break;
			}

			for(UINT32 i = 0, k = 8; i < 3; ++i)
			{
				for(UINT32 j = 0; j < 4; ++j, ++k)
				{
					batch->m_vsConstants[k] = orientationTransform.m_elements[j][i];
				}
			}
/*
			// THESE ARE NOT USED!
			batch->m_vsConstants[20] =  1.00000000f;
			batch->m_vsConstants[21] = -0.16666667f;
			batch->m_vsConstants[22] =  0.00833333f;
			batch->m_vsConstants[23] = -0.00019841f;
*/
			// Reset constant index
			particlesThisBatch = 0;
			j = 24;

			// Add batch to batch list
			m_batches.Add(batch);
		}

		REAL t = particle.m_age / particle.m_extinction;

		// If particle is a brand new one, assign it a rotational speed
		if(m_particles[i].m_particleIteration != particle.m_particleIteration)
		{
			REAL rotationSpeed = m_track_RotationSpeed.GetDiscreteKey(t * m_track_RotationSpeed.GetTrackLength());

			m_particles[i].m_particleIteration = particle.m_particleIteration;

			if(m_randomRotation)
			{
				m_particles[i].m_rotationDirection = RuGlobalMTRand().GenerateINT32() % 2 ? 1 : -1;
			}
			else
			{
				m_particles[i].m_rotationDirection = 1;
			}
		}

		INT32 subTexPerRow = max(1, currentTextureWidth / m_spriteSubTextureWidth);
		INT32 textureIndex = static_cast<INT32>(particle.m_age * m_frameRate) % m_numFrames;

		REAL scale = m_track_Scale.GetInterpolatedKey(t * m_track_Scale.GetTrackLength());
		REAL alpha = m_track_Alpha.GetInterpolatedKey(t * m_track_Alpha.GetTrackLength());
		CRuColor4 color = m_track_Color.GetInterpolatedKey(t * m_track_Color.GetTrackLength());
		REAL rotationSpeed = m_track_RotationSpeed.GetInterpolatedKey(t * m_track_RotationSpeed.GetTrackLength()) * 2.0f * ruPI / 360.0f;

		REAL sinValue;
		REAL cosValue;

		if(m_orientToDirectionOfTravel)
		{
			CRuVector3 projectedVelocity(particle.m_direction.m_z, particle.m_direction.m_y, 0.0f);
			REAL angle = 0.0f;

			switch(emitterOrientation)
			{
				case ruAXIS_X_POSITIVE:
				case ruAXIS_X_NEGATIVE:
					projectedVelocity.Set(particle.m_direction.m_z, particle.m_direction.m_y, 0.0f);
					break;

				case ruAXIS_Y_POSITIVE:
				case ruAXIS_Y_NEGATIVE:
					projectedVelocity.Set(particle.m_direction.m_x, particle.m_direction.m_z, 0.0f);
					break;

				case ruAXIS_Z_POSITIVE:
				case ruAXIS_Z_NEGATIVE:
					projectedVelocity.Set(particle.m_direction.m_x, particle.m_direction.m_y, 0.0f);
					break;
			}

			// Calculate angle
			if(projectedVelocity.SquaredMagnitude() > ruEPSILON)
			{
				projectedVelocity.Normalize();

				angle = acos(DotProduct(CRuVector3(0.0f, 1.0f, 0.0f), projectedVelocity));
				REAL tripleProduct = TripleProduct_Scalar(CRuVector3(0.0f, 1.0f, 0.0f), projectedVelocity, CRuVector3(0.0f, 0.0f, -1.0f));

				if(tripleProduct < 0.0f)
				{
					angle = ruPI * 2.0f - angle;
				}
			}

			RuSinCos(angle, &sinValue, &cosValue);
//			sinValue = sin(angle);
//			cosValue = cos(angle);
		}
		else
		{
			RuSinCos(particle.m_age * m_particles[i].m_rotationDirection * rotationSpeed + particle.m_orientation.m_x, &sinValue, &cosValue);
//			sinValue = sin(particle.m_age * m_particles[i].m_rotationDirection * rotationSpeed + particle.m_orientation.m_x);
//			cosValue = cos(particle.m_age * m_particles[i].m_rotationDirection * rotationSpeed + particle.m_orientation.m_x);
		}

		// Setup constant data for this particle
		batch->m_vsConstants[j++] = particle.m_position.x;												// position x
		batch->m_vsConstants[j++] = particle.m_position.y;												// position y
		batch->m_vsConstants[j++] = particle.m_position.z;												// position z
		batch->m_vsConstants[j++] = particle.m_scale * scale * wScale.x;								// scale

		batch->m_vsConstants[j++] = static_cast<REAL>(textureIndex % subTexPerRow);						// u index
		batch->m_vsConstants[j++] = static_cast<REAL>(textureIndex / subTexPerRow);						// v index
		batch->m_vsConstants[j++] = sinValue;															// sin(phi)
		batch->m_vsConstants[j++] = cosValue;															// cos(phi)

		batch->m_vsConstants[j++] = color.m_r;															// red
		batch->m_vsConstants[j++] = color.m_g;															// green
		batch->m_vsConstants[j++] = color.m_b;															// blue
		batch->m_vsConstants[j++] = alpha;																// alpha

		if(firstParticle == 0)
		{
			firstParticle = 1;
			meshBounds.Set(particle.m_position, 0.0f);
		}
		else
		{
			meshBounds.Merge(particle.m_position, 0.0f);
		}

		// Increment particle count
		++particlesThisBatch;
	}

	if(batch)
	{
		batch->m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, particlesThisBatch);
		batch->m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, m_blendType);

		// Setup rendering batch
		batch->m_mesh->SetProxiedMesh(m_mesh, 0, particlesThisBatch * 2);
		batch->m_mesh->SetBounds(meshBounds);
	}

	m_model->RemoveAllSubModels(); // this guy marks bounds dirty.... must override it

	if(m_batches.Count() > 0)
	{
		// Construct model from batches
		for(INT32 i = 0; i < m_batches.Count(); ++i)
		{
			m_model->AddSubModel(m_batches[i]->m_mesh, m_batches[i]->m_material);
		}

		// Set billboard parameter
		m_model->GetParameterBlock()->SetParameter(ruPARAMTYPE_FX_BILLBOARD, static_cast<UINT64>(m_billboardType));
		m_model->GetParameterBlock()->SetParameter(ruPARAMTYPE_DWORD_1, static_cast<UINT64>(m_billboardAxis));
	}

	const CRuSphere &emitterBounds = m_emitter->GetLocalEntityBounds();
	const CRuSphere &localBounds = this->GetLocalEntityBounds();

	if(!(emitterBounds == localBounds))
	{
		this->SetLocalEntityBounds(emitterBounds);
	}
}

void CRuEntity_Particle_Visualizer_Sprite::ClearBatches()
{
	// Push all used batches onto stack
	for(INT32 i = 0; i < m_batches.Count(); ++i)
	{
		m_freeBatchDescStack.Push(m_batches[i]);
	}

	// Clear batches
	m_batches.Clear();
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
