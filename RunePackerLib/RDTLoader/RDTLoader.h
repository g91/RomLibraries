#include "redux/runtime/redux.h"
#include <FilePackage\cFilePackage.h>
typedef struct
{
	ReduxTextureDesc outputDesc;
	unsigned char* imagePtr;
	unsigned char* surfacePtr;
	unsigned char* tempPtr;
	ImageFormat imageFormat;
	size_t imageSize;
	size_t surfaceSize;	
	size_t imagePosition;
} DECOMPRESS_DATA;


struct IRDTDecode
{
	virtual void Release(void)=0;
	virtual bool Decompress(DECOMPRESS_DATA& data,unsigned char *pData,int compressSize )=0;
};

typedef IRDTDecode*(*GETIRDTDECODE)();