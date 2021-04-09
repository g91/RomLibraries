#include "../../Scene/Controller/RuController_Particle_Emitter.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Particle_Emitter, IRuEntity_KeyableController)
ruHEAP_DEFINE_SUBCLASS(CRuController_Particle_Emitter, "CRuController_Particle_Emitter", "IRuEntity_KeyableController")
ruCLASSGUID_DEFINE(CRuController_Particle_Emitter, 0x83CC3071, 0xFDF6487e, 0x914DBA36, 0x5DE67C4E)

// ************************************************************************************************************************************************************

CRuController_Particle_Emitter::CRuController_Particle_Emitter()
:	m_prtSysRadius(5.0f),
	m_frameType(ruPRTFRAMETYPE_LOCAL),
	m_particlePathFrameType(ruPRTFRAMETYPE_LOCAL),
	m_particlePathRandomMirror(FALSE),
	m_particlePathRandomMirrorPlane(ruPLANE_XY),
	m_particlePathRotationAxis(0.0f, 1.0f, 0.0f),
	m_particlePathRotationDeviation(0.0f),
	m_particlePathName(NULL),
	m_particlePathLoopDelay(-1.0f),
	m_emitterPathFrameType(ruFRAMETYPE_LOCAL),
	m_emitterPathName(NULL),
	m_emitterPrerunTime(0.0f),
	m_emitterPathLoopDelay(-1.0f),
	m_emitterOrientation(ruAXIS_Z_NEGATIVE),
	m_particleEmitter(NULL)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Particle_Emitter::HandleEvent_Dispose, this, _1), NULL);

	// Override default controller clock update
	this->m_overrideClockUpdate = TRUE;

	m_track_MaxParticles.SetTrackCategory("Emitter");
	m_track_MaxParticles.SetTrackName("Maximum Particles");
	m_track_MaxParticles.AppendKey(200);

	m_track_EmitterType.SetTrackCategory("Emitter");
	m_track_EmitterType.SetTrackName("Type");
	m_track_EmitterType.AppendKey(ruPRTEMITTERTYPE_POINT);
	
	m_track_EmitterPosition.SetTrackCategory("Emitter");
	m_track_EmitterPosition.SetTrackName("Position");
	m_track_EmitterPosition.AppendKey(CRuVector3(0.0f, 0.0f, 0.0f));

	m_track_EmitterExtents.SetTrackCategory("Emitter");
	m_track_EmitterExtents.SetTrackName("Extents");
	m_track_EmitterExtents.AppendKey(CRuVector3(0.0f, 0.0f, 0.0f));

	m_track_EmitterRadius.SetTrackCategory("Emitter");
	m_track_EmitterRadius.SetTrackName("Radius");
	m_track_EmitterRadius.AppendKey(20.0f);

	m_track_EmitterMinTheta.SetTrackCategory("Emitter");
	m_track_EmitterMinTheta.SetTrackName("Minimum Theta");
	m_track_EmitterMinTheta.AppendKey(0.0f);

	m_track_EmitterMaxTheta.SetTrackCategory("Emitter");
	m_track_EmitterMaxTheta.SetTrackName("Maximum Theta");
	m_track_EmitterMaxTheta.AppendKey(0.0f);

	m_track_EmitterMinPhi.SetTrackCategory("Emitter");
	m_track_EmitterMinPhi.SetTrackName("Minimum Phi");
	m_track_EmitterMinPhi.AppendKey(0.0f);

	m_track_EmitterMaxPhi.SetTrackCategory("Emitter");
	m_track_EmitterMaxPhi.SetTrackName("Maximum Phi");
	m_track_EmitterMaxPhi.AppendKey(0.0f);

	m_track_EmissionRate.SetTrackCategory("Emitter");
	m_track_EmissionRate.SetTrackName("Emission Rate");
	m_track_EmissionRate.AppendKey(5.0f);

	m_track_EmitDirection.SetTrackCategory("Emitter");
	m_track_EmitDirection.SetTrackName("Emission Direction");
	m_track_EmitDirection.AppendKey(CRuVector3(0.0f, 1.0f, 0.0f));

	m_track_EmitDeviation.SetTrackCategory("Emitter");
	m_track_EmitDeviation.SetTrackName("Emission Deviation");
	m_track_EmitDeviation.AppendKey(0.2f);

	m_track_ParticleMinExtinction.SetTrackCategory("Particle");
	m_track_ParticleMinExtinction.SetTrackName("Minimum Extinction");
	m_track_ParticleMinExtinction.AppendKey(3.0f);

	m_track_ParticleMaxExtinction.SetTrackCategory("Particle");
	m_track_ParticleMaxExtinction.SetTrackName("Maximum Extinction");
	m_track_ParticleMaxExtinction.AppendKey(3.0f);

	m_track_ParticleMinSpeed.SetTrackCategory("Particle");
	m_track_ParticleMinSpeed.SetTrackName("Minimum Speed");
	m_track_ParticleMinSpeed.AppendKey(3.0f);

	m_track_ParticleMaxSpeed.SetTrackCategory("Particle");
	m_track_ParticleMaxSpeed.SetTrackName("Maximum Speed");
	m_track_ParticleMaxSpeed.AppendKey(3.0f);

	m_track_ParticleMinScale.SetTrackCategory("Particle");
	m_track_ParticleMinScale.SetTrackName("Minimum Scale");
	m_track_ParticleMinScale.AppendKey(1.0f);

	m_track_ParticleMaxScale.SetTrackCategory("Particle");
	m_track_ParticleMaxScale.SetTrackName("Maximum Scale");
	m_track_ParticleMaxScale.AppendKey(1.0f);

	m_track_ParticleMinOrientation.SetTrackCategory("Particle");
	m_track_ParticleMinOrientation.SetTrackName("Minimum Orientation");
	m_track_ParticleMinOrientation.AppendKey(CRuVector3(0.0f, 0.0f, 0.0f));

	m_track_ParticleMaxOrientation.SetTrackCategory("Particle");
	m_track_ParticleMaxOrientation.SetTrackName("Maximum Orientation");
	m_track_ParticleMaxOrientation.AppendKey(CRuVector3(0.0f, 0.0f, 0.0f));

	m_track_Force0.SetTrackName("Force 0");
	m_track_Force0.AppendKey(CRuVector3(0.0f, 0.0f, 0.0f));

	m_track_Force1.SetTrackName("Force 1");
	m_track_Force1.AppendKey(CRuVector3(0.0f, 0.0f, 0.0f));

	m_track_CollectorPosition.SetTrackCategory("Collector");
	m_track_CollectorPosition.SetTrackName("Position");
	m_track_CollectorPosition.AppendKey(CRuVector3(0.0f, 0.0f, 0.0f));

	m_track_CollectorStrength.SetTrackCategory("Collector");
	m_track_CollectorStrength.SetTrackName("Strength");
	m_track_CollectorStrength.AppendKey(0.0f);

	m_track_CollectorFalloff0.SetTrackCategory("Collector");
	m_track_CollectorFalloff0.SetTrackName("Constant Falloff");
	m_track_CollectorFalloff0.AppendKey(0.0f);

	m_track_CollectorFalloff1.SetTrackCategory("Collector");
	m_track_CollectorFalloff1.SetTrackName("Linear Falloff");
	m_track_CollectorFalloff1.AppendKey(0.0f);

	m_track_CollectorFalloff2.SetTrackCategory("Collector");
	m_track_CollectorFalloff2.SetTrackName("Quadratic Falloff");
	m_track_CollectorFalloff2.AppendKey(0.0f);
}

