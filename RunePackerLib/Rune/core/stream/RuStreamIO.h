#ifndef _RUSTREAMIO_H_
#define _RUSTREAMIO_H_

// Standard C Library Includes
#include <stdio.h>
#include <memory.h>

// Base Includes
#include "../../Core/Type/RuType_Base.h"
#include "../../Core/Memory/RuMemoryManager.h"

#pragma unmanaged

// --- RuneEngine Stream IO Routines ---

enum RuStreamSeekMethod
{
	ruSSM_Begin							= 0,
	ruSSM_Current						= 1,
	ruSSM_End							= 2,
	ruSSM_FORCE_DWORD					= 0x7FFFFFFF
};

enum RuStreamOpenMethod
{
	ruSOM_OpenRead						= 0,
	ruSOM_OpenReadWrite					= 1,
	ruSOM_OpenAppend					= 2,
	ruSOM_FORCE_DWORD					= 0x7FFFFFFF
};

enum RuStreamAccessMode
{
	ruSAM_ReadOnly						= 0,
	ruSAM_ReadWrite						= 1,
	ruSAM_NotOpen						= 2,
	ruSAM_FORCE_DWORD					= 0x7FFFFFFF
};

enum RuStreamErrorCode
{
	ruSEC_OK							= 0,
	ruSEC_ReadOnly						= 1,
	ruSEC_SeekError						= 2,
	ruSEC_ReadError						= 3,
	ruSEC_WriteError					= 4,
	ruSEC_CannotOpen					= 5,
	ruSEC_StreamNotOpen					= 6,
	ruSEC_Invalid						= 7,
	ruSEC_FORCE_DWORD					= 0x7FFFFFFF
};

// *** IRuStream
//     Pure Abstract Base Class. Defines interface for all stream classes.
class IRuStream
{
public:
	virtual						~IRuStream() { }

	// Required interface methods
	virtual BOOL				IsOpen() = 0;												// Determines whether the stream is open
	virtual int					EndOfStream() = 0;											// Determines whether end of stream has been reached

	virtual void				Close() = 0;												// Closes stream

	virtual DWORD				Read(void *destBuffer, DWORD readSize) = 0;					// Reads from stream
	virtual RuStreamErrorCode	Write(const void *srcBuffer, DWORD writeSize) = 0;			// Writes to stream
	virtual RuStreamErrorCode	Seek(INT offset, RuStreamSeekMethod seekType) = 0;			// Seek

	virtual RuStreamErrorCode	SetStreamSize(DWORD newSize) = 0;							// Sets stream size, any existing data past the designated size is truncated
	virtual DWORD				GetStreamSize() = 0;										// Returns stream size
	virtual DWORD				GetPosition() = 0;											// Returns current stream position

	// Stream mapping
	virtual BYTE*				OpenStreamMapping() = 0;									// Returns pointer to the memory-mapped stream
	virtual void				CloseStreamMapping() = 0;									// Closes the stream memory map, if any

	// Stream locking
	virtual RuStreamErrorCode	LockStream(INT32 offset, INT32 length, BOOL exclusive) { return ruSEC_Invalid; }
	virtual RuStreamErrorCode	UnlockStream(INT32 offset, INT32 length) { return ruSEC_Invalid; }

	// Operator overrides
	virtual void				operator=(IRuStream &operand) { }							// Copies contents of the source stream
};

// *** CRuFileStream
//     File-based implementation of a IRuStream.
class CRuFileStream : public IRuStream
{
private:
	// Member Variables
	RuStreamAccessMode			m_accessMode;
	HANDLE						m_file;
	HANDLE						m_fileMapping;
	void*						m_fileMappingAddress;

public:
	// CRuFileStream-specific interface
								CRuFileStream();															// Standard no-parameter constructor
	virtual						~CRuFileStream();

	RuStreamErrorCode			Open(const char *filename, RuStreamOpenMethod openMode = ruSOM_OpenRead);	// Opens a file
	RuStreamErrorCode			Create(const char *filename);												// Creates a file in read/write mode, truncating the old file if it exists

	// Implementation of standard IRuStream interface
	virtual BOOL				IsOpen();
	virtual void				Close();

	virtual DWORD				Read(void *destBuffer, DWORD readSize);
	virtual RuStreamErrorCode	Write(const void *srcBuffer, DWORD writeSize);
	virtual int					EndOfStream();
	virtual RuStreamErrorCode	Seek(INT offset, RuStreamSeekMethod seekType);

	virtual RuStreamErrorCode	SetStreamSize(DWORD newSize);
	virtual DWORD				GetStreamSize();
	virtual DWORD				GetPosition();

	virtual BYTE*				OpenStreamMapping();
	virtual void				CloseStreamMapping();

	virtual RuStreamErrorCode	LockStream(INT32 offset, INT32 length, BOOL exclusive);
	virtual RuStreamErrorCode	UnlockStream(INT32 offset, INT32 length);

	virtual void				operator=(IRuStream &operand);												// Copies contents of the stream
};

// *** CRuMemoryStream
//     Memory-based implementation of a IRuStream.
class CRuMemoryStream : public IRuStream
{
private:
	BYTE*						m_stream;
	DWORD						m_streamSize, m_memoryBufferSize, m_position;

public:
	// CRuMemoryStream-specific interface
								CRuMemoryStream();
	virtual						~CRuMemoryStream();

	DWORD						GetBufferSize() const { return m_memoryBufferSize; }
	DWORD						SetBufferSize(DWORD newSize = 256);
	BYTE*						GetBuffer() { return m_stream; }
	BYTE*						GetEndOfStreamPointer() { return &m_stream[m_streamSize]; }

	// Implementation of standard IRuStream interface
	virtual BOOL				IsOpen();
	virtual void				Close();

	virtual DWORD				Read(void *destBuffer, DWORD readSize);
	virtual RuStreamErrorCode	Write(const void *srcBuffer, DWORD writeSize);
	virtual int					EndOfStream();
	virtual RuStreamErrorCode	Seek(INT offset, RuStreamSeekMethod seekType);

	virtual RuStreamErrorCode	SetStreamSize(DWORD newSize);
	virtual DWORD				GetStreamSize();
	virtual DWORD				GetPosition();

	virtual BYTE*				OpenStreamMapping() { return m_stream; }
	virtual void				CloseStreamMapping() { }

	virtual RuStreamErrorCode	LockStream(INT32 offset, INT32 length, BOOL exclusive) { return ruSEC_OK; }
	virtual RuStreamErrorCode	UnlockStream(INT32 offset, INT32 length) { return ruSEC_OK; }

	virtual void				operator=(IRuStream &operand);
};

#pragma managed

#endif
