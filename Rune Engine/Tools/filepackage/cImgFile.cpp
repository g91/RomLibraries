#include <assert.h>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <ddraw.h>
#include <Shlwapi.h>
#include <sys/stat.h>
#include "../imagelib/cImage.h"
#include "cImgFile.h"

/*
#ifdef _DEBUG
  #pragma comment(lib, "FilePackage\\imagelib\\imagelib_d.lib")
#else
  #pragma comment(lib, "FilePackage\\imagelib\\imagelib.lib")
#endif *//*_DEBUG*/


#ifdef _REDUX

/* Redux include */
#include <redux/runtime/redux.h>
#pragma comment(lib, "redux_runtime.lib")

#ifdef REDUX_COMPRESS
	#ifdef USE_REDUX_EXE
		#include <shellapi.h>
		#pragma comment(lib, "shell32.lib")
	#else		
		#include <redux/offline/processor.h>
		#pragma comment(lib, "redux_offline.lib")
	#endif
#endif





/** @brief Decompression data */
typedef struct
{
	ReduxTextureDesc outputDesc;
	unsigned char* imagePtr;
	unsigned char* surfacePtr;
	unsigned char* tempPtr;
	size_t imageSize;
	size_t surfaceSize;	
	size_t imagePosition;

	const char* filename;
	bool boInitial;
} DECOMPRESS_DATA;

/** @brief Compression data */
typedef struct
{
	char* outputFilename;
	unsigned char* rdtData;	
	size_t rdtSize;	
} COMPRESS_DATA;


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
	decompData->boInitial = true;	

	if (mipmapLevel==0)
	{
		ReduxTextureDesc outputDesc;
		unsigned int result;

		result = reduxHandleGetOutputDesc(
			handle,
			outputIndex,
			&outputDesc);
		assert(result==0);

		outputDesc.level0Width = GetPower2(outputDesc.level0Width);
		outputDesc.level0Height = GetPower2(outputDesc.level0Height);

		// Allocate destination buffer of all levels
		decompData->outputDesc = outputDesc;
		decompData->imageSize = CountImageSize(reduxGetImageFormat(outputDesc), outputDesc.mipmapCount, outputDesc.level0Width, outputDesc.level0Height);
		decompData->imagePtr = 0;
		decompData->tempPtr = 0;
		decompData->imagePosition = 0;

		if ( decompData->imageSize == 0 )
		{
			int a = 0;
		}

		if (decompData->imageSize > 0)
		{
			decompData->imagePtr = new unsigned char[decompData->imageSize];			
		}
		else /*if (surfaceDesc->bytesCount > 0)*/
		{
			if ( surfaceDesc->bytesCount == 0)
			{
				int a = 0;
			}

			decompData->tempPtr = new unsigned char[surfaceDesc->bytesCount];
		}
	}
	else 
	{
		// 計算
		decompData->imagePosition += decompData->surfaceSize;

		if ( decompData->tempPtr == NULL && decompData->imagePosition >= decompData->imageSize && decompData->surfaceSize > 0)
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

	decompData->boInitial = true;	
	if ( decompData && decompData->tempPtr )
	{
		delete [] decompData->tempPtr;		
		decompData->tempPtr = NULL;
	}
}

// Redux Compress function 
void reduxCompressCmdCallback(void* reduxTextureData, size_t reduxTextureDataSize, void* userData)
{
	COMPRESS_DATA* compressData = (COMPRESS_DATA*)userData;
	if ( compressData != NULL && reduxTextureDataSize > 0 )
	{
		compressData->rdtSize = reduxTextureDataSize;
		compressData->rdtData = new unsigned char[reduxTextureDataSize];
		memcpy(compressData->rdtData, reduxTextureData, reduxTextureDataSize);
	}
}

#endif

cImgFile::cImgFile() : cFileObj()
{
	m_fileType		= FILE_TYPE_IMAGE;		// 圖形格式
	m_imageFormat	= IMAGE_UNKNOWN;		// 圖形格式
	m_width			= 0;					// 圖形寬度
	m_height		= 0;					// 圖形高度	
	m_mipmap		= 0;					// mipmap數量 (0或1都代表沒有mipmap)
}

cImgFile::~cImgFile()
{
	Destroy();
}

void cImgFile::Destroy()
{
	cFileObj::Destroy();
	m_imageFormat	= IMAGE_UNKNOWN;		// 圖形格式
	m_width			= 0;					// 圖形寬度
	m_height		= 0;					// 圖形高度
	m_mipmap		= 0;					// mipmap數量 (0或1都代表沒有mipmap)
}

int cImgFile::GetReduceMipmapLevels( int index )
{
	if ( index < 0 || index > 2 )
		return -1;
	return m_reduceMipmap[index];
}

