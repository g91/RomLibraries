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

		// �]�w�ӷ����(�ݭn��32Bits)
		void SetSourceData(const void* data, int width, int height);

		// �]�w�ഫ�榡
		void SetFormat(nvtt::Format format);

		// �]�w��X�~��
		void SetQuality(nvtt::Quality quality);

		// �]�w�O�_��X�ɮ����Y(�w�]��X)
		void SetOutputHeader(bool enabled);

		// �]�w��X�禡�B�z���c
		void SetOutputHandler(nvtt::OutputHandler* outputHandler);

		// �]�w���~�禡�B�z���c
		void SetErrorHandler(nvtt::ErrorHandler* errorHandler);

		// �]�w��X���ɮ�
		void SetFilename(const char* filename);

		// �]�w�O�_�ϥ� CUDA (GPU) ���ഫ
		void EnableCudaAcceleration(bool enabled);

		// �������Y
		bool ExcuteCompress(void* outputData = NULL);
	};
	
#endif

#define MIN_UNIT				4			// �̤p�ؤo���
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
	DXT_ROP_ADD,					// ���갵,���Ŧb���g
};

// �ϧή榡
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

// �C��ȵ��c
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

// �i�s��خ榡 A1R5G5B5 or R5G6B5 ���
struct BLOCK_DXT1
{
	WORD c[2];					// �D�n���C�C��
	DWORD index;				// ���ޭ�

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

// DXT3�i�s4Bits Alpha
struct BLOCK_DXT3
{
	WORD a[4];					// �e��64��Bits�O��16�ӹ��I��Alpha��
	BLOCK_DXT1 block;			// �PDXT1���c�ۦP
};

// DXT5�i�H�s8Bits Line Alpha,��DXT1 Block���I��
struct BLOCK_DXT5
{
	BYTE a[8];					// 2Bytes���C�C���,��L6Bytes�����ޭ�(48Bits / 16 = 3Bits)
	BLOCK_DXT1 block;			// �PDXT1���c�ۦP

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
* ���o�ϧ�mipmap�ƶq
*
* @param width  �ϧΤؤo
* @param height  �ϧΤؤo
*/
inline int GetMipmapCount( int width, int height ) 
{
	int levels = 0;
	for ( int i = max(width, height); i > 2; i >>= 1 ) levels++;
	return levels;
}

/**
* ��ĶDXT�ϧή榡,�]��DDS���Y��T
*
* @param pDest  �ت��O�������,�ݬ�32Bits Color
* @param pSrc  �ӷ���ƫ���,�e�ݥ]��DDS���Y��T
*

*/
bool DeCompressDXT( BYTE *pDest, BYTE *pSrc );

/**
* �ˬdDXT1�ϧή榡�O�_���ϥγz��
*
* @param pSrc  �ӷ���ƫ���
* @param width  �ϧμe�׭�
* @param height  �ϧΰ��׭�
*/
bool IsTransparentDXT1( BYTE* pSrc, int width, int height);

/**
* �ˬdDXT5�ϧή榡�O�_���ϥ�ALPHA
*
* @param pSrc  �ӷ���ƫ���
* @param width  �ϧμe�׭�
* @param height  �ϧΰ��׭�


*/
bool CheckTransformDXT5ToDXT1( BYTE* pSrc, int width, int height, bool& isTransparent);

/**
* ��ĶDXT1�ϧή榡
*
* @param pDest  �ت��O�������,�ݬ�32Bits Color
* @param pSrc  �ӷ���ƫ���
* @param width  �ϧμe�׭�
* @param height  �ϧΰ��׭�
*/
void PlotDXT1( BYTE *pDest, BYTE *pSrc, int width, int height );

/**
* ��ĶDXT3�ϧή榡
*
* @param pDest  �ت��O�������,�ݬ�32Bits Color
* @param pSrc  �ӷ���ƫ���
* @param width  �ϧμe�׭�
* @param height  �ϧΰ��׭�
*/
void PlotDXT3( BYTE *pDest, BYTE *pSrc, int width, int height );

/**
* ��ĶDXT5�ϧή榡
*
* @param pDest  �ت��O�������,�ݬ�32Bits Color
* @param pSrc  �ӷ���ƫ���
* @param width  �ϧμe�׭�
* @param height  �ϧΰ��׭�
*/
void PlotDXT5( BYTE *pDest, BYTE *pSrc, int width, int height );

/**
* ��ĶDXT5 Alpha���,��¥u���oAlpha
*
* @param pDest  �ت��O�������
* @param pSrc  �ӷ��O������
* @param width  �e�׭�
*/
void PlotDXT5Alpha( DXT_COLOR *pDest, BLOCK_DXT5 *pSrc, int width, int height );

/**
* ���Y��@�϶���DXT�榡
*
* @param pImageData  pointer to data (32 bit)
* @param textureFormat  ���wDXT�榡( kDXT1, kDXT3. kDXT5 )

*/
BYTE *CompressDXTBlock( BYTE *pImageData, TextureForamtEnum textureFormat );

/**
* DXT�榡�ഫ
*
* @param pDest  �ت��O�������
* @param destFormat  �ت��榡(nvTextureFormats kDXT1, kDXT3, kDXT5)
* @param pSrc  �ӷ��O�������
* @param srcFormat  �ӷ��榡(nvTextureFormats kDXT1, kDXT3, kDXT5)
* @param width  �ӷ��ϧΤj�p
* @param height  �ӷ��ϧΤj�p
*/
bool ConvertDXTFromat( void *pDest, DWORD destFormat, void *pSrc, DWORD srcFormat, int width, int height );

/**
* �ƨ�DXT�ϧ��ɮ�
*
* @param pDest  �ت��O�������
* @param destFromat  �ت��ϧή榡(nvTextureFormats kDXT1, kDXT3, kDXT5)
* @param x  �ƨ��ت��ϧήy��
* @param y  �ƨ��ت��ϧήy��
* @param destWidth  �ت��ϧΤj�p
* @param destHeight  �ت��ϧΤj�p
* @param pSrc  �ӷ��ϧΰO�������
* @param srcFormat  �ӷ��ϧή榡(nvTextureFormats kDXT1, kDXT3, kDXT5)
* @param srcWidth  �ӷ��ϧΤj�p
* @param srcHeight  �ӷ��ϧΤj�p
* @param color  �ӷ��ϧιw���C��(�w�]0xffffffff,�����C��p��)
* @param rop  �ƨ�覡(�w�]
*

*/
bool BltDXT( void *pDest, DWORD destFormat, int x, int y, int destWidth, int destHeight, void *pSrc, DWORD srcFormat,
			int srcWidth, int srcHeight, DWORD color=0xffffffff, DWORD rop=DXT_ROP_OVERLAY );

#endif //DXT_UTILITY_H