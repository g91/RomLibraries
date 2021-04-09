#include "../../Core/Stream/RuArchiveIO.h"

// ZLib Header
#include "../../../Tools/zLib/zLib.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

CRuDirectoryArchive::CRuDirectoryArchive(const char *rootDirectory)
{
	if(rootDirectory)
	{
		m_rootDirectory = ruNEW char [strlen(rootDirectory) + 1];
		strcpy(m_rootDirectory, rootDirectory);
	}
	else
	{
		m_rootDirectory = ruNEW char [1];
		m_rootDirectory[0] = NULL;
	}
}

CRuDirectoryArchive::~CRuDirectoryArchive()
{
	delete[] m_rootDirectory;
}

IRuStream *CRuDirectoryArchive::ReadResource(const char *resourceName)
{
	char aggregateName[2048];

	if(m_rootDirectory[0] != NULL)
		sprintf(aggregateName, "%s\\%s", m_rootDirectory, resourceName);
	else
		sprintf(aggregateName, "%s", resourceName);

	CRuFileStream *stream = ruNEW CRuFileStream();

	if(stream->Open(resourceName) != ruSEC_OK)
	{
		delete stream;
		return NULL;
	}

	return stream;
}

RuArchiveErrorCode CRuDirectoryArchive::WriteResource(const char *resourceName, IRuStream *resource, BOOL overwrite)
{
	return ruAEC_WriteError;
}

RuArchiveErrorCode CRuDirectoryArchive::RenameResource(const char *oldName, const char *newName)
{
	return ruAEC_WriteError;
}

RuArchiveErrorCode CRuDirectoryArchive::DeleteResource(const char *resourceName)
{
	return ruAEC_WriteError;
}

UINT32 CRuDirectoryArchive::GetResourceCount(void)
{
	return 0;
}

char **CRuDirectoryArchive::GetResourceNames(void)
{
	return NULL;
}