bool cImgFile::ReduceMinmapLevel(int reduceLevel)
{
	// 檢查層級跟資料大小
	if ( reduceLevel > m_mipmapLevels )
		return false;

	// 需要還原為原始容量
	if ( ConvertFormat(COMPRESS_FMT_NORMAL) == false )
		return false;

	int reduceSize = 0;
	int width = m_width;
	int height = m_height;
	for ( int i = 0; i < reduceLevel; i++, width >>= 1, height >>= 1 )
	{
		switch ( m_imageFormat )
		{
		case IMAGE_R5G6B5:
		case IMAGE_A1R5G5B5:
			reduceSize += max(width, 1) * max(height, 1) * 2;
			break;

		case IMAGE_R8G8B8:
			reduceSize += max(width, 1) * max(height, 1) * 3;
			break;

		case IMAGE_A8R8G8B8:
			reduceSize += max(width, 1) * max(height, 1) * 4;
			break;

		case IMAGE_DXT1:
		case IMAGE_DXT1a:
			reduceSize += max(width, MIN_UNIT) * max(height, MIN_UNIT) / 2;
			break;

		case IMAGE_DXT3:
		case IMAGE_DXT5:
			reduceSize += max(width, MIN_UNIT) * max(height, MIN_UNIT);
			break;
		}
	}

	if ( m_dataSize <= reduceSize )
		return false;

	BYTE* pData = new BYTE[m_dataSize - reduceSize];
	memcpy(pData, m_pData + reduceSize, m_dataSize - reduceSize);
	delete [] m_pData;
	m_dataSize	= m_dataSize - reduceSize;
	m_pData	= pData;
	m_width = width;
	m_height = height;

	// 記錄曾經縮減 mipmap 次數以及縮減量
	for ( int i = 0; i < 3; i++ )
	{
		if ( m_reduceMipmap[i] < m_mipmapLevels )
		{
			m_reduceMipmap[i] = m_mipmapLevels;
			break;
		}
	}
	
	m_mipmapLevels = m_mipmapLevels - reduceLevel;	
	return true;
}

bool cImgFile::CreateImage(ImageFormat format, int minmapLevels, int width, int height)
{
	if ( m_pData == NULL )
	{
		m_imageFormat	= format;
		m_mipmap		= 0;
		m_mipmapLevels	= minmapLevels;
		m_width			= width;
		m_height		= height;
		m_dataSize		= CountImageSize(format, minmapLevels, width, height);
		m_pData			= new BYTE[m_dataSize];

		// 產生記憶體失敗,強制當機
		if ( m_pData == NULL ) {
			*m_pData = 0x10;
		}

		return true;
	}

	return false;
}

bool cImgFile::Read( const TCHAR *fileNameStr, ReadImageOptions *pSpecialData )
{
	assert( fileNameStr );

	cImage img;
	if ( !img.Load( fileNameStr ) )
	{
		if ( ReadDDS( fileNameStr, pSpecialData ) )
			return true;
		return false;
	}	
	
	ReadImageOptions info;
	if ( pSpecialData ) info = *pSpecialData;
	if ( info.fmt == IMAGE_UNKNOWN )
	{
		switch (img.GetFormat())
		{
		case PF_R8G8B8:
			info.fmt = IMAGE_R5G6B5;
			break;

		case PF_I8:
		case PF_A8R8G8B8:
		default:
			info.fmt = IMAGE_A8R8G8B8;
			break;
		}
	}

	if ( img.GetDepth() != 32 )
		img.ConvertFormat(PF_A8R8G8B8);

	m_width	= img.GetXSize();
	m_height = img.GetYSize();

	switch ( info.fmt )
	{
		case IMAGE_DXT1:
		case IMAGE_DXT1a:
		case IMAGE_DXT3:
		case IMAGE_DXT5:
		{			
#ifdef ENABLE_NVTT
			// 輸入資料格式設定
			Nvtt_Config nvttConfig;
			DWORD* imageTempData = (DWORD*)img.GetPixels();
			DWORD* newImageData = NULL;
			INT power2W = GetPower2(m_width);
			INT power2H = GetPower2(m_height);			

			// 補足為2的次方大小
			if ( power2W != m_width || power2H != m_height )
			{				
				newImageData = new DWORD[power2W * power2H];
				for ( int y = 0; y < power2H; y++ )
				{
					if ( y < m_height )
					{
						memcpy(newImageData, imageTempData, sizeof(DWORD) * m_width);
						if ( power2W > m_width )
						{
							// 是否有透明值
							if ( img.IsTransparent() )
								memset(&newImageData[m_width + power2H * y], 0, sizeof(DWORD) * power2W - m_width);
							else
								memset(&newImageData[m_width + power2H * y], 0xFF, sizeof(DWORD) * power2W - m_width);
						}
					}
					else
					{
						// 是否有透明值
						if ( img.IsTransparent() )
							memset(&newImageData[power2H * y], 0, sizeof(DWORD) * power2W);
						else
							memset(&newImageData[power2H * y], 0xFF, sizeof(DWORD) * power2W);
					}
				}

				imageTempData = newImageData;
			}

			// 是否做 mipmap 處理
			int mipmapLevels = 0;
			if ( info.mipmapBool )
				mipmapLevels = GetMipmapCount(power2W, power2H);

			if ( mipmapLevels > 0 )
				nvttConfig.SetMipmapGeneration(true, mipmapLevels);
			else
				nvttConfig.SetMipmapGeneration(false);

			nvttConfig.SetSourceData(imageTempData, power2W, power2H);
			nvttConfig.SetQuality(nvtt::Quality_Fastest);

			switch ( info.fmt )
			{
			case IMAGE_DXT1:
			case IMAGE_DXT1a:
				if ( img.IsTransparent() )
					nvttConfig.SetFormat(nvtt::Format_DXT1a);
				else
					nvttConfig.SetFormat(nvtt::Format_DXT1);
				break;

			case IMAGE_DXT3:
				nvttConfig.SetFormat(nvtt::Format_DXT3);
				break;

			case IMAGE_DXT5:
				nvttConfig.SetFormat(nvtt::Format_DXT5);
				break;
			}
			// 計算目的記憶體大小
			if ( m_pData ) delete [] m_pData;
			m_dataSize = CountImageSize(info.fmt, m_mipmapLevels, power2W, power2H);
			m_pData = new BYTE[m_dataSize];

			bool boResult = nvttConfig.ExcuteCompress(m_pData);
			if ( newImageData )	delete [] newImageData;

			if ( !boResult )
			{
				delete [] m_pData;
				m_pData = NULL;
				m_dataSize = 0;

				return false;
			}
			else
			{
				m_imageFormat = info.fmt;
				m_mipmap = mipmapLevels;
				m_width = power2W;
				m_height = power2H;
				return true;
			}

			/*
			nvCompressionOptions options;
			int mipmapLevels = m_mipmapLevels;
			if ( !info.mipmapBool ) options.mipMapGeneration = kNoMipMaps;
			if ( info.fmt == IMAGE_DXT1 ) options.textureFormat = kDXT1;
			else if ( info.fmt == IMAGE_DXT1a ) options.textureFormat = kDXT1a;
			else if ( info.fmt == IMAGE_DXT3 ) options.textureFormat = kDXT3;
			else if ( info.fmt == IMAGE_DXT5 ) options.textureFormat = kDXT5;
			m_pData = CompressDXT( img.GetPixels(), m_width, m_height, m_dataSize, mipmapLevels, &options );
			m_mipmapLevels = mipmapLevels;
			m_imageFormat = info.fmt;
			*/
#endif			
			break;
		}
		case IMAGE_R5G6B5:
		case IMAGE_A1R5G5B5:
		{
			m_dataSize = m_width * m_height * 2;
			m_pData = new BYTE[m_dataSize];
			m_imageFormat = info.fmt;
			Transfrom( m_pData, m_imageFormat, 0, 0, m_width, m_height, m_width, img.GetPixels(), m_width, m_height );
			break;
		}
		case IMAGE_R8G8B8:
		{
			m_dataSize = m_width * m_height * 3;
			m_pData = new BYTE[m_dataSize];
			m_imageFormat = info.fmt;
			Transfrom( m_pData, m_imageFormat, 0, 0, m_width, m_height,m_width, img.GetPixels(), m_width, m_height );
			break;
		}
		default:
		{
			m_dataSize = m_width * m_height * 4;
			m_pData = new BYTE[m_dataSize];
			m_imageFormat = IMAGE_A8R8G8B8;
			memcpy(m_pData, img.GetPixels(), m_dataSize);
			//Transfrom( m_pData, m_imageFormat, 0, 0, m_width, m_height, img.GetPixels(), m_width, m_height );
			break;
		}
	}

	return true;
}

