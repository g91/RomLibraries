#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <ddraw.h>
#include "dxtutility.h"

static BYTE *g_writeData = NULL;
static int g_writeCount = 0;
static int g_writeStep = 0;
static bool g_writeHeaderBool = false; 

inline bool IsPower2(unsigned int x)
{              
	if ( x < 1 )
		return false;

	if (x == 1)
		return true;

	if ( x & (x-1) )        
		return false;

	return true;
}


/*
HRESULT WriteDTXnFile(size_t count, void *buffer, void * userData)
{
	if ( g_writeStep > 1 || g_writeHeaderBool ) {
		memcpy( g_writeData + g_writeCount, buffer, count );
		g_writeCount += count;
	}
	g_writeStep++;
	return 0;
}
*/
NV_ERROR_CODE WriteDTXnFile(const void *buffer, size_t count, const MIPMapData *mipMapData, void *userData)
{
	if ( g_writeStep > 1 || g_writeHeaderBool )
	{
		memcpy( g_writeData + g_writeCount, buffer, count );
		g_writeCount += count;
	}

	g_writeStep++;

	return NV_OK;
}

inline DWORD Color565To32( WORD col )
{
	DXT_COLOR c;
	c.a = 0xff;
	c.r = (BYTE)( (long)(col >> 11) * 255 / 31 );
	c.g = (BYTE)( (long)((col >> 5) & 0x3f) * 255 / 63 );
	c.b = (BYTE)( (long)(col & 0x1f) * 255 / 31 );
	return c.Col;
}

inline void WriteBlockColor( DXT_COLOR *pDest, BLOCK_DXT1 *pDxt1, int width, int height )
{
	static DXT_COLOR colorArray[4];	
	//colorArray[0].Col = Color565To32( pDxt1->c[0] );
	//colorArray[1].Col = Color565To32( pDxt1->c[1] );

	colorArray[0].a = 0xff;
	colorArray[0].r = (BYTE)( (long)(pDxt1->c[0] >> 11) * 255 / 31 );;
	colorArray[0].g = (BYTE)( (long)((pDxt1->c[0] >> 5) & 0x3f) * 255 / 63 );
	colorArray[0].b = (BYTE)( (long)(pDxt1->c[0] & 0x1f) * 255 / 31 );
	colorArray[1].a = 0xff;
	colorArray[1].r = (BYTE)( (long)(pDxt1->c[1] >> 11) * 255 / 31 );;
	colorArray[1].g = (BYTE)( (long)((pDxt1->c[1] >> 5) & 0x3f) * 255 / 63 );
	colorArray[1].b = (BYTE)( (long)(pDxt1->c[1] & 0x1f) * 255 / 31 );	

	// DXT1可以儲存一Bit當透空色
	if ( pDxt1->c[0] > pDxt1->c[1] ) {		// 未有透空色處理
		colorArray[2].a	= 0xff;
		colorArray[2].r	= (2 * colorArray[0].r + colorArray[1].r) / 3;
		colorArray[2].g	= (2 * colorArray[0].g + colorArray[1].g) / 3;
		colorArray[2].b	= (2 * colorArray[0].b + colorArray[1].b) / 3;
		colorArray[3].a	= 0xff;
		colorArray[3].r	= (colorArray[0].r + 2 * colorArray[1].r) / 3;
		colorArray[3].g	= (colorArray[0].g + 2 * colorArray[1].g) / 3;
		colorArray[3].b	= (colorArray[0].b + 2 * colorArray[1].b) / 3;
	} else {								// 內函一Bit當透空色
		colorArray[2].a	= 0xff;
		colorArray[2].r	= (colorArray[0].r + colorArray[1].r) / 2;
		colorArray[2].g	= (colorArray[0].g + colorArray[1].g) / 2;
		colorArray[2].b	= (colorArray[0].b + colorArray[1].b) / 2;
		colorArray[3].Col = 0;
	}

	// 取得顏色值
	int idx = pDxt1->index;
	for ( int yy = 0; yy < 4; yy++ ) {
		if ( yy >= height ) break;
		for ( int xx = 0; xx < 4; xx++ ) {
			if ( xx < width ) pDest[xx] = colorArray[idx & 0x03];
			idx >>= 2;
		}
		pDest += width;
	}
}

bool DeCompressDXT( BYTE *pDest, BYTE *pSrc )
{
	DDSURFACEDESC2 *pddsd = NULL;
	BYTE *pPixels = NULL;

	// 檢查是否為DXT圖形格式
	if ( pDest == NULL || pSrc == NULL || memcmp(pSrc, "DDS ", 4 ) != 0 ) {
		return false;
	}

	pddsd = (DDSURFACEDESC2*)(pSrc + 4);
	pPixels = pSrc + 4 + sizeof(DDSURFACEDESC2);
	if ( pddsd->ddpfPixelFormat.dwFourCC == DXT_1 ) {
		PlotDXT1( pDest, pPixels, pddsd->dwWidth, pddsd->dwHeight );
	} else if ( pddsd->ddpfPixelFormat.dwFourCC == DXT_3 ) {
		PlotDXT3( pDest, pPixels, pddsd->dwWidth, pddsd->dwHeight );
	} else if ( pddsd->ddpfPixelFormat.dwFourCC == DXT_5 ) {
		PlotDXT5( pDest, pPixels, pddsd->dwWidth, pddsd->dwHeight );
	} else {
		return false;
	}
	return true;
}

