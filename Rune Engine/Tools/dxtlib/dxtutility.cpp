#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <ddraw.h>
#include "dxtutility.h"

#ifdef ENABLE_NVTT

Nvtt_Config::Nvtt_Config()
{
	SetColorMap();
	SetQuality(nvtt::Quality_Fastest);
	EnableCudaAcceleration(false);
}

Nvtt_Config::~Nvtt_Config()
{
}

// Set color to normal map conversion options.
void Nvtt_Config::SetColorToNormalMap()
{
	m_inputOptions.setNormalMap(false);
	m_inputOptions.setConvertToNormalMap(true);
	m_inputOptions.setHeightEvaluation(1.0f/3.0f, 1.0f/3.0f, 1.0f/3.0f, 0.0f);
	//inputOptions.setNormalFilter(1.0f, 0, 0, 0);
	//inputOptions.setNormalFilter(0.0f, 0, 0, 1);
	m_inputOptions.setGamma(1.0f, 1.0f);
	m_inputOptions.setNormalizeMipmaps(true);
}

// Set options for normal maps.
void Nvtt_Config::SetNormalMap()
{
	m_inputOptions.setNormalMap(true);
	m_inputOptions.setConvertToNormalMap(false);
	m_inputOptions.setGamma(1.0f, 1.0f);
	m_inputOptions.setNormalizeMipmaps(true);
}

// Set options for color maps.
void Nvtt_Config::SetColorMap()
{
	m_inputOptions.setNormalMap(false);
	m_inputOptions.setConvertToNormalMap(false);
	m_inputOptions.setGamma(2.2f, 2.2f);
	m_inputOptions.setNormalizeMipmaps(false);
}

// Set mipmaps
void Nvtt_Config::SetMipmapGeneration(bool enabled, int maxlevels)
{
	m_inputOptions.setMipmapGeneration(enabled, maxlevels);
}

// 設定來源資料(需要為32Bits)
void Nvtt_Config::SetSourceData(const void* data, int width, int height)
{
	m_inputOptions.setTextureLayout(nvtt::TextureType_2D, width, height);
	m_inputOptions.setMipmapData(data, width, height);
}

// 設定轉換格式
void Nvtt_Config::SetFormat(nvtt::Format format)
{
	m_compressOptions.setFormat(format);
}

// 設定輸出品質
void Nvtt_Config::SetQuality(nvtt::Quality quality)
{
	m_compressOptions.setQuality(quality);
}

void Nvtt_Config::SetOutputHeader(bool enabled)
{
	m_outputOptions.setOutputHeader(enabled);
}

void Nvtt_Config::SetOutputHandler(nvtt::OutputHandler* outputHandler)
{	
	m_outputOptions.setOutputHandler(outputHandler);
}

void Nvtt_Config::SetErrorHandler(nvtt::ErrorHandler* errorHandler)
{
	m_outputOptions.setErrorHandler(errorHandler);
}

void Nvtt_Config::SetFilename(const char* filename)
{
	m_outputOptions.setFileName(filename);
}

void Nvtt_Config::EnableCudaAcceleration(bool enabled)
{
	m_compressor.enableCudaAcceleration(enabled);
}

bool Nvtt_Config::ExcuteCompress(void* outputData)
{	
	if ( outputData )
	{
		OutputHandlerData outputHandler(outputData);
		SetOutputHandler(&outputHandler);
		return m_compressor.process(m_inputOptions, m_compressOptions, m_outputOptions);
	}

	return m_compressor.process(m_inputOptions, m_compressOptions, m_outputOptions);
}

#endif /*ENABLE_NVTT*/



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
	DXT_COLOR colorArray[4];
	//colorArray[0].Col = Color565To32( pDxt1->c[0] );
	//colorArray[1].Col = Color565To32( pDxt1->c[1] );

	// colorArray[0][1]  5:6:5 RGB
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

bool IsTransparentDXT1( BYTE* pSrc, int width, int height)
{	
	int dxtWidth = (width + 3) / 4;
	int dxtHeight = (height + 3) / 4;
	BLOCK_DXT1* pDxt1 = (BLOCK_DXT1*)pSrc;

	for ( int i = 0; i < dxtWidth * dxtHeight; i++)
	{
		if ( pDxt1->IsTransparent() )
			return true;
		pDxt1++;
	}

	return false;
}