bool cImgFile::Read( const wchar_t *fileNameStr, ReadImageOptions *pSpecialData )
{
	char tempFilenameStr[SAFE_PATH_MAX];

	WideCharToMultiByte(CP_ACP, 0, fileNameStr, -1, tempFilenameStr, SAFE_PATH_MAX, NULL, NULL);

	return Read(tempFilenameStr, pSpecialData);
}

bool cImgFile::ReadDDS( const char *fileNameStr, ReadImageOptions *pSpecialData )
{
	wchar_t wFilenameStr[SAFE_PATH_MAX];

	MultiByteToWideChar(CP_ACP, 0, fileNameStr, -1, wFilenameStr, SAFE_PATH_MAX);

	return ReadDDS(wFilenameStr, pSpecialData);
}

bool cImgFile::ReadDDS( const wchar_t *fileNameStr, ReadImageOptions *pSpecialData )
{
	FILE *fptr;	
	if ( (fptr = _wfopen( fileNameStr, L"rb" )) == NULL )
		return false;

	int magicNumber;
	fread( &magicNumber, sizeof(int), 1, fptr );
	if ( memcmp(&magicNumber, "DDS ", 4) != 0 )
	{
		fclose( fptr );
		return false;
	}

	DDSURFACEDESC2X64 ddsd;
	fread( &ddsd, sizeof(DDSURFACEDESC2X64), 1, fptr );

	int size		= 0;
	m_width			= (int)GetPower2(ddsd.dwWidth);
	m_height		= (int)GetPower2(ddsd.dwHeight);
	m_mipmapLevels	= (int)ddsd.dwMipMapCount;

	if ( m_mipmapLevels <= 0 )
		m_mipmapLevels = 1;
	else
	{
		int max = GetMipmapCount( m_width, m_height );
		if ( m_mipmapLevels > max )
			m_mipmapLevels = max;
	}

	// Determine number of texels in the texture surfaces (including mipmaps)
	int w = max( MIN_UNIT, m_width );
	int h = max( MIN_UNIT, m_height );
	for (int i = 0; i < m_mipmapLevels; i++ )
	{
		size += w * h;
		if ( w > MIN_UNIT ) w >>= 1;
		if ( h > MIN_UNIT ) h >>= 1;
	}

	if ( size <= 0 )
	{
		fclose( fptr );
		return false;
	}

	if(ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB)
	{
		// For 32-bit textures, assume A8R8G8B8
		if(ddsd.ddpfPixelFormat.dwRGBBitCount == 32)
		{
			size *= 4;
			m_imageFormat = IMAGE_A8R8G8B8;
		}
		else
		{
			fclose( fptr );
			return false;
		}
	}
	else if(ddsd.ddpfPixelFormat.dwFlags & DDPF_FOURCC)
	{
		switch ( ddsd.ddpfPixelFormat.dwFourCC )
		{
		case DXT_1:
			size /= 2;
			m_imageFormat = IMAGE_DXT1a;
			break;

		case DXT_3:
			m_imageFormat = IMAGE_DXT3;
			break;

		case DXT_5:
			m_imageFormat = IMAGE_DXT5;
			break;

		default:
			fclose( fptr );
			return false;
		}
	}

	// Read data
	m_dataSize = size;
	m_pData = new BYTE[m_dataSize];
	fread( m_pData, m_dataSize, 1, fptr );

	fclose( fptr );
	return true;
}

