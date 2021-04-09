#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	SHADER CONSTANTS
	----------------
	[ 27 -  30] Billboard transform
*/
class CRuShader_SMUNI_Widget_Label : public CRuShader_SMUNI_Base
{
protected:
	UINT32									m_numInstances;

	BOOL									m_useAlphaChannel;

	IDirect3DPixelShader9*					m_pixelShader_NoAlphaChannel;

public:
											CRuShader_SMUNI_Widget_Label(CRuEngineD3D *engine);
	virtual									~CRuShader_SMUNI_Widget_Label();

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

inline const char *CRuShader_SMUNI_Widget_Label::GetShaderName() const
{
	return "ruSHADER_WIDGET_LABEL";
}

inline INT32 CRuShader_SMUNI_Widget_Label::GetShaderClass() const
{
	return 2000;
}

inline INT32 CRuShader_SMUNI_Widget_Label::GetShaderPriority() const
{
	return 1000;
}

inline DWORD CRuShader_SMUNI_Widget_Label::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SMUNI_Widget_Label::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SMUNI_Widget_Label::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SMUNI_Widget_Label::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_DIFFUSEMAP;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
