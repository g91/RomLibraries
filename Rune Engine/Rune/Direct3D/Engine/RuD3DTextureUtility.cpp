#include "RuD3DTextureUtility.h"

BOOL CopyLuminanceMapToTextureAlpha(UINT8 *lumMap, int width, int height, IDirect3DTexture9 *texture)
{
	D3DSURFACE_DESC desc;
	D3DLOCKED_RECT lockRect;
	UINT8 *pixels;
	UINT8 extraneousBytes;

	texture->GetLevelDesc(0, &desc);
	texture->LockRect(0, &lockRect, NULL, 0);
	pixels = (UINT8 *) lockRect.pBits;

	switch(desc.Format)
	{
		case D3DFMT_A8:
			extraneousBytes = 0;
			break;
		case D3DFMT_A8L8:
		case D3DFMT_A8P8:
		case D3DFMT_A8R3G3B2:
			extraneousBytes = 1;
			break;
		case D3DFMT_A4L4:
			extraneousBytes = 0;
			break;
		case D3DFMT_A4R4G4B4:
			extraneousBytes = 1;
			break;
		case D3DFMT_A8R8G8B8:
			extraneousBytes = 3;
			break;
	}

	for(int y = 0, i = 0; y < height; y++)
	{
		for(int x = 0, j = 0; x < width; x++, i++)
		{
			// Repeat alpha bytes for the extra bytes we don't care about
			for(int k = 0; k <= extraneousBytes; k++, j++)
				pixels[j] = lumMap[i];
		}

		pixels += lockRect.Pitch;
	}

	texture->UnlockRect(0);

	return TRUE;
}

void DecodeDXT1ColorBlock(CDXT1Block colorBlock, DWORD *output, DWORD *colors, BOOL preserveOutput)
{
	UINT32 r0, g0, b0, r1, g1, b1;

	r0 = ((colorBlock.m_color0 >> 11) & 31) * 8;
	g0 = ((colorBlock.m_color0 >> 5) & 63) * 4;
	b0 = (colorBlock.m_color0 & 31) * 8;

	r1 = ((colorBlock.m_color1 >> 11) & 31) * 8;
	g1 = ((colorBlock.m_color1 >> 5) & 63) * 4;
	b1 = (colorBlock.m_color1 & 31) * 8;

	// Reconstruct color palette
//	DWORD colors[4];
	colors[0] = D3DCOLOR_ARGB(255, r0, g0, b0);
	colors[1] = D3DCOLOR_ARGB(255, r1, g1, b1);
	if(colorBlock.m_color0 > colorBlock.m_color1)
	{
		colors[2] = D3DCOLOR_ARGB(255, r0 * 2 / 3 + r1 / 3, g0 * 2 / 3 + g1 / 3, b0 * 2 / 3 + b1 / 3);
		colors[3] = D3DCOLOR_ARGB(255, r0 / 3 + r1 * 2 / 3, g0 / 3 + g1 * 2 / 3, b0 / 3 + b1 * 2 / 3);
	}
	else
	{
		colors[2] = D3DCOLOR_ARGB(255, r0 / 2 + r1 / 2, g0 / 2 + g1 / 2, b0 / 2 + b1 / 2);
		colors[3] = D3DCOLOR_ARGB(0, 0, 0, 0);
	}

	if(preserveOutput)
	{
		// Reconstruct color block while preserving existing data via alpha-test in the output array
		for(INT32 i = 0; i < 16; i++)
		{
			if((colorBlock.m_block & 0x00000003) != 0x00000003)
			{
				output[i] = colors[colorBlock.m_block & 0x00000003];
			}

			colorBlock.m_block = colorBlock.m_block >> 2;
		}
	}
	else
	{
		// Reconstruct color block and overwrite the output array
		for(INT32 i = 0; i < 16; i++)
		{
			output[i] = colors[colorBlock.m_block & 0x00000003];
			colorBlock.m_block = colorBlock.m_block >> 2;
		}
	}
}

UINT16 Color8888To565(UINT32 color)
{
	return (UINT16) (((color >> 8) & 0x0000F800) | ((color >> 5) & 0x000007E0) | ((color >> 3) & 0x0000001F));
}