bool CheckTransformDXT5ToDXT1( BYTE* pSrc, int width, int height, bool& isTransparent)
{
	int dxtWidth = (width + 3) / 4;
	int dxtHeight = (height + 3) / 4;
	DXT_ALPHA_BLOCK alphaBlock;
	BLOCK_DXT5* pDxt5 = (BLOCK_DXT5*)pSrc;
	isTransparent = false;

	for ( int i = 0; i < dxtWidth * dxtHeight; i++ )
	{
		pDxt5->GetAlphaBlock(alphaBlock);
		for ( int n = 0; n < 16; n++ )
		{
			if ( alphaBlock.a[n] == 0 )
				isTransparent = true;
			else if ( alphaBlock.a[n] < 255 )
				return false;
		}
		if ( !isTransparent && pDxt5->block.IsTransparent() )
			isTransparent = true;
		pDxt5++;
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
	DXT_ALPHA_BLOCK alphaBlock;

	pSrc->GetAlphaBlock(alphaBlock);

	for ( int y = 0; y < 4; y++ )
	{
		for ( int x = 0; x < 4; x++ )
		{
			pDest[x].a = alphaBlock.a[x];
		}
		pDest += width;
	}	

	/*
	BYTE alphaArray[8];

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
	*/
}

BYTE *CompressDXTBlock( BYTE *pImageData, TextureForamtEnum textureFormat )
{
	static BYTE tmp[16];		

#ifdef ENABLE_NVTT

	if ( pImageData == NULL ) {
		return NULL;
	}

	Nvtt_Config nvttConfig;
	nvttConfig.SetSourceData(pImageData, MIN_UNIT, MIN_UNIT);
	nvttConfig.SetColorMap();
	nvttConfig.SetMipmapGeneration(false);	
	nvttConfig.SetQuality(nvtt::Quality_Fastest);	
	
	switch (textureFormat)
	{
	case TEXTURE_DXT1:
		nvttConfig.SetFormat(nvtt::Format_DXT1);
		break;

	case TEXTURE_DXT1a:
		nvttConfig.SetFormat(nvtt::Format_DXT1a);
		break;

	case TEXTURE_DXT3:
		nvttConfig.SetFormat(nvtt::Format_DXT3);
		break;

	case TEXTURE_DXT5:
		nvttConfig.SetFormat(nvtt::Format_DXT5);
		break;

	default:
		return NULL;
	}

	if ( !nvttConfig.ExcuteCompress(tmp) )
		return NULL;

	return tmp;
#else

	return NULL;

#endif /*ENABLE_NVTT*/	
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
		BYTE *pTmp = CompressDXTBlock( (BYTE*)color, TEXTURE_DXT5 );
		if ( pTmp != NULL )
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
		case TEXTURE_DXT1:
		{
			switch ( destFormat )
			{
				case TEXTURE_DXT1:
					memcpy( pDest, pSrc, (width * height / 2) );
					break;
				case TEXTURE_DXT3:
					DXT1ToDXT3( (BLOCK_DXT3*)pDest, (BLOCK_DXT1*)pSrc, (width * height / 2) / sizeof(BLOCK_DXT1) );
					break;
				case TEXTURE_DXT5:
					DXT1ToDXT5( (BLOCK_DXT5*)pDest, (BLOCK_DXT1*)pSrc, (width * height / 2) / sizeof(BLOCK_DXT1) );
					break;
				default:
					return false;
			}
			break;
		}
		case TEXTURE_DXT3:
		{
			switch ( destFormat )
			{
				case TEXTURE_DXT1:
					DXT3ToDXT1( (BLOCK_DXT1*)pDest, (BLOCK_DXT3*)pSrc, (width * height) / sizeof(BLOCK_DXT3) );
					break;
				case TEXTURE_DXT3:
					memcpy( pDest, pSrc, (width * height / 2) );
					break;
				case TEXTURE_DXT5:
					DXT3ToDXT5( (BLOCK_DXT5*)pDest, (BLOCK_DXT3*)pSrc, (width * height) / sizeof(BLOCK_DXT3) );
					break;
				default:
					return false;
			}
			break;
		}
		case TEXTURE_DXT5:
		{
			switch ( destFormat )
			{
				case TEXTURE_DXT1:
					DXT5ToDXT1( (BLOCK_DXT1*)pDest, (BLOCK_DXT5*)pSrc, (width * height) / sizeof(BLOCK_DXT5) );
					break;
				case TEXTURE_DXT3:
					DXT5ToDXT3( (BLOCK_DXT3*)pDest, (BLOCK_DXT5*)pSrc, (width * height) / sizeof(BLOCK_DXT5) );
					break;
				case TEXTURE_DXT5:
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
								if ( (pTmp = CompressDXTBlock( (BYTE*)dxtColor[0], TEXTURE_DXT1a )) != NULL ) {
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
							if ( (pTmp = CompressDXTBlock( (BYTE*)dxtColor[0], TEXTURE_DXT3 )) != NULL ) {
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
							if ( (pTmp = CompressDXTBlock( (BYTE*)dxtColor[0], TEXTURE_DXT5 )) != NULL ) {
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
			case TEXTURE_DXT1:
			case TEXTURE_DXT1a:
				pRowData = new BYTE[srcWidth * srcHeight / 2];
				break;
			case TEXTURE_DXT3:
			case TEXTURE_DXT5:
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
		case TEXTURE_DXT1:
		case TEXTURE_DXT1a:
		{
			BltDXT1( (BLOCK_DXT1*)pDest, x/4, y/4, destWidth/4, destHeight/4, (BLOCK_DXT1*)pRowData, srcWidth/4, srcHeight/4, c, rop );
			break;
		}
		case TEXTURE_DXT3:
		{
			BltDXT3( (BLOCK_DXT3*)pDest, x/4, y/4, destWidth/4, destHeight/4, (BLOCK_DXT3*)pRowData, srcWidth/4, srcHeight/4, c, rop );
			break;
		}
		case TEXTURE_DXT5:
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
