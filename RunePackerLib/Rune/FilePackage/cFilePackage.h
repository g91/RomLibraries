#ifndef CLASS_FILEPACKAGE_H
#define CLASS_FILEPACKAGE_H

#define FILEPACKAGE_MAGIC		0x46444201					// 檔案類型檢查碼

#include "cImgFile.h"
#include <vector>
#include <map>
#include <string>
//#include "wcxhead.h"

using namespace std;
class cFilePackage;
struct FileDbIndex
{
	FileType		m_fileType;					// 檔案類型
	FILETIME		m_fileTime;					// 檔案時間
	HANDLE			m_hFile;					// 檔案指標
	bool			m_isFromDB;					// 來自資料庫
	DWORD				m_offset;					// 該筆資料在檔案內偏移值
	string          m_Sourcename;
	string          m_shortFile;
	//cFilePackage    *m_other;

	HANDLE m_event;

	// 存檔格式
	CompressFormat	m_comFmt;					// 壓縮格式
};
struct FileDataBaseHeader
{	
	FileType	m_fileType;					// 檔案類型

	FILETIME	m_fileTime;					// 檔案時間
	DWORD		m_offset;					// 該筆資料在檔案內偏移值	
};

typedef map<string, FileDbIndex> FileDBList;
class cFilePackage
{
public:
	cFilePackage();
	virtual ~cFilePackage();

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
	* @return true 成功時需檢查cFileObj檔案類型,以及資料是否為壓縮格式  false 失敗
	*/
	bool FileExists(const char* filename);


	/**
	* 取得檔案資料
	*
	* @param filename	檔案名稱
	* @param isRdt		是否為 *.rdt 檔案資料
	*
	* @return true 成功時需檢查cFileObj檔案類型,以及資料是否為壓縮格式  false 失敗
	*/
	cFileObj* GetFileObj(const char *filename, bool isRdt=false);

#if RUSTREAMIO
	/**
	* 取得 IRuStream 界面類別指標,使用完後需 Delete
	*
	* @param keyStr  檔案名稱
	*
	* @return 失敗傳回NULL, 成功傳回類別指標,在使用時需檢查檔案類型是否為圖形檔案
	*/
	IRuStream *GetIRuStream( const char *  keyStr );
#endif

	/**
	* 動態增加檔案至索引表格內
	*
	* @param fileNameStr  檔案名稱
	* @param pSpecialData  載入檔案參數設定
	* @param format  壓縮格式
	*
	* @return true 成功  false 失敗
	*/
	bool AddFile( const char * fileNameStr, void *pSpecialData = NULL, CompressFormat format = COMPRESS_FMT_NORMAL );


	bool AddFileEX( const char * fileNameStr, const char * SourceName , CompressFormat format = COMPRESS_FMT_ZLIB );
	//bool AddFile( cFileObj *Obj );
	void merge(cFilePackage *other);


	bool DelFile(const char * cfileNameStr);
	/**
	* 載入資料庫檔案
	*
	* @param fileNameStr  檔案名稱
	*
	* @return true 成功  false 失敗
	*/
	bool LoadFromFile( const char * fileNameStr );

	void CheckDDSRecudeMips(void );

	/**
	* 寫入資料庫檔案
	*
	* @param fileNameStr  檔案名稱
	*
	* @return 失敗傳回 -1資料庫內無資料 -2開檔失敗  成功傳回總儲存筆數
	*/
	int SaveToFile(const char * fileNameStr , tProcessDataProc ProcessDataProc,void *data=NULL);
	int SaveToFile_C( const char * fileNameStr , tProcessDataProc ProcessDataProc,void *data=NULL);

	/**
	* 設定工作路徑,只有在載入動態檔案時才會用到
	*/
	void SetDirectory(const char * dirStr );

	/**
	* 取得工作路徑
	*/
	const char * GetDirectory() { return m_dirStr; }

	/**
	* 取得TABLE資料
	*/
	FileDBList& GetTable() { return m_table; }
	UINT getSize();
		void Stop()
		{
			m_cancel=true;
		}
		bool isCancel()
		{
			return m_cancel;
		}
	
	/**
	*/
	//FileDbIndex *GetFirstItem();

	/**
	*/
	//FileDbIndex *GetNextItem();

	/**
	* 增加檔案至索引表格內
	*
	* @param pInfo  檔案表格結構指標
	*
	* @return true  成功  false 失敗
	*/
	bool AddToTable(const char * filename, FileDbIndex &fileInfo);
protected:

	

	/**
	*/
	FileDbIndex *Find(const char * filename);

	/**
	* 取得完整檔案名稱
	*/
	const char * GetFullNameStr( const char * fileNameStr );


protected:
	bool m_bRecudeMinps;

	bool m_bfirst;
	bool m_cancel;
	char  m_dirStr[MAX_PATH];							// 工作路徑
	FileDBList		m_table;
	vector<HANDLE>					m_fileHandle;
};

#endif //CLASS_FILEPACKAGE_H




