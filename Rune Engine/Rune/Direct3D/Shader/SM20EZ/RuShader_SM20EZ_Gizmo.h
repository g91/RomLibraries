#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/* 
	SHADER CONSTANTS
	----------------
	[ 27 -  27] Highlight color

*/
class CRuShader_SM20EZ_Gizmo_CodedLine : public CRuShader_SM20EZ_Static_Base
{
public:
											CRuShader_SM20EZ_Gizmo_CodedLine(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Gizmo_CodedLine();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
	virtual INT32							GetShaderPriority() const;
	virtual DWORD							GetCaps() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);

	// Shader texture placement query
	virtual INT32							GetNumSamplers(INT32 passNumber) const;
	virtual RuTextureChannel				GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const;

	// Shader parameter setup
	virtual void							SetParameters(const CRuParameterCache *paramCache);
};

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Gizmo_CodedLine::GetShaderName() const
{
	return "ruSHADER_GIZMO_CODEDLINE";
}

inline INT32 CRuShader_SM20EZ_Gizmo_CodedLine::GetShaderPriority() const
{
	return 2000;
}

inline DWORD CRuShader_SM20EZ_Gizmo_CodedLine::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Gizmo_CodedLine::GetNumSamplers(INT32 passNumber) const
{
	return 0;
}

inline RuTextureChannel CRuShader_SM20EZ_Gizmo_CodedLine::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
