#ifndef _RUSHADER_COMPOSITE_H_
#define _RUSHADER_COMPOSITE_H_

// Rune system core
#include "../../Engine/Base/RuShaderBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Shader compositor. Used to combine multiple shaders into one aggregate
	multi-pass shader.

	@author John Tang
	@version 2005/11/09
*/
class CRuShader_Composite : public IRuShader
{
protected:
	struct ShaderDesc
	{
		IRuShader*						m_shader;
		INT32							m_numPasses;
		CRuParameterBlock*				m_parameterBlock;										//!< Parameter block specifically for this shader
	};

	CRuArrayList<ShaderDesc>			m_shaders;												//!< List of aggregated shaders

	IRuVertexDeclaration*				m_vertexDeclaration;
	INT32								m_shaderClass;
	INT32								m_shaderPriority;
	INT32								m_numPasses;											//!< Total number of passes

	CRuParameterCache					m_parameterCache;

public:
										CRuShader_Composite();
	virtual								~CRuShader_Composite();

	INT32								AppendShader(IRuShader *shader);

	void								SetParameter(INT32 shaderIdx, RuParameterType parameterType, UINT64 parameter);

	// Shader identifier
	virtual IRuVertexDeclaration*		GetVertexDeclaration();

	// Shader capabilities query
	virtual const char*					GetShaderName() const;
	virtual INT32						GetShaderClass() const;
	virtual INT32						GetShaderPriority() const;
	virtual DWORD						GetCaps() const;

	// Multi-Pass
	virtual INT32						GetNumPasses() const;

	// Rendering setup
	virtual void						BeginPass(INT32 passNumber);
	virtual void						EndPass(INT32 passNumber);

	// Shader texture placement query
	virtual INT32						GetNumSamplers(INT32 passNumber) const;
	virtual RuTextureChannel			GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const;

	// Shader parameter setup
	virtual void						SetDefaultParameters();
	virtual void						SetParameters(const CRuParameterCache *paramCache);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