UINT32 Color565To8888(UINT16 color)
{
	return ((color & 0x0000F800) << 8) | ((color & 0x000007E0) << 5) | ((color & 0x0000001F) << 3);
}

UINT32 Distance(UINT16 color0, UINT16 color1)
{
	INT32 rDist = (INT32) ((color0 >> 11) & 0x0000001F) - (INT32) ((color1 >> 11) & 0x0000001F);
	INT32 gDist = (INT32) ((color0 >> 5) & 0x0000003F) - (INT32) ((color1 >> 5) & 0x0000003F);
	INT32 bDist = (INT32) (color0 & 0x0000001F) - (INT32) (color1 & 0x0000001F);

	return rDist * rDist + gDist * gDist + bDist * bDist;
}

UINT32 Distance8888(DWORD color0, DWORD color1)
{
	INT32 rDist = (INT32) ((color0 >> 16) & 0x000000FF) - (INT32) ((color1 >> 16) & 0x000000FF);
	INT32 gDist = (INT32) ((color0 >> 8) & 0x000000FF) - (INT32) ((color1 >> 8) & 0x000000FF);
	INT32 bDist = (INT32) (color0 & 0x000000FF) - (INT32) (color1 & 0x000000FF);

	return rDist * rDist + gDist * gDist + bDist * bDist;
}

void QuantizeDXT1Transparent(DWORD *colorBlock, DWORD *palette)
{
	// Brute-force search

	INT32 i, j, k;

	DWORD color0, color1;
	DWORD error;
	DWORD bestError = 0xFFFFFFFF;

	for(i = 0; i < 3; i++)
	{
		color0 = colorBlock[i];

		for(j = i + 1; j < 4; j++)
		{
			color1 = colorBlock[j];

			// Determine total error
			error = 0;
			for(k = 0; k < 4; k++)
			{
//				error += min(color0 - colorBlock[k], color1 - colorBlock[k]);
				error += min(Distance8888(color0, colorBlock[k]), Distance8888(color1, colorBlock[k]));
			}

			if(error < bestError)
			{
				palette[0] = color0;
				palette[1] = color1;
				bestError = error;
			}
		}
	}
}

void EncodeDXT1ColorBlock(DWORD *colorBlock, BOOL alpha, UINT16 color0, UINT16 color1, CDXT1Block *dxt1ColorBlockOut)
{
	if(alpha)
	{
//		QuantizeDXT1Transparent(colorBlock, palette);

//		UINT16 color0 = Color8888To565(colorBlock[0]);
//		UINT16 color1 = Color8888To565(colorBlock[1]);

//		UINT16 color0 = Color8888To565(palette[0]);
//		UINT16 color1 = Color8888To565(palette[1]);

		if(color0 <= color1)
		{
			dxt1ColorBlockOut->m_color0 = color0;
			dxt1ColorBlockOut->m_color1 = color1;
		}
		else
		{
			dxt1ColorBlockOut->m_color0 = color1;
			dxt1ColorBlockOut->m_color1 = color0;
		}

		dxt1ColorBlockOut->m_block = 0x00000000;

		for(INT32 i = 15; i >= 0; i--)
		{
			dxt1ColorBlockOut->m_block = dxt1ColorBlockOut->m_block << 2;

			if(colorBlock[i] & 0xFF000000)
			{
				UINT16 curColor = Color8888To565(colorBlock[i]);

				if(Distance(curColor, dxt1ColorBlockOut->m_color0) < Distance(curColor, dxt1ColorBlockOut->m_color1))
				{
				}
				else
				{
					dxt1ColorBlockOut->m_block = dxt1ColorBlockOut->m_block | 0x00000001;
				}
			}
			else
			{
				dxt1ColorBlockOut->m_block = dxt1ColorBlockOut->m_block | 0x00000003;
			}
		}
	}
	else
	{
		UINT16 color0 = Color8888To565(colorBlock[0]);
		UINT16 color1 = Color8888To565(colorBlock[1]);

		INT a = 2;
		while(color0 == color1 && a < 16)
		{
			color1 = Color8888To565(colorBlock[a]);
			a++;
		}

		if(color0 > color1)
		{
			dxt1ColorBlockOut->m_color0 = color0;
			dxt1ColorBlockOut->m_color1 = color1;
		}
		else
		{
			dxt1ColorBlockOut->m_color0 = color1;
			dxt1ColorBlockOut->m_color1 = color0;
		}

		dxt1ColorBlockOut->m_block = 0x00000000;

		for(INT32 i = 15; i >= 0; i--)
		{
			UINT16 curColor = Color8888To565(colorBlock[i]);

			dxt1ColorBlockOut->m_block = dxt1ColorBlockOut->m_block << 2;

			if(Distance(curColor, dxt1ColorBlockOut->m_color0) < Distance(curColor, dxt1ColorBlockOut->m_color1))
			{
			}
			else
			{
				dxt1ColorBlockOut->m_block = dxt1ColorBlockOut->m_block | 1;
			}
		}
	}

}