bool cImgFile::BltToRGB( void *pDest, ImageFormat format, int x, int y, int width, int height, int pitch, int mipmapLevel )
{
	if ( pDest == NULL || m_pData == NULL ) return false;
	if ( format != IMAGE_R5G6B5 && format != IMAGE_A1R5G5B5 && format != IMAGE_R8G8B8 && format != IMAGE_A8R8G8B8 ) return false;

	RGBQUAD *pPixels = (RGBQUAD*)m_pData;
	RGBQUAD *pNew = NULL;
	if ( m_imageFormat != IMAGE_A8R8G8B8 ) {
		pNew = new RGBQUAD[m_width * m_height];
		switch ( m_imageFormat )
		{
			case IMAGE_R5G6B5:
			{
				WORD *pSrc = (WORD*)m_pData;
				for ( int i = 0; i < m_width * m_height; i++ ) {
					pNew[i].rgbReserved	= 0xFF;
					pNew[i].rgbBlue		= g_5BitsTo8BitsArray[pSrc[i] & 0x1F];
					pNew[i].rgbGreen	= g_6BitsTo8BitsArray[(pSrc[i] >> 5) & 0x3F];
					pNew[i].rgbRed		= g_5BitsTo8BitsArray[(pSrc[i] >> 11) & 0x1F];
				}
				break;
			}
			case IMAGE_A1R5G5B5:
			{
				WORD *pSrc = (WORD*)m_pData;
				for ( int i = 0; i < m_width * m_height; i++ ) {
					pNew[i].rgbReserved	= ( pSrc[i] & 0x8000 ) ? 0xFF : 0;
					pNew[i].rgbBlue		= g_5BitsTo8BitsArray[pSrc[i] & 0x1F];
					pNew[i].rgbGreen	= g_5BitsTo8BitsArray[(pSrc[i] >> 5) & 0x1F];
					pNew[i].rgbRed		= g_5BitsTo8BitsArray[(pSrc[i] >> 10) & 0x1F];
				}
				break;
			}
			case IMAGE_R8G8B8:
			{
				BGRCOLOR *pSrc = (BGRCOLOR*)m_pData;
				for ( int i = 0; i < m_width * m_height; i++ ) {
					pNew[i].rgbBlue		= pSrc[i].r;
					pNew[i].rgbGreen	= pSrc[i].g;
					pNew[i].rgbRed		= pSrc[i].r;
					pNew[i].rgbReserved	= 0xFF;
				}
				break;
			}
			case IMAGE_DXT1a:
			case IMAGE_DXT1:
				PlotDXT1( (BYTE*)pNew, m_pData, m_width, m_height );
				break;
			case IMAGE_DXT3:
				PlotDXT3( (BYTE*)pNew, m_pData, m_width, m_height );
				break;
			case IMAGE_DXT5:
				PlotDXT5( (BYTE*)pNew, m_pData, m_width, m_height );
				break;
		}
		pPixels = pNew;
	}

	if(mipmapLevel == -1)
	{
		Transfrom( pDest, format, x, y, width, height, pitch, pPixels, m_width, m_height );
	}
	else
	{
		// Find texel offset to the required mip level
		int pow_w = GetPower2( m_width );
		int pow_h = GetPower2( m_height );
		BYTE *pSrc = m_pData;
		int size = 0;
		if ( mipmapLevel > 0 )
		{
			int w = max( MIN_UNIT, pow_w );
			int h = max( MIN_UNIT, pow_h );
			for ( int levels = min( mipmapLevel, m_mipmapLevels ); levels > 0; levels-- )
			{
				size += w * h;
				if ( w > MIN_UNIT ) w >>= 1;
				if ( h > MIN_UNIT ) h >>= 1;
				if ( pow_w > MIN_UNIT ) pow_w >>= 1;
				if ( pow_h > MIN_UNIT ) pow_h >>= 1;
			}
		}

		// Multiply by texel pitch
		switch ( m_imageFormat )
		{
			case IMAGE_R5G6B5:
			case IMAGE_A1R5G5B5:
				size *= 2;
				break;

			case IMAGE_R8G8B8:
				size *= 3;
				break;

			case IMAGE_A8R8G8B8:
				size *= 4;
				break;
		}

		// Offset the source pointer
		pPixels = (RGBQUAD *) ((BYTE *) pPixels + size);

		// Copy
		Transfrom( pDest, format, x, y, width, height, pitch, pPixels, pow_w, pow_h );
	}

	if ( pNew ) delete [] pNew;

	return true;
}

