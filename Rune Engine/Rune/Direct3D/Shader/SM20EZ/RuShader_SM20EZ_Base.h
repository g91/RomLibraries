#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/* 
	NOTES
	-----
	Base vertex shader constants takes up a total of 30 registers (120 constants). The breakdown:

	[ 10 -  10] Ambient color
	[ 11 -  19] SH9 coefficients
	[ 20 -  20] Constant terms for SH9 and other purposes, defined as: (3.0, 1.0, 0.5, 0.0)
	[ 21 -  21] Specular direction (In view space?)
	[ 22 -  22] Specular color
	[ 23 -  23] (Self-illumination R, G, B, Self-illumination strength)
	[ 24 -  24] Reserved (for additional lighting)
	[ 25 -  25] Global settings, defined as: (Fog Constant, Fog Start, Fog End, Extrude Scale)
	[ 26 -  26] Global diffuse color modifier, defined as (R, G, B, A)
	[ 27 -  29] Reserved for use by subclasses

	Base pixel shader constants breakdown:

	[  0 -   0] Constants, defined as: (1.0, 0.5, 0.0, 0.05)
	[  1 -   1] Fog Color

*/
class CRuShader_SM20EZ_Base : public CRuShader_SMUNI_Base
{
public:
											CRuShader_SM20EZ_Base(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Base();

	virtual INT32							GetShaderClass() const;
	virtual INT32							GetShaderPriority() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
	virtual void							EndPass(INT32 passNumber);

	// Shader parameter setup
	virtual void							SetParameters(const CRuParameterCache *paramCache);
};

// ************************************************************************************************************************************************************

/*
	[ 27 -  30] Texture transform 0
*/
class CRuShader_SM20EZ_Static_Base : public CRuShader_SM20EZ_Base
{
protected:
	CRuMatrix4x4							m_identityMatrix;

public:
											CRuShader_SM20EZ_Static_Base(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Static_Base();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
	virtual INT32							GetShaderPriority() const;
	virtual DWORD							GetCaps() const;

	// Multi-Pass
	virtual INT32							GetNumPasses() const;

	// Shader texture placement query
	virtual INT32							GetNumSamplers(INT32 passNumber) const;
	virtual RuTextureChannel				GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const;

	// Shader parameter setup
	virtual void							SetDefaultParameters();
	virtual void							SetParameters(const CRuParameterCache *paramCache);
};

// ************************************************************************************************************************************************************

/*
	[ 27 -  30] Texture transform 0
*/
class CRuShader_SM20EZ_Skin_Base : public CRuShader_SM20EZ_Base
{
protected:
	// Skinning parameters
	INT32									m_maxSubFrames;								// Maximum number of bones the shader is able to support
	INT32									m_numSubFrames;								// Number of bones being used
	CRuMatrix4x4*							m_skinTransforms;							// Skin transform matrices
	UINT16*									m_indexMap;									// Matrix palette indices

	CRuMatrix4x4							m_identityMatrix;

public:
											CRuShader_SM20EZ_Skin_Base(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Skin_Base();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
	virtual INT32							GetShaderPriority() const;
	virtual DWORD							GetCaps() const;

	// Multi-Pass
	virtual INT32							GetNumPasses() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);

	// Shader texture placement query
	virtual INT32							GetNumSamplers(INT32 passNumber) const;
	virtual RuTextureChannel				GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const;

	// Shader parameter setup
	virtual void							SetDefaultParameters();
	virtual void							SetParameters(const CRuParameterCache *paramCache);
};

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Static_Base::GetShaderName() const
{
	return "CRuShader_SM20EZ_Static_Base";
}

inline INT32 CRuShader_SM20EZ_Static_Base::GetShaderPriority() const
{
	return 1000;
}

inline DWORD CRuShader_SM20EZ_Static_Base::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Static_Base::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_Static_Base::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SM20EZ_Static_Base::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	if(passNumber == 0 && samplerNumber == 0)
		return ruTEXCHANNEL_DIFFUSEMAP;

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Skin_Base::GetShaderName() const
{
	return "ruSHADER_SKIN_BASE";
}

inline INT32 CRuShader_SM20EZ_Skin_Base::GetShaderPriority() const
{
	return 1000;
}

inline DWORD CRuShader_SM20EZ_Skin_Base::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Skin_Base::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_Skin_Base::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SM20EZ_Skin_Base::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	if(passNumber == 0 && samplerNumber == 0)
		return ruTEXCHANNEL_DIFFUSEMAP;

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

#pragma managed(push, off)
