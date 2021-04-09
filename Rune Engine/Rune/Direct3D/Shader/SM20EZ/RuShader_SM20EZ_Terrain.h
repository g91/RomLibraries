#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuShader_SM20EZ_Terrain_Base : public CRuShader_SM20EZ_Base
{
public:
											CRuShader_SM20EZ_Terrain_Base(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Terrain_Base();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
	virtual INT32							GetShaderPriority() const;
	virtual DWORD							GetCaps() const;

	// Multi-Pass
	virtual INT32							GetNumPasses() const;

	// Shader texture placement query
	virtual INT32							GetNumSamplers(INT32 passNumber) const;
	virtual RuTextureChannel				GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const;
};

// ************************************************************************************************************************************************************

class CRuShader_SM20EZ_Terrain : public CRuShader_SM20EZ_Terrain_Base
{
protected:
	IDirect3DPixelShader9*					m_pixelShader_SM2;

	// Intrinsic settings
	BOOL									m_maxLayersPerPass;						//!< Maximum number of texture layers per blend pass
	INT32									m_maxTextureLayers;						//!< Maximum number of texture layers allowed

	// Shader settings
	INT32									m_numTextureLayers;
	INT32									m_numPasses;

public:
											CRuShader_SM20EZ_Terrain(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Terrain();

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

class CRuShader_SM20EZ_Terrain_Packed_Base : public CRuShader_SM20EZ_Base
{
protected:
	REAL									m_chunk_Spacing;
	REAL									m_chunk_Y_Base;
	REAL									m_chunk_Y_Increment;

public:
											CRuShader_SM20EZ_Terrain_Packed_Base(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Terrain_Packed_Base();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
	virtual INT32							GetShaderPriority() const;
	virtual DWORD							GetCaps() const;

	// Multi-Pass
	virtual INT32							GetNumPasses() const;

	// Shader parameter setup
	virtual void							SetParameters(const CRuParameterCache *paramCache);
};

// ************************************************************************************************************************************************************

class CRuShader_SM20EZ_Terrain_Packed : public CRuShader_SM20EZ_Terrain_Packed_Base
{
protected:
	IDirect3DVertexShader9*					m_vertexShaders[2];						//!< Vertex shaders
	IDirect3DPixelShader9*					m_pixelShader_SM2_D1;
	IDirect3DPixelShader9*					m_pixelShader_SM2_D2;

	// Intrinsic settings
	BOOL									m_maxLayersPerPass;						//!< Maximum number of texture layers per blend pass
	INT32									m_maxTextureLayers;						//!< Maximum number of texture layers allowed

	// Shader settings
	INT32									m_numTextureLayers;
	INT32									m_numPasses;

public:
											CRuShader_SM20EZ_Terrain_Packed(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Terrain_Packed();

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

class CRuShader_SM20EZ_Terrain_Packed_Backdrop : public CRuShader_SM20EZ_Terrain_Packed_Base
{
public:
											CRuShader_SM20EZ_Terrain_Packed_Backdrop(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Terrain_Packed_Backdrop();

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

inline const char *CRuShader_SM20EZ_Terrain_Base::GetShaderName() const
{
	return "CRuShader_SM20EZ_Terrain_Base";
}

inline INT32 CRuShader_SM20EZ_Terrain_Base::GetShaderPriority() const
{
	return 999;
}

inline DWORD CRuShader_SM20EZ_Terrain_Base::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Terrain_Base::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_Terrain_Base::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SM20EZ_Terrain_Base::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	if(passNumber == 0 && samplerNumber == 0)
		return ruTEXCHANNEL_DIFFUSEMAP;

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Terrain::GetShaderName() const
{
	return "ruSHADER_TERRAIN";
}

inline INT32 CRuShader_SM20EZ_Terrain::GetShaderPriority() const
{
	return 999;
}

inline DWORD CRuShader_SM20EZ_Terrain::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Terrain::GetNumPasses() const
{
	return m_numPasses;
}

inline INT32 CRuShader_SM20EZ_Terrain::GetNumSamplers(INT32 passNumber) const
{
	return 8;

	// Fog pass - no samplers
	if(m_maxLayersPerPass == 3 && passNumber == m_numPasses - 1)
	{
		return 0;
	}

	INT32 numTexturesRemaining = m_numTextureLayers - (passNumber * m_maxLayersPerPass);

	// If the number of textures remaining is greater than the maximum number of layers per pass, then return the maximum number of samplers
	if(numTexturesRemaining >= m_maxLayersPerPass)
	{
		return m_maxLayersPerPass + (m_maxLayersPerPass / 3);
	}
	else
	{
		// If the number of textures remaining can be rendered in one pass with the 1-to-3 layer shader, use it
		if(numTexturesRemaining <= 3)
			return numTexturesRemaining + 1;
		// Otherwise use the 4-to-6 layer shader
		else
			return numTexturesRemaining + 2;
	}

	return 0;
}

inline RuTextureChannel CRuShader_SM20EZ_Terrain::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	// Modify the sampler number
	INT32 baseSamplerNumber = passNumber * (m_maxLayersPerPass + m_maxLayersPerPass / 3);
	samplerNumber += baseSamplerNumber;

	switch(samplerNumber)
	{
		case 0:
			return ruTEXCHANNEL_TERRAIN_ALPHA_0;
		case 1:
			return ruTEXCHANNEL_TERRAIN_LAYER_0;
		case 2:
			return ruTEXCHANNEL_TERRAIN_LAYER_1;
		case 3:
			return ruTEXCHANNEL_TERRAIN_LAYER_2;
		case 4:
			return ruTEXCHANNEL_TERRAIN_ALPHA_1;
		case 5:
			return ruTEXCHANNEL_TERRAIN_LAYER_3;
		case 6:
			return ruTEXCHANNEL_TERRAIN_LAYER_4;
		case 7:
			return ruTEXCHANNEL_TERRAIN_LAYER_5;
		case 8:
			return ruTEXCHANNEL_TERRAIN_ALPHA_2;
		case 9:
			return ruTEXCHANNEL_TERRAIN_LAYER_6;
		case 10:
			return ruTEXCHANNEL_TERRAIN_LAYER_7;
		case 11:
			return ruTEXCHANNEL_TERRAIN_LAYER_8;
		case 12:
			return ruTEXCHANNEL_TERRAIN_ALPHA_3;
		case 13:
			return ruTEXCHANNEL_TERRAIN_LAYER_9;
		case 14:
			return ruTEXCHANNEL_TERRAIN_LAYER_10;
		case 15:
			return ruTEXCHANNEL_TERRAIN_LAYER_11;
	}

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Terrain_Packed_Base::GetShaderName() const
{
	return "ruSHADER_TERRAIN_PACKED_BASE";
}

inline INT32 CRuShader_SM20EZ_Terrain_Packed_Base::GetShaderPriority() const
{
	return 1000;
}

inline DWORD CRuShader_SM20EZ_Terrain_Packed_Base::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Terrain_Packed_Base::GetNumPasses() const
{
	return 1;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Terrain_Packed::GetShaderName() const
{
	return "ruSHADER_TERRAIN_PACKED";
}

inline INT32 CRuShader_SM20EZ_Terrain_Packed::GetShaderPriority() const
{
	return 999;
}

inline DWORD CRuShader_SM20EZ_Terrain_Packed::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Terrain_Packed::GetNumPasses() const
{
	return m_numPasses;
}

inline INT32 CRuShader_SM20EZ_Terrain_Packed::GetNumSamplers(INT32 passNumber) const
{
/*
	// Fog pass - no samplers (fog pass is for deprecated PS 1.1 pipe... ignore)
	if(m_maxLayersPerPass == 3 && passNumber == m_numPasses - 1)
	{
		return 0;
	}

	// This is also deprecated.. we only do single-pass
	INT32 numTexturesRemaining = m_numTextureLayers - (passNumber * m_maxLayersPerPass);

	// If the number of textures remaining is greater than the maximum number of layers per pass, then return the maximum number of samplers
	if(numTexturesRemaining >= m_maxLayersPerPass)
	{
		return m_maxLayersPerPass + (m_maxLayersPerPass / 3);
	}
	else
	{
		// If the number of textures remaining can be rendered in one pass with the 1-to-3 layer shader, use it
		if(numTexturesRemaining <= 3)
			return numTexturesRemaining + 1;
		// Otherwise use the 4-to-6 layer shader
		else
			return numTexturesRemaining + 2;
	}
*/
	return 9;


	if(m_numTextureLayers <= 3)
		return m_numTextureLayers + 2;
	else
		return m_numTextureLayers + 3;


	return 0;
}

inline RuTextureChannel CRuShader_SM20EZ_Terrain_Packed::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	// Modify the sampler number
	INT32 baseSamplerNumber = passNumber * (m_maxLayersPerPass + m_maxLayersPerPass / 3);
	samplerNumber += baseSamplerNumber;

	switch(samplerNumber)
	{
		case 0:
			return ruTEXCHANNEL_RTLIGHTMAP;
		case 1:
			return ruTEXCHANNEL_TERRAIN_ALPHA_0;
		case 2:
			return ruTEXCHANNEL_TERRAIN_LAYER_0;
		case 3:
			return ruTEXCHANNEL_TERRAIN_LAYER_1;
		case 4:
			return ruTEXCHANNEL_TERRAIN_LAYER_2;
		case 5:
			return ruTEXCHANNEL_TERRAIN_ALPHA_1;
		case 6:
			return ruTEXCHANNEL_TERRAIN_LAYER_3;
		case 7:
			return ruTEXCHANNEL_TERRAIN_LAYER_4;
		case 8:
			return ruTEXCHANNEL_TERRAIN_LAYER_5;



/*
		case 8:
			return ruTEXCHANNEL_TERRAIN_ALPHA_2;
		case 9:
			return ruTEXCHANNEL_TERRAIN_LAYER_6;
		case 10:
			return ruTEXCHANNEL_TERRAIN_LAYER_7;
		case 11:
			return ruTEXCHANNEL_TERRAIN_LAYER_8;
		case 12:
			return ruTEXCHANNEL_TERRAIN_ALPHA_3;
		case 13:
			return ruTEXCHANNEL_TERRAIN_LAYER_9;
		case 14:
			return ruTEXCHANNEL_TERRAIN_LAYER_10;
		case 15:
			return ruTEXCHANNEL_TERRAIN_LAYER_11;
*/
	}

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Terrain_Packed_Backdrop::GetShaderName() const
{
	return "ruSHADER_TERRAIN_PACKED_BACKDROP";
}

inline INT32 CRuShader_SM20EZ_Terrain_Packed_Backdrop::GetShaderPriority() const
{
	return 998;
}

inline DWORD CRuShader_SM20EZ_Terrain_Packed_Backdrop::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Terrain_Packed_Backdrop::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SM20EZ_Terrain_Packed_Backdrop::GetNumSamplers(INT32 passNumber) const
{
	return 0;
}

inline RuTextureChannel CRuShader_SM20EZ_Terrain_Packed_Backdrop::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
