#include "cImage.h"

#if IMAGELIB_SUPPORT_BMP
	#include "cImageBMP.h"
#endif
#if IMAGELIB_SUPPORT_GIF
	#include "cImageGIF.h"
#endif
#if IMAGELIB_SUPPORT_JPG
	#include "cImageJPG.h"
#endif
#if IMAGELIB_SUPPORT_PCX
	#include "cImagePCX.h"
#endif
#if IMAGELIB_SUPPORT_PNG
	#include "cImagePNG.h"
#endif
#if IMAGELIB_SUPPORT_TGA
	#include "cImageTGA.h"
#endif

BYTE g_5BitsTo8BitsArray[32] = {
	0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39,
	0x41, 0x4A, 0x52, 0x5A, 0x62, 0x6A, 0x73, 0x7B,
	0x83, 0x8B, 0x94, 0x9C, 0xA4, 0xAC, 0xB4, 0xBD,
	0xC5, 0xCD, 0xD5, 0xDE, 0xE6, 0xEE, 0xF6, 0xFF
};

BYTE g_6BitsTo8BitsArray[64] = {
	0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C,
	0x20, 0x24, 0x28, 0x2C, 0x30, 0x34, 0x38, 0x3C,
	0x40, 0x44, 0x48, 0x4C, 0x50, 0x55, 0x59, 0x5D,
	0x61, 0x65, 0x69, 0x6D, 0x71, 0x75, 0x79, 0x7D,
	0x81, 0x85, 0x89, 0x8D, 0x91, 0x95, 0x99, 0x9D,
	0xA1, 0xA5, 0xAA, 0xAE, 0xB2, 0xB6, 0xBA, 0xBE,
	0xC2, 0xC6, 0xCA, 0xCE, 0xD2, 0xD6, 0xDA, 0xDE,
	0xE2, 0xE6, 0xEA, 0xEE, 0xF2, 0xF6, 0xFA, 0xFF,
};

cImage::cImage()
{
	m_imageFormat		= IMAGELIB_FORMAT_UNKNOWN;
	m_pixelFormat		= PF_UNKNOWN;
	m_width				= 0;
	m_height			= 0;
	m_depth				= 0;
	m_paletteCount		= 0;
	m_transparentBool	= false;
	m_pPixels			= NULL;
	m_pPalette			= NULL;

	m_ghost				= false;
}

cImage::~cImage()
{
	Destroy();
}

void cImage::Destroy()
{
	m_imageFormat		= IMAGELIB_FORMAT_UNKNOWN;
	m_pixelFormat		= PF_UNKNOWN;
	m_width				= 0;
	m_height			= 0;
	m_depth				= 0;
	m_paletteCount		= 0;
	m_transparentBool	= false;

	if ( !m_ghost )
	{
		if ( m_pPixels ) {
			delete [] m_pPixels;			
		}
		if ( m_pPalette ) {
			delete [] m_pPalette;			
		}
	}
	m_pPixels = NULL;
	m_pPalette = NULL;
}

bool cImage::Create( int width, int height, int depth, ImageLibFormat format )
{
	Destroy();
	
	int bbp = 32;
	if ( depth <= 1 ) bbp = 1;
	else if ( depth <= 4 ) bbp = 4;
	else if ( depth <= 8 ) bbp = 8;
	else if ( depth <= 24 ) bbp = 24;		

	switch ( bbp )
	{
		case 1:
			m_pixelFormat = PF_I8;
			m_depth = 8;
			m_paletteCount = 2;
			break;
		case 4:
			m_pixelFormat = PF_I8;
			m_depth = 8;
			m_paletteCount = 16;
			break;
		case 8:
			m_pixelFormat = PF_I8;
			m_depth = 8;
			m_paletteCount = 256;
			break;
		case 24:
			m_pixelFormat = PF_R8G8B8;
			m_depth = 24;
			break;
		case 32:
			SetTransparent( true );			
			m_pixelFormat = PF_A8R8G8B8;
			m_depth = 32;
			break;
	}

	if ( m_paletteCount > 0 ) m_pPalette = new RGBQUAD[m_paletteCount];
		
	m_width			= width;
	m_height		= height;
	m_imageFormat	= format;
	m_pPixels		= new BYTE[m_width * m_height * (m_depth >> 3)];
	return true;
}