void PlotDXT1( BYTE *pDest, BYTE *pSrc, int width, int height )
{	
	int dxtWidth = (width + 3) / 4;
	int dxtHeight = (height + 3) / 4;
	BLOCK_DXT1* pBlock = (BLOCK_DXT1*)pSrc;

	for ( int yy = 0; yy < dxtHeight; yy++ ) {
		DXT_COLOR *pRowPixel = ((DXT_COLOR *)pDest) + 4 * yy * width;
		for ( int xx = 0; xx < dxtWidth; xx++ ) {
			WriteBlockColor( pRowPixel, pBlock, width, height );
			pBlock++;
			pRowPixel += 4;
		}
	}
}

void PlotDXT3( BYTE *pDest, BYTE *pSrc, int width, int height )
{
	BYTE alphaArray[16] = { 0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255 };
	BLOCK_DXT3* pBlock = (BLOCK_DXT3*)pSrc;
	int dxtWidth = (width + 3)  / 4;
	int dxtHeight = (height + 3) / 4;

	for ( int yy = 0; yy < dxtHeight; yy++ ) {
		DXT_COLOR *pRowPixel = ((DXT_COLOR *)pDest) + 4 * yy * width;
		for ( int xx = 0; xx < dxtWidth; xx++ ) {
			DXT_COLOR *pLine = pRowPixel;
			WriteBlockColor( pLine, &pBlock->block, width, height );

			for ( int j = 0; j < 4; j++ ) {
				if ( j >= height ) break;
				for ( int i = 0; i < 4; i++ ) {
					if ( i >= width ) break;
					pLine[i].a = alphaArray[(pBlock->a[j] >> (i << 2)) & 0x0F];
				}
				pLine += width;
			}

			pBlock++;
			pRowPixel += 4;
		}
	}
}

void PlotDXT5( BYTE *pDest, BYTE *pSrc, int width, int height )
{
	int dxtWidth = (width + 3) / 4;
	int dxtHeight = (height + 3) / 4;
	BLOCK_DXT5* pBlock = (BLOCK_DXT5*)pSrc;

	for ( int yy = 0; yy < dxtHeight; yy++ ) {
		DXT_COLOR *pRowPixel = ((DXT_COLOR *)pDest) + 4 * yy * width;
		for ( int xx = 0; xx < dxtWidth; xx++ ) {
			WriteBlockColor( pRowPixel, &pBlock->block, width, height );
			PlotDXT5Alpha( pRowPixel, pBlock, width, height );
			pBlock++;
			pRowPixel += 4;
		}
	}
}

