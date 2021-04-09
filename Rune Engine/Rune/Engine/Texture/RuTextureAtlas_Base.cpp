/*!
	@file RuTextureAtlas_Base.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Engine/Texture/RuTextureAtlas_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuTextureAtlas_Base, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuTextureAtlas_Base, "CRuTextureAtlas_Base", "IRuObject")
ruCLASSGUID_DEFINE(CRuTextureAtlas_Base, 0xC1C870D1, 0x81DB48d4, 0x9961406B, 0x5D7F097C)

// ************************************************************************************************************************************************************

CRuTextureAtlas_Base::~CRuTextureAtlas_Base()
{
}

// ************************************************************************************************************************************************************

#pragma managed
