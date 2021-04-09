/******************************************************************************
******************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <Shlwapi.h>
#include "cFilePackage.h"
#include "FileHashTable.h"
#include <md5\Mymd5.h>

PackageFileRead::PackageFileRead()
	: m_filePackage(NULL),
	m_hFile(INVALID_HANDLE_VALUE),
	m_offset(0),
	m_filename(NULL),
	m_tempHandle(false)
{
}

PackageFileRead::~PackageFileRead()
{
	Close();

	m_filename = NULL;
	m_filePackage = NULL;
}

// 關閉檔案
void PackageFileRead::Close()
{
	if (m_tempHandle)
	{
		m_filePackage->RemoveFileHandle(m_filename);
	}
}

// ------------------------------------------------------------------------------------------------
cTempFileData::cTempFileData()
{
	Clear();
}

// ------------------------------------------------------------------------------------------------
cTempFileData::cTempFileData(int size)
{
	Clear();

	Open(size);
}

// ------------------------------------------------------------------------------------------------
cTempFileData::cTempFileData(char* inData, int inDataSize)
{
	Clear();

	SetData(inData, inDataSize);
}

// ------------------------------------------------------------------------------------------------
cTempFileData::~cTempFileData()
{
	Close();
}

// ------------------------------------------------------------------------------------------------
void cTempFileData::Clear()
{
	m_isNew = false;
	m_data = NULL;
	m_dataSize = 0;
	m_position = 0;
}

// ------------------------------------------------------------------------------------------------
void cTempFileData::Open(int newSize)
{
	Close();

	if (newSize <= 0)
		newSize = 65536;

	m_data = new char[newSize];
	m_dataSize = newSize;
	m_position = 0;
	m_isNew = true;
}

// ------------------------------------------------------------------------------------------------
void cTempFileData::Close()
{
	if (m_isNew && m_data)
		delete[] m_data;

	Clear();
}

// ------------------------------------------------------------------------------------------------
int cTempFileData::Read(void* outData, int outDataSize)
{
	// 調整大小,最大值不超過剩餘大小
	if (outDataSize > m_dataSize - m_position)
	{
		outDataSize = m_dataSize - m_position;
	}

	// 讀取資料大於零
	if (outDataSize > 0 && outData)
	{
		memcpy(outData, m_data + m_position, outDataSize);
	}

	m_position += outDataSize;

	return outDataSize;
}

// ------------------------------------------------------------------------------------------------
int cTempFileData::Write(void* data, int dataSize)
{
	if (m_isNew == false || m_data == NULL || dataSize <= 0)
		return 0;

	int surplusSize = m_dataSize - m_position;

	// 剩餘容量不足夠,產生更大的空間
	if (dataSize > surplusSize)
	{
		int newSize = m_dataSize << 1;
		while (newSize - m_position < dataSize)
			newSize <<= 1;

		char* tempData = new char[newSize];
		memcpy(tempData, m_data, m_dataSize);
		delete[] m_data;
		m_data = tempData;
		m_dataSize = newSize;
	}

	memcpy(m_data + m_position, data, dataSize);
	m_position += dataSize;
	return dataSize;
}

// ------------------------------------------------------------------------------------------------
void cTempFileData::SetData(char* data, int size)
{
	Close();

	m_data = data;
	m_dataSize = size;
	m_isNew = false;
	m_position = 0;
}

// ----------------------------------------------------------------------------------
int cTempFileData::SetPosition(FilePosition position, int offset)
{
	switch (position)
	{
	case eFilePosition_Begin:			// 開始
		m_position = min(max(offset, 0), m_dataSize);
		break;

	case eFilePosition_Current:			// 目前位置
		m_position = min(max(m_position + offset, 0), m_dataSize);
		break;

	case eFilePosition_End:				// 結尾
		m_position = min(max(m_dataSize + offset, 0), m_dataSize);
		break;
	}

	return m_position;
}

// 讀取部份資料
DWORD PackageFileRead::Read(DWORD offset, void* destBuffer, DWORD readSize)
{
	DWORD readBytesOut = 0;

	if (m_filePackage && !m_filePackage->CheckReadLocked() && IsOpen())
	{
		SetFilePointer(m_hFile, offset + m_offset, 0, FILE_BEGIN);
		ReadFile(m_hFile, destBuffer, readSize, &readBytesOut, NULL);
	}
	return readBytesOut;
}

bool cFilePackage::TransFilename(const char* instr, char* outstr, int outsize)
{
	if (instr == NULL)
		return false;

	char buf[SAFE_PATH_MAX];
	int i = 0;
	int j = 0;
	while (instr[i]) {
		buf[j] = ::tolower(instr[i]);

		switch (buf[j])
		{
			// "/"
		case 47:
			buf[j] = 92;
			if (instr[i + 1] == 47)
				i++;
			break;

		case 92:
			if (instr[i + 1] == 92)
				i++;
			break;
		}
		i++;
		j++;
	}
	buf[j] = 0;
	strncpy(outstr, buf, outsize);
	outstr[outsize - 1] = 0;
	return true;
}

#define MAX_SEND_HEADER					10
#define CHECK_SEND_HEADER_MAGIC			0x12345678

cTempFileData* cFilePackage::NewFileObjMem(cFileObj* pFileObj)
{
	if (pFileObj == NULL)
		return NULL;

	int magaicNumber = CHECK_SEND_HEADER_MAGIC;
	int headerSize[MAX_SEND_HEADER];
	memset(headerSize, 0x00, sizeof(headerSize));
	int count = 0;

	// 記錄檔頭大小
	switch (pFileObj->GetType())
	{
	case FILE_TYPE_GENERAL:
		headerSize[count++] = sizeof(cFileObj);
		break;

	case FILE_TYPE_IMAGE:
		headerSize[count++] = sizeof(cImgFile);
		break;

	default:
		return NULL;
	}

	// 取得檔案名稱大小
	if (pFileObj->m_fileNameStr)
		headerSize[count++] = (int)strlen(pFileObj->m_fileNameStr) + 1;
	else
		headerSize[count++] = 0;

	// 
	if (pFileObj->m_realFilename)
		headerSize[count++] = (int)strlen(pFileObj->m_realFilename) + 1;
	else
		headerSize[count++] = 0;

	// 取得資料大小
	switch (pFileObj->GetCompressFormat())
	{
	case COMPRESS_FMT_NORMAL:
		headerSize[count++] = pFileObj->GetDataSize();
		break;

	case COMPRESS_FMT_RLE:
	case COMPRESS_FMT_LZO:
	case COMPRESS_FMT_ZLIB:
	case COMPRESS_FMT_RDT:
		headerSize[count++] = pFileObj->GetCompressSize();
		break;

	default:
		return NULL;
	}

	// 自定參數
	headerSize[count++] = pFileObj->m_userDataSize;

	int total = sizeof(magaicNumber) + sizeof(headerSize);
	for (int i = 0; i < MAX_SEND_HEADER; i++)
		total += headerSize[i];

	cTempFileData* newData = new cTempFileData(total);
	newData->Write(&magaicNumber, sizeof(magaicNumber));
	newData->Write(headerSize, sizeof(headerSize));
	newData->Write(&(pFileObj->m_fileType), headerSize[0]);
	newData->Write(pFileObj->m_fileNameStr, headerSize[1]);
	newData->Write(pFileObj->m_realFilename, headerSize[2]);
	newData->Write(pFileObj->m_pData, headerSize[3]);
	newData->Write(pFileObj->m_userData, headerSize[4]);

	return newData;
}

cFileObj* cFilePackage::TransMemToFileObj(char* inData, int inDataSize)
{
	if (inData == NULL)
		return NULL;

	int magaicNumber = 0;
	int headerSize[MAX_SEND_HEADER];
	memset(headerSize, 0x00, sizeof(headerSize));
	int count = 0;

	cTempFileData data((char*)inData, inDataSize);

	data.Read(&magaicNumber, sizeof(magaicNumber));

	if (magaicNumber != CHECK_SEND_HEADER_MAGIC)
		return NULL;

	cFileObj* pFileObj = NULL;

	data.Read(headerSize, sizeof(headerSize));

	if (headerSize[0] == sizeof(cFileObj))
	{
		pFileObj = new cFileObj;
		data.Read(&pFileObj->m_fileType, sizeof(cFileObj));
	}
	else if (headerSize[0] == sizeof(cImgFile))
	{
		pFileObj = new cImgFile;
		data.Read(&pFileObj->m_fileType, sizeof(cImgFile));
	}
	else
		return NULL;

	pFileObj->m_fileRead = NULL;
	count++;

	// 檔名
	if (headerSize[count] > 0)
	{
		pFileObj->m_fileNameStr = new char[headerSize[count]];
		data.Read(pFileObj->m_fileNameStr, headerSize[count]);
	}
	else
		pFileObj->m_fileNameStr = NULL;

	count++;

	// 真實檔名
	if (headerSize[count] > 0)
	{
		pFileObj->m_realFilename = new char[headerSize[count]];
		data.Read(pFileObj->m_realFilename, headerSize[count]);
	}
	else
		pFileObj->m_realFilename = NULL;

	count++;

	// 資料
	if (headerSize[count] > 0)
	{
		pFileObj->m_pData = (BYTE*)new char[headerSize[count]];
		data.Read(pFileObj->m_pData, headerSize[count]);
	}
	else
		pFileObj->m_pData = NULL;

	count++;

	// 自定資料
	if (headerSize[count] > 0)
	{
		pFileObj->m_userData = new char[headerSize[count]];
		pFileObj->m_userDataSize = headerSize[count];
		pFileObj->m_userDataAutoRelease = true;
		data.Read(pFileObj->m_userData, headerSize[count]);
	}
	else
	{
		pFileObj->m_userData = 0;
		pFileObj->m_userDataSize = 0;
		pFileObj->m_userDataAutoRelease = false;
	}

	return pFileObj;
}

cFilePackage::cFilePackage()
{
	m_readLocked = false;

	memset(m_dirStr, 0, sizeof(m_dirStr));

#ifdef _REDUX
	cImgFile::ReduxInitialize();
#endif
}

cFilePackage::~cFilePackage()
{
#ifdef _REDUX
	cImgFile::ReduxExit();
#endif

	Destroy();
}

void cFilePackage::Destroy()
{
	ReleasePathname();

	// 刪除暫存檔
	for (vector<string>::iterator iter = m_tempFile.begin(); iter != m_tempFile.end(); iter++)
	{
		DeleteFile(iter->c_str());
	}

#if FILEPACKAGE_USE_TST_DICTIONARY
	std::vector<FileHandleRef*>::iterator iterFileHandle = m_fileHandle.GetTable().begin();
	std::vector<FileHandleRef*>::iterator iterFileHandleEnd = m_fileHandle.GetTable().end();
	for (; iterFileHandle != iterFileHandleEnd; ++iterFileHandle)
	{
		CloseHandle((*iterFileHandle)->hFile);
	}

	m_table.GetTable().clear();
	m_fileHandle.GetTable().clear();
#else
	for (mapFileHandle::iterator iter = m_fileHandle.begin(); iter != m_fileHandle.end(); ++iter)
	{
		CloseHandle(iter->second.hFile);
	}

	m_table.clear();
	m_fileHandle.clear();
#endif

	//m_currentIter = m_table.end();
	memset(m_dirStr, 0, sizeof(m_dirStr));
}

bool cFilePackage::FileExists(const char* filename)
{
	if (Find(filename)) {
		return true;
	}
	else {
		const char* pathFilename = GetFullNameStr(filename);
		if (::PathFileExists(pathFilename))
			return true;
	}
	return false;
}

cFileObj* cFilePackage::GetFileObj(const char* filename, bool boLoadAllData)
{
	cFileObj* pFileObj = NULL;
	if (filename)
	{
		// 去除 \ / 
		while (*filename && (*filename == '\\' || *filename == '/'))
			filename++;

		FileDbIndex* pFileIndex = Find(filename);
		if (pFileIndex)
		{
			pFileIndex->m_filename = filename;
			pFileObj = GetFileObj(*pFileIndex, boLoadAllData);
			pFileIndex->m_filename = NULL;
		}
	}
	return pFileObj;
}

cFileObj* cFilePackage::GetFileObj(FileDbIndex& pFileIndex, bool boLoadAllData)
{
	cFileObj* pFileObj = NULL;

	if (m_readLocked)
		return NULL;

	bool boResult = false;
	switch (pFileIndex.m_fileType)
	{
	case FILE_TYPE_GENERAL:
		pFileObj = new cFileObj();
		break;

	case FILE_TYPE_IMAGE:
		pFileObj = new cImgFile();
		break;

	default:
		return NULL;
	}

	if (pFileObj == NULL)
		return NULL;

	if (pFileIndex.m_isFromDB)
	{
		if (pFileIndex.m_offset > 0)
		{
			HANDLE hFile = INVALID_HANDLE_VALUE;

			// 讀取中
			// m_readLocked = true;

			if (pFileIndex.m_hFile != INVALID_HANDLE_VALUE)
				hFile = pFileIndex.m_hFile;
			else
			{
				FileHandleRef* fileHandleRef = GetFileHandleRef(pFileIndex.m_DBFilename);
				if (fileHandleRef == NULL)
				{
					hFile = CreateFile(pFileIndex.m_DBFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
					if (hFile != INVALID_HANDLE_VALUE)
						AddFileHandle(pFileIndex.m_DBFilename, hFile);
				}
				else
				{
					hFile = fileHandleRef->hFile;
				}
			}

			if (hFile != INVALID_HANDLE_VALUE)
			{
				// 設定讀取檔案結構
				PackageFileRead* readFile = new PackageFileRead();
				if (readFile)
				{
					readFile->m_filename = pFileIndex.m_DBFilename;
					readFile->m_filePackage = this;
					readFile->m_hFile = hFile;
					readFile->m_offset = pFileIndex.m_offset;
					readFile->m_tempHandle = pFileIndex.m_hFile == INVALID_HANDLE_VALUE;
					pFileObj->SetFileReadHandle(readFile);
				}

				SetFilePointer(hFile, pFileIndex.m_offset, 0, FILE_BEGIN);
				if (pFileObj->LoadFromData(hFile, boLoadAllData) != 0)
				{
					if (!boLoadAllData)
					{
						readFile->m_offset = SetFilePointer(hFile, 0, 0, FILE_CURRENT);
					}

					boResult = true;
				}
			}

			// 解除鎖定
			// m_readLocked = false;
		}
	}
	else if (pFileIndex.m_filename)
	{
		if (pFileObj->LoadFromFile(m_dirStr, pFileIndex.m_filename) != 0)
			boResult = true;
	}

	// 失敗刪除已產生的記憶體資料
	if (!boResult) { delete pFileObj; pFileObj = NULL; }

	return pFileObj;
}

#if RUSTREAMIO

bool cFilePackage::CheckMd5(const char* keyStr, IRuStream* stream)
{
	//auto search = m_globalHashTable.find(std::string(keyStr));

	//if (search != m_globalHashTable.end())
	//{
	//	MyMD5Class myMd5Class;
	//	myMd5Class.ComputeByteHash(stream->OpenStreamMapping(), NULL, stream->GetStreamSize());
	//	std::string md5 = myMd5Class.GetMd5Str();
	//	if (md5 != search->second)
	//	{
	//		char tempFilename[1024];
	//		sprintf_s(tempFilename, "Check md5 fail\n File:%s\n FileMD5:%s\n BaseMD5:%s", keyStr, md5.c_str(), search->second.c_str());
	//		int msgboxID = MessageBox(
	//			NULL,
	//			(LPCSTR)tempFilename,
	//			(LPCSTR)L"Check md5 fail",
	//			MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2
	//		);
	//	}
	//}

	return true;
}

// keyStr 檔案索引名稱
// boLoadAllData 載入完整資料
IRuStream* cFilePackage::GetIRuStream(const char* keyStr, bool boLoadAllData)
{
	cFileObj* pFileObj = NULL;

	if (keyStr == NULL)
		return NULL;

	while (*keyStr == '\\' || *keyStr == '/')
		keyStr++;

	char fullNameStr[SAFE_PATH_MAX];
	_stprintf_s(fullNameStr, SAFE_PATH_MAX, "%s%s", m_dirStr, keyStr);

	//if (!m_globalHashTableInit)
	//{
	//	m_globalHashTableInit = true;
	//	InitGlobalHashTable();
	//}

	if (::PathFileExists(fullNameStr))
	{
		if (IsImageFile(keyStr))
			pFileObj = new cImgFile();
		else
			pFileObj = new cFileObj();

		if (boLoadAllData)
		{
			if (pFileObj->LoadFromFile(m_dirStr, keyStr))
			{
				CheckMd5(keyStr, pFileObj);
				return pFileObj;
			}
		}
		else if (pFileObj->LoadFromHandle(m_dirStr, keyStr))
		{
			CheckMd5(keyStr, pFileObj);
			return pFileObj;
		}

		delete pFileObj;
		pFileObj = NULL;
	}

	if ((pFileObj = GetFileObj(keyStr, boLoadAllData)) != NULL)
	{
		if (!pFileObj->ConvertFormat(COMPRESS_FMT_NORMAL))
		{
			delete pFileObj;
			pFileObj = NULL;
		}
	}

	CheckMd5(keyStr, pFileObj);
	return pFileObj;
}

BOOL cFilePackage::GetCRuTempFileStream(const char* keyStr, CRuFileStream& tempFileStream)
{
	BOOL resultBool = FALSE;
	IRuStream* fileStream = GetIRuStream(keyStr);
	if (fileStream)
	{
		char tempFilename[512];
		sprintf_s(tempFilename, "temp\\%s", keyStr);

		CreatePath(tempFilename);

		if (tempFileStream.Create(tempFilename) == ruSEC_OK)
		{
			tempFileStream.Write(fileStream->OpenStreamMapping(), fileStream->GetStreamSize());
			tempFileStream.Seek(0, ruSSM_Begin);

			m_tempFile.push_back(tempFilename);

			resultBool = TRUE;
		}

		delete fileStream;
	}
	return resultBool;
}
#endif

bool cFilePackage::AddFile(const char* fileNameStr, void* pSpecialData, CompressFormat format)
{
	if (fileNameStr == NULL)
		return false;

	HANDLE hFile;
	if ((hFile = CreateFile(GetFullNameStr(fileNameStr), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE) {
		return false;
	}

	FileDbIndex fileInfo;
	fileInfo.m_isFromDB = false;
	fileInfo.m_hFile = INVALID_HANDLE_VALUE;
	fileInfo.m_DBFilename = NULL;
	GetFileTime(hFile, NULL, NULL, &fileInfo.m_fileTime);
	CloseHandle(hFile);

	if (IsImageFile(fileNameStr))
		fileInfo.m_fileType = FILE_TYPE_IMAGE;
	else
		fileInfo.m_fileType = FILE_TYPE_GENERAL;

	return AddToTable(fileNameStr, fileInfo);
}

// ----------------------------------------------------------------------------
bool cFilePackage::RemoveFile(const char* fileNameStr)
{
#if FILEPACKAGE_USE_TST_DICTIONARY
	// TODO: 
#else
	mapFileIndex::iterator iter = m_table.find(fileNameStr);
	if (iter != m_table.end())
	{
		m_table.erase(iter);
		return true;
	}
#endif

	return false;
}

// ----------------------------------------------------------------------------
// 讀取檔頭資料
bool cFilePackage::LoadFromFile(const char* fileNameStr, bool keepHandle)
{
	if (fileNameStr == NULL)
		return false;

	wchar_t tempFilenameStr[SAFE_PATH_MAX];

	MultiByteToWideChar(CP_ACP, 0, fileNameStr, -1, tempFilenameStr, SAFE_PATH_MAX);

	HANDLE hFile = CreateFileW(tempFilenameStr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD magic, readBytesOut;
	ReadFile(hFile, &magic, sizeof(DWORD), &readBytesOut, NULL);
	if (magic != FILEPACKAGE_MAGIC) {
		CloseHandle(hFile);
		return false;
	}

	bool result = false;

	// 讀取總筆數
	INT count;
	ReadFile(hFile, &count, sizeof(DWORD), &readBytesOut, NULL);
	if (count > 0)
	{
		// 讀入TABLE檔頭
		FileDataBaseHeader* header = new FileDataBaseHeader[count];
		ReadFile(hFile, header, sizeof(FileDataBaseHeader) * count, &readBytesOut, NULL);

		// 字串長度陣列
		INT* textLength = new INT[count];
		ReadFile(hFile, textLength, sizeof(INT) * count, &readBytesOut, NULL);

		// 字串內容大小
		INT textSize;
		ReadFile(hFile, &textSize, sizeof(INT), &readBytesOut, NULL);

		if (textSize > 0) {
			// 字串內容
			char* text = new char[textSize];
			ReadFile(hFile, text, sizeof(char) * textSize, &readBytesOut, NULL);

			// 取得路徑字串
			const char* pathname = InsertPathname(fileNameStr);

			// 寫入表格內
			FileDbIndex info;
			unsigned int pos = 0;
			for (int i = 0; i < count; i++) {

				// 是否保留檔案HANDLE
				if (keepHandle)
					info.m_hFile = hFile;
				else
					info.m_hFile = INVALID_HANDLE_VALUE;

				info.m_isFromDB = true;
				info.m_DBFilename = pathname;
				info.m_fileType = header[i].m_fileType;
				info.m_fileTime = header[i].m_fileTime;
				info.m_offset = header[i].m_offset;

				AddToTable(text + pos, info);
				pos += textLength[i] + 1;
			}

			delete[] text;
			result = true;
		}

		delete[] textLength;
		delete[] header;
	}

	if (result && keepHandle)
	{
		char lowerFilename[SAFE_PATH_MAX];
		strcpy(lowerFilename, fileNameStr);
		_strlwr_s(lowerFilename, SAFE_PATH_MAX);
		AddFileHandle(lowerFilename, hFile);
	}
	else
	{
		CloseHandle(hFile);
	}

	return result;
}

int cFilePackage::SaveToFile(const TCHAR* fileNameStr)
{
#if FILEPACKAGE_USE_TST_DICTIONARY
	if (m_table.GetTable().empty())
		return -1;
#else
	if (m_table.empty())
		return -1;
#endif

	HANDLE hFile = CreateFile(fileNameStr, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return -2;

	DWORD writeBytesOut;
	DWORD magic = FILEPACKAGE_MAGIC;
	WriteFile(hFile, &magic, sizeof(DWORD), &writeBytesOut, NULL);		// 寫入檔案檢查碼

	int count = 0;
	/*
	for ( set<FileDbIndex*, FileDbIndex>::iterator iter = m_table.begin(); iter != m_table.end(); iter++ ) {
		cFileObj *pFileObj = NULL;
		if ( GetFileObj( (*iter)->filename, &pFileObj ) ) {
			pFileObj->SaveData( hFile );
			count++;
			delete pFileObj;
			pFileObj = NULL;
		}
	}
	*/

	const int zero = 0;
	WriteFile(hFile, &zero, sizeof(int), &writeBytesOut, NULL);			// 區塊結束
	WriteFile(hFile, &count, sizeof(int), &writeBytesOut, NULL);
	CloseHandle(hFile);

	return count;
}

