#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "cFileObj.h"

#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

struct realFile:public IFileReadBase
{
	realFile(HANDLE hFile):m_hFile(hFile)
	{



	}
	~realFile()
	{
		Close();
	
	}
	// 檔案是否已經開啟
	BOOL	IsOpen() 
	{
		return m_hFile!=0 &&m_hFile!=INVALID_HANDLE_VALUE;

	}

	// 關閉釋放檔案
	void	Close()
	{

		if (m_hFile)
			CloseHandle(m_hFile);
		m_hFile=NULL;

	}

	// 讀取部份資料
	DWORD	Read(DWORD offset, void *destBuffer, DWORD readSize) 
	{
		DWORD readBytesOut=0;
		if (IsOpen())
		{
			SetFilePointer(m_hFile, offset , 0, FILE_BEGIN);
			ReadFile(m_hFile,destBuffer, readSize, &readBytesOut, NULL );
			

		}
		
		return readBytesOut;
	}
	HANDLE m_hFile;

};

cFileObj::cFileObj()
{
	m_fileType			= FILE_TYPE_GENERAL;
	m_compressFormat	= COMPRESS_FMT_NORMAL;
	m_position			= 0;
	m_dataSize			= 0;
	m_compressSize		= 0;
	m_realFilename		= NULL;
	m_fileNameStr		= NULL;
	m_pData				= NULL;	
	m_fileRead			= NULL;

	memset( &m_fileTime, 0, sizeof(FILETIME) );

	m_userData			= NULL;					// 自定義資料
	m_userDataSize		= NULL;					// 自定義資料大小
	m_userDataAutoRelease	= false;			//
}

cFileObj::~cFileObj()
{
	Destroy();
}

bool cFileObj::CreateFilePath(const char* fullname)
{
	if ( fullname == NULL ) 
		return false;

	wchar_t tempStr[SAFE_PATH_MAX];

	// 轉換為WCHAR
	MultiByteToWideChar(CP_ACP, 0, fullname, -1, tempStr, SAFE_PATH_MAX);

	return CreateFilePath(tempStr);
}

bool cFileObj::CreateFilePath(const wchar_t* fullname)
{
	if ( fullname == NULL ) 
		return false;

	wchar_t pathNameStr[SAFE_PATH_MAX];
	wcscpy(pathNameStr, fullname);

	// 將"/"轉換為"\"
	wchar_t* lastPos = NULL;
	while ( (lastPos = (wchar_t*)wcschr(pathNameStr, L'/')) != NULL )
		*lastPos = L'\\';

	lastPos = (wchar_t*)wcschr(pathNameStr, L'\\');
	while ( lastPos != NULL )
	{
		wchar_t tempStr[SAFE_PATH_MAX];
		int size = (int)(lastPos - pathNameStr);
		wcsncpy(tempStr, pathNameStr, size);
		tempStr[size] = 0;
		if ( !PathIsDirectoryW( tempStr ) )
		{
			// 有可能多執行緒下會造成判定 ERROR_ALREADY_EXISTS 錯誤, 此時須容錯
			if ( !CreateDirectoryW( tempStr, NULL ) && GetLastError() != ERROR_ALREADY_EXISTS )
			{
				return false;
			}
		}
		lastPos = (wchar_t*)wcschr(lastPos + 1, L'\\');
	}

	return true;
}

BOOL cFileObj::IsOpen()
{
	if ( m_fileRead && m_fileRead->IsOpen() )
		return TRUE;
	else if ( m_pData != NULL )
		return TRUE;
	return FALSE;
}

int cFileObj::EndOfStream()
{
	if ( !IsOpen() || m_position == m_dataSize ) {
		return 1;
	}
	return 0;
}

