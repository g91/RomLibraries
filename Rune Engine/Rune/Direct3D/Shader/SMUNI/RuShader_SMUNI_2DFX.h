#pragma once

// Rune system core
#include "../../../Direct3D/Engine/RuEngineD3D.h"
#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	VERTEX SHADER CONSTANTS
	-----------------------
	[  0 -   0] (texture width, texture height, 0.0f, 0.0f)

	PIXEL SHADER CONSTANTS
	----------------------
	None
*/
class CRuShader_SMUNI_ru2DFX_Base : public IRuShader
{
protected:
	CRuEngineD3D*							m_engine;

	REAL									m_vsConstants[4 * 256];
	REAL									m_psConstants[4 * 8];

	IRuVertexDeclaration*					m_vertexDecl;
	IDirect3DVertexShader9*					m_vertexShader;
	IDirect3DPixelShader9*					m_pixelShader;

	INT32									m_numInstances;

	UINT8									m_stencilID;

public:
											CRuShader_SMUNI_ru2DFX_Base(CRuEngineD3D *engine);
	virtual									~CRuShader_SMUNI_ru2DFX_Base();

	IRuVertexDeclaration*					GetVertexDeclaration();

	// Shader capabilities query
	virtual const char*						GetShaderName() const = 0;
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
	virtual void							SetDefaultParameters();
	virtual void							SetParameters(const CRuParameterCache *paramCache);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_ru2DFXTYPE_FLAT : public CRuShader_SMUNI_ru2DFX_Base
{
public:
											CRuShader_SMUNI_ru2DFXTYPE_FLAT(CRuEngineD3D *engine) : CRuShader_SMUNI_ru2DFX_Base(engine) { }
	virtual									~CRuShader_SMUNI_ru2DFXTYPE_FLAT() { }

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_ru2DFXTYPE_FLAT_MASKED : public CRuShader_SMUNI_ru2DFX_Base
{
public:
											CRuShader_SMUNI_ru2DFXTYPE_FLAT_MASKED(CRuEngineD3D *engine) : CRuShader_SMUNI_ru2DFX_Base(engine) { }
	virtual									~CRuShader_SMUNI_ru2DFXTYPE_FLAT_MASKED() { }

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
	virtual void							EndPass(INT32 passNumber);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_ru2DFXTYPE_BLIT : public CRuShader_SMUNI_ru2DFX_Base
{
public:
											CRuShader_SMUNI_ru2DFXTYPE_BLIT(CRuEngineD3D *engine) : CRuShader_SMUNI_ru2DFX_Base(engine) { }
	virtual									~CRuShader_SMUNI_ru2DFXTYPE_BLIT() { }

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_ru2DFXTYPE_BLIT_ADD : public CRuShader_SMUNI_ru2DFX_Base
{
public:
											CRuShader_SMUNI_ru2DFXTYPE_BLIT_ADD(CRuEngineD3D *engine) : CRuShader_SMUNI_ru2DFX_Base(engine) { }
	virtual									~CRuShader_SMUNI_ru2DFXTYPE_BLIT_ADD() { }

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_ru2DFXTYPE_BLIT_FILTER : public CRuShader_SMUNI_ru2DFX_Base
{
public:
											CRuShader_SMUNI_ru2DFXTYPE_BLIT_FILTER(CRuEngineD3D *engine) : CRuShader_SMUNI_ru2DFX_Base(engine) { }
	virtual									~CRuShader_SMUNI_ru2DFXTYPE_BLIT_FILTER() { }

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_ru2DFXTYPE_TEXTBLIT_FILTER : public CRuShader_SMUNI_ru2DFX_Base
{
public:
											CRuShader_SMUNI_ru2DFXTYPE_TEXTBLIT_FILTER(CRuEngineD3D *engine);
	virtual									~CRuShader_SMUNI_ru2DFXTYPE_TEXTBLIT_FILTER() { }

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASK : public CRuShader_SMUNI_ru2DFX_Base
{
public:
											CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASK(CRuEngineD3D *engine) : CRuShader_SMUNI_ru2DFX_Base(engine) { }
	virtual									~CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASK() { }

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
	virtual void							EndPass(INT32 passNumber);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASKED : public CRuShader_SMUNI_ru2DFX_Base
{
public:
											CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASKED(CRuEngineD3D *engine) : CRuShader_SMUNI_ru2DFX_Base(engine) { }
	virtual									~CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASKED() { }

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
	virtual void							EndPass(INT32 passNumber);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASK_Z : public CRuShader_SMUNI_ru2DFX_Base
{
public:
											CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASK_Z(CRuEngineD3D *engine) : CRuShader_SMUNI_ru2DFX_Base(engine) { }
	virtual									~CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASK_Z() { }

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
	virtual void							EndPass(INT32 passNumber);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_ru2DFXTYPE_BLIT_LUMINANCE : public CRuShader_SMUNI_ru2DFX_Base
{
public:
											CRuShader_SMUNI_ru2DFXTYPE_BLIT_LUMINANCE(CRuEngineD3D *engine);
	virtual									~CRuShader_SMUNI_ru2DFXTYPE_BLIT_LUMINANCE() { }

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_ru2DFXTYPE_DARKEN : public CRuShader_SMUNI_ru2DFX_Base
{
public:
											CRuShader_SMUNI_ru2DFXTYPE_DARKEN(CRuEngineD3D *engine) : CRuShader_SMUNI_ru2DFX_Base(engine) { }
	virtual									~CRuShader_SMUNI_ru2DFXTYPE_DARKEN() { }

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_ru2DFXTYPE_EXPONENTIATE : public CRuShader_SMUNI_ru2DFX_Base
{
public:
											CRuShader_SMUNI_ru2DFXTYPE_EXPONENTIATE(CRuEngineD3D *engine);
	virtual									~CRuShader_SMUNI_ru2DFXTYPE_EXPONENTIATE() { }

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
};

// ************************************************************************************************************************************************************

/*!
	VERTEX SHADER CONSTANTS
	-----------------------
	[  1 -   1] (u increment mask, v increment mask, 0.0f, 0.0f)

	PIXEL SHADER CONSTANTS
	----------------------
	[  0 -   0] (0.006f, 0.061f, 0.242f, 0.383f) - gaussian kernel
*/
class CRuShader_SMUNI_ru2DFXTYPE_GAUSSIAN : public CRuShader_SMUNI_ru2DFX_Base
{
protected:
	IDirect3DVertexShader9*					m_vertexShaders[2];
	INT32									m_blurDirection;

public:
											CRuShader_SMUNI_ru2DFXTYPE_GAUSSIAN(CRuEngineD3D *engine);
	virtual									~CRuShader_SMUNI_ru2DFXTYPE_GAUSSIAN();

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Multi-Pass
	virtual INT32							GetNumPasses() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);
	virtual void							EndPass(INT32 passNumber);

	// Shader parameter setup
	virtual void							SetParameters(const CRuParameterCache *paramCache);
};

// ************************************************************************************************************************************************************

class CRuShader_SMUNI_ru2DFXTYPE_DISTORTION : public CRuShader_SMUNI_ru2DFX_Base
{
public:
											CRuShader_SMUNI_ru2DFXTYPE_DISTORTION(CRuEngineD3D *engine);
	virtual									~CRuShader_SMUNI_ru2DFXTYPE_DISTORTION() { }

	// Shader capabilities query
	virtual const char*						GetShaderName() const;

	// Rendering setup
	virtual void							BeginPass(INT32 passNumber);

	// Shader texture placement query
	virtual INT32							GetNumSamplers(INT32 passNumber) const;
	virtual RuTextureChannel				GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const;

	// Shader parameter setup
	virtual void							SetParameters(const CRuParameterCache *paramCache);
};

// ************************************************************************************************************************************************************

inline IRuVertexDeclaration *CRuShader_SMUNI_ru2DFX_Base::GetVertexDeclaration()
{
	return m_vertexDecl;
}

inline INT32 CRuShader_SMUNI_ru2DFX_Base::GetShaderClass() const
{
	return 1000;
}

inline INT32 CRuShader_SMUNI_ru2DFX_Base::GetShaderPriority() const
{
	return 1000;
}

inline DWORD CRuShader_SMUNI_ru2DFX_Base::GetCaps() const
{
	return 0x00000000;
}

inline INT32 CRuShader_SMUNI_ru2DFX_Base::GetNumPasses() const
{
	return 1;
}

inline INT32 CRuShader_SMUNI_ru2DFX_Base::GetNumSamplers(INT32 passNumber) const
{
	return 1;
}

inline RuTextureChannel CRuShader_SMUNI_ru2DFX_Base::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	if(samplerNumber == 0)
		return ruTEXCHANNEL_DIFFUSEMAP;

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

inline INT32 CRuShader_SMUNI_ru2DFXTYPE_GAUSSIAN::GetNumPasses() const
{
	return 1;
}

// ************************************************************************************************************************************************************

inline INT32 CRuShader_SMUNI_ru2DFXTYPE_DISTORTION::GetNumSamplers(INT32 passNumber) const
{
	return 2;
}

inline RuTextureChannel CRuShader_SMUNI_ru2DFXTYPE_DISTORTION::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	switch(samplerNumber)
	{
		case 0:
			return ruTEXCHANNEL_DIFFUSEMAP;

		case 1:
			return ruTEXCHANNEL_BUMPMAP;
	}

	return ruTEXCHANNEL_INVALID;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