bool cImage::Mirror()
{
	if ( m_pPixels == NULL ) return false;	
	switch ( m_depth )
	{
		case 8:
		{
			BYTE *pDest = m_pPixels;
			for ( int yy = 0; yy < m_height; yy ++ ) {
				BYTE tmp;
				for ( int xx = 0; xx < m_width / 2; xx++ ) {
					tmp = pDest[xx];
					pDest[xx] = pDest[m_width - xx - 1];
					pDest[m_width - xx - 1] = tmp;
				}
				pDest += m_width;
			}
			break;
		}
		case 24:
		{
			BGRCOLOR *pDest = (BGRCOLOR*)m_pPixels;
			for ( int yy = 0; yy < m_height; yy ++ ) {
				BGRCOLOR tmp;
				for ( int xx = 0; xx < m_width / 2; xx++ ) {
					tmp = pDest[xx];
					pDest[xx] = pDest[m_width - xx - 1];
					pDest[m_width - xx - 1] = tmp;
				}
				pDest += m_width;
			}
			break;
		}
		case 32:
		{
			RGBQUAD *pDest = (RGBQUAD*)m_pPixels;
			for ( int yy = 0; yy < m_height; yy ++ ) {
				RGBQUAD tmp;
				for ( int xx = 0; xx < m_width / 2; xx++ ) {
					tmp = pDest[xx];
					pDest[xx] = pDest[m_width - xx - 1];
					pDest[m_width - xx - 1] = tmp;
				}
				pDest += m_width;
			}
			break;
		}
		default:
			return false;
	}
	return true;
}

bool cImage::ConvertFormat( PixelFormat format )
{
	if ( m_pPixels == NULL ) return false;	
	if ( m_pixelFormat == format ) return true;

	switch ( format )
	{
		case PF_R8G8B8:
			ConvertToR8G8B8();
			break;
		case PF_A8R8G8B8:
			ConvertToA8R8G8B8();
			break;
		default:
			return false;
	}
	return true;
}

bool cImage::Copy( cImage* srcImage, int x, int y )
{
	if ( srcImage == NULL ||
		 srcImage->m_pPixels == NULL ||
		 m_pPixels == NULL ||
		 srcImage->m_pixelFormat != m_pixelFormat )
		return false;	
	
	int srcWidth = srcImage->GetXSize();
	int srcHeight = srcImage->GetYSize();
	int depth = m_depth / 8;

	BYTE* srcPixel = srcImage->GetPixels();
	BYTE* destPixel = m_pPixels;
	for ( int h = 0; h < srcHeight; h++ )
	{
		if ( y + h >= m_height )
			break;

		destPixel = m_pPixels + (y + h) * m_width * depth;
		for ( int w = 0; w < srcWidth; w++ )
		{
			if ( x + w >= m_width )
				break;

			memcpy( destPixel + depth * (x + w), srcPixel + w * depth, depth );
		}
		srcPixel += depth * srcWidth;
	}
	return true;
}

