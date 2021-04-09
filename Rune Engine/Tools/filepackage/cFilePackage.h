#ifndef CLASS_FILEPACKAGE_H
#define CLASS_FILEPACKAGE_H

#define FILEPACKAGE_MAGIC		0x46444201					// �ɮ������ˬd�X

#include "cImgFile.h"
#include <vector>
#include <map>
#include <set>
#include <string>

#define FILEPACKAGE_USE_BOOST_POOLING_STRING 1  // �O�_�ϥ� boost pool �� alloc string (�w�T�{�i�H�[�֮Ĳv)
#define FILEPACKAGE_USE_BOOST_HASH_MAP_TABLE 0  // �O�_�ϥ� hash map �Ӧs�� string (�ثe�w�]�� string hash �Ĳv����, �Ȯ�����)
#define FILEPACKAGE_USE_TST_DICTIONARY 1        // �O�_�ϥ� Ternary Search Tree �Ӧs�� string key mapping (�z�פW�̧֪��@�k)
#define FILEPACKAGE_DEBUG_PRINT_FILE_FIND 0     // �O�_�L�X cFilePackage::Find() ���e.

#if FILEPACKAGE_USE_TST_DICTIONARY
#include "cTSTDictionary.hpp"
#endif

#if FILEPACKAGE_USE_BOOST_POOLING_STRING
    #include <boost/pool/pool_alloc.hpp>
    typedef std::basic_string< char, std::char_traits<char>, boost::fast_pool_allocator<char> > BPString; // boost pooling string.
#endif

struct PackageFileRead;
class cFilePackage;

struct FileDbIndex
{	
	FileDbIndex()			{ memset(this, 0, sizeof(FileDbIndex)); }

	bool				m_isFromDB;					// �Ӧ۸�Ʈw
	FileType			m_fileType;					// �ɮ�����
	FILETIME			m_fileTime;					// �ɮ׮ɶ�
	HANDLE				m_hFile;					// �ɮ׫���
	const char*			m_DBFilename;				// fdb�ɮצW��
	const char*			m_filename;					// �u���ɮצW��(�u�ΦbŪ���Ȧs)
	unsigned int		m_offset;					// �ӵ���Ʀb�ɮפ�������
};

struct FileHandleRef
{
	FileHandleRef() : hFile(NULL), refCount(0)		{}
	
	void	AddRefCount()							{ refCount++; }
	void	DelRefCount()							{ refCount--; }
	int		RefCount()								{ return refCount; }


	HANDLE hFile;									// �Ȧs��HANDLE
	int refCount;									// ��Ȧs�ɰѦҭӼ�
};

struct FileDataBaseHeader
{	
	FileType		m_fileType;					// �ɮ�����
	FILETIME		m_fileTime;					// �ɮ׮ɶ�
	unsigned int	m_offset;					// �ӵ���Ʀb�ɮפ�������	
};

#if FILEPACKAGE_USE_BOOST_HASH_MAP_TABLE
    #include <boost/unordered_map.hpp> 
    //typedef boost::unordered_map<BPString, FileDbIndex,
    //                             boost::hash<BPString>,
    //                             std::equal_to<BPString>,
    //                             std::allocator<std::pair<const BPString, FileDbIndex> > > mapFileIndexSample; // euqal to mapFileIndex
    typedef boost::unordered_map<BPString, FileDbIndex>     mapFileIndex;
    typedef boost::unordered_map<BPString, FileHandleRef>   mapFileHandle;
#else
    #if FILEPACKAGE_USE_TST_DICTIONARY
        typedef TSTDictionary<FileDbIndex>      tstFileIndex;
        typedef TSTDictionary<FileHandleRef>    tstFileHandle;
    #else
        #if FILEPACKAGE_USE_BOOST_POOLING_STRING
            typedef std::map<BPString, FileDbIndex>         mapFileIndex;
            typedef std::map<BPString, FileHandleRef>       mapFileHandle;
        #else
            typedef std::map<std::string, FileDbIndex>      mapFileIndex;
            typedef std::map<std::string, FileHandleRef>    mapFileHandle;
        #endif
    #endif
#endif

struct PackageFileRead : public IFileReadBase
{
public:
	PackageFileRead();
	virtual ~PackageFileRead();

	// �ɮ׬O�_�w�g�}��
	BOOL	IsOpen()							{ return m_hFile != INVALID_HANDLE_VALUE; }

	// �����ɮ�
	void	Close();	

	// Ū���������
	DWORD	Read(DWORD offset, void *destBuffer, DWORD readSize);

public:
	cFilePackage*	m_filePackage;
	HANDLE			m_hFile;
	DWORD			m_offset;
	const char*		m_filename;
	bool			m_tempHandle;
};

class cTempFileData
{
public:
	cTempFileData();
	cTempFileData(int size);
	cTempFileData(char* inData, int inDataSize);
	~cTempFileData();	

	enum FilePosition
	{
		eFilePosition_Begin		= 0,			// �}�l
		eFilePosition_Current	= 1,			// �ثe��m
		eFilePosition_End		= 2,			// ����
	};

	void		Open(int newSize=65535);
	void		Close();
	int			Read(void* outData, int outDataSize);
	int			Write(void* data, int dataSize);
	void		SetData(char* data, int size);
	int			SetPosition(FilePosition position, int offset);
	char*		GetData()							{ return m_data; }	
	int			GetSize()							{ return m_dataSize; }
	int			GetPosition()						{ return m_position; }

private:
	void		Clear();

private:
	bool		m_isNew;
	char*		m_data;
	int			m_dataSize;
	int			m_position;
};