void PlotDXT5Alpha( DXT_COLOR *pDest, BLOCK_DXT5 *pSrc, int width, int height )
{
	static BYTE alphaArray[8];

	alphaArray[0] = pSrc->a[0];
	alphaArray[1] = pSrc->a[1];

	// 8-alpha or 6-alpha block?
	if ( alphaArray[0] > alphaArray[1] ) {
		// 8-alpha block:  derive the other six alphas.    
		// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
		alphaArray[2] = (6 * alphaArray[0] + 1 * alphaArray[1] + 3) / 7;	// bit code 010
		alphaArray[3] = (5 * alphaArray[0] + 2 * alphaArray[1] + 3) / 7;	// bit code 011
		alphaArray[4] = (4 * alphaArray[0] + 3 * alphaArray[1] + 3) / 7;	// bit code 100
		alphaArray[5] = (3 * alphaArray[0] + 4 * alphaArray[1] + 3) / 7;	// bit code 101
		alphaArray[6] = (2 * alphaArray[0] + 5 * alphaArray[1] + 3) / 7;	// bit code 110
		alphaArray[7] = (1 * alphaArray[0] + 6 * alphaArray[1] + 3) / 7;	// bit code 111  
	}    
	else {  
		// 6-alpha block.    
		// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
		alphaArray[2] = (4 * alphaArray[0] + 1 * alphaArray[1] + 2) / 5;	// Bit code 010
		alphaArray[3] = (3 * alphaArray[0] + 2 * alphaArray[1] + 2) / 5;	// Bit code 011
		alphaArray[4] = (2 * alphaArray[0] + 3 * alphaArray[1] + 2) / 5;	// Bit code 100
		alphaArray[5] = (1 * alphaArray[0] + 4 * alphaArray[1] + 2) / 5;	// Bit code 101
		alphaArray[6] = 0;													// Bit code 110
		alphaArray[7] = 255;												// Bit code 111
	}

	// 取得ALPHA
	pDest[0].a = alphaArray[pSrc->a[2] & 0x07];
	if ( width > 1 ) {
		pDest[1].a = alphaArray[(pSrc->a[2] >> 3) & 0x07];
		if ( width > 2 ) {
			pDest[2].a = alphaArray[(pSrc->a[2] >> 6) | ((pSrc->a[3] & 0x01) << 2)];
			if ( width > 3 ) pDest[3].a = alphaArray[(pSrc->a[3] >> 1) & 0x07];
		}
	}
	if ( height > 1 ) {
		pDest += width;
		pDest[0].a = alphaArray[(pSrc->a[3] >> 4) & 0x07];
		if ( width > 1 ) {
			pDest[1].a = alphaArray[(pSrc->a[3] >> 7) | ((pSrc->a[4] & 0x03) << 1)];
			if ( width > 2 ) {
				pDest[2].a = alphaArray[(pSrc->a[4] >> 2) & 0x07];
				if ( width > 3 ) pDest[3].a = alphaArray[pSrc->a[4] >> 5];
			}
		}
		if ( height > 2 ) {
			pDest += width;
			pDest[0].a = alphaArray[pSrc->a[5] & 0x07];
			if ( width > 1 ) {
				pDest[1].a = alphaArray[(pSrc->a[5] >> 3) & 0x07];
				if ( width > 2 ) {
					pDest[2].a = alphaArray[(pSrc->a[5] >> 6) | ((pSrc->a[6] & 0x01) << 2)];
					if ( width > 3 ) pDest[3].a = alphaArray[(pSrc->a[6] >> 1) & 0x07];
				}
			}
			if ( height > 3 ) {
				pDest += width;
				pDest[0].a = alphaArray[(pSrc->a[6] >> 4) & 0x07];
				if ( width > 1 ) {
					pDest[1].a = alphaArray[(pSrc->a[6] >> 7) | ((pSrc->a[7] & 0x03) << 1)];
					if ( width > 2 ) {
						pDest[2].a = alphaArray[(pSrc->a[7] >> 2) & 0x07];
						if ( width > 3 ) pDest[3].a = alphaArray[pSrc->a[7] >> 5];
					}
				}
			}
		}
	}
}

BYTE *CompressDXT( BYTE *pImageData, int width, int height, int &compressSize, int &mipmapLevels, nvCompressionOptions *pOptions )
{
	if ( pImageData == NULL ) return NULL;

	int pow_w, pow_h;
	for ( pow_w = 1; pow_w < width; pow_w <<= 1 ) ;
	for ( pow_h = 1; pow_h < height; pow_h <<= 1 ) ;

	nvCompressionOptions options;
	if ( pOptions ) options = *pOptions;

	// 計算轉換為DXT格式後所佔容量
	int size = 0;
	int levels = 1;
	switch ( options.mipMapGeneration )
	{
		case kGenerateMipMaps:					// have mipmap
		{
			levels = GetMipmapCount( pow_w, pow_h );
			if ( levels < 1 ) return NULL;

			if ( options.numMipMapsToWrite <= 0 || options.numMipMapsToWrite > levels ) {
				options.numMipMapsToWrite = levels;
			} else if ( options.numMipMapsToWrite < levels ) {
				levels = options.numMipMapsToWrite;
			}

			int w = max( MIN_UNIT, pow_w );
			int h = max( MIN_UNIT, pow_h );
			for ( int i = 0; i < levels; i++ ) {
				size += w * h;
				if ( w > MIN_UNIT ) w >>= 1;
				if ( h > MIN_UNIT ) h >>= 1;
			}
			break;
		}
		case kNoMipMaps:						// no mipmap
		{
			size = max(MIN_UNIT, pow_w) * max(MIN_UNIT, pow_h);			
			break;
		}
		default:
			return NULL;
	}	

	switch ( options.textureFormat )
	{
		case kDXT1:
		case kDXT1a:
		{
			size /= 2;
			break;
		}
		case kDXT3:
		case kDXT5:
			break;
		default:
			return NULL;
	}

	rgba_t *pTmp = NULL;
	if ( pow_w != width || pow_h != height ) {
		pTmp = new rgba_t[pow_w * pow_h];
		rgba_t *pDest = (rgba_t*)pTmp;
		rgba_t *pSrc = (rgba_t*)pImageData;
		for ( int yy = 0; yy < height; yy++ )
		{
			memcpy( pDest, pSrc, sizeof(rgba_t) * width );
			if ( width < pow_w )
				memset( pDest + width, 0, sizeof(rgba_t) * (pow_w - width) );

			pDest += pow_w;
			pSrc += width;
		}
		if ( height < pow_h )
			memset( pDest, 0, sizeof(rgba_t) * (pow_h - height) * pow_w );

		pImageData = (BYTE*)pTmp;
	}

#ifdef SAVEDDSHEADDER
	g_writeHeaderBool = true;
	size += DXT_HEADER_SIZE;
#else
	g_writeHeaderBool = false;
#endif

	compressSize	= size;
	mipmapLevels	= levels;
	g_writeData		= new BYTE[compressSize];
	g_writeStep		= 0;
	g_writeCount	= 0;

	HRESULT hr;
//	if ( FAILED(hr = nvDXTcompressBGRA(pImageData, pow_w, pow_h, pow_w * 4, &options, 4, 0, WriteDTXnFile)) )
	if ( FAILED(hr = nvDDS::nvDXTcompress(pImageData, pow_w, pow_h, pow_w * 4, nvBGRA, &options, WriteDTXnFile)) )
	{
		if ( pTmp ) delete [] pTmp;
		delete [] g_writeData;
		return NULL;
	}
	if ( pTmp ) delete [] pTmp;
	return g_writeData;
}

