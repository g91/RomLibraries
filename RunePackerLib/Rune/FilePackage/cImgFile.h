#ifndef CLASS_IMGFILE_H
#define CLASS_IMGFILE_H

#include "cFileObj.h"
#include "../dxtlib/dxtutility.h"

/*
#ifdef _DEBUG
  #pragma comment(lib, "FilePackage\\dxtlib\\dxtlib_d.lib")
#else
  #pragma comment(lib, "FilePackage\\dxtlib\\dxtlib.lib")
#endif */ /*_DEBUG*/

#define RGBToR5G6B5(r,g,b) (WORD) (((r) & 0xF8) << 8 | ((g) & 0xFC) << 3 | ((b) & 0xF8) >> 3)
#define RGBToR5G5B5(r,g,b) (WORD) (((r) & 0xF8) << 7 | ((g) & 0xF8) << 2 | ((b) & 0xF8) >> 3)

enum ImageFormat
{
	IMAGE_UNKNOWN		= 0,

	IMAGE_R5G6B5,
	IMAGE_A1R5G5B5,
	IMAGE_R8G8B8,
	IMAGE_A8R8G8B8,

	IMAGE_DXT1,
	IMAGE_DXT1a,
	IMAGE_DXT3,
	IMAGE_DXT5,
};

enum
{
	IMGFILE_ROP_OVERLAY		= DXT_ROP_OVERLAY,
	IMGFILE_ROP_SELECTSRC	= DXT_ROP_SELECTSRC,	
	IMGFILE_ROP_ADD			= DXT_ROP_ADD,			// 未實做,有空在撰寫
	IMGFILE_ROP_LAST,
};

// 載入圖形檔案時的參數設定
typedef struct ReadImageOptions
{
	ReadImageOptions()
	{
		fmt = IMAGE_A8R8G8B8;
		mipmapBool = false;
	};

	ImageFormat fmt;				// 指定圖形格式
	bool mipmapBool;				// 是否做MIPMAP處理
}ReadImageOptions;

inline bool IsDXTFormat( ImageFormat format )
{
	if ( format == IMAGE_DXT1 ) return true;
	else if ( format == IMAGE_DXT1a ) return true;
	else if ( format == IMAGE_DXT3 ) return true;
	else if ( format == IMAGE_DXT5 ) return true;
	return false;
}

inline int GetPower2(int size)
{
	int pow = (size > 0) ? 1 : 0;
	for ( ; pow < size; pow <<= 1 ) ;
	return pow;
}
inline bool CheckExtensionName( const char *fileNameStr, const char *extensionNameStr  )
{
	if ( fileNameStr == NULL ) return false;
	int len = strlen( fileNameStr );
	if ( len < 4 ) return false;
	if ( strcmpi( &fileNameStr[len - 4], extensionNameStr ) == 0 ) return true;	
	return false;
}

inline bool IsImageFile( const char  *fileNameStr )
{
	if ( fileNameStr == NULL ) return false;
	int len = (int)strlen( fileNameStr );
	if ( len < 4 ) return false;
	if ( strcmpi( &fileNameStr[len - 4], ".BMP" ) == 0 ) return true;
	//else if ( lstrcmpi( &fileNameStr[len - 4], _T(".GIF") ) == 0 ) return true;
	else if ( strcmpi( &fileNameStr[len - 4], ".JPG" ) == 0 ) return true;
	else if ( strcmpi( &fileNameStr[len - 4], ".PCX" ) == 0 ) return true;
	else if ( strcmpi( &fileNameStr[len - 4], ".PNG" ) == 0 ) return true;
	else if ( strcmpi( &fileNameStr[len - 4], ".TGA" ) == 0 ) return true;
	else if ( strcmpi( &fileNameStr[len - 4], ".DDS" ) == 0 ) return true;
	else if ( strcmpi( &fileNameStr[len - 4], ".RDT" ) == 0 ) return true;
	return false;
}

inline int CountImageSize(ImageFormat format, int minmapLevels, int width, int height)
{
	int dataSize = 0;
	minmapLevels = max(minmapLevels, 1);

	for ( int i = 0; i < minmapLevels; i++, width >>= 1, height >>= 1 )
	{
		switch (format)
		{
		case IMAGE_R5G6B5:
		case IMAGE_A1R5G5B5:
			dataSize += max(width, 1) * max(height, 1) * 2;
			break;

		case IMAGE_R8G8B8:
			dataSize += max(width, 1) * max(height, 1) * 3;
			break;

		case IMAGE_A8R8G8B8:
			dataSize += max(width, 1) * max(height, 1) * 4;
			break;

		case IMAGE_DXT1:
		case IMAGE_DXT1a:
			dataSize += max(width, MIN_UNIT) * max(height, MIN_UNIT) / 2;
			break;

			//case IMAGE_DXT2:
		case IMAGE_DXT3:
			//case IMAGE_DXT4:
		case IMAGE_DXT5:
			dataSize += max(width, MIN_UNIT) * max(height, MIN_UNIT);
			break;
		}
	}

	return dataSize;

}

class cImgFile : public cFileObj
{
public:
	cImgFile();
	virtual ~cImgFile();

	/**
	* 釋放所有資料
	*/
	void Destroy();

	/**
	* 取得圖形格式
	*/
	ImageFormat GetImageFormat( void ) { return m_imageFormat; }

