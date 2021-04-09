/*!
	@file RuShader_D3D_FX2D_UIFX.h

	Copyright (c) 2004-2006 All rights reserved
*/

#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	SHADER CONSTANTS
	----------------
*/
class CRuShader_SM20EZ_FX2D_UIFX_Darken : public CRuShader_SM20EZ_Base
{
protected:
	UINT32									m_numInstances;

public:
											CRuShader_SM20EZ_FX2D_UIFX_Darken(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_FX2D_UIFX_Darken();

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

inline const char *CRuShader_SM20EZ_FX2D_UIFX_Darken::GetShaderName() const
{
	return "ruSHADER_FX2D_UIFX_DARKEN";
}

inline INT32 CRuShader_SM20EZ_FX2D_UIFX_Darken::GetShaderPriority() const
{
	return 1000;
}

inline DWORD CRuShader_SM20EZ_FX2D_UIFX_Darken::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_FX2D_UIFX_Darken::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_FX2D_UIFX_Darken::GetNumSamplers(INT32 passNumber) const
{
	return 0;
}

inline RuTextureChannel CRuShader_SM20EZ_FX2D_UIFX_Darken::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
