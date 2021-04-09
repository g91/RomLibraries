#include "cImageTga.h"

#if IMAGELIB_SUPPORT_TGA

// Definitions for image types.
#define TGA_Null 0
#define TGA_Map 1
#define TGA_RGB 2
#define TGA_Mono 3
#define TGA_RLEMap 9
#define TGA_RLERGB 10
#define TGA_RLEMono 11
#define TGA_CompMap 32
#define TGA_CompMap4 33

bool cImageTGA::Decode( FILE *fptr )
{
	if ( fptr == NULL ) {
		return false;
	}

	// 取得檔案大小
	//fseek(file, 0, SEEK_END);
	//int iFileSize = ftell(file);

	fseek( fptr, 0, SEEK_SET );

	// 讀取檔頭,判斷是否為TGA圖形檔案
	TGAHEADER info;
	fread( &info, sizeof(info), 1, fptr );

	bool compressBool;
	switch ( info.bImageDataType )
	{
		case TGA_Map:
		case TGA_RGB:
		case TGA_Mono:
			compressBool = false;
			break;
		case TGA_RLEMap:
		case TGA_RLERGB:
		case TGA_RLEMono:
			compressBool = true;
			break;
		default:
			return false;
	}	

	if ( info.bDepth != 8 && info.bDepth != 15 && info.bDepth != 16 && info.bDepth != 24 && info.bDepth != 32 ) {
		return false;
	}

	if ( info.wWidth == 0 || info.wHeight == 0 || info.bDepth == 0 || info.wColorMapSize > 256 ) {
		return false;
	}

	if ( info.bFieldLength > 0 ) fseek( fptr, info.bFieldLength, SEEK_CUR );		//skip descriptor

	if ( !Create( info.wWidth, info.wHeight, info.bDepth, IMAGELIB_FORMAT_TGA ) ) {
		return false;
	}

	// 檢查色盤資料
	if ( info.bColorMapType != 0 ) {		
		if ( info.bColorMapEntry == 24 ) {
			BGRCOLOR pal[256];
			fread( pal, sizeof(BGRCOLOR) * info.wColorMapSize, 1, fptr );
			SetPalette( pal, info.wColorMapSize );
		} else if ( info.bColorMapEntry = 32 ) {
			RGBQUAD pal[256];
			fread( pal, sizeof(RGBQUAD) * info.wColorMapSize, 1, fptr );
			SetPalette( pal, info.wColorMapSize );
			SetTransparent( true );			
		} else {
			return false;
		}
	}

	// 將檔案載入記憶體內
	int pos = ftell( fptr );
	fseek( fptr, 0, SEEK_END );
	int tmpSize = ftell( fptr ) - pos;
	fseek( fptr, pos, SEEK_SET );
	BYTE *pTmp = new BYTE[tmpSize];	
	fread( pTmp, tmpSize, 1, fptr );
	
	switch ( info.bDepth )
	{
		case 8:
			m_pixelFormat = PF_I8;
			Load8Bits( pTmp, &info, compressBool );
			break;
		case 15:
		case 16:
			m_pixelFormat = PF_R8G8B8;
			Load16Bits( pTmp, &info, compressBool );
			break;
		case 24:
			m_pixelFormat = PF_R8G8B8;
			Load8Bits( pTmp, &info, compressBool );
			break;
		case 32:
			m_pixelFormat = PF_A8R8G8B8;
			Load8Bits( pTmp, &info, compressBool );
			break;
	}
	if ( info.bDescriptor & 0x10 ) Mirror();	

	delete [] pTmp;
	return true;
}

bool cImageTGA::Encode( FILE *fptr )
{
	switch(m_depth)
	{
	case 8:
		Save8Bits( fptr );
		return false;

	case 16:
	case 24:
	case 32:
		Save32Bits( fptr );
		return true;
	}
	return false;
}

void cImageTGA::Load8Bits( BYTE *pData, TGAHEADER *pInfo, bool compressedBool )
{
	int lineBytes = ((pInfo->bDepth + 1) >> 3) * m_width;
	BYTE *pDest;
	for ( int yy = 0; yy < m_height; yy++ ) {		
		if ( !(pInfo->bDescriptor & 0x20) ) {			// 下而上讀取資料
			pDest = m_pPixels + (m_height - 1 - yy) * lineBytes;
		} else {
			pDest = m_pPixels + yy * lineBytes;
		}

		if ( compressedBool ) {
			pData += Decompress( pData, pDest, lineBytes, (pInfo->bDepth + 1) >> 3 );
		} else {
			memcpy( pDest, pData, lineBytes );
			pData += lineBytes;
		}
	}
}