	/**
	* 取得圖形尺寸大小
	*
	* @return 傳回寬度值
	*/
	int GetXSize( void ) { return m_width; }

	/**
	* 取得圖形尺寸大小
	*
	* @return 傳回高度值
	*/
	int GetYSize( void ) { return m_height; }

	/**
	* 取得MIPMAP層級數量
	*
	* @return 傳回層級數量
	*/
	int GetMipmapLevels( void ) { return m_mipmapLevels; }

	/**
	* 取得圖形資料量大小
	*
	* @return 傳回圖形資料量大小,單位BYTE
	*/
	int GetPixelsSize( void ) { return m_dataSize; }

	/**
	* 取得圖形資料
	*
	* @return 傳回圖形資料指標
	*/
	BYTE *GetPixels( void ) { return m_pData; }

	/**
	* 從檔案內讀取資料
	*
	* @param workDirStr  工作路徑
	* @param fileNameStr  檔案名稱
	* @param format  檔案壓縮格式
	* @param pSpecialData  擴充用參數,指標指向ReadImageOptions結構
	*
	* @return true 成功  false 失敗
	*/
	virtual bool LoadFromFile( const char *workDirStr, const char *fileNameStr, CompressFormat format = COMPRESS_FMT_NORMAL, void *pSpecialData = NULL );	
	virtual bool ReduceMinmapLevel(int reduceLevel);
	virtual bool CreateImage(ImageFormat format, int minmapLevels, int width, int height);
	int GetReduceMipmapLevels( int index = 0 );
protected:

	/**
	* 讀取圖形檔案資料
	*
	* @param fileNameStr   檔案名稱
	* @param pSpecialData  載入圖形參數設定(ReadImageOptions)
	*
	* @return  true 成功 false 失敗
	*/
	virtual bool Read( const char *fileNameStr, ReadImageOptions *pSpecialData );

	/**
	* 讀取DDS圖形檔案
	*
	* @param fileNameStr   檔案名稱
	* @param pSpecialData  載入圖形參數設定(ReadImageOptions)
	*
	* @return  true 成功 false 失敗
	*/
	virtual bool ReadDDS( const char *fileNameStr, ReadImageOptions *pSpecialData );

public:


	/**
	* 轉換壓縮格式
	*
	* @param fileFormat  指定新壓縮格式
	*
	* @return true 成功  false 失敗
	*/
	virtual bool ConvertFormat( CompressFormat fileFormat );

//		virtual int LoadFromData( HANDLE hFile, BOOL loadDataBool = true );
	/**
	* 拷貝圖形資料至指定記憶體內,目的記憶體不可為DXT壓縮格式
	*
	* @param pDest  目的記憶體指標
	* @param format  目的記憶體圖形格式,不並不支援DXT格式
	* @param x  複制到目的圖形座標
	* @param y  複制到目的圖形座標
	* @param width  目的圖形寬度
	* @param height  目的圖形高度
	*
	* @return  true 成功  false 失敗
	*/
	bool BltToRGB( void *pDest, ImageFormat format, int x, int y, int width, int height, int pitch, int mipmapLevel = -1 );

	/**
	* 拷貝DXT圖形資料至指定記憶體內,目的記憶體需為DXT格式
	*
	* @param pDest  目的記憶體指標
	* @param format  目的記憶體圖形格式,必需為DXT格式
	* @param x  複制到目的圖形座標
	* @param y  複制到目的圖形座標
	* @param width  目的圖形寬度
	* @param height  目的圖形高度		
	* @param mipmapLevels  指定來源MIPMAP層級,第一層為0,第二層為1,其於依此類堆
	* @param color  預乘顏色值
	* @param rop  貼圖方式
	*
	* @return  true 成功  false 失敗
	*/
	bool BltToDXT( void *pDest, ImageFormat format, int x, int y, int width, int height, int mipmapLevels = 0,
		DWORD color = 0xFFFFFFFF, DWORD rop = IMGFILE_ROP_SELECTSRC );

protected:

	/**
	* 儲存檔頭資料
	*/
	virtual void SaveHeader( cListData &header );

	/**
	* 讀取檔頭資料
	*/
	virtual bool LoadHeader( HANDLE hFile );

	/**
	* 寫入資料
	*/
	virtual void WriteData( HANDLE hFile );



	/**
	* 轉換圖形格式,來源格式需為A8R8G8B8 32Bits
	* 
	* @param pDest  目的記憶體指標
	* @param format  目的圖形格式
	* @param x  目的圖形座標
	* @param y  目的圖形座標
	* @param width  目的圖形寬度
	* @param height  目的圖形高度
	* @param pSrc  來源圖形資料,需為A8R8G8B8格式
	* @param srcWidth  來源圖形寬度
	* @param srcHeight  來源圖形高度
	*
	* @return true 成功  false 失敗
	*/
	bool Transfrom( void *pDest, ImageFormat format, int x, int y, int width, int height, int pitch, void *pSrc, int srcWidth, int srcHeight);


protected:

	ImageFormat	m_imageFormat;				// 圖形格式
	int			m_width;					// 圖形寬度
	int			m_height;					// 圖形高度
	union {
		int m_mipmap;
		struct {
			char	m_mipmapLevels;			// mipmap數量 (0或1都代表沒有mipmap)
			char	m_reduceMipmap[3];		// 縮滅 mipmap 各數
		};
	};
};

#endif CLASS_IMGFILE_H