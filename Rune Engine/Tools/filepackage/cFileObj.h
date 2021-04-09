#ifndef CLASS_FILEOBJ_H
#define CLASS_FILEOBJ_H

#ifndef _TOOL
	#define RUSTREAMIO					1						// TRUE 繼承IRuStreamIO
#else
	#define RUSTREAMIO					0						// TRUE 繼承IRuStreamIO
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
	// 檔案是否已經開啟
	virtual		BOOL	IsOpen() = 0;	

	// 關閉釋放檔案
	virtual		void	Close() = 0;

	// 讀取部份資料
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
	// 繼承 IRuStream
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
	* 取得檔案類型
	*/
	static bool CreateFilePath(const char* fullname);
	static bool CreateFilePath(const wchar_t* fullname);

	/**
	* 取得檔案類型
	*/
	FileType GetType( void ) { return m_fileType; }

	/**
	* 取得檔案時間
	*/
	FILETIME GetFileTime( void ) { return m_fileTime; }

	/**
	* 取得檔案編碼格式
	*/
	CompressFormat GetCompressFormat( void ) { return m_compressFormat; }

	/*
	* 取得檔案原始資料大小
	*/
	int GetDataSize( void ) { return m_dataSize; }	

	/**
	* 取得編碼後資料大小
	*/
	int GetCompressSize( void ) { return m_compressSize; }

	/**
	* 取得檔案名稱
	*/
	TCHAR *GetFileName( void ) { return m_fileNameStr; }

	/**
	* 取得檔案資料
	*/
	BYTE* GetData( void );

	/**
	* 設定檔案名稱
	*/
	void SetFileName( const char *fileNameStr );

	/**
	* 釋放所有資料
	*/
	virtual void Destroy( void );

	/**
	* 釋放資料記憶體資料
	*/
	virtual void ReleaseData( bool boOnlyData = false );

	/**
	* 儲存檔案資料
	*
	* @param hFile  檔案指標
	*
	* @return 傳回寫入資料容量大小
	*/
	virtual int SaveData( HANDLE hFile );

	/**
	* 讀取檔案資料
	*
	* @param hFile  檔案指標
	* @param loadDataBool  是否載入檔案資料
	*

	*/
	virtual int LoadFromData( HANDLE hFile, BOOL loadDataBool = true );

	/**
	* 將資料反寫入檔案
	*
	* @param filename 檔案名稱
	*

	**/
	virtual bool SaveToFile(const char* filename);
	virtual bool SaveToFile(const wchar_t* filename);

	virtual void	SetUserData(void* userData, int dataSize, bool autoDelete=false);
	virtual void*	GetUserData()											{ return m_userData; }
	virtual int		GetUserDataSize()										{ return m_userDataSize; }

protected:

	/**
	* 儲存檔頭資料
	*/
	virtual void SaveHeader( cListData &header );

	/**
	* 讀取檔頭資料
	*/
	virtual bool LoadHeader( HANDLE hFile );

	/**
	* 寫入資料
	*/
	virtual void WriteData( HANDLE hFile );

public:	

	/**
	* 從檔案內讀取資料
	*
	* @param workDirStr  工作路徑
	* @param fileNameStr  檔案名稱
	* @param format  檔案壓縮格式
	* @param pSpecialData  擴充用參數
	*

	*/
	virtual bool LoadFromFile( const char *workDirStr, const char *fileNameStr, CompressFormat format = COMPRESS_FMT_NORMAL, void *pSpecialData = NULL );
	virtual bool LoadFromFile( const wchar_t *workDirStr, const wchar_t *fileNameStr, CompressFormat format = COMPRESS_FMT_NORMAL, void *pSpecialData = NULL );

	virtual bool LoadFromHandle(const char *workDirStr, const char *fileNameStr);

	/**
	* 轉換壓縮格式
	*
	* @param fileFormat  指定新壓縮格式
	*

	*/
	virtual bool ConvertFormat( CompressFormat fileFormat );

	/**
	/* 設定真實檔案完整路徑
	/*/
	virtual void SetRealFilename( const char* realFilename);
	virtual void SetRealFilename( const wchar_t* realFilename);

	/**
	/* 設定真實檔案完整路徑
	/*/
	virtual void SetFileReadHandle(IFileReadBase* pFileReadBase);

	/**
	/* 開啟檔案取得檔案HANDLE,並取得檔案資料
	/*/
	virtual HANDLE OpenFile(const char* fullname);
	virtual HANDLE OpenFile(const wchar_t* fullname);

	/**
	* 取得檔案資料
	*
	* @param hFile  File Handle 
	* @param fullNameStrh  完整檔案名稱(包含路徑資訊)
	* @param imageOptions  圖形使用特定格式, 參考 ReadImageOptions
	*

	*/
	virtual bool ReadFileData(HANDLE hFile, const char* fullNameStrh, void* imageOptions=NULL);

protected:
	friend class cFilePackage;
	
	FileType		m_fileType;						// 檔案類型
	FILETIME		m_fileTime;						// 檔案最後存取時間
	CompressFormat	m_compressFormat;				// 壓縮格式
	int				m_dataSize;						// 檔案資料大小
	int				m_compressSize;					// 壓縮後資料大小
	char			*m_fileNameStr;					// 檔案名稱(因為要節省大小目前還是使用char)
	char m_fixFileNameStr[256]; 
	BYTE			*m_pData;						// 資料指標
	
	int				m_position;						// 讀取及寫入指標位置

	IFileReadBase*	m_fileRead;						// 讀取檔案結構

	// 這是為了 Redux 轉換而暫時存在資訊, 之前如果由MEMORY TO MEMORY則可以刪除此參數
	char*			m_realFilename;					// 真實檔案所在位置	

	void*			m_userData;						// 自定義資料
	int				m_userDataSize;					// 自定義資料大小
	bool			m_userDataAutoRelease;			// 當結構刪除時一起釋放 userdata
};

#endif //CLASS_FILEOBJ_H