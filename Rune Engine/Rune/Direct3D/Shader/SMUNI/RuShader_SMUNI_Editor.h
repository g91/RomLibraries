#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_Editor_Translucent : public CRuShader_SMUNI_Base
{
public:
											CRuShader_SMUNI_Editor_Translucent(CRuEngineD3D *engine);
	virtual									~CRuShader_SMUNI_Editor_Translucent();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
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

inline const char *CRuShader_SMUNI_Editor_Translucent::GetShaderName() const
{
	return "ruSHADER_EDITOR_TRANSLUCENT";
}

inline INT32 CRuShader_SMUNI_Editor_Translucent::GetShaderPriority() const
{
	return 3000;
}

inline DWORD CRuShader_SMUNI_Editor_Translucent::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SMUNI_Editor_Translucent::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SMUNI_Editor_Translucent::GetNumSamplers(INT32 passNumber) const
{
	return 0;
}

inline RuTextureChannel CRuShader_SMUNI_Editor_Translucent::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
