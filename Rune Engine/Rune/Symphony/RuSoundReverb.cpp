/*!
	@file RuSoundReverb.cpp

	Copyright (c) 2004-2013 Runewaker Entertainment Ltd. All rights reserved.
*/

#include "RuSoundReverb.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuSoundReverb, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuSoundReverb, "IRuSoundReverb", "IRuObject")
ruCLASSGUID_DEFINE(IRuSoundReverb, 0x61C6D36B, 0xA36D41f3, 0xB5503567, 0x65C1F7AA)

// ************************************************************************************************************************************************************

IRuSoundReverb::~IRuSoundReverb()
{
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
