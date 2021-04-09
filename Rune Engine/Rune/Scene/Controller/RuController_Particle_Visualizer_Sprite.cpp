#include "../../Scene/Controller/RuController_Particle_Visualizer_Sprite.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Particle_Visualizer_Sprite, IRuEntity_KeyableController)
ruHEAP_DEFINE_SUBCLASS(CRuController_Particle_Visualizer_Sprite, "CRuController_Particle_Visualizer_Sprite", "IRuEntity_KeyableController")
ruCLASSGUID_DEFINE(CRuController_Particle_Visualizer_Sprite, 0xEECEC858, 0x501F4bf8, 0xA4B9061B, 0x215CA387)

// ************************************************************************************************************************************************************

CRuController_Particle_Visualizer_Sprite::CRuController_Particle_Visualizer_Sprite()
:	m_particleVisualizer(NULL),
	m_allowWorldRotation(FALSE),
	m_orientation(ruAXIS_Z_POSITIVE),
	m_billboardType(ruBILLBOARDTYPE_UNCONSTRAINED),
	m_billboardAxis(ruAXIS_Y_POSITIVE),
	m_blendType(ruALPHAMODE_NORMAL),
	m_spriteTextureName(NULL),
	m_spriteSubTextureWidth(16),
	m_spriteSubTextureHeight(16),
	m_frameRate(1.0f),
	m_numFrames(1),
	m_randomRotation(FALSE),
	m_quadScale(1.0f, 1.0f, 0.0f)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Particle_Visualizer_Sprite::HandleEvent_Dispose, this, _1), NULL);

	m_track_Color.SetTrackName("Color");
	m_track_Color.AppendKey(CRuColor4(1.0f, 1.0f, 1.0f, 1.0f));

	m_track_Alpha.SetTrackName("Alpha");
	m_track_Alpha.AppendKey(1.0f);

	m_track_Scale.SetTrackName("Scale");
	m_track_Scale.AppendKey(1.0f);

	m_track_RotationSpeed.SetTrackName("Rotation Speed");
	m_track_RotationSpeed.AppendKey(1.0f);
}

CRuController_Particle_Visualizer_Sprite::~CRuController_Particle_Visualizer_Sprite()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

IRuObject *CRuController_Particle_Visualizer_Sprite::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_Particle_Visualizer_Sprite();
	CRuController_Particle_Visualizer_Sprite *controller = static_cast<CRuController_Particle_Visualizer_Sprite *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_KeyableController::Clone(clonedObject);

	// Clone parameters
	controller->m_allowWorldRotation = m_allowWorldRotation;
	controller->m_orientation = m_orientation;
	controller->m_billboardType = m_billboardType;
	controller->m_billboardAxis = m_billboardAxis;
	controller->m_blendType = m_blendType;

	if(m_spriteTextureName)
	{
		controller->m_spriteTextureName = ruNEW char [strlen(m_spriteTextureName) + 1];
		strcpy(controller->m_spriteTextureName, m_spriteTextureName);
	}

	controller->m_spriteSubTextureWidth = m_spriteSubTextureWidth;
	controller->m_spriteSubTextureHeight = m_spriteSubTextureHeight;
	controller->m_frameRate = m_frameRate;
	controller->m_numFrames = m_numFrames;
	controller->m_orientToDirectionOfTravel = m_orientToDirectionOfTravel;
	controller->m_randomRotation = m_randomRotation;
	controller->m_quadOffset = m_quadOffset;
	controller->m_quadScale = m_quadScale;

	// Clone tracks
	CloneTracks(controller);

	return clonedObject;
}

