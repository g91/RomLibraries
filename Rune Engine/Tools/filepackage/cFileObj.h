#ifndef CLASS_FILEOBJ_H
#define CLASS_FILEOBJ_H

#ifndef _TOOL
	#define RUSTREAMIO					1						// TRUE �~��IRuStreamIO
#else
	#define RUSTREAMIO					0						// TRUE �~��IRuStreamIO
#endif

#define SAFE_PATH_MAX				1024

#ifndef SAFE_DELETE
	#define SAFE_DELETE(x)				if (x) { delete x; x = NULL; }
#endif

#ifndef SAFE_DELETE_ARRAY
	#define SAFE_DELETE_ARRAY(x)		if (x) { delete [] x; x = NULL; }
#endif

#include <vector>
#include "compressFile.h"
#if RUSTREAMIO
	#include "../../Rune/Core/Stream/RuStreamIO.h"
#else
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
#endif /*RUSTREAMIO*/

class cFilePackage;

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

struct IFileReadBase
{
public:
	// �ɮ׬O�_�w�g�}��
	virtual		BOOL	IsOpen() = 0;	

	// ���������ɮ�
	virtual		void	Close() = 0;

	// Ū���������
	virtual		DWORD	Read(DWORD offset, void *destBuffer, DWORD readSize) = 0;
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

	//////////////////////////////////////////////////////////////////////////
	//
	// �~�� IRuStream
	// Required interface methods
	virtual BOOL				IsOpen();

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

#if RUSTREAMIO

	// Operator overrides
	virtual void				operator=(IRuStream &operand) {;}
#endif

	/**
	* ���o�ɮ�����
	*/
	static bool CreateFilePath(const char* fullname);
	static bool CreateFilePath(const wchar_t* fullname);

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
	TCHAR *GetFileName( void ) { return m_fileNameStr; }

	/**
	* ���o�ɮ׸��
	*/
	BYTE* GetData( void );

	/**
	* �]�w�ɮצW��
	*/
	void SetFileName( const char *fileNameStr );

	/**
	* ����Ҧ����
	*/
	virtual void Destroy( void );

	/**
	* �����ưO������
	*/
	virtual void ReleaseData( bool boOnlyData = false );

	/**
	* �x�s�ɮ׸��
	*
	* @param hFile  �ɮ׫���
	*
	* @return �Ǧ^�g�J��Ʈe�q�j�p
	*/
	virtual int SaveData( HANDLE hFile );

	/**
	* Ū���ɮ׸��
	*
	* @param hFile  �ɮ׫���
	* @param loadDataBool  �O�_���J�ɮ׸��
	*

	*/
	virtual int LoadFromData( HANDLE hFile, BOOL loadDataBool = true );

	/**
	* �N��Ƥϼg�J�ɮ�
	*
	* @param filename �ɮצW��
	*

	**/
	virtual bool SaveToFile(const char* filename);
	virtual bool SaveToFile(const wchar_t* filename);

	virtual void	SetUserData(void* userData, int dataSize, bool autoDelete=false);
	virtual void*	GetUserData()											{ return m_userData; }
	virtual int		GetUserDataSize()										{ return m_userDataSize; }

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

	/**
	* �q�ɮפ�Ū�����
	*
	* @param workDirStr  �u�@���|
	* @param fileNameStr  �ɮצW��
	* @param format  �ɮ����Y�榡
	* @param pSpecialData  �X�R�ΰѼ�
	*

	*/
	virtual bool LoadFromFile( const char *workDirStr, const char *fileNameStr, CompressFormat format = COMPRESS_FMT_NORMAL, void *pSpecialData = NULL );
	virtual bool LoadFromFile( const wchar_t *workDirStr, const wchar_t *fileNameStr, CompressFormat format = COMPRESS_FMT_NORMAL, void *pSpecialData = NULL );

	virtual bool LoadFromHandle(const char *workDirStr, const char *fileNameStr);

	/**
	* �ഫ���Y�榡
	*
	* @param fileFormat  ���w�s���Y�榡
	*

	*/
	virtual bool ConvertFormat( CompressFormat fileFormat );

	/**
	/* �]�w�u���ɮק�����|
	/*/
	virtual void SetRealFilename( const char* realFilename);
	virtual void SetRealFilename( const wchar_t* realFilename);

	/**
	/* �]�w�u���ɮק�����|
	/*/
	virtual void SetFileReadHandle(IFileReadBase* pFileReadBase);

	/**
	/* �}���ɮר��o�ɮ�HANDLE,�è��o�ɮ׸��
	/*/
	virtual HANDLE OpenFile(const char* fullname);
	virtual HANDLE OpenFile(const wchar_t* fullname);

	/**
	* ���o�ɮ׸��
	*
	* @param hFile  File Handle 
	* @param fullNameStrh  �����ɮצW��(�]�t���|��T)
	* @param imageOptions  �ϧΨϥίS�w�榡, �Ѧ� ReadImageOptions
	*

	*/
	virtual bool ReadFileData(HANDLE hFile, const char* fullNameStrh, void* imageOptions=NULL);

protected:
	friend class cFilePackage;
	
	FileType		m_fileType;						// �ɮ�����
	FILETIME		m_fileTime;						// �ɮ׳̫�s���ɶ�
	CompressFormat	m_compressFormat;				// ���Y�榡
	int				m_dataSize;						// �ɮ׸�Ƥj�p
	int				m_compressSize;					// ���Y���Ƥj�p
	char			*m_fileNameStr;					// �ɮצW��(�]���n�`�٤j�p�ثe�٬O�ϥ�char)
	char m_fixFileNameStr[256]; 
	BYTE			*m_pData;						// ��ƫ���
	
	int				m_position;						// Ū���μg�J���Ц�m

	IFileReadBase*	m_fileRead;						// Ū���ɮ׵��c

	// �o�O���F Redux �ഫ�ӼȮɦs�b��T, ���e�p�G��MEMORY TO MEMORY�h�i�H�R�����Ѽ�
	char*			m_realFilename;					// �u���ɮשҦb��m	

	void*			m_userData;						// �۩w�q���
	int				m_userDataSize;					// �۩w�q��Ƥj�p
	bool			m_userDataAutoRelease;			// ���c�R���ɤ@�_���� userdata
};

#endif //CLASS_FILEOBJ_H