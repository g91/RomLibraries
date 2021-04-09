/*!
	@file RuTextureAtlas_Base.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUTEXTUREATLAS_BASE_H_
#define _RUTEXTUREATLAS_BASE_H_

#include "../../Engine/Base/RuCore.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuTextureAtlas_Base : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~CRuTextureAtlas_Base();

	virtual IRuTexture*						GetAtlasTexture() = 0;
	virtual IRuTexture*						GetTexture(INT32 textureHandle) = 0;
	virtual UINT32							GetTextureWidth(INT32 textureHandle) = 0;
	virtual UINT32							GetTextureHeight(INT32 textureHandle) = 0;

	virtual BOOL							CanBindIntoAtlas(IRuObject *bindingObject) = 0;
	virtual INT32							BindIntoAtlas(IRuObject *bindingObject, BOOL atlasBindingRequired) = 0;
	virtual BOOL							UnbindFromAtlas(INT32 textureHandle) = 0;
	virtual BOOL							UnbindAllFromAtlas() = 0;
	virtual BOOL							TranslateUV(INT32 textureHandle, REAL u, REAL v, REAL uBias, REAL vBias, REAL texelOffset, REAL &uOut, REAL &vOut) = 0;
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
