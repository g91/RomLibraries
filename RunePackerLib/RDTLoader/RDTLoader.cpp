#include <windows.h>
#include "RDTLoader.h"

#include <shellapi.h>
#include <sys/stat.h>
#pragma comment(lib, "redux_runtime.lib")
#pragma comment(lib, "shell32.lib")

/* Redux include */


/** @brief Decompression data */

ImageFormat reduxGetImageFormat(ReduxTextureDesc& textureDesc)
{
	switch (textureDesc.pixelFormat)
	{
	case Redux_PF_RGBA:
	case Redux_PF_RGBx:
		return IMAGE_A8R8G8B8;

	case Redux_PF_RGB:
		return IMAGE_R8G8B8;

	case Redux_PF_L:
		assert(false);
		break;

	case Redux_PF_RGBA|Redux_PF_16b:
	case Redux_PF_RGBx|Redux_PF_16b:
		return IMAGE_A1R5G5B5;

	case Redux_PF_RGB|Redux_PF_16b:
		return IMAGE_R5G6B5;

	case Redux_PF_L|Redux_PF_16b:
		assert(false);
		break;

	case Redux_PF_DXT5:
		return IMAGE_DXT5;

	case Redux_PF_DXT3:
		return IMAGE_DXT3;

	case Redux_PF_DXT1:	
		return IMAGE_DXT1;

	case Redux_PF_DXT1a:
		return IMAGE_DXT1a;
	}

	return IMAGE_UNKNOWN;
}

unsigned int gChunkIndex = 0;

static cImgFile* g_ImageFile = NULL;

/** @brief Load a file from filepath
@param filepath The ASCII filepath to load
@return An allocated buffer w/ 16bytes alignment
@post The buffer must be destroyed with reduxTutorialCommonLoadFree function */
void* reduxTutorialCommonLoadFileReadAll(const char* filepath, size_t *chunkSize)
{
	unsigned char* dataContent;
	unsigned int dataByteCount;
	FILE *file;
	struct stat st;

	file = fopen(filepath, "rb");
	if(!file || stat(filepath, &st)) {
		printf("Failed to load: %s\n", filepath);
		return NULL;
	}

	dataByteCount = st.st_size;
	dataContent = (unsigned char *)_aligned_malloc(dataByteCount,16);
	fread(dataContent, dataByteCount, 1, file);
	fclose(file);

	*chunkSize = dataByteCount;

	return dataContent;
}


/** @brief Free a buffer allocated w/ reduxTutorialCommonLoadFileReadAll */
void reduxTutorialCommonLoadFree(void *ptr)
{
	_aligned_free(ptr);
}


/** @brief Create the template to build chunks filepaths from header filepath 
@param[out] resultFilepath Chunks filepath template
@param resultFilepathSize Maximum size of the path
@param headerFilepath The filepath of the header */
void reduxTutorialCommonLoadChunksFilepath(
	char* resultFilepath,
	size_t resultFilepathSize,
	const char* headerFilepath)
{
	char *dotpos;
	strncpy(resultFilepath,headerFilepath,resultFilepathSize);

	dotpos = strrchr(resultFilepath,'.');
	if (dotpos==NULL)
	{
		dotpos = resultFilepath+strlen(resultFilepath);
	}

	strcpy(dotpos,"_%04d.rdxc");
}

/** @brief 'open binary chunk' callback function */
void REDUX_CALLBACK reduxTutorialCallbackChunkOpen(
	ReduxHandle *handle,
	size_t userData,
	unsigned int chunkIndex,
	size_t* userChunk,
	ReduxAccessModeEnum mode)
{
	char filepath[1024];
	FILE *file;

	(void)handle;
	printf("Open chunk (index %d)\n",chunkIndex);

	// The chunks filepath template is transmitted by 'userData' 
	_snprintf(filepath,sizeof(filepath)-1,(const char*)userData,chunkIndex);

	file = fopen(filepath, "rb");
	if(!file)
	{
		printf("Failed to load: %s\n", filepath);
		*userChunk = 0;
		return;
	}

	*userChunk = (size_t)file;
}


/** @brief 'close binary chunk' callback function */
void REDUX_CALLBACK reduxTutorialCallbackChunkClose(
	ReduxHandle *handle,
	size_t userData,
	unsigned int chunkIndex,
	size_t* userChunk)
{
	(void)handle;

	printf("Close chunk (index %d)\n",chunkIndex);

	if (*userChunk!=0)
	{
		fclose((FILE*)*userChunk);
	}
}


/** @brief 'read binary chunk' callback function */
void REDUX_CALLBACK reduxTutorialCallbackChunkRead(
	ReduxHandle *handle,
	size_t userData,
	unsigned int chunkIndex,
	size_t* userChunk,
	size_t size,
	void *buffer)
{
	printf("Read chunk (index %d)\n",chunkIndex);

	if (*userChunk!=NULL)
	{
		fread(buffer, sizeof(char), size, (FILE*)*userChunk);
	}
}