BOOL CRuController_Particle_Visualizer_Sprite::SerializeFrom(IRuStream *stream)
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
				// Read orientation
				stream->Read(&m_orientation, sizeof(RuOrthogonalAxis));

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
			}

			break;

		case 2:
			{
				// Read orientation
				stream->Read(&m_orientation, sizeof(RuOrthogonalAxis));

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
				stream->Read(&m_orientToDirectionOfTravel, sizeof(BOOL));
				stream->Read(&m_randomRotation, sizeof(BOOL));

				// Serialize tracks
				m_track_Color.SerializeFrom(stream);
				m_track_Alpha.SerializeFrom(stream);
				m_track_Scale.SerializeFrom(stream);
				m_track_RotationSpeed.SerializeFrom(stream);
			}

			break;

		case 3:
			{
				// Read orientation
				stream->Read(&m_orientation, sizeof(RuOrthogonalAxis));

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
				stream->Read(&m_orientToDirectionOfTravel, sizeof(BOOL));
				stream->Read(&m_randomRotation, sizeof(BOOL));

				stream->Read(&m_quadOffset.m_x, sizeof(REAL));
				stream->Read(&m_quadOffset.m_y, sizeof(REAL));
				stream->Read(&m_quadOffset.m_z, sizeof(REAL));

				// Serialize tracks
				m_track_Color.SerializeFrom(stream);
				m_track_Alpha.SerializeFrom(stream);
				m_track_Scale.SerializeFrom(stream);
				m_track_RotationSpeed.SerializeFrom(stream);
			}

			break;

		case 4:
			{
				// Read orientation
				stream->Read(&m_orientation, sizeof(RuOrthogonalAxis));

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
				stream->Read(&m_orientToDirectionOfTravel, sizeof(BOOL));
				stream->Read(&m_randomRotation, sizeof(BOOL));

				stream->Read(&m_quadOffset.m_x, sizeof(REAL));
				stream->Read(&m_quadOffset.m_y, sizeof(REAL));
				stream->Read(&m_quadOffset.m_z, sizeof(REAL));

				stream->Read(&m_quadScale.m_x, sizeof(REAL));
				stream->Read(&m_quadScale.m_y, sizeof(REAL));
				stream->Read(&m_quadScale.m_z, sizeof(REAL));

				// Serialize tracks
				m_track_Color.SerializeFrom(stream);
				m_track_Alpha.SerializeFrom(stream);
				m_track_Scale.SerializeFrom(stream);
				m_track_RotationSpeed.SerializeFrom(stream);
			}

			break;

		case 5:
			{
				// Read orientation
				stream->Read(&m_orientation, sizeof(RuOrthogonalAxis));

				// Read billboard settings
				stream->Read(&m_billboardType, sizeof(RuBillboardType));
				stream->Read(&m_billboardAxis, sizeof(RuOrthogonalAxis));

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
				stream->Read(&m_orientToDirectionOfTravel, sizeof(BOOL));
				stream->Read(&m_randomRotation, sizeof(BOOL));

				stream->Read(&m_quadOffset.m_x, sizeof(REAL));
				stream->Read(&m_quadOffset.m_y, sizeof(REAL));
				stream->Read(&m_quadOffset.m_z, sizeof(REAL));

				stream->Read(&m_quadScale.m_x, sizeof(REAL));
				stream->Read(&m_quadScale.m_y, sizeof(REAL));
				stream->Read(&m_quadScale.m_z, sizeof(REAL));

				// Serialize tracks
				m_track_Color.SerializeFrom(stream);
				m_track_Alpha.SerializeFrom(stream);
				m_track_Scale.SerializeFrom(stream);
				m_track_RotationSpeed.SerializeFrom(stream);
			}

			break;

		case 6:
			{
				// Read allow world rotation
				stream->Read(&m_allowWorldRotation, sizeof(BOOL));

				// Read orientation
				stream->Read(&m_orientation, sizeof(RuOrthogonalAxis));

				// Read billboard settings
				stream->Read(&m_billboardType, sizeof(RuBillboardType));
				stream->Read(&m_billboardAxis, sizeof(RuOrthogonalAxis));

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
				stream->Read(&m_orientToDirectionOfTravel, sizeof(BOOL));
				stream->Read(&m_randomRotation, sizeof(BOOL));

				stream->Read(&m_quadOffset.m_x, sizeof(REAL));
				stream->Read(&m_quadOffset.m_y, sizeof(REAL));
				stream->Read(&m_quadOffset.m_z, sizeof(REAL));

				stream->Read(&m_quadScale.m_x, sizeof(REAL));
				stream->Read(&m_quadScale.m_y, sizeof(REAL));
				stream->Read(&m_quadScale.m_z, sizeof(REAL));

				// Serialize tracks
				m_track_Color.SerializeFrom(stream);
				m_track_Alpha.SerializeFrom(stream);
				m_track_Scale.SerializeFrom(stream);
				m_track_RotationSpeed.SerializeFrom(stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_Particle_Visualizer_Sprite::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 6;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeTo(stream);

	// Write allow world rotation
	stream->Write(&m_allowWorldRotation, sizeof(BOOL));

	// Write orientation
	stream->Write(&m_orientation, sizeof(RuOrthogonalAxis));

	// Write billboard settings
	stream->Write(&m_billboardType, sizeof(RuBillboardType));
	stream->Write(&m_billboardAxis, sizeof(RuOrthogonalAxis));

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
	stream->Write(&m_orientToDirectionOfTravel, sizeof(BOOL));
	stream->Write(&m_randomRotation, sizeof(BOOL));

	stream->Write(&m_quadOffset.m_x, sizeof(REAL));
	stream->Write(&m_quadOffset.m_y, sizeof(REAL));
	stream->Write(&m_quadOffset.m_z, sizeof(REAL));

	stream->Write(&m_quadScale.m_x, sizeof(REAL));
	stream->Write(&m_quadScale.m_y, sizeof(REAL));
	stream->Write(&m_quadScale.m_z, sizeof(REAL));

	// Serialize tracks
	m_track_Color.SerializeTo(stream);
	m_track_Alpha.SerializeTo(stream);
	m_track_Scale.SerializeTo(stream);
	m_track_RotationSpeed.SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_Particle_Visualizer_Sprite::ResetStates()
{
	m_age = 0.0f;

	return IRuEntity_KeyableController::ResetStates();
}

BOOL CRuController_Particle_Visualizer_Sprite::Update(REAL elapsedTime)
{
	if(m_enabled)
	{
	}

	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

IRuObject *CRuController_Particle_Visualizer_Sprite::GetTarget()
{
	return m_particleVisualizer;
}

BOOL CRuController_Particle_Visualizer_Sprite::SetTarget(IRuObject *targetEntity)
{
	CRuEntity_Particle_Visualizer_Sprite *visualizer = NULL;

	if(targetEntity->GetType().IsSubClassOf(CRuEntity_Particle_Visualizer_Sprite::Type()))
	{
		visualizer = static_cast<CRuEntity_Particle_Visualizer_Sprite *>(targetEntity);
	}
	else if(targetEntity->GetType().IsSubClassOf(IRuEntity_Particle_Emitter::Type()))
	{
		IRuEntity_Particle_Visualizer *vizInterface = static_cast<IRuEntity_Particle_Emitter *>(targetEntity)->GetVisualizer();

		if(vizInterface && vizInterface->GetType().IsSubClassOf(CRuEntity_Particle_Visualizer_Sprite::Type()))
		{
			visualizer = static_cast<CRuEntity_Particle_Visualizer_Sprite *>(vizInterface);
		}
	}

	if(visualizer)
	{
		// If we had a previous target, unregister it
		if(m_particleVisualizer)
		{
			m_particleVisualizer->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		// Store target reference, setup event listener, and setup target parameter block
		m_particleVisualizer = visualizer;
		m_particleVisualizer->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Particle_Visualizer_Sprite::HandleEvent_Dispose, this, _1), NULL);

		m_particleVisualizer->SetAllowWorldRotation(m_allowWorldRotation);
		m_particleVisualizer->SetOrientation(m_orientation);
		m_particleVisualizer->SetBillboardType(m_billboardType);
		m_particleVisualizer->SetBillboardAxis(m_billboardAxis);
		m_particleVisualizer->SetBlendType(m_blendType);
		m_particleVisualizer->SetTextureName(m_spriteTextureName);
		m_particleVisualizer->SetSubTextureWidth(m_spriteSubTextureWidth);
		m_particleVisualizer->SetSubTextureHeight(m_spriteSubTextureHeight);
		m_particleVisualizer->SetFrameRate(m_frameRate);
		m_particleVisualizer->SetNumFrames(m_numFrames);
		m_particleVisualizer->SetOrientToDirectionOfTravel(m_orientToDirectionOfTravel);
		m_particleVisualizer->SetRandomRotation(m_randomRotation);
		m_particleVisualizer->SetQuadOffset(m_quadOffset);
		m_particleVisualizer->SetQuadScale(m_quadScale);

		m_track_Color.CopyTo(&m_particleVisualizer->GetTrack_Color());
		m_track_Alpha.CopyTo(&m_particleVisualizer->GetTrack_Alpha());
		m_track_Scale.CopyTo(&m_particleVisualizer->GetTrack_Scale());
		m_track_RotationSpeed.CopyTo(&m_particleVisualizer->GetTrack_RotationSpeed());

		m_particleVisualizer->ResetUpdateTimer();
	}

	return TRUE;
}

INT32 CRuController_Particle_Visualizer_Sprite::GetNumTracks() const
{
	return 4;
}

BOOL CRuController_Particle_Visualizer_Sprite::GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut)
{
	switch(trackIndex)
	{
		case 0:
			trackOut = &m_track_Color;
			return TRUE;

		case 1:
			trackOut = &m_track_Alpha;
			return TRUE;

		case 2:
			trackOut = &m_track_Scale;
			return TRUE;

		case 3:
			trackOut = &m_track_RotationSpeed;
			return TRUE;
	}

	return FALSE;
}

BOOL CRuController_Particle_Visualizer_Sprite::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_particleVisualizer)
	{
		// Unregister from target entity
		m_particleVisualizer->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_particleVisualizer = NULL;
	}

	if ( m_spriteTextureName )
	{
		delete[] m_spriteTextureName;
		m_spriteTextureName = NULL;
	}

	return TRUE;
}

