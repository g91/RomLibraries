#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	SHADER CONSTANTS
	----------------
	[ 27 -  30] Billboard transform
	[ 31 -  31] (u increment, v increment, start scale, end scale)
	[ 32 -  32] (256.0f, first instance dataset offset, constant rows per instance dataset, reciprocal of fade distance)
	[ 33 -  33] (0.0f, 1.0f, 0.0f, 0.0f)
	[ 34 -  34] (start r, start g, start b, start a)
	[ 35 -  35] (end r, end g, end b, end a)
	[ 36 -  36] Start of instance dataset
*/
class CRuShader_SMUNI_Particle_SpriteEx : public CRuShader_SMUNI_Base
{
protected:
	INT32									m_extraConstantCount;
	UINT32									m_numInstances;
	RuBillboardType							m_billboardType;
	RuOrthogonalAxis						m_billboardAxis;

	CRuMatrix4x4*							m_viewTransform;

public:
											CRuShader_SMUNI_Particle_SpriteEx(CRuEngineD3D *engine);
	virtual									~CRuShader_SMUNI_Particle_SpriteEx();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
	virtual INT32							GetShaderClass() const;
	virtual INT32							GetShaderPriority() const;
	virtual DWORD							GetCaps() const;

	// Multi-Pass
	virtual INT32							GetNumPasses() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
	virtual void							EndPass(INT32 passNumber);

	// Shader texture placement query
	virtual INT32							GetNumSamplers(INT32 passNumber) const;
	virtual RuTextureChannel				GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const;

	// Shader parameter setup
	virtual void							SetParameters(const CRuParameterCache *paramCache);
};

// ************************************************************************************************************************************************************

inline const char *CRuShader_SMUNI_Particle_SpriteEx::GetShaderName() const
{
	return "ruSHADER_PARTICLE_SPRITEEX";
}

inline INT32 CRuShader_SMUNI_Particle_SpriteEx::GetShaderClass() const
{
	return 2000;
}

inline INT32 CRuShader_SMUNI_Particle_SpriteEx::GetShaderPriority() const
{
	return 1100;
}

inline DWORD CRuShader_SMUNI_Particle_SpriteEx::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SMUNI_Particle_SpriteEx::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SMUNI_Particle_SpriteEx::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SMUNI_Particle_SpriteEx::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_DIFFUSEMAP;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
