#pragma once

// Rune system core
#include "../../Engine/Base/RuCore.h"

// Rune base renderer core
#include "../../Engine/Geometry/RuMeshBase.h"
#include "../../Engine/Base/RuEngine_Engine.h"
#include "../../Engine/Base/RuShaderBase.h"
#include "../../Engine/Base/RuMaterialBase.h"
#include "../../Engine/Instantiator/RuMeshInstantiator_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*
	NOTES
	-----
	It seems nonsensical to instance VB and IB separately. So maybe it's a good idea to have the instance
	manager just return an instanced mesh, which knows how to set itself up into vertex and index streams.

	The key problem now is how to keep the interfaces independent of Direct3D, without CRusing class
	relationships to become convoluted. So, in the case of IRuInstancedMesh, its D3D implementation would be
	CRuInstancedMeshD3D. How does this implementation access the Direct3D device?

	One possible solution would be to have a global(perhaps a singleton) 3D context class which is specific
	to each implementation. For D3D, this would be called C3DContextD3D. CRuInstancedMeshD3D would expect such
	a singleton to always exist. The implementation of SetForRendering(...) would directly invoke
	C3DContextD3D to set vertex and index streams. C3DContextD3D knows nothing about any other classes, but
	instead simply provides a rendering environment and the D3D device. Renderstate caching should also
	go with the context object.

	This approach of course limits us to just one device per application. Is there a more flexible way to do
	this?

	---

	Now we have a related problem - in order to properly support the morph target(which contains only vertices)
	to vertex connectivity relationship, a one-to-many(one IB to many VB) relationship is required. So this means
	we CANNOT instance both together - they must be done separately.

	So the way to do this is: instance index buffers keyed on the GUID of the mesh, and instance vertex buffers keyed
	on the GUID of the morph target. Obviously it is not possible to pass the two buffers back separately, since that
	would completely break the high level instanced mesh abstraction. It is also damn near impossible to try to track
	reference counts used by each instanced mesh(witness the insanity of trying to manage two caches simultaneously
	in a coherent way). Instead, IRuEngine_InstanceManager makes only one guarantee: an instanced mesh that it returns is only
	valid until the next GetInstance(...) call. This isn't so bad because the only time GetInstance(...) is called is
	immediately before rendering an object. By imposing this limitation, we can now store just one instanced mesh
	object inside the D3D instance manager, and return the pointer to this object every time an instanced mesh is
	requested. The only things we change are its pointers to the IB and VB.
*/

// ************************************************************************************************************************************************************

class IRuEngine_InstanceManager : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual								~IRuEngine_InstanceManager() { }

	// Settings
	virtual UINT32						GetCacheSize() = 0;

	virtual IRuEngine*					GetEngine() = 0;

	// Mesh instantiator registration
	virtual BOOL						RegisterMeshInstantiator(IRuMeshInstantiator *meshInstantiator) = 0;

	// Instance retrieval
	virtual IRuVertexAssembly*			GetVertexAssembly(IRuMesh *mesh, IRuVertexDeclaration *vertexDecl) = 0;

	virtual IRuVertexBuffer*			GetInstancedVertexBuffer(IRuMesh *mesh, IRuVertexStreamLayout *streamLayout) = 0;
	virtual IRuIndexBuffer*				GetInstancedIndexBuffer(IRuMesh *mesh) = 0;

	// Instance retrieval for textures
	virtual IRuBaseTexture*				GetInstance(IRuBaseTexture *texture) = 0;
	virtual BOOL						DeInstance(IRuBaseTexture *texture) = 0;

	// Data invalidation
	virtual BOOL						ClearCache() = 0;

	// Metrics
	virtual INT32						GetNumMetricStrings() = 0;
	virtual const char*					GetMetricString(INT32 stringIndex) = 0;

	// Debugging
	virtual BOOL						OutputInstantiationManifest() = 0;
};

// ************************************************************************************************************************************************************

extern IRuEngine_InstanceManager*				g_ruInstanceManager;

// ************************************************************************************************************************************************************

#pragma managed(pop)
