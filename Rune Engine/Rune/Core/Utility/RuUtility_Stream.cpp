#include "../../Core/Utility/RuUtility_Stream.h"

#include "../../../Tools/zLib/zlib.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

CRuStreamCompressor::CRuStreamCompressor()
:	m_bufferSize(262144)
{
	m_readBuffer = ruNEW BYTE [m_bufferSize];
	m_writeBuffer = ruNEW BYTE [m_bufferSize];

	m_compressedChunkStream = ruNEW CRuMemoryStream();
}

CRuStreamCompressor::~CRuStreamCompressor()
{
	delete[] m_readBuffer;
	delete[] m_writeBuffer;

	delete m_compressedChunkStream;
}

BOOL CRuStreamCompressor::Compress(IRuStream *inputStream, IRuStream *outputStream)
{
	z_stream zStream;
	memset(&zStream, 0, sizeof(z_stream));
	deflateInit(&zStream, Z_DEFAULT_COMPRESSION);

	UINT32 bytesRead, outputSize = 0;

	inputStream->Seek(0, ruSSM_Begin);
	while(!inputStream->EndOfStream())
	{
		bytesRead = inputStream->Read(m_readBuffer, m_bufferSize);

		zStream.next_in = m_readBuffer;
		zStream.avail_in = bytesRead;
		zStream.next_out = m_writeBuffer;
		zStream.avail_out = m_bufferSize;

		do
		{
			switch(inputStream->EndOfStream() ? deflate(&zStream, Z_SYNC_FLUSH) : deflate(&zStream, Z_NO_FLUSH))
			{
				case Z_OK:				// Some progress made
				case Z_STREAM_END:		// Compression of the entire block is done
					break;

				default:
				case Z_BUF_ERROR:
				case Z_STREAM_ERROR:
					{
						deflateEnd(&zStream);
					}

					return FALSE;
			}

			// Save available output if there is any available
			if(zStream.avail_out < (UINT32) m_bufferSize)
			{
				outputSize += (m_bufferSize - zStream.avail_out);
				outputStream->Write(m_writeBuffer, m_bufferSize - zStream.avail_out);
				zStream.next_out = m_writeBuffer;
				zStream.avail_out = m_bufferSize;
			}

		} while(zStream.avail_in > 0);
	}

	deflateEnd(&zStream);

	return TRUE;
}

BOOL CRuStreamCompressor::Decompress(IRuStream *inputStream, IRuStream *outputStream)
{
	INT32 compressedChunkSize = inputStream->GetStreamSize();

	// Decompress stream into memory
	INT32 readInputSize = 0;
	INT32 bytesRead;

	z_stream zStream;
	memset(&zStream, 0, sizeof(z_stream));
	inflateInit(&zStream);

	while(readInputSize < compressedChunkSize)
	{
		bytesRead = inputStream->Read(m_readBuffer, min(m_bufferSize, compressedChunkSize - readInputSize));
		readInputSize += bytesRead;

		zStream.next_in = m_readBuffer;
		zStream.avail_in = bytesRead;
		zStream.next_out = m_writeBuffer;
		zStream.avail_out = m_bufferSize;

		do
		{
			switch(inflate(&zStream, Z_SYNC_FLUSH))
			{
				case Z_OK:				// Some progress made
				case Z_STREAM_END:		// Compression of the entire block is done
					break;

				default:
				case Z_BUF_ERROR:
				case Z_STREAM_ERROR:
					{
						inflateEnd(&zStream);
					}

					return FALSE;
			}

			// If there are decompressed bytes available, write into the chunk stream
			if(zStream.avail_out < (UINT32) m_bufferSize)
			{
				outputStream->Write(m_writeBuffer, m_bufferSize - zStream.avail_out);
				zStream.next_out = m_writeBuffer;
				zStream.avail_out = m_bufferSize;
			}
		} while(zStream.avail_in > 0);
	}

	inflateEnd(&zStream);

	return TRUE;
}

BOOL CRuStreamCompressor::Decompress(IRuStream *inputStream, INT32 offset, INT32 compressedDataSize, IRuStream *outputStream)
{
	// Seek to location of the water body stream
	inputStream->Seek(offset, ruSSM_Begin);

	// Read compressed water body stream
	m_compressedChunkStream->SetStreamSize(compressedDataSize);
	if(inputStream->Read(m_compressedChunkStream->GetBuffer(), compressedDataSize) != compressedDataSize)
	{
		// Read failed
		return FALSE;
	}

	// Set output stream to empty
	outputStream->SetStreamSize(0);

	// Decompress the stream
	m_compressedChunkStream->Seek(0, ruSSM_Begin);
	Decompress(m_compressedChunkStream, outputStream);

	// Rewind output stream
	outputStream->Seek(0, ruSSM_Begin);

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
