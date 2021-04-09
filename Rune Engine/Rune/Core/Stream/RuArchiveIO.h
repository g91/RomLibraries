#ifndef _RUARCHIVEIO_H_
#define _RUARCHIVEIO_H_

// Standard C Library Includes
#include <memory.h>
#include <stdio.h>
#include <string.h>

// Base Includes
#include "../../Core/Type/RuType_Base.h"
#include "../../Core/Collection/RuCollections.h"
#include "../../Core/Stream/RuStreamIO.h"

#pragma unmanaged

/*
	CRuFileStream �ϥλ���
	-------------------
	{
		CRuFileStream *file;

		// �}���ɮ�
		file = new CRuFileStream();
		if(file->Open("test.png") != ruAEC_OK)
			return;

		// Ū��
		char buffer[80];
		file->Seek(0, ruSSM_Begin);
		file->Read(buffer, 80);

		// ����
		delete file;
	}

	CRuMemoryStream �ϥλ���
	-------------------
	{
		CRuMemoryStream *memStream;

		memStream = new CRuMemoryStream();
		
		// �w�Ƴ]�w�x�s�Ϥj�p
		memStream->SetBufferSize(600);

		// �g�J - ���e�]�w���x�s�Ϭ�600b, �{�b�n�g�J1000b, CRuMemoryStream�N�|�۰ʽվ��x�s�Ϥj�p.
		memStream->Write(buffer, 1000);

		// ����
		delete memStream;
	}

	CRuCompressedArchive �ϥλ���
	--------------------------

	�إ����Y��
	{
		// �إ�CRuCompressedArchive����
		CRuCompressedArchive *archive = new CRuCompressedArchive();

		// �إ����Y��
		archive->Create("archive.cav");

		// �}�ұ��[�J������
		CRuFileStream *resource = new CRuFileStream();
		resource->Open("test.png");

		// �[�J����
		archive->WriteResource("Resource1", resource, FALSE);

		// �����w�[�J������
		delete resource;

		// �������Y��
		archive->Close();
		delete archive;
	}

	Ū�����Y��
	{
		// �إ�CRuCompressedArchive����
		CRuCompressedArchive *archive = new CRuCompressedArchive();

		// �}�����Y��(��2�Ѽ�--�O�_�}�Ұ�Ū�Ҧ�)
		archive->Open("archive.cav", TRUE);

		// Ū������
		IRuStream *resource = archive->ReadResource("Resource1");

		// �����wŪ�J������
		delete resource;

		// Ū������M��
		UINT32 numResources = archive->GetResourceCount();
		char **resList = archive->GetResourceNames();

		for(UINT32 i = 0; i < numResources; i++)
		{
			MessageBox(NULL, resList[i], "", MB_OK);
			delete[] resList[i];
		}
		delete[] resList;

		// �������Y��
		archive->Close();
		delete archive;
	}
*/

enum RuArchiveErrorCode
{
	ruAEC_OK							= 0,
	ruAEC_ReadOnly						= 1,
	ruAEC_SeekError						= 2,
	ruAEC_ReadError						= 3,
	ruAEC_WriteError					= 4,
	ruAEC_CannotOpen					= 5,
	ruAEC_ArchiveNotOpen				= 6,
	ruAEC_Invalid						= 7,
	ruAEC_ResourceNotFound				= 8,
	ruAEC_ResourceAlreadyExists			= 9,
	ruAEC_ResourceInvalid				= 10,
	ruAEC_FORCE_DWORD					= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

// *** IRuArchive
//     Pure abstract base class. Defines interface for all archive classes
class IRuArchive
{
public:
	virtual							~IRuArchive() { };

	// Resource management
	virtual IRuStream*				ReadResource(const char *resourceName) = 0;
	virtual RuArchiveErrorCode		WriteResource(const char *resourceName, IRuStream *resource, BOOL overwrite = FALSE) = 0;
	virtual RuArchiveErrorCode		RenameResource(const char *oldName, const char *newName) = 0;
	virtual RuArchiveErrorCode		DeleteResource(const char *resourceName) = 0;

	// Queries
	virtual UINT32					GetResourceCount(void) = 0;
	virtual char**			 		GetResourceNames(void) = 0;
};

// ************************************************************************************************************************************************************

class CRuDirectoryArchive : public IRuArchive
{
protected:
	char*							m_rootDirectory;

public:
									CRuDirectoryArchive(const char *rootDirectory);
	virtual							~CRuDirectoryArchive();