BOOL TextureCopyDXT1(IDirect3DTexture9 *srcTexture, IDirect3DTexture9 *destTexture, INT32 x, INT32 y, INT32 xEnd, INT32 yEnd)
{
	INT32 destWidth = xEnd - x;
	INT32 destHeight = yEnd - y;

	D3DSURFACE_DESC srcDesc, destDesc;

	srcTexture->GetLevelDesc(0, &srcDesc);
	destTexture->GetLevelDesc(0, &destDesc);

	// Destination texture and destination block cannot be smaller than source
	if(destDesc.Width < srcDesc.Width || destDesc.Height < srcDesc.Height)
		return FALSE;

	// Destination area cannot be larger than source
	if((UINT32) destWidth > srcDesc.Width || (UINT32) destHeight > srcDesc.Height)
		return FALSE;

	// Verify destination area is valid (not outside the bounds of the destination texture)
	if(x < 0 || y < 0 || (UINT32) xEnd > destDesc.Width || (UINT32) yEnd > destDesc.Height)
		return FALSE;

	// Destination position and area must be a multiple of 4
	if(x % 4 || y % 4 || xEnd % 4 || yEnd % 4)
		return FALSE;

	UINT32 srcLevelCount = srcTexture->GetLevelCount();
	UINT32 destLevelCount = destTexture->GetLevelCount();

	for(UINT32 curDestLevel = 0; curDestLevel < destLevelCount; curDestLevel++)
	{
		D3DLOCKED_RECT srcLR, destLR;

		srcTexture->LockRect(curDestLevel, &srcLR, NULL, D3DLOCK_READONLY);
		destTexture->LockRect(curDestLevel, &destLR, NULL, 0);

		CDXT1Block *srcPixels = (CDXT1Block *) srcLR.pBits;
		CDXT1Block *destPixels = (CDXT1Block *) destLR.pBits;

		INT32 curX, curY;
		for(curY = y; curY < yEnd; curY += 4)
		{
			srcPixels = (CDXT1Block *) ((BYTE *) srcLR.pBits + srcLR.Pitch * (curY - y) / 4);
			destPixels = (CDXT1Block *) ((BYTE *) destLR.pBits + (destLR.Pitch * curY + x * 8) / 4);

			for(curX = x; curX < xEnd; curX += 4)
			{
				// Copy entire block of 64 bits
				*destPixels = *srcPixels;

				++srcPixels;
				++destPixels;
			}
		}

		srcTexture->UnlockRect(curDestLevel);
		destTexture->UnlockRect(curDestLevel);

		// Adjust dest area for next mip level
		x = x / 2;
		y = y / 2;
		xEnd = xEnd / 2;
		yEnd = yEnd / 2;
	}

	return TRUE;
}

