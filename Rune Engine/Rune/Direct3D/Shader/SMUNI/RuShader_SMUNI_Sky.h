#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_SkyDome : public CRuShader_SMUNI_Base
{
public:
											CRuShader_SMUNI_SkyDome(CRuEngineD3D *engine);
	virtual									~CRuShader_SMUNI_SkyDome();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
	virtual INT32							GetShaderClass() const;
	virtual INT32							GetShaderPriority() const;
	virtual DWORD							GetCaps() const;

	// Rendering setup
	virtual INT32							GetNumPasses() const;
	virtual void							BeginPass(INT32 passNumber);
	virtual void							EndPass(INT32 passNumber);

	// Shader texture placement query
	virtual INT32							GetNumSamplers(INT32 passNumber) const;
	virtual RuTextureChannel				GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const;

	// Shader parameter setup
	virtual void							SetParameters(const CRuParameterCache *paramCache);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_CloudDome : public CRuShader_SMUNI_Base
{
protected:
	UINT32									m_numInstances;

public:
											CRuShader_SMUNI_CloudDome(CRuEngineD3D *engine);
	virtual									~CRuShader_SMUNI_CloudDome();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
	virtual INT32							GetShaderClass() const;
	virtual INT32							GetShaderPriority() const;
	virtual DWORD							GetCaps() const;

	// Rendering setup
	virtual INT32							GetNumPasses() const;
	virtual void							BeginPass(INT32 passNumber);
	virtual void							EndPass(INT32 passNumber);

	// Shader texture placement query
	virtual INT32							GetNumSamplers(INT32 passNumber) const;
	virtual RuTextureChannel				GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const;

	// Shader parameter setup
	virtual void							SetParameters(const CRuParameterCache *paramCache);
};

// ************************************************************************************************************************************************************

inline const char *CRuShader_SMUNI_SkyDome::GetShaderName() const
{
	return "ruSHADER_SKYDOME";
}

inline INT32 CRuShader_SMUNI_SkyDome::GetShaderClass() const
{
	return 900;
}

inline INT32 CRuShader_SMUNI_SkyDome::GetShaderPriority() const
{
	return 1000;
}

inline DWORD CRuShader_SMUNI_SkyDome::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SMUNI_SkyDome::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SMUNI_SkyDome::GetNumSamplers(INT32 passNumber) const
{
	return 2;
}

inline RuTextureChannel CRuShader_SMUNI_SkyDome::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	switch(samplerNumber)
	{
		case 0:
			return ruTEXCHANNEL_SKYDOME_GRADIENT_0;
		case 1:
			return ruTEXCHANNEL_SKYDOME_GRADIENT_1;
	}

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SMUNI_CloudDome::GetShaderName() const
{
	return "ruSHADER_CLOUD_DOME";
}

inline INT32 CRuShader_SMUNI_CloudDome::GetShaderClass() const
{
	return 901;
}

inline INT32 CRuShader_SMUNI_CloudDome::GetShaderPriority() const
{
	return 1000;
}

inline DWORD CRuShader_SMUNI_CloudDome::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SMUNI_CloudDome::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SMUNI_CloudDome::GetNumSamplers(INT32 passNumber) const
{
	return 8;
}

inline RuTextureChannel CRuShader_SMUNI_CloudDome::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	switch(samplerNumber)
	{
		case 0:
			return ruTEXCHANNEL_CUSTOM_0;
		case 1:
			return ruTEXCHANNEL_CUSTOM_1;
		case 2:
			return ruTEXCHANNEL_CUSTOM_2;
		case 3:
			return ruTEXCHANNEL_CUSTOM_3;
		case 4:
			return ruTEXCHANNEL_CUSTOM_4;
		case 5:
			return ruTEXCHANNEL_CUSTOM_5;
		case 6:
			return ruTEXCHANNEL_CUSTOM_6;
		case 7:
			return ruTEXCHANNEL_CUSTOM_7;
	}

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
