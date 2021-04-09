#include "cImageBMP.h"

#if IMAGELIB_SUPPORT_BMP

bool cImageBMP::Decode( FILE *fptr )
{
	if ( fptr == NULL ) return false;

	BITMAPFILEHEADER bf;
	fseek( fptr, 0, SEEK_SET );
	fread( &bf, min(14, sizeof(bf)), 1, fptr );

	if ( bf.bfType != BFT_BITMAP ) return false;			// 檢查是否為BMP格式


	// 圖形資料
	BITMAPINFOHEADER info;
	fread( &info, sizeof(info), 1, fptr );

	if ( info.biCompression != BI_RGB ) return false;		// 暫時不直援RLE壓縮

	if ( !Create( info.biWidth, info.biHeight, info.biBitCount, IMAGELIB_FORMAT_BMP ) ) {
		return false;
	}
	
	switch ( info.biBitCount )
	{		
		// 24Bits Color 像素排列為BGR,BGR,BGR...
		case 24:
		{
			int lineBytes = ALIGN_LONG(info.biWidth * 3);
			BYTE *pTmp = new BYTE[lineBytes * m_height];

			if ( bf.bfOffBits != 0 ) fseek( fptr, bf.bfOffBits, SEEK_SET );
			fread( pTmp, lineBytes * m_height, 1, fptr );
			m_pixelFormat = PF_R8G8B8;
			for ( int yy = 0; yy < m_height; yy++ ) {
				memcpy( m_pPixels + (yy * m_width * 3), pTmp + (m_height - yy - 1) * lineBytes, m_width * 3 );
			}
			delete [] pTmp;
			break;
		}
		case 8:
		case 4:
		case 1:
		{
			int lineBytes = ALIGN_LONG( info.biWidth / (8 / info.biBitCount) );
			BYTE *pTmp = new BYTE[lineBytes * m_height];
			m_paletteCount = info.biClrUsed;
			fread( m_pPalette, info.biClrUsed, sizeof(RGBQUAD), fptr );

			if ( bf.bfOffBits != 0 ) fseek( fptr, bf.bfOffBits, SEEK_SET );
			fread( pTmp, lineBytes * m_height, 1, fptr );
			m_pixelFormat = PF_I8;
			switch ( info.biBitCount )
			{
				case 8:
				{				
					for ( int yy = 0; yy < m_height; yy++ ) {
						memcpy( m_pPixels + (yy * m_width), pTmp + (m_height - yy -1) * lineBytes, m_width );
					}
					break;
				}
				case 4:
				{
					BYTE *pDest = m_pPixels;
					BYTE *pSrc = pTmp + (m_height - 1) * lineBytes;
					for ( int yy = 0; yy < m_height; yy++ ) {
						for ( int xx = 0; xx < m_width; xx += 2 ) {
							pDest[xx]		= pSrc[xx >> 1] & 0x0F;
							pDest[xx + 1]	= pSrc[xx >> 1] >> 4;
						}
						pDest += m_width;
						pSrc -= lineBytes;
					}
					break;
				}
				case 1:
				{
					BYTE *pDest = m_pPixels;
					BYTE *pSrc = pTmp + (m_height - 1) * lineBytes;
					for ( int yy = 0; yy < m_height; yy++ ) {
						for ( int xx = 0; xx < m_width; xx += 8 ) {
							pDest[xx]		= pSrc[xx >> 3] & 0x01;
							pDest[xx + 1]	= (pSrc[xx >> 3] >> 1) & 0x01;
							pDest[xx + 2]	= (pSrc[xx >> 3] >> 2) & 0x01;
							pDest[xx + 3]	= (pSrc[xx >> 3] >> 3) & 0x01;
							pDest[xx + 4]	= (pSrc[xx >> 3] >> 4) & 0x01;
							pDest[xx + 5]	= (pSrc[xx >> 3] >> 5) & 0x01;
							pDest[xx + 6]	= (pSrc[xx >> 3] >> 6) & 0x01;
							pDest[xx + 7]	= (pSrc[xx >> 3] >> 7) & 0x01;
						}
						pDest += m_width;
						pSrc -= lineBytes;
					}
					break;
				}
			}

			delete [] pTmp;
			break;
		}
		default:
		{		
			Destroy();
			return false;
		}
	}
	
	return true;
}

#endif //IMAGELIB_SUPPORT_BMP