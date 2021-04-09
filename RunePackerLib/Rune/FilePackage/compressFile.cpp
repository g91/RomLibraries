#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "../zlib/zlib.h"
#include "../lzo/config1x.h"
#include "compressFile.h"

/*
#ifdef _DEBUG
  #pragma comment(lib, "filepackage\\zlib\\zlib_d.lib")
  #pragma comment(lib, "filepackage\\lzopack\\lzo_d.lib")
#else
  #pragma comment(lib, "filepackage\\zlib\\zlib.lib")
  #pragma comment(lib, "filepackage\\lzopack\\lzo.lib")
#endif */ /*_DEBUG*/

// -------------------------------------------------------------------------------
//rle 壓縮
//
//  傳入 : 原始資料 buffer , 原始資料buffer大小 , 壓縮後大小
//
//  傳回 : 壓縮後之資料 buffer 
//
void* CompressRLE( BYTE *pSrc, int size, int &compressSize )
{
	BYTE *tmp = new BYTE[size + (size >> 1)];
	BYTE buf[128];
	int count = 0, not = 0, repeat = 1;

	for ( int i = 0; i < size; i++ ) {
		if ( i != size - 1 && pSrc[i] == pSrc[i + 1] ) {
			repeat++;
		} else if ( repeat > 1 ) {
			if ( not > 0 ) {
				tmp[count++] = not - 1;
				memcpy( tmp + count, buf, not );
				count += not;
				not = 0;
			}

			for ( int j = 0; j < repeat/128; j++ ) {
				tmp[count++] = 0xFF;
				tmp[count++] = pSrc[j];
			}

			if ( (repeat%128) != 0 ) {
				tmp[count++] = 0x80 | ((repeat % 128) - 1);
				tmp[count++] = pSrc[i];
			}
			repeat = 1;
		} else {
			buf[not++] = pSrc[i];
			if ( not > 127 ) {
				tmp[count++] = not - 1;
				memcpy( tmp + count, buf, not );
				count += not;
				not = 0;
			}
		}
	}

	if (not>0) {
		tmp[count++] = not - 1;
		memcpy( tmp + count, buf, not );
		count += not;
	}
	compressSize = count;
	return tmp;
}

// ----------------------------------------------------------------------------
//RLE 解壓縮
// pSrc 來源壓縮資料指標
// pDes 目的存放資料指標
// iSize 來源壓縮資料大小
// return 解壓縮後資料大小
int DecompressRLE( BYTE* pSrc, int compressSize, BYTE* pDest )
{
	int reSize = 0;
	int len = 0;
	int repeat;

	while( len < compressSize ) {
		repeat = (pSrc[len] & 0x7F) + 1;
		if ( pSrc[len++] & 0x80 ) {
			memset( &pDest[reSize], pSrc[len++], repeat );
			reSize += repeat;
		} else {
			memcpy( &pDest[reSize], &pSrc[len], repeat );
			reSize += repeat;
			len += repeat;
		}
	}

	return reSize;
}


// -------------------------------------------------------------------------------
//lzo 壓縮
//
//  傳入 : 原始資料 buffer , 原始資料buffer大小 , 壓縮後大小
//
//  傳回 : 壓縮後之資料 buffer 
//
void* CompressLZO( void *pSrc, int srcLen, int &compressSize )
{	
	lzo_byte    *lzo_d = (lzo_byte*) new BYTE[srcLen+(srcLen>>1)+12];
	lzo_voidp   wrkMem = (lzo_voidp) new BYTE[LZO1X_1_MEM_COMPRESS];
	lzo_uint	size;

	lzo1x_1_compress((lzo_byte*) pSrc ,srcLen ,lzo_d , &size ,wrkMem);

	compressSize = size;
	delete [] wrkMem;
	return (void*)lzo_d;
}

