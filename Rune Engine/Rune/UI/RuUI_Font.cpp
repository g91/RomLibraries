#include "../UI/RuUI_Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define FONT_CANONICAL_SIZE					64
#define FONT_BITMAP_WIDTH					512
#define FONT_BITMAP_HEIGHT					128

#define FONT_TEXTURE_WIDTH					2048
#define FONT_TEXTURE_HEIGHT					1024
#define FONT_TEXTURE_UNIT_SIZE				32

#define FONT_TEXTURE_BLOCK_XSIZE			16
#define FONT_TEXTURE_BLOCK_YSIZE			FONT_TEXTURE_UNIT_SIZE

#pragma managed(push, off)

// --------------------------------------------------------------------------------------
// static member value
// CRuFontType
bool								CRuFontType::s_useFreeType = false;
CRuArrayList<CRuFontType*>			CRuFontType::s_fontType;
vector<RuFontBase*>					CRuFontType::s_fontBase;

HDC									CRuFontType::s_hDC;
HFONT								CRuFontType::s_hCurrentFont;
HBITMAP								CRuFontType::s_hbmBitmap;
DWORD*								CRuFontType::s_pBitmapBits;
CRuImageFX_Resampler_32				CRuFontType::s_imageResampler;

void*								CRuFontType::s_fontLibrary = NULL;

// CRuFontTexture
CRuHashMap<INT64, CRuFontTexture*>	CRuFontTexture::s_fontTexIndex(2048);
CRuStack<CRuFontTexture*>			CRuFontTexture::s_freeFont;
CRuFontTexture*						CRuFontTexture::s_unitTexture;
IRuTexture*							CRuFontTexture::s_pRuTexture;
bool								CRuFontTexture::s_isLockRally;
RuFontTextureCallBack				CRuFontTexture::s_rallyFunc;
DWORD*								CRuFontTexture::s_pPixels;
CRuArrayList<INT64>					CRuFontTexture::s_removedFontTexList;
CRuFontTexture*						CRuFontTexture::s_blockTexture;

IRuTexture*							CRuFontTexture2::s_pRuTexture;
bool								CRuFontTexture2::s_isLockRally;
RuFontTextureCallBack				CRuFontTexture2::s_rallyFunc;
CRuFontTexture2*					CRuFontTexture2::s_blockTexture;
CRuFontTexture2*					CRuFontTexture2::s_useBlock[32][128];
int									CRuFontTexture2::s_blockXSize;
int									CRuFontTexture2::s_blockYSize;
int									CRuFontTexture2::s_rallyYLast;

// CRuFontChar
CRuHashMap<INT64, CRuFontChar*>		CRuFontChar::s_fontChar(4096);

// CRuFontString
map<wstring, CRuFontString*>		CRuFontString::s_fontString;
bool								CRuFontString::s_shutdownLocked = false;

// --------------------------------------------------------------------------------------
void RTLChangeSign(WCHAR& ch)
{
	switch ( ch )
	{
	case L'<':
		ch = L'>';
		break;

	case L'>':
		ch = L'<';
		break;

	case L'(':
		ch = L')';
		break;

	case L')':
		ch = L'(';
		break;

	case L'{':
		ch = L'}';
		break;

	case L'}':
		ch = L'{';
		break;

	case L'[':
		ch = L']';
		break;

	case L']':
		ch = L'[';
		break;
	}
}

// --------------------------------------------------------------------------------------
CRuFontType::CRuFontType()
{
	m_fontBase		= NULL;
	m_serial		= 0;
	m_height		= 0;
	m_fontFace		= NULL;
}

// --------------------------------------------------------------------------------------
CRuFontType::~CRuFontType()
{
	if(m_fontFace)
	{
		FT_Done_Face(reinterpret_cast<FT_Face>(m_fontFace));
	}
}

// --------------------------------------------------------------------------------------
void CRuFontType::Initialize()
{
	if(s_useFreeType)
	{
		// Initialize FreeType
		INT32 error = FT_Init_FreeType(reinterpret_cast<FT_Library *>(&s_fontLibrary));

		if(error)
		{
			OutputDebugString("FreeType init error\n");
		}
	}
	else
	{
		s_hDC			= NULL;
		s_hbmBitmap		= NULL;
		s_hCurrentFont	= NULL;
		s_pBitmapBits	= NULL;

		// Prepare to create a bitmap
		BITMAPINFO bmi;
		ZeroMemory( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );
		bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth       =  FONT_BITMAP_WIDTH;
		bmi.bmiHeader.biHeight      = -FONT_BITMAP_HEIGHT;
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biBitCount    = 32;

		// Create generic device contexts
		s_hDC				= CreateCompatibleDC( NULL );

		// Create a bitmap for the target font size
		s_hbmBitmap			= CreateDIBSection( s_hDC, &bmi, DIB_RGB_COLORS, (VOID**) &s_pBitmapBits, NULL, 0 );	

		// Setup target DC mapping mode and select target font bitmap into it
		SetMapMode(   s_hDC, MM_TEXT );
		SelectObject( s_hDC, s_hbmBitmap );

		// Set text properties
		SetTextColor( s_hDC, RGB(255, 255, 255) );
		SetBkColor(   s_hDC, 0x00000000 );
		SetTextAlign( s_hDC, TA_TOP );	
	}
}

// --------------------------------------------------------------------------------------
void CRuFontType::Shutdown()
{
	// Delete all CRuFontType objects
	for(INT32 i = 0; i < s_fontType.Count(); ++i)
	{
		delete s_fontType[i];
	}
	s_fontType.Clear();

	// Delete all RuFontBase structures
	for(vector<RuFontBase*>::iterator iter = s_fontBase.begin(); iter != s_fontBase.end(); iter++)
	{
		RuFontBase *fontBase = *iter;

		if ( fontBase->hFont )
		{
			DeleteObject(fontBase->hFont);
			fontBase->hFont = NULL;
		}

		ruSAFE_DELETE_ARRAY(fontBase->fontStr);
		ruSAFE_DELETE(fontBase->m_fontStream);
		ruSAFE_DELETE(fontBase);
	}
	s_fontBase.clear();

	// Release GDI objects
	s_hCurrentFont	= NULL;
	s_pBitmapBits	= NULL;

	if ( s_hbmBitmap )
	{
		DeleteObject( s_hbmBitmap );
		s_hbmBitmap = NULL;
	}	

	if ( s_hDC )
	{
		DeleteDC( s_hDC );
		s_hDC = NULL;
	}

	// Shutdown FreeType
	if ( s_fontLibrary )
	{
		FT_Done_FreeType(reinterpret_cast<FT_Library>(s_fontLibrary));
		s_fontLibrary = NULL;
	}
}

// --------------------------------------------------------------------------------------
int CALLBACK EnumFontProc( ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam )
{
	int a = 0;
	return 0;
}

// --------------------------------------------------------------------------------------
CRuFontType* CRuFontType::GetFontType(const char *fontNameStr, int height, RuFontWeight weight, RuFontOutline outline)
{
	static int lastSerial = 0x40;

	// Filter out invalid fonts
	if (fontNameStr == NULL || height < 6)
		return NULL;

	// Search for existing, matching font type
	for(INT32 i = 0; i < s_fontType.Count(); ++i)
	{
		RuFontBase *fontBase = s_fontType[i]->m_fontBase;
		if ( s_fontType[i]->m_height == height && stricmp(fontBase->fontStr, fontNameStr) == 0 && fontBase->outline == outline && fontBase->weight == weight )
		{
			return s_fontType[i];
		}
	}

	// Find an existing FontBase structure with the specified font name
	RuFontBase *fontBase = NULL;
	for(vector<RuFontBase*>::iterator iter = s_fontBase.begin(); iter != s_fontBase.end(); iter++)
	{
		RuFontBase* baseTmp = *iter;
		if ( stricmp(baseTmp->fontStr, fontNameStr) == 0 && baseTmp->weight == weight && baseTmp->outline == outline )
		{
			fontBase = baseTmp;
			break;
		}
	}	

	if(fontBase == NULL)
	{
		// Create new RuFontBase structure	
		fontBase					= ruNEW RuFontBase;
		fontBase->index				= (int)s_fontBase.size();
		fontBase->fontStr			= ruNEW char[strlen(fontNameStr) + 1];
		strcpy(fontBase->fontStr, fontNameStr);
		fontBase->outline			= outline;
		fontBase->weight			= weight;
		fontBase->hFont				= NULL;
		fontBase->m_fontStream		= NULL;

		if(s_useFreeType)
		{
			// Freetype2
			IRuStream *fileStream = g_ruResourceManager->LoadStream(fontNameStr);

			if(fileStream == NULL)
			{
				fileStream = g_ruResourceManager->LoadStream("fonts\\dfheimdu.ttf");
			}

			if(fileStream)
			{
				fontBase->m_fontStream = ruNEW CRuMemoryStream();
				fontBase->m_fontStream->SetBufferSize(fileStream->GetStreamSize());
				*(fontBase->m_fontStream) = *fileStream;
				ruSAFE_DELETE(fileStream);
			}
			else
			{
				fontBase->m_fontStream = NULL;
			}
		}
		else
		{
			DWORD fw;
			switch ( weight )
			{
				case ruFONTWEIGHT_THIN:
					fw = FW_EXTRALIGHT;
					break;
				case ruFONTWEIGHT_NORMAL:
					fw = FW_SEMIBOLD;
					break;
				case ruFONTWEIGHT_BOLD:
					fw = FW_HEAVY;
					break;
			}

			wchar_t* wfontName = MultiByteToWChar(fontNameStr);
			
			// Create New Font
			int nHeight = FONT_CANONICAL_SIZE;

			fontBase->hFont = CreateFontW(nHeight, 0, 0, 0, fw, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, wfontName );
			ruSAFE_DELETE_ARRAY(wfontName);

			if ( fontBase->hFont == NULL )
			{
				ruSAFE_DELETE(fontBase);
				return NULL;
			}

			s_hCurrentFont = fontBase->hFont;
			SelectObject(s_hDC, s_hCurrentFont);

			TEXTMETRIC textMetric;
			GetTextMetrics(s_hDC, &textMetric);
		}

		s_fontBase.push_back(fontBase);
	}

	// Create CRuFontType object
	CRuFontType *fontType	= ruNEW CRuFontType;
	fontType->m_height		= height;
	fontType->m_fontBase	= fontBase;
	fontType->m_serial		= lastSerial++;

	if(s_useFreeType)
	{
		if(fontBase->m_fontStream)
		{
			// Create new font face based on the input font stream and the given font height
			FT_New_Memory_Face(reinterpret_cast<FT_Library>(s_fontLibrary), fontBase->m_fontStream->GetBuffer(), fontBase->m_fontStream->GetStreamSize(), 0, reinterpret_cast<FT_Face *>(&fontType->m_fontFace));
		}

		// Set transform based on weight
		switch(weight)
		{
			case ruFONTWEIGHT_THIN:
				{
	/*				FT_Matrix fontTransform;
					fontTransform.xx = (FT_Fixed) (0.95f * 0x10000L);
					fontTransform.xy = (FT_Fixed) (0.0f * 0x10000L);
					fontTransform.yx = (FT_Fixed) (0.0f * 0x10000L);
					fontTransform.yy = (FT_Fixed) (1.0f * 0x10000L);
					FT_Set_Transform(reinterpret_cast<FT_Face>(fontType->m_fontFace), &fontTransform, NULL);*/
				}

				break;

			case ruFONTWEIGHT_BOLD:
				{
	/*				FT_Matrix fontTransform;
					fontTransform.xx = (FT_Fixed) (2.5f * 0x10000L);
					fontTransform.xy = (FT_Fixed) (0.0f * 0x10000L);
					fontTransform.yx = (FT_Fixed) (0.0f * 0x10000L);
					fontTransform.yy = (FT_Fixed) (1.0f * 0x10000L);
					FT_Set_Transform(reinterpret_cast<FT_Face>(fontType->m_fontFace), &fontTransform, NULL);*/
				}

				break;

			default:
			case ruFONTWEIGHT_NORMAL:
				// Do nothing
				break;
		}
	}

	// Add font type to list of font types
	s_fontType.Add(fontType);

	return fontType;
}