/** @brief 'seek binary chunk' callback function */
void REDUX_CALLBACK reduxTutorialCallbackChunkSeek(
	ReduxHandle *handle,
	size_t userData,
	unsigned int chunkIndex,
	size_t* userChunk,
	size_t offset)
{
	(void) handle;

	printf("Seek chunk (index %d) : Offset=%ld\n",chunkIndex, offset);

	if (*userChunk!=NULL)
	{
		fseek((FILE*)*userChunk, offset, 0);
	}
}

/** @brief Current written DDS file */
static FILE *gOutputDDSFile = NULL;

/** @brief Current written DDS surface pointer */
static BYTE *gOutputDDSSurfacePtr = NULL;

/** @brief Current written DDS surface size */
static size_t gOutputDDSSurfaceSize = 0;

static size_t gOutputSurfaceSize = 0;

static size_t gOutputImageSize = 0;

static BYTE *gReduxLastTempData = NULL;


/** @brief 'result output texture get buffer' callback function */
void* REDUX_CALLBACK reduxTutorialCallbackOutputGetBuffer(
	ReduxHandle *handle,
	size_t userData,
	unsigned int outputIndex,
	unsigned int mipmapLevel,
	ReduxSurfaceDesc *surfaceDesc)
{
	if (mipmapLevel==0)
	{
		// Base level: create the file
		ReduxTextureDesc textureDesc;
		unsigned int res;
		unsigned int size = 0;
		ImageFormat format;

		// Get texture description
		res = reduxHandleGetOutputDesc(handle, outputIndex,	&textureDesc);
		assert(res==0);

		if ( res != 0 || textureDesc.mipmapCount > 20 || textureDesc.level0Width > 2048 || textureDesc.level0Height > 2048 )
		{
			int a = 0;
		}

		// NOTE: NO MORE NEED OF CALLING getDesc( ReduxTextureDesc ),
		// we'll see how to improve this (it was only needed to get texture identifier,
		// so overkill, now the identifier is directly in ReduxSurfaceDesc).		

		switch (textureDesc.pixelFormat)
		{
		case Redux_PF_RGBA:
		case Redux_PF_RGBx:
			format = IMAGE_A8R8G8B8;
			break;

		case Redux_PF_RGB:
			format = IMAGE_R8G8B8;
			break;

		case Redux_PF_L:
			assert(false);
			break;

		case Redux_PF_RGBA|Redux_PF_16b:
		case Redux_PF_RGBx|Redux_PF_16b:
			format = IMAGE_A1R5G5B5;
			break;

		case Redux_PF_RGB|Redux_PF_16b:
			format = IMAGE_R5G6B5;
			break;

		case Redux_PF_L|Redux_PF_16b:
			assert(false);
			break;

		case Redux_PF_DXT5:
			format = IMAGE_DXT5;
			break;

		case Redux_PF_DXT3:
			format = IMAGE_DXT3;
			break;

		case Redux_PF_DXT1:
			format = IMAGE_DXT1;
			break;
		}

		g_ImageFile->CreateImage(format, textureDesc.mipmapCount, surfaceDesc->width, surfaceDesc->height);		

		gOutputDDSSurfacePtr = g_ImageFile->GetData();

		gOutputImageSize = g_ImageFile->GetDataSize();

		// Base level: create the file
		//wchar_t filepath[1024];
		//wchar_t directory[1024];
		//unsigned int res;
		//unsigned char ddsheader[REDUX_TUTORIAL_DDSSAVE_HEADERSIZE];

		// Build DDS name from identifier
		/*_snwprintf(
		filepath,
		sizeof(filepath)/sizeof(wchar_t)-1,
		L"%s\\%s.dds",
		gWorkDirectory,
		surfaceDesc->identifier);
		*/
		// Open file
		//retrievePath(directory,filepath);
		//res = SHCreateDirectoryEx(NULL,directory,NULL);

		//if (ERROR_SUCCESS==res || ERROR_ALREADY_EXISTS==res)
		{
			//gOutputDDSFile = _wfopen(filepath,L"wb");

			// Allocate destination buffer of all levels

			// Write DDS header
			/*reduxTutorialDDSSaveFillHeader(
			surfaceDesc->width,
			surfaceDesc->height,
			surfaceDesc->pitch,
			textureDesc.mipmapCount,
			textureDesc.pixelFormat,
			ddsheader);
			fwrite(ddsheader,1,sizeof(ddsheader),gOutputDDSFile);*/
		}
		/*else
		{
		assert(0);
		}*/
	}
	else
	{
		gOutputDDSSurfacePtr += gOutputDDSSurfaceSize;

		gOutputImageSize -= gOutputDDSSurfaceSize;

		// 最後一個 minmap 
		if ( (int)gOutputImageSize <= 0 )
		{
			gReduxLastTempData = new BYTE[gOutputDDSSurfaceSize];

			if ( gOutputDDSSurfaceSize > 1024 )
			{
				int a = 0;
			}

			gOutputDDSSurfacePtr = gReduxLastTempData;
		}		

		if(gOutputDDSFile)
		{
			// Write previous level
			//fwrite(gOutputDDSSurfacePtr,1,gOutputDDSSurfaceSize,gOutputDDSFile);
		}
	}

	gOutputDDSSurfaceSize = surfaceDesc->bytesCount;

	// Force channel order (for RGBA8, discarded otherwise)
	surfaceDesc->channelsOrder = Redux_ChanOrder_RGBA;

	return gOutputDDSSurfacePtr;
}