void cFilePackage::CreatePath(const char* filename)
{
	if (filename == NULL)
		return;

	const char* beginFilename = filename;
	while (true)
	{
		const char* lastPathIndex = strchr(beginFilename, '\\');
		if (lastPathIndex == NULL)
			lastPathIndex = strchr(beginFilename, '/');

		if (lastPathIndex)
		{
			char temp[512];
			memcpy(temp, filename, lastPathIndex - filename);
			temp[lastPathIndex - filename] = 0;

			if (!PathIsDirectory(temp))
			{
				CreateDirectory(temp, NULL);
			}
			beginFilename = lastPathIndex + 1;
		}
		else
		{
			return;
		}
	}
}

/*
FileDbIndex *cFilePackage::GetFirstItem()
{
	if ( m_table.empty() ) return NULL;
	m_currentIter = m_table.begin();
	return *m_currentIter;
}

FileDbIndex *cFilePackage::GetNextItem()
{
	if ( m_currentIter != m_table.end() ) m_currentIter++;
	if ( m_currentIter != m_table.end() ) {
		return *m_currentIter;
	}
	return NULL;
}
*/

void cFilePackage::SetDirectory(const TCHAR* dirStr)
{
	if (dirStr)
	{
		_tcscpy(m_dirStr, dirStr);
		int len = (int)_tcslen(m_dirStr);
		if (len > 0 && m_dirStr[len] != _T('\\'))
			_tcscat(m_dirStr, _T("\\"));
	}
}