void cImageTGA::Load16Bits( BYTE *pData, TGAHEADER *pInfo, bool compressedBool )
{
	int lineBytes = ((pInfo->bDepth + 1) >> 3) * m_width;	
	WORD *pTmp = new WORD[lineBytes / 2];
	BGRCOLOR *pDest;
	for ( int yy = 0; yy < m_height; yy++ ) {
		if ( !(pInfo->bDescriptor & 0x20) ) {			// 下而上讀取資料
			pDest = (BGRCOLOR *)m_pPixels + (m_height - 1 - yy) * m_width;
		} else {
			pDest = (BGRCOLOR *)m_pPixels + yy * m_width;
		}

		if ( compressedBool ) {
			pData += Decompress( pData, (BYTE*)pTmp, lineBytes, (pInfo->bDepth + 1) >> 3 );
		} else {
			memcpy( pTmp, pData, lineBytes );
			pData += lineBytes;
		}

		// 轉換為24Bits
		for ( int xx = 0; xx < m_width; xx++ ) {
			pDest[xx].b = g_5BitsTo8BitsArray[(pTmp[xx] & 0x7C00) >> 10];
			pDest[xx].g = g_5BitsTo8BitsArray[(pTmp[xx] & 0x3E0) >> 5];
			pDest[xx].r = g_5BitsTo8BitsArray[(pTmp[xx] & 0x1F)];
		}
	}

	delete [] pTmp;
}

void cImageTGA::Save8Bits( FILE* fptr )
{
	// 自己寫就有了
}

void cImageTGA::Save32Bits( FILE* fptr )
{
	if ( fptr == NULL )
		return;

	TGAHEADER fileHeader;	
	memset(&fileHeader, 0, sizeof(fileHeader));

	fileHeader.bFieldLength = 0;
	fileHeader.bColorMapType = 0;
	fileHeader.bImageDataType = 2;					// 未壓縮
	fileHeader.wColorMapFirstIndex = 0;
	fileHeader.wColorMapSize = 0;
	fileHeader.bColorMapEntry = 0;
	fileHeader.wOriginX = 0;
	fileHeader.wOriginY = 0;
	fileHeader.wWidth = m_width;
	fileHeader.wHeight = m_height;
	fileHeader.bDepth = m_depth;
	fileHeader.bDescriptor = 0x20;

	TGAFOOTER fileFooter;
	memset(&fileFooter, 0, sizeof(fileFooter));
	memcpy(fileFooter.szSignature, "TRUEVISION-XFILE.", 17);

	fwrite(&fileHeader, 1, sizeof(TGAHEADER), fptr);
	fwrite(m_pPixels, 1, (unsigned int)(sizeof(BYTE) * (m_depth / 8) * m_width * m_height), fptr);
	fwrite(&fileFooter, 1, sizeof(TGAFOOTER), fptr);
}

int cImageTGA::Compress( BYTE *pSrc, BYTE *pDest, int size, int bpp )
{
	BYTE tmp[128 * 4];
	int count = 0;
	int not = 0;	
	int repeat = 1;

	for ( int len = 0; len < size; len += bpp ) {
		if ( len != size - bpp && memcmp( pSrc + len, pSrc + (len + bpp), bpp) == 0 ) {
			repeat++;
		} else if ( repeat > 1 ) {
			if ( not > 0 ) {
				pDest[count++] = not - 1;
				memcpy( pDest + count, tmp, not * bpp );
				count += not * bpp;
				not = 0;
			}
			for ( int i = 0; i < repeat / 128; i++ ) {
				pDest[count++] = 0xFF;
				memcpy( pDest + count, pSrc + len, bpp );
				count += bpp;
			}
			if ( (repeat % 128) != 0 ) {
				pDest[count++] = 0x80 | ((repeat%128) - 1);
				memcpy( pDest + count, pSrc + len, bpp );
				count += bpp;
			}
			repeat = 1;
		} else {
			memcpy( tmp + (bpp * not++), pSrc + len, bpp );
			if ( not > 127 ) {
				pDest[count++] = not - 1;
				memcpy( pDest + count, tmp, not * bpp );
				count += not * bpp;
				not = 0;
			}
		}
	}

	if ( not > 0 ) {
		pDest[count++] = not - 1;
		memcpy( pDest + count, tmp, not * bpp );
		count += not * bpp;
	}

	return count;
}

int cImageTGA::Decompress( BYTE* pSrc, BYTE* pDest, int size, int bpp )
{
	int repeat;
	int srcCount = 0;
	int destCount = 0;
	while( destCount < size )	{
		repeat = ( pSrc[srcCount] & 0x7F ) + 1;
		if ( pSrc[srcCount++] & 0x80 ) {
			for ( int i = 0; i < repeat; i++ ) {
				memcpy( &pDest[destCount], &pSrc[srcCount], bpp );
				destCount += bpp;
			}
			srcCount += bpp;
		} else {
			memcpy( &pDest[destCount], &pSrc[srcCount], repeat * bpp );
			srcCount += repeat * bpp;
			destCount += repeat * bpp;
		}
	}
	return srcCount;
}

#endif