class cFilePackage
{
public:
	cFilePackage();
	virtual ~cFilePackage();

	static bool	TransFilename(const char* instr, char* outstr, int outsize);
	static cTempFileData* NewFileObjMem(cFileObj* pFileObj);
	static cFileObj* TransMemToFileObj(char* inData, int inDataSize);

	/*
	struct lstr
	{
		operator () (const char* p1, const char* p2) const
		{
			return strcmp(p2, p2) < 0;
		}
	};
	*/

	/**
	* ����Ҧ����
	*/
	void Destroy();

	/**
	* �ɮ׬O�_�s�b���
	* @param filename  �ɮצW��
	*

	*/
	bool FileExists(const char* filename);

	/**
	* ���o�ɮ׸��
	*
	* @param filename	�ɮצW��	
	* @param boLoadAllData  �O�_���J������
	*

	*/
	cFileObj* GetFileObj(const char *filename, bool boLoadAllData=true);

	/**
	* ���o�ɮ׸��(�S��γ~)
	*
	* @param pFileIndex  table���ޭ�
	* @param boLoadAllData  �O�_���J������
	*

	*/
	cFileObj* GetFileObj(FileDbIndex& pFileIndex, bool boLoadAllData=true);

#if RUSTREAMIO
	/**
	* ���o IRuStream �ɭ����O����,�ϥΧ���� Delete
	*
	* @param keyStr  �ɮצW��
	* @param boLoadAllData  �O�_���J������
	*

	*/
	bool CheckMd5(const char* keyStr, IRuStream* stream);

	IRuStream *GetIRuStream(const char *keyStr, bool boLoadAllData=true);

	/**
	* ���o CRuTempFileStream �ɭ����O����,�ϥΧ���� Delete
	*
	* @param keyStr  �ɮצW��
	*

	*/
	BOOL GetCRuTempFileStream( const char *keyStr, CRuFileStream& tempFileStream );
#endif

	/**
	* �ʺA�W�[�ɮצܯ��ު�椺
	*
	* @param fileNameStr  �ɮצW��
	* @param pSpecialData  ���J�ɮװѼƳ]�w
	* @param format  ���Y�榡
	*

	*/
	bool AddFile( const char *fileNameStr, void *pSpecialData = NULL, CompressFormat format = COMPRESS_FMT_NORMAL );	

	/**
	/* �R���Y�@������ɮ�	
	*
	* @param fileNameStr  �ɮצW��
	*
	 
	/**/
	bool RemoveFile( const char *fileNameStr );

	/**
	* ���J��Ʈw�ɮ�
	*
	* @param fileNameStr  �ɮצW��
	* @param keepHandle  �O�d�ɮ����Y,�i�H�קK�ɮ׳Q��ʩΪ̴�֭��_�}�ɰʧ@
	*

	*/
	bool LoadFromFile( const char *fileNameStr, bool keepHandle = true);	

	/**
	* �g�J��Ʈw�ɮ�
	*
	* @param fileNameStr  �ɮצW��
	*

	*/
	int SaveToFile( const TCHAR *fileNameStr );

	/**
	* �]�w�u�@���|,�u���b���J�ʺA�ɮ׮ɤ~�|�Ψ�
	*/
	void SetDirectory( const TCHAR *dirStr );

	/**
	* ���o�u�@���|
	*/
	TCHAR *GetDirectory() { return m_dirStr; }
	
	/**
	*/
	//FileDbIndex *GetFirstItem();

	/**
	*/
	//FileDbIndex *GetNextItem();

	/**
	*/
	void CreatePath(const char* filename);


	/**
	*/
#if FILEPACKAGE_USE_TST_DICTIONARY
#else
    mapFileIndex&   GetTable()  { return m_table; }
#endif

	/**
	* �O�_��w��,Ū���ɮפ�
	*/
	bool CheckReadLocked()  { return m_readLocked; }

	/**
	* �W�[�ɮצܯ��ު�椺
	*
	* @param pInfo  �ɮת�浲�c����
	*

	*/
	bool AddToTable(string filename, FileDbIndex &fileInfo);

protected:
friend struct PackageFileRead;
	/**
	*/
	FileDbIndex *Find(string filename);

	/**
	* ���o�����ɮצW��
	*/
	TCHAR *GetFullNameStr( const TCHAR *fileNameStr );

	/**
	* �s�W���|�W��
	*
	* @param pathname  �ɮ׸��|
	*
	* @return �[�J�b�}�C���r��Ҧb����
	*/
	const char* InsertPathname( const char* pathname );

	/**
	* ����Ҧ����ͪ����|�r��
	*/
	void ReleasePathname();	

	void AddFileHandle(const char* filename, HANDLE hFile);
	void RemoveFileHandle(const char* filename);
	FileHandleRef* GetFileHandleRef(const char* filename);

protected:

	TCHAR m_dirStr[MAX_PATH];							// �u�@���|

#if FILEPACKAGE_USE_TST_DICTIONARY
    tstFileIndex                m_table;
    tstFileHandle               m_fileHandle;
#else
    mapFileIndex                m_table;
    mapFileHandle               m_fileHandle;
#endif

	vector<char*>               m_openPathname;		// �}�Ҫ�FDB�ɮ׸��|
    vector<string>              m_tempFile;
	bool                        m_readLocked;
};

#endif //CLASS_FILEPACKAGE_H