CRuController_Particle_Emitter::~CRuController_Particle_Emitter()
{
	// NOTE: Destruction of resources is done during the ruENTITYEVENT_ONDESTROY event.
}

IRuObject *CRuController_Particle_Emitter::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_Particle_Emitter();
	CRuController_Particle_Emitter *controller = static_cast<CRuController_Particle_Emitter *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_KeyableController::Clone(clonedObject);

	// Clone parameters
	controller->m_prtSysRadius = m_prtSysRadius;
	controller->m_frameType = m_frameType;
	controller->m_particlePathFrameType = m_particlePathFrameType;

	controller->m_particlePathRandomMirror = m_particlePathRandomMirror;
	controller->m_particlePathRandomMirrorPlane = m_particlePathRandomMirrorPlane;
	controller->m_particlePathRotationAxis = m_particlePathRotationAxis;
	controller->m_particlePathRotationDeviation = m_particlePathRotationDeviation;

	if(m_particlePathName)
	{
		controller->m_particlePathName = ruNEW char [strlen(m_particlePathName) + 1];
		strcpy(controller->m_particlePathName, m_particlePathName);
	}

	controller->m_particlePathLoopDelay = m_particlePathLoopDelay;

	controller->m_emitterPathFrameType = m_emitterPathFrameType;
	if(m_emitterPathName)
	{
		controller->m_emitterPathName = ruNEW char [strlen(m_emitterPathName) + 1];
		strcpy(controller->m_emitterPathName, m_emitterPathName);
	}

	controller->m_emitterPathLoopDelay = m_emitterPathLoopDelay;
	controller->m_emitterPrerunTime = m_emitterPrerunTime;
	controller->m_emitterOrientation = m_emitterOrientation;

	// Clone tracks
	CloneTracks(controller);

	return clonedObject;
}