// --------------------------------------------------------------------------------------
void CRuFontType::PaintToMem(const WCHAR wchar, const WCHAR* toneSymbol, IRuTexture *dstTexture, INT32 x, INT32 y, SIZE &size)
{
	if(s_useFreeType)
	{
		FT_Face fontFace = reinterpret_cast<FT_Face>(m_fontFace);

		INT32 levels = dstTexture->GetLevelCount();

		for(INT32 curLevel = 0, levelUnitSize = FONT_TEXTURE_UNIT_SIZE; curLevel < levels; ++curLevel, levelUnitSize = levelUnitSize >> 1)
		{
			levelUnitSize -= 1;

			// Find a pixel size that will fit
			INT32 error;
			INT32 deduction = 0;
			INT32 height;
			do 
			{
				error = FT_Set_Pixel_Sizes(reinterpret_cast<FT_Face>(m_fontFace), 0, levelUnitSize - deduction);

				height = (fontFace->size->metrics.height >> 6);

				++deduction;
			} while (height > levelUnitSize);


			// Get character glyph
			INT32 glyphIdx = FT_Get_Char_Index(reinterpret_cast<FT_Face>(m_fontFace), wchar);

			// Load and render glyph
			error = FT_Load_Glyph(reinterpret_cast<FT_Face>(m_fontFace), glyphIdx, FT_LOAD_TARGET_LIGHT);
			error = FT_Render_Glyph(reinterpret_cast<FT_Face>(m_fontFace)->glyph, FT_RENDER_MODE_LIGHT);

			// Shortcuts
			FT_GlyphSlot glyphSlot = reinterpret_cast<FT_Face>(m_fontFace)->glyph;
			FT_Bitmap glyphBitmap = reinterpret_cast<FT_Face>(m_fontFace)->glyph->bitmap;

			if(curLevel == 0)
			{
				// Font size is the advance of the glyph as width and the entire height of the bitmap window as height
				size.cx = glyphSlot->advance.x >> 6;
				size.cy = levelUnitSize;
			}

			BYTE *dstBits, *srcBits;
			INT32 dstPitch, srcPitch;

			dstTexture->Lock(curLevel, (void **) &dstBits, &dstPitch);
			srcBits = glyphBitmap.buffer;
			srcPitch = glyphBitmap.pitch;

			dstBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(dstBits) + (x >> curLevel) * sizeof(BYTE) + (y >> curLevel) * dstPitch);

			// Clear the entire field
			BYTE *orgDstBits = dstBits;
			for ( int h = 0; h < levelUnitSize; h++ )
			{
				for (int w = 0; w < levelUnitSize; w++)
				{
					dstBits[w] = 0x00;
				}

				dstBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(dstBits) + dstPitch);
			}

			dstBits = orgDstBits;

			// Determine height placement - ascender minus horizontal bearing is the number of empty rows at the top of the glyph
			int rowsToSkip = (glyphSlot->face->size->metrics.ascender - glyphSlot->metrics.horiBearingY) >> 6;

			// Offset destination pointer so we start at the correct pixel
			if((glyphSlot->bitmap_left + glyphBitmap.width) > (glyphSlot->advance.x >> 6))
			{
				// This detects situations where the font is passing back bad metrics... where the advance is smaller than the damn glyph
				dstBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(orgDstBits) + dstPitch * rowsToSkip);
			}
			else
			{
				dstBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(orgDstBits) + dstPitch * rowsToSkip + glyphSlot->bitmap_left);
			}

			// Copy the glyph
			for(int h = 0; h < glyphBitmap.rows; ++h)
			{
				for(int w = 0; w < glyphBitmap.width; ++w)
				{
					dstBits[w] = srcBits[w];
				}

				srcBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(srcBits) + srcPitch);
				dstBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(dstBits) + dstPitch);
			}

			if(m_fontBase->weight == ruFONTWEIGHT_BOLD)
			{
				// Offset destination pointer so we start at the correct pixel
				if((glyphSlot->bitmap_left + glyphBitmap.width) > (glyphSlot->advance.x >> 6))
				{
					// This detects situations where the font is passing back bad metrics... where the advance is smaller than the damn glyph
					dstBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(orgDstBits) + dstPitch * rowsToSkip + 1);
				}
				else
				{
					dstBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(orgDstBits) + dstPitch * rowsToSkip + glyphSlot->bitmap_left + 1);
				}

				srcBits = glyphBitmap.buffer;

				// Copy the glyph
				for(int h = 0; h < glyphBitmap.rows; ++h)
				{
					for(int w = 0; w < glyphBitmap.width; ++w)
					{
						dstBits[w] = min(255, dstBits[w] + srcBits[w]);
					}

					srcBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(srcBits) + srcPitch);
					dstBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(dstBits) + dstPitch);
				}
			}

			dstTexture->Unlock(curLevel);

			levelUnitSize += 1;
		}
	}
	else
	{
		static DWORD outTemp[65536];
		assert(s_hDC);

		if ( s_hCurrentFont != m_fontBase->hFont )
		{
			s_hCurrentFont = m_fontBase->hFont;
			SelectObject( s_hDC, s_hCurrentFont );
		}

		//
		memset(s_pBitmapBits, 0, sizeof(DWORD)*FONT_BITMAP_WIDTH*FONT_BITMAP_HEIGHT);

		SIZE srcSize;
		BOOL getTextExtentResult;

		if ( toneSymbol == NULL )
		{
			// Output source font to DC
			if ( !ExtTextOutW(s_hDC, 0, 0, ETO_OPAQUE, NULL, &wchar, 1, NULL) )
				return;

			// Get text extents for target font
			getTextExtentResult = GetTextExtentPoint32W(s_hDC, &wchar, 1, &srcSize);			
		}
		else
		{
			WCHAR buf[256];
			swprintf_s(buf, 256, L"%c%s", wchar, toneSymbol);

			int length = (int)wcslen(buf);

			// Output source font to DC
			if ( !ExtTextOutW(s_hDC, 0, 0, ETO_OPAQUE, NULL, buf, length, NULL) )
				return;

			// Get text extents for target font
			getTextExtentResult = GetTextExtentPoint32W(s_hDC, buf, length, &srcSize);
		}

		assert(getTextExtentResult);

		// 檢查繪出字型大小
		if ( srcSize.cx < 0 || srcSize.cx > 128 || srcSize.cy < 0 || srcSize.cy > 128 )
			return;

		if ( srcSize.cx > srcSize.cy )
		{
			size.cx = FONT_TEXTURE_UNIT_SIZE - 1;
			size.cy = size.cx * srcSize.cy / srcSize.cx;
		}
		else
		{
			size.cy = FONT_TEXTURE_UNIT_SIZE - 1;
			size.cx = size.cy * srcSize.cx / srcSize.cy;
		}

		INT32 levels = dstTexture->GetLevelCount();

		for(INT32 curLevel = 0, levelUnitSize = FONT_TEXTURE_UNIT_SIZE; curLevel < levels; ++curLevel, levelUnitSize = levelUnitSize >> 1)
		{
			levelUnitSize -= 1;

			// Clear output buffer
			memset(outTemp, 0, sizeof(DWORD) * 65536);
			
			// Re-sample source font into target font
			s_imageResampler.ResampleImage(s_pBitmapBits, srcSize.cx, srcSize.cy, FONT_BITMAP_WIDTH, outTemp, size.cx >> curLevel, size.cy >> curLevel, levelUnitSize);

			BYTE *dstBits;
			INT32 dstPitch;
			dstTexture->Lock(curLevel, (void **) &dstBits, &dstPitch);

			dstBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(dstBits) + (x >> curLevel) * sizeof(BYTE) + (y >> curLevel) * dstPitch);

			DWORD *pSrc = outTemp;
			for ( int h = 0; h < levelUnitSize ; h++ )
			{
				for (int w = 0; w < levelUnitSize; w++)
				{
					if (pSrc[w] & 0xFF)
						dstBits[w] = static_cast<BYTE>(pSrc[w] & 0xFF);
					else
						dstBits[w] = 0;
				}

				pSrc = reinterpret_cast<DWORD *>(reinterpret_cast<BYTE *>(pSrc) + levelUnitSize * sizeof(DWORD));
				dstBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(dstBits) + dstPitch);
			}

			dstTexture->Unlock(curLevel);

			levelUnitSize += 1;
		}
	}
}

