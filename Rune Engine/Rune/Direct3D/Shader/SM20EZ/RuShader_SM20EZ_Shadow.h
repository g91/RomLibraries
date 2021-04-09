#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Base.h"
#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Terrain.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuShader_SM20EZ_Static_DepthMap : public CRuShader_SM20EZ_Static_Base
{
public:
											CRuShader_SM20EZ_Static_DepthMap(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Static_DepthMap();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
	virtual INT32							GetShaderPriority() const;
	virtual DWORD							GetCaps() const;

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

class CRuShader_SM20EZ_Static_Shadow : public CRuShader_SM20EZ_Static_Base
{
public:
											CRuShader_SM20EZ_Static_Shadow(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Static_Shadow();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
	virtual INT32							GetShaderPriority() const;
	virtual DWORD							GetCaps() const;

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

class CRuShader_SM20EZ_Terrain_Packed_DepthMap : public CRuShader_SM20EZ_Terrain_Packed_Base
{
public:
											CRuShader_SM20EZ_Terrain_Packed_DepthMap(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Terrain_Packed_DepthMap();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
	virtual INT32							GetShaderPriority() const;
	virtual DWORD							GetCaps() const;

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

class CRuShader_SM20EZ_Terrain_Packed_Shadow : public CRuShader_SM20EZ_Terrain_Packed_Base
{
public:
											CRuShader_SM20EZ_Terrain_Packed_Shadow(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Terrain_Packed_Shadow();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;
	virtual INT32							GetShaderPriority() const;
	virtual DWORD							GetCaps() const;

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

inline const char *CRuShader_SM20EZ_Static_DepthMap::GetShaderName() const
{
	return "ruSHADER_DEPTHMAP";
}

inline INT32 CRuShader_SM20EZ_Static_DepthMap::GetShaderPriority() const
{
	return 2000;
}

inline DWORD CRuShader_SM20EZ_Static_DepthMap::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Static_DepthMap::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SM20EZ_Static_DepthMap::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	switch(passNumber)
	{
		case 0:
			{
				switch(samplerNumber)
				{
					case 0:	return ruTEXCHANNEL_DIFFUSEMAP;
				}
			}

			break;
	}

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Static_Shadow::GetShaderName() const
{
	return "ruSHADER_STATIC_SHADOW";
}

inline INT32 CRuShader_SM20EZ_Static_Shadow::GetShaderPriority() const
{
	return 2000;
}

inline DWORD CRuShader_SM20EZ_Static_Shadow::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Static_Shadow::GetNumSamplers(INT32 passNumber) const
{
	return 2;
}

inline RuTextureChannel CRuShader_SM20EZ_Static_Shadow::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	switch(passNumber)
	{
		case 0:
			{
				switch(samplerNumber)
				{
					case 0:	return ruTEXCHANNEL_DIFFUSEMAP;
					case 1:	return ruTEXCHANNEL_SHADOWDEPTHMAP;
				}
			}

			break;
	}

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Terrain_Packed_DepthMap::GetShaderName() const
{
	return "ruSHADER_TERRAIN_PACKED_DEPTHMAP";
}

inline INT32 CRuShader_SM20EZ_Terrain_Packed_DepthMap::GetShaderPriority() const
{
	return 2000;
}

inline DWORD CRuShader_SM20EZ_Terrain_Packed_DepthMap::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Terrain_Packed_DepthMap::GetNumSamplers(INT32 passNumber) const
{
	return 0;
}

inline RuTextureChannel CRuShader_SM20EZ_Terrain_Packed_DepthMap::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

inline const char *CRuShader_SM20EZ_Terrain_Packed_Shadow::GetShaderName() const
{
	return "ruSHADER_TERRAIN_PACKED_SHADOW";
}

inline INT32 CRuShader_SM20EZ_Terrain_Packed_Shadow::GetShaderPriority() const
{
	return 2000;
}

inline DWORD CRuShader_SM20EZ_Terrain_Packed_Shadow::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Terrain_Packed_Shadow::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SM20EZ_Terrain_Packed_Shadow::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	switch(passNumber)
	{
		case 0:
			{
				switch(samplerNumber)
				{
					case 0:	return ruTEXCHANNEL_SHADOWDEPTHMAP;
				}
			}

			break;
	}

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