/*
void cImage::ConvertToR5G6B5()
{
	WORD *pNew = new WORD[m_width * m_height];
	WORD *pDest = pNew;
	BYTE *pSrc = m_pPixels;
	RGBQUAD *pPalette = (RGBQUAD *)m_pPalette;
	RGBQUAD c;
	for ( int yy = 0; yy < m_height; yy++ ) {
		for ( int xx = 0; xx < m_width; xx++ ) {
			if ( m_pixelFormat == PF_I8 ) {
				c = pPalette[pSrc[xx]];
			} else if ( m_pixelFormat == PF_R8G8B8 ) {
				c.rgbBlue		= ((BGRCOLOR*)pSrc + xx)->b;
				c.rgbGreen		= ((BGRCOLOR*)pSrc + xx)->g;
				c.rgbRed		= ((BGRCOLOR*)pSrc + xx)->r;
				//c.rgbReserved	= 0xFF;
				//memcpy( &c, (BGRCOLOR*)pSrc + xx, 3 );
			} else {
				c = *((RGBQUAD*)pSrc + xx);
			}
			pDest[xx] = (WORD)( (c.rgbRed & 0xF8) << 11 | (c.rgbGreen & 0xFC) << 5 | (c.rgbBlue & 0xF8) >> 3 );
		}
		pDest += m_width;
		pSrc += (m_depth >> 3) * m_width;
	}
	delete [] m_pPixels;
	m_pPixels = (BYTE*)pNew;
	m_transparentBool = false;
	m_depth = 16;
}

void cImage::ConvertToA1R5G5B5()
{
	WORD *pNew = new WORD[m_width * m_height];
	WORD *pDest = pNew;
	BYTE *pSrc = m_pPixels;
	RGBQUAD *pPalette = (RGBQUAD *)m_pPalette;
	RGBQUAD c;
	for ( int yy = 0; yy < m_height; yy++ ) {
		for ( int xx = 0; xx < m_width; xx++ ) {
			if ( m_pixelFormat == PF_I8 ) {
				c = pPalette[pSrc[xx]];
			} else if ( m_pixelFormat == PF_R8G8B8 ) {
				c.rgbBlue		= ((BGRCOLOR*)pSrc + xx)->b;
				c.rgbGreen		= ((BGRCOLOR*)pSrc + xx)->g;
				c.rgbRed		= ((BGRCOLOR*)pSrc + xx)->r;
				c.rgbReserved	= 0xFF;
				//memcpy( &c, (BGRCOLOR*)pSrc + xx, 3 );				
			} else {
				c = *((RGBQUAD*)pSrc + xx);
			}
			if ( c.rgbReserved ) {
				pDest[xx] = 0x8000 | (c.rgbRed & 0xF8) << 10 | (c.rgbGreen & 0xF8) << 5 | (c.rgbBlue & 0xF8) >> 3;
			} else {
				pDest[xx] = (c.rgbRed & 0xF8) << 10 | (c.rgbGreen & 0xF8) << 5 | (c.rgbBlue & 0xF8) >> 3;
			}
			
		}
		pDest += m_width;
		pSrc += (m_depth >> 3) * m_width;
	}
	delete [] m_pPixels;
	m_pPixels = (BYTE*)pNew;
	m_transparentBool = false;
	m_depth = 16;
}
*/

void cImage::ConvertToR8G8B8()
{
	cImage newImg;
	if ( newImg.Create( m_width, m_height, 24, m_imageFormat ) ) {
		BGRCOLOR *pDest = (BGRCOLOR *)newImg.GetPixels();
		BYTE *pSrc = m_pPixels;
		RGBQUAD c;
		for ( int yy = 0; yy < m_height; yy++ ) {			
			for ( int xx = 0; xx < m_width; xx++ ) {
				if ( m_pixelFormat == PF_I8 ) {
					c = m_pPalette[pSrc[xx]];
				} else {					
					c = *((RGBQUAD*)pSrc + xx);
				}
				pDest[xx].r	= c.rgbBlue;
				pDest[xx].g = c.rgbGreen;
				pDest[xx].b = c.rgbRed;
			}
			pDest += m_width;
			pSrc += (m_depth >> 3) * m_width;
		}
		Destroy();
		Transfer( newImg );
		SetTransparent( false );
	}
}

void cImage::ConvertToA8R8G8B8()
{
	cImage newImg;
	if ( newImg.Create( m_width, m_height, 32, m_imageFormat ) ) {
		RGBQUAD *pDest = (RGBQUAD *)newImg.GetPixels();
		BYTE *pSrc = m_pPixels;
		RGBQUAD c;
		for ( int yy = 0; yy < m_height; yy++ ) {
			for ( int xx = 0; xx < m_width; xx++ ) {
				if ( m_pixelFormat == PF_I8 ) {
					c = m_pPalette[pSrc[xx]];
				} else if ( m_pixelFormat == PF_R8G8B8 ) {
					c.rgbBlue		= ((BGRCOLOR*)pSrc + xx)->r;
					c.rgbGreen		= ((BGRCOLOR*)pSrc + xx)->g;
					c.rgbRed		= ((BGRCOLOR*)pSrc + xx)->b;
					c.rgbReserved	= 0xFF;
					//memcpy( &c, (BGRCOLOR*)pSrc + xx, 3 );
				} else {
					c = *((RGBQUAD*)pSrc + xx);
				}
				pDest[xx] = c;
			}
			pDest += m_width;
			pSrc += (m_depth >> 3) * m_width;
		}
		Destroy();
		Transfer( newImg );
		SetTransparent( true );
	}
}