// --------------------------------------------------------------------------------------
void CRuFontType::PaintToMem(WCHAR* text, int length, IRuTexture *dstTexture, INT32 x, INT32 y, SIZE &size)
{
	static DWORD outTemp[65536];
	assert(s_hDC);

	if ( s_hCurrentFont != m_fontBase->hFont )
	{
		s_hCurrentFont = m_fontBase->hFont;
		SelectObject( s_hDC, s_hCurrentFont );
	}

	//
	memset(s_pBitmapBits, 0, sizeof(DWORD)*FONT_BITMAP_WIDTH*FONT_BITMAP_HEIGHT);

	SIZE paintSize;
	BOOL getTextExtentResult;

	// Output source font to DC
	if ( !ExtTextOutW(s_hDC, 0, 0, ETO_OPAQUE, NULL, text, length, NULL) )
		return;

	// Get text extents for target font
	getTextExtentResult = GetTextExtentPoint32W(s_hDC, text, length, &paintSize);

	assert(getTextExtentResult);

	// 檢查繪出字型大小
	if ( paintSize.cx <= 0 || paintSize.cy <= 0 )
		return;

	size.cy = FONT_TEXTURE_UNIT_SIZE - 1;
	size.cx = size.cy * paintSize.cx / paintSize.cy;

	INT32 unitWidth = (size.cx + FONT_TEXTURE_BLOCK_XSIZE - 1) / FONT_TEXTURE_BLOCK_XSIZE * FONT_TEXTURE_BLOCK_XSIZE;
	//INT32 unitWidth = size.cx;
	INT32 unitHeight = FONT_TEXTURE_UNIT_SIZE;
	INT32 levels = dstTexture->GetLevelCount();

	for(INT32 curLevel = 0, levelUnitSize = FONT_TEXTURE_UNIT_SIZE; curLevel < levels; ++curLevel, levelUnitSize = levelUnitSize >> 1, unitWidth >>= 1, unitHeight >>= 1)
	{
		levelUnitSize -= 1;

		// Clear output buffer
		memset(outTemp, 0, sizeof(DWORD) * 65536);

		// Re-sample source font into target font
		s_imageResampler.ResampleImage(s_pBitmapBits, paintSize.cx, paintSize.cy, FONT_BITMAP_WIDTH, outTemp, size.cx >> curLevel, size.cy >> curLevel, unitWidth);

		BYTE *dstBits;
		INT32 dstPitch;
		dstTexture->Lock(curLevel, (void **) &dstBits, &dstPitch);

		dstBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(dstBits) + (x >> curLevel) * sizeof(BYTE) + (y >> curLevel) * dstPitch);

		DWORD *pSrc = outTemp;
		for ( int h = 0; h < unitHeight ; h++ )
		{
			for (int w = 0; w < unitWidth; w++)
			{
				if (pSrc[w] & 0xFF)
					dstBits[w] = static_cast<BYTE>(pSrc[w] & 0xFF);
				else
					dstBits[w] = 0;
			}

			pSrc = reinterpret_cast<DWORD *>(reinterpret_cast<BYTE *>(pSrc) + unitWidth * sizeof(DWORD));
			dstBits = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(dstBits) + dstPitch);
		}

		dstTexture->Unlock(curLevel);

		levelUnitSize += 1;
	}
}

// --------------------------------------------------------------------------------------
void CRuFontType::GetFontSize(const WCHAR wchar, WCHAR* toneSymbol, SIZE& outSize)
{
	if(s_useFreeType)
	{
		FT_Face fontFace = reinterpret_cast<FT_Face>(m_fontFace);

		// Find a pixel size that will fit
		INT32 levelUnitSize = FONT_TEXTURE_UNIT_SIZE - 1;
		INT32 error;
		INT32 deduction = 0;
		INT32 height;
		do 
		{
			error = FT_Set_Pixel_Sizes(reinterpret_cast<FT_Face>(m_fontFace), 0, levelUnitSize - deduction);

			height = (fontFace->size->metrics.height >> 6);

			++deduction;
		} while (height > levelUnitSize);

		// Get character glyph
		INT32 glyphIdx = FT_Get_Char_Index(reinterpret_cast<FT_Face>(m_fontFace), wchar);

		// Load and render glyph
		error = FT_Load_Glyph(reinterpret_cast<FT_Face>(m_fontFace), glyphIdx, FT_LOAD_DEFAULT);
		error = FT_Render_Glyph(reinterpret_cast<FT_Face>(m_fontFace)->glyph, FT_RENDER_MODE_NORMAL);

		// Shortcuts
		FT_GlyphSlot glyphSlot = reinterpret_cast<FT_Face>(m_fontFace)->glyph;
		FT_Bitmap glyphBitmap = reinterpret_cast<FT_Face>(m_fontFace)->glyph->bitmap;

		// Font size is the advance of the glyph as width and the entire height of the bitmap window as height
		outSize.cx = glyphSlot->advance.x >> 6;
		outSize.cy = levelUnitSize;
	}
	else
	{
		if ( s_hCurrentFont != m_fontBase->hFont )
		{
			s_hCurrentFont = m_fontBase->hFont;
			SelectObject(s_hDC, s_hCurrentFont);
		}

		if ( toneSymbol == NULL )
		{
			// Get text extents for target font
			GetTextExtentPoint32W(s_hDC, &wchar, 1, &outSize);
		}
		else
		{
			WCHAR buf[256];
			swprintf_s(buf, 256, L"%c%s", wchar, toneSymbol);
			GetTextExtentPoint32W(s_hDC, buf, (int)wcslen(buf), &outSize);
		}		
	}
}

// --------------------------------------------------------------------------------------
void CRuFontType::GetFontSize(const WCHAR* text, int length, SIZE& outSize)
{
	if ( s_hCurrentFont != m_fontBase->hFont )
	{
		s_hCurrentFont = m_fontBase->hFont;
		SelectObject(s_hDC, s_hCurrentFont);
	}

	GetTextExtentPoint32W(s_hDC, text, length, &outSize);
}

// --------------------------------------------------------------------------------------
CRuFontTexture::CRuFontTexture()
{
	m_rect.Set(0.0f, 0.0f, 0.0f, 0.0f);

	m_toneSymbol = NULL;
}

// --------------------------------------------------------------------------------------
CRuFontTexture::~CRuFontTexture()
{
	if ( m_toneSymbol )
	{
		delete [] m_toneSymbol;
		m_toneSymbol = NULL;
	}
}

// --------------------------------------------------------------------------------------
bool CRuFontTexture::Initialize( RuFontTextureCallBack func )
{
	int size, row, col;
	s_fontTexIndex.Clear();	
	s_freeFont.Clear();
	s_isLockRally	= false;
	s_pRuTexture	= NULL;
	s_rallyFunc		= NULL;

	if ( (s_pRuTexture = g_ruUIRenderer->GetReservedDXT5Texture()) == NULL  )
		return false;
		
	// 計算貼圖總存放字元個數
	row = FONT_TEXTURE_HEIGHT / FONT_TEXTURE_UNIT_SIZE;
	col	= FONT_TEXTURE_WIDTH / FONT_TEXTURE_UNIT_SIZE;
	size = row * col;
	s_unitTexture = ruNEW CRuFontTexture[size];
	CRuFontTexture* fontTexture;
	for ( int i = 0; i < size; i++ )
	{
		fontTexture = s_unitTexture + i;
		fontTexture->m_rect.m_left		= (float)((i % col) * FONT_TEXTURE_UNIT_SIZE);
		fontTexture->m_rect.m_top		= (float)((i / col) * FONT_TEXTURE_UNIT_SIZE);
		fontTexture->m_rect.m_right		= fontTexture->m_rect.m_left;
		fontTexture->m_rect.m_bottom	= fontTexture->m_rect.m_top;

		// 第一個區塊特殊用途,當文字背景區塊用
		if ( i == 0 )
		{
			// 填成純白資料
			BYTE *pRow = NULL;
			INT32 pitch;
			INT32 levels = s_pRuTexture->GetLevelCount();
			for(INT32 curLevel = 0, levelUnitSize = FONT_TEXTURE_UNIT_SIZE; curLevel < levels; ++curLevel, levelUnitSize = levelUnitSize >> 1)
			{
				if ( s_pRuTexture->Lock(curLevel, (void**)&pRow, &pitch) )
				{
					for ( int h = 0; h < levelUnitSize; h++ )
					{
						memset(pRow, 0xFF, sizeof(BYTE) * levelUnitSize);
						pRow += pitch;
					}
					s_pRuTexture->Unlock(curLevel);
				}
			}

			fontTexture->m_rect.m_right		+= FONT_TEXTURE_UNIT_SIZE;
			fontTexture->m_rect.m_bottom	+= FONT_TEXTURE_UNIT_SIZE;
			s_blockTexture = fontTexture;
		}
		else
		{
			s_freeFont.Push(fontTexture);
		}
	}

	s_pPixels = ruNEW DWORD[FONT_TEXTURE_UNIT_SIZE * FONT_TEXTURE_UNIT_SIZE * 2];
	s_rallyFunc = func;
	return true;
}

// --------------------------------------------------------------------------------------
void CRuFontTexture::Shutdown()
{
	ReleaseAllFontIndex();

	s_rallyFunc = NULL;
	s_fontTexIndex.Clear();	
	s_freeFont.Clear();
	
	if ( s_unitTexture )
	{
		delete [] s_unitTexture;
		s_unitTexture = NULL;
	}

	if ( s_pPixels )
	{
		delete[] s_pPixels;
		s_pPixels = NULL;
	}
}

// --------------------------------------------------------------------------------------
void CRuFontTexture::ReleaseAllFontIndex()
{
	Rally(true);
}

// --------------------------------------------------------------------------------------
CRuFontTexture*	CRuFontTexture::GetRuFontTexture(CRuFontType* fontType, WCHAR wchar, WCHAR* toneSymbol)
{
	CRuFontTexture* fontTexture = NULL;
	if ( fontType && wchar >= 0x20 )
	{
		INT64 toneSymbolNumber = CRuFontChar::GetToneSymbolNumber(toneSymbol);
		INT64 key = toneSymbolNumber << 32 | (fontType->GetFontBaseIndex() & 0xFFFF) << 16 | wchar;
		DWORD keyHash = RuGlobalHashGen().GetHashCode(key);		
		if ( !s_fontTexIndex.Get(key, keyHash, fontTexture) )
		{
			fontTexture = GetFreeFontTexture();
			if ( fontTexture )
			{
				// Paint character to texture
				SIZE fontSize;
				fontType->PaintToMem(wchar, toneSymbol, s_pRuTexture, static_cast<INT32>(fontTexture->m_rect.m_left), static_cast<INT32>(fontTexture->m_rect.m_top), fontSize);

				// 設定聲調符號
				fontTexture->SetToneSymbol(toneSymbol);

				// Set texture size according to the size structure returned by PaintToMem
				fontTexture->m_rect.m_right = fontTexture->m_rect.m_left + fontSize.cx;
				fontTexture->m_rect.m_bottom = fontTexture->m_rect.m_top + fontSize.cy;

				// 設定資料已被取用
				s_fontTexIndex.Add(key, keyHash, fontTexture);
			}
		}
	}

	return fontTexture;
}

