#include "../../Engine/Manager/RuEngine_InstanceManager.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuEngine_InstanceManager, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuEngine_InstanceManager, "IRuEngine_InstanceManager", "IRuObject")
ruCLASSGUID_DEFINE(IRuEngine_InstanceManager, 0x53BCE798, 0xBFE44929, 0x9F9275AB, 0x54E0170B)

// ************************************************************************************************************************************************************

IRuEngine_InstanceManager*						g_ruInstanceManager = NULL;

// ************************************************************************************************************************************************************

#pragma managed(pop)
