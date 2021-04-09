#ifndef CLASS_FILEPACKAGE_H
#define CLASS_FILEPACKAGE_H

#define FILEPACKAGE_MAGIC		0x46444201					// 檔案類型檢查碼

#include "cImgFile.h"
#include <vector>
#include <map>
#include <set>
#include <string>

#define FILEPACKAGE_USE_BOOST_POOLING_STRING 1  // 是否使用 boost pool 來 alloc string (已確認可以加快效率)
#define FILEPACKAGE_USE_BOOST_HASH_MAP_TABLE 0  // 是否使用 hash map 來存放 string (目前預設的 string hash 效率不佳, 暫時關閉)
#define FILEPACKAGE_USE_TST_DICTIONARY 1        // 是否使用 Ternary Search Tree 來存放 string key mapping (理論上最快的作法)
#define FILEPACKAGE_DEBUG_PRINT_FILE_FIND 0     // 是否印出 cFilePackage::Find() 內容.

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

	bool				m_isFromDB;					// 來自資料庫
	FileType			m_fileType;					// 檔案類型
	FILETIME			m_fileTime;					// 檔案時間
	HANDLE				m_hFile;					// 檔案指標
	const char*			m_DBFilename;				// fdb檔案名稱
	const char*			m_filename;					// 真實檔案名稱(只用在讀取暫存)
	unsigned int		m_offset;					// 該筆資料在檔案內偏移值
};

struct FileHandleRef
{
	FileHandleRef() : hFile(NULL), refCount(0)		{}
	
	void	AddRefCount()							{ refCount++; }
	void	DelRefCount()							{ refCount--; }
	int		RefCount()								{ return refCount; }


	HANDLE hFile;									// 暫存檔HANDLE
	int refCount;									// 改暫存檔參考個數
};

struct FileDataBaseHeader
{	
	FileType		m_fileType;					// 檔案類型
	FILETIME		m_fileTime;					// 檔案時間
	unsigned int	m_offset;					// 該筆資料在檔案內偏移值	
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

	// 檔案是否已經開啟
	BOOL	IsOpen()							{ return m_hFile != INVALID_HANDLE_VALUE; }

	// 關閉檔案
	void	Close();	

	// 讀取部份資料
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
		eFilePosition_Begin		= 0,			// 開始
		eFilePosition_Current	= 1,			// 目前位置
		eFilePosition_End		= 2,			// 結尾
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
	* 釋放所有資料
	*/
	void Destroy();

	/**
	* 檔案是否存在資料
	* @param filename  檔案名稱
	*

	*/
	bool FileExists(const char* filename);

	/**
	* 取得檔案資料
	*
	* @param filename	檔案名稱	
	* @param boLoadAllData  是否載入完整資料
	*

	*/
	cFileObj* GetFileObj(const char *filename, bool boLoadAllData=true);

	/**
	* 取得檔案資料(特殊用途)
	*
	* @param pFileIndex  table索引值
	* @param boLoadAllData  是否載入完整資料
	*

	*/
	cFileObj* GetFileObj(FileDbIndex& pFileIndex, bool boLoadAllData=true);

#if RUSTREAMIO
	/**
	* 取得 IRuStream 界面類別指標,使用完後需 Delete
	*
	* @param keyStr  檔案名稱
	* @param boLoadAllData  是否載入完整資料
	*

	*/
	bool CheckMd5(const char* keyStr, IRuStream* stream);

	IRuStream *GetIRuStream(const char *keyStr, bool boLoadAllData=true);

	/**
	* 取得 CRuTempFileStream 界面類別指標,使用完後需 Delete
	*
	* @param keyStr  檔案名稱
	*

	*/
	BOOL GetCRuTempFileStream( const char *keyStr, CRuFileStream& tempFileStream );
#endif

	/**
	* 動態增加檔案至索引表格內
	*
	* @param fileNameStr  檔案名稱
	* @param pSpecialData  載入檔案參數設定
	* @param format  壓縮格式
	*

	*/
	bool AddFile( const char *fileNameStr, void *pSpecialData = NULL, CompressFormat format = COMPRESS_FMT_NORMAL );	

	/**
	/* 刪除某一筆資料檔案	
	*
	* @param fileNameStr  檔案名稱
	*
	 
	/**/
	bool RemoveFile( const char *fileNameStr );

	/**
	* 載入資料庫檔案
	*
	* @param fileNameStr  檔案名稱
	* @param keepHandle  保留檔案檔頭,可以避免檔案被更動或者減少重復開檔動作
	*

	*/
	bool LoadFromFile( const char *fileNameStr, bool keepHandle = true);	

	/**
	* 寫入資料庫檔案
	*
	* @param fileNameStr  檔案名稱
	*

	*/
	int SaveToFile( const TCHAR *fileNameStr );

	/**
	* 設定工作路徑,只有在載入動態檔案時才會用到
	*/
	void SetDirectory( const TCHAR *dirStr );

	/**
	* 取得工作路徑
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
	* 是否鎖定中,讀取檔案中
	*/
	bool CheckReadLocked()  { return m_readLocked; }

	/**
	* 增加檔案至索引表格內
	*
	* @param pInfo  檔案表格結構指標
	*

	*/
	bool AddToTable(string filename, FileDbIndex &fileInfo);

protected:
friend struct PackageFileRead;
	/**
	*/
	FileDbIndex *Find(string filename);

	/**
	* 取得完整檔案名稱
	*/
	TCHAR *GetFullNameStr( const TCHAR *fileNameStr );

	/**
	* 新增路徑名稱
	*
	* @param pathname  檔案路徑
	*
	* @return 加入在陣列內字串所在指標
	*/
	const char* InsertPathname( const char* pathname );

	/**
	* 釋放所有產生的路徑字串
	*/
	void ReleasePathname();	

	void AddFileHandle(const char* filename, HANDLE hFile);
	void RemoveFileHandle(const char* filename);
	FileHandleRef* GetFileHandleRef(const char* filename);

protected:

	TCHAR m_dirStr[MAX_PATH];							// 工作路徑

#if FILEPACKAGE_USE_TST_DICTIONARY
    tstFileIndex                m_table;
    tstFileHandle               m_fileHandle;
#else
    mapFileIndex                m_table;
    mapFileHandle               m_fileHandle;
#endif

	vector<char*>               m_openPathname;		// 開啟的FDB檔案路徑
    vector<string>              m_tempFile;
	bool                        m_readLocked;
};

#endif //CLASS_FILEPACKAGE_H
