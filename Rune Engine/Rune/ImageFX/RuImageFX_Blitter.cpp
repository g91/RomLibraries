/*!
	@project Rune
	@file RuImageFX_Blitter.cpp

	Copyright (c) 2004-2006 All rights reserved

	@author John Tang
	@date 2005/12/08
*/

#include "../ImageFX/RuImageFX_Blitter.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

void RuImageFX_Blit_A8R8G8B8ToA4R4G4B4(UINT16 *dstBits, INT32 x, INT32 y, INT32 dstPitch, INT32 dstChannel, UINT32 *srcBits, INT32 srcWidth, INT32 srcHeight, INT32 srcPitch, INT32 srcChannel)
{
	INT32 srcShift = 0;
	INT32 dstShift = 0;

	switch(srcChannel)
	{
		case 0: srcShift = 16; break;		// Red
		case 1: srcShift = 8; break;		// Green
		case 2: srcShift = 0; break;		// Blue
		case 3: srcShift = 24; break;		// Alpha
	}

	switch(dstChannel)
	{
		case 0: dstShift = 8; break;		// Red
		case 1: dstShift = 4; break;		// Green
		case 2: dstShift = 0; break;		// Blue
		case 3: dstShift = 12; break;		// Alpha
	}

	// Offset by row
	dstBits += (dstPitch / 2) * y;

	for(INT32 sy = 0, dy = y; sy < srcHeight; ++sy, ++dy)
	{
		for(INT32 sx = 0, dx = x; sx < srcWidth; ++sx, ++dx)
		{
			// Truncate texel value to 4 bits
			UINT8 texelValue = static_cast<UINT8>((srcBits[sx] >> srcShift) & 0xFF);
			texelValue = texelValue >> 4;

			// Store to destination
			dstBits[dx] = dstBits[dx] | (texelValue << dstShift);
		}

		srcBits += (srcPitch / 4);
		dstBits += (dstPitch / 2);
	}
}

// ************************************************************************************************************************************************************

void RuImageFX_Blit_16(UINT16 *dstBits, INT32 x, INT32 y, INT32 dstPitch, UINT16 *srcBits, INT32 srcWidth, INT32 srcHeight, INT32 srcPitch)
{
	// Offset by row
	dstBits += (dstPitch / 2) * y;

	for(INT32 sy = 0, dy = y; sy < srcHeight; ++sy, ++dy)
	{
		for(INT32 sx = 0, dx = x; sx < srcWidth; ++sx, ++dx)
		{
			// Store to destination
			dstBits[dx] = srcBits[sx];
		}

		srcBits += (srcPitch / 2);
		dstBits += (dstPitch / 2);
	}
}

// ************************************************************************************************************************************************************

void RuImageFX_Clear_16(UINT16 *dstBits, INT32 x, INT32 y, INT32 width, INT32 height, INT32 dstPitch)
{
	// Offset by row
	dstBits += (dstPitch / 2) * y;

	for(INT32 sy = 0, dy = y; sy < height; ++sy, ++dy)
	{
		for(INT32 sx = 0, dx = x; sx < width; ++sx, ++dx)
		{
			// Store to destination
			dstBits[dx] = 0;
		}

		dstBits += (dstPitch / 2);
	}
}

// ************************************************************************************************************************************************************

void RuImageFX_Clear_32(UINT32 *dstBits, INT32 x, INT32 y, INT32 width, INT32 height, INT32 dstPitch, DWORD color)
{
	// Offset by row
	dstBits += (dstPitch / 4) * y;

	for(INT32 sy = 0, dy = y; sy < height; ++sy, ++dy)
	{
		for(INT32 sx = 0, dx = x; sx < width; ++sx, ++dx)
		{
			// Store to destination
			dstBits[dx] = color;
		}

		dstBits += (dstPitch / 4);
	}
}

// ************************************************************************************************************************************************************

#pragma managed
