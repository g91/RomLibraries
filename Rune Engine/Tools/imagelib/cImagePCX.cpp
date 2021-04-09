#include "cImagePCX.h"

#if IMAGELIB_SUPPORT_PCX

bool cImagePCX::Decode( FILE *fptr )
{
	if ( fptr == NULL ) return false;
	fseek ( fptr, 0, SEEK_SET );	

	// 取得檔頭
	PCXHEADER header;
	memset(&header, 0, sizeof(header));
	//ZeroMemory(&header, sizeof(header));
	fread( &header, 1, sizeof(PCXHEADER), fptr );

	// 檢查是否PCX檔案
	if ( header.bManufacturer != 0x0A || header.bEncoding != 1 ) return false;	

	int width = header.shXmax - header.shXmin + 1;
	int height = header.shYmax - header.shYmin + 1;
	int depth = header.bBitsPerPixel * header.bPlanes;

	Create( width, height, depth, IMAGELIB_FORMAT_PCX );
	
	// 將檔案載入記憶體內
	int pos = ftell( fptr );
	fseek( fptr, 0, SEEK_END );
	int tmpSize = ftell( fptr ) - pos;
	fseek( fptr, pos, SEEK_SET );
	BYTE *tmp = new BYTE[tmpSize];	
	fread( tmp, tmpSize, 1, fptr );

	// 有調色盤
	if ( header.bPlanes == 1 ) {
		BGRCOLOR *pal = (BGRCOLOR*)(tmp + (tmpSize - 768));
		for ( int i = 0; i < 256; i++ ) {
			SetPaletteColor( i, pal[i].r, pal[i].g, pal[i].b );
		}
	}

	int lineBytes = header.shBytesPerLine * (depth >> 3);
	BYTE *row_pixels = new BYTE[10 + lineBytes];
	BYTE *pDest = GetPixels();
	BYTE *pSrc = tmp;

	for ( int yy = 0; yy < height; yy++ ) {
		pSrc += Decompress( pSrc, row_pixels, lineBytes );

		if ( header.bPlanes == 1 ) {
			memcpy( pDest, row_pixels, width );
			pDest += width;
		} else {
			for ( int i = 0; i < width; i++ ) {
				pDest[3*i]		= row_pixels[2 * header.shBytesPerLine + i];
				pDest[3*i+1]	= row_pixels[header.shBytesPerLine + i];
				pDest[3*i+2]	= row_pixels[i];
			}
			pDest += width * 3;
		}
	}

	delete [] tmp;
	delete [] row_pixels;	

	return true;
}

int cImagePCX::Compress( BYTE *pSrc, BYTE *pDest, int size )
{
	int	count = 0;
	int	repeat = 1;	

	// 以行為單位壓縮圖形為PCX
	for ( int i = 0; i < size ; i++ ) {
		if ( i != size - 1 && pSrc[i] == pSrc[i+1] ) {
			repeat++;
		} else if (repeat > 1) {
			for ( int j = 0 ; j < repeat / 63; j++ ) {
				pDest[count++] = 0xFF;
				pDest[count++] = pSrc[i];
			}
			if ( (repeat%63) != 0 ) {
				pDest[count++] = 0xC0 + repeat%63;
				pDest[count++] = pSrc[i];
			}
			repeat = 1;
		} else if ( pSrc[i] >= 0xC0 ) {
			pDest[count++] = 0xC1;
		} else {					
			pDest[count++] = pSrc[i];
		}
	}

	return count;
}

int cImagePCX::Decompress( BYTE *pSrc, BYTE *pDest, int size )
{
	int repeat;
	int src_count = 0;
	int des_count = 0;
	while (des_count < size) {
		if ( pSrc[src_count] > 0xC0 ) {
			repeat = pSrc[src_count++] & 0x3F;
			memset(&pDest[des_count], pSrc[src_count++], repeat);
			des_count += repeat;
		} else {
			pDest[des_count++] = pSrc[src_count++];
		}
	}
	return src_count;
}

#endif //IMAGELIB_SUPPORT_PCX