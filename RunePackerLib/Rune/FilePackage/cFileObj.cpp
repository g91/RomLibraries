#include <windows.h>
#include <stdio.h>

#include "cFileObj.h"


#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

bool cFileObj::CreateFilePath(const char* fullname)
{
	if ( fullname == NULL )
		return false;	

	const char* lastPos = strchr(fullname, '\\');
	while ( lastPos != NULL )
	{
		char temp[MAX_PATH];
		int size = lastPos - fullname;
		memcpy(temp, fullname, size);
		temp[size] = 0;
		if ( !PathIsDirectoryA( temp ) )
		{
			if ( !CreateDirectoryA( temp, NULL ) )
				return false;
		}
		lastPos = strchr(lastPos + 1, '\\');
	}

	return true;	
}
//tProcessDataProc cFileObj::m_pProc;
cFileObj::cFileObj()
{
	m_reladFilename		= NULL;
	m_fileType			= FILE_TYPE_GENERAL;	
	m_compressFormat	= COMPRESS_FMT_NORMAL;
	m_dataSize			= 0;	
	m_compressSize		= 0;
	m_fileNameStr		= NULL;
	m_pData				= NULL;	

	memset( &m_fileTime, 0, sizeof(FILETIME) );
#if RUSTREAMIO
	m_position			= 0;
#endif
}

cFileObj::~cFileObj()
{
	Destroy();
}

#if RUSTREAMIO
int cFileObj::EndOfStream()
{
	if ( !IsOpen() || m_position == m_dataSize ) {
		return 1;
	}
	return 0;
}

DWORD cFileObj::Read( void *destBuffer, DWORD readSize )
{
	if ( !IsOpen() || m_dataSize == 0 || m_position >= m_dataSize ) return 0;
	if ( (int)readSize > m_dataSize - m_position ) {
		readSize = m_dataSize - m_position;
	}
	memcpy( destBuffer, m_pData + m_position, readSize );
	m_position += readSize;
	return readSize;
}

RuStreamErrorCode cFileObj::Seek(INT offset, RuStreamSeekMethod seekType)
{
	if ( !IsOpen() ) return ruSEC_SeekError;	

	switch(seekType)
	{
		case ruSSM_Begin:
		{		
			if( offset < 0 || offset > m_dataSize ) return ruSEC_SeekError;
			m_position = offset;
			break;
		}
		case ruSSM_Current:
		{		
			if( (m_position + offset < 0) || (m_position + offset > m_dataSize) ) return ruSEC_SeekError;
			m_position += offset;
			break;
		}
		case ruSSM_End:
		{
			if( (m_dataSize + offset < 0) || (m_dataSize + offset > m_dataSize) ) return ruSEC_SeekError;
			m_position = m_dataSize + offset;
			break;
		}
		default:
			return ruSEC_SeekError;
	}

	return ruSEC_OK;
}

#endif //#if RUSTREAMIO

void cFileObj::Destroy( void )
{
	m_fileType			= FILE_TYPE_GENERAL;
	m_compressFormat	= COMPRESS_FMT_NORMAL;
	m_dataSize			= 0;
	m_compressSize		= 0;
	memset( &m_fileTime, 0, sizeof(FILETIME) );

#if RUSTREAMIO
	m_position			= 0;
#endif

	if ( m_fileNameStr ) {
		delete [] m_fileNameStr;
		m_fileNameStr = NULL;
	}
	if ( m_pData ) {
		delete [] m_pData;
		m_pData = NULL;
	}
}

void cFileObj::SetFileName( const char *fileNameStr )
{
	if ( fileNameStr ) {		
		if ( m_fileNameStr ) delete [] m_fileNameStr;
		if ( *fileNameStr == _T('\\') ) {
			m_fileNameStr = new char[strlen(fileNameStr + 1) + 1];
			strcpy( m_fileNameStr, fileNameStr + 1 );
		} else {
			m_fileNameStr = new char[strlen(fileNameStr) + 1];
			strcpy( m_fileNameStr, fileNameStr );
		}
	}
}
#define BUF_SIZE 128*1024 // 128 KB buffer
static char dataBuff[BUF_SIZE];

