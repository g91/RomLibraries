#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <ddraw.h>
#include "../imagelib/cImage.h"
#include "cImgFile.h"
#include "RDTLoader.h"

/*
#ifdef _DEBUG
  #pragma comment(lib, "FilePackage\\imagelib\\imagelib_d.lib")
#else
  #pragma comment(lib, "FilePackage\\imagelib\\imagelib.lib")
#endif *//*_DEBUG*/
struct RedUXWrapModule
{
	HMODULE  m_lib;

	IRDTDecode* m_pInterface;
	bool IsWork()
	{

		return (m_pInterface)? true:false;
	}
	bool Decompress(DECOMPRESS_DATA& data,cImgFile* file );

	void ReleaseData(DECOMPRESS_DATA& data);
	
	void Initial(void);
	void Finish(void);
	RedUXWrapModule():m_lib(NULL),m_pInterface(NULL)
	{

	}
	~RedUXWrapModule()
	{
		
		m_pInterface=NULL;
		m_lib=NULL;
	}

	
};
void  RedUXWrapModule::ReleaseData(DECOMPRESS_DATA& data)
{
	if (m_pInterface)	
		 //m_pInterface->ReleaseData(data); // mupu
		 m_pInterface->Release();

	
}
bool  RedUXWrapModule:: Decompress(DECOMPRESS_DATA& data,cImgFile* file )
{

	
	if (m_pInterface)	
	return m_pInterface->Decompress(data,file->GetData(),file->GetCompressSize());
		
	return false;
}


void RedUXWrapModule::Initial(void)
{

	HMODULE lib=NULL;
	lib= LoadLibraryA("RDTLoader.dll");
	if (lib)
	{
		
		GETIRDTDECODE GetIRDT =(GETIRDTDECODE)GetProcAddress(lib,"GetIRDT");
		if (GetIRDT)
		{
			IRDTDecode* pInterface=GetIRDT();
			if (pInterface)
				m_pInterface=pInterface;
		}
	}
	m_lib=lib;
}
void RedUXWrapModule::Finish(void)
{
	if (m_pInterface)
	{
		m_pInterface->Release();

		m_pInterface=0;
	}

	if (m_lib)
		FreeLibrary(m_lib);
	m_lib=0;

	

}
cImgFile::cImgFile() : cFileObj()
{
	m_fileType		= FILE_TYPE_IMAGE;		// 圖形格式
	m_imageFormat	= IMAGE_UNKNOWN;		// 圖形格式
	m_width			= 0;					// 圖形寬度
	m_height		= 0;					// 圖形高度
	m_mipmapLevels	= 0;					// mipmap數量 (0或1都代表沒有mipmap)
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
	m_mipmapLevels	= 0;					// mipmap數量 (0或1都代表沒有mipmap)
}
int cImgFile::GetReduceMipmapLevels( int index )
{
	if ( index < 0 || index > 2 )
		return -1;
	return m_reduceMipmap[index];
}
bool cImgFile::LoadFromFile( const char  *workDirStr, const char *fileNameStr, CompressFormat format, void *pSpecialData )
{
	Destroy();

	char fullNameStr[MAX_PATH];
	sprintf( fullNameStr, "%s%s", workDirStr, fileNameStr );
	if ( !Read( fullNameStr, (ReadImageOptions*)pSpecialData ) ) return false;

	
	HANDLE hFile;
	sprintf( fullNameStr, "%s%s", workDirStr, fileNameStr );
	if ( (hFile = CreateFileA( fullNameStr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL ) ) == INVALID_HANDLE_VALUE ) {
		return false;
	}
	

	m_fileType			= FILE_TYPE_IMAGE;
	m_compressFormat	= COMPRESS_FMT_NORMAL;

	//m_dataSize			= GetFileSize( hFile, NULL );		// 檔案大小
	/*
	if (m_pData)
		delete []m_pData;
	m_pData				= new BYTE[m_dataSize];
	*/
	::GetFileTime( hFile, NULL, NULL, &m_fileTime );

	//SYSTEMTIME time;
	//FileTimeToSystemTime(&m_fileTime,&time);
	//DWORD readBytesOut;

	// 讀取檔案資料	
	//ReadFile( hFile, m_pData, m_dataSize, &readBytesOut, NULL );

	CloseHandle( hFile );

	//SetFileName( fileNameStr );
	ConvertFormat( format );			// 轉換為指定壓縮格式

	return true;
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
		m_mipmapLevels	= minmapLevels;
		m_width			= width;
		m_height		= height;
		m_dataSize		= CountImageSize(format, minmapLevels, width, height);
		m_pData			= new BYTE[m_dataSize];

		return true;
	}

	return false;
}


bool cImgFile::Read( const char *fileNameStr, ReadImageOptions *pSpecialData )
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

	img.Destroy();

	return true;
}


static RedUXWrapModule* g_ReduxWrap;
struct  AutoLoaderRedUXWrap
{
	AutoLoaderRedUXWrap()
	{
		if (!g_ReduxWrap)
		{
			
			g_ReduxWrap=new RedUXWrapModule();
			g_ReduxWrap->Initial();

		}
	}
	~AutoLoaderRedUXWrap()
	{
		if (g_ReduxWrap)
		{
			g_ReduxWrap->Finish();

			delete g_ReduxWrap;
		}
		g_ReduxWrap=NULL;
	}

};

