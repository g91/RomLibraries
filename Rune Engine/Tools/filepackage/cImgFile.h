#ifndef CLASS_IMGFILE_H
#define CLASS_IMGFILE_H

#include "cFileObj.h"
#include "../dxtlib/dxtutility.h"

struct DDCOLORKEYX64
{
	DWORD       dwColorSpaceLowValue;   // low boundary of color space that is to
	// be treated as Color Key, inclusive
	DWORD       dwColorSpaceHighValue;  // high boundary of color space that is
	// to be treated as Color Key, inclusive
};

struct DDPIXELFORMATX64
{
	DWORD       dwSize;                 // size of structure
	DWORD       dwFlags;                // pixel format flags
	DWORD       dwFourCC;               // (FOURCC code)
	union
	{
		DWORD   dwRGBBitCount;          // how many bits per pixel
		DWORD   dwYUVBitCount;          // how many bits per pixel
		DWORD   dwZBufferBitDepth;      // how many total bits/pixel in z buffer (including any stencil bits)
		DWORD   dwAlphaBitDepth;        // how many bits for alpha channels
		DWORD   dwLuminanceBitCount;    // how many bits per pixel
		DWORD   dwBumpBitCount;         // how many bits per "buxel", total
		DWORD   dwPrivateFormatBitCount;// Bits per pixel of private driver formats. Only valid in texture
		// format list and if DDPF_D3DFORMAT is set
	};
	union
	{
		DWORD   dwRBitMask;             // mask for red bit
		DWORD   dwYBitMask;             // mask for Y bits
		DWORD   dwStencilBitDepth;      // how many stencil bits (note: dwZBufferBitDepth-dwStencilBitDepth is total Z-only bits)
		DWORD   dwLuminanceBitMask;     // mask for luminance bits
		DWORD   dwBumpDuBitMask;        // mask for bump map U delta bits
		DWORD   dwOperations;           // DDPF_D3DFORMAT Operations
	};
	union
	{
		DWORD   dwGBitMask;             // mask for green bits
		DWORD   dwUBitMask;             // mask for U bits
		DWORD   dwZBitMask;             // mask for Z bits
		DWORD   dwBumpDvBitMask;        // mask for bump map V delta bits
		struct
		{
			WORD    wFlipMSTypes;       // Multisample methods supported via flip for this D3DFORMAT
			WORD    wBltMSTypes;        // Multisample methods supported via blt for this D3DFORMAT
		} MultiSampleCaps;

	};
	union
	{
		DWORD   dwBBitMask;             // mask for blue bits
		DWORD   dwVBitMask;             // mask for V bits
		DWORD   dwStencilBitMask;       // mask for stencil bits
		DWORD   dwBumpLuminanceBitMask; // mask for luminance in bump map
	};
	union
	{
		DWORD   dwRGBAlphaBitMask;      // mask for alpha channel
		DWORD   dwYUVAlphaBitMask;      // mask for alpha channel
		DWORD   dwLuminanceAlphaBitMask;// mask for alpha channel
		DWORD   dwRGBZBitMask;          // mask for Z channel
		DWORD   dwYUVZBitMask;          // mask for Z channel
	};
};
struct DDSCAPS2X64
{
	DWORD       dwCaps;         // capabilities of surface wanted
	DWORD       dwCaps2;
	DWORD       dwCaps3;
	union
	{
		DWORD       dwCaps4;
		DWORD       dwVolumeDepth;
	};
} ;
struct DDSURFACEDESC2X64
{
	DWORD               dwSize;                 // size of the DDSURFACEDESC structure
	DWORD               dwFlags;                // determines what fields are valid
	DWORD               dwHeight;               // height of surface to be created
	DWORD               dwWidth;                // width of input surface
	union
	{
		LONG            lPitch;                 // distance to start of next line (return value only)
		DWORD           dwLinearSize;           // Formless late-allocated optimized surface size
	};
	union
	{
		DWORD           dwBackBufferCount;      // number of back buffers requested
		DWORD           dwDepth;                // the depth if this is a volume texture 
	};
	union
	{
		DWORD           dwMipMapCount;          // number of mip-map levels requestde
		// dwZBufferBitDepth removed, use ddpfPixelFormat one instead
		DWORD           dwRefreshRate;          // refresh rate (used when display mode is described)
		DWORD           dwSrcVBHandle;          // The source used in VB::Optimize
	};
	DWORD               dwAlphaBitDepth;        // depth of alpha buffer requested
	DWORD               dwReserved;             // reserved
	INT32              lpSurface;              // pointer to the associated surface memory
	union
	{
		DDCOLORKEYX64      ddckCKDestOverlay;      // color key for destination overlay use
		DWORD           dwEmptyFaceColor;       // Physical color for empty cubemap faces
	};
	DDCOLORKEYX64          ddckCKDestBlt;          // color key for destination blt use
	DDCOLORKEYX64          ddckCKSrcOverlay;       // color key for source overlay use
	DDCOLORKEYX64          ddckCKSrcBlt;           // color key for source blt use
	union
	{
		DDPIXELFORMATX64   ddpfPixelFormat;        // pixel format description of the surface
		DWORD           dwFVF;                  // vertex format description of vertex buffers
	};
	DDSCAPS2X64            ddsCaps;                // direct draw surface capabilities
	DWORD               dwTextureStage;         // stage in multitexture cascade
};