int cFileObj::SaveData( HANDLE hFile, tProcessDataProc ProcessDataProc ,void *data)
{
	if ( hFile == INVALID_HANDLE_VALUE || m_pData == NULL ) {
		return false;
	}
	
	int size = sizeof(int);
	if ( m_compressFormat == COMPRESS_FMT_NORMAL ) {
		size += m_dataSize;
	} else {
		size += m_compressSize;
	}

	cListData header;
	SaveHeader( header );
	size += header.Size();

	/*
	* 存檔結構說明
	* 主要分為三大部份
	* 1.記錄區塊大小(int)
	* 2.檔頭部份,包函所有參數數值
	* 3.資料區塊,記錄檔案名稱及檔案資料
	*/

	DWORD writeBytesOut;

	// 1.寫入整筆資料區塊大小
	WriteFile( hFile, &size, sizeof(int), &writeBytesOut, NULL );

	// 2.寫入檔頭資訊
	WriteFile( hFile, header.Begin(), header.Size(), &writeBytesOut, NULL );

	int dataLength;
	// 3.寫入資料區塊	
	if ( m_compressFormat == COMPRESS_FMT_NORMAL ) 
	{
		dataLength=m_dataSize;
		//WriteFile( hFile, m_pData, m_dataSize, &writeBytesOut, NULL );
	}
	else
	{
		dataLength=m_compressSize;
		//WriteFile( hFile, m_pData, m_compressSize, &writeBytesOut, NULL );
	}

	int curPos=0;
	int sizeBuf=BUF_SIZE;
	int curSize;

	char  *name=GetFileName();
	//ProcessDataProc((char*)name, -1);

	//ProcessDataProc( (char*)name, (int)dataLength ) ;
	while( curPos < dataLength )
	{
		curSize = ((( dataLength - curPos ) >= sizeBuf )?( sizeBuf ):( dataLength - curPos ));
	

		WriteFile( hFile, &m_pData[curPos], curSize, &writeBytesOut, NULL );
	curPos += curSize;
		if (curSize>0)
		{
		//	if(ProcessDataProc&& !ProcessDataProc( name, (int)curSize ,dataLength) )
			//	throw (HRESULT)E_EABORTED;
		}
	}
	if (ProcessDataProc)
	ProcessDataProc(data,name,100,100);

	//ProcessDataProc((char*)name, -100);
	return size;
}

int cFileObj::LoadFromData( HANDLE hFile, BOOL loadDataBool )
{
	if ( hFile == INVALID_HANDLE_VALUE ) return 0;
	DWORD readBytesOut;
	int size;

	Destroy();

	// 1. 讀取整個區塊大小
	ReadFile( hFile, &size, sizeof(int), &readBytesOut, NULL );
	if ( size <= 0 ) return 0;

	// 2. 讀取檔頭資料結構
	LoadHeader( hFile );

	// 3. 讀取資料
	if ( loadDataBool ) {
		if ( m_compressFormat == COMPRESS_FMT_NORMAL ) {	// 未壓縮格式
			m_pData = new BYTE[m_dataSize];
			ReadFile( hFile, m_pData, m_dataSize, &readBytesOut, NULL );
		} else {											// 壓縮格式
			m_pData = new BYTE[m_compressSize];
			ReadFile( hFile, m_pData, m_compressSize, &readBytesOut, NULL );
		}
	}

	return size;
}

void cFileObj::SaveHeader( cListData &header )
{
	header.Push( &m_fileType, sizeof(FileType) );
	header.Push( &m_compressFormat, sizeof(CompressFormat) );	
	header.Push( &m_dataSize, sizeof(int) );
	header.Push( &m_compressSize, sizeof(int) );
	header.Push( &m_fileTime, sizeof(FILETIME) );

	int len =0;
	if (m_fileNameStr)
		len = (int)strlen( m_fileNameStr ) + 1;
	else
		len=0;
	header.Push( &len, sizeof(int) );
	if ( len > 1 ) header.Push( m_fileNameStr, len );
	
}

