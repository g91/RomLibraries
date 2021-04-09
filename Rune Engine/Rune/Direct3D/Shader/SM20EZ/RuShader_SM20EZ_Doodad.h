#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	SHADER CONSTANTS
	----------------
	[ 27 -  27] (u increment, v increment, current t)
	[ 28 -  28] (256.0f, constant rows per instance dataset, first instance dataset offset, reciprocal of fade distance)
	[ 29 -  29] (0.0f, 1.0f, 0.0f, 0.0f)
	[ 30 -  30] ( 3.14159265,  0.50000000,  6.28318531,  0.15915494) <-- Used for sin/cos calculation
	[ 31 -  31] ( 1.00000000, -0.50000000,  0.04166667, -0.00138889)
	[ 32 -  32] ( 1.00000000, -0.16666667,  0.00833333, -0.00019841)
	[ 33 -  33] Start of instance dataset
*/
class CRuShader_SM20EZ_Particle_Doodad : public CRuShader_SM20EZ_Base
{
protected:
	UINT32									m_numInstances;
	INT32									m_numConstantRows;

public:
											CRuShader_SM20EZ_Particle_Doodad(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Particle_Doodad();

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

inline const char *CRuShader_SM20EZ_Particle_Doodad::GetShaderName() const
{
	return "ruSHADER_PARTICLE_DOODAD";
}

inline INT32 CRuShader_SM20EZ_Particle_Doodad::GetShaderClass() const
{
	return 2000;
}

inline INT32 CRuShader_SM20EZ_Particle_Doodad::GetShaderPriority() const
{
	return 1000;
}

inline DWORD CRuShader_SM20EZ_Particle_Doodad::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Particle_Doodad::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_Particle_Doodad::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SM20EZ_Particle_Doodad::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_DIFFUSEMAP;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
