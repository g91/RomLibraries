#include "../../Engine/Base/RuShader_Composite.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

CRuShader_Composite::CRuShader_Composite()
:	m_vertexDeclaration(NULL),
	m_shaderClass(0),
	m_shaderPriority(0),
	m_numPasses(0)
{
}

CRuShader_Composite::~CRuShader_Composite()
{
	// Free resources used by shader descriptors
	for(INT32 i = 0; i < m_shaders.Count(); ++i)
	{
		if(m_shaders[i].m_parameterBlock)
		{
			delete m_shaders[i].m_parameterBlock;
		}
	}
}

INT32 CRuShader_Composite::AppendShader(IRuShader *shader)
{
	if(m_vertexDeclaration == NULL || m_vertexDeclaration == shader->GetVertexDeclaration())
	{
		// Append shader to shader list
		INT32 index = m_shaders.Count();
		m_shaders.Add(ShaderDesc());

		m_shaders[index].m_shader = shader;
		m_shaders[index].m_numPasses = shader->GetNumPasses();
		m_shaders[index].m_parameterBlock = NULL;

		// ...
		m_vertexDeclaration = shader->GetVertexDeclaration();
		m_shaderClass = max(m_shaderClass, shader->GetShaderClass());
		m_shaderPriority = max(m_shaderPriority, shader->GetShaderPriority());
		m_numPasses += shader->GetNumPasses();

		return index;
	}

	return -1;
}

void CRuShader_Composite::SetParameter(INT32 shaderIdx, RuParameterType parameterType, UINT64 parameter)
{
	if(shaderIdx >= 0 && shaderIdx < m_shaders.Count())
	{
		if(m_shaders[shaderIdx].m_parameterBlock == NULL)
		{
			m_shaders[shaderIdx].m_parameterBlock = ruNEW CRuParameterBlock();
		}

		m_shaders[shaderIdx].m_parameterBlock->SetParameter(parameterType, parameter);
	}
}

IRuVertexDeclaration *CRuShader_Composite::GetVertexDeclaration()
{
	return m_vertexDeclaration;
}

const char *CRuShader_Composite::GetShaderName() const
{
	return "";
}

INT32 CRuShader_Composite::GetShaderClass() const
{
	return m_shaderClass;
}

INT32 CRuShader_Composite::GetShaderPriority() const
{
	return m_shaderPriority;
}

DWORD CRuShader_Composite::GetCaps() const
{
	return 0;
}

INT32 CRuShader_Composite::GetNumPasses() const
{
	return m_numPasses;
}

void CRuShader_Composite::BeginPass(INT32 passNumber)
{
	for(INT32 i = 0; i < m_shaders.Count(); ++i)
	{
		// If pass number is within range of the current shader, call its begin pass
		if(passNumber < m_shaders[i].m_numPasses)
		{
			// If there is a parameter block for the current shader, apply it
			if(m_shaders[i].m_parameterBlock)
			{
				m_parameterCache.ClearParameterCache();
				m_parameterCache.CacheParameterBlock(m_shaders[i].m_parameterBlock);

				m_shaders[i].m_shader->SetParameters(&m_parameterCache);
			}

			m_shaders[i].m_shader->BeginPass(passNumber);
			return;
		}

		// Decrement pass number
		passNumber -= m_shaders[i].m_numPasses;
	}
}

void CRuShader_Composite::EndPass(INT32 passNumber)
{
	for(INT32 i = 0; i < m_shaders.Count(); ++i)
	{
		// If pass number is within range of the current shader, call its end pass
		if(passNumber < m_shaders[i].m_numPasses)
		{
			m_shaders[i].m_shader->EndPass(passNumber);
			return;
		}

		// Decrement pass number
		passNumber -= m_shaders[i].m_numPasses;
	}
}

INT32 CRuShader_Composite::GetNumSamplers(INT32 passNumber) const
{
	for(INT32 i = 0; i < m_shaders.Count(); ++i)
	{
		// If pass number is within range of the current shader, call its GetNumSamplers
		if(passNumber < m_shaders[i].m_numPasses)
		{
			return m_shaders[i].m_shader->GetNumSamplers(passNumber);
		}

		// Decrement pass number
		passNumber -= m_shaders[i].m_numPasses;
	}

	return 0;
}

RuTextureChannel CRuShader_Composite::GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const
{
	for(INT32 i = 0; i < m_shaders.Count(); ++i)
	{
		// If pass number is within range of the current shader, call its GetTextureChannel
		if(passNumber < m_shaders[i].m_numPasses)
		{
			return m_shaders[i].m_shader->GetTextureChannel(passNumber, samplerNumber);
		}

		// Decrement pass number
		passNumber -= m_shaders[i].m_numPasses;
	}

	return ruTEXCHANNEL_INVALID;
}

void CRuShader_Composite::SetDefaultParameters()
{
	// Note: this is a bit of a performance drain if we end up with a huge chain of shaders
	for(INT32 i = 0; i < m_shaders.Count(); ++i)
	{
		m_shaders[i].m_shader->SetDefaultParameters();
	}
}

void CRuShader_Composite::SetParameters(const CRuParameterCache *paramCache)
{
	// Note: this is a bit of a performance drain if we end up with a huge chain of shaders
	for(INT32 i = 0; i < m_shaders.Count(); ++i)
	{
		m_shaders[i].m_shader->SetParameters(paramCache);
	}
}

// ************************************************************************************************************************************************************

#pragma managed