BOOL CRuController_Particle_Emitter::SerializeFrom(IRuStream *stream)
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
				// Serialize Tracks
				m_track_MaxParticles.SerializeFrom(stream);
				m_track_EmitterType.SerializeFrom(stream);
				m_track_EmitterPosition.SerializeFrom(stream);
				m_track_EmitterExtents.SerializeFrom(stream);
				m_track_EmitterRadius.SerializeFrom(stream);
				m_track_EmitterMinTheta.SerializeFrom(stream);
				m_track_EmitterMaxTheta.SerializeFrom(stream);
				m_track_EmitterMinPhi.SerializeFrom(stream);
				m_track_EmitterMaxPhi.SerializeFrom(stream);
				m_track_EmissionRate.SerializeFrom(stream);
				m_track_EmitDirection.SerializeFrom(stream);
				m_track_EmitDeviation.SerializeFrom(stream);
				m_track_ParticleMinExtinction.SerializeFrom(stream);
				m_track_ParticleMaxExtinction.SerializeFrom(stream);
				m_track_ParticleMinSpeed.SerializeFrom(stream);
				m_track_ParticleMaxSpeed.SerializeFrom(stream);
				m_track_Force0.SerializeFrom(stream);
				m_track_Force1.SerializeFrom(stream);
			}

			break;

		case 2:
			{
				stream->Read(&m_frameType, sizeof(RuParticleFrameType));

				// Serialize Tracks
				m_track_MaxParticles.SerializeFrom(stream);
				m_track_EmitterType.SerializeFrom(stream);
				m_track_EmitterPosition.SerializeFrom(stream);
				m_track_EmitterExtents.SerializeFrom(stream);
				m_track_EmitterRadius.SerializeFrom(stream);
				m_track_EmitterMinTheta.SerializeFrom(stream);
				m_track_EmitterMaxTheta.SerializeFrom(stream);
				m_track_EmitterMinPhi.SerializeFrom(stream);
				m_track_EmitterMaxPhi.SerializeFrom(stream);
				m_track_EmissionRate.SerializeFrom(stream);
				m_track_EmitDirection.SerializeFrom(stream);
				m_track_EmitDeviation.SerializeFrom(stream);
				m_track_ParticleMinExtinction.SerializeFrom(stream);
				m_track_ParticleMaxExtinction.SerializeFrom(stream);
				m_track_ParticleMinSpeed.SerializeFrom(stream);
				m_track_ParticleMaxSpeed.SerializeFrom(stream);
				m_track_Force0.SerializeFrom(stream);
				m_track_Force1.SerializeFrom(stream);
			}

			break;

		case 3:
			{
				stream->Read(&m_frameType, sizeof(RuParticleFrameType));

				// Serialize Tracks
				m_track_MaxParticles.SerializeFrom(stream);
				m_track_EmitterType.SerializeFrom(stream);
				m_track_EmitterPosition.SerializeFrom(stream);
				m_track_EmitterExtents.SerializeFrom(stream);
				m_track_EmitterRadius.SerializeFrom(stream);
				m_track_EmitterMinTheta.SerializeFrom(stream);
				m_track_EmitterMaxTheta.SerializeFrom(stream);
				m_track_EmitterMinPhi.SerializeFrom(stream);
				m_track_EmitterMaxPhi.SerializeFrom(stream);
				m_track_EmissionRate.SerializeFrom(stream);
				m_track_EmitDirection.SerializeFrom(stream);
				m_track_EmitDeviation.SerializeFrom(stream);
				m_track_ParticleMinExtinction.SerializeFrom(stream);
				m_track_ParticleMaxExtinction.SerializeFrom(stream);
				m_track_ParticleMinSpeed.SerializeFrom(stream);
				m_track_ParticleMaxSpeed.SerializeFrom(stream);
				m_track_ParticleMinScale.SerializeFrom(stream);
				m_track_ParticleMaxScale.SerializeFrom(stream);
				m_track_Force0.SerializeFrom(stream);
				m_track_Force1.SerializeFrom(stream);
			}

			break;

		case 4:
			{
				stream->Read(&m_frameType, sizeof(RuParticleFrameType));

				// Serialize Tracks
				m_track_MaxParticles.SerializeFrom(stream);
				m_track_EmitterType.SerializeFrom(stream);
				m_track_EmitterPosition.SerializeFrom(stream);
				m_track_EmitterExtents.SerializeFrom(stream);
				m_track_EmitterRadius.SerializeFrom(stream);
				m_track_EmitterMinTheta.SerializeFrom(stream);
				m_track_EmitterMaxTheta.SerializeFrom(stream);
				m_track_EmitterMinPhi.SerializeFrom(stream);
				m_track_EmitterMaxPhi.SerializeFrom(stream);
				m_track_EmissionRate.SerializeFrom(stream);
				m_track_EmitDirection.SerializeFrom(stream);
				m_track_EmitDeviation.SerializeFrom(stream);
				m_track_ParticleMinExtinction.SerializeFrom(stream);
				m_track_ParticleMaxExtinction.SerializeFrom(stream);
				m_track_ParticleMinSpeed.SerializeFrom(stream);
				m_track_ParticleMaxSpeed.SerializeFrom(stream);
				m_track_ParticleMinScale.SerializeFrom(stream);
				m_track_ParticleMaxScale.SerializeFrom(stream);
				m_track_ParticleMinOrientation.SerializeFrom(stream);
				m_track_ParticleMaxOrientation.SerializeFrom(stream);
				m_track_Force0.SerializeFrom(stream);
				m_track_Force1.SerializeFrom(stream);
			}

			break;

		case 5:
			{
				stream->Read(&m_frameType, sizeof(RuParticleFrameType));

				// Serialize Tracks
				m_track_MaxParticles.SerializeFrom(stream);
				m_track_EmitterType.SerializeFrom(stream);
				m_track_EmitterPosition.SerializeFrom(stream);
				m_track_EmitterExtents.SerializeFrom(stream);
				m_track_EmitterRadius.SerializeFrom(stream);
				m_track_EmitterMinTheta.SerializeFrom(stream);
				m_track_EmitterMaxTheta.SerializeFrom(stream);
				m_track_EmitterMinPhi.SerializeFrom(stream);
				m_track_EmitterMaxPhi.SerializeFrom(stream);
				m_track_EmissionRate.SerializeFrom(stream);
				m_track_EmitDirection.SerializeFrom(stream);
				m_track_EmitDeviation.SerializeFrom(stream);
				m_track_ParticleMinExtinction.SerializeFrom(stream);
				m_track_ParticleMaxExtinction.SerializeFrom(stream);
				m_track_ParticleMinSpeed.SerializeFrom(stream);
				m_track_ParticleMaxSpeed.SerializeFrom(stream);
				m_track_ParticleMinScale.SerializeFrom(stream);
				m_track_ParticleMaxScale.SerializeFrom(stream);
				m_track_ParticleMinOrientation.SerializeFrom(stream);
				m_track_ParticleMaxOrientation.SerializeFrom(stream);
				m_track_Force0.SerializeFrom(stream);
				m_track_Force1.SerializeFrom(stream);
				m_track_CollectorPosition.SerializeFrom(stream);
				m_track_CollectorStrength.SerializeFrom(stream);
				m_track_CollectorFalloff0.SerializeFrom(stream);
				m_track_CollectorFalloff1.SerializeFrom(stream);
				m_track_CollectorFalloff2.SerializeFrom(stream);
			}

			break;

		case 6:
			{
				stream->Read(&m_frameType, sizeof(RuParticleFrameType));
				stream->Read(&m_particlePathFrameType, sizeof(RuParticleFrameType));

				// Read length of path file name including NULL, followed by the name
				INT32 len = 0;
				stream->Read(&len, sizeof(INT32));
				m_particlePathName = ruNEW char [len];
				stream->Read(m_particlePathName, len);

				// Serialize Tracks
				m_track_MaxParticles.SerializeFrom(stream);
				m_track_EmitterType.SerializeFrom(stream);
				m_track_EmitterPosition.SerializeFrom(stream);
				m_track_EmitterExtents.SerializeFrom(stream);
				m_track_EmitterRadius.SerializeFrom(stream);
				m_track_EmitterMinTheta.SerializeFrom(stream);
				m_track_EmitterMaxTheta.SerializeFrom(stream);
				m_track_EmitterMinPhi.SerializeFrom(stream);
				m_track_EmitterMaxPhi.SerializeFrom(stream);
				m_track_EmissionRate.SerializeFrom(stream);
				m_track_EmitDirection.SerializeFrom(stream);
				m_track_EmitDeviation.SerializeFrom(stream);
				m_track_ParticleMinExtinction.SerializeFrom(stream);
				m_track_ParticleMaxExtinction.SerializeFrom(stream);
				m_track_ParticleMinSpeed.SerializeFrom(stream);
				m_track_ParticleMaxSpeed.SerializeFrom(stream);
				m_track_ParticleMinScale.SerializeFrom(stream);
				m_track_ParticleMaxScale.SerializeFrom(stream);
				m_track_ParticleMinOrientation.SerializeFrom(stream);
				m_track_ParticleMaxOrientation.SerializeFrom(stream);
				m_track_Force0.SerializeFrom(stream);
				m_track_Force1.SerializeFrom(stream);
				m_track_CollectorPosition.SerializeFrom(stream);
				m_track_CollectorStrength.SerializeFrom(stream);
				m_track_CollectorFalloff0.SerializeFrom(stream);
				m_track_CollectorFalloff1.SerializeFrom(stream);
				m_track_CollectorFalloff2.SerializeFrom(stream);
			}

			break;

		case 7:
			{
				stream->Read(&m_frameType, sizeof(RuParticleFrameType));
				stream->Read(&m_particlePathFrameType, sizeof(RuParticleFrameType));

				// Read length of path file name including NULL, followed by the name
				INT32 len = 0;
				stream->Read(&len, sizeof(INT32));
				m_particlePathName = ruNEW char [len];
				stream->Read(m_particlePathName, len);

				stream->Read(&m_emitterPrerunTime, sizeof(REAL));

				// Serialize Tracks
				m_track_MaxParticles.SerializeFrom(stream);
				m_track_EmitterType.SerializeFrom(stream);
				m_track_EmitterPosition.SerializeFrom(stream);
				m_track_EmitterExtents.SerializeFrom(stream);
				m_track_EmitterRadius.SerializeFrom(stream);
				m_track_EmitterMinTheta.SerializeFrom(stream);
				m_track_EmitterMaxTheta.SerializeFrom(stream);
				m_track_EmitterMinPhi.SerializeFrom(stream);
				m_track_EmitterMaxPhi.SerializeFrom(stream);
				m_track_EmissionRate.SerializeFrom(stream);
				m_track_EmitDirection.SerializeFrom(stream);
				m_track_EmitDeviation.SerializeFrom(stream);
				m_track_ParticleMinExtinction.SerializeFrom(stream);
				m_track_ParticleMaxExtinction.SerializeFrom(stream);
				m_track_ParticleMinSpeed.SerializeFrom(stream);
				m_track_ParticleMaxSpeed.SerializeFrom(stream);
				m_track_ParticleMinScale.SerializeFrom(stream);
				m_track_ParticleMaxScale.SerializeFrom(stream);
				m_track_ParticleMinOrientation.SerializeFrom(stream);
				m_track_ParticleMaxOrientation.SerializeFrom(stream);
				m_track_Force0.SerializeFrom(stream);
				m_track_Force1.SerializeFrom(stream);
				m_track_CollectorPosition.SerializeFrom(stream);
				m_track_CollectorStrength.SerializeFrom(stream);
				m_track_CollectorFalloff0.SerializeFrom(stream);
				m_track_CollectorFalloff1.SerializeFrom(stream);
				m_track_CollectorFalloff2.SerializeFrom(stream);
			}

			break;

		case 8:
			{
				INT32 len = 0;

				stream->Read(&m_frameType, sizeof(RuParticleFrameType));
				stream->Read(&m_particlePathFrameType, sizeof(RuParticleFrameType));

				// Read length of path file name including NULL, followed by the name
				stream->Read(&len, sizeof(INT32));
				m_particlePathName = ruNEW char [len];
				stream->Read(m_particlePathName, len);

				stream->Read(&m_emitterPathFrameType, sizeof(RuFrameType));

				// Read length of path file name including NULL, followed by the name
				stream->Read(&len, sizeof(INT32));
				m_emitterPathName = ruNEW char [len];
				stream->Read(m_emitterPathName, len);

				stream->Read(&m_emitterPrerunTime, sizeof(REAL));

				// Serialize Tracks
				m_track_MaxParticles.SerializeFrom(stream);
				m_track_EmitterType.SerializeFrom(stream);
				m_track_EmitterPosition.SerializeFrom(stream);
				m_track_EmitterExtents.SerializeFrom(stream);
				m_track_EmitterRadius.SerializeFrom(stream);
				m_track_EmitterMinTheta.SerializeFrom(stream);
				m_track_EmitterMaxTheta.SerializeFrom(stream);
				m_track_EmitterMinPhi.SerializeFrom(stream);
				m_track_EmitterMaxPhi.SerializeFrom(stream);
				m_track_EmissionRate.SerializeFrom(stream);
				m_track_EmitDirection.SerializeFrom(stream);
				m_track_EmitDeviation.SerializeFrom(stream);
				m_track_ParticleMinExtinction.SerializeFrom(stream);
				m_track_ParticleMaxExtinction.SerializeFrom(stream);
				m_track_ParticleMinSpeed.SerializeFrom(stream);
				m_track_ParticleMaxSpeed.SerializeFrom(stream);
				m_track_ParticleMinScale.SerializeFrom(stream);
				m_track_ParticleMaxScale.SerializeFrom(stream);
				m_track_ParticleMinOrientation.SerializeFrom(stream);
				m_track_ParticleMaxOrientation.SerializeFrom(stream);
				m_track_Force0.SerializeFrom(stream);
				m_track_Force1.SerializeFrom(stream);
				m_track_CollectorPosition.SerializeFrom(stream);
				m_track_CollectorStrength.SerializeFrom(stream);
				m_track_CollectorFalloff0.SerializeFrom(stream);
				m_track_CollectorFalloff1.SerializeFrom(stream);
				m_track_CollectorFalloff2.SerializeFrom(stream);
			}

			break;

		case 9:
			{
				INT32 len = 0;

				stream->Read(&m_frameType, sizeof(RuParticleFrameType));
				stream->Read(&m_particlePathFrameType, sizeof(RuParticleFrameType));

				// Read length of path file name including NULL, followed by the name
				stream->Read(&len, sizeof(INT32));
				m_particlePathName = ruNEW char [len];
				stream->Read(m_particlePathName, len);

				stream->Read(&m_particlePathLoopDelay, sizeof(REAL));

				stream->Read(&m_emitterPathFrameType, sizeof(RuFrameType));

				// Read length of path file name including NULL, followed by the name
				stream->Read(&len, sizeof(INT32));
				m_emitterPathName = ruNEW char [len];
				stream->Read(m_emitterPathName, len);

				stream->Read(&m_emitterPathLoopDelay, sizeof(REAL));
				stream->Read(&m_emitterPrerunTime, sizeof(REAL));

				// Serialize Tracks
				m_track_MaxParticles.SerializeFrom(stream);
				m_track_EmitterType.SerializeFrom(stream);
				m_track_EmitterPosition.SerializeFrom(stream);
				m_track_EmitterExtents.SerializeFrom(stream);
				m_track_EmitterRadius.SerializeFrom(stream);
				m_track_EmitterMinTheta.SerializeFrom(stream);
				m_track_EmitterMaxTheta.SerializeFrom(stream);
				m_track_EmitterMinPhi.SerializeFrom(stream);
				m_track_EmitterMaxPhi.SerializeFrom(stream);
				m_track_EmissionRate.SerializeFrom(stream);
				m_track_EmitDirection.SerializeFrom(stream);
				m_track_EmitDeviation.SerializeFrom(stream);
				m_track_ParticleMinExtinction.SerializeFrom(stream);
				m_track_ParticleMaxExtinction.SerializeFrom(stream);
				m_track_ParticleMinSpeed.SerializeFrom(stream);
				m_track_ParticleMaxSpeed.SerializeFrom(stream);
				m_track_ParticleMinScale.SerializeFrom(stream);
				m_track_ParticleMaxScale.SerializeFrom(stream);
				m_track_ParticleMinOrientation.SerializeFrom(stream);
				m_track_ParticleMaxOrientation.SerializeFrom(stream);
				m_track_Force0.SerializeFrom(stream);
				m_track_Force1.SerializeFrom(stream);
				m_track_CollectorPosition.SerializeFrom(stream);
				m_track_CollectorStrength.SerializeFrom(stream);
				m_track_CollectorFalloff0.SerializeFrom(stream);
				m_track_CollectorFalloff1.SerializeFrom(stream);
				m_track_CollectorFalloff2.SerializeFrom(stream);
			}

			break;

		case 10:
			{
				INT32 len = 0;

				stream->Read(&m_frameType, sizeof(RuParticleFrameType));
				stream->Read(&m_particlePathFrameType, sizeof(RuParticleFrameType));

				stream->Read(&m_particlePathRandomMirror, sizeof(BOOL));
				stream->Read(&m_particlePathRandomMirrorPlane, sizeof(RuOrthogonalPlane));

				::SerializeFrom(stream, &m_particlePathRotationAxis);
				stream->Read(&m_particlePathRotationDeviation, sizeof(REAL));

				// Read length of path file name including NULL, followed by the name
				stream->Read(&len, sizeof(INT32));
				m_particlePathName = ruNEW char [len];
				stream->Read(m_particlePathName, len);

				stream->Read(&m_particlePathLoopDelay, sizeof(REAL));

				stream->Read(&m_emitterPathFrameType, sizeof(RuFrameType));

				// Read length of path file name including NULL, followed by the name
				stream->Read(&len, sizeof(INT32));
				m_emitterPathName = ruNEW char [len];
				stream->Read(m_emitterPathName, len);

				stream->Read(&m_emitterPathLoopDelay, sizeof(REAL));
				stream->Read(&m_emitterPrerunTime, sizeof(REAL));

				stream->Read(&m_emitterOrientation, sizeof(RuOrthogonalAxis));

				// Serialize Tracks
				m_track_MaxParticles.SerializeFrom(stream);
				m_track_EmitterType.SerializeFrom(stream);
				m_track_EmitterPosition.SerializeFrom(stream);
				m_track_EmitterExtents.SerializeFrom(stream);
				m_track_EmitterRadius.SerializeFrom(stream);
				m_track_EmitterMinTheta.SerializeFrom(stream);
				m_track_EmitterMaxTheta.SerializeFrom(stream);
				m_track_EmitterMinPhi.SerializeFrom(stream);
				m_track_EmitterMaxPhi.SerializeFrom(stream);
				m_track_EmissionRate.SerializeFrom(stream);
				m_track_EmitDirection.SerializeFrom(stream);
				m_track_EmitDeviation.SerializeFrom(stream);
				m_track_ParticleMinExtinction.SerializeFrom(stream);
				m_track_ParticleMaxExtinction.SerializeFrom(stream);
				m_track_ParticleMinSpeed.SerializeFrom(stream);
				m_track_ParticleMaxSpeed.SerializeFrom(stream);
				m_track_ParticleMinScale.SerializeFrom(stream);
				m_track_ParticleMaxScale.SerializeFrom(stream);
				m_track_ParticleMinOrientation.SerializeFrom(stream);
				m_track_ParticleMaxOrientation.SerializeFrom(stream);
				m_track_Force0.SerializeFrom(stream);
				m_track_Force1.SerializeFrom(stream);
				m_track_CollectorPosition.SerializeFrom(stream);
				m_track_CollectorStrength.SerializeFrom(stream);
				m_track_CollectorFalloff0.SerializeFrom(stream);
				m_track_CollectorFalloff1.SerializeFrom(stream);
				m_track_CollectorFalloff2.SerializeFrom(stream);
			}

			break;

		case 11:
			{
				INT32 len = 0;

				stream->Read(&m_prtSysRadius, sizeof(REAL));

				stream->Read(&m_frameType, sizeof(RuParticleFrameType));
				stream->Read(&m_particlePathFrameType, sizeof(RuParticleFrameType));

				stream->Read(&m_particlePathRandomMirror, sizeof(BOOL));
				stream->Read(&m_particlePathRandomMirrorPlane, sizeof(RuOrthogonalPlane));

				::SerializeFrom(stream, &m_particlePathRotationAxis);
				stream->Read(&m_particlePathRotationDeviation, sizeof(REAL));

				// Read length of path file name including NULL, followed by the name
				stream->Read(&len, sizeof(INT32));
				m_particlePathName = ruNEW char [len];
				stream->Read(m_particlePathName, len);

				stream->Read(&m_particlePathLoopDelay, sizeof(REAL));

				stream->Read(&m_emitterPathFrameType, sizeof(RuFrameType));

				// Read length of path file name including NULL, followed by the name
				stream->Read(&len, sizeof(INT32));
				m_emitterPathName = ruNEW char [len];
				stream->Read(m_emitterPathName, len);

				stream->Read(&m_emitterPathLoopDelay, sizeof(REAL));
				stream->Read(&m_emitterPrerunTime, sizeof(REAL));

				stream->Read(&m_emitterOrientation, sizeof(RuOrthogonalAxis));

				// Serialize Tracks
				m_track_MaxParticles.SerializeFrom(stream);
				m_track_EmitterType.SerializeFrom(stream);
				m_track_EmitterPosition.SerializeFrom(stream);
				m_track_EmitterExtents.SerializeFrom(stream);
				m_track_EmitterRadius.SerializeFrom(stream);
				m_track_EmitterMinTheta.SerializeFrom(stream);
				m_track_EmitterMaxTheta.SerializeFrom(stream);
				m_track_EmitterMinPhi.SerializeFrom(stream);
				m_track_EmitterMaxPhi.SerializeFrom(stream);
				m_track_EmissionRate.SerializeFrom(stream);
				m_track_EmitDirection.SerializeFrom(stream);
				m_track_EmitDeviation.SerializeFrom(stream);
				m_track_ParticleMinExtinction.SerializeFrom(stream);
				m_track_ParticleMaxExtinction.SerializeFrom(stream);
				m_track_ParticleMinSpeed.SerializeFrom(stream);
				m_track_ParticleMaxSpeed.SerializeFrom(stream);
				m_track_ParticleMinScale.SerializeFrom(stream);
				m_track_ParticleMaxScale.SerializeFrom(stream);
				m_track_ParticleMinOrientation.SerializeFrom(stream);
				m_track_ParticleMaxOrientation.SerializeFrom(stream);
				m_track_Force0.SerializeFrom(stream);
				m_track_Force1.SerializeFrom(stream);
				m_track_CollectorPosition.SerializeFrom(stream);
				m_track_CollectorStrength.SerializeFrom(stream);
				m_track_CollectorFalloff0.SerializeFrom(stream);
				m_track_CollectorFalloff1.SerializeFrom(stream);
				m_track_CollectorFalloff2.SerializeFrom(stream);
			}

			break;
	}

	if(version < 11)
	{
		RuParticleEmitterType emitterType = m_track_EmitterType.GetDiscreteKey(0.0f);

		switch(emitterType)
		{
			default:
				{
					m_prtSysRadius = m_track_EmitterRadius.GetInterpolatedKey(0.0f);
				}

				break;

			case ruPRTEMITTERTYPE_VOLUME:
				{
					CRuVector3 emitterExtents = m_track_EmitterExtents.GetInterpolatedKey(0.0f);

					m_prtSysRadius = max(max(emitterExtents.x, emitterExtents.y), emitterExtents.z);
				}

				break;
		}
	}

	return TRUE;
}

