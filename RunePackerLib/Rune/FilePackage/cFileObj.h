#ifndef CLASS_FILEOBJ_H
#define CLASS_FILEOBJ_H

#define RUSTREAMIO					1						// TRUE 繼承IRuStreamIO 
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
	// 繼承 IRuStream
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
	char  *GetFileName( void ) { return m_fileNameStr; }

	/**
	* 取得檔案資料
	*/
	BYTE* GetData( void ) { return m_pData; }

	/**
	* 設定檔案名稱
	*/
	void SetFileName(const char *fileNameStr );

	/**
	* 釋放所有資料
	*/
	virtual void Destroy( void );

	/**
	* 儲存檔案資料
	*
	* @param hFile  檔案指標
	*
	* @return 傳回寫入資料容量大小
	*/
	virtual int SaveData( HANDLE hFile, tProcessDataProc ProcessDataProc,void *data=NULL);

	/**
	* 讀取檔案資料
	*
	* @param hFile  檔案指標
	* @param loadDataBool  是否載入檔案資料
	*
	* @return 成功傳回非零值  失敗傳回0
	*/
	virtual int LoadFromData( HANDLE hFile, BOOL loadDataBool = true );
	/**
	* 從 (.rdt) 檔案內讀取資料
	*
	* @param workDirStr  工作路徑
	* @param fileNameStr  檔案名稱
	*
	* @return true 成功  false 失敗
	*/
	virtual bool LoadFromRDTFile( const char *workDirStr, const char *fileNameStr );	
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

	static bool CreateFilePath(const char* fullname);
	/**
	* 從檔案內讀取資料
	*
	* @param workDirStr  工作路徑
	* @param fileNameStr  檔案名稱
	* @param format  檔案壓縮格式
	* @param pSpecialData  擴充用參數
	*
	* @return true 成功  false 失敗
	*/
	virtual bool LoadFromFile(const char *workDirStr, const char *fileNameStr, CompressFormat format = COMPRESS_FMT_NORMAL, void *pSpecialData = NULL );	

	/**
	* 轉換壓縮格式
	*
	* @param fileFormat  指定新壓縮格式
	*
	* @return true 成功  false 失敗
	*/
	virtual bool ConvertFormat( CompressFormat fileFormat );


	bool SaveToFile(const char* filename);
//	static tProcessDataProc m_pProc;
protected:

#if RUSTREAMIO
	int				m_position;						// 讀取及寫入指標位置
#endif

	FileType		m_fileType;						// 檔案類型
	FILETIME		m_fileTime;						// 檔案最後存取時間
	CompressFormat	m_compressFormat;				// 壓縮格式
	int				m_dataSize;						// 檔案資料大小
	int				m_compressSize;					// 壓縮後資料大小
	char 			*m_fileNameStr;					// 檔案名稱
	char 			*m_reladFilename;
	BYTE			*m_pData;						// 資料指標
};

#endif //CLASS_FILEOBJ_H