//#define REDUX_COMPRESS

#ifdef _REDUX

#ifdef REDUX_COMPRESS

	#define USE_REDUX_EXE					// 使用reduxcommandline.exe解壓

	#define REDUX_DONT_ARGB					// ARGB 格式不使用rdt壓縮

	//#define REDUX_DONT_DXT1A				// dxt1 格式不使用rdt壓縮

#endif /*REDUX_COMPRESS*/

#endif /*_REDUX*/

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
	int len = lstrlen( fileNameStr );
	if ( len < 4 ) return false;
	if ( lstrcmpi( &fileNameStr[len - 4], extensionNameStr ) == 0 ) return true;	
	return false;
}

inline bool CheckExtensionNameW( const wchar_t *fileNameStr, const wchar_t *extensionNameStr  )
{
	if ( fileNameStr == NULL ) return false;
	int len = (int)wcslen( fileNameStr );
	if ( len < 4 ) return false;
	if ( _wcsicmp( &fileNameStr[len - 4], extensionNameStr ) == 0 ) return true;	
	return false;
}

inline bool IsImageFile( const TCHAR *fileNameStr )
{
	if ( fileNameStr == NULL ) return false;
	int len = lstrlen( fileNameStr );
	if ( len < 4 ) return false;
	if ( lstrcmpi( &fileNameStr[len - 4], _T(".BMP") ) == 0 ) return true;
	//else if ( lstrcmpi( &fileNameStr[len - 4], _T(".GIF") ) == 0 ) return true;
	else if ( lstrcmpi( &fileNameStr[len - 4], _T(".JPG") ) == 0 ) return true;
	else if ( lstrcmpi( &fileNameStr[len - 4], _T(".PCX") ) == 0 ) return true;
	else if ( lstrcmpi( &fileNameStr[len - 4], _T(".PNG") ) == 0 ) return true;
	else if ( lstrcmpi( &fileNameStr[len - 4], _T(".TGA") ) == 0 ) return true;
	else if ( lstrcmpi( &fileNameStr[len - 4], _T(".DDS") ) == 0 ) return true;
	else if ( lstrcmpi( &fileNameStr[len - 4], _T(".RDT") ) == 0 ) return true;
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
	* 取得最源使的MIPMAP層級數量,可以檢查是否重覆降低Mipmap層級	
	*
	* @return 傳回層級數量
	*/
	int GetReduceMipmapLevels( int index = 0 );

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
	* 降低Minmap層數
	*
	* @param reduceLevel 要降低的層級數量,必需大於目前Minmap層級數
	*

	*/
	virtual bool ReduceMinmapLevel(int reduceLevel);

	/**

	*
	* @param format 圖形格式
	* @param minmapLevels minmap 層級
	* @param width 圖形寬度
	* @param height 圖形高度
	*

	*/
	virtual bool CreateImage(ImageFormat format, int minmapLevels, int width, int height);

protected:	

	/**
	* 讀取圖形檔案資料
	*
	* @param fileNameStr   檔案名稱
	* @param pSpecialData  載入圖形參數設定(ReadImageOptions)
	*

	*/
	virtual bool Read( const char *fileNameStr, ReadImageOptions *pSpecialData );
	virtual bool Read( const wchar_t *fileNameStr, ReadImageOptions *pSpecialData );

	/**
	* 讀取DDS圖形檔案
	*
	* @param fileNameStr   檔案名稱
	* @param pSpecialData  載入圖形參數設定(ReadImageOptions)
	*

	*/
	virtual bool ReadDDS( const char *fileNameStr, ReadImageOptions *pSpecialData );
	virtual bool ReadDDS( const wchar_t *fileNameStr, ReadImageOptions *pSpecialData );

public:

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

	*/
	bool BltToDXT( void *pDest, ImageFormat format, int x, int y, int width, int height, int mipmapLevels = 0,
		DWORD color = 0xFFFFFFFF, DWORD rop = IMGFILE_ROP_SELECTSRC );

#ifdef _REDUX

	// Initialize Redux processor
	static bool ReduxInitialize();

	// Initialize Redux processor
	static bool ReduxExit();

	/**
	* 轉換壓縮格式
	*
	* @param fileFormat  指定新壓縮格式
	*

	*/
	virtual bool ConvertFormat( CompressFormat fileFormat );

#endif

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
	* 取得檔案資料
	*
	* @param hFile  File Handle 
	* @param fullNameStrh  完整檔案名稱(包含路徑資訊)
	* @param imageOptions  圖形使用特定格式, 參考 ReadImageOptions
	*

	*/
	virtual bool ReadFileData(HANDLE hFile, const char* fullNameStrh, void* imageOptions=NULL);

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

	*/
	bool Transfrom( void *pDest, ImageFormat format, int x, int y, int width, int height, int pitch, void *pSrc, int srcWidth, int srcHeight);

	/**
	* 是否圖形資料有透明值
	*/
	bool IsTransparent();

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