bool cImage::Load( LPCSTR fileNameStr, ImageLibFormat format )
{
	FILE *  hFile;
	bool retBool;
	//hFile=fopen(fileNameStr,"rb");
	//if ( (hFile = CreateFile( fileNameStr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL ) ) == INVALID_HANDLE_VALUE ) {
		//return false;
	//}
	hFile=fopen(fileNameStr,"rb");
	if (hFile==NULL)
		return false;
	retBool = Decode( hFile, format );
	//CloseHandle( hFile );	
	fclose(hFile);
	return retBool;
}

bool cImage::Decode( FILE *   hFile, ImageLibFormat format )
{
	if ( hFile == NULL || m_pPixels != NULL || m_pPalette != NULL ) {
		return false;
	}

	switch ( format )
	{
	case IMAGELIB_FORMAT_UNKNOWN:
		{
#if IMAGELIB_SUPPORT_BMP
			if ( Decode( hFile, IMAGELIB_FORMAT_BMP ) ) return true;
#endif
#if IMAGELIB_SUPPORT_GIF
			if ( Decode( hFile, IMAGELIB_FORMAT_GIF ) ) return true;
#endif
#if IMAGELIB_SUPPORT_JPG
			if ( Decode( hFile, IMAGELIB_FORMAT_JPG ) ) return true;
#endif
#if IMAGELIB_SUPPORT_PCX
			if ( Decode( hFile, IMAGELIB_FORMAT_PCX ) ) return true;
#endif
#if IMAGELIB_SUPPORT_PNG
			if ( Decode( hFile, IMAGELIB_FORMAT_PNG ) ) return true;
#endif
#if IMAGELIB_SUPPORT_TGA
			if ( Decode( hFile, IMAGELIB_FORMAT_TGA ) ) return true;
#endif
			break;
		}
#if IMAGELIB_SUPPORT_BMP
	case IMAGELIB_FORMAT_BMP:
		{
			cImageBMP img;			
			if ( img.Decode( hFile ) ) {
				Transfer(img);
				return true;
			}
			break;
		}
#endif
#if IMAGELIB_SUPPORT_GIF
	case IMAGELIB_FORMAT_GIF:
		{
			cImageGIF img;
			if ( img.Decode( hFile ) ) {
				Transfer(img);
				return true;
			}
			break;
		}
#endif
#if IMAGELIB_SUPPORT_JPG
	case IMAGELIB_FORMAT_JPG:
		{
			cImageJPG img;
			if ( img.Decode( hFile ) ) {
				Transfer(img);
				return true;
			}
			break;
		}
#endif
#if IMAGELIB_SUPPORT_PCX
	case IMAGELIB_FORMAT_PCX:
		{
			cImagePCX img;
			if ( img.Decode( hFile ) ) {
				Transfer(img);
				return true;
			}
			break;
		}
#endif
#if IMAGELIB_SUPPORT_PNG
	case IMAGELIB_FORMAT_PNG:
		{
			cImagePNG img;
			if ( img.Decode( hFile ) ) {
				Transfer(img);
				return true;
			}
			break;
		}
#endif
#if IMAGELIB_SUPPORT_TGA
	case IMAGELIB_FORMAT_TGA:
		{
			cImageTGA img;
			if ( img.Decode( hFile ) ) {
				Transfer(img);
				return true;
			}
			break;
		}
#endif
	}
	return false;
}

bool cImage::Save(LPCSTR fileNameStr, ImageLibFormat format )
{
	//HANDLE  hFile;
	bool retBool;
	//if ( (hFile = CreateFile( fileNameStr, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL ) ) == INVALID_HANDLE_VALUE ) {
	//	return false;
	//}
	FILE * hFile=fopen(fileNameStr,"rb");
	if (hFile==NULL)
		return false;
	retBool = Encode( hFile, format );
	//CloseHandle( hFile );	
	fclose(hFile);

	return retBool;
}