BYTE *CompressDXTBlock( BYTE *pImageData, nvTextureFormats textureFormat )
{
	static BYTE tmp[16];
	static nvCompressionOptions options;
	HRESULT hr;

	if ( pImageData == NULL ) {
		return NULL;
	}
	options.textureFormat = textureFormat;
	options.mipMapGeneration = kNoMipMaps;

	g_writeData = tmp;
	g_writeStep = 0;
	g_writeCount = 0;
	g_writeHeaderBool = false;
//	if ( FAILED(hr = nvDXTcompressBGRA(pImageData, 4, 4, 16, &options, 4, NULL, WriteDTXnFile)) )
	if ( FAILED(hr = nvDDS::nvDXTcompress(pImageData, 4, 4, 16, nvBGRA, &options, WriteDTXnFile)) )
	{
		return NULL;
	}

	return tmp;
}

static void DXT1ToDXT3( BLOCK_DXT3 *pDest, BLOCK_DXT1 *pSrc, int size )
{
	for ( int i = 0; i < size; i++ ) {
		pDest[i].block = pSrc[i];			// 複制RGB顏色資訊

		// DXT1可以儲存一Bit當透空色
		if ( pSrc[i].c[0] > pSrc[i].c[1] ) {			// 未有透空色處理
			memset( pDest[i].a, 0xff, sizeof(pDest[i].a) );
		} else {
			DWORD idx = pSrc[i].index;
			memset( pDest[i].a, 0, sizeof(pDest[i].a) );
			
			for ( int yy = 0; yy < 4; yy++ ) {
				pDest->a[yy] = 0;
				for ( int xx = 0; xx < 4; xx++ ) {
					if ( (idx & 0x03) != 0x03 ) pDest->a[yy] |= 0x0f << (4 * xx);
					idx >>= 2;
				}
			}

			/*
			if ( (idx & 0x03) != 0x03 ) pDest[i].a[0] |= 0xf;
			if ( ((idx >> 2 ) & 0x03) != 0x03 ) pDest[i].a[0] |= 0xf0;
			if ( ((idx >> 4 ) & 0x03) != 0x03 ) pDest[i].a[0] |= 0xf00;
			if ( ((idx >> 6 ) & 0x03) != 0x03 ) pDest[i].a[0] |= 0xf000;
			if ( ((idx >> 8 ) & 0x03) != 0x03 ) pDest[i].a[1] |= 0xf;
			if ( ((idx >> 10) & 0x03) != 0x03 ) pDest[i].a[1] |= 0xf0;
			if ( ((idx >> 12) & 0x03) != 0x03 ) pDest[i].a[1] |= 0xf00;
			if ( ((idx >> 14) & 0x03) != 0x03 ) pDest[i].a[1] |= 0xf000;
			if ( ((idx >> 16) & 0x03) != 0x03 ) pDest[i].a[2] |= 0xf;
			if ( ((idx >> 18) & 0x03) != 0x03 ) pDest[i].a[2] |= 0xf0;
			if ( ((idx >> 20) & 0x03) != 0x03 ) pDest[i].a[2] |= 0xf00;
			if ( ((idx >> 22) & 0x03) != 0x03 ) pDest[i].a[2] |= 0xf000;
			if ( ((idx >> 24) & 0x03) != 0x03 ) pDest[i].a[3] |= 0xf;
			if ( ((idx >> 26) & 0x03) != 0x03 ) pDest[i].a[3] |= 0xf0;
			if ( ((idx >> 28) & 0x03) != 0x03 ) pDest[i].a[3] |= 0xf00;
			if ( ((idx >> 30) & 0x03) != 0x03 ) pDest[i].a[3] |= 0xf000;
			*/
		}
	}
}