bool cFilePackage::AddToTable(string filename, FileDbIndex& fileInfo)
{
	char buf[SAFE_PATH_MAX];
	TransFilename(filename.c_str(), buf, SAFE_PATH_MAX);


#if FILEPACKAGE_USE_TST_DICTIONARY
	m_table.Insert(buf, fileInfo);
#else
	m_table[buf] = fileInfo;
#endif

	//m_table.insert( make_pair(buf, fileInfo) );
	return true;
}

FileDbIndex* cFilePackage::Find(string filename)
{
	char buf[SAFE_PATH_MAX];
	TransFilename(filename.c_str(), buf, SAFE_PATH_MAX);

#if FILEPACKAGE_USE_TST_DICTIONARY
#if FILEPACKAGE_DEBUG_PRINT_FILE_FIND
	OutputDebugString("cFilePackage::Find(");
	OutputDebugString(buf);
	OutputDebugString(")\n");
#endif
	return m_table.Find(buf);
#else
	mapFileIndex::iterator iter = m_table.find(buf);
	if (iter != m_table.end())
	{
		return &(iter->second);
	}

	return NULL;
#endif
}

TCHAR* cFilePackage::GetFullNameStr(const TCHAR* fileNameStr)
{
	static TCHAR fullNameStr[SAFE_PATH_MAX];
	if (fileNameStr) {
		_tcscpy(fullNameStr, m_dirStr);
		_tcscat(fullNameStr, fileNameStr);
		return fullNameStr;
	}
	return NULL;
}

