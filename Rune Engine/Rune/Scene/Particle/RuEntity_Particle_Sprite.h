/*!
	@file RuEntity_Particle_Sprite.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_PARTICLE_SPRITE_H_
#define _RUENTITY_PARTICLE_SPRITE_H_

#include "../../Scene/Particle/RuEntity_Particle_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuParticle_Sprite
{
public:
	CRuVector3								m_position;							//!< Initial position
	CRuVector3								m_velocity;							//!< Current velocity
	REAL									m_angularVelocity;					//!< Angular velocity (rotational speed)
	REAL									m_extinction;						//!< Particle extinction value
	REAL									m_age;								//!< Particle age

	REAL									m_timeSinceLastFrameUpdate;

	REAL									m_scale;							//!< Particle scale
	UINT32									m_textureIndex;						//!< Texture index
	BOOL									m_active;							//!< Is the particle active?
};

// ************************************************************************************************************************************************************

class CRuEntity_Particle_Sprite : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
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
			m_vsConstants = ruNEW REAL [8 * maxParticlesPerBatch + 36];

			// Create proxy mesh
			m_mesh = ruNEW CRuProxyMesh();

			// Create material and assign constants array to it
			m_material = ruNEW CRuMaterial();
			m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_PARTICLE_SPRITE"));
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

	// Runtime data
	CRuVector3								m_currentEmitterPosition;			//!< Current emitter position

	INT32									m_maxParticlesPerBatch;				//!< Maximum number of particles to draw per batch

	INT32									m_maxParticles;
	char*									m_spriteTextureName;				//!< Sprite texture name
	IRuTexture*								m_spriteTexture;					//!< Sprite texture
	INT32									m_spriteSubTextureWidth;
	INT32									m_spriteSubTextureHeight;
	REAL									m_frameRate;
	INT32									m_numFrames;

	RuAlphaMode								m_blendType;						//!< Blend mode
	RuParticleFrameType						m_frameType;						//!< Frame of reference mode

	// Emitter settings
	RuParticleEmitterType					m_emitterType;						//!< Emitter type
	CRuVector3								m_emitterPosition;					//!< Original emitter position
	CRuVector3								m_emitterExtents;					//!< Emitter extents, used by volume emitters only
	REAL									m_emitterRadius;					//!< Emitter radius, used by sphere emitters only
	REAL									m_emitterMinTheta;					//!< Parameter specifying spherical coordinate constraints during particle generation
	REAL									m_emitterMaxTheta;					//!< Parameter specifying spherical coordinate constraints during particle generation
	REAL									m_emitterMinPhi;					//!< Parameter specifying spherical coordinate constraints during particle generation
	REAL									m_emitterMaxPhi;					//!< Parameter specifying spherical coordinate constraints during particle generation

	RuParticleEmitterMotionType				m_emitterMotionType;				//!< Emitter motion type
	REAL									m_emitterSpeed;						//!< Emitter speed

	REAL									m_accumT;							//!< Accumulated t during which no particles were generated
	REAL									m_minExtinctionT;					//!< Particle extinction time (minimum particle life)
	REAL									m_maxExtinctionT;					//!< Particle extinction time (maximum particle life)
	REAL									m_emissionRate;						//!< Number of particles to generate per second

	REAL									m_minSpeed;							//!< Minimum particle speed
	REAL									m_maxSpeed;							//!< Maximum particle speed

	CRuVector3								m_emitDirection;					//!< Emission direction
	CRuVector3								m_deviationAxis0;					//!< Deviation axis, calculated from m_emitDirection
	CRuVector3								m_deviationAxis1;					//!< Deviation axis, calculated from m_emitDirection
	REAL									m_emitDeviation;					//!< Deviation degree in radians
	DWORD									m_startColor;
	DWORD									m_endColor;
	REAL									m_startScale;
	REAL									m_endScale;

	CRuVector3								m_force0;
	CRuVector3								m_force1;

	CRuMesh*								m_mesh;								//!< Particle system mesh

	CRuArrayList<CRuParticle_Sprite>		m_particles;						//!< List of particles
	CRuStack<INT32>							m_inactiveParticles;				//!< Stack of indices to inactive particles

	CRuArrayList<BatchDesc *>				m_batches;							//!< Currently active batch list
	CRuStack<BatchDesc *>					m_freeBatchDescStack;				//!< Stack of unused batch descriptors

public:
											CRuEntity_Particle_Sprite();
	virtual									~CRuEntity_Particle_Sprite();

	// Base entity interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);
	virtual BOOL							ResetStates();

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	// IRuEntity_Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

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

	RuAlphaMode								GetBlendType() const;
	void									SetBlendType(RuAlphaMode blendType);

	RuParticleFrameType						GetFrameType() const;
	void									SetFrameType(RuParticleFrameType frameType);

	RuParticleEmitterType					GetEmitterType() const;
	void									SetEmitterType(RuParticleEmitterType emitterType);

	const CRuVector3&						GetEmitterPosition() const;
	void									SetEmitterPosition(CRuVector3 &position);

	const CRuVector3&						GetEmitterExtents() const;
	void									SetEmitterExtents(CRuVector3 &extents);

	REAL									GetEmitterRadius() const;
	void									SetEmitterRadius(REAL radius);

	RuParticleEmitterMotionType				GetEmitterMotionType() const;
	void									SetEmitterMotionType(RuParticleEmitterMotionType emitterMotionType);

	REAL									GetEmissionRate() const;
	void									SetEmissionRate(REAL emissionRate);

	REAL									GetMinSpeed() const;
	void									SetMinSpeed(REAL minSpeed);

	REAL									GetMaxSpeed() const;
	void									SetMaxSpeed(REAL maxSpeed);

	const CRuVector3&						GetDirection() const;
	void									SetDirection(CRuVector3 &direction);

	REAL									GetDeviation() const;
	void									SetDeviation(REAL deviation);

	REAL									GetMinExtinction() const;
	void									SetMinExtinction(REAL extinction);

	REAL									GetMaxExtinction() const;
	void									SetMaxExtinction(REAL extinction);

	DWORD									GetStartColor() const;
	void									SetStartColor(DWORD color);

	DWORD									GetEndColor() const;
	void									SetEndColor(DWORD color);

	REAL									GetStartScale() const;
	void									SetStartScale(REAL scale);

	REAL									GetEndScale() const;
	void									SetEndScale(REAL scale);

	const CRuVector3&						GetForce0() const;
	void									SetForce0(CRuVector3 &force);

	const CRuVector3&						GetForce1() const;
	void									SetForce1(CRuVector3 &force);

protected:
	void									BuildBatches();
	void									ClearBatches();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_Particle_Sprite, CRuEntity_Particle_Sprite)

// ************************************************************************************************************************************************************

#pragma managed

#endif