	// Resource management
	virtual IRuStream*				ReadResource(const char *resourceName);
	virtual RuArchiveErrorCode		WriteResource(const char *resourceName, IRuStream *resource, BOOL overwrite = FALSE);
	virtual RuArchiveErrorCode		RenameResource(const char *oldName, const char *newName);
	virtual RuArchiveErrorCode		DeleteResource(const char *resourceName);

	// Queries
	virtual UINT32					GetResourceCount(void);
	virtual char**			 		GetResourceNames(void);
};

// ************************************************************************************************************************************************************
/*
// *** CRuCompressedArchive
//     Compressed archive class using zlib/deflate.
class CRuCompressedArchive : public IRuArchive
{
private:
	struct SCAHeader
	{
		DWORD m_fileID;					// 4-byte file ID code
		DWORD m_indexOffset;			// Offset to the first data index
		DWORD m_numIndices;				// Number of indices in the data file
	};

	struct SCAIndex
	{
		char *m_name;
		DWORD m_nameLength;
		DWORD m_offset;					// Offset to the first data byte
		DWORD m_chunkSize;				// Length of the data stream
		DWORD m_originalSize;			// Original size of the uncompressed data stream

		SCAIndex(void) : m_name(NULL) { }
		~SCAIndex(void) { delete[] m_name; }

		BOOL SetName(const char *newName)
		{
			if(newName == NULL)
				return FALSE;

			if(strlen(newName) == 0)
				return FALSE;

			delete[] m_name;
			m_name = new char [strlen(newName) + 1];
			m_nameLength = (DWORD) strlen(newName) + 1;
			strcpy(m_name, newName);

			return TRUE;
		}

		void operator=(const SCAIndex &operand)
		{
			delete[] m_name;

			if(operand.m_name != NULL)
			{
				m_name = new char [operand.m_nameLength];
				strcpy(m_name, operand.m_name);
			}
			else
			{
				m_name = NULL;
			}

			m_nameLength = operand.m_nameLength;
			m_offset = operand.m_offset;
			m_chunkSize = operand.m_chunkSize;
			m_originalSize = operand.m_originalSize;
		}

		BOOL operator<(const SCAIndex &operand) const
		{
			if((m_name != NULL) && (operand.m_name != NULL))
			{
				if(strcmp(m_name, operand.m_name) < 0)
					return TRUE;
			}

			return FALSE;
		}

		BOOL operator>(const SCAIndex &operand) const
		{
			if((m_name != NULL) && (operand.m_name != NULL))
			{
				if(strcmp(m_name, operand.m_name) > 0)
					return TRUE;
			}

			return FALSE;
		}

		BOOL operator==(const SCAIndex &operand) const
		{
			if((m_name != NULL) && (operand.m_name != NULL))
			{
				if(strcmp(m_name, operand.m_name) == 0)
					return TRUE;
			}

			return FALSE;
		}
	};

	CRuBinaryTree<SCAIndex, SCAIndex>	m_indexTree;	// In-RAM index search tree
	IRuStream*						m_stream;
	BOOL							m_readOnly;

public:
									CRuCompressedArchive(void) : m_stream(NULL), m_readOnly(FALSE) { }
									~CRuCompressedArchive(void) { Close(); }

	// CRuCompressedArchive-specific
	RuArchiveErrorCode				Create(const char *archiveName);
	RuArchiveErrorCode				Compact(void);

	// IRuArchive implementation
	virtual RuArchiveErrorCode		Open(const char *archiveName, BOOL readOnly = TRUE);
	virtual void					Close(void);
	virtual IRuStream*				ReadResource(const char *resourceName);
	virtual RuArchiveErrorCode		WriteResource(const char *resourceName, IRuStream *resource, BOOL overwrite = FALSE);
	virtual RuArchiveErrorCode		RenameResource(const char *oldName, const char *newName);
	virtual RuArchiveErrorCode		DeleteResource(const char *resourceName);

	virtual UINT32					GetResourceCount(void);
	virtual char**			 		GetResourceNames(void);

private:
	RuArchiveErrorCode				ReadIndex(SCAIndex *index);
	RuArchiveErrorCode				WriteIndex(SCAIndex *index);
};
*/
#pragma managed

#endif
