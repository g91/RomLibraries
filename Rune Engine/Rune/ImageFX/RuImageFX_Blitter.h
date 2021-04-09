/*!
	@project Rune
	@file RuImageFX_Blitter.h

	Copyright (c) 2004-2006 All rights reserved

	@author John Tang
	@date 2006/03/01
*/

#ifndef _RUIMAGEFX_BLITTER_H_
#define _RUIMAGEFX_BLITTER_H_

#include "../Core/Type/RuType_Object.h"
#include "../Engine/Type/RuType_Core.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

void RuImageFX_Blit_A8R8G8B8ToA4R4G4B4(UINT16 *dstBits, INT32 x, INT32 y, INT32 dstPitch, INT32 dstChannel, UINT32 *srcBits, INT32 srcWidth, INT32 srcHeight, INT32 srcPitch, INT32 srcChannel);
void RuImageFX_Blit_16(UINT16 *dstBits, INT32 x, INT32 y, INT32 dstPitch, UINT16 *srcBits, INT32 srcWidth, INT32 srcHeight, INT32 srcPitch);
void RuImageFX_Clear_16(UINT16 *dstBits, INT32 x, INT32 y, INT32 width, INT32 height, INT32 dstPitch);
void RuImageFX_Clear_32(UINT32 *dstBits, INT32 x, INT32 y, INT32 width, INT32 height, INT32 dstPitch, DWORD color);

// ************************************************************************************************************************************************************

#pragma managed

#endif
