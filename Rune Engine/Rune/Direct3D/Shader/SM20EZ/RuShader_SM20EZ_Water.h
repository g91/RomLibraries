#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	VERTEX SHADER CONSTANTS
	-----------------------
	[ 27 -  30] Projected texture transform
	[ 31 -  31] (base color R, base color G, base color B, alpha factor)

	PIXEL SHADER CONSTANTS
	----------------------
	[  2 -   2] (reflect factor, refract factor, base factor, specular strength)
*/
class CRuShader_SM20EZ_Water_Base : public CRuShader_SM20EZ_Base
{
public:
											CRuShader_SM20EZ_Water_Base(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Water_Base();

	// Shader capabilities query
	virtual INT32							GetShaderClass() const;
	virtual INT32							GetShaderPriority() const;
	virtual DWORD							GetCaps() const;

	// Shader parameter setup
	virtual void							SetParameters(const CRuParameterCache *paramCache);
};

// ************************************************************************************************************************************************************

class CRuShader_SM20EZ_Water_Full : public CRuShader_SM20EZ_Water_Base
{
public:
											CRuShader_SM20EZ_Water_Full(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Water_Full();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual INT32							GetNumPasses() const;
	virtual void							BeginPass(INT32 passNumber);
	virtual void							EndPass(INT32 passNumber);

	// Shader texture placement query
	virtual INT32							GetNumSamplers(INT32 passNumber) const;
	virtual RuTextureChannel				GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const;
};

// ************************************************************************************************************************************************************

class CRuShader_SM20EZ_Water_Minimal : public CRuShader_SM20EZ_Water_Base
{
public:
											CRuShader_SM20EZ_Water_Minimal(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Water_Minimal();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual INT32							GetNumPasses() const;
	virtual void							BeginPass(INT32 passNumber);
	virtual void							EndPass(INT32 passNumber);

	// Shader texture placement query
	virtual INT32							GetNumSamplers(INT32 passNumber) const;
	virtual RuTextureChannel				GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const;
};

// ************************************************************************************************************************************************************

class CRuShader_SM20EZ_Water_Lava : public CRuShader_SM20EZ_Water_Base
{
public:
											CRuShader_SM20EZ_Water_Lava(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Water_Lava();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual INT32							GetNumPasses() const;
	virtual void							BeginPass(INT32 passNumber);
	virtual void							EndPass(INT32 passNumber);

	// Shader texture placement query
	virtual INT32							GetNumSamplers(INT32 passNumber) const;
	virtual RuTextureChannel				GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const;
};

// ************************************************************************************************************************************************************

inline INT32 CRuShader_SM20EZ_Water_Base::GetShaderClass() const
{
	return 1500;
}

inline INT32 CRuShader_SM20EZ_Water_Base::GetShaderPriority() const
{
	return 1000;
}

inline DWORD CRuShader_SM20EZ_Water_Base::GetCaps() const
{
	return 0x00000000;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Water_Full::GetShaderName() const
{
	return "ruSHADER_WATER_FULL";
}

inline INT32 CRuShader_SM20EZ_Water_Full::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_Water_Full::GetNumSamplers(INT32 passNumber) const
{
	return 3;
}

inline RuTextureChannel CRuShader_SM20EZ_Water_Full::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	switch(samplerNumber)
	{
		case 0:
			return ruTEXCHANNEL_REFLECTIONMAP;
		case 1:
			return ruTEXCHANNEL_REFRACTIONMAP;
		case 2:
			return ruTEXCHANNEL_NORMALMAP;
	}

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Water_Minimal::GetShaderName() const
{
	return "ruSHADER_WATER_MINIMAL";
}

inline INT32 CRuShader_SM20EZ_Water_Minimal::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_Water_Minimal::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SM20EZ_Water_Minimal::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	switch(samplerNumber)
	{
		case 0:
			return ruTEXCHANNEL_NORMALMAP;
	}

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Water_Lava::GetShaderName() const
{
	return "ruSHADER_WATER_LAVA";
}

inline INT32 CRuShader_SM20EZ_Water_Lava::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_Water_Lava::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SM20EZ_Water_Lava::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	switch(samplerNumber)
	{
		case 0:
			return ruTEXCHANNEL_NORMALMAP;

		case 1:
			return ruTEXCHANNEL_GLOWMAP;
	}

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