static void DXT1ToDXT5( BLOCK_DXT5 *pDest, BLOCK_DXT1 *pSrc, int size )
{
	for ( int i = 0; i < size; i++ ) {
		pDest[i].block = pSrc[i];
		pDest[i].a[0] = 0;
		pDest[i].a[1] = 0xFF;

		// DXT1可以儲存一Bit當透空色
		if ( pSrc[i].c[0] > pSrc[i].c[1] ) {			// 未有透空色處理
			memset( pDest[i].a + 2, 0xff, 6 );
		} else {										// 內函一Bit當透空色			
			DWORD idx = pSrc[i].index;
			memset( pDest[i].a + 2, 0, 6 );
			
			// 效率上展開程式效率較高
			int i;
			DWORD *tmp = (DWORD *)(pDest->a + 2);
			for( i = 0; i < 18; i += 3  ) {
				if ( (idx & 0x03) != 0x03 ) *tmp |= 0x07 << i;
				idx >>= 2;
			}
			tmp = (DWORD *)(pDest->a + 2 + 2);
			for( i = 2 ; i < 32 ; i += 3 ) {
				if ( (idx & 0x03) != 0x03 ) *tmp |= 0x07 << i;
				idx >>= 2;
			}

			/*
			if ( (idx & 0x03) != 0x03 ) {
				pDest[i].a[2] |= 0x07;
			}
			if ( ((idx >> 2) & 0x03) != 0x03 ) {
				pDest[i].a[2] |= 0x38;
			}
			if ( ((idx >> 4) & 0x03) != 0x03 ) {
				pDest[i].a[2] |= 0xC0;
				pDest[i].a[3] |= 0x01;
			}
			if ( ((idx >> 6) & 0x03) != 0x03 ) {
				pDest[i].a[3] |= 0x0E;
			}
			if ( ((idx >> 8) & 0x03) != 0x03 ) {
				pDest[i].a[3] |= 0x70;
			}
			if ( ((idx >> 10) & 0x03) != 0x03 ) {
				pDest[i].a[3] |= 0x80;
				pDest[i].a[4] |= 0x03;
			}
			if ( ((idx >> 12) & 0x03) != 0x03 ) {
				pDest[i].a[4] |= 0x1C;
			}
			if ( ((idx >> 14) & 0x03) != 0x03 ) {
				pDest[i].a[4] |= 0xE0;
			}
			if ( ((idx >> 16) & 0x03) != 0x03 ) {
				pDest[i].a[5] |= 0x07;
			}
			if ( ((idx >> 18) & 0x03) != 0x03 ) {
				pDest[i].a[5] |= 0x38;
			}
			if ( ((idx >> 20) & 0x03) != 0x03 ) {
				pDest[i].a[5] |= 0xC0;
				pDest[i].a[6] |= 0x01;
			}
			if ( ((idx >> 22) & 0x03) != 0x03 ) {
				pDest[i].a[6] |= 0x0E;
			}
			if ( ((idx >> 24) & 0x03) != 0x03 ) {
				pDest[i].a[6] |= 0x70;
			}
			if ( ((idx >> 26) & 0x03) != 0x03 ) {
				pDest[i].a[6] |= 0x80;
				pDest[i].a[7] |= 0x03;
			}
			if ( ((idx >> 28) & 0x03) != 0x03 ) {
				pDest[i].a[7] |= 0x1C;
			}
			if ( ((idx >> 30) & 0x03) != 0x03 ) {
				pDest[i].a[7] |= 0xE0;
			}
			*/
		}
	}
}

static void DXT3ToDXT1( BLOCK_DXT1 *pDest, BLOCK_DXT3 *pSrc, int size )
{
	for ( int i = 0; i < size; i++ ) {
		pDest[i] = pSrc[i].block;
	}
}

static void DXT3ToDXT5( BLOCK_DXT5 *pDest, BLOCK_DXT3 *pSrc, int size )
{
	for ( int i = 0; i < size; i++ ) {
		static DXT_COLOR color[16];				
		PlotDXT3( (BYTE*)color, (BYTE*)(pSrc + i), 4, 4 );
		BYTE *pTmp = CompressDXTBlock( (BYTE*)color, kDXT5 );
		memcpy( pDest + i, pTmp, sizeof(BLOCK_DXT5) );
	}
}

static void DXT5ToDXT1( BLOCK_DXT1 *pDest, BLOCK_DXT5 *pSrc, int size )
{
	for ( int i = 0; i < size; i++ ) {
		pDest[i] = pSrc[i].block;
	}
}