// --------------------------------------------------------------------------------------
void CRuFontTexture::PaintBegin()
{		
	g_ruUIRenderer->SetTexture( s_pRuTexture, FALSE );	
	g_ruUIRenderer->SetEffectType( ru2DFXTYPE_TEXTBLIT_FILTER );
	g_ruUIRenderer->SetEffectParameter( ru2DFXPARAM_ROTATE, ru2DROTATE_NONE );
}

// --------------------------------------------------------------------------------------
void CRuFontTexture::Clear()
{
	if ( m_toneSymbol )
	{
		delete [] m_toneSymbol;
		m_toneSymbol = NULL;
	}
}

// --------------------------------------------------------------------------------------
void CRuFontTexture::SetToneSymbol(const WCHAR* toneSymbol)
{
	if ( m_toneSymbol )
	{
		delete [] m_toneSymbol;
		m_toneSymbol = NULL;
	}

	if ( toneSymbol )
	{
		m_toneSymbol = ruNEW WCHAR[wcslen(toneSymbol) + 1];
		wcscpy(m_toneSymbol, toneSymbol);
	}
}

// --------------------------------------------------------------------------------------
CRuFontTexture* CRuFontTexture::GetFreeFontTexture()
{	
	if ( s_freeFont.IsEmpty() )
		Rally();

	if ( s_freeFont.IsEmpty() == FALSE )
		return s_freeFont.Pop();

	return NULL;
}

// --------------------------------------------------------------------------------------
bool CRuFontTexture::Recycle( CRuFontTexture* pTexture )
{
	/*
	if ( pTexture->m_pFontChar )
	{
		pTexture->m_pFontChar->m_pFontTex = NULL;
		pTexture->m_pFontChar = NULL;
	}
	*/

	if ( pTexture == NULL || pTexture == s_blockTexture )
		return false;	

	pTexture->Clear();

	s_freeFont.Push(pTexture);
	return true;
}

// --------------------------------------------------------------------------------------
// 清除元索引值
void CRuFontTexture::Rally(bool removeAll)
{
	if ( !s_isLockRally )
	{
		s_isLockRally = true;

		int val = rand() % 3;
		if ( removeAll )
			val = -1;

		s_removedFontTexList.Clear();

		s_fontTexIndex.ForAllElements(RallyCallback, reinterpret_cast<void *>(&val));

		for(INT32 i = 0; i < s_removedFontTexList.Count(); ++i)
		{
			INT64 key = s_removedFontTexList[i];
			DWORD keyHash = RuGlobalHashGen().GetHashCode(key);
			s_fontTexIndex.Remove(key, keyHash);
		}

		if ( s_rallyFunc )
			s_rallyFunc();

		s_isLockRally = false;
	}
}

// --------------------------------------------------------------------------------------
BOOL CRuFontTexture::RallyCallback(const void *key, const void *data, void *userData)
{
	CRuFontTexture *fontTexture = *((CRuFontTexture **) data);

	int &val = *reinterpret_cast<int *>(userData);

	// 移除全部索引值
	if ( val < 0 )
	{
		if ( Recycle( fontTexture ) )
			s_removedFontTexList.Add(*((INT64 *) key));
	}
	// 釋放1/3索引值
	else if ( val > 1 )
	{
		val = 0;
		if ( Recycle( fontTexture ) )
			s_removedFontTexList.Add(*((INT64 *) key));
	}
	else
		val++;

	return TRUE;
}

// --------------------------------------------------------------------------------------
CRuFontChar::CRuFontChar()
{
	m_pFontType			= NULL;
	m_char				= 0;
	m_x					= 0;
	m_y					= 0;
	m_toneSymbol		= NULL;
}

// --------------------------------------------------------------------------------------
CRuFontChar::~CRuFontChar()
{
	if ( m_toneSymbol )
	{
		delete [] m_toneSymbol;
		m_toneSymbol = NULL;
	}
}

// --------------------------------------------------------------------------------------
void CRuFontChar::Initialize( RuFontTextureCallBack func )
{
	CRuFontType::Initialize();
	CRuFontTexture::Initialize( func );

	s_fontChar.Clear();
}

// --------------------------------------------------------------------------------------
void CRuFontChar::Shutdown()
{
	s_fontChar.Clear();

	CRuFontTexture::Shutdown();
	CRuFontType::Shutdown();	
}

// --------------------------------------------------------------------------------------
int CRuFontChar::GetToneSymbolNumber( WCHAR *toneSymbol )
{
	if ( toneSymbol == NULL )
		return 0;

	int toneNumber = 0;
	for ( int i = 0; i < 4 && toneSymbol[i] != 0; i++ )
	{
		// 泰文
		if ( toneSymbol[i] >= 0x0E00 && toneSymbol[i] <= 0x0E7F )
			toneNumber += toneSymbol[i] - 0x0E00;
	}

	return toneNumber;
}

// --------------------------------------------------------------------------------------
// 取得字元結構
// 參數 : pFontType 字型類型
// 參數 : wchar 顯示單字元
// 參數 : toneSymbol 聲調字元串需要零結尾(泰文會有聲調符號)
CRuFontChar* CRuFontChar::GetFontChar( CRuFontType *pFontType, WCHAR wchar, WCHAR *toneSymbol )
{
	assert( pFontType );

	if ( toneSymbol && toneSymbol[0] == 0 )
		toneSymbol = NULL;

	INT64 toneSymbolNumber = CRuFontChar::GetToneSymbolNumber(toneSymbol);
	INT64 key = toneSymbolNumber << 32 | (pFontType->GetSerial() & 0xFFFF) << 16 | wchar;
	DWORD keyHash = RuGlobalHashGen().GetHashCode(key);
	CRuFontChar *fontChar = NULL;
	if(s_fontChar.Get(key, keyHash, fontChar))
	{
		return fontChar;
	}	

	fontChar = ruNEW CRuFontChar();
	fontChar->m_char = wchar;

	// 是否有聲調符號
	if ( toneSymbol != NULL )
	{
		fontChar->m_toneSymbol = ruNEW WCHAR[wcslen(toneSymbol) + 1];
		wcscpy(fontChar->m_toneSymbol, toneSymbol);
	}
	else
	{
		fontChar->m_toneSymbol = NULL;
	}

	// 取得字型寬高
	SIZE fontSize;
	pFontType->GetFontSize(wchar, toneSymbol, fontSize);

	fontChar->m_pFontType		= pFontType;
	fontChar->m_y				= (float)pFontType->GetHeight();
	fontChar->m_x				= (float)fontChar->m_y * fontSize.cx / fontSize.cy;
	s_fontChar.Add(key, keyHash, fontChar);
	// CRuFontTexture::ReadFontTexture(fontChar);
	return fontChar;
}

// --------------------------------------------------------------------------------------
// 繪出字串
bool CRuFontChar::DrawText( CRuFontType* pFontType, float x, float y, int color, const WCHAR* textStr )
{	
	if ( pFontType == NULL || textStr == NULL )
		return false;

	float x_draw = x;
	float y_draw = y;

	PaintBegin();
	for ( int i = 0; textStr[i]; i++ )
	{
		if ( textStr[i] == L'\n' )
		{
			x_draw = (float)x;
			y_draw += (float)pFontType->GetHeight();
		}
		else if ( textStr[i] >= L' ' )
		{
			CRuFontChar* pChar = GetFontChar( pFontType, textStr[i] );
			if ( pChar )
			{
				pChar->Paint( x_draw, y_draw, 1.0f, color );
				x_draw += (float)pChar->GetXSize();
			}
		}
	}
	return true;
}

// ----------------------------------------------------------------------------------
bool CRuFontChar::DrawTextA( CRuFontType* pFontType, float x, float y, int color, const char* textStr )
{
	// 轉換為雙字元組
	WCHAR* tmp = TransToWChar(textStr);
	bool result = DrawText( pFontType, x, y, color, tmp );
	if ( tmp ) delete [] tmp;
	return result;
}