BOOL TextureCopyDXT1(IDirect3DTexture9 *srcTexture, IDirect3DTexture9 *destTexture, INT32 x, INT32 y, INT32 xEnd, INT32 yEnd, DWORD modulationColor)
{
	INT32 destWidth = xEnd - x;
	INT32 destHeight = yEnd - y;

	D3DSURFACE_DESC srcDesc, destDesc;

	srcTexture->GetLevelDesc(0, &srcDesc);
	destTexture->GetLevelDesc(0, &destDesc);

	// Destination texture and destination block cannot be smaller than source
	if(destDesc.Width < srcDesc.Width || destDesc.Height < srcDesc.Height)
		return FALSE;

	// Destination area cannot be larger than source
	if((UINT32) destWidth > srcDesc.Width || (UINT32) destHeight > srcDesc.Height)
		return FALSE;

	// Verify destination area is valid (not outside the bounds of the destination texture)
	if(x < 0 || y < 0 || (UINT32) xEnd > destDesc.Width || (UINT32) yEnd > destDesc.Height)
		return FALSE;

	// Destination position and area must be a multiple of 4
	if(x % 4 || y % 4 || xEnd % 4 || yEnd % 4)
		return FALSE;

	UINT32 srcLevelCount = srcTexture->GetLevelCount();
	UINT32 destLevelCount = destTexture->GetLevelCount();

	UINT32 r0, g0, b0, r1, g1, b1;

	r0 = (modulationColor >> 16) & 0x000000FF;
	g0 = (modulationColor >> 8) & 0x000000FF;
	b0 = modulationColor & 0x000000FF;

	// Select the proper source texture mip level
	UINT32 curSrcLevel = 0;
	while((UINT32) destWidth < srcDesc.Width || (UINT32) destHeight < srcDesc.Height)
	{
		destWidth = destWidth * 2;
		destHeight = destHeight * 2;
		++curSrcLevel;
	}

	// Destination aspect ratio must be equal to source aspect ratio
	if((UINT32) destWidth != srcDesc.Width || (UINT32) destHeight != srcDesc.Height)
		return FALSE;

	for(UINT32 curDestLevel = 0; curDestLevel < destLevelCount; curDestLevel++, curSrcLevel++)
	{
		if(curSrcLevel >= srcLevelCount)
			break;

		D3DLOCKED_RECT srcLR, destLR;
		
		if(srcTexture->LockRect(curSrcLevel, &srcLR, NULL, D3DLOCK_READONLY) != D3D_OK)
			return TRUE;

		if(destTexture->LockRect(curDestLevel, &destLR, NULL, 0) != D3D_OK)
			return TRUE;

		CDXT1Block *srcPixels = (CDXT1Block *) srcLR.pBits;
		CDXT1Block *destPixels = (CDXT1Block *) destLR.pBits;

		INT32 curX, curY;
		for(curY = y; curY < yEnd; curY += 4)
		{
			srcPixels = (CDXT1Block *) ((BYTE *) srcLR.pBits + srcLR.Pitch * (curY - y) / 4);
			destPixels = (CDXT1Block *) ((BYTE *) destLR.pBits + (destLR.Pitch * curY + x * 8) / 4);

			for(curX = x; curX < xEnd; curX += 4)
			{
				r1 = (((*srcPixels).m_color0 >> 11) & 31) * 8;
				g1 = (((*srcPixels).m_color0 >> 5) & 63) * 4;
				b1 = ((*srcPixels).m_color0 & 31) * 8;

				UINT16 color0 = Color8888To565(D3DCOLOR_ARGB(255, r0 * r1 / 255, g0 * g1 / 255, b0 * b1 / 255));

				r1 = (((*srcPixels).m_color1 >> 11) & 31) * 8;
				g1 = (((*srcPixels).m_color1 >> 5) & 63) * 4;
				b1 = ((*srcPixels).m_color1 & 31) * 8;

				UINT16 color1 = Color8888To565(D3DCOLOR_ARGB(255, r0 * r1 / 255, g0 * g1 / 255, b0 * b1 / 255));

				if((*srcPixels).m_color0 > (*srcPixels).m_color1)
				{
					if(color0 > color1)
					{
						(*destPixels).m_color0 = color0;
						(*destPixels).m_color1 = color1;
						(*destPixels).m_block = (*srcPixels).m_block;
					}
					else if(color0 == color1)
					{
						(*destPixels).m_color0 = color0;
						(*destPixels).m_color1 = color1;

						DWORD srcBlock = (*srcPixels).m_block;
						DWORD outputBlock = 0x00000000;

						for(INT32 i = 0; i < 16; i++)
						{
							outputBlock = outputBlock << 2;

							switch((srcBlock & 0xC0000000) >> 30)
							{
							case 0:
								outputBlock = outputBlock | 1;
								break;
							case 1:
								break;
							case 2:
								outputBlock = outputBlock | 2;
								break;
							case 3:
								outputBlock = outputBlock | 2;
								break;
							}

							srcBlock = srcBlock << 2;
						}

						(*destPixels).m_block = outputBlock;
					}
					else
					{
						(*destPixels).m_color0 = color1;
						(*destPixels).m_color1 = color0;

						DWORD srcBlock = (*srcPixels).m_block;
						DWORD outputBlock = 0x00000000;

						for(INT32 i = 0; i < 16; i++)
						{
							outputBlock = outputBlock << 2;

							switch((srcBlock & 0xC0000000) >> 30)
							{
							case 0:
								outputBlock = outputBlock | 1;
								break;
							case 1:
								break;
							case 2:
								outputBlock = outputBlock | 3;
								break;
							case 3:
								outputBlock = outputBlock | 2;
								break;
							}

							srcBlock = srcBlock << 2;
						}

						(*destPixels).m_block = outputBlock;
					}
				}
				else
				{
					if(color0 < color1)
					{
						(*destPixels).m_color0 = color0;
						(*destPixels).m_color1 = color1;
						(*destPixels).m_block = (*srcPixels).m_block;
					}
					else if(color0 == color1)
					{
						(*destPixels).m_color0 = color0;
						(*destPixels).m_color1 = color1;

						DWORD srcBlock = (*srcPixels).m_block;
						DWORD outputBlock = 0x00000000;

						for(INT32 i = 0; i < 16; i++)
						{
							outputBlock = outputBlock << 2;

							switch((srcBlock & 0xC0000000) >> 30)
							{
							case 0:
								outputBlock = outputBlock | 1;
								break;
							case 1:
								break;
							case 2:
								outputBlock = outputBlock | 2;
								break;
							case 3:
								outputBlock = outputBlock | 3;
								break;
							}

							srcBlock = srcBlock << 2;
						}

						(*destPixels).m_block = outputBlock;
					}
					else
					{
						// Must flip source colors and the data block
						(*destPixels).m_color0 = color1;
						(*destPixels).m_color1 = color0;

						DWORD srcBlock = (*srcPixels).m_block;
						DWORD outputBlock = 0x00000000;

						for(INT32 i = 0; i < 16; i++)
						{
							outputBlock = outputBlock << 2;

							switch((srcBlock & 0xC0000000) >> 30)
							{
							case 0:
								outputBlock = outputBlock | 1;
								break;
							case 1:
								break;
							case 2:
								outputBlock = outputBlock | 2;
								break;
							case 3:
								outputBlock = outputBlock | 3;
								break;
							}

							srcBlock = srcBlock << 2;
						}

						(*destPixels).m_block = outputBlock;
					}
				}

				++srcPixels;
				++destPixels;
			}
		}

		srcTexture->UnlockRect(curSrcLevel);
		destTexture->UnlockRect(curDestLevel);

		// Adjust dest area for next mip level
		x = x / 2;
		y = y / 2;
		xEnd = xEnd / 2;
		yEnd = yEnd / 2;

		// Round x coordinates to multiple of 4
		INT32 diff;

		diff = x % 4;
		if(diff >= 2)
		{
			x = x + 4 - diff;
			xEnd = xEnd + 4 - diff;
		}
		else
		{
			x = x - diff;
			xEnd = xEnd - diff;
		}

		// Round y coordinates to multiple of 4
		diff = y % 4;
		if(diff >= 2)
		{
			y = y + 4 - diff;
			yEnd = yEnd + 4 - diff;
		}
		else
		{
			y = y - diff;
			yEnd = yEnd - diff;
		}
	}

	return TRUE;
}