inline DWORD TransferFormatType( ImageFormat format )
{
	if ( format == IMAGE_DXT1 ) return TEXTURE_DXT1;		
	else if ( format == IMAGE_DXT1a ) return TEXTURE_DXT1a;
	else if ( format == IMAGE_DXT3 ) return TEXTURE_DXT3;
	else if ( format == IMAGE_DXT5 ) return TEXTURE_DXT5;
	return format;
}

bool cImgFile::BltToDXT( void *pDest, ImageFormat format, int x, int y, int width, int height, int mipmapLevels, DWORD color, DWORD rop )
{
	if ( pDest == NULL || m_pData == NULL || !IsDXTFormat(format) || !IsDXTFormat(m_imageFormat) ) return false;
	
	int pow_w = GetPower2( m_width );
	int pow_h = GetPower2( m_height );
	BYTE *pSrc = m_pData;
	int size = 0;
	if ( mipmapLevels > 0 ) {
		int w = max( MIN_UNIT, pow_w );
		int h = max( MIN_UNIT, pow_h );
		for ( int levels = min( mipmapLevels, m_mipmapLevels ); levels > 0; levels-- ) {
			size += w * h;
			if ( w > MIN_UNIT ) w >>= 1;
			if ( h > MIN_UNIT ) h >>= 1;
			if ( pow_w > MIN_UNIT ) pow_w >>= 1;
			if ( pow_h > MIN_UNIT ) pow_h >>= 1;
		}
	}
	if ( m_imageFormat == IMAGE_DXT1 || m_imageFormat == IMAGE_DXT1a ) size /= 2;
	pSrc += size;

	return BltDXT( pDest, TransferFormatType(format), x, y, width, height, pSrc, TransferFormatType(m_imageFormat), pow_w, pow_h, color, rop );
}

#ifdef _REDUX

// Initialize Redux processor
bool cImgFile::ReduxInitialize()
{	
	unsigned int res;
#ifdef REDUX_COMPRESS
#ifndef USE_REDUX_EXE	
	res = reduxCmdProcessorInit();
	assert(res == 0);

	// set callbacks
	res = reduxCmdProcessorSetCallback( Redux_CmdCallback_Compress, (void*)reduxCompressCmdCallback);
	assert(res==0);	
#endif
#endif	
	// set callbacks
	res = reduxCallbackSet(
		Redux_Callback_OutputGetBuffer,
		(void*)reduxDecompressCallbackOutputGetBuffer);
	assert(res==0);

	res = reduxCallbackSet(
		Redux_Callback_OutputCompleted,
		(void*)reduxDecompressCallbackOutputCompleted);
	assert(res==0);

	return true;
}

// Initialize Redux processor
bool cImgFile::ReduxExit()
{
#ifdef REDUX_COMPRESS
#ifndef USE_REDUX_EXE
	return reduxCmdProcessorExit() == 0;
#endif
#endif
	return true;
}