BOOL CRuController_Particle_Emitter::ResetStates()
{
	m_age = 0.0f;

	return IRuEntity_KeyableController::ResetStates();
}

BOOL CRuController_Particle_Emitter::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 11;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeTo(stream);

	stream->Write(&m_prtSysRadius, sizeof(REAL));
	
	stream->Write(&m_frameType, sizeof(RuParticleFrameType));
	stream->Write(&m_particlePathFrameType, sizeof(RuParticleFrameType));

	stream->Write(&m_particlePathRandomMirror, sizeof(BOOL));
	stream->Write(&m_particlePathRandomMirrorPlane, sizeof(RuOrthogonalPlane));

	::SerializeTo(stream, &m_particlePathRotationAxis);
	stream->Write(&m_particlePathRotationDeviation, sizeof(REAL));

	// Write length of path file name including NULL, followed by the name
	if(m_particlePathName)
	{
		INT32 len = (INT32)strlen(m_particlePathName) + 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write(m_particlePathName, len);
	}
	else
	{
		INT32 len = 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write("", len);
	}

	stream->Write(&m_particlePathLoopDelay, sizeof(REAL));

	stream->Write(&m_emitterPathFrameType, sizeof(RuFrameType));

	// Write length of path file name including NULL, followed by the name
	if(m_emitterPathName)
	{
		INT32 len = (INT32)strlen(m_emitterPathName) + 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write(m_emitterPathName, len);
	}
	else
	{
		INT32 len = 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write("", len);
	}

	stream->Write(&m_emitterPathLoopDelay, sizeof(REAL));
	stream->Write(&m_emitterPrerunTime, sizeof(REAL));

	stream->Write(&m_emitterOrientation, sizeof(RuOrthogonalAxis));

	// Serialize Tracks
	m_track_MaxParticles.SerializeTo(stream);
	m_track_EmitterType.SerializeTo(stream);
	m_track_EmitterPosition.SerializeTo(stream);
	m_track_EmitterExtents.SerializeTo(stream);
	m_track_EmitterRadius.SerializeTo(stream);
	m_track_EmitterMinTheta.SerializeTo(stream);
	m_track_EmitterMaxTheta.SerializeTo(stream);
	m_track_EmitterMinPhi.SerializeTo(stream);
	m_track_EmitterMaxPhi.SerializeTo(stream);
	m_track_EmissionRate.SerializeTo(stream);
	m_track_EmitDirection.SerializeTo(stream);
	m_track_EmitDeviation.SerializeTo(stream);
	m_track_ParticleMinExtinction.SerializeTo(stream);
	m_track_ParticleMaxExtinction.SerializeTo(stream);
	m_track_ParticleMinSpeed.SerializeTo(stream);
	m_track_ParticleMaxSpeed.SerializeTo(stream);
	m_track_ParticleMinScale.SerializeTo(stream);
	m_track_ParticleMaxScale.SerializeTo(stream);
	m_track_ParticleMinOrientation.SerializeTo(stream);
	m_track_ParticleMaxOrientation.SerializeTo(stream);
	m_track_Force0.SerializeTo(stream);
	m_track_Force1.SerializeTo(stream);
	m_track_CollectorPosition.SerializeTo(stream);
	m_track_CollectorStrength.SerializeTo(stream);
	m_track_CollectorFalloff0.SerializeTo(stream);
	m_track_CollectorFalloff1.SerializeTo(stream);
	m_track_CollectorFalloff2.SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_Particle_Emitter::Update(REAL elapsedTime)
{
	if(m_enabled)
	{
/*
		if(m_particleEmitter)
		{
			REAL relativeT = m_age - m_startTime;

			// If looping is enabled, clamp time range to the duration
			if(relativeT > m_duration && m_duration > 0.0f && m_loopTime >= 0.0f)
			{
				relativeT = fmod(relativeT, m_duration);
			}

			m_particleEmitter->SetMaxParticles(m_track_MaxParticles.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetFrameType(m_frameType);
			m_particleEmitter->SetEmitterType(m_track_EmitterType.GetDiscreteKey(relativeT));
			m_particleEmitter->SetEmitterPosition(m_track_EmitterPosition.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetEmitterExtents(m_track_EmitterExtents.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetEmitterRadius(m_track_EmitterRadius.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetEmissionRate(m_track_EmissionRate.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetMinSpeed(m_track_ParticleMinSpeed.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetMaxSpeed(m_track_ParticleMaxSpeed.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetDirection(m_track_EmitDirection.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetDeviation(m_track_EmitDeviation.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetMinExtinction(m_track_ParticleMinExtinction.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetMaxExtinction(m_track_ParticleMaxExtinction.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetMinScale(m_track_ParticleMinScale.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetMaxScale(m_track_ParticleMaxScale.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetMinOrientation(m_track_ParticleMinOrientation.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetMaxOrientation(m_track_ParticleMaxOrientation.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetForce0(m_track_Force0.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetForce1(m_track_Force1.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetCollectorPosition(m_track_CollectorPosition.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetCollectorStrength(m_track_CollectorStrength.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetCollectorFalloff0(m_track_CollectorFalloff0.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetCollectorFalloff1(m_track_CollectorFalloff1.GetInterpolatedKey(relativeT));
			m_particleEmitter->SetCollectorFalloff2(m_track_CollectorFalloff2.GetInterpolatedKey(relativeT));
		}
*/
	}

	// Chain update call to base controller class
	return IRuEntity_KeyableController::Update(elapsedTime);
}

