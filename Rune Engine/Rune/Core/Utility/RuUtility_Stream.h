/*!
	@file RuUtility_Stream.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUUTILITY_STREAM_H_
#define _RUUTILITY_STREAM_H_

// Base Includes
#include "../../Core/Type/RuType_Object.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	A utility class for compressing data streams.

	@version 2005/01/13
*/
class CRuStreamCompressor
{
protected:
	INT32									m_bufferSize;
	BYTE*									m_readBuffer;
	BYTE*									m_writeBuffer;

	CRuMemoryStream*						m_compressedChunkStream;			//!< Temporary buffer space for compressed streams

public:
											CRuStreamCompressor();
											~CRuStreamCompressor();

	BOOL									Compress(IRuStream *inputStream, IRuStream *outputStream);
	BOOL									Decompress(IRuStream *inputStream, IRuStream *outputStream);

	BOOL									Decompress(IRuStream *inputStream, INT32 offset, INT32 compressedDataSize, IRuStream *outputStream);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
