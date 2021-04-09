#include "../../Engine/Instantiator/RuMeshInstantiator_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuMeshInstantiator, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuMeshInstantiator, "IRuMeshInstantiator", "IRuObject")
ruCLASSGUID_DEFINE(IRuMeshInstantiator, 0x4B33FDC2, 0x85CB4e8a, 0x81041FBC, 0x8D0B0DBD)

// ************************************************************************************************************************************************************

IRuMeshInstantiator::~IRuMeshInstantiator()
{
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
