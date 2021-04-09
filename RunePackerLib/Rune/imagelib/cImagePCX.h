#ifndef CLASS_IMAGE_PCX_H
#define CLASS_IMAGE_PCX_H

#include "cimage.h"

#if IMAGELIB_SUPPORT_PCX

class cImagePCX : public cImage
{	
#pragma pack(1)
// PCX 檔頭資訊	
typedef struct tagPcxHeader
{
	char bManufacturer;			//Zsoft PCX檔的識別碼 0Ah
	char bVersionNo;			//版本編號,一般都為 5
	char bEncoding;				//編碼方法,目前只有 1
	char bBitsPerPixel;			//每個 plane上的 bits/plane數
	short shXmin;				//圖面的左上角的 X 座標
	short shYmin;				//圖面的左上角的 Y 座標
	short shXmax;				//圖面的右下角的 X 座標
	short shYmax;				//圖面的右下角的 Y 座標
	short shHResolution;		//原先產生此圖面的螢幕之水平解析度(dots/inch)
	short shVResolution;		//原先產生此圖面的螢幕之垂平解析度(dots/inch)
	char bHeaderPalette[48];	//檔頭調色盤(16色系使用)
	char bResered;				//規定為 0
	char bPlanes;				//此圖檔模式中 bit plane的數量
	short shBytesPerLine;		//在一個 bit plane上每一條水平線所佔的char數
	short shPaletteType;		//調色盤的種類,一般為 1
	short shHScreenSize;		//螢幕的水平 pixel 數減 1
	short shVScreenSize;		//螢幕的垂直 pixel 數減 1
	char bfiller[54];			//保留不用
}PCXHEADER;
#pragma pack()

public:
	bool Decode( FILE *fptr );
	bool Encode( FILE *fptr )	{ return false; }

protected:
	int Compress( BYTE *pSrc, BYTE *pDest, int size );
	int Decompress( BYTE *pSrc, BYTE *pDest, int size );		
};

#endif


#endif //CLASS_IMAGE_PNG_H