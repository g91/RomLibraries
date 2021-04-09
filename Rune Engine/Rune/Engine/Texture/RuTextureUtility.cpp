/*!
	@file RuTextureAtlas_Base.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Engine/Texture/RuTextureUtility.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

INT32 RuTextureUtility_GetMemoryFootprint(IRuBaseTexture *texture, CRuKeyHash<PTRVALUE> *enumHash)
{
	if(texture)
	{
		if(texture->GetType().IsSubClassOf(IRuTexture::Type()))
		{
			return RuTextureUtility_GetMemoryFootprint(static_cast<IRuTexture *>(texture), enumHash);
		}
	}

	return 0;
}

BOOL RuTextureUtility_Copy(IRuBaseTexture *srcTexture, IRuBaseTexture *dstTexture)
{
	if(srcTexture == dstTexture)
	{
		return TRUE;
	}

	if(srcTexture->GetType().IsSubClassOf(IRuTexture::Type()) && dstTexture->GetType().IsSubClassOf(IRuTexture::Type()))
	{
		return RuTextureUtility_Copy_IRuTexture(static_cast<IRuTexture *>(srcTexture), static_cast<IRuTexture *>(dstTexture));
	}
	else if(srcTexture->GetType().IsSubClassOf(IRuVolumeTexture::Type()) && dstTexture->GetType().IsSubClassOf(IRuVolumeTexture::Type()))
	{
		return RuTextureUtility_Copy_IRuVolumeTexture(static_cast<IRuVolumeTexture *>(srcTexture), static_cast<IRuVolumeTexture *>(dstTexture));
	}

	return FALSE;
}

INT32 RuTextureUtility_GetMemoryFootprint(IRuTexture *texture, CRuKeyHash<PTRVALUE> *enumHash)
{
	// Perform check against enumeration hash
	if(enumHash)
	{
		if(enumHash->ContainsKey(reinterpret_cast<PTRVALUE>(texture)))
		{
			return -1;
		}
		else
		{
			enumHash->Add(reinterpret_cast<PTRVALUE>(texture));
		}
	}

	INT32 footprint = 0;

	switch(texture->GetSurfaceFormat())
	{
		default:
			{
				INT32 bytesPerTexel = RuSurfaceFormat_GetBitsPerTexel(texture->GetSurfaceFormat()) >> 3;

				for(INT32 i = 0, levelCount = texture->GetLevelCount(); i < levelCount; ++i)
				{
					footprint += texture->GetLevelWidth(i) * texture->GetLevelHeight(i) * bytesPerTexel;
				}
			}

			break;

		case ruSURFACEFMT_DXT1:
		case ruSURFACEFMT_DXT2:
		case ruSURFACEFMT_DXT3:
		case ruSURFACEFMT_DXT4:
		case ruSURFACEFMT_DXT5:
			{
				INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(texture->GetSurfaceFormat()) << 1;

				for(INT32 i = 0, levelCount = texture->GetLevelCount(); i < levelCount; ++i)
				{
					footprint += (texture->GetLevelWidth(i) >> 2) * (texture->GetLevelHeight(i) >> 2) * bytesPerBlock;
				}
			}

			break;
	}

	return footprint;
}

IRuSurface *RuTextureUtility_GetLevelSurfaceBySize(IRuTexture *texture, INT32 width, INT32 height)
{
	for(UINT32 i = 0; i < texture->GetLevelCount(); ++i)
	{
		if(texture->GetLevelWidth(i) == width && texture->GetLevelHeight(i) == height)
		{
			return texture->GetLevelSurface(i);
		}
	}

	return NULL;
}

BOOL RuTextureUtility_Blit(IRuSurface *srcSurface, IRuSurface *dstSurface, RuAlphaMode alphaMode)
{
	RuSurfaceFormat srcFormat = srcSurface->GetSurfaceFormat();
	RuSurfaceFormat dstFormat = dstSurface->GetSurfaceFormat();

	INT32 srcWidth = srcSurface->GetWidth();
	INT32 srcHeight = srcSurface->GetHeight();
	INT32 dstWidth = dstSurface->GetWidth();
	INT32 dstHeight = dstSurface->GetHeight();

	// Ensure that source and destination formats are the same, and that they are the same size
	if(srcFormat != dstFormat || srcWidth != dstWidth ||srcHeight != dstHeight)
	{
		return FALSE;
	}

	switch(srcFormat)
	{
		case ruSURFACEFMT_A8R8G8B8:
			{
				switch(alphaMode)
				{
					case ruALPHAMODE_NORMAL:
						{
							DWORD *srcBits, *dstBits;
							INT32 srcPitch, dstPitch;

							srcSurface->Lock(reinterpret_cast<void **>(&srcBits), &srcPitch);
							dstSurface->Lock(reinterpret_cast<void **>(&dstBits), &dstPitch);

							for(INT32 y = 0; y < srcHeight; ++y)
							{
								for(INT32 x = 0; x < srcWidth; ++x)
								{
									INT32 srcAlpha = (srcBits[x] >> 24) & 0xFF;
									INT32 srcR = ((srcBits[x] >> 16) & 0xFF);
									INT32 srcG = ((srcBits[x] >> 8) & 0xFF);
									INT32 srcB = ((srcBits[x] >> 0) & 0xFF);

									INT32 dstAlpha = (dstBits[x] >> 24) & 0xFF;
									INT32 dstR = ((dstBits[x] >> 16) & 0xFF);
									INT32 dstG = ((dstBits[x] >> 8) & 0xFF);
									INT32 dstB = ((dstBits[x] >> 0) & 0xFF);

									INT32 finAlpha = max(srcAlpha, dstAlpha);
									INT32 finR = (srcR * srcAlpha + dstR * (255 - srcAlpha)) / 255;
									INT32 finG = (srcG * srcAlpha + dstG * (255 - srcAlpha)) / 255;
									INT32 finB = (srcB * srcAlpha + dstB * (255 - srcAlpha)) / 255;

									dstBits[x] = RUCOLOR_ARGB(finAlpha, finR, finG, finB);
								}

								srcBits = reinterpret_cast<DWORD *>(reinterpret_cast<BYTE *>(srcBits) + srcPitch);
								dstBits = reinterpret_cast<DWORD *>(reinterpret_cast<BYTE *>(dstBits) + dstPitch);
							}

							srcSurface->Unlock();
							dstSurface->Unlock();
						}

						return TRUE;

					case ruALPHAMODE_DISABLE:
						{
							DWORD *srcBits, *dstBits;
							INT32 srcPitch, dstPitch;

							srcSurface->Lock(reinterpret_cast<void **>(&srcBits), &srcPitch);
							dstSurface->Lock(reinterpret_cast<void **>(&dstBits), &dstPitch);

							for(INT32 y = 0; y < srcHeight; ++y)
							{
								memcpy(dstBits, srcBits, sizeof(DWORD) * srcWidth);

								srcBits = reinterpret_cast<DWORD *>(reinterpret_cast<BYTE *>(srcBits) + srcPitch);
								dstBits = reinterpret_cast<DWORD *>(reinterpret_cast<BYTE *>(dstBits) + dstPitch);
							}

							srcSurface->Unlock();
							dstSurface->Unlock();
						}

						return TRUE;
				}
			}

			break;
	}

	return FALSE;
}

BOOL RuTextureUtility_Copy_IRuTexture(IRuTexture *srcTexture, IRuTexture *dstTexture)
{
	BOOL retVal = FALSE;

	if(	srcTexture->GetSurfaceFormat() == dstTexture->GetSurfaceFormat() &&
		srcTexture->GetLevelWidth(0) == dstTexture->GetLevelWidth(0) &&
		srcTexture->GetLevelHeight(0) == dstTexture->GetLevelHeight(0) &&
		srcTexture->GetLevelCount() == dstTexture->GetLevelCount())
	{
		switch(srcTexture->GetSurfaceFormat())
		{
			default:
				{
					INT32 bytesPerTexel = RuSurfaceFormat_GetBitsPerTexel(srcTexture->GetSurfaceFormat()) >> 3;

					for(INT32 i = 0, levelCount = srcTexture->GetLevelCount(); i < levelCount; ++i)
					{
						BYTE *srcBits, *dstBits;
						INT32 srcPitch, dstPitch;

						BOOL srcLockResult = srcTexture->Lock(i, reinterpret_cast<void **>(&srcBits), &srcPitch);
						BOOL dstLockResult = dstTexture->Lock(i, reinterpret_cast<void **>(&dstBits), &dstPitch);

						if(!srcLockResult || !dstLockResult)
							return FALSE;

						for(INT32 j = 0, rows = srcTexture->GetLevelHeight(i), columns = srcTexture->GetLevelWidth(i); j < rows; ++j)
						{
							memcpy(dstBits, srcBits, bytesPerTexel * columns);

							srcBits += srcPitch;
							dstBits += dstPitch;
						}

						srcTexture->Unlock(i);
						dstTexture->Unlock(i);
					}

					retVal = TRUE;
				}

				break;

			case ruSURFACEFMT_DXT1:
			case ruSURFACEFMT_DXT2:
			case ruSURFACEFMT_DXT3:
			case ruSURFACEFMT_DXT4:
			case ruSURFACEFMT_DXT5:
				{
					INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(srcTexture->GetSurfaceFormat()) << 1;

					for(INT32 i = 0, levelCount = srcTexture->GetLevelCount(); i < levelCount; ++i)
					{
						BYTE *srcBits, *dstBits;
						INT32 srcPitch, dstPitch;

						BOOL srcLockResult = srcTexture->Lock(i, reinterpret_cast<void **>(&srcBits), &srcPitch);
						BOOL dstLockResult = dstTexture->Lock(i, reinterpret_cast<void **>(&dstBits), &dstPitch);

						if(!srcLockResult || !dstLockResult)
							return FALSE;

						for(INT32 j = 0, rows = srcTexture->GetLevelHeight(i) >> 2, columns = srcTexture->GetLevelWidth(i) >> 2; j < rows; ++j)
						{
							memcpy(dstBits, srcBits, bytesPerBlock * columns);

							srcBits += srcPitch;
							dstBits += dstPitch;
						}

						srcTexture->Unlock(i);
						dstTexture->Unlock(i);
					}

					retVal = TRUE;
				}

				break;
		}
	}
	else
	{
		OutputDebugString("What\n");
	}

	return retVal;
}

BOOL RuTextureUtility_Clear(IRuTexture *dstTexture)
{
	BOOL retVal = FALSE;

	switch(dstTexture->GetSurfaceFormat())
	{
		default:
			{
				INT32 bytesPerTexel = RuSurfaceFormat_GetBitsPerTexel(dstTexture->GetSurfaceFormat()) >> 3;

				for(INT32 i = 0, levelCount = dstTexture->GetLevelCount(); i < levelCount; ++i)
				{
					BYTE *dstBits;
					INT32 dstPitch;

					dstTexture->Lock(i, reinterpret_cast<void **>(&dstBits), &dstPitch);

					for(INT32 j = 0, rows = dstTexture->GetLevelHeight(i), columns = dstTexture->GetLevelWidth(i); j < rows; ++j)
					{
						memset(dstBits, 0, bytesPerTexel * columns);

						dstBits += dstPitch;
					}

					dstTexture->Unlock(i);
				}

				retVal = TRUE;
			}

			break;

		case ruSURFACEFMT_DXT1:
		case ruSURFACEFMT_DXT2:
		case ruSURFACEFMT_DXT3:
		case ruSURFACEFMT_DXT4:
		case ruSURFACEFMT_DXT5:
			{
				INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(dstTexture->GetSurfaceFormat()) << 1;

				for(INT32 i = 0, levelCount = dstTexture->GetLevelCount(); i < levelCount; ++i)
				{
					BYTE *dstBits;
					INT32 dstPitch;

					dstTexture->Lock(i, reinterpret_cast<void **>(&dstBits), &dstPitch);

					for(INT32 j = 0, rows = dstTexture->GetLevelHeight(i) >> 2, columns = dstTexture->GetLevelWidth(i) >> 2; j < rows; ++j)
					{
						memset(dstBits, 0, bytesPerBlock * columns);

						dstBits += dstPitch;
					}

					dstTexture->Unlock(i);
				}

				retVal = TRUE;
			}

			break;
	}

	return retVal;
}

BOOL RuTextureUtility_SerializeFrom(IRuTexture *texture, INT32 level, IRuStream *stream)
{
	BOOL retVal = FALSE;

	switch(texture->GetSurfaceFormat())
	{
		default:
			{
				INT32 bytesPerTexel = RuSurfaceFormat_GetBitsPerTexel(texture->GetSurfaceFormat()) >> 3;

				// Lock current surface level for writing
				BYTE *bits;
				INT32 pitch;
				texture->Lock(level, reinterpret_cast<void **>(&bits), &pitch);

				// Iterate through all blocks
				for(INT32 j = 0, rows = texture->GetLevelHeight(level), columns = texture->GetLevelWidth(level); j < rows; ++j)
				{
					// Read scanline
					stream->Read(bits, bytesPerTexel * columns);

					// Advance to next row
					bits = bits + pitch;
				}

				// Unlock current surface level
				texture->Unlock(level);

				// Set return value
				retVal = TRUE;
			}

			break;

		case ruSURFACEFMT_DXT1:
		case ruSURFACEFMT_DXT2:
		case ruSURFACEFMT_DXT3:
		case ruSURFACEFMT_DXT4:
		case ruSURFACEFMT_DXT5:
			{
				INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(texture->GetSurfaceFormat()) << 1;

				// Lock current surface level for reading
				BYTE *bits;
				INT32 pitch;
				texture->Lock(level, reinterpret_cast<void **>(&bits), &pitch);

				// Iterate through all blocks
				for(INT32 j = 0, rows = texture->GetLevelHeight(level) >> 2, columns = texture->GetLevelWidth(level) >> 2; j < rows; ++j)
				{
					// Read scanline
					stream->Read(bits, bytesPerBlock * columns);

					// Advance to next row
					bits = bits + pitch;
				}

				// Unlock current surface level
				texture->Unlock(level);

				// Set return value
				retVal = TRUE;
			}

			break;
	}

	return retVal;
}

BOOL RuTextureUtility_SerializeTo(IRuTexture *texture, INT32 level, IRuStream *stream)
{
	BOOL retVal = FALSE;

	switch(texture->GetSurfaceFormat())
	{
		default:
			{
				INT32 bytesPerTexel = RuSurfaceFormat_GetBitsPerTexel(texture->GetSurfaceFormat()) >> 3;

				// Lock current surface level for reading
				BYTE *bits;
				INT32 pitch;
				texture->Lock(level, reinterpret_cast<void **>(&bits), &pitch);

				// Iterate through all blocks
				for(INT32 j = 0, rows = texture->GetLevelHeight(level), columns = texture->GetLevelWidth(level); j < rows; ++j)
				{
					// Write scanline
					stream->Write(bits, bytesPerTexel * columns);

					// Advance to next row
					bits = bits + pitch;
				}

				// Unlock current surface level
				texture->Unlock(level);

				// Set return value
				retVal = TRUE;
			}

			break;

		case ruSURFACEFMT_DXT1:
		case ruSURFACEFMT_DXT2:
		case ruSURFACEFMT_DXT3:
		case ruSURFACEFMT_DXT4:
		case ruSURFACEFMT_DXT5:
			{
				INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(texture->GetSurfaceFormat()) << 1;

				// Lock current surface level for reading
				BYTE *bits;
				INT32 pitch;
				texture->Lock(level, reinterpret_cast<void **>(&bits), &pitch);

				// Iterate through all blocks
				for(INT32 j = 0, rows = texture->GetLevelHeight(level) >> 2, columns = texture->GetLevelWidth(level) >> 2; j < rows; ++j)
				{
					// Write scanline
					stream->Write(bits, bytesPerBlock * columns);

					// Advance to next row
					bits = bits + pitch;
				}

				// Unlock current surface level
				texture->Unlock(level);

				// Set return value
				retVal = TRUE;
			}

			break;
	}

	return retVal;
}

BOOL RuTextureUtility_Copy_IRuVolumeTexture(IRuVolumeTexture *srcTexture, IRuVolumeTexture *dstTexture)
{
	BOOL retVal = FALSE;

	if(	srcTexture->GetSurfaceFormat() == dstTexture->GetSurfaceFormat() &&
		srcTexture->GetLevelWidth(0) == dstTexture->GetLevelWidth(0) &&
		srcTexture->GetLevelHeight(0) == dstTexture->GetLevelHeight(0) &&
		srcTexture->GetLevelDepth(0) == dstTexture->GetLevelDepth(0) &&
		srcTexture->GetLevelCount() == dstTexture->GetLevelCount())
	{
		switch(srcTexture->GetSurfaceFormat())
		{
			default:
				{
					INT32 bytesPerTexel = RuSurfaceFormat_GetBitsPerTexel(srcTexture->GetSurfaceFormat()) >> 3;

					for(INT32 i = 0, levelCount = srcTexture->GetLevelCount(); i < levelCount; ++i)
					{
						BYTE *srcSliceBits, *dstSliceBits;
						INT32 srcRowPitch, dstRowPitch;
						INT32 srcSlicePitch, dstSlicePitch;

						srcTexture->Lock(i, reinterpret_cast<void **>(&srcSliceBits), &srcRowPitch, &srcSlicePitch);
						dstTexture->Lock(i, reinterpret_cast<void **>(&dstSliceBits), &dstRowPitch, &dstSlicePitch);

						for(INT32 j = 0, slices = srcTexture->GetLevelDepth(i); j < slices; ++j)
						{
							BYTE *srcBits = srcSliceBits;
							BYTE *dstBits = dstSliceBits;

							for(INT32 k = 0, rows = srcTexture->GetLevelHeight(i), columns = srcTexture->GetLevelWidth(i); k < rows; ++k)
							{
								memcpy(dstBits, srcBits, bytesPerTexel * columns);

								srcBits += srcRowPitch;
								dstBits += dstRowPitch;
							}

							srcSliceBits += srcSlicePitch;
							dstSliceBits += dstSlicePitch;
						}

						srcTexture->Unlock(i);
						dstTexture->Unlock(i);
					}

					retVal = TRUE;
				}

				break;

			case ruSURFACEFMT_DXT1:
			case ruSURFACEFMT_DXT2:
			case ruSURFACEFMT_DXT3:
			case ruSURFACEFMT_DXT4:
			case ruSURFACEFMT_DXT5:
				{
					INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(srcTexture->GetSurfaceFormat()) << 1;

					for(INT32 i = 0, levelCount = srcTexture->GetLevelCount(); i < levelCount; ++i)
					{
						BYTE *srcSliceBits, *dstSliceBits;
						INT32 srcRowPitch, dstRowPitch;
						INT32 srcSlicePitch, dstSlicePitch;

						srcTexture->Lock(i, reinterpret_cast<void **>(&srcSliceBits), &srcRowPitch, &srcSlicePitch);
						dstTexture->Lock(i, reinterpret_cast<void **>(&dstSliceBits), &dstRowPitch, &dstSlicePitch);

						for(INT32 j = 0, slices = srcTexture->GetLevelDepth(i); j < slices; ++j)
						{
							BYTE *srcBits = srcSliceBits;
							BYTE *dstBits = dstSliceBits;

							for(INT32 k = 0, rows = srcTexture->GetLevelHeight(i) >> 2, columns = srcTexture->GetLevelWidth(i) >> 2; k < rows; ++k)
							{
								memcpy(dstBits, srcBits, bytesPerBlock * columns);

								srcBits += srcRowPitch;
								dstBits += dstRowPitch;
							}

							srcSliceBits += srcSlicePitch;
							dstSliceBits += dstSlicePitch;
						}

						srcTexture->Unlock(i);
						dstTexture->Unlock(i);
					}

					retVal = TRUE;
				}

				break;
		}
	}

	return retVal;
}

BOOL RuTextureUtility_SerializeFrom(IRuVolumeTexture *texture, INT32 level, IRuStream *stream)
{
	BOOL retVal = FALSE;

	switch(texture->GetSurfaceFormat())
	{
		default:
			{
				INT32 bytesPerTexel = RuSurfaceFormat_GetBitsPerTexel(texture->GetSurfaceFormat()) >> 3;

				BYTE *sliceBits;
				INT32 rowPitch, slicePitch;

				texture->Lock(level, reinterpret_cast<void **>(&sliceBits), &rowPitch, &slicePitch);

				for(INT32 j = 0, slices = texture->GetLevelDepth(level); j < slices; ++j)
				{
					BYTE *bits = sliceBits;

					for(INT32 k = 0, rows = texture->GetLevelHeight(level), columns = texture->GetLevelWidth(level); k < rows; ++k)
					{
						// Read scanline
						stream->Read(bits, bytesPerTexel * columns);

						bits += rowPitch;
					}

					sliceBits += slicePitch;
				}

				texture->Unlock(level);

				// Set return value
				retVal = TRUE;
			}

			break;

		case ruSURFACEFMT_DXT1:
		case ruSURFACEFMT_DXT2:
		case ruSURFACEFMT_DXT3:
		case ruSURFACEFMT_DXT4:
		case ruSURFACEFMT_DXT5:
			{
				INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(texture->GetSurfaceFormat()) << 1;

				BYTE *sliceBits;
				INT32 rowPitch, slicePitch;

				texture->Lock(level, reinterpret_cast<void **>(&sliceBits), &rowPitch, &slicePitch);

				for(INT32 j = 0, slices = texture->GetLevelDepth(level); j < slices; ++j)
				{
					BYTE *bits = sliceBits;

					for(INT32 k = 0, rows = texture->GetLevelHeight(level) >> 2, columns = texture->GetLevelWidth(level) >> 2; k < rows; ++k)
					{
						// Read scanline
						stream->Read(bits, bytesPerBlock * columns);

						bits += rowPitch;
					}

					sliceBits += slicePitch;
				}

				texture->Unlock(level);

				// Set return value
				retVal = TRUE;
			}

			break;
	}

	return retVal;
}

BOOL RuTextureUtility_SerializeTo(IRuVolumeTexture *texture, INT32 level, IRuStream *stream)
{
	BOOL retVal = FALSE;

	switch(texture->GetSurfaceFormat())
	{
		default:
			{
				INT32 bytesPerTexel = RuSurfaceFormat_GetBitsPerTexel(texture->GetSurfaceFormat()) >> 3;

				BYTE *sliceBits;
				INT32 rowPitch, slicePitch;

				texture->Lock(level, reinterpret_cast<void **>(&sliceBits), &rowPitch, &slicePitch);

				for(INT32 j = 0, slices = texture->GetLevelDepth(level); j < slices; ++j)
				{
					BYTE *bits = sliceBits;

					for(INT32 k = 0, rows = texture->GetLevelHeight(level), columns = texture->GetLevelWidth(level); k < rows; ++k)
					{
						// Write scanline
						stream->Write(bits, bytesPerTexel * columns);

						bits += rowPitch;
					}

					sliceBits += slicePitch;
				}

				texture->Unlock(level);

				// Set return value
				retVal = TRUE;
			}

			break;

		case ruSURFACEFMT_DXT1:
		case ruSURFACEFMT_DXT2:
		case ruSURFACEFMT_DXT3:
		case ruSURFACEFMT_DXT4:
		case ruSURFACEFMT_DXT5:
			{
				INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(texture->GetSurfaceFormat()) << 1;

				BYTE *sliceBits;
				INT32 rowPitch, slicePitch;

				texture->Lock(level, reinterpret_cast<void **>(&sliceBits), &rowPitch, &slicePitch);

				for(INT32 j = 0, slices = texture->GetLevelDepth(level); j < slices; ++j)
				{
					BYTE *bits = sliceBits;

					for(INT32 k = 0, rows = texture->GetLevelHeight(level) >> 2, columns = texture->GetLevelWidth(level) >> 2; k < rows; ++k)
					{
						// Write scanline
						stream->Write(bits, bytesPerBlock * columns);

						bits += rowPitch;
					}

					sliceBits += slicePitch;
				}

				texture->Unlock(level);

				// Set return value
				retVal = TRUE;
			}

			break;
	}

	return retVal;
}

// ************************************************************************************************************************************************************

void RuDXT1BlockDecode_A8R8G8B8(RuColorBlock_DXT1 &dxtBlock, RuPixel_A8R8G8B8 *dstBuffer, INT32 dstPitch)
{
	RuPixel_A8R8G8B8 colorArray[4];

	// Decode color 0 and color 1 into the color array
	colorArray[0].m_a = 0xFF;
	colorArray[0].m_r = (BYTE) ((UINT32) (dxtBlock.m_color0 & 0xF800) >> 8);
	colorArray[0].m_g = (BYTE) ((UINT32) (dxtBlock.m_color0 & 0x07E0) >> 3);
	colorArray[0].m_b = (BYTE) ((UINT32) (dxtBlock.m_color0 & 0x001F) << 3);
	colorArray[1].m_a = 0xFF;
	colorArray[1].m_r = (BYTE) ((UINT32) (dxtBlock.m_color1 & 0xF800) >> 8);
	colorArray[1].m_g = (BYTE) ((UINT32) (dxtBlock.m_color1 & 0x07E0) >> 3);
	colorArray[1].m_b = (BYTE) ((UINT32) (dxtBlock.m_color1 & 0x001F) << 3);

	if(dxtBlock.m_color0 > dxtBlock.m_color1)		// DXT1 without 1-bit alpha
	{
		colorArray[2].m_a = 0xFF;
		colorArray[2].m_r = (2 * colorArray[0].m_r + colorArray[1].m_r) / 3;
		colorArray[2].m_g = (2 * colorArray[0].m_g + colorArray[1].m_g) / 3;
		colorArray[2].m_b = (2 * colorArray[0].m_b + colorArray[1].m_b) / 3;
		colorArray[3].m_a = 0xFF;
		colorArray[3].m_r = (colorArray[0].m_r + 2 * colorArray[1].m_r) / 3;
		colorArray[3].m_g = (colorArray[0].m_g + 2 * colorArray[1].m_g) / 3;
		colorArray[3].m_b = (colorArray[0].m_b + 2 * colorArray[1].m_b) / 3;
	}
	else											// DXT1 with 1-bit alpha
	{
		colorArray[2].m_a = 0xFF;
		colorArray[2].m_r = (colorArray[0].m_r + colorArray[1].m_r) / 2;
		colorArray[2].m_g = (colorArray[0].m_g + colorArray[1].m_g) / 2;
		colorArray[2].m_b = (colorArray[0].m_b + colorArray[1].m_b) / 2;
		colorArray[3].m_color = 0x00000000;
	}

	// Decode color block
	INT32 idx = dxtBlock.m_index;

	for(INT32 yy = 0; yy < 4; ++yy)
	{
//		if( yy >= height)
//			break;

		for(INT32 xx = 0; xx < 4; ++xx)
		{
//			if(xx < width)
			dstBuffer[xx] = colorArray[idx & 0x03];

			// Advance to next index
			idx >>= 2;
		}

		// Advance to next line in destination buffer
		dstBuffer = (RuPixel_A8R8G8B8 *) ((BYTE *) dstBuffer + dstPitch);
	}
}

BOOL RuSurfaceUtility_Convert(IRuSurface *srcSurface, IRuSurface *dstSurface)
{
	if(srcSurface->GetSurfaceFormat() == dstSurface->GetSurfaceFormat())
	{
		switch(srcSurface->GetSurfaceFormat())
		{
			default:
				{
					INT32 bytesPerTexel = RuSurfaceFormat_GetBitsPerTexel(srcSurface->GetSurfaceFormat()) >> 3;

					BYTE *srcBits, *dstBits;
					INT32 srcPitch, dstPitch;

					BOOL srcLockResult = srcSurface->Lock(reinterpret_cast<void **>(&srcBits), &srcPitch);
					BOOL dstLockResult = dstSurface->Lock(reinterpret_cast<void **>(&dstBits), &dstPitch);

					if(!srcLockResult || !dstLockResult)
						return FALSE;

					for(INT32 j = 0, rows = srcSurface->GetHeight(), columns = srcSurface->GetWidth(); j < rows; ++j)
					{
						memcpy(dstBits, srcBits, bytesPerTexel * columns);

						srcBits += srcPitch;
						dstBits += dstPitch;
					}

					srcSurface->Unlock();
					dstSurface->Unlock();

					return TRUE;
				}

				break;

			case ruSURFACEFMT_DXT1:
			case ruSURFACEFMT_DXT2:
			case ruSURFACEFMT_DXT3:
			case ruSURFACEFMT_DXT4:
			case ruSURFACEFMT_DXT5:
				{
					INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(srcSurface->GetSurfaceFormat()) << 1;

					BYTE *srcBits, *dstBits;
					INT32 srcPitch, dstPitch;

					BOOL srcLockResult = srcSurface->Lock(reinterpret_cast<void **>(&srcBits), &srcPitch);
					BOOL dstLockResult = dstSurface->Lock(reinterpret_cast<void **>(&dstBits), &dstPitch);

					if(!srcLockResult || !dstLockResult)
						return FALSE;

					for(INT32 j = 0, rows = srcSurface->GetHeight() >> 2, columns = srcSurface->GetWidth() >> 2; j < rows; ++j)
					{
						memcpy(dstBits, srcBits, bytesPerBlock * columns);

						srcBits += srcPitch;
						dstBits += dstPitch;
					}

					srcSurface->Unlock();
					dstSurface->Unlock();

					return TRUE;
				}

				break;
		}
	}
	else
	{
		switch(srcSurface->GetSurfaceFormat())
		{
			case ruSURFACEFMT_DXT1:
				{
					switch(dstSurface->GetSurfaceFormat())
					{
						case ruSURFACEFMT_A8R8G8B8:
							{
								RuColorBlock_DXT1 *srcBits;
								RuPixel_A8R8G8B8 *dstBits;
								INT32 srcPitch;
								INT32 dstPitch;

								srcSurface->Lock(reinterpret_cast<void **>(&srcBits), &srcPitch);
								dstSurface->Lock(reinterpret_cast<void **>(&dstBits), &dstPitch);

	//							INT32 dxtWidth = (srcSurface->GetWidth() + 3) / 4;
	//							INT32 dxtHeight = (srcSurface->GetHeight() + 3) / 4;

								INT32 dxtWidth = srcSurface->GetWidth() / 4;
								INT32 dxtHeight = srcSurface->GetHeight() / 4;

								for(INT32 yy = 0; yy < dxtHeight; ++yy)
								{
									for(INT32 xx = 0; xx < dxtWidth; ++xx)
									{
										// Decode current DXT1 block
										RuDXT1BlockDecode_A8R8G8B8(srcBits[xx], &dstBits[xx * 4], dstPitch);
									}

									// Advance to next destination row
									dstBits = (RuPixel_A8R8G8B8 *) ((BYTE *) dstBits + dstPitch * 4);

									// Advance to next source row
									srcBits = (RuColorBlock_DXT1 *) ((BYTE *) srcBits + srcPitch);
								}

								srcSurface->Unlock();
								dstSurface->Unlock();
							}

							return TRUE;
					}
				}

				break;

			case ruSURFACEFMT_DXT2:
			case ruSURFACEFMT_DXT3:
			case ruSURFACEFMT_DXT4:
			case ruSURFACEFMT_DXT5:
				break;
		}
	}

	return FALSE;
}

BOOL RuSurfaceUtility_SerializeFrom(IRuSurface *surface, IRuStream *stream)
{
	BOOL retVal = FALSE;

	switch(surface->GetSurfaceFormat())
	{
		default:
			{
				INT32 bytesPerTexel = RuSurfaceFormat_GetBitsPerTexel(surface->GetSurfaceFormat()) >> 3;

				// Lock surface for writing
				BYTE *bits;
				INT32 pitch;
				surface->Lock(reinterpret_cast<void **>(&bits), &pitch);

				// Iterate through all blocks
				for(INT32 j = 0, rows = surface->GetHeight(), columns = surface->GetWidth(); j < rows; ++j)
				{
					// Read scanline
					stream->Read(bits, bytesPerTexel * columns);

					// Advance to next row
					bits = bits + pitch;
				}

				// Unlock surface
				surface->Unlock();

				// Set return value
				retVal = TRUE;
			}

			break;

		case ruSURFACEFMT_DXT1:
		case ruSURFACEFMT_DXT2:
		case ruSURFACEFMT_DXT3:
		case ruSURFACEFMT_DXT4:
		case ruSURFACEFMT_DXT5:
			{
				INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(surface->GetSurfaceFormat()) << 1;

				// Lock surface for reading
				BYTE *bits;
				INT32 pitch;
				surface->Lock(reinterpret_cast<void **>(&bits), &pitch);

				// Iterate through all blocks
				for(INT32 j = 0, rows = surface->GetHeight() >> 2, columns = surface->GetWidth() >> 2; j < rows; ++j)
				{
					// Read scanline
					stream->Read(bits, bytesPerBlock * columns);

					// Advance to next row
					bits = bits + pitch;
				}

				// Unlock surface
				surface->Unlock();

				// Set return value
				retVal = TRUE;
			}

			break;
	}

	return retVal;
}

BOOL RuSurfaceUtility_SerializeTo(IRuSurface *surface, IRuStream *stream)
{
	BOOL retVal = FALSE;

	switch(surface->GetSurfaceFormat())
	{
		default:
			{
				INT32 bytesPerTexel = RuSurfaceFormat_GetBitsPerTexel(surface->GetSurfaceFormat()) >> 3;

				// Lock surface for reading
				BYTE *bits;
				INT32 pitch;
				surface->Lock(reinterpret_cast<void **>(&bits), &pitch);

				// Iterate through all blocks
				for(INT32 j = 0, rows = surface->GetHeight(), columns = surface->GetWidth(); j < rows; ++j)
				{
					// Write scanline
					stream->Write(bits, bytesPerTexel * columns);

					// Advance to next row
					bits = bits + pitch;
				}

				// Unlock surface
				surface->Unlock();

				// Set return value
				retVal = TRUE;
			}

			break;

		case ruSURFACEFMT_DXT1:
		case ruSURFACEFMT_DXT2:
		case ruSURFACEFMT_DXT3:
		case ruSURFACEFMT_DXT4:
		case ruSURFACEFMT_DXT5:
			{
				INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(surface->GetSurfaceFormat()) << 1;

				// Lock surface for reading
				BYTE *bits;
				INT32 pitch;
				surface->Lock(reinterpret_cast<void **>(&bits), &pitch);

				// Iterate through all blocks
				for(INT32 j = 0, rows = surface->GetHeight() >> 2, columns = surface->GetWidth() >> 2; j < rows; ++j)
				{
					// Write scanline
					stream->Write(bits, bytesPerBlock * columns);

					// Advance to next row
					bits = bits + pitch;
				}

				// Unlock surface
				surface->Unlock();

				// Set return value
				retVal = TRUE;
			}

			break;
	}

	return retVal;
}

// ************************************************************************************************************************************************************

CRuFileStream *CRuDXTCompressor::m_outputStream = NULL;

CRuDXTCompressor::CRuDXTCompressor()
{
	m_outputStream = NULL;
}

CRuDXTCompressor::~CRuDXTCompressor()
{
	delete m_outputStream;
}

BOOL CRuDXTCompressor::SaveToDXT(granny_texture *srcTexture, const char *filename, BOOL premultipliedAlpha)
{
	CRuString sourceFilename(srcTexture->FromFileName);
	sourceFilename.ToLower();
	BOOL useDXT5 = sourceFilename.IndexOf(".png") >= 0 ? TRUE : FALSE;

	// Copy raw bits for mip level 0
	INT32 texelCount = srcTexture->Width * srcTexture->Height;
	UINT8 *imageBits = ruNEW UINT8 [texelCount * 4];
	GrannyCopyTextureImage(srcTexture, 0, 0, GrannyBGRA8888PixelFormat, srcTexture->Width, srcTexture->Height, srcTexture->Width * 4, imageBits);

	// If premultiplied alpha is enabled, perform the multiplication now
	if(premultipliedAlpha)
	{
		RUCOLOR *imageColors = reinterpret_cast<RUCOLOR *>(imageBits);
		for(INT32 i = 0; i < texelCount; ++i)
		{
			CRuColor4 texelColor(imageColors[i]);
			texelColor.m_r *= texelColor.m_a;
			texelColor.m_g *= texelColor.m_a;
			texelColor.m_b *= texelColor.m_a;

			imageColors[i] = texelColor.GetARGB();
		}
	}

	// Compress and output
	nvCompressionOptions options;

	options.mipMapGeneration = kGenerateMipMaps;
	if(useDXT5)
		options.textureFormat = kDXT5;
	else
		options.textureFormat = kDXT1a;

	m_outputStream = ruNEW CRuFileStream();
	m_outputStream->Create(filename);

	//nvDDS::nvDXTcompress(imageBits, srcTexture->Width, srcTexture->Height, srcTexture->Width * 4, nvBGRA, &options, (DXTWriteCallback) &CRuDXTCompressor::WriteDXTnFile);

	delete m_outputStream;
	m_outputStream = NULL;

	return TRUE;
}

BOOL CRuDXTCompressor::SaveToDXT(RUCOLOR *pixelArray, INT32 width, INT32 height, const char *filename, BOOL premultipliedAlpha, BOOL useDXT5)
{
	// If premultiplied alpha is enabled, perform the multiplication now
	if(premultipliedAlpha)
	{
		INT32 texelCount = width * height;
		for(INT32 i = 0; i < texelCount; ++i)
		{
			CRuColor4 texelColor(pixelArray[i]);
			texelColor.m_r *= texelColor.m_a;
			texelColor.m_g *= texelColor.m_a;
			texelColor.m_b *= texelColor.m_a;

			pixelArray[i] = texelColor.GetARGB();
		}
	}

	// Compress and output
	nvCompressionOptions options;

	options.mipMapGeneration = kGenerateMipMaps;
	if(useDXT5)
		options.textureFormat = kDXT5;
	else
		options.textureFormat = kDXT1a;

	m_outputStream = ruNEW CRuFileStream();
	m_outputStream->Create(filename);

	//nvDDS::nvDXTcompress(reinterpret_cast<UINT8 *>(pixelArray), width, height, width * 4, nvBGRA, &options, (DXTWriteCallback) &CRuDXTCompressor::WriteDXTnFile);

	delete m_outputStream;
	m_outputStream = NULL;

	return TRUE;
}

BOOL CRuDXTCompressor::ConvertToDXT(IRuTexture *srcTexture, IRuTexture *dstTexture)
{
	INT32 lvWidth = srcTexture->GetLevelWidth(0);
	INT32 lvHeight = srcTexture->GetLevelHeight(0);

	// Allocate enough space for the highest LOD
	INT32 texelCount = lvWidth * lvHeight;
	UINT8 *imageBits = ruNEW UINT8 [texelCount * 4];
/*
	// Convert each mip level
	for(INT32 i = 0, levelCount = srcTexture->GetLevelCount(); i < levelCount; ++i)
	{
		INT32 lvWidth = srcTexture->GetLevelHeight(i);
		INT32 lvHeight = srcTexture->GetLevelWidth(i);

		ATI_TC_Texture atiSrcTex;
		atiSrcTex.dwSize = sizeof(atiSrcTex);
		atiSrcTex.dwWidth = lvWidth;
		atiSrcTex.dwHeight = lvHeight;
		atiSrcTex.format = ATI_TC_FORMAT_ARGB_8888;

		if(srcTexture->Lock(i, reinterpret_cast<void **>(&atiSrcTex.pData), reinterpret_cast<INT32 *>(&atiSrcTex.dwPitch)))
		{
			ATI_TC_Texture atiDstTex;
			atiDstTex.dwSize = sizeof(atiDstTex);
			atiDstTex.dwWidth = lvWidth;
			atiDstTex.dwHeight = lvHeight;
			atiDstTex.format = ATI_TC_FORMAT_DXT1;

			ATI_TC_CompressOptions atiCompressOptions;
			atiCompressOptions.dwSize = sizeof(atiCompressOptions);
			atiCompressOptions.bUseChannelWeighting = false;
			atiCompressOptions.bUseAdaptiveWeighting = false;
			atiCompressOptions.bDXT1UseAlpha = false;

			if(dstTexture->Lock(i, reinterpret_cast<void **>(&atiDstTex.pData), reinterpret_cast<INT32 *>(&atiDstTex.dwPitch)))
			{
				ATI_TC_ConvertTexture(&atiSrcTex, &atiDstTex, &atiCompressOptions, NULL, NULL, NULL);

				dstTexture->Unlock(i);
			}


			srcTexture->Unlock(i);
		}
	}
*/

	// Image conversion to 32-bit BGRA
	switch(srcTexture->GetSurfaceFormat())
	{
		case ruSURFACEFMT_X1R5G5B5:
			{
				UINT16 *srcImageBits;
				INT32 srcImagePitch;

				UINT32 *dstImageBits = reinterpret_cast<UINT32 *>(imageBits);

				if(srcTexture->Lock(0, reinterpret_cast<void **>(&srcImageBits), &srcImagePitch))
				{
					for(INT32 z = 0; z < lvHeight; ++z)
					{
						for(INT32 x = 0; x < lvWidth; ++x)
						{
							dstImageBits[x] = RUCOLOR_ARGB(0xFF, (srcImageBits[x] & 0x7C00) >> 7, (srcImageBits[x] & 0x03E0) >> 2, (srcImageBits[x] & 0x001F) << 3);
						}

						// Increment source and destination pointers to the next row
						srcImageBits = reinterpret_cast<UINT16 *>(reinterpret_cast<BYTE *>(srcImageBits) + srcImagePitch);
						dstImageBits = dstImageBits + lvWidth;
					}

					srcTexture->Unlock(0);
				}
			}

			break;

		case ruSURFACEFMT_A8R8G8B8:
			{
				UINT32 *srcImageBits;
				INT32 srcImagePitch;

				UINT32 *dstImageBits = reinterpret_cast<UINT32 *>(imageBits);

				if(srcTexture->Lock(0, reinterpret_cast<void **>(&srcImageBits), &srcImagePitch))
				{
					for(INT32 z = 0; z < lvHeight; ++z)
					{
						// Copy whole row
						memcpy(dstImageBits, srcImageBits, sizeof(UINT32) * lvWidth);

						// Increment source and destination pointers to the next row
						srcImageBits = reinterpret_cast<UINT32 *>(reinterpret_cast<BYTE *>(srcImageBits) + srcImagePitch);
						dstImageBits = dstImageBits + lvWidth;
					}

					srcTexture->Unlock(0);
				}
			}

			break;
	}

	// Compress
	DXTnWriteStruct writeStruct = { dstTexture, 0 };

	nvCompressionOptions options;

	options.quality = kQualityFastest;
	options.mipMapGeneration = kNoMipMaps;
	options.mipFilterType = kMipFilterBox;
	options.numMipMapsToWrite = 1;
	options.bForceDXT1FourColors = true;
	options.textureFormat = kDXT1;
	options.weightType = kGreyScaleWeighting;
	options.weight[0] = 1.0f / 3.0f;
	options.weight[1] = 1.0f / 3.0f;
	options.weight[2] = 1.0f / 3.0f;
	options.user_data = reinterpret_cast<void *>(&writeStruct);

	//nvDDS::nvDXTcompress(imageBits, lvWidth, lvHeight, lvWidth * 4, nvBGRA, &options, (DXTWriteCallback) &CRuDXTCompressor::WriteDXTnMemory);

	delete[] imageBits;

	return TRUE;
}