/** @brief 'result output texture completed' callback function */
void REDUX_CALLBACK reduxTutorialCallbackOutputCompleted(
	ReduxHandle *handle,
	size_t userData,
	unsigned int outputIndex)
{
	if ( gReduxLastTempData )
	{
		delete [] gReduxLastTempData;
		gReduxLastTempData = NULL;
	}

	// Write last level
	//fwrite(gOutputDDSSurfacePtr,1,gOutputDDSSurfaceSize,gOutputDDSFile);
	//_aligned_free(gOutputDDSSurfacePtr);

	// Close file
	//fclose(gOutputDDSFile);
	gOutputDDSFile = 0;
}

/** @brief Decompression 'output texture get buffer' callback function */
void* REDUX_CALLBACK reduxDecompressCallbackOutputGetBuffer(
	ReduxHandle *handle,
	size_t userData,
	unsigned int outputIndex,
	unsigned int mipmapLevel,
	ReduxSurfaceDesc *surfaceDesc)
{
	DECOMPRESS_DATA* decompData = *(DECOMPRESS_DATA**)userData;

	if (mipmapLevel==0)
	{
		ReduxTextureDesc outputDesc;
		unsigned int result;

		result = reduxHandleGetOutputDesc(
			handle,
			outputIndex,
			&outputDesc);
		assert(result==0);

		// Allocate destination buffer of all levels
		decompData->outputDesc = outputDesc;
		decompData->imageSize = CountImageSize(reduxGetImageFormat(outputDesc), outputDesc.mipmapCount, outputDesc.level0Width, outputDesc.level0Height);
		decompData->imagePtr = 0;
		decompData->tempPtr = 0;
		decompData->imagePosition = 0;

		if ( decompData->imageSize > 0 )
		{
			decompData->imagePtr = new unsigned char[decompData->imageSize];			
		}		
		else
		{
			decompData->tempPtr = new unsigned char[surfaceDesc->bytesCount];
		}
	}
	else 
	{
		// 計算
		decompData->imagePosition += decompData->surfaceSize;

		if ( decompData->tempPtr == NULL && decompData->imagePosition >= decompData->imageSize )
		{
			decompData->tempPtr = new unsigned char[decompData->surfaceSize];
		}
	}

	decompData->surfaceSize = surfaceDesc->bytesCount;

	if ( decompData->tempPtr )
		return decompData->tempPtr;

	return decompData->imagePtr + decompData->imagePosition;
}


/** @brief Decompression 'output texture completed' callback function */
void REDUX_CALLBACK reduxDecompressCallbackOutputCompleted(
	ReduxHandle *handle,
	size_t userData,
	unsigned int outputIndex)
{
	DECOMPRESS_DATA* decompData = *(DECOMPRESS_DATA**)userData;
	if ( decompData && decompData->tempPtr )
	{
		delete [] decompData->tempPtr;		
		decompData->tempPtr = NULL;
	}
	/*
	if ( decompData && decompData->imagePtr )
	{
		delete [] decompData->imagePtr;		
		decompData->imagePtr = NULL;
	}*/
}





class CRDTLoader:public IRDTDecode
{
public:
	CRDTLoader();
	virtual ~CRDTLoader();
	void Release()
	{
		delete this;
	}
	void ReleaseData(DECOMPRESS_DATA& decompressionData)
	{
		if (decompressionData.imagePtr )
		{
			delete [] decompressionData.imagePtr;		
			decompressionData.imagePtr = NULL;
		}
	}
	
	 bool Decompress(DECOMPRESS_DATA& decompressionData,unsigned char *pData,int compressSize );
};

CRDTLoader::CRDTLoader()
{

}
CRDTLoader::~CRDTLoader()
{

}
bool CRDTLoader::Decompress(DECOMPRESS_DATA& decompressionData,unsigned char *pData,int compressSize )
{
	bool ret=false;
	unsigned int res;

	memset(&decompressionData, 0, sizeof(decompressionData));

	// set callbacks
	res = reduxCallbackSet(
		Redux_Callback_OutputGetBuffer,
		(void*)reduxDecompressCallbackOutputGetBuffer);
	assert(res==0);

	res = reduxCallbackSet(
		Redux_Callback_OutputCompleted,
		(void*)reduxDecompressCallbackOutputCompleted);
	assert(res==0);

	res = reduxHandleDecompress( pData, compressSize, (size_t)&decompressionData);
	if ( res == 0 && decompressionData.imagePtr != 0 )
	{
		decompressionData.imageFormat= reduxGetImageFormat(decompressionData.outputDesc);
	
		ret= true;
	}
	

	return ret;
}
IRDTDecode* GetIRDT(void)
{

	return new CRDTLoader();

}