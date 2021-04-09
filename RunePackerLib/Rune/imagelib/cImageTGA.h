#ifndef CLASS_IMAGE_TGA_H
#define CLASS_IMAGE_TGA_H

#include "cImage.h"

#if IMAGELIB_SUPPORT_TGA

#pragma pack(1)
// TGA���Y��T
typedef struct tagTgaHeader
{
	BYTE bFieldLength;				// �ϭ���r��Ķ�����
	BYTE bColorMapType;				// �C���ϥΪ��A
	BYTE bImageDataType;			// �ϭ��x�s�覡	
	WORD wColorMapFirstIndex;		// �C���Ĥ@��Entry�N��
	WORD wColorMapSize;				// �C���Entry�`��
	BYTE bColorMapEntry;			// �C���Entry�j�p(15,16,24,32)
	WORD wOriginX;					// �ϭ����W��X�y�Э�
	WORD wOriginY;					// �ϭ����W��Y�y�Э�
	WORD wWidth;					// �ϧμe��
	WORD wHeight;					// �ϧΰ���
	BYTE bDepth;					// ���IBit��
	BYTE bDescriptor;				// �ϧδy�z��T
}TGAHEADER;
#pragma pack()

// TGA�ɧ���T
typedef struct tagTgaFooter
{
	int iExtensionOffset;				// Extension �첾��
	int iDeveloperOffset;				// Developer Directory �첾��
	BYTE szSignature[18];				// �s��TGA�ѧO�r��
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
	* ���Y���
	* @param pSrc �ӷ��ϧΫ���
	* @param pDest �s�����Y��ت�����
	* @param size �ӷ��ϧΤj�p(���:Byte)
	* @param bpp �C�@���I�s���Ƥj�p(���:Byte)
	* @return : ���Y���Ƥj�p(Byte)
	*/
	int Compress( BYTE *pSrc, byte *pDest, int size, int bpp );

	/**
	* �����Y���
	* @param pSrc �ӷ����Y�L�ϧΫ���
	* @param pDest �Ѷ}��ت��s�����,�ݭn���}�n�O�����m
	* @param size �Ѷ}��ϧθ�Ƥj�p(���:Byte)
	* @param bpp �C�@���I�s���Ƥj�p(���:Byte)
	* @return �@Ū���h��BYTE���
	*/
	int Decompress( BYTE* pSrc, BYTE* pDest, int size, int bpp );
};

#endif

#endif // CLASS_IMAGETGA_H