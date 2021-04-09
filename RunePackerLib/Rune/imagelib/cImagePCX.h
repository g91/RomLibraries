#ifndef CLASS_IMAGE_PCX_H
#define CLASS_IMAGE_PCX_H

#include "cimage.h"

#if IMAGELIB_SUPPORT_PCX

class cImagePCX : public cImage
{	
#pragma pack(1)
// PCX ���Y��T	
typedef struct tagPcxHeader
{
	char bManufacturer;			//Zsoft PCX�ɪ��ѧO�X 0Ah
	char bVersionNo;			//�����s��,�@�볣�� 5
	char bEncoding;				//�s�X��k,�ثe�u�� 1
	char bBitsPerPixel;			//�C�� plane�W�� bits/plane��
	short shXmin;				//�ϭ������W���� X �y��
	short shYmin;				//�ϭ������W���� Y �y��
	short shXmax;				//�ϭ����k�U���� X �y��
	short shYmax;				//�ϭ����k�U���� Y �y��
	short shHResolution;		//������ͦ��ϭ����ù��������ѪR��(dots/inch)
	short shVResolution;		//������ͦ��ϭ����ù��������ѪR��(dots/inch)
	char bHeaderPalette[48];	//���Y�զ�L(16��t�ϥ�)
	char bResered;				//�W�w�� 0
	char bPlanes;				//�����ɼҦ��� bit plane���ƶq
	short shBytesPerLine;		//�b�@�� bit plane�W�C�@�������u�Ҧ���char��
	short shPaletteType;		//�զ�L������,�@�묰 1
	short shHScreenSize;		//�ù������� pixel �ƴ� 1
	short shVScreenSize;		//�ù������� pixel �ƴ� 1
	char bfiller[54];			//�O�d����
}PCXHEADER;
#pragma pack()

public:
	bool Decode( FILE *fptr );
	bool Encode( FILE *fptr )	{ return false; }

protected:
	int Compress( BYTE *pSrc, BYTE *pDest, int size );
	int Decompress( BYTE *pSrc, BYTE *pDest, int size );		
};

#endif


#endif //CLASS_IMAGE_PNG_H