static void DXT5ToDXT3( BLOCK_DXT3 *pDest, BLOCK_DXT5 *pSrc, int size )
{
	for ( int i = 0; i < size; i++ ) {
		static DXT_COLOR color[16];
		pDest[i].block = pSrc[i].block;
		PlotDXT5Alpha( color, pSrc + i, 4, 4 );
		pDest[i].a[0] = color[0].a >> 4;
		pDest[i].a[0] |= color[1].a & 0xf0 ;
		pDest[i].a[0] |= (color[2].a & 0xf0) << 4;
		pDest[i].a[0] |= (color[3].a & 0xf0) << 8;
		pDest[i].a[1] = color[4].a >> 4;
		pDest[i].a[1] |= color[5].a & 0xf0 ;
		pDest[i].a[1] |= (color[6].a & 0xf0) << 4;
		pDest[i].a[1] |= (color[7].a & 0xf0) << 8;
		pDest[i].a[2] = color[8].a >> 4;
		pDest[i].a[2] |= color[9].a & 0xf0 ;
		pDest[i].a[2] |= (color[10].a & 0xf0) << 4;
		pDest[i].a[2] |= (color[11].a & 0xf0) << 8;
		pDest[i].a[3] = color[12].a >> 4;
		pDest[i].a[3] |= color[13].a & 0xf0 ;
		pDest[i].a[3] |= (color[14].a & 0xf0) << 4;
		pDest[i].a[3] |= (color[15].a & 0xf0) << 8;
	}
}

bool ConvertDXTFromat( void *pDest, DWORD destFormat, void *pSrc, DWORD srcFormat, int width, int height )
{
	if ( pDest == NULL || pSrc == NULL ) {
		return false;
	}

	// 檢查尺寸是否為2的次方
	if ( !IsPower2(width) || !IsPower2(height) ) {
		return false;
	}

	switch ( srcFormat )
	{
		case kDXT1:
		{
			switch ( destFormat )
			{
				case kDXT1:
					memcpy( pDest, pSrc, (width * height / 2) );
					break;
				case kDXT3:
					DXT1ToDXT3( (BLOCK_DXT3*)pDest, (BLOCK_DXT1*)pSrc, (width * height / 2) / sizeof(BLOCK_DXT1) );
					break;
				case kDXT5:
					DXT1ToDXT5( (BLOCK_DXT5*)pDest, (BLOCK_DXT1*)pSrc, (width * height / 2) / sizeof(BLOCK_DXT1) );
					break;
				default:
					return false;
			}
			break;
		}
		case kDXT3:
		{
			switch ( destFormat )
			{
				case kDXT1:
					DXT3ToDXT1( (BLOCK_DXT1*)pDest, (BLOCK_DXT3*)pSrc, (width * height) / sizeof(BLOCK_DXT3) );
					break;
				case kDXT3:
					memcpy( pDest, pSrc, (width * height / 2) );
					break;
				case kDXT5:
					DXT3ToDXT5( (BLOCK_DXT5*)pDest, (BLOCK_DXT3*)pSrc, (width * height) / sizeof(BLOCK_DXT3) );
					break;
				default:
					return false;
			}
			break;
		}
		case kDXT5:
		{
			switch ( destFormat )
			{
				case kDXT1:
					DXT5ToDXT1( (BLOCK_DXT1*)pDest, (BLOCK_DXT5*)pSrc, (width * height) / sizeof(BLOCK_DXT5) );
					break;
				case kDXT3:
					DXT5ToDXT3( (BLOCK_DXT3*)pDest, (BLOCK_DXT5*)pSrc, (width * height) / sizeof(BLOCK_DXT5) );
					break;
				case kDXT5:
					memcpy( pDest, pSrc, (width * height / 2) );
					break;
				default:
					return false;
			}
			break;
		}
		default:
			return false;
	}
	return true;
}

inline void ModulateDXT1Color( BLOCK_DXT1 &dxt, DXT_COLOR c )
{
	WORD w[2];
	w[0] = R5G6B5XR8G8B8( dxt.c[0], c.r, c.g, c.b );
	w[1] = R5G6B5XR8G8B8( dxt.c[1], c.r, c.g, c.b );
	if ( w[0] == w[1] ) {
		if ( w[0] == 0 ) w[1]++;
		else if ( w[0] & 0x1F ) w[0] -= 1;
		else if ( w[0] & 0x7E0 ) w[0] -= 0x20;
		else w[0] -= 0x800;

		if ( dxt.c[1] > dxt.c[0] ) {
			dxt.c[0] = w[0];
			dxt.c[1] = w[1];
		} else {
			dxt.c[0] = w[1];
			dxt.c[1] = w[0];
		}
	}
	else if ( (dxt.c[0] > dxt.c[1]) == (w[0] > w[1]) ) {
		memcpy( dxt.c, w, sizeof(dxt.c) );
	} else {
		int idx = 0;
		dxt.c[0] = w[1];
		dxt.c[1] = w[0];
		for ( int i = 0 ; i < 32; i += 2 ) {
			switch ( (dxt.index >> i) & 0x03 )
			{
			case 0:		// 0->1
				idx |= 0x01 << i;
				break;
			case 2:		// 2->3
				idx |= 0x03 << i;
				break;
			case 3:		// 3->2
				idx |= 0x02 << i;
				break;
			}
		}
		dxt.index = idx;
	}
}

