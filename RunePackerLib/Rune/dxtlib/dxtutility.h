#ifndef DXT_UTILITY_H
#define DXT_UTILITY_H

#include <dxtlib/dxtlib.h>

#define MIN_UNIT				4			// 最小尺寸單位
#define DXT_HEADER_SIZE			128
#define DXT_1					((DWORD)827611204)
#define DXT_3					((DWORD)861165636)
#define DXT_5					((DWORD)894720068)
#define R5G6B5XR8G8B8(w,r,g,b) (WORD)( ((long)((w) >> 11) * (r) / 255) << 11 | ((long)(((w) >> 5) & 0x3F) * (g) / 255) << 5 | ((long)((w) & 0x1F) * (b) / 255) )

enum
{
	DXT_ROP_DISABLE = 0,			//
	DXT_ROP_OVERLAY,				// 
	DXT_ROP_SELECTSRC,				//
	DXT_ROP_ADD,					// 未實做,有空在撰寫
};

// 顏色值結構
struct DXT_COLOR
{	
	union {
		struct { BYTE b, g, r, a; };
		DWORD Col;
	};
};

// 可存兩種格式 A1R5G5B5 or R5G6B5 兩種
struct BLOCK_DXT1
{
	WORD c[2];					// 主要高低顏色
	DWORD index;				// 索引值
};

// DXT3可存4Bits Alpha
struct BLOCK_DXT3
{
	WORD a[4];					// 前面64個Bits記錄16個像點的Alpha值
	BLOCK_DXT1 block;			// 與DXT1結構相同
};

// DXT5可以存8Bits Line Alpha,跟DXT1 Block有點像
struct BLOCK_DXT5
{
	BYTE a[8];					// 2Bytes高低顏色值,其他6Bytes為索引值(48Bits / 16 = 3Bits)
	BLOCK_DXT1 block;			// 與DXT1結構相同
};

/**
* 取得圖形mipmap數量
*
* @param width  圖形尺寸
* @param height  圖形尺寸
*/
inline int GetMipmapCount( int width, int height ) 
{
	int levels = 0;
	for ( int i = max(width, height); i > 2; i >>= 1 ) levels++;
	return levels;
}

/**
* 解譯DXT圖形格式,包函DDS檔頭資訊
*
* @param pDest  目的記憶體指標,需為32Bits Color
* @param pSrc  來源資料指標,前端包函DDS檔頭資訊
*
* @return  成功 true, 失敗回傳 false
*/
bool DeCompressDXT( BYTE *pDest, BYTE *pSrc );

/**
* 解譯DXT1圖形格式
*
* @param pDest  目的記憶體指標,需為32Bits Color
* @param pSrc  來源資料指標
* @param width  圖形寬度值
* @param height  圖形高度值
*/
void PlotDXT1( BYTE *pDest, BYTE *pSrc, int width, int height );

/**
* 解譯DXT3圖形格式
*
* @param pDest  目的記憶體指標,需為32Bits Color
* @param pSrc  來源資料指標
* @param width  圖形寬度值
* @param height  圖形高度值
*/
void PlotDXT3( BYTE *pDest, BYTE *pSrc, int width, int height );

/**
* 解譯DXT5圖形格式
*
* @param pDest  目的記憶體指標,需為32Bits Color
* @param pSrc  來源資料指標
* @param width  圖形寬度值
* @param height  圖形高度值
*/
void PlotDXT5( BYTE *pDest, BYTE *pSrc, int width, int height );

/**
* 解譯DXT5 Alpha資料,單純只取得Alpha
*
* @param pDest  目的記憶體指標
* @param pSrc  來源記憶體資料
* @param width  寬度值
*/
void PlotDXT5Alpha( DXT_COLOR *pDest, BLOCK_DXT5 *pSrc, int width, int height );

/**
* 將圖形壓縮為DXT格式
*
* @param pImageData  pointer to data (32 bit)
* @param width  圖形尺寸
* @param height  圖形尺寸
* @param compressSize  回傳壓縮後資料大小
* @param mipmapLevels  回傳mipmap levels
* @param pOptions  細部資料設定,請參考"nvdxt_options.h"
*
* @return 成功 傳回非零值代表編碼成功,所傳回的指標需要外部做記憶體釋放  失敗傳回零值
*/
BYTE *CompressDXT( BYTE *pImageData, int width, int height, int &compressSize, int &mipmapLevels, nvCompressionOptions *pOptions = NULL );

/**
* 壓縮單一區塊為DXT格式
*
* @param pImageData  pointer to data (32 bit)
* @param textureFormat  指定DXT格式( kDXT1, kDXT3. kDXT5 )
* @return 成功 傳回非零值代表編碼成功,傳回指標記憶體不用釋放  失敗傳回零值
*/
BYTE *CompressDXTBlock( BYTE *pImageData, nvTextureFormats textureFormat );

/**
* DXT格式轉換
*
* @param pDest  目的記憶體指標
* @param destFormat  目的格式(nvTextureFormats kDXT1, kDXT3, kDXT5)
* @param pSrc  來源記憶體指標
* @param srcFormat  來源格式(nvTextureFormats kDXT1, kDXT3, kDXT5)
* @param width  來源圖形大小
* @param height  來源圖形大小
*/
bool ConvertDXTFromat( void *pDest, DWORD destFormat, void *pSrc, DWORD srcFormat, int width, int height );

/**
* 複制DXT圖形檔案
*
* @param pDest  目的記憶體指標
* @param destFromat  目的圖形格式(nvTextureFormats kDXT1, kDXT3, kDXT5)
* @param x  複制到目的圖形座標
* @param y  複制到目的圖形座標
* @param destWidth  目的圖形大小
* @param destHeight  目的圖形大小
* @param pSrc  來源圖形記憶體指標
* @param srcFormat  來源圖形格式(nvTextureFormats kDXT1, kDXT3, kDXT5)
* @param srcWidth  來源圖形大小
* @param srcHeight  來源圖形大小
* @param color  來源圖形預乘顏色(預設0xffffffff,不做顏色計算)
* @param rop  複制方式(預設
*
* @return true 成功  false 失敗
*/
bool BltDXT( void *pDest, DWORD destFormat, int x, int y, int destWidth, int destHeight, void *pSrc, DWORD srcFormat,
			int srcWidth, int srcHeight, DWORD color=0xffffffff, DWORD rop=DXT_ROP_OVERLAY );

#endif //DXT_UTILITY_H