INT32 CRuDXTCompressor::WriteDXTnFile(const void *buffer, size_t count, const MIPMapData *mipMapData, void *userData)
{
	m_outputStream->Write(buffer, (DWORD)count);

	return 0;
}

INT32 CRuDXTCompressor::WriteDXTnMemory(const void *buffer, size_t count, const MIPMapData *mipMapData, void *userData)
{
	if(mipMapData)
	{
		DXTnWriteStruct &writeStruct = *reinterpret_cast<DXTnWriteStruct *>(userData);

		const BYTE *srcBits = reinterpret_cast<const BYTE *>(buffer);
		INT32 bufferOffset = 0;

		BYTE *dstBits;
		INT32 dstPitch;

		if(writeStruct.m_dstTexture->Lock((UINT32)mipMapData->mipLevel, reinterpret_cast<void **>(&dstBits), &dstPitch))
		{
			for(UINT32 z = 0, rows = (UINT32)(mipMapData->height >> 2); z < rows; ++z)
			{
				memcpy(dstBits, &srcBits[bufferOffset], (mipMapData->width >> 2) * 8);

				bufferOffset += (INT32)(mipMapData->width >> 2) * 8;
				dstBits += dstPitch;
			}

			writeStruct.m_dstTexture->Unlock((UINT32)mipMapData->mipLevel);
		}
	}

	return 0;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
