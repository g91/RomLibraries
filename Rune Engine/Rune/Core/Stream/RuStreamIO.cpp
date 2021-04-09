#include "../../Core/Stream/RuStreamIO.h"
#include "../../Core/Utility/RuUtility.h"

#pragma unmanaged

// *** Desc : Default constructor
//     Ret  : N/A
CRuFileStream::CRuFileStream()
:	m_accessMode(ruSAM_NotOpen),
	m_file(INVALID_HANDLE_VALUE),
	m_fileMapping(NULL),
	m_fileMappingAddress(NULL)
{
}

// *** Desc : Default destructor
//     Ret  : N/A
CRuFileStream::~CRuFileStream()
{
	Close();
}

// *** Desc : Opens the specified file using openMode
//     Ret  : ruSEC_OK if successful, ruSEC_CannotOpen if open failed, or
//            ruSEC_Invalid if the specified open mode is invalid
RuStreamErrorCode CRuFileStream::Open(const char *filename, RuStreamOpenMethod openMode)
{
	m_accessMode = ruSAM_NotOpen;

	if(openMode == ruSOM_OpenRead)
	{
		if((m_file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE)
		{
			DWORD dwErr = GetLastError();
			return ruSEC_CannotOpen;
		}

		m_accessMode = ruSAM_ReadOnly;

		return ruSEC_OK;
	}
	else if(openMode == ruSOM_OpenReadWrite)
	{
		if((m_file = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE)
			return ruSEC_CannotOpen;

		m_accessMode = ruSAM_ReadWrite;

		return ruSEC_OK;
	}

	return ruSEC_Invalid;
}

// *** Desc : Creates a new file using the specified file name, and the file is opened in read-write mode
//     Ret  : ruSEC_OK is successful, ruSEC_CannotOpen if create failed
RuStreamErrorCode CRuFileStream::Create(const char *filename, BOOL createPath)
{
	m_accessMode = ruSAM_NotOpen;

	if((m_file = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL)) == INVALID_HANDLE_VALUE)
	{
		DWORD lastError = GetLastError();

		switch(lastError)
		{
			default:
				return ruSEC_CannotOpen;

			case ERROR_PATH_NOT_FOUND:
			case ERROR_BAD_PATHNAME:
				{
					if(createPath)
					{
						CRuString pathName(filename);
						pathName.TruncateAt(max(pathName.LastIndexOf("\\"), pathName.LastIndexOf("/")));

						BOOL result = SHCreateDirectoryEx(NULL, static_cast<char *>(pathName), NULL);
						if(result == ERROR_SUCCESS)
						{
							return Create(filename);
						}
					}
				}

				break;
		}

		return ruSEC_CannotOpen;
	}

	m_accessMode = ruSAM_ReadWrite;

	return ruSEC_OK;
}

// *** Desc : Determines whether the stream is open
//     Ret  : TRUE if stream is open, FALSE otherwise
BOOL CRuFileStream::IsOpen()
{
	if(m_accessMode == ruSAM_NotOpen)
		return FALSE;

	return TRUE;
}

// *** Desc : Closes the file handle associated with the CRuFileStream object
//     Ret  : None
void CRuFileStream::Close()
{
	// Close any file mappings
	CloseStreamMapping();

	// Close the file
	if(m_file != INVALID_HANDLE_VALUE)
		CloseHandle(m_file);

	// Reset member variables
	m_file = INVALID_HANDLE_VALUE;
	m_accessMode = ruSAM_NotOpen;
}

// *** Desc : Reads a chunk of data of size readSize at the current file position and stores the data
//            into destBuffer
//     Ret  : The number of bytes read
DWORD CRuFileStream::Read(void *destBuffer, DWORD readSize)
{
	DWORD bytesRead;

	if(m_accessMode != ruSAM_NotOpen)
	{
		if(!ReadFile(m_file, destBuffer, readSize, &bytesRead, NULL))
			return 0;

		return bytesRead;
	}

	return 0;
}

// *** Desc : Writes a chunk of data of size writeSize at the current file position
//     Ret  : ruSEC_OK if successful, ruSEC_WriteError if a general write error occured,
//            or ruSEC_ReadOnly if file is not open in write mode
RuStreamErrorCode CRuFileStream::Write(const void *srcBuffer, DWORD writeSize)
{
	DWORD bytesWritten;

	if(m_accessMode == ruSAM_ReadWrite)
	{
		if(!WriteFile(m_file, srcBuffer, writeSize, &bytesWritten, NULL))
			return ruSEC_WriteError;

		return ruSEC_OK;
	}

	return ruSEC_ReadOnly;
}

// *** Desc : Determines whether the current stream position is at the end of the stream
//     Ret  : TRUE if EOF, FALSE otherwise
int CRuFileStream::EndOfStream()
{
	if(m_accessMode == ruSAM_NotOpen)
		return 1;

	BYTE dummyBuffer[4];
	INT32 readSize = 4;
	DWORD bytesRead;
	if(ReadFile(m_file, dummyBuffer, readSize, &bytesRead, NULL) && bytesRead == 0)
		return 1;

	// Seek back
	Seek(-readSize, ruSSM_Current);

	return 0;
}

// *** Desc : Performs a seek to set the current file position to the specified position
//            using the specified seek method
//     Ret  : ruSEC_OK if successful, ruSEC_SeekError if a general seek error
//            occured, or ruSEC_StreamNotOpen if the file is not open
RuStreamErrorCode CRuFileStream::Seek(INT offset, RuStreamSeekMethod seekType)
{
	if(m_accessMode != ruSAM_NotOpen)
	{
		switch(seekType)
		{
			case ruSSM_Begin:
				if(SetFilePointer(m_file, offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
					return ruSEC_SeekError;
				break;
			case ruSSM_Current:
				if(SetFilePointer(m_file, offset, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
					return ruSEC_SeekError;
				break;
			case ruSSM_End:
				if(SetFilePointer(m_file, offset, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
					return ruSEC_SeekError;
				break;
		}

		return ruSEC_OK;
	}

	return ruSEC_StreamNotOpen;
}

// *** Desc : Sets the size of the stream to the specified size
//     Ret  : ruSEC_OK if successful, ruSEC_StreamNotOpen if file is
//            not open
RuStreamErrorCode CRuFileStream::SetStreamSize(DWORD newSize)
{
	if(m_accessMode != ruSAM_NotOpen)
	{
		SetFilePointer(m_file, newSize - 1, NULL, FILE_BEGIN);
		SetEndOfFile(m_file);

		return ruSEC_OK;
	}

	return ruSEC_StreamNotOpen;
}

// *** Desc : Gets the size of the stream
//     Ret  : The size of the stream
DWORD CRuFileStream::GetStreamSize()
{
	if(m_accessMode != ruSAM_NotOpen)
	{
		DWORD fileSizeHigh;
		return GetFileSize(m_file, &fileSizeHigh);
	}

	return 0;
}

// *** Desc : Gets the current position in the stream
//     Ret  : The current position in the stream
DWORD CRuFileStream::GetPosition()
{
	if(m_accessMode != ruSAM_NotOpen)
	{
		return SetFilePointer(m_file, 0, NULL, FILE_CURRENT);
	}

	return 0;
}

BYTE *CRuFileStream::OpenStreamMapping()
{
	if(m_accessMode == ruSAM_NotOpen)
	{
		return NULL;
	}

	if(m_fileMapping == NULL)
	{
		m_fileMapping = CreateFileMapping(m_file, NULL, PAGE_READONLY, 0, 0, NULL);
	}

	if(m_fileMapping != NULL)
	{
		m_fileMappingAddress = MapViewOfFile(m_fileMapping, FILE_MAP_READ, 0, 0, 0);
	}

	return (BYTE *) m_fileMappingAddress;
}

void CRuFileStream::CloseStreamMapping()
{
	if(m_accessMode == ruSAM_NotOpen)
	{
		return;
	}

	if(m_fileMappingAddress)
	{
		UnmapViewOfFile(m_fileMappingAddress);
	}

	if(m_fileMapping)
	{
		CloseHandle(m_fileMapping);
	}

	m_fileMapping = NULL;
	m_fileMappingAddress = NULL;
}

RuStreamErrorCode CRuFileStream::LockStream(INT32 offset, INT32 length, BOOL exclusive)
{
	if(m_accessMode == ruSAM_ReadWrite)
	{
		OVERLAPPED overlappedStruct;
		overlappedStruct.Offset = offset;
		overlappedStruct.OffsetHigh = 0;
		overlappedStruct.hEvent = 0;

		if(exclusive)
		{
			if(LockFileEx(m_file, LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY, 0, length, 0, &overlappedStruct))
			{
				return ruSEC_OK;
			}
		}
		else
		{
			if(LockFileEx(m_file, LOCKFILE_FAIL_IMMEDIATELY, 0, length, 0, &overlappedStruct))
			{
				return ruSEC_OK;
			}
		}
	}

	return ruSEC_Invalid;
}

RuStreamErrorCode CRuFileStream::UnlockStream(INT32 offset, INT32 length)
{
	if(m_accessMode == ruSAM_ReadWrite)
	{
		OVERLAPPED overlappedStruct;
		overlappedStruct.Offset = offset;
		overlappedStruct.OffsetHigh = 0;
		overlappedStruct.hEvent = 0;

		if(UnlockFileEx(m_file, 0, length, 0, &overlappedStruct))
		{
			return ruSEC_OK;
		}
	}

	return ruSEC_Invalid;
}

// *** Desc : Clones the stream
//     Ret  : None
void CRuFileStream::operator=(IRuStream &operand)
{
	int bytesRead;
	char tempBuffer[2048];

	if(m_accessMode != ruSAM_ReadWrite)
		return;

	operand.Seek(0, ruSSM_Begin);
	Seek(0, ruSSM_Begin);
	while(!operand.EndOfStream())
	{
		bytesRead = operand.Read(tempBuffer, 2048);
		Write(tempBuffer, bytesRead);
	}
}












CRuMemoryStream::CRuMemoryStream()
: m_stream(NULL), m_streamSize(0), m_memoryBufferSize(0), m_position(0)
{
	SetBufferSize();
}

CRuMemoryStream::~CRuMemoryStream()
{
	Close();
}

BOOL CRuMemoryStream::IsOpen()
{
	if(m_stream == NULL)
		return FALSE;

	return TRUE;
}

DWORD CRuMemoryStream::SetBufferSize(DWORD newSize)
{
	BYTE *newBuffer;

	if((newSize <= 0) || (newSize < m_streamSize))
		return ruSEC_Invalid;

	newBuffer = ruNEW BYTE [newSize];
	if(m_stream != NULL)
	{
		if(m_streamSize > 0)
			memcpy(newBuffer, m_stream, m_streamSize);
		delete[] m_stream;
	}
	m_stream = newBuffer;
	m_memoryBufferSize = newSize;

	return ruSEC_OK;
}

void CRuMemoryStream::Close()
{
	if(m_stream != NULL)
		delete[] m_stream;

	m_stream = NULL;
	m_streamSize = 0;
	m_memoryBufferSize = 0;
	m_position = 0;
}

DWORD CRuMemoryStream::Read(void *destBuffer, DWORD readSize)
{
	if((m_stream == NULL) || (m_streamSize == 0) || (m_position >= m_streamSize))
		return 0;

	if(readSize <= (m_streamSize - m_position))
	{
		memcpy(destBuffer, &m_stream[m_position], readSize);
		m_position += readSize;

		return readSize;
	}
	else
	{
		readSize = m_streamSize - m_position;
		memcpy(destBuffer, &m_stream[m_position], readSize);
		m_position += readSize;

		return readSize;
	}

	return 0;
}

RuStreamErrorCode CRuMemoryStream::Write(const void *srcBuffer, DWORD writeSize)
{
	if(writeSize <= 0)
		return ruSEC_OK;

	// Not enough buffer space, so we reallocate a new buffer
	if((m_stream == NULL) || (writeSize > (m_memoryBufferSize - m_position)))
	{
		BYTE *newBuffer;
		DWORD NewBufferSize;

		// NOTE - NewBufferSize is extremely critical to the performance of CRuMemoryStream...
		//        Too small, and the buffer will have to be resized very soon again. Too large,
		//        memory is wasted.
		if((m_memoryBufferSize - m_position) < 0)
			NewBufferSize = m_position + writeSize;
		else
			NewBufferSize = m_streamSize + writeSize;

		newBuffer = ruNEW BYTE [NewBufferSize];
		if(m_stream != NULL)
		{
			if(m_streamSize > 0)
				memcpy(newBuffer, m_stream, m_streamSize);
			delete[] m_stream;
		}
		m_stream = newBuffer;
		m_memoryBufferSize = NewBufferSize;
	}

	// write data into memory stream
	memcpy(&m_stream[m_position], srcBuffer, writeSize);
	m_position += writeSize;
	if(m_position > m_streamSize)
		m_streamSize = m_position;

	return ruSEC_OK;
}

int CRuMemoryStream::EndOfStream()
{
	if(m_position >= m_streamSize)
		return 1;

	return 0;
}

RuStreamErrorCode CRuMemoryStream::Seek(int offset, RuStreamSeekMethod seekType)
{
	switch(seekType)
	{
		case ruSSM_Begin:
			if((offset < 0) || ((DWORD) offset > m_streamSize))
				return ruSEC_SeekError;
			m_position = offset;
			break;
		case ruSSM_Current:
			if(((m_position + offset) < 0) || ((m_position + offset) > m_streamSize))
				return ruSEC_SeekError;
			m_position += offset;
			break;
		case ruSSM_End:
			if(((m_streamSize + offset) < 0) || ((m_streamSize + offset) > m_streamSize))
				return ruSEC_SeekError;
			m_position = m_streamSize + offset;
			break;
	}

	return ruSEC_OK;
}

RuStreamErrorCode CRuMemoryStream::SetStreamSize(DWORD newSize)
{
	BYTE *newBuffer;

	// New size cannot be less than 0!
	if(newSize < 0)
		return ruSEC_Invalid;

	if(newSize <= m_memoryBufferSize)
	{
		m_streamSize = newSize;

		// If position pointer is past the end of the stream, set it at the end of the stream
		if(m_position >= m_streamSize)
			m_position = m_streamSize;
	}
	else
	{
		newBuffer = ruNEW BYTE [newSize];
		if(m_stream != NULL)
		{
			if(m_streamSize > 0)
				memcpy(newBuffer, m_stream, m_streamSize);
			delete[] m_stream;
		}
		m_stream = newBuffer;
		m_memoryBufferSize = newSize;
		m_streamSize = newSize;
	}

	return ruSEC_OK;
}

DWORD CRuMemoryStream::GetStreamSize()
{
	return m_streamSize;
}

DWORD CRuMemoryStream::GetPosition()
{
	return m_position;
}

void CRuMemoryStream::operator=(IRuStream &operand)
{
	int bytesRead;
	char tempBuffer[2048];
	operand.Seek(0, ruSSM_Begin);
	Seek(0, ruSSM_Begin);
	while(!operand.EndOfStream())
	{
		bytesRead = operand.Read(tempBuffer, 2048);
		Write(tempBuffer, bytesRead);
	}
}

#ifdef KALYDO
CRuMultiFileStream::CRuMultiFileStream(DWORD sizePerFile)
:	m_sizePerFile(sizePerFile),
	m_currentFile(-1),
	m_currentCacheFile(-1)
{
	for (int i = 0; i < RUMULTIFILE_CACHE_SIZE; ++i)
		m_filesInCache[i] = -1;
}

CRuMultiFileStream::~CRuMultiFileStream()
{
	Close();
}

RuStreamErrorCode CRuMultiFileStream::Open(const char *filename, RuStreamOpenMethod openMode)
{
	if (openMode != ruSOM_OpenRead)
		return ruSEC_Invalid;

	m_baseFileName = filename;
	return SwitchFile(0);
}

BOOL CRuMultiFileStream::IsOpen()
{
	return m_currentCacheFile != -1;
}

void CRuMultiFileStream::Close()
{
	for (int i = 0; i < RUMULTIFILE_CACHE_SIZE; ++i)
	{
		m_filesInCache[i] = -1;

		if (m_cache[m_currentCacheFile].IsOpen())
			m_cache[m_currentCacheFile].Close();
	}

	m_currentCacheFile = -1;
}


DWORD CRuMultiFileStream::Read(void *destBuffer, DWORD readSize)
{
	DWORD initialReadSize = readSize;
	DWORD bytesRead = 0;
	
	while (true)
	{
		if (m_currentCacheFile != -1)
			bytesRead = m_cache[m_currentCacheFile].Read(destBuffer, readSize);
		else
			bytesRead = 0;

		if (bytesRead == readSize)
			return initialReadSize;

		readSize -= bytesRead;
		((char*&)destBuffer) += bytesRead;

		// switch file if necessary.
		RuStreamErrorCode ret = Seek(0, ruSSM_Current);
		if (ret != ruSEC_OK)
			return initialReadSize - readSize;
	}
}

RuStreamErrorCode CRuMultiFileStream::Write(const void *srcBuffer, DWORD writeSize)
{
	return ruSEC_Invalid;
}

int CRuMultiFileStream::EndOfStream()
{
	return 0;
}

RuStreamErrorCode CRuMultiFileStream::Seek(INT offset, RuStreamSeekMethod seekType)
{
	INT globalOffset = offset;
	switch (seekType)
	{
	case ruSSM_Begin:
		break;
	case ruSSM_Current:
		globalOffset += GetPosition();
		break;
	case ruSSM_End:
		return ruSEC_Invalid;
	}

	RuStreamErrorCode ret = SwitchFile(globalOffset / m_sizePerFile);
	if (ret != ruSEC_OK)
		return ret;
	return m_cache[m_currentCacheFile].Seek(globalOffset % m_sizePerFile, ruSSM_Begin);
}

RuStreamErrorCode CRuMultiFileStream::SetStreamSize(DWORD newSize)
{
	// do nothing
	return ruSEC_Invalid;
}

DWORD CRuMultiFileStream::GetStreamSize()
{
	return -1;
}

DWORD CRuMultiFileStream::GetPosition()
{
	if (m_currentCacheFile == -1)
		return 0;
	return (m_sizePerFile * m_currentFile) + m_cache[m_currentCacheFile].GetPosition();
}

BYTE* CRuMultiFileStream::OpenStreamMapping()
{
	return NULL;
}

void CRuMultiFileStream::CloseStreamMapping()
{
	// do nothing
}

RuStreamErrorCode CRuMultiFileStream::LockStream(INT32 offset, INT32 length, BOOL exclusive)
{
	return ruSEC_Invalid;
}

RuStreamErrorCode CRuMultiFileStream::UnlockStream(INT32 offset, INT32 length)
{
	return ruSEC_Invalid;
}

void CRuMultiFileStream::operator=(IRuStream &operand)
{
	// do nothing
}

RuStreamErrorCode CRuMultiFileStream::SwitchFile(DWORD file)
{
	if (m_currentFile == file)
		return ruSEC_OK;

	m_currentFile = file;

	for (int i = 0; i < RUMULTIFILE_CACHE_SIZE; ++i)
	{
		if (m_filesInCache[i] == file)
		{
#ifdef _DEBUG
			OutputDebugStringA(std::string("Switched to cached file " + GetFileName(file) + "\n").c_str());
#endif
			m_currentCacheFile = i;		
			return ruSEC_OK;
		}
	}

	m_currentCacheFile = (m_currentCacheFile + 1) % RUMULTIFILE_CACHE_SIZE;

	if (m_cache[m_currentCacheFile].IsOpen())
		m_cache[m_currentCacheFile].Close();

	m_filesInCache[m_currentCacheFile] = file;
	RuStreamErrorCode rec = m_cache[m_currentCacheFile].Open(GetFileName(file).c_str());

#ifdef _DEBUG
	OutputDebugStringA(std::string("Switched to file " + GetFileName(file) + "\n").c_str());
	if (rec != ruSEC_OK)
	{
		OutputDebugStringA(std::string("Could not open file " + GetFileName(file) + "\n").c_str());
	}
#endif

	return rec;
}

std::string CRuMultiFileStream::GetFileName(DWORD file)
{
	std::string filename = m_baseFileName;
	char num[128];
	itoa(file, num, 10);
	filename += num;

	return filename;
}

BOOL CRuMultiFileStream::GetFilesContaining(INT32 position, INT32 size, DWORD& startFile, DWORD& endFile)
{
	startFile = position / m_sizePerFile;
	if (size == 0)
		endFile = startFile;
	else
		endFile = (position + size - 1) / m_sizePerFile;
	return TRUE;
}

#endif //KALYDO
#pragma managed
