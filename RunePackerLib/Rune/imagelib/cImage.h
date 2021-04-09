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
	* ���o�ϧή榡
	* @return �Ǧ^�ϧή榡
	*/
	ImageLibFormat GetFormat() { return m_imageFormat; }

	/**
	* ���o�ϧμe��
	* @return �Ǧ^�ϼe
	*/
	int GetXSize() { return m_width; }

	/**
	* ���o�ϧΰ���
	* @return �Ǧ^�ϰ�
	*/
	int GetYSize() { return m_height; }

	/**
	* ���o�ϧΨC�@���I�һ�bits
	@ return 8 or 24 or 32
	*/
	int GetDepth() { return m_depth; }

	/**
	* ���o�զ�L�C���`��
	* @return �զ�L�`��
	*/
	int GetPaletteCount() { return m_paletteCount; }

	/**
	* �O�_�禳�z����T(8 or 32 Bits Color)
	* @return true ���z����T false �L�z����T
	*/
	bool IsTransparent() { return m_transparentBool; }

	/**
	* ���o�ϧΫ���
	* @return �Ǧ^�ϧΫ��Ц�m
	*/
	BYTE *GetPixels() { return m_pPixels; }

	/**
	* ���o�զ�L����
	* @return �Ǧ^��L����
	*/
	RGBQUAD *GetPalette() { return m_pPalette; }

	/**
	* ����Ҧ��ϧθ��
	*/
	void Destroy();

	/**
	* �إ߷s���ťչϧθ��,�|�̾کҶǤJ���Ѽ�,�t�m�A�X���O����Ŷ�
	* @param width  �ϧμe��
	* @param height  �ϧΰ���
	* @param depth  �ϧβ`��(8, 24, 32 Bits)
	* @param format  �ϧΨӷ��榡
	* @return true �إߦ��\  false ����
	*/
	bool Create( int width, int height, int depth, ImageLibFormat format = IMAGELIB_FORMAT_UNKNOWN );	

	/**
	* ��g�B�z
	* @return true ���\  false ����
	*/
	bool Mirror();

	/**
	* �ഫ�ϧή榡,�L�k�ഫ��256��
	* @param format  �ت��C��
	* @return true ���\  false ����
	*/
	bool ConvertFormat( PixelFormat format );

	/**
	* �ƨ�ϧθ��,�ݭn�ۦP�榡�~�వ��
	* @param srcImage �ӷ��ϧθ��
	* @param x �ƨ��ت��}�l�y��
	* @param y �ƨ��ت��}�l�y��
	* @return true �إߦ��\  false ����
	*/
	bool Copy( cImage* srcImage, int x, int y );

protected:
	
	void ConvertToR5G6B5();
	void ConvertToA1R5G5B5();
	void ConvertToR8G8B8();
	void ConvertToA8R8G8B8();

public:

	/**
	* ���J�ϧ���
	* @param fileNameStr  �ϧ��ɮצW��
	* @param format  ���w�ӷ��ϧή榡
	* @return  true ���J���\  false ���J����
	*/
	bool Load( LPCSTR fileNameStr, ImageLibFormat format = IMAGELIB_FORMAT_UNKNOWN );
	bool Decode( FILE *  file, ImageLibFormat format );

	/**
	* �s�x�ɮ�
	* @param fileNameStr  �ϧ��ɮצW��
	* @param format  ���w�ت��ϧή榡(�ثe�䴩TGA)
	* @return  true �x�ɦ��\  false �x�ɥ���
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

	ImageLibFormat m_imageFormat;	// �ӷ��ϧή榡
	PixelFormat m_pixelFormat;		// �ӷ����I�榡
	int m_width;					// �ϧΤؤo
	int m_height;					// �ϧΤؤo
	int m_depth;					// �C�@�����һݦ줸(2,4,8,16,24,32)
	int m_paletteCount;				// �զ�L�C���`��
	bool m_transparentBool;			// �O�_���z����
	BYTE *m_pPixels;				// �������
	RGBQUAD *m_pPalette;			// �զ�L���

	bool m_ghost;					// �ëD����,��Ƭ��ƨ�Ө�
};