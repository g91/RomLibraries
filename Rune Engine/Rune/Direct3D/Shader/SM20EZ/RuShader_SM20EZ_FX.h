#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	SHADER CONSTANTS
	----------------
*/
class CRuShader_SM20EZ_FX_GhostTrail : public CRuShader_SM20EZ_Base
{
protected:
	UINT32									m_numInstances;

public:
											CRuShader_SM20EZ_FX_GhostTrail(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_FX_GhostTrail();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
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

class CRuShader_SM20EZ_FX_Decal : public CRuShader_SM20EZ_Base
{
protected:
	UINT32									m_numInstances;

public:
											CRuShader_SM20EZ_FX_Decal(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_FX_Decal();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
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

class CRuShader_SM20EZ_FX_DecalShadow : public CRuShader_SM20EZ_Base
{
protected:
	UINT32									m_numInstances;

public:
											CRuShader_SM20EZ_FX_DecalShadow(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_FX_DecalShadow();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
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

class CRuShader_SM20EZ_FX_Lightning : public CRuShader_SM20EZ_Base
{
protected:
	UINT32									m_numInstances;

public:
											CRuShader_SM20EZ_FX_Lightning(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_FX_Lightning();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
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

inline const char *CRuShader_SM20EZ_FX_GhostTrail::GetShaderName() const
{
	return "ruSHADER_FX_GHOSTTRAIL";
}

inline INT32 CRuShader_SM20EZ_FX_GhostTrail::GetShaderPriority() const
{
	return 1500;
}

inline DWORD CRuShader_SM20EZ_FX_GhostTrail::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_FX_GhostTrail::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_FX_GhostTrail::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SM20EZ_FX_GhostTrail::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_DIFFUSEMAP;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_FX_Decal::GetShaderName() const
{
	return "ruSHADER_FX_DECAL";
}

inline INT32 CRuShader_SM20EZ_FX_Decal::GetShaderPriority() const
{
	return 1500;
}

inline DWORD CRuShader_SM20EZ_FX_Decal::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_FX_Decal::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_FX_Decal::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SM20EZ_FX_Decal::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_DIFFUSEMAP;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_FX_DecalShadow::GetShaderName() const
{
	return "ruSHADER_FX_DECALSHADOW";
}

inline INT32 CRuShader_SM20EZ_FX_DecalShadow::GetShaderPriority() const
{
	return 1500;
}

inline DWORD CRuShader_SM20EZ_FX_DecalShadow::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_FX_DecalShadow::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_FX_DecalShadow::GetNumSamplers(INT32 passNumber) const
{
	return 2;
}

inline RuTextureChannel CRuShader_SM20EZ_FX_DecalShadow::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	switch(samplerNumber)
	{
		case 0:
			return ruTEXCHANNEL_DIFFUSEMAP;
		case 1:
			return ruTEXCHANNEL_SYSTEM_CLAMP;
	}

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_FX_Lightning::GetShaderName() const
{
	return "ruSHADER_FX_LIGHTNING";
}

inline INT32 CRuShader_SM20EZ_FX_Lightning::GetShaderPriority() const
{
	return 1500;
}

inline DWORD CRuShader_SM20EZ_FX_Lightning::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_FX_Lightning::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_FX_Lightning::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SM20EZ_FX_Lightning::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_DIFFUSEMAP;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
