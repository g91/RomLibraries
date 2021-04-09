/*!
	@file RuEntity_Particle_Sprite.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Particle/RuEntity_Particle_Sprite.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Particle_Sprite, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Particle_Sprite, "CRuEntity_Particle_Sprite", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuEntity_Particle_Sprite, 0xF09DC2AF, 0x1FDE4a60, 0xA382ABF6, 0x99079C8D)

// ************************************************************************************************************************************************************

CRuEntity_Particle_Sprite::CRuEntity_Particle_Sprite()
:	m_emitterType(ruPRTEMITTERTYPE_POINT),
	m_emitterPosition(0.0f, 0.0f, 0.0f),
	m_emitterExtents(0.0f, 0.0f, 0.0f),
	m_emitterRadius(0.0f),
	m_emitterMotionType(ruPRTEMITTERMOTIONTYPE_STATIC),
	m_accumT(0.0f),
	m_minExtinctionT(1.0f),
	m_maxExtinctionT(1.0f),
	m_emissionRate(100.0f),
	m_minSpeed(0.0f),
	m_maxSpeed(1.0f),
	m_emitDirection(0.0f, 1.0f, 0.0f),
	m_deviationAxis0(1.0f, 0.0f, 0.0f),
	m_deviationAxis1(0.0f, 0.0f, 1.0f),
	m_emitDeviation(0.0f),
	m_startColor(0xFFFFFFFF),
	m_endColor(0xFFFFFFFF),
	m_startScale(1.0f),
	m_endScale(1.0f),
	m_spriteTextureName(NULL),
	m_spriteTexture(NULL),
	m_spriteSubTextureWidth(16),
	m_spriteSubTextureHeight(16),
	m_numFrames(1),
	m_frameRate(1.0f)
{
	m_maxParticlesPerBatch = 100;

	m_mesh = ruNEW CRuMesh();
	m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, 4 * m_maxParticlesPerBatch, 2 * m_maxParticlesPerBatch, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetDiffuse();
	UINT16 *indices = m_mesh->GetIndices();
	
	for(INT32 i = 0, vIdx = 0, fIdx = 0; i < m_maxParticlesPerBatch; ++i)
	{
		// v0
		diffuse[vIdx] = (i << 24) | (0x00 << 16) | (0x00 << 8);
		position[vIdx].Set(0.5f, 0.5f, 0.0f);
		++vIdx;

		// v1
		diffuse[vIdx] = (i << 24) | (0x00 << 16) | (0xFF << 8);
		position[vIdx].Set(0.5f, -0.5f, 0.0f);
		++vIdx;

		// v2
		diffuse[vIdx] = (i << 24) | (0xFF << 16) | (0xFF << 8);
		position[vIdx].Set(-0.5f, -0.5f, 0.0f);
		++vIdx;

		// v3
		diffuse[vIdx] = (i << 24) | (0xFF << 16) | (0x00 << 8);
		position[vIdx].Set(-0.5f, 0.5f, 0.0f);
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

CRuEntity_Particle_Sprite::~CRuEntity_Particle_Sprite()
{
	// Clear all batches
	ClearBatches();

	// Release mesh
	ruSAFE_RELEASE(m_mesh);

	// Release texture name string
	delete m_spriteTextureName;

	// Release sprite texture
	ruSAFE_RELEASE(m_spriteTexture);

	// Release resources used by batch structures
	while(m_freeBatchDescStack.IsEmpty() == FALSE)
	{
		delete m_freeBatchDescStack.Pop();
	}
}

IRuObject *CRuEntity_Particle_Sprite::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_Particle_Sprite();

	// Chain cloning call to parent class
	IRuEntity_Renderable::Clone(clonedObject);

	// Shortcut reference to the cloned entity
	CRuEntity_Particle_Sprite *clonedPSys = static_cast<CRuEntity_Particle_Sprite *>(clonedObject);

	clonedPSys->m_maxParticlesPerBatch = m_maxParticlesPerBatch;

	clonedPSys->m_maxParticles = m_maxParticles;
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
	clonedPSys->m_numFrames = m_numFrames;
	clonedPSys->m_frameRate = m_frameRate;

	clonedPSys->m_blendType = m_blendType;
	clonedPSys->m_frameType = m_frameType;

	clonedPSys->m_emitterType = m_emitterType;
	clonedPSys->m_emitterPosition = m_emitterPosition;
	clonedPSys->m_emitterExtents = m_emitterExtents;
	clonedPSys->m_emitterRadius = m_emitterRadius;

	clonedPSys->m_emitterMotionType = m_emitterMotionType;
	clonedPSys->m_emitterSpeed = m_emitterSpeed;

	clonedPSys->m_accumT = 0.0f;
	clonedPSys->m_minExtinctionT = m_minExtinctionT;
	clonedPSys->m_maxExtinctionT = m_maxExtinctionT;
	clonedPSys->m_emissionRate = m_emissionRate;

	clonedPSys->m_minSpeed = m_minSpeed;
	clonedPSys->m_maxSpeed = m_maxSpeed;

	clonedPSys->m_emitDirection = m_emitDirection;
	clonedPSys->m_deviationAxis0 = m_deviationAxis0;
	clonedPSys->m_deviationAxis1 = m_deviationAxis1;
	clonedPSys->m_emitDeviation = m_emitDeviation;
	clonedPSys->m_startColor = m_startColor;
	clonedPSys->m_endColor = m_endColor;
	clonedPSys->m_startScale = m_startScale;
	clonedPSys->m_endScale = m_endScale;

	clonedPSys->m_force0 = m_force0;
	clonedPSys->m_force1 = m_force1;

	return clonedPSys;
}

BOOL CRuEntity_Particle_Sprite::SerializeFrom(IRuStream *stream)
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

	stream->Read(&m_maxParticles, sizeof(INT32));

	// Read length of texture name including NULL, followed by the name
	INT32 len = 0;
	stream->Read(&len, sizeof(INT32));
	m_spriteTextureName = ruNEW char [len];
	stream->Read(m_spriteTextureName, len);

	stream->Read(&m_spriteSubTextureWidth, sizeof(INT32));
	stream->Read(&m_spriteSubTextureHeight, sizeof(INT32));
	stream->Read(&m_numFrames, sizeof(INT32));
	stream->Read(&m_frameRate, sizeof(REAL));
	stream->Read(&m_blendType, sizeof(INT32));
	stream->Read(&m_frameType, sizeof(INT32));

	stream->Read(&m_emitterType, sizeof(INT32));
	stream->Read(&m_emitterPosition.x, sizeof(REAL));
	stream->Read(&m_emitterPosition.y, sizeof(REAL));
	stream->Read(&m_emitterPosition.z, sizeof(REAL));
	stream->Read(&m_emitterExtents.x, sizeof(REAL));
	stream->Read(&m_emitterExtents.y, sizeof(REAL));
	stream->Read(&m_emitterExtents.z, sizeof(REAL));
	stream->Read(&m_emitterRadius, sizeof(REAL));

	stream->Read(&m_emitterMotionType, sizeof(INT32));
	stream->Read(&m_emitterSpeed, sizeof(REAL));

	stream->Read(&m_minExtinctionT, sizeof(REAL));
	stream->Read(&m_maxExtinctionT, sizeof(REAL));
	stream->Read(&m_emissionRate, sizeof(REAL));

	stream->Read(&m_minSpeed, sizeof(REAL));
	stream->Read(&m_maxSpeed, sizeof(REAL));

	stream->Read(&m_emitDirection.x, sizeof(REAL));
	stream->Read(&m_emitDirection.y, sizeof(REAL));
	stream->Read(&m_emitDirection.z, sizeof(REAL));
	stream->Read(&m_emitDeviation, sizeof(REAL));
	stream->Read(&m_startColor, sizeof(DWORD));
	stream->Read(&m_endColor, sizeof(DWORD));
	stream->Read(&m_startScale, sizeof(REAL));
	stream->Read(&m_endScale, sizeof(REAL));

	stream->Read(&m_force0.x, sizeof(REAL));
	stream->Read(&m_force0.y, sizeof(REAL));
	stream->Read(&m_force0.z, sizeof(REAL));
	stream->Read(&m_force1.x, sizeof(REAL));
	stream->Read(&m_force1.y, sizeof(REAL));
	stream->Read(&m_force1.z, sizeof(REAL));

	// Reload texture
	ruSAFE_RELEASE(m_spriteTexture);
	m_spriteTexture = g_ruResourceManager->LoadTexture(m_spriteTextureName, ruSURFACEFMT_DEFAULT, 0, TRUE);

	return TRUE;
}

BOOL CRuEntity_Particle_Sprite::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	stream->Write(&m_maxParticles, sizeof(INT32));

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

	stream->Write(&m_spriteSubTextureWidth, sizeof(INT32));
	stream->Write(&m_spriteSubTextureHeight, sizeof(INT32));
	stream->Write(&m_numFrames, sizeof(INT32));
	stream->Write(&m_frameRate, sizeof(REAL));
	stream->Write(&m_blendType, sizeof(INT32));
	stream->Write(&m_frameType, sizeof(INT32));

	stream->Write(&m_emitterType, sizeof(INT32));
	stream->Write(&m_emitterPosition.x, sizeof(REAL));
	stream->Write(&m_emitterPosition.y, sizeof(REAL));
	stream->Write(&m_emitterPosition.z, sizeof(REAL));
	stream->Write(&m_emitterExtents.x, sizeof(REAL));
	stream->Write(&m_emitterExtents.y, sizeof(REAL));
	stream->Write(&m_emitterExtents.z, sizeof(REAL));
	stream->Write(&m_emitterRadius, sizeof(REAL));

	stream->Write(&m_emitterMotionType, sizeof(INT32));
	stream->Write(&m_emitterSpeed, sizeof(REAL));

	stream->Write(&m_minExtinctionT, sizeof(REAL));
	stream->Write(&m_maxExtinctionT, sizeof(REAL));
	stream->Write(&m_emissionRate, sizeof(REAL));

	stream->Write(&m_minSpeed, sizeof(REAL));
	stream->Write(&m_maxSpeed, sizeof(REAL));

	stream->Write(&m_emitDirection.x, sizeof(REAL));
	stream->Write(&m_emitDirection.y, sizeof(REAL));
	stream->Write(&m_emitDirection.z, sizeof(REAL));
	stream->Write(&m_emitDeviation, sizeof(REAL));
	stream->Write(&m_startColor, sizeof(DWORD));
	stream->Write(&m_endColor, sizeof(DWORD));
	stream->Write(&m_startScale, sizeof(REAL));
	stream->Write(&m_endScale, sizeof(REAL));

	stream->Write(&m_force0.x, sizeof(REAL));
	stream->Write(&m_force0.y, sizeof(REAL));
	stream->Write(&m_force0.z, sizeof(REAL));
	stream->Write(&m_force1.x, sizeof(REAL));
	stream->Write(&m_force1.y, sizeof(REAL));
	stream->Write(&m_force1.z, sizeof(REAL));

	return TRUE;
}

BOOL CRuEntity_Particle_Sprite::ResetStates()
{
	return TRUE;
}

BOOL CRuEntity_Particle_Sprite::Update(REAL elapsedTime)
{
	REAL framePeriod = 1.0f / m_frameRate;

	CRuVector3 m_accel = m_force0 + m_force1;

	// Update time values
	if(m_emissionRate > 0.0f)
		m_accumT += elapsedTime;

	// Update particles
	for(INT32 i = 0; i < m_particles.Count(); ++i)
	{
		if(m_particles[i].m_active)
		{
			m_particles[i].m_velocity += m_accel * elapsedTime;
			m_particles[i].m_position += m_particles[i].m_velocity * elapsedTime;

			m_particles[i].m_age += elapsedTime;

			// If the particle's age is longer than the extinction time, deactivate it
			if(m_particles[i].m_age >= m_particles[i].m_extinction)
			{
				m_particles[i].m_active = FALSE;
				m_inactiveParticles.Push(i);
			}
			else
			{
				m_particles[i].m_timeSinceLastFrameUpdate += elapsedTime;

				if(m_particles[i].m_timeSinceLastFrameUpdate > framePeriod)
				{
					m_particles[i].m_timeSinceLastFrameUpdate = 0.0f;

					// Update texture index
					++m_particles[i].m_textureIndex;
					m_particles[i].m_textureIndex %= m_numFrames;
				}
			}
		}
	}

	// Move the emitter based on the motion type
	switch(m_emitterMotionType)
	{
		default:
		case ruPRTEMITTERMOTIONTYPE_STATIC:
			{
			}

			break;
	}

	INT32 particlesToGenerate = static_cast<INT32>(m_accumT * m_emissionRate);

	if(particlesToGenerate > 0)
	{
		CRuQuaternion quat0;
		CRuQuaternion quat1;
		REAL speedRange = m_maxSpeed - m_minSpeed;
		REAL extinctionRange = m_maxExtinctionT - m_minExtinctionT;

		// Reset accumulated T
		m_accumT = 0.0f;

		// Generate particles
		for(INT32 i = 0; i < particlesToGenerate; ++i)
		{
			INT32 particleIndex;
			if(m_inactiveParticles.IsEmpty())
			{
				particleIndex = m_particles.Count();
				m_particles.Add(CRuParticle_Sprite());
			}
			else
			{
				particleIndex = m_inactiveParticles.Pop();
			}

			// Generate particle position based on the emitter type
			switch(m_emitterType)
			{
				case ruPRTEMITTERTYPE_POINT:
					{
						m_particles[particleIndex].m_position.Set(m_currentEmitterPosition);
					}

					break;

				case ruPRTEMITTERTYPE_VOLUME:
					{
						m_particles[particleIndex].m_position.Set(	m_currentEmitterPosition.x + (RuGlobalMTRand().GenerateFloat() - 0.5f) * 2.0f * m_emitterExtents.x,
																	m_currentEmitterPosition.y + (RuGlobalMTRand().GenerateFloat() - 0.5f) * 2.0f * m_emitterExtents.y,
																	m_currentEmitterPosition.z + (RuGlobalMTRand().GenerateFloat() - 0.5f) * 2.0f * m_emitterExtents.z);
					}

					break;

				case ruPRTEMITTERTYPE_SPHERE:
					{
						m_particles[particleIndex].m_position.Set(m_currentEmitterPosition);
					}

					break;

				case ruPRTEMITTERTYPE_SHELL:
					{
						m_emitterMinTheta = 0.0f;
						m_emitterMaxTheta = ruPI * 2.0f;

						m_emitterMinPhi = 0.0f;
						m_emitterMaxPhi = ruPI * 2.0f;

						REAL theta = RuGlobalMTRand().GenerateFloat() * (m_emitterMaxTheta - m_emitterMinTheta) + m_emitterMinTheta;
						REAL phi = RuGlobalMTRand().GenerateFloat() * (m_emitterMaxPhi - m_emitterMinPhi) + m_emitterMinPhi;

						REAL sinPhi = sin(phi);
						REAL sinTheta = sin(theta);
						REAL cosPhi = cos(phi);
						REAL cosTheta = cos(theta);

						m_particles[particleIndex].m_position.Set(	m_currentEmitterPosition.x + m_emitterRadius * sinPhi * cosTheta,
																	m_currentEmitterPosition.y + m_emitterRadius * sinPhi * sinTheta,
																	m_currentEmitterPosition.z + m_emitterRadius * cosPhi);
					}

					break;
			}

			// Calculate normalized velocity
			CRuVector3 direction(m_emitDirection);
			quat0.FromAngleAxis(m_deviationAxis0, (RuGlobalMTRand().GenerateFloat() - 0.5f) * m_emitDeviation * 2.0f);
			quat1.FromAngleAxis(m_deviationAxis1, (RuGlobalMTRand().GenerateFloat() - 0.5f) * m_emitDeviation * 2.0f);
			quat0 = quat0 * quat1;
//			direction = quat0 * direction;
			quat0.GetMatrix().TransformVector(direction);


			m_particles[particleIndex].m_velocity = direction * m_minSpeed + direction * (RuGlobalMTRand().GenerateFloat() * speedRange);

			m_particles[particleIndex].m_extinction = m_minExtinctionT + RuGlobalMTRand().GenerateFloat() * extinctionRange;
			m_particles[particleIndex].m_age = 0.0f;

			m_particles[particleIndex].m_timeSinceLastFrameUpdate = 0.0f;

			m_particles[particleIndex].m_scale = 0.5f;
			m_particles[particleIndex].m_textureIndex = 0;
			m_particles[particleIndex].m_active = TRUE;
		}
	}

	// Rebuild particle batches
	BuildBatches();

	switch(m_emitterType)
	{
		case ruPRTEMITTERTYPE_POINT:
			{
				this->SetLocalEntityBounds(CRuSphere(m_currentEmitterPosition, m_emitterRadius));
			}

			break;

		case ruPRTEMITTERTYPE_VOLUME:
			{
				this->SetLocalEntityBounds(CRuSphere(m_currentEmitterPosition, max(max(m_emitterExtents.x, m_emitterExtents.y), m_emitterExtents.z)));
			}

			break;

		case ruPRTEMITTERTYPE_SPHERE:
			{
				this->SetLocalEntityBounds(CRuSphere(m_currentEmitterPosition, m_emitterRadius));
			}

			break;

		case ruPRTEMITTERTYPE_SHELL:
			{
				this->SetLocalEntityBounds(CRuSphere(m_currentEmitterPosition, m_emitterRadius));
			}

			break;
	}

	return CRuEntity::Update(elapsedTime);
}

INT32 CRuEntity_Particle_Sprite::GetNumMeshes() const
{
	return m_batches.Count();
}

BOOL CRuEntity_Particle_Sprite::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex >= 0 && meshIndex < m_batches.Count())
	{
		*meshOut = m_batches[meshIndex]->m_mesh;
		*materialOut = m_batches[meshIndex]->m_material;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_Particle_Sprite::QueryCollision(CRuCollisionQuery *colQuery)
{
	// Save previous collision transform
	CRuMatrix4x4 *originalWorldTransform = colQuery->m_worldTransform;
	CRuMatrix4x4 *originalInvWorldTransform = colQuery->m_invWorldTransform;

	// Store the inverse of the current world transform into the collision query
	colQuery->m_worldTransform = const_cast<CRuMatrix4x4 *>(&this->GetWorldTransform());
	colQuery->m_invWorldTransform = const_cast<CRuMatrix4x4 *>(&this->GetInverseWorldTransform());

	switch(colQuery->m_queryType)
	{
		case ruCOLQUERY_RAY:
			{
				CRuCollisionQuery_Ray *rayQuery = static_cast<CRuCollisionQuery_Ray *>(colQuery);

				// Test ray with bounding box
				if(Intersects(rayQuery->m_colRay, this->GetWorldBounds()))
				{
					rayQuery->m_colPoints->Add(m_currentEmitterPosition);
					rayQuery->m_colDistances->Add(0.0f);
				}
			}

			break;
	}

	// Restore original collision transform
	colQuery->m_worldTransform = originalWorldTransform;
	colQuery->m_invWorldTransform = originalInvWorldTransform;

	return TRUE;
}

const char *CRuEntity_Particle_Sprite::GetTextureName()
{
	if(m_spriteTextureName)
	{
		return m_spriteTextureName;
	}

	return "";
}

void CRuEntity_Particle_Sprite::SetTextureName(const char *textureName)
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

INT32 CRuEntity_Particle_Sprite::GetSubTextureWidth() const
{
	return m_spriteSubTextureWidth;
}

void CRuEntity_Particle_Sprite::SetSubTextureWidth(INT32 width)
{
	if(width > 0)
	{
		m_spriteSubTextureWidth = width;
	}
}

INT32 CRuEntity_Particle_Sprite::GetSubTextureHeight() const
{
	return m_spriteSubTextureHeight;
}

void CRuEntity_Particle_Sprite::SetSubTextureHeight(INT32 height)
{
	if(height > 0)
	{
		m_spriteSubTextureHeight = height;
	}
}

REAL CRuEntity_Particle_Sprite::GetFrameRate() const
{
	return m_frameRate;
}

void CRuEntity_Particle_Sprite::SetFrameRate(REAL frameRate)
{
	if(frameRate > 0.0f)
	{
		m_frameRate = frameRate;
	}
}

INT32 CRuEntity_Particle_Sprite::GetNumFrames() const
{
	return m_numFrames;
}

void CRuEntity_Particle_Sprite::SetNumFrames(INT32 numFrames)
{
	if(numFrames > 0)
	{
		m_numFrames = numFrames;
	}
}

RuAlphaMode CRuEntity_Particle_Sprite::GetBlendType() const
{
	return m_blendType;
}

void CRuEntity_Particle_Sprite::SetBlendType(RuAlphaMode blendType)
{
	m_blendType = blendType;
}

RuParticleFrameType CRuEntity_Particle_Sprite::GetFrameType() const
{
	return m_frameType;
}

void CRuEntity_Particle_Sprite::SetFrameType(RuParticleFrameType frameType)
{
	m_frameType = frameType;
}

RuParticleEmitterType CRuEntity_Particle_Sprite::GetEmitterType() const
{
	return m_emitterType;
}

void CRuEntity_Particle_Sprite::SetEmitterType(RuParticleEmitterType emitterType)
{
	m_emitterType = emitterType;
}

const CRuVector3 &CRuEntity_Particle_Sprite::GetEmitterPosition() const
{
	return m_emitterPosition;
}

void CRuEntity_Particle_Sprite::SetEmitterPosition(CRuVector3 &position)
{
	m_emitterPosition = position;
}

const CRuVector3 &CRuEntity_Particle_Sprite::GetEmitterExtents() const
{
	return m_emitterExtents;
}

void CRuEntity_Particle_Sprite::SetEmitterExtents(CRuVector3 &extents)
{
	m_emitterExtents = extents;
}

REAL CRuEntity_Particle_Sprite::GetEmitterRadius() const
{
	return m_emitterRadius;
}

void CRuEntity_Particle_Sprite::SetEmitterRadius(REAL radius)
{
	m_emitterRadius = radius;
}

RuParticleEmitterMotionType CRuEntity_Particle_Sprite::GetEmitterMotionType() const
{
	return m_emitterMotionType;
}

void CRuEntity_Particle_Sprite::SetEmitterMotionType(RuParticleEmitterMotionType emitterMotionType)
{
	m_emitterMotionType = emitterMotionType;
}

REAL CRuEntity_Particle_Sprite::GetEmissionRate() const
{
	return m_emissionRate;
}

void CRuEntity_Particle_Sprite::SetEmissionRate(REAL emissionRate)
{
	m_emissionRate = emissionRate;
}

REAL CRuEntity_Particle_Sprite::GetMinSpeed() const
{
	return m_minSpeed;
}

void CRuEntity_Particle_Sprite::SetMinSpeed(REAL minSpeed)
{
	m_minSpeed = minSpeed;
}

REAL CRuEntity_Particle_Sprite::GetMaxSpeed() const
{
	return m_maxSpeed;
}

void CRuEntity_Particle_Sprite::SetMaxSpeed(REAL maxSpeed)
{
	m_maxSpeed = maxSpeed;
}

const CRuVector3 &CRuEntity_Particle_Sprite::GetDirection() const
{
	return m_emitDirection;
}

void CRuEntity_Particle_Sprite::SetDirection(CRuVector3 &direction)
{
	m_emitDirection = direction;

	if(m_emitDirection.Magnitude() <= ruEPSILON)
	{
		m_emitDirection.Set(0.0f, 1.0f, 0.0f);
		m_deviationAxis0.Set(1.0f, 0.0f, 0.0f);
		m_deviationAxis1.Set(0.0f, 0.0f, 1.0f);
	}
	else
	{
		m_emitDirection.Normalize();
		m_deviationAxis0.Set(1.0f, 0.0f, 0.0f);
		m_deviationAxis1 = CrossProduct(m_emitDirection, m_deviationAxis0);
		if(m_deviationAxis1.Magnitude() < ruEPSILON)
		{
			m_deviationAxis0.Set(0.0f, 1.0f, 0.0f);
			m_deviationAxis1 = CrossProduct(m_emitDirection, m_deviationAxis0);
		}

		m_deviationAxis0 = CrossProduct(m_emitDirection, m_deviationAxis1);

		m_deviationAxis0.Normalize();
		m_deviationAxis1.Normalize();
	}
}

REAL CRuEntity_Particle_Sprite::GetDeviation() const
{
	return m_emitDeviation;
}

void CRuEntity_Particle_Sprite::SetDeviation(REAL deviation)
{
	m_emitDeviation = deviation;
}

REAL CRuEntity_Particle_Sprite::GetMinExtinction() const
{
	return m_minExtinctionT;
}

void CRuEntity_Particle_Sprite::SetMinExtinction(REAL extinction)
{
	m_minExtinctionT = extinction;
}

REAL CRuEntity_Particle_Sprite::GetMaxExtinction() const
{
	return m_maxExtinctionT;
}

void CRuEntity_Particle_Sprite::SetMaxExtinction(REAL extinction)
{
	m_maxExtinctionT = extinction;
}

DWORD CRuEntity_Particle_Sprite::GetStartColor() const
{
	return m_startColor;
}

void CRuEntity_Particle_Sprite::SetStartColor(DWORD color)
{
	m_startColor = color;
}

DWORD CRuEntity_Particle_Sprite::GetEndColor() const
{
	return m_endColor;
}

void CRuEntity_Particle_Sprite::SetEndColor(DWORD color)
{
	m_endColor = color;
}

REAL CRuEntity_Particle_Sprite::GetStartScale() const
{
	return m_startScale;
}

void CRuEntity_Particle_Sprite::SetStartScale(REAL scale)
{
	m_startScale = scale;
}

REAL CRuEntity_Particle_Sprite::GetEndScale() const
{
	return m_endScale;
}

void CRuEntity_Particle_Sprite::SetEndScale(REAL scale)
{
	m_endScale = scale;
}

const CRuVector3 &CRuEntity_Particle_Sprite::GetForce0() const
{
	return m_force0;
}

void CRuEntity_Particle_Sprite::SetForce0(CRuVector3 &force)
{
	m_force0 = force;
}

const CRuVector3 &CRuEntity_Particle_Sprite::GetForce1() const
{
	return m_force1;
}

void CRuEntity_Particle_Sprite::SetForce1(CRuVector3 &force)
{
	m_force1 = force;
}

void CRuEntity_Particle_Sprite::BuildBatches()
{
	// Clear existing batches
	ClearBatches();

	// Calculate bounds....

	// Iterate through all particles and build batches
	BatchDesc *batch = NULL;
	INT32 particlesThisBatch = 0;
	INT32 currentTextureWidth = 0;
	INT32 currentTextureHeight = 0;
	for(INT32 i = 0, j = 24; i < m_particles.Count(); ++i)
	{
		// Store reference to the particle
		const CRuParticle_Sprite &particle = m_particles[i];

		if(particle.m_active == FALSE)
			continue;

		// Detect if there is a need for a new batch
		if(batch == NULL || particlesThisBatch >= m_maxParticlesPerBatch)
		{
			// Set number of particles for the previous batch if applicable
			if(batch)
			{
				batch->m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, particlesThisBatch);
				batch->m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, m_blendType);

				// Setup rendering batch
				batch->m_mesh->SetProxiedMesh(m_mesh, m_mesh->AddBatch(0, particlesThisBatch * 2));
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
				batch->m_vsConstants[2] = m_startScale;
				batch->m_vsConstants[3] = m_endScale;
			}
			else
			{
				batch->m_material->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, NULL);

				currentTextureWidth = 16;
				currentTextureHeight = 16;

				// Setup header constants
				batch->m_vsConstants[0] = 0.0f;
				batch->m_vsConstants[1] = 0.0f;
				batch->m_vsConstants[2] = 0.0f;
				batch->m_vsConstants[3] = 0.0f;
			}

			batch->m_vsConstants[4] = 256.0f;						// diffuse color scale factor - so we can get a proper integer index
			batch->m_vsConstants[5] = 2.0f;							// 2 constant rows per instance dataset
			batch->m_vsConstants[6] = 37.0f;						// Instance dataset begins at c33
			batch->m_vsConstants[7] = 1.0f / 480.0f;				// reciprocal fade distance

			batch->m_vsConstants[8] = 0.0f;							// normal x
			batch->m_vsConstants[9] = 1.0f;							// normal y
			batch->m_vsConstants[10] = 0.0f;						// normal z
			batch->m_vsConstants[11] = 0.0f;						// unused

			batch->m_vsConstants[12] = ((m_startColor >> 16) & 0xFF) / 255.0f;
			batch->m_vsConstants[13] = ((m_startColor >> 8) & 0xFF) / 255.0f;
			batch->m_vsConstants[14] = ((m_startColor >> 0) & 0xFF) / 255.0f;
			batch->m_vsConstants[15] = ((m_startColor >> 24) & 0xFF) / 255.0f;

			batch->m_vsConstants[16] = ((m_endColor >> 16) & 0xFF) / 255.0f;
			batch->m_vsConstants[17] = ((m_endColor >> 8) & 0xFF) / 255.0f;
			batch->m_vsConstants[18] = ((m_endColor >> 0) & 0xFF) / 255.0f;
			batch->m_vsConstants[19] = ((m_endColor >> 24) & 0xFF) / 255.0f;

			batch->m_vsConstants[20] =  1.00000000f;
			batch->m_vsConstants[21] = -0.16666667f;
			batch->m_vsConstants[22] =  0.00833333f;
			batch->m_vsConstants[23] = -0.00019841f;

			// Reset constant index
			particlesThisBatch = 0;
			j = 24;

			// Add batch to batch list
			m_batches.Add(batch);
		}

		INT32 subTexPerRow = max(1, currentTextureWidth / m_spriteSubTextureWidth);

		// Setup constant data for this particle
		batch->m_vsConstants[j++] = particle.m_position.x;												// position x
		batch->m_vsConstants[j++] = particle.m_position.y;												// position y
		batch->m_vsConstants[j++] = particle.m_position.z;												// position z
		batch->m_vsConstants[j++] = particle.m_scale;													// scale

		batch->m_vsConstants[j++] = static_cast<REAL>(particle.m_textureIndex % subTexPerRow);			// u index
		batch->m_vsConstants[j++] = static_cast<REAL>(particle.m_textureIndex / subTexPerRow);			// v index
		batch->m_vsConstants[j++] = particle.m_age / particle.m_extinction;								// t scale
		batch->m_vsConstants[j++] = 0.0f;

		// Increment particle count
		++particlesThisBatch;
	}

	if(batch)
	{
		batch->m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, particlesThisBatch);
		batch->m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, m_blendType);

		// Setup rendering batch
		batch->m_mesh->SetProxiedMesh(m_mesh, m_mesh->AddBatch(0, particlesThisBatch * 2));
	}

	// Set billboard parameters
	this->GetParameterBlock()->SetParameter(ruPARAMTYPE_FX_BILLBOARD, static_cast<UINT64>(ruBILLBOARDTYPE_UNCONSTRAINED));
	this->GetParameterBlock()->SetParameter(ruPARAMTYPE_DWORD_0, static_cast<UINT64>(ruAXIS_Z_POSITIVE));
}

void CRuEntity_Particle_Sprite::ClearBatches()
{
	// Push all used batches onto stack
	for(INT32 i = 0; i < m_batches.Count(); ++i)
	{
		m_freeBatchDescStack.Push(m_batches[i]);
	}

	// Clear batches
	m_batches.Clear();

	// Clear batches
	m_mesh->ClearBatches();
}

// ************************************************************************************************************************************************************

#pragma managed
