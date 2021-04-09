#include "../Collision/RuCollision_Base.h"

#include "../Engine/Geometry/RuMeshBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuCollisionObject, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuCollisionObject, "IRuCollisionObject", "IRuObject")
ruCLASSGUID_DEFINE(IRuCollisionObject, 0x910712BC, 0x00384f66, 0x8C3F52E5, 0xE4ABFF10)

// ************************************************************************************************************************************************************

#pragma managed