BOOL TextureBlendDXT1(IDirect3DTexture9 *srcTexture, IDirect3DTexture9 *destTexture, INT32 x, INT32 y, INT32 xEnd, INT32 yEnd)
{
	INT32 destWidth = xEnd - x;
	INT32 destHeight = yEnd - y;

	D3DSURFACE_DESC srcDesc, destDesc;

	srcTexture->GetLevelDesc(0, &srcDesc);
	destTexture->GetLevelDesc(0, &destDesc);

	// Destination texture and destination block cannot be smaller than source
	if(destDesc.Width < srcDesc.Width || destDesc.Height < srcDesc.Height)
		return FALSE;

	// Destination area cannot be larger than source
	if((UINT32) destWidth > srcDesc.Width || (UINT32) destHeight > srcDesc.Height)
		return FALSE;

	// Verify destination area is valid (not outside the bounds of the destination texture)
	if(x < 0 || y < 0 || (UINT32) xEnd > destDesc.Width || (UINT32) yEnd > destDesc.Height)
		return FALSE;

	// Destination position and area must be a multiple of 4
	if(x % 4 || y % 4 || xEnd % 4 || yEnd % 4)
		return FALSE;

	UINT32 srcLevelCount = srcTexture->GetLevelCount();
	UINT32 destLevelCount = destTexture->GetLevelCount();

	DWORD colorBlock[16];

	for(UINT32 curDestLevel = 0; curDestLevel < destLevelCount; curDestLevel++)
	{
		D3DLOCKED_RECT srcLR, destLR;

		srcTexture->LockRect(curDestLevel, &srcLR, NULL, D3DLOCK_READONLY);
		destTexture->LockRect(curDestLevel, &destLR, NULL, 0);

		CDXT1Block *srcPixels = (CDXT1Block *) srcLR.pBits;
		CDXT1Block *destPixels = (CDXT1Block *) destLR.pBits;

		INT32 curX, curY;
		for(curY = y; curY < yEnd; curY += 4)
		{
			srcPixels = (CDXT1Block *) ((BYTE *) srcLR.pBits + srcLR.Pitch * (curY - y) / 4);
			destPixels = (CDXT1Block *) ((BYTE *) destLR.pBits + (destLR.Pitch * curY + x * 8) / 4);

			for(curX = x; curX < xEnd; curX += 4)
			{
				if((*srcPixels).m_color0 > (*srcPixels).m_color1)
				{
					// Copy entire block of 64 bits
					*destPixels = *srcPixels;
				}
				else
				{
					// Process only if the source block is not entirely transparent
					if((*srcPixels).m_block != 0xFFFFFFFF)
					{
/*
						// Determine whether the source block is entirely opaque
						BOOL isOpaque = TRUE;
						DWORD srcBlock = (*srcPixels).m_block;
						for(INT32 i = 0; i < 16; i++)
						{
							if((srcBlock & 0x00000003) ^ 0x00000003)
							{
								isOpaque = FALSE;
								break;
							}

							srcBlock = srcBlock >> 2;
						}

						if(isOpaque)
						{
							// Copy entire block of 64 bits
							*destPixels = *srcPixels;
						}
						else*/
						{
							DWORD srcColors[4], destColors[4];

							// Decode destination color block
							DecodeDXT1ColorBlock(*destPixels, colorBlock, srcColors, FALSE);

							// Decode source color block onto desination color block
							DecodeDXT1ColorBlock(*srcPixels, colorBlock, destColors, TRUE);
/*
							DWORD blah[4];
							DWORD palette[2];
							blah[0] = Color565To8888((*srcPixels).m_color0);
							blah[1] = Color565To8888((*srcPixels).m_color1);
							blah[2] = Color565To8888((*destPixels).m_color0);
							blah[3] = Color565To8888((*destPixels).m_color1);

							QuantizeDXT1Transparent(blah, palette);
*/
//							UINT16 palette[2];
//							palette[0] = (*srcPixels).m_color0;
//							palette[1] = (*destPixels).m_color0;

							// Encode and store combined color block
//							EncodeDXT1ColorBlock(colorBlock, TRUE, palette[0], palette[1], destPixels);
//							EncodeDXT1ColorBlock(colorBlock, TRUE, Color8888To565(palette[0]), Color8888To565(palette[1]), destPixels);
							EncodeDXT1ColorBlock(colorBlock, TRUE, Color8888To565(srcColors[0]), Color8888To565(destColors[0]), destPixels);
						}
					}
				}

				++srcPixels;
				++destPixels;
			}
		}

		srcTexture->UnlockRect(curDestLevel);
		destTexture->UnlockRect(curDestLevel);

		// Adjust dest area for next mip level
		x = x / 2;
		y = y / 2;
		xEnd = xEnd / 2;
		yEnd = yEnd / 2;
	}

	return TRUE;
}
/*
BOOL TextureBlendDXT1(IDirect3DTexture9 *srcTexture0, IDirect3DTexture9 *srcTexture1, IDirect3DTexture9 *destTexture, INT32 x, INT32 y, INT32 xEnd, INT32 yEnd)
{
	INT32 destWidth = xEnd - x;
	INT32 destHeight = yEnd - y;

	D3DSURFACE_DESC srcDesc0, srcDesc1, destDesc;

	srcTexture0->GetLevelDesc(0, &srcDesc0);
	srcTexture1->GetLevelDesc(0, &srcDesc1);
	destTexture->GetLevelDesc(0, &destDesc);

	// Both source textures must have the same dimensions
	if(srcDesc0.Width != srcDesc1.Width	|| srcDesc0.Height != srcDesc1.Height)
		return FALSE;

	// Destination texture and destination block cannot be smaller than source
	if(destDesc.Width < srcDesc0.Width || destDesc.Height < srcDesc0.Height)
		return FALSE;

	// Destination area cannot be larger than source
	if((UINT32) destWidth > srcDesc0.Width || (UINT32) destHeight > srcDesc0.Height)
		return FALSE;

	// Verify destination area is valid (not outside the bounds of the destination texture)
	if(x < 0 || y < 0 || (UINT32) xEnd > destDesc.Width || (UINT32) yEnd > destDesc.Height)
		return FALSE;

	// Destination position and area must be a multiple of 4
	if(x % 4 || y % 4 || xEnd % 4 || yEnd % 4)
		return FALSE;

	UINT32 srcLevelCount0 = srcTexture0->GetLevelCount();
	UINT32 srcLevelCount1 = srcTexture1->GetLevelCount();
	UINT32 destLevelCount = destTexture->GetLevelCount();

	for(UINT32 curDestLevel = 0; curDestLevel < destLevelCount; curDestLevel++)
	{
		D3DLOCKED_RECT srcLR0, srcLR1, destLR;

		srcTexture0->LockRect(curDestLevel, &srcLR0, NULL, D3DLOCK_READONLY);
		srcTexture1->LockRect(curDestLevel, &srcLR1, NULL, D3DLOCK_READONLY);
		destTexture->LockRect(curDestLevel, &destLR, NULL, 0);

		CDXT1Block *srcPixels0 = (CDXT1Block *) srcLR0.pBits;
		CDXT1Block *srcPixels1 = (CDXT1Block *) srcLR1.pBits;
		CDXT1Block *destPixels = (CDXT1Block *) destLR.pBits;

		INT32 curX, curY;
		for(curY = y; curY < yEnd; curY += 4)
		{
			srcPixels0 = (CDXT1Block *) ((BYTE *) srcLR0.pBits + srcLR0.Pitch * (curY - y) / 4);
			srcPixels1 = (CDXT1Block *) ((BYTE *) srcLR1.pBits + srcLR1.Pitch * (curY - y) / 4);
			destPixels = (CDXT1Block *) ((BYTE *) destLR.pBits + destLR.Pitch * curY / 4);

			for(curX = x; curX < xEnd; curX += 4)
			{
				// Copy entire block of 64 bits
				*destPixels = *srcPixels0;

				++srcPixels0;
				++srcPixels1;
				++destPixels;
			}
		}

		srcTexture0->UnlockRect(curDestLevel);
		srcTexture1->UnlockRect(curDestLevel);
		destTexture->UnlockRect(curDestLevel);

		// Adjust dest area for next mip level
		x = x / 2;
		y = y / 2;
		xEnd = xEnd / 2;
		yEnd = yEnd / 2;
	}

	return TRUE;
}
*/