// ----------------------------------------------------------------------------------
void CRuFontChar::Paint( float x, float y, float scale, DWORD color, RuFontShadow *pShadow, DWORD blockColor, bool bOutline )
{
	CRuFontTexture* fontTexture = CRuFontTexture::GetRuFontTexture(m_pFontType, m_char, m_toneSymbol);	
	
	if ( fontTexture )
	{
		float insert_x = 0.0f;
		float insert_y = 0.0f;

		if( bOutline != false )
		{
			insert_x = m_x * scale * 0.083f;
			insert_y = m_y * scale * 0.083f;
		}
		else
		{
			switch ( m_pFontType->GetOutLine() )
			{
			case ruFONTOUTLINE_NORMAL:
				insert_x = m_x * scale * 0.083f;
				insert_y = m_y * scale * 0.083f;
				break;

			case ruFONTOUTLINE_THICK:
				insert_x = m_x * scale * 0.083f * 2.0f;
				insert_y = m_y * scale * 0.083f * 2.0f;
				break;
			}
		}

		if ( insert_x > 1.0f ) insert_x = 1.0f;
		if ( insert_y > 1.0f ) insert_y = 1.0f;

		INT32 rectIdx = 0;

		CRuRectangle destRect;

		if ( bOutline != false )
		{
			destRect.m_left		= x + insert_x;
			destRect.m_top		= y + insert_y;
			destRect.m_right	= x + (m_x * scale) - insert_x;
			destRect.m_bottom	= y + (m_y * scale) - insert_y;
		}
		else
		{
			destRect.m_left		= x;
			destRect.m_top		= y;
			destRect.m_right	= x + (m_x * scale);
			destRect.m_bottom	= y + (m_y * scale);
		}		

		if ( blockColor != 0 && CRuFontTexture::GetBlockTexture() )
		{
			m_runtime_SrcRects[rectIdx]				= CRuFontTexture::GetBlockTexture()->GetRectangle();

			m_runtime_DestRects[rectIdx].m_left		= x;
			m_runtime_DestRects[rectIdx].m_top		= y;
			m_runtime_DestRects[rectIdx].m_right	= x + (m_x * scale);
			m_runtime_DestRects[rectIdx].m_bottom	= y + (m_y * scale);
			
			m_runtime_Colors[rectIdx]				= blockColor;

			++rectIdx;
		}

		if ( insert_x > 0.01f && insert_y > 0.01f )
		{
			CRuRectangle inset;
			DWORD blackColor = (color & 0xFF000000) | 0x00;
			
			// 左
			m_runtime_SrcRects[rectIdx]				= fontTexture->GetRectangle();

			m_runtime_DestRects[rectIdx].m_top		= destRect.m_top;
			m_runtime_DestRects[rectIdx].m_left		= destRect.m_left - insert_x;
			m_runtime_DestRects[rectIdx].m_bottom	= destRect.m_bottom;
			m_runtime_DestRects[rectIdx].m_right	= destRect.m_right - insert_x;

			m_runtime_Colors[rectIdx]				= blackColor;

			++rectIdx;

			// 右
			m_runtime_SrcRects[rectIdx]				= m_runtime_SrcRects[rectIdx - 1];

			m_runtime_DestRects[rectIdx].m_top		= destRect.m_top;
			m_runtime_DestRects[rectIdx].m_left		= destRect.m_left + insert_x;
			m_runtime_DestRects[rectIdx].m_bottom	= destRect.m_bottom;
			m_runtime_DestRects[rectIdx].m_right	= destRect.m_right + insert_x;

			m_runtime_Colors[rectIdx]				= blackColor;

			++rectIdx;

			// 下
			m_runtime_SrcRects[rectIdx]				= m_runtime_SrcRects[rectIdx - 1];

			m_runtime_DestRects[rectIdx].m_top		= destRect.m_top - insert_y;
			m_runtime_DestRects[rectIdx].m_left		= destRect.m_left;
			m_runtime_DestRects[rectIdx].m_bottom	= destRect.m_bottom - insert_y;
			m_runtime_DestRects[rectIdx].m_right	= destRect.m_right;

			m_runtime_Colors[rectIdx]				= blackColor;

			++rectIdx;

			// 上
			m_runtime_SrcRects[rectIdx]				= m_runtime_SrcRects[rectIdx - 1];

			m_runtime_DestRects[rectIdx].m_top		= destRect.m_top + insert_y;
			m_runtime_DestRects[rectIdx].m_left		= destRect.m_left;
			m_runtime_DestRects[rectIdx].m_bottom	= destRect.m_bottom + insert_y;
			m_runtime_DestRects[rectIdx].m_right	= destRect.m_right;

			m_runtime_Colors[rectIdx]				= blackColor;

			++rectIdx;
		}
		else if ( pShadow && (pShadow->color >> 24) > 0x08 )
		{
			m_runtime_SrcRects[rectIdx]				= fontTexture->GetRectangle();

			m_runtime_DestRects[rectIdx].m_left		= destRect.m_left + pShadow->x;
			m_runtime_DestRects[rectIdx].m_top		= destRect.m_top + pShadow->y;
			m_runtime_DestRects[rectIdx].m_right	= destRect.m_right + pShadow->x;
			m_runtime_DestRects[rectIdx].m_bottom	= destRect.m_bottom + pShadow->y;

			m_runtime_Colors[rectIdx]				= pShadow->color & 0xFFFFFF;

			++rectIdx;
		}

		m_runtime_SrcRects[rectIdx]					= fontTexture->GetRectangle();

		m_runtime_DestRects[rectIdx].m_left			= destRect.m_left;
		m_runtime_DestRects[rectIdx].m_top			= destRect.m_top;
		m_runtime_DestRects[rectIdx].m_right		= destRect.m_right;
		m_runtime_DestRects[rectIdx].m_bottom		= destRect.m_bottom;

		/*
		m_runtime_DestRects[rectIdx].m_left			= x;
		m_runtime_DestRects[rectIdx].m_top			= y;
		m_runtime_DestRects[rectIdx].m_right		= x + m_x * scale;
		m_runtime_DestRects[rectIdx].m_bottom		= y + m_y * scale;
		*/

		m_runtime_Colors[rectIdx]					= color;

		++rectIdx;

		g_ruUIRenderer->Submit(rectIdx, m_runtime_SrcRects, m_runtime_DestRects, m_runtime_Colors);





/*
		CRuRectangle srcRect, destRect;

		destRect.m_left		= x;
		destRect.m_top		= y;
		destRect.m_right	= x + m_x * scale;
		destRect.m_bottom	= y + m_y * scale;
		srcRect				= fontTexture->GetRectangle();

		if ( blockColor != 0 )
		{
			CRuRectangle blockRect = CRuFontTexture::GetBlockTexture()->GetRectangle();
			g_ruUIRenderer->Submit(1, &blockRect, &destRect, &blockColor);
		}

		float insert_x = 0.0f;
		float insert_y = 0.0f;

		if( bOutline != false )
		{
			insert_x = m_x * scale * 0.083f;
			insert_y = m_y * scale * 0.083f;
		}
		else
		{
			switch ( m_pFontType->GetOutLine() )
			{
			case ruFONTOUTLINE_NORMAL:
				insert_x = m_x * scale * 0.083f;
				insert_y = m_y * scale * 0.083f;
				break;

			case ruFONTOUTLINE_THICK:
				insert_x = m_x * scale * 0.083f * 2.0f;
				insert_y = m_y * scale * 0.083f * 2.0f;
				break;
			}
		}

		if ( insert_x > 0.01f && insert_y > 0.01f )
		{
			if ( insert_x > 1.0f ) insert_x = 1.0f;
			if ( insert_y > 1.0f ) insert_y = 1.0f;

			CRuRectangle inset;
			DWORD blackColor = (color & 0xFF000000) | 0x00;
			// 左
			inset.m_top		= destRect.m_top;
			inset.m_left	= destRect.m_left - insert_x;
			inset.m_bottom	= destRect.m_bottom;
			inset.m_right	= destRect.m_right - insert_x;
			g_ruUIRenderer->Submit(1, &srcRect, &inset, &blackColor);

			// 右
			inset.m_top		= destRect.m_top;
			inset.m_left	= destRect.m_left + insert_x;
			inset.m_bottom	= destRect.m_bottom;
			inset.m_right	= destRect.m_right + insert_x;
			g_ruUIRenderer->Submit(1, &srcRect, &inset, &blackColor);

			// 下
			inset.m_top		= destRect.m_top - insert_y;
			inset.m_left	= destRect.m_left;
			inset.m_bottom	= destRect.m_bottom - insert_y;
			inset.m_right	= destRect.m_right;
			g_ruUIRenderer->Submit(1, &srcRect, &inset, &blackColor);

			// 上
			inset.m_top		= destRect.m_top + insert_y;
			inset.m_left	= destRect.m_left;
			inset.m_bottom	= destRect.m_bottom + insert_y;
			inset.m_right	= destRect.m_right;
			g_ruUIRenderer->Submit(1, &srcRect, &inset, &blackColor);
		}
		else if ( pShadow && (pShadow->color >> 24) > 0x08 )
		{
			CRuRectangle shadowRect;
			DWORD shadowColor = pShadow->color & 0xFFFFFF;

			shadowRect.m_left	= destRect.m_left + pShadow->x;
			shadowRect.m_top	= destRect.m_top + pShadow->y;
			shadowRect.m_right	= destRect.m_right + pShadow->x;
			shadowRect.m_bottom	= destRect.m_bottom + pShadow->y;

			shadowColor |= ((pShadow->color >> 24) * (color >> 24) / 255) << 24;
			g_ruUIRenderer->Submit(1, &srcRect, &shadowRect, &shadowColor);
		}

		g_ruUIRenderer->Submit(1, &srcRect, &destRect, &color);
*/
	}
}

// --------------------------------------------------------------------------------------
CRuFontTexture2::CRuFontTexture2()
{
}

// --------------------------------------------------------------------------------------
CRuFontTexture2::~CRuFontTexture2()
{
	Release();
}

// --------------------------------------------------------------------------------------
bool CRuFontTexture2::Initialize( RuFontTextureCallBack func )
{
	s_isLockRally		= false;
	s_pRuTexture		= NULL;	
	s_rallyFunc			= NULL;
	s_blockTexture		= NULL;
	s_blockXSize		= 0;
	s_blockYSize		= 0;
	s_rallyYLast		= 0;
	memset(s_useBlock, 0, sizeof(s_useBlock));
	
	if ( (s_pRuTexture = g_ruUIRenderer->GetReservedDXT5Texture()) == NULL  )
		return false;

	// 計算貼圖總存放字元個數
	s_blockXSize		= FONT_TEXTURE_WIDTH / FONT_TEXTURE_BLOCK_XSIZE;
	s_blockYSize		= FONT_TEXTURE_HEIGHT / FONT_TEXTURE_BLOCK_YSIZE;
	s_rallyFunc			= func;

	GetBlockTexture();

	return true;
}

// --------------------------------------------------------------------------------------
void CRuFontTexture2::Shutdown()
{
	// 清除所有貼圖
	CRuFontTexture2* pFontTexture = NULL;
	for ( int y = 0; y < s_blockYSize; y++ )
	{
		for ( int x = 0; x < s_blockXSize; x++ )
		{			
			if ( s_useBlock[y][x] != pFontTexture )
			{
				pFontTexture = s_useBlock[y][x];
				ReleaseFontTexture(pFontTexture);
			}
			s_useBlock[y][x] = NULL;
		}
	}

	s_isLockRally		= false;
	s_pRuTexture		= NULL;	
	s_rallyFunc			= NULL;
	s_blockTexture		= NULL;
	s_blockXSize		= 0;
	s_blockYSize		= 0;
	s_rallyYLast		= 0;
	memset(s_useBlock, 0, sizeof(s_useBlock));
}

// --------------------------------------------------------------------------------------
void CRuFontTexture2::ReleaseAllFontIndex()
{
	Rally(true);
}

// --------------------------------------------------------------------------------------
void CRuFontTexture2::ReleaseFontTexture(CRuFontTexture2* fontTexture)
{
	if ( fontTexture )
	{
		if ( fontTexture == s_blockTexture )
			s_blockTexture = NULL;

		delete fontTexture;
	}	
}