bool cFileObj::LoadHeader( HANDLE hFile )
{	
	DWORD readBytesOut;
	int len;
	ReadFile( hFile, &m_fileType, sizeof(FileType), &readBytesOut, NULL );
	switch (m_fileType)
	{
	case FILE_TYPE_GENERAL:
	case FILE_TYPE_IMAGE:
		break;

	default:
		return false;
	}

	ReadFile( hFile, &m_compressFormat, sizeof(CompressFormat), &readBytesOut, NULL );
	switch (m_compressFormat)
	{
	case COMPRESS_FMT_NORMAL:
	case COMPRESS_FMT_RLE:
	case COMPRESS_FMT_LZO:
	case COMPRESS_FMT_ZLIB:
	case COMPRESS_FMT_RDT:
		break;

	default:
		return false;
	}		

	ReadFile( hFile, &m_dataSize, sizeof(int), &readBytesOut, NULL );
	if ( m_dataSize < 0 )
		return false;

	ReadFile( hFile, &m_compressSize, sizeof(int), &readBytesOut, NULL );
	if ( m_compressSize < 0 || m_compressSize > 0x10000000 )
		return false;

	ReadFile( hFile, &m_fileTime, sizeof(FILETIME), &readBytesOut, NULL );
	ReadFile( hFile, &len, sizeof(int), &readBytesOut, NULL );
	if ( len <= 0 || len > 0x200 )
		return false;


	if ( len ) 
	{
		char nameStr[MAX_PATH];
		ReadFile( hFile, nameStr, len, &readBytesOut, NULL );
		SetFileName( nameStr );
	}
	return true;

}

void cFileObj::WriteData( HANDLE hFile )
{
	DWORD writeBytesOut;

	// 1.寫入整筆資料區塊大小
	WriteFile( hFile, m_pData, m_dataSize, &writeBytesOut, NULL );
}
bool cFileObj::LoadFromFile( const char *workDirStr, const char *fileNameStr,CompressFormat format, void *pSpecialData )
{
	Destroy();

	char  fullNameStr[MAX_PATH];
	DWORD readBytesOut;
	HANDLE hFile;
	sprintf( fullNameStr, "%s%s", workDirStr, fileNameStr );
	if ( (hFile = CreateFileA( fullNameStr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL ) ) == INVALID_HANDLE_VALUE ) {
		return false;
	}
	
	m_fileType			= FILE_TYPE_GENERAL;
	m_compressFormat	= COMPRESS_FMT_NORMAL;
	m_dataSize			= GetFileSize( hFile, NULL );		// 檔案大小
	m_pData				= new BYTE[m_dataSize];
	::GetFileTime( hFile, NULL, NULL, &m_fileTime );

	SYSTEMTIME time;
	FileTimeToSystemTime(&m_fileTime,&time);

	SetFileName( fileNameStr );

	// 讀取檔案資料	
	ReadFile( hFile, m_pData, m_dataSize, &readBytesOut, NULL );

	CloseHandle( hFile );
	ConvertFormat( format );			// 轉換為指定壓縮格式

	return true;
}

bool cFileObj::ConvertFormat( CompressFormat fileFormat )
{
	BYTE *tmp = NULL;
//return true;
	if ( m_pData == NULL || m_dataSize <= 0 ) return false;
	if ( fileFormat == m_compressFormat ) return true;
	switch ( fileFormat )
	{
		case COMPRESS_FMT_NORMAL:
		{
			int tmpSize = m_dataSize;
			tmp = m_pData;
			//if (m_compressFormat!=COMPRESS_FMT_LZO)
			{

			
			m_pData = new BYTE[m_dataSize];
			if ( (m_dataSize = DecompressFile( tmp, m_compressSize, m_compressFormat, m_pData, tmpSize)) > 0 ) {
				delete [] tmp;
				m_compressSize = 0;
				m_compressFormat = fileFormat;
				return true;
			}
			delete [] tmp;
			m_dataSize = tmpSize;
			}
			break;
		}
		case COMPRESS_FMT_RLE:
		case COMPRESS_FMT_LZO:
		case COMPRESS_FMT_ZLIB:
		{
			if ( m_compressFormat != COMPRESS_FMT_NORMAL ) 
			{
				if( ConvertFormat( COMPRESS_FMT_NORMAL )==false)
					return false;
			}
			if ( (tmp = (BYTE*)CompressFile( m_pData, m_dataSize, fileFormat, m_compressSize)) != NULL ) {
				delete [] m_pData;
				m_pData = new BYTE[m_compressSize];
				memcpy( m_pData, tmp, m_compressSize );
				m_compressFormat = fileFormat;


				delete [] tmp;
				return true;
			}
			break;
		}
	}

	return false;
}

