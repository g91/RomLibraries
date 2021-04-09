#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuShader_SM20EZ_Editor_PolyHighlight : public CRuShader_SM20EZ_Static_Base
{
public:
											CRuShader_SM20EZ_Editor_PolyHighlight(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Editor_PolyHighlight();

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
};

// ************************************************************************************************************************************************************

class CRuShader_SM20EZ_Editor_ColVis : public CRuShader_SM20EZ_Base
{
public:
											CRuShader_SM20EZ_Editor_ColVis(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Editor_ColVis();

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
};

// ************************************************************************************************************************************************************

class CRuShader_SM20EZ_Editor_Translucent : public CRuShader_SM20EZ_Base
{
public:
											CRuShader_SM20EZ_Editor_Translucent(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Editor_Translucent();

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

inline const char *CRuShader_SM20EZ_Editor_PolyHighlight::GetShaderName() const
{
	return "ruSHADER_EDITOR_POLYHIGHLIGHT";
}

inline INT32 CRuShader_SM20EZ_Editor_PolyHighlight::GetShaderPriority() const
{
	return 2000;
}

inline DWORD CRuShader_SM20EZ_Editor_PolyHighlight::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Editor_PolyHighlight::GetNumPasses() const
{
	return 2;
}

inline INT32 CRuShader_SM20EZ_Editor_PolyHighlight::GetNumSamplers(INT32 passNumber) const
{
	return 0;
}

inline RuTextureChannel CRuShader_SM20EZ_Editor_PolyHighlight::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Editor_ColVis::GetShaderName() const
{
	return "ruSHADER_EDITOR_COLVIS";
}

inline INT32 CRuShader_SM20EZ_Editor_ColVis::GetShaderPriority() const
{
	return 2000;
}

inline DWORD CRuShader_SM20EZ_Editor_ColVis::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Editor_ColVis::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_Editor_ColVis::GetNumSamplers(INT32 passNumber) const
{
	return 0;
}

inline RuTextureChannel CRuShader_SM20EZ_Editor_ColVis::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Editor_Translucent::GetShaderName() const
{
	return "ruSHADER_EDITOR_TRANSLUCENT";
}

inline INT32 CRuShader_SM20EZ_Editor_Translucent::GetShaderPriority() const
{
	return 3000;
}

inline DWORD CRuShader_SM20EZ_Editor_Translucent::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Editor_Translucent::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_Editor_Translucent::GetNumSamplers(INT32 passNumber) const
{
	return 0;
}

inline RuTextureChannel CRuShader_SM20EZ_Editor_Translucent::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