// --------------------------------------------------------------------------------------
void CRuFontTexture2::PaintBegin()
{		
	g_ruUIRenderer->SetTexture( s_pRuTexture, FALSE );	
	g_ruUIRenderer->SetEffectType( ru2DFXTYPE_TEXTBLIT_FILTER );
	g_ruUIRenderer->SetEffectParameter( ru2DFXPARAM_ROTATE, ru2DROTATE_NONE );
}

// --------------------------------------------------------------------------------------
void CRuFontTexture2::Release()
{
	if ( m_pFontString )
	{
		CRuFontString::ReleaseFontString(m_pFontString);
		m_pFontString = NULL;
	}
}

// --------------------------------------------------------------------------------------
bool CRuFontTexture2::GetFreeUnitTexture(int unitWidth, int &x, int &y)
{
	x = y = 0;
	for ( y = 0; y < s_blockYSize; y++ )
	{
		int count = unitWidth;
		for ( x = 0; x < s_blockXSize; x++ )
		{
			if ( s_useBlock[y][x] == NULL )
			{
				count--;
				if ( count <= 0 )
				{
					x -= (unitWidth - 1);
					return true;
				}
			}
			else
				count = unitWidth;
		}		
	}

	return false;
}

// --------------------------------------------------------------------------------------
void CRuFontTexture2::Rally(bool removeAll)
{
	if ( !s_isLockRally )
	{
		s_isLockRally = true;

		if ( removeAll )
		{
			for ( int y = 0; y < s_blockYSize; y++ )
			{
				CRuFontTexture2* pFontTexture = NULL;
				for ( int x = 0; x < s_blockXSize; x++ )
				{				
					if ( s_useBlock[y][x] != pFontTexture )
					{
						pFontTexture = s_useBlock[y][x];
						ReleaseFontTexture(pFontTexture);
					}
					s_useBlock[y][x] = NULL;
				}
			}
			s_rallyYLast = 0;
		}
		else
		{
			int y = s_rallyYLast;
			for ( int i = 0; i < s_blockYSize / 3; i++, y++ )
			{
				if ( y >= s_blockYSize )
					y = 0;

				CRuFontTexture2* pFontTexture = NULL;
				for ( int x = 0; x < s_blockXSize; x++ )
				{				
					if ( s_useBlock[y][x] != pFontTexture )
					{
						pFontTexture = s_useBlock[y][x];
						ReleaseFontTexture(pFontTexture);
					}
					s_useBlock[y][x] = NULL;
				}
			}
			s_rallyYLast = y;
		}
		
		if ( s_rallyFunc )
			s_rallyFunc();

		s_isLockRally = false;
	}
}

// --------------------------------------------------------------------------------------
CRuFontTexture2* CRuFontTexture2::GetBlockTexture()
{
	int x, y;
	if ( s_blockTexture == NULL && GetFreeUnitTexture(1, x, y) )
	{
		s_blockTexture					= ruNEW CRuFontTexture2;	
		s_blockTexture->m_pFontString	= NULL;
		s_blockTexture->m_x				= x;
		s_blockTexture->m_y				= y;
		s_blockTexture->m_length		= 1;
		s_blockTexture->m_rect.m_left	= (float)(x * FONT_TEXTURE_BLOCK_XSIZE);
		s_blockTexture->m_rect.m_top	= (float)(y * FONT_TEXTURE_BLOCK_YSIZE);
		s_blockTexture->m_rect.m_right	= s_blockTexture->m_rect.m_left + FONT_TEXTURE_BLOCK_XSIZE;
		s_blockTexture->m_rect.m_bottom	= s_blockTexture->m_rect.m_top + FONT_TEXTURE_BLOCK_YSIZE;
		s_useBlock[y][x]				= s_blockTexture;
		
		// 填成純白資料		
		INT32 offsetX = x * FONT_TEXTURE_BLOCK_XSIZE;
		INT32 offsetY = y * FONT_TEXTURE_BLOCK_YSIZE;
		INT32 levels = s_pRuTexture->GetLevelCount();		
		for(INT32 curLevel = 0; curLevel < levels; ++curLevel )
		{
			BYTE *pRow = NULL;
			INT32 pitch;
			if ( s_pRuTexture->Lock(curLevel, (void**)&pRow, &pitch) )
			{
				pRow = reinterpret_cast<BYTE *>(reinterpret_cast<BYTE *>(pRow) + (offsetX >> curLevel) * sizeof(BYTE) + (offsetY >> curLevel) * pitch);
				for ( int h = 0; h < (FONT_TEXTURE_BLOCK_YSIZE >> curLevel); h++ )
				{
					memset(pRow, 0xFF, sizeof(BYTE) * (FONT_TEXTURE_BLOCK_XSIZE >> curLevel));
					pRow += pitch;
				}
				s_pRuTexture->Unlock(curLevel);
			}
		}
	}

	return s_blockTexture;
}

// --------------------------------------------------------------------------------------
CRuFontTexture2* CRuFontTexture2::GetRuFontTexture(CRuFontString* pFontString)
{
	if ( pFontString == NULL )
		return NULL;

	SIZE size;
	if ( pFontString->GetFontPaintSize(size) == false )
		return  NULL;

	int width = (FONT_TEXTURE_UNIT_SIZE - 1) * size.cx / size.cy;

	// 找尋空的欄位
	int length = (width + FONT_TEXTURE_BLOCK_XSIZE - 1) / FONT_TEXTURE_BLOCK_XSIZE;
	int x, y;
	if ( GetFreeUnitTexture(length, x, y) == false )
	{
		// 找不到合適位置,刪除舊有資料
		Rally();

		// 找尋空的欄位
		if ( GetFreeUnitTexture(length, x, y) == false )
		{
			return NULL;
		}
	}

	CRuFontTexture2* pFontTexture = ruNEW CRuFontTexture2;	
	pFontTexture->m_pFontString = pFontString;
	pFontTexture->m_x			= x;
	pFontTexture->m_y			= y;
	pFontTexture->m_length		= length;
	pFontTexture->m_rect.m_left	= (float)(x * FONT_TEXTURE_BLOCK_XSIZE);
	pFontTexture->m_rect.m_top	= (float)(y * FONT_TEXTURE_BLOCK_YSIZE);

	// Paint character to texture
	SIZE fontSize;
	pFontString->GetFontType()->PaintToMem(pFontString->GetText(),
		pFontString->GetTextLength(),
		s_pRuTexture,
		static_cast<INT32>(pFontTexture->m_rect.m_left),
		static_cast<INT32>(pFontTexture->m_rect.m_top),
		fontSize);

	// Set texture size according to the size structure returned by PaintToMem	
	pFontTexture->m_rect.m_right	= pFontTexture->m_rect.m_left + fontSize.cx;
	pFontTexture->m_rect.m_bottom	= pFontTexture->m_rect.m_top + fontSize.cy;

	for ( int i = 0; i < length; i++ )
	{
		s_useBlock[y][x + i] = pFontTexture;
	}

	return pFontTexture;
}

// --------------------------------------------------------------------------------------
CRuFontString::CRuFontString()
{
	m_x				= 0.0f;
	m_y				= 0.0f;
	m_pFontType		= NULL;
	m_pFontTexture	= NULL;
	m_text			= NULL;
	m_textLength	= 0;
	m_isReleaseSelf	= FALSE;
}

// --------------------------------------------------------------------------------------
CRuFontString::~CRuFontString()
{
	if ( !s_shutdownLocked && m_pFontType && m_text )
	{
		wstring nTT;
		for ( int i = 0; i < m_textLength; i++ )
			nTT += m_text[i];

		nTT += m_pFontType->GetSerial();
		s_fontString.erase(nTT);
	}

	ruSAFE_DELETE(m_text);
}

// --------------------------------------------------------------------------------------
void CRuFontString::Initialize( RuFontTextureCallBack func )
{
	CRuFontType::Initialize();
	CRuFontTexture2::Initialize( func );

	s_fontString.clear();
}

// --------------------------------------------------------------------------------------
void CRuFontString::Shutdown()
{
	s_shutdownLocked = true;

	// 釋放所有貼圖
	CRuFontTexture2::Shutdown();

	// 刪除字串元件
	for ( map<wstring, CRuFontString*>::iterator iter = s_fontString.begin(); iter != s_fontString.end(); iter++ )
		delete iter->second;
	s_fontString.clear();	

	// 移除字體
	CRuFontType::Shutdown();

	s_shutdownLocked = false;
}

// --------------------------------------------------------------------------------------
CRuFontString* CRuFontString::GetFontString( CRuFontType *pFontType, const WCHAR* text, int length, BOOL isReleaseWhenTextureNull )
{
	assert(pFontType);

	if ( text == NULL || length <= 0 )
		return NULL;

	// 找尋是否有相同字串
	wstring nTT;
	for ( int i = 0; i < length; i++ )
		nTT += text[i];
	nTT += pFontType->GetSerial();

	map<wstring, CRuFontString*>::iterator iter = s_fontString.find(nTT);
	if ( iter != s_fontString.end() )
		return iter->second;

	CRuFontString* fontString = ruNEW CRuFontString();
	fontString->SetText(text, length);

	// 取得字型寬高
	SIZE fontSize;
	pFontType->GetFontSize(text, length, fontSize);

	fontString->m_pFontType		= pFontType;
	fontString->m_y				= (float)pFontType->GetHeight();
	fontString->m_x				= (float)fontString->m_y * fontSize.cx / fontSize.cy;
	fontString->m_isReleaseSelf	= false;	
	//fontString->m_isReleaseSelf	= isReleaseWhenTextureNull;	
	s_fontString[nTT]			= fontString;

	return fontString;
}

// --------------------------------------------------------------------------------------
bool CRuFontString::DrawText( CRuFontType* pFontType, float x, float y, int color, const WCHAR* textStr )
{
	if ( pFontType == NULL || textStr == NULL )
		return false;
	
	float x_draw = x;
	float y_draw = y;

	PaintBegin();

	vector<wstring> strList;
	SplitString(textStr, (int)wcslen(textStr), strList);

	for ( vector<wstring>::iterator iter = strList.begin(); iter != strList.end(); iter++ )
	{
		if ( iter->compare(L"\n") == 0 )
		{
			x_draw = (float)x;
			y_draw += (float)pFontType->GetHeight();
		}
		else
		{
			CRuFontString* pFontString = GetFontString(pFontType, iter->c_str(), (int)iter->length(), TRUE);
			if ( pFontString )
			{
				pFontString->Paint(x_draw, y_draw, 1.0f, color);
				x_draw += (float)pFontString->GetXSize();
			}
		}
	}
	
	return true;
}

