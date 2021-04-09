#ifndef DXT_UTILITY_H
#define DXT_UTILITY_H

#include <dxtlib/dxtlib.h>

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

// �C��ȵ��c
struct DXT_COLOR
{	
	union {
		struct { BYTE b, g, r, a; };
		DWORD Col;
	};
};

// �i�s��خ榡 A1R5G5B5 or R5G6B5 ���
struct BLOCK_DXT1
{
	WORD c[2];					// �D�n���C�C��
	DWORD index;				// ���ޭ�
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
};

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
* @return  ���\ true, ���Ѧ^�� false
*/
bool DeCompressDXT( BYTE *pDest, BYTE *pSrc );

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
* �N�ϧ����Y��DXT�榡
*
* @param pImageData  pointer to data (32 bit)
* @param width  �ϧΤؤo
* @param height  �ϧΤؤo
* @param compressSize  �^�����Y���Ƥj�p
* @param mipmapLevels  �^��mipmap levels
* @param pOptions  �ӳ���Ƴ]�w,�аѦ�"nvdxt_options.h"
*
* @return ���\ �Ǧ^�D�s�ȥN��s�X���\,�ҶǦ^�����лݭn�~�����O��������  ���ѶǦ^�s��
*/
BYTE *CompressDXT( BYTE *pImageData, int width, int height, int &compressSize, int &mipmapLevels, nvCompressionOptions *pOptions = NULL );

/**
* ���Y��@�϶���DXT�榡
*
* @param pImageData  pointer to data (32 bit)
* @param textureFormat  ���wDXT�榡( kDXT1, kDXT3. kDXT5 )
* @return ���\ �Ǧ^�D�s�ȥN��s�X���\,�Ǧ^���аO���餣������  ���ѶǦ^�s��
*/
BYTE *CompressDXTBlock( BYTE *pImageData, nvTextureFormats textureFormat );

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
* @return true ���\  false ����
*/
bool BltDXT( void *pDest, DWORD destFormat, int x, int y, int destWidth, int destHeight, void *pSrc, DWORD srcFormat,
			int srcWidth, int srcHeight, DWORD color=0xffffffff, DWORD rop=DXT_ROP_OVERLAY );

#endif //DXT_UTILITY_H