inline void ModulateDXTBlock( DXT_COLOR *pDest, DXT_COLOR *pSrc )
{	
	for ( int i = 0; i < 16; i++ ) {
		if ( pSrc->a == 0xFF ) {
			*pDest = *pSrc;
		} else if ( pSrc->a > 0 ) {
			pDest->a = max( pDest->a, pSrc->a );
			pDest->r = (BYTE)( ((long)pDest->r * (255 - pSrc->a) + (long)pSrc->r * pSrc->a) / 255);
			pDest->g = (BYTE)( ((long)pDest->g * (255 - pSrc->a) + (long)pSrc->g * pSrc->a) / 255);
			pDest->b = (BYTE)( ((long)pDest->b * (255 - pSrc->a) + (long)pSrc->b * pSrc->a) / 255);
		}
		pDest++;
		pSrc++;
	}
}

static void BltDXT1( BLOCK_DXT1 *pDest, int x, int y, int destWidth, int destHeight, BLOCK_DXT1 *pSrc, int srcWidth, int srcHeight, DXT_COLOR color, DWORD rop )
{	
	BLOCK_DXT1 *pLine = pDest + y * destWidth;
	DXT_COLOR dxtColor[2][16];	
	for ( int yy = 0; yy < srcHeight && (yy + y) < destHeight; yy++ ) {
		if ( rop == DXT_ROP_SELECTSRC && (color.Col & 0xFFFFFF) == 0xFFFFFF ) {
			memcpy( &pLine[x], pSrc, sizeof(BLOCK_DXT1) * srcWidth );
		} else {
			for ( int xx = 0; xx < srcWidth && (xx + x) < destWidth; xx++ ) {
				BLOCK_DXT1 dxt = pSrc[xx];

				// 預乘顏色
				if ( (color.Col & 0xFFFFFF) != 0xFFFFFF ) {
					ModulateDXT1Color( dxt, color );
				}

				switch ( rop )
				{
					case DXT_ROP_OVERLAY:
					{
						if ( dxt.c[0] > dxt.c[1] ) {
							// 沒有1Bits Alpha,可以直接複制
							pLine[xx + x] = dxt;
						} else {
							int i;
							for ( i = 0; i < 32 && ((dxt.index >> i) & 0x03) != 0x03; i+=2 ) ;

							if ( i < 32 ) {		// 最差情況要解開在重壓一次
								BYTE *pTmp;
								PlotDXT1( (BYTE*)&dxtColor[0][0], (BYTE*)&pLine[xx + x], 4, 4 );
								PlotDXT1( (BYTE*)&dxtColor[1][0], (BYTE*)&dxt, 4, 4 );
								ModulateDXTBlock( dxtColor[0], dxtColor[1] );
								if ( (pTmp = CompressDXTBlock( (BYTE*)dxtColor[0], kDXT1a )) != NULL ) {
									memcpy( &pLine[xx + x], pTmp, sizeof(BLOCK_DXT1) );
								}
							} else {
								// 沒有1Bits Alpha,可以直接複制
								pLine[xx + x] = dxt;
							}
						}
						break;
					}
					case DXT_ROP_SELECTSRC:
					{
						pLine[xx + x] = dxt;
						break;
					}
					case DXT_ROP_ADD:
					break;
				}
			}
		}
		pSrc += srcWidth;
		pLine += destWidth;
	}
}

static void BltDXT3( BLOCK_DXT3 *pDest, int x, int y, int destWidth, int destHeight, BLOCK_DXT3 *pSrc, int srcWidth, int srcHeight, DXT_COLOR color, DWORD rop )
{
	BLOCK_DXT3 *pLine = pDest + y * destWidth;
	DXT_COLOR dxtColor[2][16];	
	for ( int yy = 0; yy < srcHeight && (yy + y) < destHeight; yy++ ) {		
		if ( rop == DXT_ROP_SELECTSRC && (color.Col & 0xFFFFFF) == 0xFFFFFF ) {
			memcpy( &pLine[x], pSrc, sizeof(BLOCK_DXT3) * srcWidth );
		} else {
			for ( int xx = 0; xx < srcWidth && (xx + x) < destWidth; xx++ ) {
				BLOCK_DXT3 dxt = pSrc[xx];

				// 預乘顏色
				if ( (color.Col & 0xFFFFFF) != 0xFFFFFF ) {
					ModulateDXT1Color( dxt.block, color );
				}

				switch ( rop )
				{
					case DXT_ROP_OVERLAY:
					{
						static int v[2] = { 0xFFFFFFFF, 0xFFFFFFFF };
						if ( memcmp( dxt.a, v, sizeof(dxt.a) ) == 0 ) {
							pLine[xx + x] = dxt;
						} else {		// 最差情況要解開在重壓一次							
							BYTE *pTmp;
							PlotDXT3( (BYTE*)&dxtColor[0][0], (BYTE*)&pLine[xx + x], 4, 4 );
							PlotDXT3( (BYTE*)&dxtColor[1][0], (BYTE*)&dxt, 4, 4 );
							ModulateDXTBlock( dxtColor[0], dxtColor[1] );
							if ( (pTmp = CompressDXTBlock( (BYTE*)dxtColor[0], kDXT3 )) != NULL ) {
								memcpy( &pLine[xx + x], pTmp, sizeof(BLOCK_DXT3) );
							}
						}

						break;
					}
					case DXT_ROP_SELECTSRC:
					{
						pLine[xx + x] = dxt;
						break;
					}
					case DXT_ROP_ADD:
					break;
				}
			}
		}
		pSrc += srcWidth;
		pLine += destWidth;
	}
}