bool cFileObj::LoadFromRDTFile( const char *workDirStr, const char *fileNameStr )
{
	//Destroy();

	char fullNameStr[MAX_PATH];
	DWORD readBytesOut;
	HANDLE hFile;
	sprintf( fullNameStr, "%s%s", workDirStr, fileNameStr );




	int len=strlen(fullNameStr);
	for (int i=len-1;i>=0;i--)
	{
		if (fullNameStr[i]=='.')
		{

		//	temp[i]=0;
			strcpy(&fullNameStr[i],".rdt");
			break;
		}
	}

	//if ( (hFile = CreateFile( fullNameStr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL ) ) == INVALID_HANDLE_VALUE ) {
	if ( (hFile = CreateFileA( fullNameStr, GENERIC_READ, NULL, NULL, OPEN_EXISTING, 0, NULL ) ) == INVALID_HANDLE_VALUE ) 
	{
		return false;
	}

	m_fileType			= FILE_TYPE_IMAGE;
	m_compressFormat	= COMPRESS_FMT_RDT;
	m_dataSize			= GetFileSize( hFile, NULL );		// 檔案大小
	::GetFileTime( hFile, NULL, NULL, &m_fileTime );
	SetFileName( fileNameStr );

	if ( m_dataSize > 0 )
	{
		m_compressSize=m_dataSize;
		m_pData			= new BYTE[m_dataSize];

		if ( ReadFile( hFile, m_pData, m_dataSize, &readBytesOut, NULL ) == FALSE )
		{
			// 讀取檔案資料
			// 區分為1M 一個區域讀取
			int blockSize = 1024 * 1024;
			int offset = 0;
			int dataSize = m_dataSize;
			while ( true )
			{
				if ( dataSize < blockSize )
				{
					ReadFile( hFile, m_pData + offset, dataSize, &readBytesOut, NULL );
					offset += dataSize;
					break;
				}
				else
				{
					ReadFile( hFile, m_pData + offset, blockSize, &readBytesOut, NULL );
					offset += blockSize;
					dataSize -= blockSize;
				}
			}
		}
	}

	CloseHandle( hFile );

	return true;
}
bool cFileObj::SaveToFile(const char* filename)
{
	char filenameStr[512];

	if ( m_dataSize <= 0 || filename == NULL )
		return false;

	strcpy(filenameStr, filename);
	if ( m_fileType == FILE_TYPE_IMAGE )
	{
		int len = strlen( filenameStr );
		if ( len >= 4 )
		{
			if ( stricmp(&filenameStr[len - 4], ".BMP") == 0 ||
				stricmp(&filenameStr[len - 4], ".JPG") == 0 ||
				stricmp(&filenameStr[len - 4], ".PCX") == 0 ||
				stricmp(&filenameStr[len - 4], ".PNG") == 0 ||
				stricmp(&filenameStr[len - 4], ".TGA") == 0 )
			{
				memcpy(&filenameStr[len - 4], ".DDS", 4);
			}
		}
	}

	HANDLE hFile = CreateFileA( filenameStr, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	if ( hFile == INVALID_HANDLE_VALUE )
		return false;

	ConvertFormat( COMPRESS_FMT_NORMAL );

	WriteData(hFile);

	CloseHandle(hFile);

	return true;
}