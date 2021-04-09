#pragma once

// Rune system core
#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuShader_SM20EZ_Paperdoll : public CRuShader_SM20EZ_Skin_Base
{
public:
											CRuShader_SM20EZ_Paperdoll(CRuEngineD3D *engine);
	virtual									~CRuShader_SM20EZ_Paperdoll();

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

inline const char *CRuShader_SM20EZ_Paperdoll::GetShaderName() const
{
	return "ruSHADER_PAPERDOLL";
}

inline INT32 CRuShader_SM20EZ_Paperdoll::GetShaderPriority() const
{
	return 1000;
}

inline DWORD CRuShader_SM20EZ_Paperdoll::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SM20EZ_Paperdoll::GetNumSamplers(INT32 passNumber) const
{
	return 5;
}

inline RuTextureChannel CRuShader_SM20EZ_Paperdoll::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	switch(passNumber)
	{
		case 0:
			{
				switch(samplerNumber)
				{
					case 0:	return ruTEXCHANNEL_DIFFUSEMAP;
					case 1:	return ruTEXCHANNEL_PAPERDOLL_MASK_COMPOSITE;
					case 2:	return ruTEXCHANNEL_PAPERDOLL_COLORMAP;
					case 3:	return ruTEXCHANNEL_GLOSSMAP;
					case 4:	return ruTEXCHANNEL_GLOWMAP;
				}
			}

			break;
	}

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
