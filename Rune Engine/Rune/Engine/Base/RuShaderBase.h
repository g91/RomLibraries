#ifndef _RUSHADERBASE_H_
#define _RUSHADERBASE_H_

// Rune system core
#include "../../Engine/Base/RuCore.h"

// Rune base renderer core
#include "../../Engine/Geometry/RuMeshBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*
	NOTES
	-----
	Shader-implemented-as-interface. In the future, shaders can be loaded from DLLs.
	Should simply call an export function inside shader DLLs to register shaders
	with the system. Something like an enumeration - the registration function
	calls new on each of the shaders it exports, and returns a pointer to it so
	the caller can register the shader.

	It is assumed that all passes use the same stream format
*/
class IRuShader : public IRuReferenceCounted
{
public:
										IRuShader();
	virtual								~IRuShader();

	// Shader identifier
	virtual IRuVertexDeclaration*		GetVertexDeclaration() = 0;									// Gets the vertex declaration used by the shader

	// Shader capabilities query
	virtual const char*					GetShaderName() const = 0;
	virtual INT32						GetShaderClass() const = 0;
	virtual INT32						GetShaderPriority() const = 0;
	virtual DWORD						GetCaps() const = 0;												// Retrieves the shader capability flags

	// Multi-Pass
	virtual INT32						GetNumPasses() const = 0;											// Gets the number of passes required by the shader

	// Rendering setup
	virtual void						BeginPass(INT32 passNumber) = 0;									// Sets up states for rendering
	virtual void						EndPass(INT32 passNumber) = 0;										// Cleans up states after rendering

	// Shader texture placement query
	virtual INT32						GetNumSamplers(INT32 passNumber) const = 0;							// Gets the number of samplers/texture stages required by the shader
	virtual RuTextureChannel			GetTextureChannel(INT32 passNumber, INT32 samplerNumber) const = 0;	// Gets the texture channel used by a particular sampler

	// Shader parameter setup
	virtual void						SetDefaultParameters() = 0;											// Resets parameters to default state
	virtual void						SetParameters(const CRuParameterCache *paramCache) = 0;
};

// ************************************************************************************************************************************************************

inline IRuShader::IRuShader()
{
}

inline IRuShader::~IRuShader()
{
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