bool cImgFile::ConvertFormat( CompressFormat fileFormat )
{	
	if ( fileFormat == m_compressFormat ) return true;
	if ( m_dataSize <= 0 ) return false;

	switch ( fileFormat )
	{
	case COMPRESS_FMT_NORMAL:
		if ( m_compressFormat == COMPRESS_FMT_RDT )
		{
			DECOMPRESS_DATA decompressionData;
			unsigned int res;

			memset(&decompressionData, 0, sizeof(decompressionData));
			decompressionData.filename = m_fileNameStr;
			
			/*
			// set callbacks
			res = reduxCallbackSet(
				Redux_Callback_OutputGetBuffer,
				(void*)reduxDecompressCallbackOutputGetBuffer);
			assert(res==0);

			res = reduxCallbackSet(
				Redux_Callback_OutputCompleted,
				(void*)reduxDecompressCallbackOutputCompleted);
			assert(res==0);
			*/

			res = reduxHandleDecompress( GetData(), m_compressSize, (size_t)&decompressionData);
			if ( res == 0 && decompressionData.imagePtr != 0 )
			{
				delete [] m_pData;
				m_compressFormat	= COMPRESS_FMT_NORMAL;
				m_compressSize		= 0;
				m_pData				= decompressionData.imagePtr;
				m_dataSize			= decompressionData.imageSize;
				m_imageFormat		= reduxGetImageFormat(decompressionData.outputDesc);
				m_mipmapLevels		= decompressionData.outputDesc.mipmapCount;
				m_height			= decompressionData.outputDesc.level0Height;
				m_width				= decompressionData.outputDesc.level0Width;
				return true;
			}
			else
			{
				return false;
			}
		}
		break;

	case COMPRESS_FMT_RDT:
#ifdef REDUX_COMPRESS
		// 先轉回原始格式
		if ( m_compressFormat != COMPRESS_FMT_NORMAL && !ConvertFormat( COMPRESS_FMT_NORMAL ) ) {
			return false;
		} else {

#ifdef REDUX_DONT_ARGB
			if ( m_imageFormat == IMAGE_R5G6B5 || m_imageFormat == IMAGE_A1R5G5B5 || m_imageFormat == IMAGE_R8G8B8 || m_imageFormat == IMAGE_A8R8G8B8 )
			{
				fileFormat = COMPRESS_FMT_LZO;
				break;
			}
#endif

#ifdef REDUX_DONT_DXT1A
			// if ( m_imageFormat == IMAGE_DXT1a || m_imageFormat == IMAGE_DXT1 )
			if ( (m_imageFormat == IMAGE_DXT1a || m_imageFormat == IMAGE_DXT1) && IsTransparent() )
			{			
				fileFormat = COMPRESS_FMT_LZO;
				break;
			}			
#endif
			unsigned int res = 0;
			bool isCreateTempFile = false;
			bool boSuccess = false;
			TCHAR tCurrentDirectory[SAFE_PATH_MAX];
			TCHAR tTempDdsFilename[SAFE_PATH_MAX];
			TCHAR tTempRdtFilename[SAFE_PATH_MAX];

			// 如果真實檔案不存在或者不是DDS格式,則另存新檔
			if ( m_realFilename == NULL || CheckExtensionName(m_realFilename, ".dds") )
			{
				GetCurrentDirectory(SAFE_PATH_MAX, tCurrentDirectory);
				
				sprintf_s(tTempDdsFilename, SAFE_PATH_MAX, "%s\\temp\\%s", tCurrentDirectory, m_fileNameStr);

				if ( !cFileObj::CreateFilePath(tTempDdsFilename) || !SaveToFile(tTempDdsFilename) )
				{
					return false;
				}

				isCreateTempFile = true;
			}
			else
			{
				strcpy(tTempDdsFilename, m_realFilename);
			}

			strcpy(tTempRdtFilename, tTempDdsFilename);
			const char* lastDot = strrchr(tTempRdtFilename, '.');
			if ( lastDot )
				strcpy((char*)lastDot, ".rdt");
			else
				strcat(tTempRdtFilename, ".rdt");

			// version 1
#ifdef USE_REDUX_EXE			
			char parameters[SAFE_PATH_MAX];			
			sprintf_s(parameters, SAFE_PATH_MAX, "compress \"%s\" \"%s\" 1", tTempDdsFilename, tTempRdtFilename);
			
			SHELLEXECUTEINFO ShExecInfo = {0};
			ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
			ShExecInfo.hwnd = NULL;
			ShExecInfo.lpVerb = NULL;
			ShExecInfo.lpFile = "reduxcommandline.exe";
			ShExecInfo.lpParameters = parameters;
			ShExecInfo.lpDirectory = NULL;
			ShExecInfo.nShow = SW_HIDE;
			ShExecInfo.hInstApp = NULL;

			if ( ShellExecuteEx(&ShExecInfo) )
			{
				WaitForSingleObject(ShExecInfo.hProcess, INFINITE);		
				HANDLE hFile = CreateFile( tTempRdtFilename, GENERIC_READ, NULL, NULL, OPEN_EXISTING, 0, NULL );
				if ( hFile != INVALID_HANDLE_VALUE )
				{
					DWORD dataSize = GetFileSize( hFile, NULL );		// 檔案大小
					if ( dataSize > 0 )
					{
						DWORD readBytesOut;
						BYTE* pData = new BYTE[dataSize];
						if ( ReadFile( hFile, pData, dataSize, &readBytesOut, NULL ) )
						{
							boSuccess = TRUE;

							delete [] m_pData;
							m_pData = pData;
							m_compressSize = dataSize;
							m_compressFormat = COMPRESS_FMT_RDT;						
						}
						else
						{
							delete [] pData;
						}
					}
					CloseHandle(hFile);
				}
			}

			// 刪除暫存檔
			if ( isCreateTempFile )	DeleteFile(tTempDdsFilename);
			DeleteFile(tTempRdtFilename);
#else			
			// version 2
			//res = reduxCmdProcessorInit();
			//assert(res == 0);

			//res = reduxCmdProcessorSetCallback( Redux_CmdCallback_Compress, (void*)reduxCompressCmdCallback);
			//assert(res==0);

			COMPRESS_DATA compressData;
			memset(&compressData, 0, sizeof(compressData));
			wchar_t parameters[1024];
			wchar_t wTempDdsFilename[SAFE_PATH_MAX];
			wchar_t wTempRdtFilename[SAFE_PATH_MAX];
			memset(parameters, 0, sizeof(parameters));
			MultiByteToWideChar(CP_ACP, 0, tTempDdsFilename, -1, wTempDdsFilename, SAFE_PATH_MAX);
			MultiByteToWideChar(CP_ACP, 0, tTempRdtFilename, -1, wTempRdtFilename, SAFE_PATH_MAX);
			
			compressData.outputFilename = tTempRdtFilename;
			swprintf_s(parameters, 1024, L"compress \"%s\" \"%s\" 1", wTempDdsFilename, wTempRdtFilename);
			// Run command line using Redux processor
			if ( (res = reduxCmdProcessorRun(parameters, (void*)&compressData) == 0) && compressData.rdtData != NULL )
			{
				boSuccess = TRUE;

				delete [] m_pData;
				m_pData = compressData.rdtData;
				m_compressSize = compressData.rdtSize;
				m_compressFormat = COMPRESS_FMT_RDT;
			}
			else
			{
				int a = 0;
			}

			//res = reduxCmdProcessorExit();

			// 刪除暫存檔
			if ( isCreateTempFile )	DeleteFile(tTempDdsFilename);
			// DeleteFile(tTempRdtFilename);
#endif
			return boSuccess;
		}
#else
		return false;
#endif		
		break;
	}

	return cFileObj::ConvertFormat(fileFormat);
}