DWORD cFileObj::Read( void *destBuffer, DWORD readSize )
{
	if ( m_dataSize == 0 || m_position >= m_dataSize || destBuffer == NULL ) 
		return 0;

	// 修正讀取最大剩餘容量
	if ( readSize > (DWORD)(m_dataSize - m_position) ) 
	{
		readSize = m_dataSize - m_position;
	}

	// 已經有資料(已載入到記憶體內)
	if ( m_pData )
		memcpy( destBuffer, m_pData + m_position, readSize );
	else if ( m_fileRead == NULL )
		return 0;
	else
	{
		DWORD error = S_OK;
		DWORD resultReadSize = m_fileRead->Read(m_position, destBuffer, readSize);
		if ( resultReadSize != readSize )
		{
			readSize = resultReadSize;
			error = GetLastError();
			return 0;
		}
	}

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

void cFileObj::Destroy( void )
{

	if ( m_fileRead )
		m_fileRead->Close();

	SAFE_DELETE(m_fileRead);

	//m_fileType			= FILE_TYPE_GENERAL;
	m_compressFormat	= COMPRESS_FMT_NORMAL;
	m_position			= 0;
	m_dataSize			= 0;
	m_compressSize		= 0;
	memset( &m_fileTime, 0, sizeof(FILETIME) );

	SAFE_DELETE_ARRAY(m_realFilename);
	SAFE_DELETE_ARRAY(m_fileNameStr);
	SAFE_DELETE_ARRAY(m_pData);	
}

BYTE* cFileObj::GetData( void )
{
	if ( m_pData )
		return m_pData;
	else if ( m_dataSize > 0 && m_fileRead )
	{
		m_pData = new BYTE[m_dataSize];
		m_fileRead->Read(0, m_pData, m_dataSize);
		return m_pData;
	}

	return NULL;
}

void cFileObj::ReleaseData( bool boOnlyData )
{
	if ( !boOnlyData )
	{
		m_compressFormat = COMPRESS_FMT_NORMAL;
		m_dataSize = 0;
	}
	SAFE_DELETE_ARRAY(m_pData);	
}

void cFileObj::SetFileName( const char *fileNameStr )
{
	if ( fileNameStr ) {		
		if ( m_fileNameStr ) delete [] m_fileNameStr;
		if ( *fileNameStr == _T('\\') ) {
			m_fileNameStr = new TCHAR[_tcslen(fileNameStr + 1) + 1];
			_tcscpy( m_fileNameStr, fileNameStr + 1 );
		} else {
			m_fileNameStr = new TCHAR[_tcslen(fileNameStr) + 1];
			_tcscpy( m_fileNameStr, fileNameStr );
		}
		_tcscpy(m_fixFileNameStr, m_fileNameStr);
	}
}

int cFileObj::SaveData( HANDLE hFile )
{
	// 避免空 Data 造成檔案仍然指到下一個
	if ( hFile == INVALID_HANDLE_VALUE /*|| m_pData == NULL*/ ) {
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

	// 3.寫入資料區塊	
	if ( m_compressFormat == COMPRESS_FMT_NORMAL ) 
	{
		BYTE* data = GetData();
		WriteFile( hFile, data, m_dataSize, &writeBytesOut, NULL );
	} else {
		WriteFile( hFile, m_pData, m_compressSize, &writeBytesOut, NULL );
	}

	return size;
}

int cFileObj::LoadFromData( HANDLE hFile, BOOL loadDataBool )
{
	if ( hFile == INVALID_HANDLE_VALUE ) return 0;
	DWORD readBytesOut;
	int size;

	/*
	if ( m_pData != NULL || m_dataSize > 0 )
		Destroy();
	*/

	// 1. 讀取整個區塊大小
	ReadFile( hFile, &size, sizeof(int), &readBytesOut, NULL );
	if ( size <= 0 ) return 0;

	// 2. 讀取檔頭資料結構
	if ( LoadHeader( hFile ) == false )
		return 0;

	// 3. 讀取資料
	if ( loadDataBool )
	{
		switch ( m_compressFormat )
		{
		case COMPRESS_FMT_NORMAL:
			if ( m_dataSize > 0 )
			{
				m_pData = new BYTE[m_dataSize];
				ReadFile(hFile, m_pData, m_dataSize, &readBytesOut, NULL);
			}
			break;

		default:
			if ( m_compressSize > 0 )
			{
				m_pData = new BYTE[m_compressSize];
				ReadFile(hFile, m_pData, m_compressSize, &readBytesOut, NULL);
			}
			break;
		}
	}

	return size;
}

bool cFileObj::SaveToFile(const char* filename)
{
	if ( filename == NULL || filename[0] == 0 )
		return false;

	wchar_t tempStr[SAFE_PATH_MAX];

	// 轉換為WCHAR
	MultiByteToWideChar(CP_ACP, 0, filename, -1, tempStr, SAFE_PATH_MAX);

	return SaveToFile(tempStr);
}

bool cFileObj::SaveToFile(const wchar_t* filename)
{
	wchar_t filenameStr[SAFE_PATH_MAX];

	if ( filename == NULL )
		return false;

	wcsncpy(filenameStr, filename, SAFE_PATH_MAX);
	if ( m_fileType == FILE_TYPE_IMAGE )
	{
		int len = (int)wcslen( filenameStr );
		if ( len >= 4 )
		{
			if ( _wcsicmp(&filenameStr[len - 4], L".bmp") == 0 ||
				 _wcsicmp(&filenameStr[len - 4], L".jpg") == 0 ||
				 _wcsicmp(&filenameStr[len - 4], L".pcx") == 0 ||
				 _wcsicmp(&filenameStr[len - 4], L".png") == 0 ||
				 _wcsicmp(&filenameStr[len - 4], L".tga") == 0 )
			{
				wcscpy(&filenameStr[len - 4], L".dds");
			}
		}
	}

	HANDLE hFile = CreateFileW( filenameStr, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	if ( hFile == INVALID_HANDLE_VALUE )
		return false;

	if ( !ConvertFormat( COMPRESS_FMT_NORMAL ) )
	{
		CloseHandle(hFile);
		return false;
	}

	WriteData(hFile);

	CloseHandle(hFile);

	return true;
}

void cFileObj::SetUserData(void* userData, int dataSize, bool autoDelete)
{
	if ( m_userData && m_userDataAutoRelease )
		delete m_userData;

	m_userData = userData;
	m_userDataSize = dataSize;
	m_userDataAutoRelease = autoDelete;
}

void cFileObj::SaveHeader( cListData &header )
{
	header.Push( &m_fileType, sizeof(FileType) );
	header.Push( &m_compressFormat, sizeof(CompressFormat) );	
	header.Push( &m_dataSize, sizeof(int) );
	header.Push( &m_compressSize, sizeof(int) );
	header.Push( &m_fileTime, sizeof(FILETIME) );

	int len = (int)_tcslen( m_fileNameStr ) + 1;
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

	if ( len ) {
		TCHAR nameStr[MAX_PATH];
		ReadFile( hFile, nameStr, len, &readBytesOut, NULL );
		SetFileName( nameStr );
	}

	return true;
}

void cFileObj::WriteData( HANDLE hFile )
{
	DWORD writeBytesOut;

	// 1.寫入整筆資料區塊大小
	BYTE *data=GetData();
	WriteFile( hFile, data, m_dataSize, &writeBytesOut, NULL );
}
bool cFileObj::LoadFromHandle(const char *workDirStr, const char *fileNameStr)
{
	Destroy();

	char fullNameStr[SAFE_PATH_MAX];	
	HANDLE hFile;

	sprintf_s(fullNameStr, "%s%s", workDirStr, fileNameStr);
	if ( (hFile = OpenFile(fullNameStr)) == INVALID_HANDLE_VALUE )
	{
		return false;
	}	

	m_dataSize = GetFileSize(hFile, NULL);

	SetFileReadHandle(new realFile(hFile));

	return true;
}

bool cFileObj::LoadFromFile( const char *workDirStr, const char *fileNameStr, CompressFormat format, void *pSpecialData )
{
	Destroy();
	
	char fullNameStr[SAFE_PATH_MAX];	
	HANDLE hFile;

	sprintf_s(fullNameStr, "%s%s", workDirStr, fileNameStr);
	if ( (hFile = OpenFile(fullNameStr)) == INVALID_HANDLE_VALUE )
	{
		return false;
	}	
	

	// 讀取DATA
	if ( ReadFileData(hFile, fullNameStr, pSpecialData) )
	{
		// 暫存真實檔案下路徑
		SetRealFilename(fullNameStr);
		
		m_compressFormat	= COMPRESS_FMT_NORMAL;
		::GetFileTime( hFile, NULL, NULL, &m_fileTime );
		SetFileName( fileNameStr );

		ConvertFormat( format );			// 轉換為指定壓縮格式
		CloseHandle(hFile);
		return true;
	}	

	CloseHandle(hFile);
	
	return false;
	
}

bool cFileObj::LoadFromFile( const wchar_t *workDirStr, const wchar_t *fileNameStr, CompressFormat format, void *pSpecialData )
{
	char dirStr[SAFE_PATH_MAX];
	char fileStr[SAFE_PATH_MAX];

	WideCharToMultiByte(CP_ACP, 0, workDirStr, -1, dirStr, SAFE_PATH_MAX, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, fileNameStr, -1, fileStr, SAFE_PATH_MAX, NULL, NULL);

	return LoadFromFile(dirStr, fileStr, format, pSpecialData);
}

bool cFileObj::ConvertFormat( CompressFormat fileFormat )
{
	BYTE *tmp = NULL;

	if ( fileFormat == m_compressFormat ) return true;
	if ( m_dataSize <= 0 ) return false;
	switch ( fileFormat )
	{
		case COMPRESS_FMT_NORMAL:
		{
			BYTE* newData = new BYTE[m_dataSize];
			if ( newData )
			{
				int newDataSize = DecompressFile( GetData(), m_compressSize, m_compressFormat, newData, m_dataSize );
				if ( newDataSize > 0 )
				{
					delete [] m_pData;
					m_pData = newData;
					m_dataSize = newDataSize;
					m_compressSize = 0;
					m_compressFormat = fileFormat;
					return true;
				}
				else
				{
					delete [] newData;
				}
			}
			break;
		}
		case COMPRESS_FMT_RLE:
		case COMPRESS_FMT_LZO:
		case COMPRESS_FMT_ZLIB:
		{
			if ( m_compressFormat != COMPRESS_FMT_NORMAL ) {
				ConvertFormat( COMPRESS_FMT_NORMAL );
			}
			BYTE *data=GetData();
			if ( (tmp = (BYTE*)CompressFile( m_pData, m_dataSize, fileFormat, m_compressSize )) != NULL ) {
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

void cFileObj::SetRealFilename( const char* realFilename)
{
	SAFE_DELETE_ARRAY(m_realFilename);

	if ( realFilename )
	{
		m_realFilename = new char[strlen(realFilename) + 1];
		strcpy(m_realFilename, realFilename);
	}	
}

void cFileObj::SetRealFilename( const wchar_t* realFilename)
{
	SAFE_DELETE_ARRAY(m_realFilename);	

	char tempFilenameStr[SAFE_PATH_MAX];

	WideCharToMultiByte(CP_ACP, 0, realFilename, -1, tempFilenameStr, SAFE_PATH_MAX, NULL, NULL);

	SetRealFilename(tempFilenameStr);
}

void cFileObj::SetFileReadHandle(IFileReadBase* pFileReadBase)
{
	m_fileRead = pFileReadBase;
}


HANDLE cFileObj::OpenFile(const char* fullname)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;

	if ( fullname && fullname[0] )
	{
		wchar_t wTempFilenameStr[SAFE_PATH_MAX];

		MultiByteToWideChar(CP_ACP, 0, fullname, -1, wTempFilenameStr, SAFE_PATH_MAX);

		hFile = OpenFile(wTempFilenameStr);
	}

	return hFile;
}

HANDLE cFileObj::OpenFile(const wchar_t* fullname)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;

	if ( fullname && fullname[0] )
	{
		hFile = CreateFileW( fullname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	}

	return hFile;
}

bool cFileObj::ReadFileData(HANDLE hFile, const char* fullNameStrh, void* imageOptions)
{
	DWORD readBytesOut;

	DWORD dataSize = GetFileSize(hFile, NULL);

	if ( dataSize > 0 )
	{
		// 刪除舊有資料
		SAFE_DELETE(m_pData);

		// 產生相同檔案大小記憶體
		m_dataSize = dataSize;
		m_pData = new BYTE[m_dataSize];

		// 載入資料
		ReadFile( hFile, m_pData, m_dataSize, &readBytesOut, NULL );
	}

	return true;
}