BOOL CRuController_Particle_Visualizer_Sprite::GetAllowWorldRotation() const
{
	return m_allowWorldRotation;
}

void CRuController_Particle_Visualizer_Sprite::SetAllowWorldRotation(BOOL allowWorldRotation)
{
	m_allowWorldRotation = allowWorldRotation;
}

RuOrthogonalAxis CRuController_Particle_Visualizer_Sprite::GetOrientation() const
{
	return m_orientation;
}

void CRuController_Particle_Visualizer_Sprite::SetOrientation(RuOrthogonalAxis orientation)
{
	m_orientation = orientation;
}

RuBillboardType CRuController_Particle_Visualizer_Sprite::GetBillboardType() const
{
	return m_billboardType;
}

void CRuController_Particle_Visualizer_Sprite::SetBillboardType(RuBillboardType billboardType)
{
	m_billboardType = billboardType;
}

RuOrthogonalAxis CRuController_Particle_Visualizer_Sprite::GetBillboardAxis() const
{
	return m_billboardAxis;
}

void CRuController_Particle_Visualizer_Sprite::SetBillboardAxis(RuOrthogonalAxis billboardAxis)
{
	m_billboardAxis = billboardAxis;
}

RuAlphaMode CRuController_Particle_Visualizer_Sprite::GetBlendType() const
{
	return m_blendType;
}

