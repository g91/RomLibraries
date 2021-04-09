/*!
	@file RuEntity_Particle_Visualizer_Sprite.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_PARTICLE_VISUALIZER_SPRITE_H_
#define _RUENTITY_PARTICLE_VISUALIZER_SPRITE_H_

#include "../../Scene/Base/RuEntityModel.h"
#include "../../Scene/Particle/RuEntity_Particle_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuEntity_Particle_Visualizer_Sprite : public IRuEntity_Particle_Visualizer
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ParticleDesc
	{
		INT32							m_particleIteration;
		INT32							m_rotationDirection;

		ParticleDesc()
			:	m_particleIteration(0),
				m_rotationDirection(1)
		{
		}
	};

	class BatchDesc
	{
	public:
		INT32								m_numParticles;						//!< Number of particles in this batch
		REAL*								m_vsConstants;						//!< Vertex shader constants array
		CRuProxyMesh*						m_mesh;								//!< Proxy mesh
		CRuMaterial*						m_material;							//!< Material to use for the batch

	public:
		BatchDesc(INT32 maxParticlesPerBatch)
		{
			// Create constants array
			m_vsConstants = ruNEW REAL [12 * maxParticlesPerBatch + 36];

			// Create proxy mesh
			m_mesh = ruNEW CRuProxyMesh();

			// Create material and assign constants array to it
			m_material = ruNEW CRuMaterial();
			m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_PARTICLE_SPRITEEX"));
			m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, 0);
			m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_DATA, (UINT64) m_vsConstants);
		}

		~BatchDesc()
		{
			// Delete constants array
			delete[] m_vsConstants;

			// Release proxy mesh
			ruSAFE_RELEASE(m_mesh);

			// Release material
			ruSAFE_RELEASE(m_material);
		}
	};

	// Particle visualization settings
	BOOL									m_allowWorldRotation;
	RuOrthogonalAxis						m_orientation;						//!< Orientation of the particle polygon
	RuBillboardType							m_billboardType;					//!< Billboard type
	RuOrthogonalAxis						m_billboardAxis;					//!< Billboard axis
	RuAlphaMode								m_blendType;						//!< Blend mode

	char*									m_spriteTextureName;				//!< Sprite texture name
	IRuTexture*								m_spriteTexture;					//!< Sprite texture
	INT32									m_spriteSubTextureWidth;
	INT32									m_spriteSubTextureHeight;
	REAL									m_frameRate;
	INT32									m_numFrames;

	BOOL									m_orientToDirectionOfTravel;
	BOOL									m_randomRotation;

	CRuVector3								m_quadOffset;						//!< Sprite quad offset
	CRuVector3								m_quadScale;						//!< Sprite quad scale

	CRuAnimTrack<CRuColor4>					m_track_Color;						//!< Color track
	CRuAnimTrack<REAL>						m_track_Alpha;						//!< Alpha track
	CRuAnimTrack<REAL>						m_track_Scale;						//!< Scale track
	CRuAnimTrack<REAL>						m_track_RotationSpeed;				//!< 

	// Particle emitter
	IRuEntity_Particle_Emitter*				m_emitter;							//!< Particle emitter

	// Runtime data
	INT32									m_maxParticlesPerBatch;				//!< Maximum number of particles to draw per batch
	CRuMatrix4x4							m_broadcastWorldMatrix;
	CRuMatrix4x4							m_broadcastViewMatrix;
	CRuMatrix4x4							m_aggregateMatrix;

	REAL									m_timeSinceLastUpdate;
	REAL									m_updateInterval;

	CRuModel*								m_model;							//!< Particle system model
	CRuMesh*								m_mesh;								//!< Particle system mesh

	CRuArrayList<ParticleDesc>				m_particles;						//!< List of allocated particle entities
	CRuArrayList<BatchDesc *>				m_batches;							//!< Currently active batch list
	CRuStack<BatchDesc *>					m_freeBatchDescStack;				//!< Stack of unused batch descriptors

	INT32									m_iterationCount;

	INT32									m_probe_UpdateCount;
	INT32									m_probe_ValidUpdateCount;

public:
											CRuEntity_Particle_Visualizer_Sprite();
	virtual									~CRuEntity_Particle_Visualizer_Sprite();

	// Base entity interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);
	virtual BOOL							ResetStates();

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// Container interface
	virtual INT32							GetNumEntities();
	virtual CRuEntity*						GetEntity(INT32 index);

	// Visualizer interface
	virtual IRuEntity_Particle_Emitter*		GetEmitter();
	virtual BOOL							SetEmitter(IRuEntity_Particle_Emitter *emitter);

	// Event handlers
	BOOL									HandleEvent_Transform(RuEventArgs *eventArgs);

	void									SetUpdateInterval(REAL updateInterval);
	void									ResetUpdateTimer();

	void									SetAllowWorldRotation(BOOL allowWorldRotation);

	RuOrthogonalAxis						GetOrientation() const;
	void									SetOrientation(RuOrthogonalAxis orientation);

	RuBillboardType							GetBillboardType() const;
	void									SetBillboardType(RuBillboardType billboardType);

	RuOrthogonalAxis						GetBillboardAxis() const;
	void									SetBillboardAxis(RuOrthogonalAxis billboardAxis);

	RuAlphaMode								GetBlendType() const;
	void									SetBlendType(RuAlphaMode blendType);

	const char*								GetTextureName();
	void									SetTextureName(const char *textureName);

	INT32									GetSubTextureWidth() const;
	void									SetSubTextureWidth(INT32 width);

	INT32									GetSubTextureHeight() const;
	void									SetSubTextureHeight(INT32 height);

	REAL									GetFrameRate() const;
	void									SetFrameRate(REAL frameRate);

	INT32									GetNumFrames() const;
	void									SetNumFrames(INT32 numFrames);

	BOOL									GetOrientToDirectionOfTravel() const;
	void									SetOrientToDirectionOfTravel(BOOL orientToDirectionOfTravel);

	BOOL									GetRandomRotation() const;
	void									SetRandomRotation(BOOL randomRotation);

	const CRuVector3&						GetQuadOffset() const;
	void									SetQuadOffset(const CRuVector3 &quadOffset);

	const CRuVector3&						GetQuadScale() const;
	void									SetQuadScale(const CRuVector3 &quadScale);

	// Track access
	CRuAnimTrack<CRuColor4>&				GetTrack_Color();
	CRuAnimTrack<REAL>&						GetTrack_Alpha();
	CRuAnimTrack<REAL>&						GetTrack_Scale();
	CRuAnimTrack<REAL>&						GetTrack_RotationSpeed();

	// Deprecated
	DWORD									GetStartColor() const;
	void									SetStartColor(DWORD color);

	DWORD									GetEndColor() const;
	void									SetEndColor(DWORD color);

	REAL									GetStartAlpha() const;
	void									SetStartAlpha(REAL alpha);

	REAL									GetEndAlpha() const;
	void									SetEndAlpha(REAL alpha);

	REAL									GetStartScale() const;
	void									SetStartScale(REAL scale);

	REAL									GetEndScale() const;
	void									SetEndScale(REAL scale);

	REAL									GetRotationSpeed() const;
	void									SetRotationSpeed(REAL speed);

protected:
	void									BuildMesh();
	void									BuildBatches();
	void									ClearBatches();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_Particle_Visualizer_Sprite, CRuEntity_Particle_Visualizer_Sprite)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