// ************************************************************************************************************************************************************
/*
RuArchiveErrorCode CRuCompressedArchive::Create(const char *archiveName)
{
	SCAHeader header;

	// Close existing directory file
	Close();

	// Create new directory file
	m_stream = new CRuFileStream;
	((CRuFileStream *) m_stream)->Create(archiveName);
	m_readOnly = FALSE;

	// Save dummy header to reserve space for the header
	header.m_fileID = 0x00000000;
	header.m_indexOffset = 0;
	header.m_numIndices = 0;
	m_stream->Seek(0, ruSSM_Begin);
	m_stream->Write(&header, sizeof(SCAHeader));

	return ruAEC_OK;
}

RuArchiveErrorCode CRuCompressedArchive::Compact(void)
{
	return ruAEC_OK;
}

RuArchiveErrorCode CRuCompressedArchive::Open(const char *archiveName, BOOL readOnly)
{
	DWORD i;
	SCAHeader header;
	SCAIndex *index;

	// Close existing archive
	Close();

	// Open archive
	RuStreamErrorCode openEC;
	m_stream = new CRuFileStream;
	if(readOnly)
		openEC = ((CRuFileStream *) m_stream)->Open(archiveName, ruSOM_OpenRead);
	else
		openEC = ((CRuFileStream *) m_stream)->Open(archiveName, ruSOM_OpenReadWrite);
	if(openEC != ruSEC_OK)
		return ruAEC_CannotOpen;
	m_readOnly = readOnly;

	// Read header data
	if(m_stream->Seek(0, ruSSM_Begin))
		return ruAEC_SeekError;
	if(m_stream->Read(&header, sizeof(SCAHeader)) != sizeof(SCAHeader))
		return ruAEC_ReadError;

	// Build index tree
	m_stream->Seek(header.m_indexOffset, ruSSM_Begin);
	for(i = 0; i < header.m_numIndices; i++)
	{
		// Check for read error
		index = new SCAIndex;
		if(ReadIndex(index))
		{
			delete index;
			return ruAEC_ReadError;
		}

		// Insert index into tree
		m_indexTree.Insert(index, index, FALSE);
	}

	// Balance index tree
	m_indexTree.BalanceTree();

	return ruAEC_OK;
}

void CRuCompressedArchive::Close(void)
{
	SCAHeader header;
	CRuArrayList<SCAIndex *> indexList;

	// Enumerate all indices
	m_indexTree.EnumerateKeys(&indexList, ruTTM_PreOrder);

	if(m_stream != NULL)
	{
		// Save index data to end of archive if not in read-only mode
		if(!m_readOnly)
		{
			// Setup archive header
			m_stream->Seek(0, ruSSM_End);
			header.m_fileID = 0x00000000;
			header.m_indexOffset = m_stream->GetPosition();
			header.m_numIndices = indexList.Count();

			// Save each index entry
			for(INT32 i = 0; i < indexList.Count(); i++)
				WriteIndex(indexList[i]);

			// Save header data
			m_stream->Seek(0, ruSSM_Begin);
			m_stream->Write(&header, sizeof(SCAHeader));
		}

		// Close stream and delete stream object
		m_stream->Close();
		delete m_stream;
		m_stream = NULL;
	}

	// Release index tree
	for(INT32 i = 0; i < indexList.Count(); i++)
		delete indexList[i];
	m_indexTree.Release();
}

IRuStream *CRuCompressedArchive::ReadResource(const char *resourceName)
{
	SCAIndex key, *index;
	z_stream zStream;
	unsigned char *inBuffer, outBuffer[8192];
	unsigned int inBytes;
	BOOL done = FALSE;

	// If directory is not open, return
	if(m_stream == NULL)
		return NULL;
	if(resourceName == NULL)
		return NULL;
	if(strlen(resourceName) == 0)
		return NULL;

	// If object is not found, return
	key.SetName(resourceName);
	index = m_indexTree.Find(&key);
	if(index == NULL)
		return NULL;
	if((index->m_chunkSize == 0) || (index->m_originalSize == 0))
		return NULL;

	// Clear zStream structure
	memset(&zStream, 0, sizeof(z_stream));

	// Decompress object stream data while reading
	CRuMemoryStream *deIRuStream;
	deIRuStream = new CRuMemoryStream;
	deIRuStream->SetBufferSize(index->m_originalSize);
	inBuffer = new BYTE [index->m_chunkSize];

	// Read all input bytes
	deIRuStream->Seek(0, ruSSM_Begin);
	m_stream->Seek(index->m_offset, ruSSM_Begin);
	inBytes = m_stream->Read(inBuffer, index->m_chunkSize);
	if(inBytes != index->m_chunkSize)
	{
		delete[] inBuffer;
		return NULL;
	}
	inflateInit(&zStream);

	// Perform actual decompression
	if(inBytes > 0)
	{
		zStream.next_in = inBuffer;
		zStream.avail_in = inBytes;
		zStream.next_out = outBuffer;
		zStream.avail_out = 8192;
		do
		{
			switch(inflate(&zStream, 0))
			{
				case Z_OK:
					if(zStream.avail_out < 8192)
					{
						deIRuStream->Write(outBuffer, 8192 - zStream.avail_out);
						zStream.next_out = outBuffer;
						zStream.avail_out = 8192;
					}
					break;
				case Z_STREAM_END:
					if(zStream.avail_out < 8192)
					{
						deIRuStream->Write(outBuffer, 8192 - zStream.avail_out);
						zStream.next_out = outBuffer;
						zStream.avail_out = 8192;
					}
					done = TRUE;
					break;
				case Z_NEED_DICT:
				case Z_STREAM_ERROR:
				case Z_MEM_ERROR:
				case Z_DATA_ERROR:
				case Z_BUF_ERROR:
				default:
					delete[] inBuffer;
					inflateEnd(&zStream);
					delete deIRuStream;
					return NULL;
			}
		} while(!done);
	}
	delete[] inBuffer;
	inflateEnd(&zStream);

	// Return decompressed data stream
	return deIRuStream;
}

RuArchiveErrorCode CRuCompressedArchive::WriteResource(const char *resourceName, IRuStream *resource, BOOL overwrite)
{
	SCAIndex key, *index;
	z_stream zStream;
	unsigned char *inBuffer, outBuffer[8192];
	unsigned int inBytes;
	BOOL done = FALSE;

	// If stream resource is not open, return
	if((resource == NULL) || (!resource->IsOpen()))
		return ruAEC_ResourceInvalid;

	// If directory is not open or readonly mode, return
	if(m_stream == NULL)
		return ruAEC_ArchiveNotOpen;
	if(m_readOnly)
		return ruAEC_ReadOnly;

	// If resource is found and overwrite is FALSE, return
	key.SetName(resourceName);
	index = m_indexTree.Find(&key);
	if(index != NULL)
	{
		if(overwrite == FALSE)
			return ruAEC_ResourceAlreadyExists;
		else
		{
			m_indexTree.Remove(index);
			delete index;
		}
	}

	// Clear zStream structure
	memset(&zStream, 0, sizeof(z_stream));

	// Compress resource stream and write to file
	resource->Seek(0, ruSSM_Begin);
	m_stream->Seek(0, ruSSM_End);
	index = new SCAIndex;
	index->SetName(resourceName);
	index->m_originalSize = resource->GetStreamSize();
	index->m_offset = m_stream->GetPosition();
	index->m_chunkSize = 0;
	inBuffer = new BYTE [index->m_originalSize];

	// Read all input data from source resource
	inBytes = resource->Read(inBuffer, index->m_originalSize);
	if(inBytes != index->m_originalSize)
	{
		delete[] inBuffer;
		return ruAEC_WriteError;
	}
	deflateInit(&zStream, Z_DEFAULT_COMPRESSION);

	// Perform actual compression
	if(inBytes > 0)
	{
		zStream.next_in = inBuffer;
		zStream.avail_in = inBytes;
		zStream.next_out = outBuffer;
		zStream.avail_out = 8192;
		do
		{
			switch(deflate(&zStream, Z_FINISH))
			{
				// Some progress made
				case Z_OK:
					if(zStream.avail_out < 8192)
					{
						m_stream->Write(outBuffer, 8192 - zStream.avail_out);
						index->m_chunkSize += (8192 - zStream.avail_out);
						zStream.next_out = outBuffer;
						zStream.avail_out = 8192;
					}
					break;
				// Compression of the entire block is done
				case Z_STREAM_END:
					if(zStream.avail_out < 8192)
					{
						m_stream->Write(outBuffer, 8192 - zStream.avail_out);
						index->m_chunkSize += (8192 - zStream.avail_out);
						zStream.next_out = outBuffer;
						zStream.avail_out = 8192;
					}
					done = TRUE;
					break;
				default:
				case Z_BUF_ERROR:
				case Z_STREAM_ERROR:
//					cout << "Stream error!" << endl;
					delete[] inBuffer;
					deflateEnd(&zStream);
					return ruAEC_WriteError;
			}
		} while(!done);
	}
	delete[] inBuffer;
	deflateEnd(&zStream);

	// Insert new index data into index tree
	if(!m_indexTree.Insert(index, index, TRUE))
		return ruAEC_Invalid;

	return ruAEC_OK;
}

RuArchiveErrorCode CRuCompressedArchive::RenameResource(const char *oldName, const char *newName)
{
	SCAIndex key, *index;

	// If directory is not open or readonly mode, return
	if(m_stream == NULL)
		return ruAEC_ArchiveNotOpen;
	if(m_readOnly)
		return ruAEC_ReadOnly;

	// If new name is already taken, return
	key.SetName(newName);
	index = m_indexTree.Find(&key);
	if(index != NULL)
		return ruAEC_ResourceAlreadyExists;

	// If old name is not found, return
	key.SetName(oldName);
	index = m_indexTree.Find(&key);
	if(index == NULL)
		return ruAEC_ResourceNotFound;

	// Remove the old index from tree
	m_indexTree.Remove(index);

	// Change object name
	index->SetName(newName);

	// Store new key
	if(!m_indexTree.Insert(index, index, FALSE))
		return ruAEC_Invalid;

	return ruAEC_OK ;
}

RuArchiveErrorCode CRuCompressedArchive::DeleteResource(const char *resourceName)
{
	SCAIndex key;

	// If directory is not open or readonly mode, return
	if(m_stream == NULL)
		return ruAEC_ArchiveNotOpen;
	if(m_readOnly)
		return ruAEC_ReadOnly;

	// Delete the key
	key.SetName(resourceName);
	if(!m_indexTree.Remove(&key))
		return ruAEC_ResourceNotFound;

	return ruAEC_OK;
}

UINT32 CRuCompressedArchive::GetResourceCount(void)
{
	return m_indexTree.Count();
}

char **CRuCompressedArchive::GetResourceNames(void)
{
	UINT32 i, resCount = m_indexTree.Count();

	// Abort if there are no resources
	if(resCount == 0)
		return NULL;

	// Get the list of indices
	CRuArrayList<SCAIndex *> keyList;
	m_indexTree.EnumerateKeys(&keyList);

	// Iterate through the indices and build a name list
	char **nameList = new char * [resCount];
	for(i = 0; i < resCount; i++)
	{
		nameList[i] = new char [keyList[i]->m_nameLength];
		strcpy(nameList[i], keyList[i]->m_name);
	}

	return nameList;
}

RuArchiveErrorCode CRuCompressedArchive::ReadIndex(SCAIndex *index)
{
	m_stream->Read(&index->m_nameLength, sizeof(index->m_nameLength));
	m_stream->Read(&index->m_offset, sizeof(index->m_offset));
	m_stream->Read(&index->m_chunkSize, sizeof(index->m_chunkSize));
	m_stream->Read(&index->m_originalSize, sizeof(index->m_originalSize));
	if(index->m_name != NULL)
		delete[] index->m_name;
	index->m_name = new char [index->m_nameLength];
	m_stream->Read(index->m_name, index->m_nameLength);
	return ruAEC_OK;
}

RuArchiveErrorCode CRuCompressedArchive::WriteIndex(SCAIndex *index)
{
	m_stream->Write(&index->m_nameLength, sizeof(index->m_nameLength));
	m_stream->Write(&index->m_offset, sizeof(index->m_offset));
	m_stream->Write(&index->m_chunkSize, sizeof(index->m_chunkSize));
	m_stream->Write(&index->m_originalSize, sizeof(index->m_originalSize));
	if(index->m_name != NULL)
		m_stream->Write(index->m_name, index->m_nameLength);
	return ruAEC_OK;
}
*/
// ************************************************************************************************************************************************************

#pragma managed