bool cImage::Encode( FILE *  hFile, ImageLibFormat format )
{
	if ( hFile == NULL || m_pPixels == NULL ) {
		return false;
	}

	switch ( format )
	{
#if IMAGELIB_SUPPORT_BMP
	case IMAGELIB_FORMAT_BMP:
		{
			cImageBMP img;
			img.Ghost(this);
			if ( img.Encode( hFile ) ) {
				return true;
			}
			break;
		}
#endif
#if IMAGELIB_SUPPORT_GIF
	case IMAGELIB_FORMAT_GIF:
		{
			cImageGIF img;
			img.Ghost(this);
			if ( img.Encode( hFile ) ) {
				return true;
			}
			break;
		}
#endif
#if IMAGELIB_SUPPORT_JPG
	case IMAGELIB_FORMAT_JPG:
		{
			cImageJPG img;
			img.Ghost(this);
			if ( img.Encode( hFile ) ) {
				return true;
			}
			break;
		}
#endif
#if IMAGELIB_SUPPORT_PCX
	case IMAGELIB_FORMAT_PCX:
		{
			cImagePCX img;
			img.Ghost(this);
			if ( img.Encode( hFile ) ) {
				return true;
			}
			break;
		}
#endif
#if IMAGELIB_SUPPORT_PNG
	case IMAGELIB_FORMAT_PNG:
		{
			cImagePNG img;
			img.Ghost(this);
			if ( img.Encode( hFile ) ) {
				return true;
			}
			break;
		}
#endif
#if IMAGELIB_SUPPORT_TGA
	case IMAGELIB_FORMAT_TGA:
		{
			cImageTGA img;
			img.Ghost(this);
			if ( img.Encode( hFile ) ) {
				return true;
			}
			break;
		}
#endif
	}
	return false;
}

void cImage::Transfer( cImage &img )
{
	m_imageFormat		= img.m_imageFormat;
	m_pixelFormat		= img.m_pixelFormat;
	m_width				= img.m_width;
	m_height			= img.m_height;
	m_depth				= img.m_depth;
	m_paletteCount		= img.m_paletteCount;
	m_transparentBool	= img.m_transparentBool;
	m_pPixels			= img.m_pPixels;
	m_pPalette			= img.m_pPalette;
	img.m_pPixels		= NULL;
	img.m_pPalette		= NULL;
}

void cImage::Ghost( cImage *img )
{
	if ( img == NULL )
		return;

	m_ghost				= true;
	m_imageFormat		= img->m_imageFormat;
	m_pixelFormat		= img->m_pixelFormat;
	m_width				= img->m_width;
	m_height			= img->m_height;
	m_depth				= img->m_depth;
	m_paletteCount		= img->m_paletteCount;
	m_transparentBool	= img->m_transparentBool;
	m_pPixels			= img->m_pPixels;
	m_pPalette			= img->m_pPalette;
}

void cImage::SetPalette( RGBQUAD *color, int colorSize )
{
	if ( m_pPalette ) {
		memcpy( m_pPalette, color, sizeof(RGBQUAD) * min(colorSize, m_paletteCount) );
	}
}

void cImage::SetPalette( BGRCOLOR *color, int colorSize )
{
	if ( m_pPalette ) {
		for ( int i = 0; i < min(m_paletteCount, colorSize); i++ ) {
			m_pPalette[i].rgbBlue	= color[i].r;
			m_pPalette[i].rgbGreen	= color[i].g;
			m_pPalette[i].rgbRed	= color[i].b;
		}
	}
}

void cImage::SetPaletteColor( DWORD i, BYTE r, BYTE g, BYTE b )
{
	if ( m_pPalette && i < (DWORD)m_paletteCount ) {
		m_pPalette[i].rgbBlue = b;
		m_pPalette[i].rgbGreen = g;
		m_pPalette[i].rgbRed = r;
		m_pPalette[i].rgbReserved = 0xFF;
	}
}

void cImage::SetPaletteAlpha( BYTE *table, int num )
{
	if ( m_pPalette && table ) {
		for ( int i = 0; i < min(m_paletteCount, num); i++ ) {
			m_pPalette[i].rgbReserved = table[i];
		}
	}
}

void cImage::SetGrayPalette()
{
	if ( m_pPalette == NULL ) return;
	for ( int i = 0; i < m_paletteCount; i++ ) {
		m_pPalette[i].rgbBlue = m_pPalette[i].rgbGreen = m_pPalette[i].rgbRed = (i * 255) / (m_paletteCount - 1);
		m_pPalette[i].rgbReserved = 0xFF;
	}
}