#endif

void cImgFile::SaveHeader( cListData &header )
{
	cFileObj::SaveHeader( header );	
	header.Push( &m_imageFormat, sizeof(ImageFormat) );
	header.Push( &m_width, sizeof(int) );
	header.Push( &m_height, sizeof(int) );
	header.Push( &m_mipmap, sizeof(int) );
}

bool cImgFile::LoadHeader( HANDLE hFile )
{
	DWORD readBytesOut;
	if ( cFileObj::LoadHeader( hFile ) == false )
		return false;

	if ( m_fileType == FILE_TYPE_IMAGE )
	{
		ReadFile( hFile, &m_imageFormat, sizeof(ImageFormat), &readBytesOut, NULL );
		switch (m_imageFormat)
		{
		case IMAGE_UNKNOWN:
		case IMAGE_R5G6B5:
		case IMAGE_A1R5G5B5:
		case IMAGE_R8G8B8:
		case IMAGE_A8R8G8B8:
		case IMAGE_DXT1:
		case IMAGE_DXT1a:
		case IMAGE_DXT3:
		case IMAGE_DXT5:
			break;

		default:
			return false;
		}

		ReadFile( hFile, &m_width, sizeof(int), &readBytesOut, NULL );
		if ( m_width < 0 || m_width > 0x2000 )
			return false;

		ReadFile( hFile, &m_height, sizeof(int), &readBytesOut, NULL );
		if ( m_height < 0 || m_height > 0x2000 )
			return false;

		ReadFile( hFile, &m_mipmap, sizeof(int), &readBytesOut, NULL );
		if ( m_mipmapLevels < 0 || m_mipmapLevels > 0x10 )
			return false;		

		if ( !IsPower2(m_width) || !IsPower2(m_height) )
			return false;
	}
	return true;
}

void cImgFile::WriteData( HANDLE hFile )
{
	DWORD writeBytesOut;	

	// if ( m_mipmapLevels > 1 )
	{
		DWORD dwMagic = 0x20534444;			// "DDS "

		DDSURFACEDESC2 ddsd;
		memset(&ddsd, 0, sizeof(ddsd));

		ddsd.dwSize			= sizeof(ddsd);
		ddsd.dwWidth		= m_width;
		ddsd.dwHeight		= m_height;
		ddsd.dwMipMapCount	= m_mipmapLevels;		

		if ( m_mipmapLevels > 1 )
			ddsd.dwFlags	= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT | DDSD_LINEARSIZE;
		else
			ddsd.dwFlags	= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_LINEARSIZE;

		//ddsd.dwRefreshRate	= 2;
		//ddsd.dwSrcVBHandle	= 2;

		// DDPIXELFORMAT
		ddsd.ddpfPixelFormat.dwSize		= sizeof(DDPIXELFORMAT);		

		// DDS
		switch (m_imageFormat)
		{
		case IMAGE_R5G6B5:
			ddsd.lPitch		= m_width * m_height * 2;
			ddsd.ddpfPixelFormat.dwFlags		= DDPF_RGB;
			ddsd.ddpfPixelFormat.dwRGBBitCount	= 16;
			break;
		case IMAGE_A1R5G5B5:
			ddsd.lPitch		= m_width * m_height * 2;
			ddsd.ddpfPixelFormat.dwFlags		= DDPF_RGB;
			ddsd.ddpfPixelFormat.dwRGBBitCount	= 16;
			break;
		case IMAGE_R8G8B8:
			ddsd.lPitch		= m_width * m_height * 3;
			ddsd.ddpfPixelFormat.dwFlags		= DDPF_RGB;
			ddsd.ddpfPixelFormat.dwRGBBitCount	= 24;
			ddsd.ddpfPixelFormat.dwRBitMask		= 0x00ff0000;
			ddsd.ddpfPixelFormat.dwGBitMask		= 0x0000ff00;
			ddsd.ddpfPixelFormat.dwBBitMask		= 0x000000ff;
			break;
		case IMAGE_A8R8G8B8:
			ddsd.lPitch		= m_width * m_height * 4;
			ddsd.ddpfPixelFormat.dwFlags		= DDPF_RGB | DDPF_ALPHAPIXELS;
			ddsd.ddpfPixelFormat.dwRGBBitCount	= 32;
			ddsd.ddpfPixelFormat.dwRBitMask		= 0x00ff0000;
			ddsd.ddpfPixelFormat.dwGBitMask		= 0x0000ff00;
			ddsd.ddpfPixelFormat.dwBBitMask		= 0x000000ff;
			ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
			break;
		case IMAGE_DXT1:
			ddsd.lPitch		= m_width * m_height / 2;
			ddsd.ddpfPixelFormat.dwFlags	= DDPF_FOURCC;
			ddsd.ddpfPixelFormat.dwFourCC	= 0x31545844;				// DXT1
			break;
		case IMAGE_DXT1a:
			ddsd.lPitch		= m_width * m_height / 2;
			ddsd.ddpfPixelFormat.dwFlags	= DDPF_FOURCC;
			ddsd.ddpfPixelFormat.dwFourCC	= 0x31545844;				// DXT1
			break;
		case IMAGE_DXT3:
			ddsd.lPitch		= m_width * m_height;
			ddsd.ddpfPixelFormat.dwFlags	= DDPF_FOURCC;
			ddsd.ddpfPixelFormat.dwFourCC	= 0x33545844;				// DXT3
			break;
		case IMAGE_DXT5:
			ddsd.lPitch		= m_width * m_height;
			ddsd.ddpfPixelFormat.dwFlags	= DDPF_FOURCC;
			ddsd.ddpfPixelFormat.dwFourCC	= 0x35545844;				// DXT5
			break;
		}
		ddsd.dwLinearSize	= ddsd.lPitch;		

		// 判斷DDS檔案
		WriteFile( hFile, &dwMagic, sizeof(DWORD), &writeBytesOut, NULL );

		// DDS 檔頭
		WriteFile( hFile, &ddsd, sizeof(DDSURFACEDESC2), &writeBytesOut, NULL );
	}
	/*
	else
	{
		//TGA
		//cImage image;

		//image.Create(m_width, m_height, 32, IMAGELIB_FORMAT_TGA);

		//BltToRGB(image.GetPixels(), IMAGE_A8R8G8B8, 0, 0, m_width, m_height, m_width * 4);
	}
	*/

	cFileObj::WriteData(hFile);
}