void CRuController_Particle_Visualizer_Sprite::SetBlendType(RuAlphaMode blendType)
{
	m_blendType = blendType;
}

const char *CRuController_Particle_Visualizer_Sprite::GetTextureName()
{
	if(m_spriteTextureName)
	{
		return m_spriteTextureName;
	}

	return "";
}

void CRuController_Particle_Visualizer_Sprite::SetTextureName(const char *textureName)
{
	if(textureName)
	{
		if(m_spriteTextureName)
		{
			delete[] m_spriteTextureName;
		}

		m_spriteTextureName = ruNEW char [strlen(textureName) + 1];
		strcpy(m_spriteTextureName, textureName);
	}
}

INT32 CRuController_Particle_Visualizer_Sprite::GetSubTextureWidth() const
{
	return m_spriteSubTextureWidth;
}

void CRuController_Particle_Visualizer_Sprite::SetSubTextureWidth(INT32 width)
{
	if(width > 0)
	{
		m_spriteSubTextureWidth = width;
	}
}

INT32 CRuController_Particle_Visualizer_Sprite::GetSubTextureHeight() const
{
	return m_spriteSubTextureHeight;
}

void CRuController_Particle_Visualizer_Sprite::SetSubTextureHeight(INT32 height)
{
	if(height > 0)
	{
		m_spriteSubTextureHeight = height;
	}
}

REAL CRuController_Particle_Visualizer_Sprite::GetFrameRate() const
{
	return m_frameRate;
}

void CRuController_Particle_Visualizer_Sprite::SetFrameRate(REAL frameRate)
{
	if(frameRate > 0.0f)
	{
		m_frameRate = frameRate;
	}
}

INT32 CRuController_Particle_Visualizer_Sprite::GetNumFrames() const
{
	return m_numFrames;
}

void CRuController_Particle_Visualizer_Sprite::SetNumFrames(INT32 numFrames)
{
	if(numFrames > 0)
	{
		m_numFrames = numFrames;
	}
}

BOOL CRuController_Particle_Visualizer_Sprite::GetOrientToDirectionOfTravel() const
{
	return m_orientToDirectionOfTravel;
}

void CRuController_Particle_Visualizer_Sprite::SetOrientToDirectionOfTravel(BOOL orientToDirectionOfTravel)
{
	m_orientToDirectionOfTravel = orientToDirectionOfTravel;
}

BOOL CRuController_Particle_Visualizer_Sprite::GetRandomRotation() const
{
	return m_randomRotation;
}

void CRuController_Particle_Visualizer_Sprite::SetRandomRotation(BOOL randomRotation)
{
	m_randomRotation = randomRotation;
}

const CRuVector3 &CRuController_Particle_Visualizer_Sprite::GetQuadOffset() const
{
	return m_quadOffset;
}

void CRuController_Particle_Visualizer_Sprite::SetQuadOffset(const CRuVector3 &quadOffset)
{
	m_quadOffset = quadOffset;
}

const CRuVector3 &CRuController_Particle_Visualizer_Sprite::GetQuadScale() const
{
	return m_quadScale;
}

void CRuController_Particle_Visualizer_Sprite::SetQuadScale(const CRuVector3 &quadScale)
{
	m_quadScale = quadScale;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
