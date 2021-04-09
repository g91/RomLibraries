#ifndef DXT_UTILITY_H
#define DXT_UTILITY_H

struct Nvtt_Config;

//#define ENABLE_NVTT

#ifdef ENABLE_NVTT
	#include <nvtt/nvtt.h>
	#pragma comment(lib, "nvtt.lib")

	struct OutputHandlerData : public nvtt::OutputHandler	
	{
		OutputHandlerData(void* data) : destData(data) {}
		virtual ~OutputHandlerData() {}	

		virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel)
		{
			// ignore.
		}

		// Output data.
		virtual bool writeData(const void * data, int size)
		{
			if ( destData != NULL )
				memcpy(destData, data, size);

			progress += size;		
			return true;
		}

		__int64 progress;
		void* destData;
	};

	struct Nvtt_Config
	{
	public:
		nvtt::InputOptions				m_inputOptions;		
		nvtt::OutputOptions				m_outputOptions;
		nvtt::CompressionOptions		m_compressOptions;
		nvtt::Compressor				m_compressor;

	public:
		Nvtt_Config();
		~Nvtt_Config();

		// Set color to normal map conversion options.
		void SetColorToNormalMap();

		// Set options for normal maps.
		void SetNormalMap();		

		// Set options for color maps.
		void SetColorMap();

		// Set mipmaps
		void SetMipmapGeneration(bool enabled, int maxlevels = -1);

		// 設定來源資料(需要為32Bits)
		void SetSourceData(const void* data, int width, int height);

		// 設定轉換格式
		void SetFormat(nvtt::Format format);

		// 設定輸出品質
		void SetQuality(nvtt::Quality quality);

		// 設定是否輸出檔案檔頭(預設輸出)
		void SetOutputHeader(bool enabled);

		// 設定輸出函式處理結構
		void SetOutputHandler(nvtt::OutputHandler* outputHandler);

		// 設定錯誤函式處理結構
		void SetErrorHandler(nvtt::ErrorHandler* errorHandler);

		// 設定輸出為檔案
		void SetFilename(const char* filename);

		// 設定是否使用 CUDA (GPU) 做轉換
		void EnableCudaAcceleration(bool enabled);

		// 執行壓縮
		bool ExcuteCompress(void* outputData = NULL);
	};
	
#endif

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

// 圖形格式
enum TextureForamtEnum
{
	TEXTURE_P8						= 0,
	TEXTURE_A4R4G4B4				,
	TEXTURE_A1R5G5B5				,
	TEXTURE_R5G6B5					,
	TEXTURE_R8G8B8					,
	TEXTURE_A8R8G8B8				,
	TEXTURE_DXT1					,
	TEXTURE_DXT1a					,	
	TEXTURE_DXT3					,	
	TEXTURE_DXT5					,
};

// 顏色值結構
struct DXT_COLOR
{	
	union {
		struct { BYTE b, g, r, a; };
		DWORD Col;
	};
};

struct DXT_ALPHA_BLOCK
{
	BYTE a[16];
};

// 可存兩種格式 A1R5G5B5 or R5G6B5 兩種
struct BLOCK_DXT1
{
	WORD c[2];					// 主要高低顏色
	DWORD index;				// 索引值

	bool IsTransparent()
	{
		if ( c[0] <= c[1] )
		{
			for ( int i = 0; i < 32; i+=2 )
			{
				if ( ((index >> i) & 0x03) == 3 )
					return true;
			}
		}

		return false;
	}
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

	void GetAlphaBlock(DXT_ALPHA_BLOCK& alphaBlock)
	{
		BYTE alphaArray[8];
		INT64 index;
		memcpy(&index, &a[2], 6);
		
		alphaArray[0] = a[0];
		alphaArray[1] = a[1];

		if ( alphaArray[0] > alphaArray[1] ) {
			// 8-alpha block:  derive the other six alphas.    
			// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
			alphaArray[2] = (6 * alphaArray[0] + 1 * alphaArray[1] + 3) / 7;	// bit code 010
			alphaArray[3] = (5 * alphaArray[0] + 2 * alphaArray[1] + 3) / 7;	// bit code 011
			alphaArray[4] = (4 * alphaArray[0] + 3 * alphaArray[1] + 3) / 7;	// bit code 100
			alphaArray[5] = (3 * alphaArray[0] + 4 * alphaArray[1] + 3) / 7;	// bit code 101
			alphaArray[6] = (2 * alphaArray[0] + 5 * alphaArray[1] + 3) / 7;	// bit code 110
			alphaArray[7] = (1 * alphaArray[0] + 6 * alphaArray[1] + 3) / 7;	// bit code 111  
		}    
		else {  
			// 6-alpha block.    
			// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
			alphaArray[2] = (4 * alphaArray[0] + 1 * alphaArray[1] + 2) / 5;	// Bit code 010
			alphaArray[3] = (3 * alphaArray[0] + 2 * alphaArray[1] + 2) / 5;	// Bit code 011
			alphaArray[4] = (2 * alphaArray[0] + 3 * alphaArray[1] + 2) / 5;	// Bit code 100
			alphaArray[5] = (1 * alphaArray[0] + 4 * alphaArray[1] + 2) / 5;	// Bit code 101
			alphaArray[6] = 0;													// Bit code 110
			alphaArray[7] = 255;												// Bit code 111
		}		

		for ( int i = 0; i < 16; i++ )
			alphaBlock.a[i] = alphaArray[(index >> (i * 3)) & 0x07];
	}
};

inline bool IsPower2(unsigned int x)
{              
	if ( x < 1 )return false;
	if (x == 1)	return true;
	if ( x & (x-1) ) return false;
	return true;
}

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

*/
bool DeCompressDXT( BYTE *pDest, BYTE *pSrc );

/**
* 檢查DXT1圖形格式是否有使用透明
*
* @param pSrc  來源資料指標
* @param width  圖形寬度值
* @param height  圖形高度值
*/
bool IsTransparentDXT1( BYTE* pSrc, int width, int height);

/**
* 檢查DXT5圖形格式是否有使用ALPHA
*
* @param pSrc  來源資料指標
* @param width  圖形寬度值
* @param height  圖形高度值


*/
bool CheckTransformDXT5ToDXT1( BYTE* pSrc, int width, int height, bool& isTransparent);

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
* 壓縮單一區塊為DXT格式
*
* @param pImageData  pointer to data (32 bit)
* @param textureFormat  指定DXT格式( kDXT1, kDXT3. kDXT5 )

*/
BYTE *CompressDXTBlock( BYTE *pImageData, TextureForamtEnum textureFormat );

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

*/
bool BltDXT( void *pDest, DWORD destFormat, int x, int y, int destWidth, int destHeight, void *pSrc, DWORD srcFormat,
			int srcWidth, int srcHeight, DWORD color=0xffffffff, DWORD rop=DXT_ROP_OVERLAY );

#endif //DXT_UTILITY_H