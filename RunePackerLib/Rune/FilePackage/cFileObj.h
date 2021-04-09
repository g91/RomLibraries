#ifndef CLASS_FILEOBJ_H
#define CLASS_FILEOBJ_H

#define RUSTREAMIO					1						// TRUE �~��IRuStreamIO 
#define E_EABORTED        21            /* Function aborted by user */
typedef int (WINAPI *tProcessDataProc)(void *data,char *  FileName,int Size,int max);
#include <tchar.h>
#include <vector>

#include "compressFile.h"
#if RUSTREAMIO
  #include "../../Rune/Core/Stream/RuStreamIO.h"
#endif

enum FileType
{
	FILE_TYPE_GENERAL			= 1,
	FILE_TYPE_IMAGE,
	FILE_TYPE_LAST,
	FILE_TYPE_FORCE_DWORD		= 0x7FFFFFFF,
};

using namespace std;

class cListData
{
public:
	cListData() { m_data.clear(); }
	~cListData() { m_data.clear(); }
	
	void Push( void *pSrc, int size )
	{
		BYTE *pTmp = (BYTE*)pSrc;
		for ( int i = 0; i < size; i++ ) m_data.push_back( pTmp[i] );
	}

	BYTE *Begin()
	{
		return &m_data[0];
	}

	int Size()
	{
		return (int)m_data.size();
	}

private:
	vector<BYTE> m_data;
};

#if RUSTREAMIO
  class cFileObj : public IRuStream
#else 
  class cFileObj
#endif
{
public:
	cFileObj();
	virtual ~cFileObj();


#if RUSTREAMIO
	//////////////////////////////////////////////////////////////////////////
	//
	// �~�� IRuStream
	// Required interface methods
	virtual BOOL				IsOpen() { return m_pData != NULL; }
	virtual int					EndOfStream();

	virtual void				Close()	{ Destroy(); }

	virtual DWORD				Read(void *destBuffer, DWORD readSize);
	virtual RuStreamErrorCode	Write(const void *srcBuffer, DWORD writeSize) { return ruSEC_ReadOnly; }
	virtual RuStreamErrorCode	Seek(INT offset, RuStreamSeekMethod seekType);

	virtual RuStreamErrorCode	SetStreamSize(DWORD newSize) { return ruSEC_ReadOnly; }
	virtual DWORD				GetStreamSize() { return GetDataSize(); }
	virtual DWORD				GetPosition() { return m_position; }

	// Stream mapping
	virtual BYTE*				OpenStreamMapping()	{ return GetData(); }
	virtual void				CloseStreamMapping() {;}

	// Operator overrides
	virtual void				operator=(IRuStream &operand) {;}
#endif

	/**
	* ���o�ɮ�����
	*/
	FileType GetType( void ) { return m_fileType; }

	/**
	* ���o�ɮ׮ɶ�
	*/
	FILETIME GetFileTime( void ) { return m_fileTime; }

	/**
	* ���o�ɮ׽s�X�榡
	*/
	CompressFormat GetCompressFormat( void ) { return m_compressFormat; }

	/*
	* ���o�ɮ׭�l��Ƥj�p
	*/
	int GetDataSize( void ) { return m_dataSize; }	

	/**
	* ���o�s�X���Ƥj�p
	*/
	int GetCompressSize( void ) { return m_compressSize; }

	/**
	* ���o�ɮצW��
	*/
	char  *GetFileName( void ) { return m_fileNameStr; }

	/**
	* ���o�ɮ׸��
	*/
	BYTE* GetData( void ) { return m_pData; }

	/**
	* �]�w�ɮצW��
	*/
	void SetFileName(const char *fileNameStr );

	/**
	* ����Ҧ����
	*/
	virtual void Destroy( void );

	/**
	* �x�s�ɮ׸��
	*
	* @param hFile  �ɮ׫���
	*
	* @return �Ǧ^�g�J��Ʈe�q�j�p
	*/
	virtual int SaveData( HANDLE hFile, tProcessDataProc ProcessDataProc,void *data=NULL);

	/**
	* Ū���ɮ׸��
	*
	* @param hFile  �ɮ׫���
	* @param loadDataBool  �O�_���J�ɮ׸��
	*
	* @return ���\�Ǧ^�D�s��  ���ѶǦ^0
	*/
	virtual int LoadFromData( HANDLE hFile, BOOL loadDataBool = true );
	/**
	* �q (.rdt) �ɮפ�Ū�����
	*
	* @param workDirStr  �u�@���|
	* @param fileNameStr  �ɮצW��
	*
	* @return true ���\  false ����
	*/
	virtual bool LoadFromRDTFile( const char *workDirStr, const char *fileNameStr );	
protected:

	/**
	* �x�s���Y���
	*/
	virtual void SaveHeader( cListData &header );

	/**
	* Ū�����Y���
	*/
	virtual bool LoadHeader( HANDLE hFile );


	/**
	* �g�J���
	*/
	virtual void WriteData( HANDLE hFile );

public:	

	static bool CreateFilePath(const char* fullname);
	/**
	* �q�ɮפ�Ū�����
	*
	* @param workDirStr  �u�@���|
	* @param fileNameStr  �ɮצW��
	* @param format  �ɮ����Y�榡
	* @param pSpecialData  �X�R�ΰѼ�
	*
	* @return true ���\  false ����
	*/
	virtual bool LoadFromFile(const char *workDirStr, const char *fileNameStr, CompressFormat format = COMPRESS_FMT_NORMAL, void *pSpecialData = NULL );	

	/**
	* �ഫ���Y�榡
	*
	* @param fileFormat  ���w�s���Y�榡
	*
	* @return true ���\  false ����
	*/
	virtual bool ConvertFormat( CompressFormat fileFormat );


	bool SaveToFile(const char* filename);
//	static tProcessDataProc m_pProc;
protected:

#if RUSTREAMIO
	int				m_position;						// Ū���μg�J���Ц�m
#endif

	FileType		m_fileType;						// �ɮ�����
	FILETIME		m_fileTime;						// �ɮ׳̫�s���ɶ�
	CompressFormat	m_compressFormat;				// ���Y�榡
	int				m_dataSize;						// �ɮ׸�Ƥj�p
	int				m_compressSize;					// ���Y���Ƥj�p
	char 			*m_fileNameStr;					// �ɮצW��
	char 			*m_reladFilename;
	BYTE			*m_pData;						// ��ƫ���
};

#endif //CLASS_FILEOBJ_H