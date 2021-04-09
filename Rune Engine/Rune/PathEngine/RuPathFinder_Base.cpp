/*!
	@project Rune
	@file RuPathFinder_Base.cpp

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/21
*/

#include "../PathEngine/RuPathFinder_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuPathFinder_Base, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuPathFinder_Base, "IRuPathFinder_Base", "IRuObject")
ruCLASSGUID_DEFINE(IRuPathFinder_Base, 0xC61F729D, 0x740B413d, 0x9BE65D5B, 0x6B0A8340)

// ************************************************************************************************************************************************************

#pragma managed