bool cImgFile::ConvertFormat( CompressFormat fileFormat )
{
	if ( m_dataSize <= 0 ) return false;
	if ( fileFormat == m_compressFormat ) return true;

	switch ( fileFormat )
	{
	case COMPRESS_FMT_NORMAL:
		

		
		if ( m_compressFormat == COMPRESS_FMT_RDT )
		{
			static AutoLoaderRedUXWrap autoMMM;
			
			DECOMPRESS_DATA decompressionData;

			if (g_ReduxWrap->Decompress(decompressionData,this))

			{



				delete [] m_pData;
				m_compressFormat	= COMPRESS_FMT_NORMAL;
				m_compressSize		= 0;
				m_dataSize			= decompressionData.imageSize;
				m_pData				= new BYTE [m_dataSize];
				memcpy(m_pData,decompressionData.imagePtr,m_dataSize);				
				m_imageFormat		=decompressionData.imageFormat;
				m_mipmapLevels		= decompressionData.outputDesc.mipmapCount;
				m_height			= decompressionData.outputDesc.level0Height;
				m_width				= decompressionData.outputDesc.level0Width;

				g_ReduxWrap->ReleaseData(decompressionData);

				return true;
			}			

		
	
		}
		else
			return false;
		break;

	case COMPRESS_FMT_RDT:
		// 先轉回原始格式
		if ( m_compressFormat != COMPRESS_FMT_NORMAL && !ConvertFormat( COMPRESS_FMT_NORMAL ) ) {
			return false;
		} else {
			bool isCreateTempFile = false;
			bool boSuccess = false;
			char tCurrentDirectory[MAX_PATH];
			char tTempDdsFilename[MAX_PATH];
			char tTempRdtFilename[MAX_PATH];

			// 如果真實檔案不存在或者不是DDS格式,則另存新檔
			if ( m_reladFilename == NULL || CheckExtensionName(m_reladFilename, ".dds") )
			{				
				GetCurrentDirectoryA(MAX_PATH, tCurrentDirectory);

				sprintf_s(tTempDdsFilename, MAX_PATH, "%s\\temp\\%s", tCurrentDirectory, m_fileNameStr);

				if ( !cFileObj::CreateFilePath(tTempDdsFilename) || !SaveToFile(tTempDdsFilename) )
					return false;

				isCreateTempFile = true;
			}
			else
			{
				strcpy(tTempDdsFilename, m_reladFilename);
			}

			strcpy(tTempRdtFilename, tTempDdsFilename);
			const char* lastDot = strrchr(tTempRdtFilename, '.');
			if ( lastDot )
				strcpy((char*)lastDot, ".rdt");
			else
				strcat(tTempRdtFilename, ".rdt");			

			char parameters[512];
			sprintf_s(parameters, 512, "compress \"%s\" \"%s\"", tTempDdsFilename, tTempRdtFilename);

			SHELLEXECUTEINFOA ShExecInfo = {0};
			ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
			ShExecInfo.hwnd = NULL;
			ShExecInfo.lpVerb = NULL;
			ShExecInfo.lpFile = "reduxcommandline.exe";
			ShExecInfo.lpParameters = parameters;
			ShExecInfo.lpDirectory = NULL;
			ShExecInfo.nShow = SW_HIDE;
			ShExecInfo.hInstApp = NULL;
			if ( ShellExecuteExA(&ShExecInfo) )
			{
				WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

				HANDLE hFile = CreateFileA( tTempRdtFilename, GENERIC_READ, NULL, NULL, OPEN_EXISTING, 0, NULL );
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
			if ( isCreateTempFile )	DeleteFileA(tTempDdsFilename);
			DeleteFileA(tTempRdtFilename);

			return boSuccess;
		}
	}

	return cFileObj::ConvertFormat(fileFormat);
}

bool cImgFile::ReadDDS( const char *fileNameStr, ReadImageOptions *pSpecialData )
{
	FILE *fptr;	
	if ( (fptr = fopen( fileNameStr, "rb" )) == NULL )
		return false;

	int magicNumber;
	fread( &magicNumber, sizeof(int), 1, fptr );
	if ( memcmp(&magicNumber, "DDS ", 4) != 0 )
	{
		fclose( fptr );
		return false;
	}

	DDSURFACEDESC2 ddsd;
	fread( &ddsd, sizeof(DDSURFACEDESC2), 1, fptr );

	int size		= 0;
	m_width			= (int)ddsd.dwWidth;
	m_height		= (int)ddsd.dwHeight;
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
	int w = max( MIN_UNIT, ddsd.dwWidth );
	int h = max( MIN_UNIT, ddsd.dwHeight );
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
		int bytes=ddsd.ddpfPixelFormat.dwRGBBitCount/8;

		size *=bytes;
		if(ddsd.ddpfPixelFormat.dwRGBBitCount == 32)
			m_imageFormat = IMAGE_A8R8G8B8;

		else if( ddsd.ddpfPixelFormat.dwRGBBitCount == 24)
		{


			m_imageFormat = IMAGE_R8G8B8;
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
	if ( format == IMAGE_DXT1 ) return kDXT1;		
	else if ( format == IMAGE_DXT1a ) return kDXT1a;
	else if ( format == IMAGE_DXT3 ) return kDXT3;
	else if ( format == IMAGE_DXT5 ) return kDXT5;
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
/*
int cImgFile::LoadFromData( HANDLE hFile, BOOL loadDataBool )
{

}*/
void cImgFile::SaveHeader( cListData &header )
{
	cFileObj::SaveHeader( header );	
	header.Push( &m_imageFormat, sizeof(ImageFormat) );
	header.Push( &m_width, sizeof(int) );
	header.Push( &m_height, sizeof(int) );
	header.Push( &m_mipmap, sizeof(int) );
}
inline bool IsPower2(unsigned int x)
{              
	if ( x < 1 )return false;
	if (x == 1)	return true;
	if ( x & (x-1) ) return false;
	return true;
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
