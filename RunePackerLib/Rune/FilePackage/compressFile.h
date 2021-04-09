#ifndef COMPRESS_FILE_H
#define COMPRESS_FILE_H

enum CompressFormat
{
	COMPRESS_FMT_NORMAL		= 0,
	COMPRESS_FMT_RLE,
	COMPRESS_FMT_LZO,
	COMPRESS_FMT_ZLIB,
	COMPRESS_FMT_RDT,
	COMPRESS_FMT_LAST,
}; 

/**
* ���Y���
*/
void* CompressFile( void *pSrc, int size, CompressFormat format, int &compressSize );

/**
* �������
*/
int DecompressFile( void *pSrc, int compressSize, CompressFormat format, void *pDest, int destSize );

#endif //COMPRESS_FILE_H