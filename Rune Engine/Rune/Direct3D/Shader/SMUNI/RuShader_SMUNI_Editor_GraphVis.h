#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_Editor_GraphVis : public CRuShader_SMUNI_Base
{
protected:
	INT32									m_numInstances;

public:
											CRuShader_SMUNI_Editor_GraphVis(CRuEngineD3D *engine);
	virtual									~CRuShader_SMUNI_Editor_GraphVis();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
	virtual INT32							GetShaderPriority() const;
	virtual DWORD							GetCaps() const;

	// Rendering setup
	virtual INT32							GetNumPasses() const;
	virtual void							BeginPass(INT32 passNumber);

	// Shader texture placement query
	virtual INT32							GetNumSamplers(INT32 passNumber) const;
	virtual RuTextureChannel				GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const;

	// Shader parameter setup
	virtual void							SetParameters(const CRuParameterCache *paramCache);
};

// ************************************************************************************************************************************************************

inline const char *CRuShader_SMUNI_Editor_GraphVis::GetShaderName() const
{
	return "ruSHADER_EDITOR_GRAPHVIS";
}

inline INT32 CRuShader_SMUNI_Editor_GraphVis::GetShaderPriority() const
{
	return 2000;
}

inline DWORD CRuShader_SMUNI_Editor_GraphVis::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SMUNI_Editor_GraphVis::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SMUNI_Editor_GraphVis::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SMUNI_Editor_GraphVis::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_DIFFUSEMAP;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