// --------------------------------------------------------------------------------------
bool CRuFontString::DrawTextA( CRuFontType* pFontType, float x, float y, int color, const char* textStr )
{
	// 轉換為雙字元組
	WCHAR* tmp = TransToWChar(textStr);
	bool result = DrawText( pFontType, x, y, color, tmp );
	if ( tmp ) delete [] tmp;
	return result;
}

/*
// --------------------------------------------------------------------------------------
void CRuFontString::SplitString( const WCHAR* text, int length, list<wstring> &outVec )
{
	if ( text == NULL || length <= 0 )
		return;

	// 將左右不同段分字拆開處理
	wstring wstr;	
	WORD charType[2] = { 0, 0 };
	
	int end = 0;
	while ( end < length )
	{
		GetStringTypeW(CT_CTYPE2, text + end, 1, &charType[1]);
		if ( end == 0 )
		{
			charType[0] = charType[1];
		}
		else
		{
			if ( charType[0] != charType[1] )
			{
				if ( charType[0] == C2_RIGHTTOLEFT )
				{
					if ( text[end] == L'\n' )
					{
						break;
					}

					if ( charType[1] == C2_LEFTTORIGHT )
					{
						int backCount = 0;
						while ( backCount < end )
						{
							GetStringTypeW(CT_CTYPE2, text + (end - backCount - 1), 1, &charType[1]);
							if ( charType[1] != C2_RIGHTTOLEFT )
								backCount++;
							else
								break;
						}
						end -= backCount;
						break;
					}
				}
				else if ( charType[1] == C2_RIGHTTOLEFT )
				{
					break;
				}
			}
		}
		end++;
	}

	int i = 0;
	while ( i < end )
	{
		// 阿拉伯字元判斷為連字
		if ( IsCharAlphaW(text[i]) && text[i] >= 0x0600 && text[i] <= 0x06FF )
		{
			wstr += text[i];
		}
		else
		{
			if ( !wstr.empty() )
			{
				if ( charType[0] == C2_RIGHTTOLEFT )
					outVec.push_front(wstr);
				else
					outVec.push_back(wstr);
				wstr.clear();
			}

			wstr += text[i];

			// 非泰文聲調符號,檢查下一個字元是否為聲調字元
			if ( IsToneSymbol(text[i]) == false )
			{
				while ( i + 1 < end && IsToneSymbol(text[i + 1]) )
					wstr += text[++i];
			}

			if ( charType[0] == C2_RIGHTTOLEFT )
				outVec.push_front(wstr);
			else
				outVec.push_back(wstr);
			wstr.clear();
		}
		i++;
	}
	if ( !wstr.empty() )
	{
		if ( charType[0] == C2_RIGHTTOLEFT )
			outVec.push_front(wstr);
		else
			outVec.push_back(wstr);
	}

	if ( end < length )
	{
		list<wstring> tempList;
		SplitString(text + end, length - end, tempList);
		for ( list<wstring>::iterator iter = tempList.begin(); iter != tempList.end(); iter++ )
			outVec.push_back(*iter);
	}
}
*/

// --------------------------------------------------------------------------------------
// 將字串拆解成可以用的顯示字串(主要處理泰文,阿位伯文)
void CRuFontString::SplitString( const WCHAR* text, int size, vector<wstring> &outVec )
{	
	int i = 0;
	int ns = 0;	
	bool isRTL = false;	
	bool isFirstRTL = false;	
	WORD* charType = new WORD[size];

	// 分段處理
	while ( text[i] && i < size )
	{		
		// 下一個字元位置
		const wchar_t* charNext = CRuFontString::CharNext(&text[i], size - i);

		int ln = int(charNext - &text[i]);

		wstring newStr;
		for ( int n = 0; n < ln; n++ )
			newStr += text[i + n];

		GetStringTypeW(CT_CTYPE2, text + i, 1, charType + ns);
		if ( charType[ns] == C2_RIGHTTOLEFT )
		{
			if ( i == 0 )
				isFirstRTL = true;

			isRTL = true;
		}
		outVec.push_back(newStr);
		
		ns++;
		i += ln;
	}

	// 右到左顯示需要調整顯示順序
	if ( isRTL )
	{
		vector<wstring> temp = outVec;
		int r = (int)outVec.size() - 1;
		int l = 0;
		int ls[1024];

		i = 0;
		bool isPreRTL = isFirstRTL;
		while ( i < ns )
		{
			int count = 0;
			while ( i + count < ns )
			{
				if ( charType[i + count] == C2_LEFTTORIGHT )
				{
					if ( isPreRTL )
					{
						isPreRTL = false;
						break;
					}
				}
				else if ( charType[i + count] == C2_RIGHTTOLEFT )
				{
					if ( !isPreRTL )
					{
						isPreRTL = true;
						break;
					}
				}
				else if ( charType[i + count] == C2_EUROPESEPARATOR || charType[i + count] == C2_OTHERNEUTRAL )
				{
					// 目前與開頭都是RTL
					if ( isPreRTL && isFirstRTL )
					{
						RTLChangeSign(temp[i + count][0]);
					}
					// 開頭是RTL
					else if ( isFirstRTL )
					{
						bool end = true;
						isPreRTL = true;

						// 檢查下一個字元
						for ( int k = i + count + 1; k < ns; k++ )
						{
							if ( charType[k] == C2_LEFTTORIGHT ||
								 charType[k] == C2_EUROPENUMBER ||
								 charType[k] == C2_ARABICNUMBER )
							{
								isPreRTL = false;
								end = false;
								break;
							}
							else if ( charType[k] == C2_RIGHTTOLEFT )
							{
								break;
							}							
						}

						if ( isPreRTL )
							RTLChangeSign(temp[i + count][0]);

						if ( end )
							break;
					}
					else if ( isPreRTL )
					{
						bool end = true;
						isPreRTL = false;

						// 檢查下一個字元
						for ( int k = i + count + 1; k < ns; k++ )
						{
							if ( charType[k] == C2_LEFTTORIGHT )
							{
								break;
							}
							else if ( charType[k] == C2_RIGHTTOLEFT || charType[k] == C2_EUROPENUMBER || charType[k] == C2_ARABICNUMBER )
							{
								isPreRTL = true;
								end = false;
								break;
							}
						}

						if ( isPreRTL )
							RTLChangeSign(temp[i + count][0]);

						if ( end )
							break;						
					}
				}

				count++;
			}

			// RTL 需要將數字做反向
			vector<int> order;
			int j = 0;
			while ( j < count )
			{
				int len = 1;

				if ( charType[i] == C2_RIGHTTOLEFT || (isFirstRTL && (charType[i] != C2_LEFTTORIGHT)) )
				{
					// 英文做反向
					if ( charType[i + (count - j - len)] == C2_LEFTTORIGHT )
					{
						while ( len < count - j )
						{
							bool isBreak = true;
							switch ( charType[i + (count - j - len - 1)] )
							{
							case C2_LEFTTORIGHT:							
							case C2_EUROPENUMBER:
							case C2_EUROPESEPARATOR:
							case C2_EUROPETERMINATOR:
							case C2_COMMONSEPARATOR:							
							case C2_BLOCKSEPARATOR:
							case C2_WHITESPACE:
							case C2_OTHERNEUTRAL:
								isBreak = false;
								len++;
								break;

								/*
							case C2_RIGHTTOLEFT:
								while ( charType[i + (count - j - len)] != C2_LEFTTORIGHT )
									len--;								
								break;
								*/
							}

							if (isBreak)
								break;
						}
					}
					// 如果是RTL要將數字做反向
					else if ( charType[i + (count - j - len)] == C2_EUROPENUMBER )
					{					
						while ( len < count - j )
						{
							bool isBreak = true;
							switch ( charType[i + (count - j - len - 1)] )
							{
							case C2_EUROPENUMBER:
							case C2_EUROPESEPARATOR:
							case C2_EUROPETERMINATOR:
							case C2_COMMONSEPARATOR:
								isBreak = false;
								len++;
								break;
							}

							if (isBreak)
								break;
						}
					}

					for ( int n = len; n > 0; n-- )
						order.push_back( i + (count - j - n) );
				}
				else
				{
					order.push_back(i + j);
				}

				j += len;
			}

			if ( isFirstRTL )
			{
				for ( vector<int>::reverse_iterator iter = order.rbegin(); iter != order.rend(); iter++ )
				{
					ls[r--] = *iter;
				}
			}
			else
			{
				for ( vector<int>::iterator iter = order.begin(); iter != order.end(); iter++ )
				{
					ls[l++] = *iter;
				}
			}

			i += count;
		}

		for ( int k = 0; k < outVec.size(); k++ )
		{
			outVec[k] = temp[ls[k]];
		}
	}

	delete [] charType;
}

// --------------------------------------------------------------------------------------
void CRuFontString::ReleaseFontString(CRuFontString* pFontString)
{
	if ( pFontString && !s_shutdownLocked )
	{
		pFontString->m_pFontTexture = NULL;
		if ( pFontString->m_isReleaseSelf )
		{
			ruSAFE_DELETE(pFontString);
		}
	}
}

// --------------------------------------------------------------------------------------
const WCHAR* CRuFontString::CharNext(const WCHAR* inString, int length)
{
	bool isArablic = false;
	int i = 0;

	if ( length == -1 )
		length = (int)wcslen(inString);

	while ( i < length )
	{
		// 阿拉伯字元判斷為連字
		if ( IsCharAlphaW(inString[i]) && inString[i] >= 0x0600 && inString[i] <= 0x06FF )
		{
			i++;
			isArablic = true;
		}
		else if ( isArablic )
		{
			break;
		}
		else
		{
			i++;

			// 非泰文聲調符號,檢查下一個字元是否為聲調字元
			if ( IsToneSymbol(inString[i - 1]) == false )
			{
				while ( i < length && IsToneSymbol(inString[i]) )
					i++;
			}

			break;
		}
	}	

	return inString + i;
}

// --------------------------------------------------------------------------------------
void CRuFontString::SetText(const WCHAR* text, int length)
{
	if ( m_text )
	{
		delete [] m_text;
		m_text = NULL;
	}
	m_textLength = length;

	if ( text )
	{
		m_text = ruNEW WCHAR[length];
		memcpy( m_text, text, sizeof(WCHAR) * length );
	}
}