IRuObject *CRuController_Particle_Emitter::GetTarget()
{
	return m_particleEmitter;
}

BOOL CRuController_Particle_Emitter::SetTarget(IRuObject *targetEntity)
{
	CRuEntity_Particle_Emitter *emitter = NULL;

	if(targetEntity->GetType().IsSubClassOf(IRuEntity_Particle_Visualizer::Type()))
	{
		IRuEntity_Particle_Emitter *emitterInterface = static_cast<IRuEntity_Particle_Visualizer *>(targetEntity)->GetEmitter();

		if(emitterInterface->GetType() == CRuEntity_Particle_Emitter::Type())
		{
			emitter = static_cast<CRuEntity_Particle_Emitter *>(emitterInterface);
		}
	}
	else if(targetEntity->GetType() == CRuEntity_Particle_Emitter::Type())
	{
		emitter = static_cast<CRuEntity_Particle_Emitter *>(targetEntity);
	}

	if(emitter)
	{
		// If we had a previous target, unregister it
		if(m_particleEmitter)
		{
			m_particleEmitter->Event_Dispose().UnregisterHandler(this->GetGUID());
			m_particleEmitter->Event_UpdateStep().UnregisterHandler(this->GetGUID());
		}

		// Store target reference, setup event listener, and setup target parameter block
		m_particleEmitter = emitter;
		m_particleEmitter->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Particle_Emitter::HandleEvent_Dispose, this, _1), NULL);
		m_particleEmitter->Event_UpdateStep().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Particle_Emitter::HandleEvent_UpdateStep, this, _1));

		// Setup particle path interpolator parameters
		m_particleEmitter->SetPathFrameType(m_particlePathFrameType);
		m_particleEmitter->SetPathRandomMirror(m_particlePathRandomMirror);
		m_particleEmitter->SetPathRandomMirrorPlane(m_particlePathRandomMirrorPlane);
		m_particleEmitter->SetPathRotationAxis(m_particlePathRotationAxis);
		m_particleEmitter->SetPathRotationDeviation(m_particlePathRotationDeviation);
		m_particleEmitter->SetPathInterpolator(NULL);

		if(m_particlePathName && strlen(m_particlePathName) > 0)
		{
			CRuAnimation_GR2 *animationGR2 = g_ruResourceManager->LoadAnimation_GR2(m_particlePathName);

			if(animationGR2)
			{
				CRuAnimationInterpolator_GR2 *pathInterpolator = ruNEW CRuAnimationInterpolator_GR2();
				pathInterpolator->SetAnimation(animationGR2);
				pathInterpolator->SetLoopDelay(m_particlePathLoopDelay);
				m_particleEmitter->SetPathInterpolator(pathInterpolator);
				ruSAFE_RELEASE(pathInterpolator);
				ruSAFE_RELEASE(animationGR2);
			}
			else
			{
				CRuAnimation *animation = g_ruResourceManager->LoadAnimation(m_particlePathName);
				if(animation)
				{
					CRuAnimationInterpolator *pathInterpolator = ruNEW CRuAnimationInterpolator(1);
					pathInterpolator->PlayAnimation(animation, 0.0f, 0.0f, 0.0f, m_particlePathLoopDelay);
					m_particleEmitter->SetPathInterpolator(pathInterpolator);
					ruSAFE_RELEASE(pathInterpolator);
					ruSAFE_RELEASE(animation);
				}
			}
		}

		// Setup emitter path interpolator parameters
		m_particleEmitter->SetEmitterPathFrameType(m_emitterPathFrameType);
		m_particleEmitter->SetEmitterPathInterpolator(NULL);

		if(m_emitterPathName && strlen(m_emitterPathName) > 0)
		{
			CRuAnimation_GR2 *animationGR2 = g_ruResourceManager->LoadAnimation_GR2(m_emitterPathName);

			if(animationGR2)
			{
				CRuAnimationInterpolator_GR2 *emitterPathInterpolator = ruNEW CRuAnimationInterpolator_GR2();
				emitterPathInterpolator->SetAnimation(animationGR2);
				emitterPathInterpolator->SetLoopDelay(m_emitterPathLoopDelay);
				m_particleEmitter->SetEmitterPathInterpolator(emitterPathInterpolator);
				ruSAFE_RELEASE(emitterPathInterpolator);
				ruSAFE_RELEASE(animationGR2);
			}
			else
			{
				CRuAnimation *animation = g_ruResourceManager->LoadAnimation(m_emitterPathName);
				if(animation)
				{
					CRuAnimationInterpolator *emitterPathInterpolator = ruNEW CRuAnimationInterpolator(1);
					emitterPathInterpolator->PlayAnimation(animation, 0.0f, 0.0f, 0.0f, m_emitterPathLoopDelay);
					m_particleEmitter->SetEmitterPathInterpolator(emitterPathInterpolator);
					ruSAFE_RELEASE(emitterPathInterpolator);
					ruSAFE_RELEASE(animation);
				}
			}
		}

		m_particleEmitter->SetEmitterOrientation(m_emitterOrientation);

		m_particleEmitter->SetEmitterPrerunTime(m_emitterPrerunTime);

		// If the emitter pre-run time is set, run a update cycle to update the controller's time
		if(m_emitterPrerunTime > 0.0f)
		{
			this->Update(m_emitterPrerunTime);
		}
	}

	return TRUE;
}

