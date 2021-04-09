/*!
	@file RuSoundChannel.cpp

	Copyright (c) 2004-2011 Runewaker Entertainment Ltd. All rights reserved.
*/

#include "RuSoundChannel.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuSoundChannel, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuSoundChannel, "IRuSoundChannel", "IRuObject")
ruCLASSGUID_DEFINE(IRuSoundChannel, 0x1EA955D6, 0xBF984e35, 0x9B4CAA40, 0x0C0B210B)

// ************************************************************************************************************************************************************

IRuSoundChannel::~IRuSoundChannel()
{
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