// --------------------------------------------------------------------------------------
void CRuFontString::Paint( float x, float y, float scale, DWORD color, RuFontShadow *pShadow, DWORD blockColor, bool bOutline, bool RTLChangeSign)
{
	CRuRectangle	runtime_SrcRects[12];
	CRuRectangle	runtime_DestRects[12];
	DWORD			runtime_Colors[12];

	CRuFontTexture2* pFontTexture = GetFontTexture();
	if ( pFontTexture )
	{		
		float insert_x = 0.0f;
		float insert_y = 0.0f;

		if( bOutline != false )
		{
			insert_x = m_x * scale * 0.083f;
			insert_y = m_y * scale * 0.083f;
		}
		else
		{
			switch ( m_pFontType->GetOutLine() )
			{
			case ruFONTOUTLINE_NORMAL:
				insert_x = m_x * scale * 0.083f;
				insert_y = m_y * scale * 0.083f;
				break;

			case ruFONTOUTLINE_THICK:
				insert_x = m_x * scale * 0.083f * 2.0f;
				insert_y = m_y * scale * 0.083f * 2.0f;
				break;
			}
		}

		if ( insert_x > 1.0f ) insert_x = 1.0f;
		if ( insert_y > 1.0f ) insert_y = 1.0f;

		INT32 rectIdx = 0;

		CRuRectangle destRect;
		CRuRectangle srcRect = pFontTexture->GetRectangle();
		if ( RTLChangeSign != false && m_textLength == 1 )
		{
			switch (m_text[0])
			{
			case L'[':
			case L']':
			case L'{':
			case L'}':
			case L'(':
			case L')':
			case L'<':
			case L'>':
				swap(srcRect.m_left, srcRect.m_right);
				break;
			}
		}

		if ( bOutline != false )
		{
			destRect.m_left		= x + insert_x;
			destRect.m_top		= y + insert_y;
			destRect.m_right	= x + (m_x * scale) - insert_x;
			destRect.m_bottom	= y + (m_y * scale) - insert_y;
		}
		else
		{
			destRect.m_left		= x;
			destRect.m_top		= y;
			destRect.m_right	= x + (m_x * scale);
			destRect.m_bottom	= y + (m_y * scale);
		}		

		if ( blockColor != 0 && CRuFontTexture2::GetBlockTexture() )
		{
			runtime_SrcRects[rectIdx]			= CRuFontTexture2::GetBlockTexture()->GetRectangle();

			runtime_DestRects[rectIdx].m_left	= x;
			runtime_DestRects[rectIdx].m_top	= y;
			runtime_DestRects[rectIdx].m_right	= x + (m_x * scale);
			runtime_DestRects[rectIdx].m_bottom	= y + (m_y * scale);

			runtime_Colors[rectIdx]				= blockColor;

			++rectIdx;
		}

		if ( insert_x > 0.01f && insert_y > 0.01f )
		{
			CRuRectangle inset;
			DWORD blackColor = (color & 0xFF000000) | 0x00;

			// 左
			runtime_SrcRects[rectIdx]			= srcRect;

			runtime_DestRects[rectIdx].m_top	= destRect.m_top;
			runtime_DestRects[rectIdx].m_left	= destRect.m_left - insert_x;
			runtime_DestRects[rectIdx].m_bottom	= destRect.m_bottom;
			runtime_DestRects[rectIdx].m_right	= destRect.m_right - insert_x;

			runtime_Colors[rectIdx]				= blackColor;

			++rectIdx;

			// 右
			runtime_SrcRects[rectIdx]			= runtime_SrcRects[rectIdx - 1];

			runtime_DestRects[rectIdx].m_top	= destRect.m_top;
			runtime_DestRects[rectIdx].m_left	= destRect.m_left + insert_x;
			runtime_DestRects[rectIdx].m_bottom	= destRect.m_bottom;
			runtime_DestRects[rectIdx].m_right	= destRect.m_right + insert_x;

			runtime_Colors[rectIdx]				= blackColor;

			++rectIdx;

			// 下
			runtime_SrcRects[rectIdx]			= runtime_SrcRects[rectIdx - 1];

			runtime_DestRects[rectIdx].m_top	= destRect.m_top - insert_y;
			runtime_DestRects[rectIdx].m_left	= destRect.m_left;
			runtime_DestRects[rectIdx].m_bottom	= destRect.m_bottom - insert_y;
			runtime_DestRects[rectIdx].m_right	= destRect.m_right;

			runtime_Colors[rectIdx]				= blackColor;

			++rectIdx;

			// 上
			runtime_SrcRects[rectIdx]			= runtime_SrcRects[rectIdx - 1];

			runtime_DestRects[rectIdx].m_top	= destRect.m_top + insert_y;
			runtime_DestRects[rectIdx].m_left	= destRect.m_left;
			runtime_DestRects[rectIdx].m_bottom	= destRect.m_bottom + insert_y;
			runtime_DestRects[rectIdx].m_right	= destRect.m_right;

			runtime_Colors[rectIdx]				= blackColor;

			++rectIdx;
		}
		else if ( pShadow && (pShadow->color >> 24) > 0x08 )
		{
			runtime_SrcRects[rectIdx]			= srcRect;

			runtime_DestRects[rectIdx].m_left	= destRect.m_left + pShadow->x;
			runtime_DestRects[rectIdx].m_top	= destRect.m_top + pShadow->y;
			runtime_DestRects[rectIdx].m_right	= destRect.m_right + pShadow->x;
			runtime_DestRects[rectIdx].m_bottom	= destRect.m_bottom + pShadow->y;

			runtime_Colors[rectIdx]				= pShadow->color & 0xFFFFFF;

			++rectIdx;
		}

		runtime_SrcRects[rectIdx]				= srcRect;

		runtime_DestRects[rectIdx].m_left		= destRect.m_left;
		runtime_DestRects[rectIdx].m_top		= destRect.m_top;
		runtime_DestRects[rectIdx].m_right		= destRect.m_right;
		runtime_DestRects[rectIdx].m_bottom		= destRect.m_bottom;

		/*
		m_runtime_DestRects[rectIdx].m_left			= x;
		m_runtime_DestRects[rectIdx].m_top			= y;
		m_runtime_DestRects[rectIdx].m_right		= x + m_x * scale;
		m_runtime_DestRects[rectIdx].m_bottom		= y + m_y * scale;
		*/

		runtime_Colors[rectIdx]					= color;

		++rectIdx;

		g_ruUIRenderer->Submit(rectIdx, runtime_SrcRects, runtime_DestRects, runtime_Colors);
	}
}

// --------------------------------------------------------------------------------------
CRuFontTexture2* CRuFontString::GetFontTexture()
{
	if ( m_pFontTexture == NULL )
		m_pFontTexture = CRuFontTexture2::GetRuFontTexture(this);		
	return m_pFontTexture;
}

// ----------------------------------------------------------------------------------
int CRuFontString::Compare(const WCHAR* dest)
{
	return wcsncmp(m_text, dest, m_textLength);
}

// ----------------------------------------------------------------------------------
WORD CRuFontString::GetFirstWordType()
{
	if ( m_text == NULL )
		return 0;

	WORD charType;
	GetStringTypeW(CT_CTYPE2, m_text, 1, &charType);
	return charType;
}

// ----------------------------------------------------------------------------------
bool CRuFontString::GetFontPaintSize(SIZE& size)
{
	if ( m_pFontType == NULL )
		return false;

	m_pFontType->GetFontSize(m_text, m_textLength, size);
	return true;
}

// ----------------------------------------------------------------------------------
char *TransToChar(const WCHAR* text)
{
	char* tmp = NULL;
	if (text)
	{
		int nLen = WideCharToMultiByte(CP_ACP, 0, text, -1, NULL, NULL, NULL, NULL);
		char* tmp = ruNEW char[nLen];
		WideCharToMultiByte(CP_ACP, 0, text, -1, tmp, nLen, NULL, NULL);
		return tmp;
	}
	return tmp;
}

// ----------------------------------------------------------------------------------
WCHAR *TransToWChar(const char* text)
{	
	WCHAR *wbuff = NULL;
	if (text)
	{
		int nLen = MultiByteToWideChar(CP_ACP, 0, text, -1, NULL, NULL);
		wbuff = ruNEW WCHAR[nLen];
		MultiByteToWideChar(CP_ACP, 0, text, -1, wbuff, nLen);
	}
	return wbuff;
}

// ----------------------------------------------------------------------------------
WCHAR* Utf8ToWChar(const char* text)
{
	WCHAR *wbuff = NULL;
	if ( text )
	{
		int nLen = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, NULL);
		wbuff = ruNEW WCHAR[nLen];
		MultiByteToWideChar(CP_UTF8, 0, text, -1, wbuff, nLen);
	}
	return wbuff;
}
// ----------------------------------------------------------------------------------
char* WCharToUtf8(const wchar_t* text)
{
	char *szBuff = NULL;
	if ( text )
	{
		int nLen = WideCharToMultiByte(CP_UTF8, 0, text, -1, NULL, NULL, 0, 0 );
		szBuff = ruNEW char[nLen];
		WideCharToMultiByte(CP_UTF8, 0, text, -1, szBuff, nLen, 0, 0 );
	}
	return szBuff;
}
// ----------------------------------------------------------------------------------
WCHAR* MultiByteToWChar(const char* s)
{
	WCHAR* ws = NULL;
	int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s, -1, NULL, NULL);
	if ( n > 0 ) {
		ws = ruNEW WCHAR[n];
		MultiByteToWideChar(CP_UTF8, 0, s, -1, ws, n);
	} else {
		int n = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, NULL);
		ws = ruNEW WCHAR[n];
		MultiByteToWideChar(CP_ACP, 0, s, -1, ws, n);
	}
	return ws;
}

// ----------------------------------------------------------------------------------
bool IsToneSymbol(WCHAR wch)
{
	// 泰文聲調或特殊符號
	switch(wch)
	{
	case 0x0E31:
	case 0x0E33:
	case 0x0E34:
	case 0x0E35:
	case 0x0E36:
	case 0x0E37:
	case 0x0E38:
	case 0x0E39:
	case 0x0E3A:
	case 0x0E47:
	case 0x0E48:
	case 0x0E49:
	case 0x0E4A:
	case 0x0E4B:
	case 0x0E4C:
	case 0x0E4D:
	case 0x0E4E:
		return true;
		break;
	}
	return false;
}

#pragma managed(pop)