INT32 CRuController_Particle_Emitter::GetNumTracks() const
{
	return 27;
}

BOOL CRuController_Particle_Emitter::GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut)
{
	switch(trackIndex)
	{
		case 0:
			trackOut = &m_track_MaxParticles;
			return TRUE;

		case 1:
			trackOut = &m_track_EmitterType;
			return TRUE;

		case 2:
			trackOut = &m_track_EmitterPosition;
			return TRUE;

		case 3:
			trackOut = &m_track_EmitterExtents;
			return TRUE;

		case 4:
			trackOut = &m_track_EmitterRadius;
			return TRUE;

		case 5:
			trackOut = &m_track_EmitterMinTheta;
			return TRUE;

		case 6:
			trackOut = &m_track_EmitterMaxTheta;
			return TRUE;

		case 7:
			trackOut = &m_track_EmitterMinPhi;
			return TRUE;

		case 8:
			trackOut = &m_track_EmitterMaxPhi;
			return TRUE;

		case 9:
			trackOut = &m_track_EmissionRate;
			return TRUE;

		case 10:
			trackOut = &m_track_EmitDirection;
			return TRUE;

		case 11:
			trackOut = &m_track_EmitDeviation;
			return TRUE;

		case 12:
			trackOut = &m_track_ParticleMinExtinction;
			return TRUE;

		case 13:
			trackOut = &m_track_ParticleMaxExtinction;
			return TRUE;

		case 14:
			trackOut = &m_track_ParticleMinSpeed;
			return TRUE;

		case 15:
			trackOut = &m_track_ParticleMaxSpeed;
			return TRUE;

		case 16:
			trackOut = &m_track_ParticleMinScale;
			return TRUE;

		case 17:
			trackOut = &m_track_ParticleMaxScale;
			return TRUE;

		case 18:
			trackOut = &m_track_ParticleMinOrientation;
			return TRUE;

		case 19:
			trackOut = &m_track_ParticleMaxOrientation;
			return TRUE;

		case 20:
			trackOut = &m_track_Force0;
			return TRUE;

		case 21:
			trackOut = &m_track_Force1;
			return TRUE;

		case 22:
			trackOut = &m_track_CollectorPosition;
			return TRUE;

		case 23:
			trackOut = &m_track_CollectorStrength;
			return TRUE;

		case 24:
			trackOut = &m_track_CollectorFalloff0;
			return TRUE;

		case 25:
			trackOut = &m_track_CollectorFalloff1;
			return TRUE;

		case 26:
			trackOut = &m_track_CollectorFalloff2;
			return TRUE;
	}

	return FALSE;
}