const char* cFilePackage::InsertPathname(const char* pathname)
{
	if (pathname == NULL)
		return NULL;

	// 轉換小寫
	char pathnameLower[512];
	strncpy(pathnameLower, pathname, 512);
	_strlwr_s(pathnameLower, 512);

	// 順序比對
	for (vector<char*>::iterator iter = m_openPathname.begin(); iter != m_openPathname.end(); iter++)
	{
		if (strcmp(pathnameLower, *iter) == 0)
			return *iter;
	}

	// 產生新字串
	char* newPathName = new char[strlen(pathnameLower) + 1];
	strcpy(newPathName, pathnameLower);

	// 堆入堆疊內
	m_openPathname.push_back(newPathName);
	return newPathName;
}

void cFilePackage::ReleasePathname()
{
	for (vector<char*>::iterator iter = m_openPathname.begin(); iter != m_openPathname.end(); iter++)
		delete[] * iter;

	m_openPathname.clear();
}

void cFilePackage::AddFileHandle(const char* filename, HANDLE hFile)
{
	if (filename == NULL)
		return;

#if FILEPACKAGE_USE_BOOST_POOLING_STRING
#if FILEPACKAGE_USE_TST_DICTIONARY
#else
	BPString filenameStr = filename;
#endif
#else
	string filenameStr = filename;
#endif

#if FILEPACKAGE_USE_TST_DICTIONARY
	FileHandleRef* pFileHandle = m_fileHandle.Find(filename);
	if (pFileHandle != NULL)
	{
		pFileHandle->AddRefCount();
	}
	else
	{
		FileHandleRef handleRef;
		handleRef.hFile = hFile;
		handleRef.AddRefCount();
		m_fileHandle.Insert(filename, handleRef);
	}
#else
	mapFileHandle::iterator iter = m_fileHandle.find(filenameStr);
	if (iter != m_fileHandle.end())
	{
		iter->second.AddRefCount();
	}
	else
	{
		FileHandleRef handleRef;
		handleRef.hFile = hFile;
		handleRef.AddRefCount();
		m_fileHandle.insert(make_pair(filenameStr, handleRef));
	}
#endif
}

