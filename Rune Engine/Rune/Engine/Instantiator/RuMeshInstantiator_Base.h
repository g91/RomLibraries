#ifndef _RUMESHINSTANTIATOR_BASE_H_
#define _RUMESHINSTANTIATOR_BASE_H_

// Rune system core
#include "../../Engine/Base/RuEngine_Engine.h"
#include "../../Engine/Geometry/RuMeshBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class IRuMeshInstantiator : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuMeshInstantiator();

	virtual const IRuVertexStreamLayout*	GetVertexStreamLayout() const = 0;
	virtual void							Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const = 0;
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