bool cImgFile::ReadFileData(HANDLE hFile, const char* fullNameStrh, void* imageOptions)
{
	if ( Read(fullNameStrh, (ReadImageOptions*)imageOptions) )
	{
		return true;
	}	

	return false;
}

bool cImgFile::Transfrom( void *pDest, ImageFormat format, int x, int y, int width, int height, int pitch, void *pSrc, int srcWidth, int srcHeight)
{
	RGBQUAD *pPixels = (RGBQUAD *)pSrc;
	switch ( format )
	{
		case IMAGE_R5G6B5:
		case IMAGE_A1R5G5B5:
		{
			WORD *pLine = (WORD*)pDest + y * (pitch / 2);
			for ( int yy = 0; yy < srcHeight; yy++ ) {
				if ( yy + y >= height ) break;
				for ( int xx = 0; xx < srcWidth; xx++ ) {
					if ( xx + x >= width ) break;
					if ( format == IMAGE_R5G6B5 ) {
						pLine[xx + x] = RGBToR5G6B5( pPixels[xx].rgbRed, pPixels[xx].rgbGreen, pPixels[xx].rgbBlue );
					} else if ( format == IMAGE_A1R5G5B5 ) {
						pLine[xx + x] = RGBToR5G6B5( pPixels[xx].rgbRed, pPixels[xx].rgbGreen, pPixels[xx].rgbBlue );
						if ( pPixels[xx].rgbReserved & 0x80 ) pLine[xx + x] |= 0x8000;
					}
				}
				pPixels += srcWidth;
				pLine += (pitch / 2);
			}
			break;
		}
		case IMAGE_R8G8B8:
		{
			// We do not support 24 bit
			assert(FALSE);

			BGRCOLOR *pLine = (BGRCOLOR*)pDest + y * width;
			for ( int yy = 0; yy < srcHeight; yy++ ) {
				if ( yy + y >= height ) break;
				for ( int xx = 0; xx < srcWidth; xx++ ) {
					if ( xx + x >= width ) break;
					pLine[xx].r = pPixels[xx].rgbBlue;
					pLine[xx].g = pPixels[xx].rgbGreen;
					pLine[xx].b = pPixels[xx].rgbRed;
				}
				pPixels += srcWidth;
				pLine += width;				
			}
			break;
		}
		case IMAGE_A8R8G8B8:
		{
			RGBQUAD* pLine = (RGBQUAD*)pDest + y * (pitch / 4);
			for ( int yy = 0; yy < srcHeight; yy++ ) {
				if ( yy + y >= height ) break;
				memcpy( pLine + x, pPixels, sizeof(RGBQUAD) * min(width - x, srcWidth) );
				pLine += pitch / 4;
				pPixels += srcWidth;
			}
			break;
		}
		default:
			return false;
	}
	return true;
}

bool cImgFile::IsTransparent()
{
	bool isTransparent = false;
	bool transformDXT1 = false;
	switch ( m_imageFormat )
	{
	case IMAGE_R5G6B5:
		isTransparent = false;
		break;

	case IMAGE_A1R5G5B5:
		isTransparent = true;
		break;

	case IMAGE_R8G8B8:
		isTransparent = false;
		break;

	case IMAGE_A8R8G8B8:
		isTransparent = true;
		break;

	case IMAGE_DXT1:
	case IMAGE_DXT1a:
		isTransparent = IsTransparentDXT1(m_pData, m_width, m_height);
		break;
	
	case IMAGE_DXT3:
		isTransparent = true;
		break;

	case IMAGE_DXT5:
		transformDXT1 = CheckTransformDXT5ToDXT1(m_pData, m_width, m_height, isTransparent);
		break;
	}
	return isTransparent;
}