// -------------------------------------------------------------------------------
// zlib 壓縮
//
//  傳入 : 原始資料 buffer , 原始資料buffer大小 , 壓縮後大小
//
//  傳回 : 壓縮後之資料 buffer 
//
void* CompressZLIB( BYTE *pSrc, int size, int &compressSize )
{
	z_stream zStream;
	DWORD outputSize = 0;
	DWORD tmpSize = size + (size >> 1);
	Bytef *tmp = new Bytef[tmpSize];
	memset(&zStream, 0, sizeof(z_stream));
	deflateInit(&zStream, Z_DEFAULT_COMPRESSION);

	zStream.next_in = pSrc;				// 來源資料指標
	zStream.avail_in = size;			// 來源大小
	zStream.next_out = tmp;				// 輸出暫存指標
	zStream.avail_out = tmpSize;		// 輸出暫存資料大小

	do
	{
		switch( deflate(&zStream, Z_SYNC_FLUSH) )
		{
		case Z_OK:				// Some progress made
			break;

		case Z_STREAM_END:		// Compression of the entire block is done
			break;

		default:
		case Z_BUF_ERROR:
		case Z_STREAM_ERROR:
			deflateEnd(&zStream);
			return 0;
		}

		/*
		if(zStream.avail_out < desSize)
		{
			outputSize += zStream.avail_out;
			zStream.next_out = pDest;
			zStream.avail_out = desSize;
		}
		*/		

		// Save available output
		if( zStream.avail_out < tmpSize ) {
			outputSize += (tmpSize - zStream.avail_out);
			zStream.next_out = tmp;
			zStream.avail_out = tmpSize;
		}

	} while(zStream.avail_in > 0);

	deflateEnd(&zStream);
	compressSize = outputSize;
	return tmp;
}

// -------------------------------------------------------------------------------
// zlib 解壓縮
//
//  傳入 : 原始資料 buffer , 原始資料buffer大小 , 壓縮後大小
//
//  傳回 : 壓縮後之資料 buffer 
//
int DecompressZLIB( BYTE *pSrc, int size, BYTE *pDest, int desSize )
{
	z_stream zStream;
	DWORD outputSize = 0;
	memset(&zStream, 0, sizeof(z_stream));
	inflateInit(&zStream);

	zStream.next_in = pSrc;
	zStream.avail_in = size;
	zStream.next_out = pDest;
	zStream.avail_out = desSize;

	do
	{
		switch(inflate(&zStream, Z_SYNC_FLUSH))
		{
		case Z_OK:				// Some progress made
			break;

		case Z_STREAM_END:		// Compression of the entire block is done
			break;

		default:
		case Z_BUF_ERROR:
		case Z_STREAM_ERROR:
			inflateEnd(&zStream);
			return 0;
		}

		if(zStream.avail_out < (DWORD)desSize)
		{
			outputSize += ( desSize - zStream.avail_out );
			zStream.next_out = pDest;
			zStream.avail_out = desSize;
		}

	} while(zStream.avail_in > 0);

	inflateEnd(&zStream);
	return outputSize;
}

void* CompressFile( void *pSrc, int size, CompressFormat format, int &compressSize )
{
	void *pCompressData;
	if ( pSrc == NULL || size <= 0 ) {
		return NULL;
	}

	switch ( format )
	{		
		case COMPRESS_FMT_RLE:
		{
			pCompressData = CompressRLE( (BYTE*)pSrc, size, compressSize );
			break;
		}			
		case COMPRESS_FMT_LZO:
		{
			pCompressData = CompressLZO( pSrc, size, compressSize );
			break;
		}			
		case COMPRESS_FMT_ZLIB:
		{
			pCompressData = CompressZLIB( (BYTE*)pSrc, size, compressSize );
			break;
		}
		default:
			return NULL;
	}
	return pCompressData;
}

int DecompressFile( void *pSrc, int compressSize, CompressFormat format, void *pDest, int destSize )
{
	int size;
	if ( pSrc == NULL || compressSize <= 0 ) {
		return 0;
	}

	switch ( format )
	{
		case COMPRESS_FMT_RLE:
		{		
			size = DecompressRLE( (BYTE*)pSrc, compressSize, (BYTE*)pDest );
			break;
		}
		case COMPRESS_FMT_LZO:
		{
			if ( lzo1x_decompress( (lzo_byte*)pSrc, compressSize, (lzo_byte*)pDest, (lzo_uint*)&destSize, NULL ) != LZO_E_OK ) {
				size = 0;
			} else {
				size = destSize;
			}
			break;
		}
		case COMPRESS_FMT_ZLIB:
		{
			size = DecompressZLIB( (BYTE*)pSrc, compressSize, (BYTE*)pDest, destSize );
			break;
		}
		default:
			return 0;
	}
	return size;
}

