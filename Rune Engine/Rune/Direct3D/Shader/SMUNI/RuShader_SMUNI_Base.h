#pragma once

// Rune system core
#include "../../../Direct3D/Engine/RuEngineD3D.h"
#include "../../../Engine/Base/RuShaderBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

extern IDirect3DVertexShader9*				RuD3DAssembleVertexShader(const char *vsString);
extern IDirect3DPixelShader9*				RuD3DAssemblePixelShader(const char *psString);
extern IDirect3DVertexShader9*				RuD3DCompileVertexShader(const char *vsString, const char *entryFunction, const char *profile, ID3DXConstantTable **constantTableOut);
extern IDirect3DPixelShader9*				RuD3DCompilePixelShader(const char *psString, const char *entryFunction, const char *profile, ID3DXConstantTable **constantTableOut);

// ************************************************************************************************************************************************************

/* 
	VERTEX SHADER CONSTANTS BREAKDOWN
	---------------------------------
	[  0 -   2] 4x3 World transform
	[  3 -   5] 4x3 View transform
	[  6 -   9] 4x4 Projection transform
*/
class CRuShader_SMUNI_Base : public IRuShader
{
protected:
	CRuEngineD3D*							m_engine;									//!< Engine

	IRuVertexDeclaration*					m_vertexDecl;								//!< Vertex declaration
	IDirect3DVertexShader9*					m_vertexShader;								//!< Vertex shader
	IDirect3DPixelShader9*					m_pixelShader;								//!< Pixel shader

	INT32									m_numBaseConstants;
	REAL									m_shaderConstants[4 * 256];

	INT32									m_numBasePixelShaderConstants;
	REAL									m_pixelShaderConstants[4 * 32];

	// Inheritance options
	BOOL									m_override_Fog;
	BOOL									m_override_AlphaTest;
	BOOL									m_override_AlphaBlend;
	BOOL									m_override_ZTest;
	BOOL									m_override_ZWrite;

	// Material options
	BOOL									m_2Sided;									//!< 2-sided geometry?
	BOOL									m_faceted;									//!< Faceted?
	BOOL									m_wire;										//!< Wireframe?
	BOOL									m_alphaTestEnabled;							//!< Enable alpha test?
	UINT32									m_alphaTestThreshold;						//!< Alpha test threshold (>=)
	RuAlphaTestMode							m_alphaTestMode;
	RuAlphaMode								m_material_AlphaMode;
	BOOL									m_material_Translucent;						//!< The material is translucent
	RuZTestMode								m_material_ZTestMode;
	RuZWriteMode							m_material_ZWriteMode;						//!< Material z write mode
	REAL									m_material_Alpha;
	REAL									m_material_AlphaModifier;
	REAL									m_material_EffectiveAlpha;
	BOOL									m_material_ColorWriteEnabled;

	CRuColor4								m_material_Diffuse;
	CRuColor4								m_material_SelfIlluminationColor;
	REAL									m_material_SelfIllumination;

	INT32									m_rawData_Offset;
	INT32									m_rawData_Count;
	REAL*									m_rawData_Source;

	INT32									m_rawPSData_Offset;
	INT32									m_rawPSData_Count;
	REAL*									m_rawPSData_Source;

	REAL									m_fog_Constant;
	REAL									m_fog_Start;
	REAL									m_fog_End;
	CRuColor4								m_fog_Color;

	// FX options
	BOOL									m_fx_AlphaZPass;
	BOOL									m_fx_ReverseCull;
	BOOL									m_fx_StencilEnable;
	UINT8									m_fx_StencilID;
	REAL									m_fx_ExtrudeScale;

public:
											CRuShader_SMUNI_Base(CRuEngineD3D *engine);
	virtual									~CRuShader_SMUNI_Base();

	virtual IRuVertexDeclaration*			GetVertexDeclaration();

	virtual INT32							GetShaderClass() const;
	virtual INT32							GetShaderPriority() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
	virtual void							EndPass(INT32 passNumber);

	// Shader parameter setup
	virtual void							SetDefaultParameters();
	virtual void							SetParameters(const CRuParameterCache *paramCache);

protected:
	void									SetZTestMode(D3DCMPFUNC defaultValue);
	void									SetZWriteMode(BOOL defaultValue);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_NULL : public CRuShader_SMUNI_Base
{
public:
											CRuShader_SMUNI_NULL(CRuEngineD3D *engine);
	virtual									~CRuShader_SMUNI_NULL();

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
};

// ************************************************************************************************************************************************************

inline IRuVertexDeclaration *CRuShader_SMUNI_Base::GetVertexDeclaration()
{
	return m_vertexDecl;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SMUNI_NULL::GetShaderName() const
{
	return "ruSHADER_NULL";
}

inline INT32 CRuShader_SMUNI_NULL::GetShaderPriority() const
{
	return 1001;
}

inline DWORD CRuShader_SMUNI_NULL::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SMUNI_NULL::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SMUNI_NULL::GetNumSamplers(INT32 passNumber) const
{
	return 0;
}

inline RuTextureChannel CRuShader_SMUNI_NULL::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
