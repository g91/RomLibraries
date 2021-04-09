#ifndef CLASS_IMAGE_TGA_H
#define CLASS_IMAGE_TGA_H

#include "cImage.h"

#if IMAGELIB_SUPPORT_TGA

#pragma pack(1)
// TGA檔頭資訊
typedef struct tagTgaHeader
{
	BYTE bFieldLength;				// 圖面文字註譯欄長度
	BYTE bColorMapType;				// 顏色表使用狀態
	BYTE bImageDataType;			// 圖面儲存方式	
	WORD wColorMapFirstIndex;		// 顏色表第一個Entry代號
	WORD wColorMapSize;				// 顏色表的Entry總數
	BYTE bColorMapEntry;			// 顏色表中Entry大小(15,16,24,32)
	WORD wOriginX;					// 圖面左上角X座標值
	WORD wOriginY;					// 圖面左上角Y座標值
	WORD wWidth;					// 圖形寬度
	WORD wHeight;					// 圖形高度
	BYTE bDepth;					// 圖點Bit數
	BYTE bDescriptor;				// 圖形描述資訊
}TGAHEADER;
#pragma pack()

// TGA檔尾資訊
typedef struct tagTgaFooter
{
	int iExtensionOffset;				// Extension 位移值
	int iDeveloperOffset;				// Developer Directory 位移值
	BYTE szSignature[18];				// 新版TGA識別字串
}TGAFOOTER;

class cImageTGA : public cImage
{
public:
	bool Decode( FILE *fptr );
	bool Encode( FILE *fptr );	

protected:

	void Load8Bits( BYTE *pData, TGAHEADER *pInfo, bool compressedBool );
	void Load16Bits( BYTE *pData, TGAHEADER *pInfo, bool compressedBool );

	void Save8Bits( FILE* fptr );
	void Save32Bits( FILE* fptr );

	/**
	* 壓縮資料
	* @param pSrc 來源圖形指標
	* @param pDest 存放壓縮後目的指標
	* @param size 來源圖形大小(單位:Byte)
	* @param bpp 每一像點存放資料大小(單位:Byte)
	* @return : 壓縮後資料大小(Byte)
	*/
	int Compress( BYTE *pSrc, byte *pDest, int size, int bpp );

	/**
	* 解壓縮資料
	* @param pSrc 來源壓縮過圖形指標
	* @param pDest 解開後目的存放指標,需要先開好記憶體位置
	* @param size 解開後圖形資料大小(單位:Byte)
	* @param bpp 每一像點存放資料大小(單位:Byte)
	* @return 共讀取多少BYTE資料
	*/
	int Decompress( BYTE* pSrc, BYTE* pDest, int size, int bpp );
};

#endif

#endif // CLASS_IMAGETGA_H