BOOL CRuController_Particle_Emitter::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_particleEmitter)
	{
		// Unregister from target entity
		m_particleEmitter->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_particleEmitter->Event_UpdateStep().UnregisterHandler(this->GetGUID());
		m_particleEmitter = NULL;
	}

	delete[] m_particlePathName;
	m_particlePathName = NULL;

	delete[] m_emitterPathName;
	m_emitterPathName = NULL;

	return TRUE;
}

BOOL CRuController_Particle_Emitter::HandleEvent_UpdateStep(RuEventArgs *eventArgs)
{
	RuEntityEvent_UpdateStep_Args *updateArgs = static_cast<RuEntityEvent_UpdateStep_Args *>(eventArgs);

	// Update controller age
	m_age += updateArgs->m_elapsedTime;

	if(m_particleEmitter)
	{
		REAL relativeT = m_age - m_startTime;

		// If looping is enabled, clamp time range to the duration
		if(relativeT > m_duration && m_duration > 0.0f && m_loopTime >= 0.0f)
		{
			relativeT = fmod(relativeT, m_duration);
		}

		m_particleEmitter->SetMaxParticles(m_track_MaxParticles.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetParticleSystemRadius(m_prtSysRadius);
		m_particleEmitter->SetFrameType(m_frameType);
		m_particleEmitter->SetEmitterType(m_track_EmitterType.GetDiscreteKey(relativeT));
		m_particleEmitter->SetEmitterPosition(m_track_EmitterPosition.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetEmitterExtents(m_track_EmitterExtents.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetEmitterRadius(m_track_EmitterRadius.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetEmissionRate(m_track_EmissionRate.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMinSpeed(m_track_ParticleMinSpeed.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMaxSpeed(m_track_ParticleMaxSpeed.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetDirection(m_track_EmitDirection.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetDeviation(m_track_EmitDeviation.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMinExtinction(m_track_ParticleMinExtinction.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMaxExtinction(m_track_ParticleMaxExtinction.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMinScale(m_track_ParticleMinScale.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMaxScale(m_track_ParticleMaxScale.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMinOrientation(m_track_ParticleMinOrientation.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMaxOrientation(m_track_ParticleMaxOrientation.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetForce0(m_track_Force0.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetForce1(m_track_Force1.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetCollectorPosition(m_track_CollectorPosition.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetCollectorStrength(m_track_CollectorStrength.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetCollectorFalloff0(m_track_CollectorFalloff0.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetCollectorFalloff1(m_track_CollectorFalloff1.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetCollectorFalloff2(m_track_CollectorFalloff2.GetInterpolatedKey(relativeT));
	}

	return TRUE;
}

REAL CRuController_Particle_Emitter::GetParticleSystemRadius() const
{
	return m_prtSysRadius;
}

void CRuController_Particle_Emitter::SetParticleSystemRadius(REAL radius)
{
	m_prtSysRadius = radius;
}

RuParticleFrameType CRuController_Particle_Emitter::GetFrameType() const
{
	return m_frameType;
}

void CRuController_Particle_Emitter::SetFrameType(RuParticleFrameType frameType)
{
	m_frameType = frameType;
}

RuParticleFrameType CRuController_Particle_Emitter::GetParticlePathFrameType() const
{
	return m_particlePathFrameType;
}

void CRuController_Particle_Emitter::SetParticlePathFrameType(RuParticleFrameType frameType)
{
	m_particlePathFrameType = frameType;
}

BOOL CRuController_Particle_Emitter::GetParticlePathRandomMirror() const
{
	return m_particlePathRandomMirror;
}

void CRuController_Particle_Emitter::SetParticlePathRandomMirror(BOOL randomMirror)
{
	m_particlePathRandomMirror = randomMirror;
}

RuOrthogonalPlane CRuController_Particle_Emitter::GetParticlePathRandomMirrorPlane() const
{
	return m_particlePathRandomMirrorPlane;
}

void CRuController_Particle_Emitter::SetParticlePathRandomMirrorPlane(RuOrthogonalPlane mirrorPlane)
{
	m_particlePathRandomMirrorPlane = mirrorPlane;
}

const CRuVector3 &CRuController_Particle_Emitter::GetParticlePathRotationAxis() const
{
	return m_particlePathRotationAxis;
}

void CRuController_Particle_Emitter::SetParticlePathRotationAxis(const CRuVector3 &rotationAxis)
{
	if(rotationAxis.Magnitude() > ruEPSILON)
	{
		m_particlePathRotationAxis = rotationAxis;
		m_particlePathRotationAxis.Normalize();
	}
}

REAL CRuController_Particle_Emitter::GetParticlePathRotationDeviation() const
{
	return m_particlePathRotationDeviation;
}

void CRuController_Particle_Emitter::SetParticlePathRotationDeviation(REAL rotationDeviation)
{
	m_particlePathRotationDeviation = rotationDeviation;
}

const char *CRuController_Particle_Emitter::GetParticlePathName() const
{
	if(m_particlePathName)
	{
		return m_particlePathName;
	}

	return "";
}

void CRuController_Particle_Emitter::SetParticlePathName(const char *particlePathName)
{
	if(particlePathName)
	{
		if(m_particlePathName)
		{
			delete[] m_particlePathName;
		}

		m_particlePathName = ruNEW char [strlen(particlePathName) + 1];
		strcpy(m_particlePathName, particlePathName);
	}
}

REAL CRuController_Particle_Emitter::GetParticlePathLoopDelay() const
{
	return m_particlePathLoopDelay;
}

void CRuController_Particle_Emitter::SetParticlePathLoopDelay(REAL loopDelay)
{
	m_particlePathLoopDelay = loopDelay;
}

RuFrameType CRuController_Particle_Emitter::GetEmitterPathFrameType() const
{
	return m_emitterPathFrameType;
}

void CRuController_Particle_Emitter::SetEmitterPathFrameType(RuFrameType frameType)
{
	m_emitterPathFrameType = frameType;
}

const char *CRuController_Particle_Emitter::GetEmitterPathName() const
{
	if(m_emitterPathName)
	{
		return m_emitterPathName;
	}

	return "";
}

void CRuController_Particle_Emitter::SetEmitterPathName(const char *emitterPathName)
{
	if(emitterPathName)
	{
		if(m_emitterPathName)
		{
			delete[] m_emitterPathName;
		}

		m_emitterPathName = ruNEW char [strlen(emitterPathName) + 1];
		strcpy(m_emitterPathName, emitterPathName);
	}
}

REAL CRuController_Particle_Emitter::GetEmitterPathLoopDelay() const
{
	return m_emitterPathLoopDelay;
}

void CRuController_Particle_Emitter::SetEmitterPathLoopDelay(REAL loopDelay)
{
	m_emitterPathLoopDelay = loopDelay;
}

RuOrthogonalAxis CRuController_Particle_Emitter::GetEmitterOrientation() const
{
	return m_emitterOrientation;
}

void CRuController_Particle_Emitter::SetEmitterOrientation(RuOrthogonalAxis orientation)
{
	m_emitterOrientation = orientation;
}

REAL CRuController_Particle_Emitter::GetEmitterPrerunTime() const
{
	return m_emitterPrerunTime;
}

void CRuController_Particle_Emitter::SetEmitterPrerunTime(REAL prerunTime)
{
	m_emitterPrerunTime = prerunTime;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