static void BltDXT5( BLOCK_DXT5 *pDest, int x, int y, int destWidth, int destHeight, BLOCK_DXT5 *pSrc, int srcWidth, int srcHeight, DXT_COLOR color, DWORD rop )
{
	BLOCK_DXT5 *pLine = pDest + y * destWidth;
	DXT_COLOR dxtColor[2][16];
	for ( int yy = 0; yy < srcHeight && (yy + y) < destHeight; yy++ ) {
		if ( rop == DXT_ROP_SELECTSRC && (color.Col & 0xFFFFFF) == 0xFFFFFF ) {
			memcpy( &pLine[x], pSrc, sizeof(BLOCK_DXT5) * srcWidth );
		} else {
			for ( int xx = 0; xx < srcWidth && (xx + x) < destWidth; xx++ ) {
				BLOCK_DXT5 dxt = pSrc[xx];

				// 預乘顏色
				if ( (color.Col & 0xFFFFFF) != 0xFFFFFF ) {
					ModulateDXT1Color( dxt.block, color );
				}

				switch ( rop )
				{
					case DXT_ROP_OVERLAY:
					{
						int i;
						PlotDXT5Alpha( dxtColor[0], &dxt, 4, 4 );
						for ( i = 0; i < 16 && dxtColor[0][i].a > 0xF0; i++ ) ;
						if ( i < 16 ) {		// 最差情況要解開在重壓一次			
							BYTE *pTmp;
							PlotDXT5( (BYTE*)&dxtColor[0][0], (BYTE*)&pLine[xx + x], 4, 4 );
							PlotDXT5( (BYTE*)&dxtColor[1][0], (BYTE*)&dxt, 4, 4 );
							ModulateDXTBlock( dxtColor[0], dxtColor[1] );
							if ( (pTmp = CompressDXTBlock( (BYTE*)dxtColor[0], kDXT5 )) != NULL ) {
								memcpy( &pLine[xx + x], pTmp, sizeof(BLOCK_DXT5) );
							}
						} else {
							pLine[xx + x] = dxt;
						}
						break;
					}
					case DXT_ROP_SELECTSRC:
					{
						pLine[xx + x] = dxt;
						break;
					}
					case DXT_ROP_ADD:
					break;
				}
			}
		}
		pSrc += srcWidth;
		pLine += destWidth;
	}
}

bool BltDXT( void *pDest, DWORD destFormat, int x, int y, int destWidth, int destHeight,
			void *pSrc, DWORD srcFormat, int srcWidth, int srcHeight, DWORD color, DWORD rop )
{	
	BYTE *pRowData = (BYTE*)pSrc;
	bool flagBool = false;
	DXT_COLOR c;

	if ( pDest == NULL || pSrc == NULL ) {
		return false;
	}

	// 若資料格式不相同需要做格式之間轉換
	if ( destFormat != srcFormat ) {		
		flagBool = true;
		switch ( destFormat )
		{
			case kDXT1:
			case kDXT1a:
				pRowData = new BYTE[srcWidth * srcHeight / 2];
				break;
			case kDXT3:
			case kDXT5:
				pRowData = new BYTE[srcWidth * srcHeight];
				break;
			default:
				return false;
		}
		if ( !ConvertDXTFromat( pRowData, destFormat, pSrc, srcFormat, srcWidth, srcHeight ) ) {
			delete [] pRowData;
			return false;
		}
	}	

	c.Col = color;	
	switch ( destFormat )
	{
		case kDXT1:
		case kDXT1a:
		{
			BltDXT1( (BLOCK_DXT1*)pDest, x/4, y/4, destWidth/4, destHeight/4, (BLOCK_DXT1*)pRowData, srcWidth/4, srcHeight/4, c, rop );
			break;
		}
		case kDXT3:
		{
			BltDXT3( (BLOCK_DXT3*)pDest, x/4, y/4, destWidth/4, destHeight/4, (BLOCK_DXT3*)pRowData, srcWidth/4, srcHeight/4, c, rop );
			break;
		}
		case kDXT5:
		{
			BltDXT5( (BLOCK_DXT5*)pDest, x/4, y/4, destWidth/4, destHeight/4, (BLOCK_DXT5*)pRowData, srcWidth/4, srcHeight/4, c, rop );
			break;
		}
		default:
			return false;
	}

	if ( flagBool ) {
		delete [] pRowData;
	}

	return true;
}
