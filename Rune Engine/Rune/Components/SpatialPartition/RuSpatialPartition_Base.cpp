#include "../../Components/SpatialPartition/RuSpatialPartition_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuSpatialPartition, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuSpatialPartition, "IRuSpatialPartition", "IRuObject")
ruCLASSGUID_DEFINE(IRuSpatialPartition, 0xF8415520, 0x7BC949ce, 0x88B2A541, 0x7044F144)

// ************************************************************************************************************************************************************

IRuSpatialPartition::~IRuSpatialPartition()
{
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
