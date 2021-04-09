#include "../../Engine/Manager/RuEngine_ResourceManager_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuEngine_ResourceManager, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuEngine_ResourceManager, "IRuEngine_ResourceManager", "IRuObject")
ruCLASSGUID_DEFINE(IRuEngine_ResourceManager, 0xCF21E565, 0x9AA146b1, 0x97676B6A, 0x14C94538)

// ************************************************************************************************************************************************************

IRuEngine_ResourceManager*			g_ruResourceManager = NULL;

// ************************************************************************************************************************************************************

#pragma managed(pop)
