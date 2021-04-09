#pragma once

#include "imagedef.h"

#define IMAGELIB_SUPPORT_BMP		1
#define IMAGELIB_SUPPORT_GIF		0
#define IMAGELIB_SUPPORT_JPG		1
#define IMAGELIB_SUPPORT_PCX		1
#define IMAGELIB_SUPPORT_PNG		1
#define IMAGELIB_SUPPORT_TGA		1

enum ImageLibFormat
{
	IMAGELIB_FORMAT_UNKNOWN	= 0,
#if IMAGELIB_SUPPORT_BMP
	IMAGELIB_FORMAT_BMP,
#endif
#if IMAGELIB_SUPPORT_GIF
	IMAGELIB_FORMAT_GIF,
#endif
#if IMAGELIB_SUPPORT_JPG
	IMAGELIB_FORMAT_JPG,
#endif
#if IMAGELIB_SUPPORT_PCX
	IMAGELIB_FORMAT_PCX,
#endif
#if IMAGELIB_SUPPORT_PNG
	IMAGELIB_FORMAT_PNG,
#endif
#if IMAGELIB_SUPPORT_TGA
	IMAGELIB_FORMAT_TGA,
#endif
	IMAGELIB_LAST,
};

#if !defined(PIXEL_FORMAT)
	#define PIXEL_FORMAT
	enum PixelFormat
	{
		PF_UNKNOWN			= 0,
		PF_I8,		
		PF_R8G8B8,
		PF_A8R8G8B8,
		PF_LAST,
	};
#endif

extern IMAGE_DECL BYTE g_5BitsTo8BitsArray[32];
extern IMAGE_DECL BYTE g_6BitsTo8BitsArray[64];

typedef struct tagBGRCOLOR
{
	BYTE r, g, b;
}BGRCOLOR;

inline void RGB2BGR( BYTE* pDes, BYTE* pSrc, int size )
{
	for ( int i = 0; i < size; i++ ) {
		pDes[3*i]	= pSrc[3*i+2];
		pDes[3*i+1]	= pSrc[3*i+1];
		pDes[3*i+2]	= pSrc[3*i];
	}
}

class IMAGE_DECL cImage
{
public:
	cImage();
	virtual ~cImage();

	/**
	* 取得圖形格式
	* @return 傳回圖形格式
	*/
	ImageLibFormat GetFormat() { return m_imageFormat; }

	/**
	* 取得圖形寬度
	* @return 傳回圖寬
	*/
	int GetXSize() { return m_width; }

	/**
	* 取得圖形高度
	* @return 傳回圖高
	*/
	int GetYSize() { return m_height; }

	/**
	* 取得圖形每一像點所需bits
	@ return 8 or 24 or 32
	*/
	int GetDepth() { return m_depth; }

	/**
	* 取得調色盤顏色總數
	* @return 調色盤總數
	*/
	int GetPaletteCount() { return m_paletteCount; }

	/**
	* 是否函有透明資訊(8 or 32 Bits Color)
	* @return true 有透明資訊 false 無透明資訊
	*/
	bool IsTransparent() { return m_transparentBool; }

	/**
	* 取得圖形指標
	* @return 傳回圖形指標位置
	*/
	BYTE *GetPixels() { return m_pPixels; }

	/**
	* 取得調色盤指標
	* @return 傳回色盤指標
	*/
	RGBQUAD *GetPalette() { return m_pPalette; }

	/**
	* 釋放所有圖形資料
	*/
	void Destroy();

	/**
	* 建立新的空白圖形資料,會依據所傳入的參數,配置適合的記憶體空間
	* @param width  圖形寬度
	* @param height  圖形高度
	* @param depth  圖形深度(8, 24, 32 Bits)
	* @param format  圖形來源格式
	* @return true 建立成功  false 失敗
	*/
	bool Create( int width, int height, int depth, ImageLibFormat format = IMAGELIB_FORMAT_UNKNOWN );	

	/**
	* 鏡射處理
	* @return true 成功  false 失敗
	*/
	bool Mirror();

	/**
	* 轉換圖形格式,無法轉換為256色
	* @param format  目的顏色
	* @return true 成功  false 失敗
	*/
	bool ConvertFormat( PixelFormat format );

	/**
	* 複制圖形資料,需要相同格式才能做用
	* @param srcImage 來源圖形資料
	* @param x 複制到目的開始座標
	* @param y 複制到目的開始座標
	* @return true 建立成功  false 失敗
	*/
	bool Copy( cImage* srcImage, int x, int y );

protected:
	
	void ConvertToR5G6B5();
	void ConvertToA1R5G5B5();
	void ConvertToR8G8B8();
	void ConvertToA8R8G8B8();

public:

	/**
	* 載入圖形檔
	* @param fileNameStr  圖形檔案名稱
	* @param format  指定來源圖形格式
	* @return  true 載入成功  false 載入失敗
	*/
	bool Load( LPCSTR fileNameStr, ImageLibFormat format = IMAGELIB_FORMAT_UNKNOWN );
	bool Decode( FILE *  file, ImageLibFormat format );

	/**
	* 存儲檔案
	* @param fileNameStr  圖形檔案名稱
	* @param format  指定目的圖形格式(目前支援TGA)
	* @return  true 儲檔成功  false 儲檔失敗
	*/
	bool Save( LPCSTR fileNameStr, ImageLibFormat format = IMAGELIB_FORMAT_TGA );	
	bool Encode(FILE *  file, ImageLibFormat format );

protected:

	void SetTransparent( bool transparentBool ) { m_transparentBool = transparentBool; }
	void Transfer( cImage &img );
	void Ghost( cImage *img );
	void SetPalette( RGBQUAD *color, int colorSize );	
	void SetPalette( BGRCOLOR *color, int colorSize );
	void SetPaletteColor( DWORD i, BYTE r, BYTE g, BYTE b );
	void SetPaletteAlpha( BYTE *table, int num );
	void SetGrayPalette();

protected:

	ImageLibFormat m_imageFormat;	// 來源圖形格式
	PixelFormat m_pixelFormat;		// 來源像點格式
	int m_width;					// 圖形尺寸
	int m_height;					// 圖形尺寸
	int m_depth;					// 每一像素所需位元(2,4,8,16,24,32)
	int m_paletteCount;				// 調色盤顏色總數
	bool m_transparentBool;			// 是否有透明色
	BYTE *m_pPixels;				// 像素資料
	RGBQUAD *m_pPalette;			// 調色盤資料

	bool m_ghost;					// 並非實體,資料為複制而來
};