void cFilePackage::RemoveFileHandle(const char* filename)
{
#if FILEPACKAGE_USE_TST_DICTIONARY
	FileHandleRef* pFileHandle = m_fileHandle.Find(filename);
	if (pFileHandle != NULL)
	{
		pFileHandle->DelRefCount();
		if (pFileHandle->RefCount() <= 0)
		{
			CloseHandle(pFileHandle->hFile);
		}

		//m_fileHandle.erase(?); // TODO: 
	}
#else
	mapFileHandle::iterator iter = m_fileHandle.find(filename);
	if (iter != m_fileHandle.end())
	{
		iter->second.DelRefCount();
		if (iter->second.RefCount() <= 0)
		{
			CloseHandle(iter->second.hFile);
		}

		m_fileHandle.erase(iter);
	}
#endif
}

FileHandleRef* cFilePackage::GetFileHandleRef(const char* filename)
{
#if FILEPACKAGE_USE_TST_DICTIONARY
	FileHandleRef* pFileHandle = m_fileHandle.Find(filename);
	if (pFileHandle != NULL)
	{
		pFileHandle->AddRefCount();
	}
	return pFileHandle;
#else
	mapFileHandle::iterator iter = m_fileHandle.find(filename);
	if (iter != m_fileHandle.end())
	{
		iter->second.AddRefCount();
		return &(iter->second);
	}

	return NULL;
#endif
}
