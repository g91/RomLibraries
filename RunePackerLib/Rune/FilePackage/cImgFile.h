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
	IMGFILE_ROP_ADD			= DXT_ROP_ADD,			// ���갵,���Ŧb���g
	IMGFILE_ROP_LAST,
};

// ���J�ϧ��ɮ׮ɪ��ѼƳ]�w
typedef struct ReadImageOptions
{
	ReadImageOptions()
	{
		fmt = IMAGE_A8R8G8B8;
		mipmapBool = false;
	};

	ImageFormat fmt;				// ���w�ϧή榡
	bool mipmapBool;				// �O�_��MIPMAP�B�z
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
	* ����Ҧ����
	*/
	void Destroy();

	/**
	* ���o�ϧή榡
	*/
	ImageFormat GetImageFormat( void ) { return m_imageFormat; }

	/**
	* ���o�ϧΤؤo�j�p
	*
	* @return �Ǧ^�e�׭�
	*/
	int GetXSize( void ) { return m_width; }

	/**
	* ���o�ϧΤؤo�j�p
	*
	* @return �Ǧ^���׭�
	*/
	int GetYSize( void ) { return m_height; }

	/**
	* ���oMIPMAP�h�żƶq
	*
	* @return �Ǧ^�h�żƶq
	*/
	int GetMipmapLevels( void ) { return m_mipmapLevels; }

	/**
	* ���o�ϧθ�ƶq�j�p
	*
	* @return �Ǧ^�ϧθ�ƶq�j�p,���BYTE
	*/
	int GetPixelsSize( void ) { return m_dataSize; }

	/**
	* ���o�ϧθ��
	*
	* @return �Ǧ^�ϧθ�ƫ���
	*/
	BYTE *GetPixels( void ) { return m_pData; }

	/**
	* �q�ɮפ�Ū�����
	*
	* @param workDirStr  �u�@���|
	* @param fileNameStr  �ɮצW��
	* @param format  �ɮ����Y�榡
	* @param pSpecialData  �X�R�ΰѼ�,���Ы��VReadImageOptions���c
	*
	* @return true ���\  false ����
	*/
	virtual bool LoadFromFile( const char *workDirStr, const char *fileNameStr, CompressFormat format = COMPRESS_FMT_NORMAL, void *pSpecialData = NULL );	
	virtual bool ReduceMinmapLevel(int reduceLevel);
	virtual bool CreateImage(ImageFormat format, int minmapLevels, int width, int height);
	int GetReduceMipmapLevels( int index = 0 );
protected:

	/**
	* Ū���ϧ��ɮ׸��
	*
	* @param fileNameStr   �ɮצW��
	* @param pSpecialData  ���J�ϧΰѼƳ]�w(ReadImageOptions)
	*
	* @return  true ���\ false ����
	*/
	virtual bool Read( const char *fileNameStr, ReadImageOptions *pSpecialData );

	/**
	* Ū��DDS�ϧ��ɮ�
	*
	* @param fileNameStr   �ɮצW��
	* @param pSpecialData  ���J�ϧΰѼƳ]�w(ReadImageOptions)
	*
	* @return  true ���\ false ����
	*/
	virtual bool ReadDDS( const char *fileNameStr, ReadImageOptions *pSpecialData );

public:


	/**
	* �ഫ���Y�榡
	*
	* @param fileFormat  ���w�s���Y�榡
	*
	* @return true ���\  false ����
	*/
	virtual bool ConvertFormat( CompressFormat fileFormat );

//		virtual int LoadFromData( HANDLE hFile, BOOL loadDataBool = true );
	/**
	* �����ϧθ�Ʀܫ��w�O���餺,�ت��O���餣�i��DXT���Y�榡
	*
	* @param pDest  �ت��O�������
	* @param format  �ت��O����ϧή榡,���ä��䴩DXT�榡
	* @param x  �ƨ��ت��ϧήy��
	* @param y  �ƨ��ت��ϧήy��
	* @param width  �ت��ϧμe��
	* @param height  �ت��ϧΰ���
	*
	* @return  true ���\  false ����
	*/
	bool BltToRGB( void *pDest, ImageFormat format, int x, int y, int width, int height, int pitch, int mipmapLevel = -1 );

	/**
	* ����DXT�ϧθ�Ʀܫ��w�O���餺,�ت��O����ݬ�DXT�榡
	*
	* @param pDest  �ت��O�������
	* @param format  �ت��O����ϧή榡,���ݬ�DXT�榡
	* @param x  �ƨ��ت��ϧήy��
	* @param y  �ƨ��ت��ϧήy��
	* @param width  �ت��ϧμe��
	* @param height  �ت��ϧΰ���		
	* @param mipmapLevels  ���w�ӷ�MIPMAP�h��,�Ĥ@�h��0,�ĤG�h��1,���̦�����
	* @param color  �w���C���
	* @param rop  �K�Ϥ覡
	*
	* @return  true ���\  false ����
	*/
	bool BltToDXT( void *pDest, ImageFormat format, int x, int y, int width, int height, int mipmapLevels = 0,
		DWORD color = 0xFFFFFFFF, DWORD rop = IMGFILE_ROP_SELECTSRC );

protected:

	/**
	* �x�s���Y���
	*/
	virtual void SaveHeader( cListData &header );

	/**
	* Ū�����Y���
	*/
	virtual bool LoadHeader( HANDLE hFile );

	/**
	* �g�J���
	*/
	virtual void WriteData( HANDLE hFile );



	/**
	* �ഫ�ϧή榡,�ӷ��榡�ݬ�A8R8G8B8 32Bits
	* 
	* @param pDest  �ت��O�������
	* @param format  �ت��ϧή榡
	* @param x  �ت��ϧήy��
	* @param y  �ت��ϧήy��
	* @param width  �ت��ϧμe��
	* @param height  �ت��ϧΰ���
	* @param pSrc  �ӷ��ϧθ��,�ݬ�A8R8G8B8�榡
	* @param srcWidth  �ӷ��ϧμe��
	* @param srcHeight  �ӷ��ϧΰ���
	*
	* @return true ���\  false ����
	*/
	bool Transfrom( void *pDest, ImageFormat format, int x, int y, int width, int height, int pitch, void *pSrc, int srcWidth, int srcHeight);


protected:

	ImageFormat	m_imageFormat;				// �ϧή榡
	int			m_width;					// �ϧμe��
	int			m_height;					// �ϧΰ���
	union {
		int m_mipmap;
		struct {
			char	m_mipmapLevels;			// mipmap�ƶq (0��1���N��S��mipmap)
			char	m_reduceMipmap[3];		// �Y�� mipmap �U��
		};
	};
};

#endif CLASS_IMGFILE_H