/******************************************************************************
******************************************************************************/


#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <Shlwapi.h>
#include "cFilePackage.h"
#include "..\..\MinG_Debug.h"

cFilePackage::cFilePackage():m_cancel(false),m_bRecudeMinps(false),m_bfirst(true)
{	
	m_table.clear();
	m_fileHandle.clear();	
	//m_currentIter = m_table.end();
	memset( m_dirStr, 0, sizeof(m_dirStr) );
}

cFilePackage::~cFilePackage()
{
	Destroy();
}
void cFilePackage::merge(cFilePackage *other)
{

	map<string, FileDbIndex>& table=other->GetTable();
	map<string, FileDbIndex>::iterator it=table.begin();

	for(;it!=table.end();++it)
	{
		AddToTable(it->first.c_str(),it->second);


	}



}
void cFilePackage::Destroy()
{
	/*
	for ( set<FileDbIndex*, FileDbIndex>::iterator iter = m_table.begin(); iter != m_table.end(); iter++ ) {
		if ( (*iter)->fileNameStr ) {
			delete [] (*iter)->fileNameStr;
			(*iter)->fileNameStr = NULL;
		}
	}
	*/

	for ( vector<HANDLE>::iterator iter = m_fileHandle.begin(); iter != m_fileHandle.end(); iter++ ) {
		CloseHandle( *iter );
	}

	m_table.clear();
	m_fileHandle.clear();	
	//m_currentIter = m_table.end();
	memset( m_dirStr, 0, sizeof(m_dirStr) );
}

bool cFilePackage::FileExists(const char * filename)
{
	
	if ( Find(filename) ) {
		return true;
	} else {
	const	char * pathFilename = GetFullNameStr(filename);
		if ( ::PathFileExistsA(pathFilename) )
			return true;
	}
	return false;
}

cFileObj* cFilePackage::GetFileObj(const char* filename, bool isRdt)
{
	cFileObj *pFileObj = NULL;
	cImgFile *imgObj=NULL;
	bool DDS=false;
	bool file=false;
	if ( filename )
	{
		int start = 0;
		while (filename[start] && (filename[start] == 92 || filename[start] == 47))
			start++;

		FileDbIndex* pFileIndex = Find(filename + start);
		if ( pFileIndex )
		{
			switch ( pFileIndex->m_fileType )
			{
			case FILE_TYPE_GENERAL:
				pFileObj = new cFileObj();
				break;
			case FILE_TYPE_IMAGE:
				 
				imgObj= new cImgFile();
				pFileObj=imgObj;
				break;
			default:
				return NULL;
			}

			


			bool ret = true;
			if ( pFileIndex->m_isFromDB )
			{
				SetFilePointer(pFileIndex->m_hFile,pFileIndex->m_offset,0,FILE_BEGIN);
				if ( !pFileObj->LoadFromData(pFileIndex->m_hFile) )
					ret = false;
				
			}
			else
			{
				
				
				if ( isRdt )
				{
				

					ret = pFileObj->LoadFromRDTFile(m_dirStr, filename);
				}
				else
				{
				// GKy 100308 modify by a-min
//				if ( !pFileObj->LoadFromFile(m_dirStr,pFileIndex->m_Sourcename.c_str() ,pFileIndex->m_comFmt) ) // origin
				if ( !pFileObj->LoadFromFile(""/*m_dirStr*/,pFileIndex->m_Sourcename.c_str() ,pFileIndex->m_comFmt) )
					ret = false;

				pFileObj->SetFileName(filename);
				}

				file=true;
			}

			if ( !ret )
			{
				delete pFileObj;
				pFileObj = NULL;
			}
			else
			{
				if (imgObj&&m_bRecudeMinps)
				{
					ImageFormat fmt=imgObj->GetImageFormat();
					switch (fmt)
					{
					case IMAGE_DXT1:
					case IMAGE_DXT1a:
					case IMAGE_DXT3:
					case IMAGE_DXT5:
						DDS=true;
						break;
					}
					if (DDS&&pFileIndex->m_hFile!=m_fileHandle[0]|| file)
					{
						if (imgObj->GetReduceMipmapLevels(0)==0)
						{
							imgObj->ReduceMinmapLevel(1);
							imgObj->ConvertFormat(COMPRESS_FMT_ZLIB);

						}
					}
				}
			}
		}
	}


	
	return pFileObj;
}
class RdtFile
{
public:

RdtFile	(const char *name,BOOL rdt,std::vector<std::string>& files);
~RdtFile();
BOOL m_bRDT;
char RDTName[2048];

};
struct preCompressData
{
	int index;
	DWORD m_dwThreadId;
	FileDbIndex* m_dbIndex;
	preCompressData()
	{
		index=-1;
		m_dbIndex=0;
	}
	

};
DWORD WINAPI CallDDS2RDT(LPVOID pParam)
{
	preCompressData* data=(preCompressData*)pParam;


	HANDLE Event=CreateEvent(NULL,
		TRUE,
		FALSE,
		NULL);



	data->m_dbIndex->m_event=Event;
	const char *name=data->m_dbIndex->m_Sourcename.c_str();

	char Parameters[4096];
	char RDTName[2048];
	char pathName[2048];
	int len=strlen(name);
	strcpy(pathName,name);


	for (int i=len-1;i>=0;i--)
	{
		if (pathName[i]=='.')
		{
			pathName[i]=0;
			break;
		}
	}

	strcpy(RDTName,pathName);
	strcat(RDTName,".rdt");

	sprintf(Parameters,"compress \"%s.dds\" \"%s\"",pathName,RDTName);

	//reduxcommandline.exe compress d:\temp\dummy.dds d:\temp\compressed.rdt

	SHELLEXECUTEINFOA ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = "open";
	ShExecInfo.lpFile = "reduxcommandline.exe";		
	ShExecInfo.lpParameters =Parameters;// _T("patch");	
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;	
	ShellExecuteExA(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);

	SetEvent(Event);
	return 0;
}

RdtFile::RdtFile	(const char *name,BOOL rdt,std::vector<std::string>& files):m_bRDT(rdt)
{

	if (rdt)
	{
		int len=strlen(name);
		strcpy(RDTName,name);


		for (int i=len-1;i>=0;i--)
		{
			if (RDTName[i]=='.')
			{
				RDTName[i]=0;
				break;
			}
		}
	
		strcat(RDTName,".rdt");

		files.push_back(RDTName);

	}
	

}
RdtFile::~RdtFile()
{
//	if (m_bRDT)
//DeleteFileA(RDTName);
}


IRuStream *cFilePackage::GetIRuStream(const char* keyStr)
{
	cFileObj *pFileObj;

	if ( keyStr == NULL )
		return NULL;

	// 試著開啟真實路徑下檔案
	if ( IsImageFile( keyStr ) )
	{
		pFileObj = new cImgFile();
		if ( pFileObj->LoadFromFile(m_dirStr, keyStr) )
			return pFileObj;
        delete pFileObj;
	}
	else
	{
		pFileObj = new cFileObj();
		if ( pFileObj->LoadFromFile(m_dirStr, keyStr) )
			return pFileObj;
		delete pFileObj;
	}

	if ( (pFileObj = GetFileObj(keyStr)) != NULL )
	{
		if ( !pFileObj->ConvertFormat( COMPRESS_FMT_NORMAL ) )
		{
			delete pFileObj;
			pFileObj = NULL;
		}
	}

	return pFileObj;
}

bool cFilePackage::AddFileEX( const char * fileNameStr, const char * SourceName , CompressFormat format )
{
	if (fileNameStr == NULL) 
		return false;

	HANDLE hFile;
	if ( (hFile = CreateFileA( SourceName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL ) ) == INVALID_HANDLE_VALUE ) {
		return false;
	}

	FileDbIndex fileInfo;
	fileInfo.m_isFromDB		= false;
	fileInfo.m_hFile		= NULL;
	fileInfo.m_Sourcename=SourceName;
	fileInfo.m_comFmt=format;
	GetFileTime( hFile, NULL, NULL, &fileInfo.m_fileTime );
	CloseHandle( hFile );


	if ( IsImageFile( SourceName ) )
		fileInfo.m_fileType = FILE_TYPE_IMAGE;
	else
		fileInfo.m_fileType = FILE_TYPE_GENERAL;
	//WideCharToMultiByte(CP_ACP, 0, fileNameStr, -1, csmbFullName, 1023, NULL, NULL);

	return AddToTable(fileNameStr, fileInfo);


}

bool cFilePackage::AddFile(const char * fileNameStr, void *pSpecialData, CompressFormat format)
{
	if (fileNameStr == NULL) 
		return false;
	
	
	const char *  FullName=GetFullNameStr(fileNameStr);
	HANDLE hFile;
	if ( (hFile = CreateFileA( GetFullNameStr(fileNameStr) , GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL ) ) == INVALID_HANDLE_VALUE ) {
		return false;
	}

	//char csmbFullName[1024];

	//WideCharToMultiByte(CP_ACP, 0, FullName, -1, csmbFullName, 1023, NULL, NULL);

	FileDbIndex fileInfo;
	fileInfo.m_isFromDB		= false;
	fileInfo.m_hFile		= NULL;
	fileInfo.m_Sourcename=FullName;
	fileInfo.m_comFmt=format;
	GetFileTime( hFile, NULL, NULL, &fileInfo.m_fileTime );
	CloseHandle( hFile );

	
	if ( IsImageFile( fileNameStr ) )
		fileInfo.m_fileType = FILE_TYPE_IMAGE;
	else
		fileInfo.m_fileType = FILE_TYPE_GENERAL;
	//WideCharToMultiByte(CP_ACP, 0, fileNameStr, -1, csmbFullName, 1023, NULL, NULL);
	//	OutputDebugStringA(fileNameStr);
	return AddToTable(fileNameStr, fileInfo);
}

void cFilePackage::CheckDDSRecudeMips(void )
{
	
	if (m_bfirst)
	{
		m_bfirst=false;
		if (m_fileHandle.size()<=0)
			return ;
		
		map<string, FileDbIndex>::iterator iter = m_table.begin();
		for (;iter != m_table.end(); iter++ )
		{
			FileDbIndex &tab=iter->second;
			if (tab.m_isFromDB&&tab.m_hFile==m_fileHandle[0])
			{

				if (tab.m_fileType==FILE_TYPE_IMAGE)
				{

				 	cImgFile* Object=(cImgFile*) GetFileObj(iter->first.c_str());
					if (!Object)
						continue;

					ImageFormat fmt=Object->GetImageFormat();

					bool DDS=false;
					switch (fmt)
					{
					case IMAGE_DXT1:
					case IMAGE_DXT1a:
					case IMAGE_DXT3:
					case IMAGE_DXT5:
						DDS=true;
						break;
					}
					if (DDS)
					{
						m_bRecudeMinps=Object->GetReduceMipmapLevels(0)>0;

						delete Object;
						return ;
					}
					delete Object;
					
				}

			}
			
		}


		
	}

}

// ----------------------------------------------------------------------------
// 讀取檔頭資料
bool cFilePackage::LoadFromFile(const char * fileNameStr )
{
	HANDLE hFile = CreateFileA( fileNameStr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if ( hFile == INVALID_HANDLE_VALUE ) return false;

	DWORD magic, readBytesOut;
	ReadFile( hFile, &magic, sizeof(DWORD), &readBytesOut, NULL );
	if ( magic != FILEPACKAGE_MAGIC ) {
		CloseHandle( hFile );
		return false;
	}

	// 讀取總筆數
	INT count;
	ReadFile( hFile, &count, sizeof(DWORD), &readBytesOut, NULL );
	if (readBytesOut!=sizeof(DWORD))
		return false;

	// 讀入TABLE檔頭
	FileDataBaseHeader* header = new FileDataBaseHeader[count];
	ReadFile( hFile, header, sizeof(FileDataBaseHeader) * count, &readBytesOut, NULL );

	if (readBytesOut!=sizeof(FileDataBaseHeader) * count)
	{
		delete header;
		return false;
	}
	// 字串長度陣列
	INT* textLength = new INT[count];
	ReadFile( hFile, textLength, sizeof(INT) * count, &readBytesOut, NULL );
	if (readBytesOut!=sizeof(INT) * count)
	{

		delete []textLength;
		delete []header;
		return false;
	}
	// 字串內容大小
	INT textSize;
	ReadFile( hFile, &textSize, sizeof(INT), &readBytesOut, NULL );

	if (readBytesOut!=sizeof(INT))
		return false;
	// 字串內容
	char* text = new char[textSize];
	ReadFile( hFile, text, sizeof(char) * textSize, &readBytesOut, NULL );
	if (readBytesOut!=sizeof(char) * textSize)
	{
		delete []textLength;
		delete []header;
		delete []text;
		return false;

	}
	// 寫入表格內
	FileDbIndex info;
	//TCHAR filename[256];

	int pos = 0;
	for ( int i = 0; i < count; i++ ) 
	{
		if (header[i].m_offset!=0)
		{
			CompressFormat fmt;


			info.m_hFile		= hFile;
			info.m_isFromDB		= true;
			info.m_fileType		= header[i].m_fileType;
			info.m_fileTime		= header[i].m_fileTime;
			info.m_offset		= header[i].m_offset;
			SetFilePointer(hFile,header[i].m_offset+8,0,FILE_BEGIN);
			ReadFile( hFile, &fmt, sizeof(fmt) , &readBytesOut, NULL );
			info.m_comFmt=fmt;


			//MultiByteToWideChar(::GetACP(), 0,text + pos,textLength[i],filename,255);

			///AddToTable(filename, info);

			AddToTable(text + pos, info);
		}
		else
			DPF(0,_T(" NULL %s"),text + pos);

		
		pos += textLength[i]+1;
	}

	delete [] text;
	delete [] textLength;
	delete [] header;

	m_fileHandle.push_back( hFile );
	return true;
}
bool  cFilePackage::DelFile(const char * cfileNameStr)
{
	bool ret=false;

	FileDBList::iterator it=m_table.find(cfileNameStr);
	if (it!=m_table.end())
	{
		m_table.erase(it);
		ret=true;

	}
	return ret;
}
UINT cFilePackage::getSize()
{

	UINT size=0;
	if (m_fileHandle.size()>0)
		size=GetFileSize(m_fileHandle[0],0);
	return  size;

}
#define EXE_MAX_COUNT 3 
DWORD WINAPI preCompressRDT (void *datax)
{
	
	cFilePackage *pak=(cFilePackage *)datax;
	FileDBList&table= pak->GetTable();
	FileDBList::iterator iter = table.begin();
	char  filex[2048];


	preCompressData*  data=new preCompressData[EXE_MAX_COUNT];
HANDLE pList[EXE_MAX_COUNT]={0};
int count=0;
	for (; iter != table.end(); iter++ )
	{
		if (pak->isCancel())
			return 0;
		string filename=(iter->second).m_Sourcename;

		strcpy(filex,filename.c_str());
		bool bRDT=iter->second.m_comFmt==COMPRESS_FMT_RDT;
		const char *name=(iter->first).c_str();

		
		if (bRDT)
		{	
			bool wait=true;
			do 
			{
				if (pak->isCancel())
					return 0;
				int i=0;
				for (;i<EXE_MAX_COUNT;i++)
				{
					if (pList[i]==NULL)
						break;
				}
				if(i<EXE_MAX_COUNT)
				{
					data[i].index=i;
					data[i].m_dbIndex=&iter->second;
					pList[i]=CreateThread(NULL,
						0,
						&CallDDS2RDT,
						&data[i],
						0,
						&data[i].m_dwThreadId);
					count++;

					wait=false;

				}
				else
				{
					DWORD dwRet =	WaitForMultipleObjects(EXE_MAX_COUNT,pList,false,INFINITE);

					 DWORD nIndex = dwRet - WAIT_OBJECT_0;	
					 if (nIndex<EXE_MAX_COUNT)
					 {
						 count--;
						 CloseHandle(pList[nIndex]);
						 pList[nIndex]=NULL;
					 }
				}
					
			} while(wait);
		}
	}

	do 
	{
		if (pak->isCancel())
			return 0;
		DWORD dwRet =	WaitForMultipleObjects(count,pList,false,INFINITE);

		DWORD nIndex = dwRet - WAIT_OBJECT_0;	
		if (nIndex<count)
		{
			count--;
			CloseHandle(pList[nIndex]);
				pList[nIndex]=NULL;
			for (int i=nIndex;i<(count);i++)
			{

				pList[i]=pList[i+1];
			}
		
		}

	} while(count);

	



	delete []data;
	return 0;


}
void DeleteAllFile(std::vector<std::string>& files)
{

	int fileCount=files.size();
	for (int i=0;i<fileCount;i++)
	{
		DeleteFileA(files[i].c_str());
	}

}

int cFilePackage::SaveToFile_C( const char * fileNameStr , tProcessDataProc ProcessDataProc,void *data)
{
	if ( m_table.empty() ) return -1;
	DeleteFileA(fileNameStr);
	CheckDDSRecudeMips();


	HANDLE hFile = CreateFileA( fileNameStr, GENERIC_WRITE,FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS, NULL );

	DWORD offset=0;

	if ( hFile == INVALID_HANDLE_VALUE ) return -2;

	DWORD writeBytesOut;
	DWORD magic = FILEPACKAGE_MAGIC;
	WriteFile( hFile, &magic, sizeof(DWORD), &writeBytesOut, NULL );		// 寫入檔案檢查碼
	offset+=writeBytesOut;
	int count=m_table.size();

	WriteFile( hFile, &count, sizeof(DWORD), &writeBytesOut, NULL );		// 寫入檔案數量
	offset+=writeBytesOut;
	FileDataBaseHeader* tempFileDataBaseHeader=new FileDataBaseHeader[count];

	memset( tempFileDataBaseHeader, 0, sizeof(FileDataBaseHeader)*count ); // GKy 091118 initial value

	DWORD _offset=8;
	map<string, FileDbIndex>::iterator iter = m_table.begin();
	for (int i=0 ; iter != m_table.end(); iter++ ,i++)
	{


		FileDbIndex &tab=iter->second;
		tempFileDataBaseHeader[i].m_fileTime=tab.m_fileTime;
		tempFileDataBaseHeader[i].m_fileType=tab.m_fileType;
		tab.m_event=NULL;

		WriteFile( hFile, &tempFileDataBaseHeader[i], sizeof(FileDataBaseHeader), &writeBytesOut, NULL );		// 寫入檔案表頭

		offset+=writeBytesOut;
	}

	int len=0;
	int StringBuffSize=0;
	iter = m_table.begin();
	for ( ; iter != m_table.end(); iter++ )
	{



		len=strlen((iter->first).c_str());


		WriteFile( hFile, &len, sizeof(int), &writeBytesOut, NULL );		//寫入 string len
		offset+=writeBytesOut;

		StringBuffSize+=len+1;

	}

	WriteFile( hFile, &StringBuffSize, sizeof(int), &writeBytesOut, NULL );		//寫入 string List size
	offset+=writeBytesOut;
	//char tempName[2048];

	iter = m_table.begin();
	for (  ; iter != m_table.end(); iter++ )
	{

		const	char *fileName=(iter->first).c_str();
		len=strlen(fileName);
		
		WriteFile( hFile, fileName, len+1, &writeBytesOut, NULL );		//寫入 string data
		offset+=writeBytesOut;

	}
	

	

	count = 0;	
	char  filex[2048];
	iter = m_table.begin();
	for (int i=0 ; iter != m_table.end(); iter++,i++ )
	{
		FileDbIndex &tab=iter->second;
		string filename=tab.m_Sourcename;

		strcpy(filex,filename.c_str());
		bool bRDT=iter->second.m_comFmt==COMPRESS_FMT_RDT;
		const char *name=(iter->first).c_str();
		if (m_cancel)
			break ;

	


		

		cFileObj *pFileObj = GetFileObj(name,bRDT);

		


		if ( pFileObj )
		{ 
			if (0)//(bRDT)
			{

				if(pFileObj->ConvertFormat(COMPRESS_FMT_NORMAL))
					pFileObj->ConvertFormat(COMPRESS_FMT_ZLIB);

				



			}

			tempFileDataBaseHeader[i].m_offset=offset;

			int size=pFileObj->GetDataSize();

			size=pFileObj->SaveData( hFile,ProcessDataProc,data);

			offset+=size;
			count++;
			delete pFileObj;
			pFileObj = NULL;
		}
		
	}


	
	const int zero = 0;
	WriteFile( hFile, &zero, sizeof(int), &writeBytesOut, NULL );			// 區塊結束
	WriteFile( hFile, &count, sizeof(int), &writeBytesOut, NULL );

	SetFilePointer(hFile,_offset,0,FILE_BEGIN);
	WriteFile( hFile, tempFileDataBaseHeader, sizeof(FileDataBaseHeader)*count	, &writeBytesOut, NULL );		// 寫入檔案表頭


	CloseHandle( hFile );

	delete []tempFileDataBaseHeader;
	return count;


}
int cFilePackage::SaveToFile( const char * fileNameStr , tProcessDataProc ProcessDataProc,void *data)
{
	if ( m_table.empty() ) return -1;
	DeleteFileA(fileNameStr);

	HANDLE hFile = CreateFileA( fileNameStr, GENERIC_WRITE,FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS, NULL );

	DWORD offset=0;

	if ( hFile == INVALID_HANDLE_VALUE ) return -2;

	DWORD writeBytesOut;
	DWORD magic = FILEPACKAGE_MAGIC;
	WriteFile( hFile, &magic, sizeof(DWORD), &writeBytesOut, NULL );		// 寫入檔案檢查碼
	offset+=writeBytesOut;
	int count=m_table.size();

	WriteFile( hFile, &count, sizeof(DWORD), &writeBytesOut, NULL );		// 寫入檔案數量
	offset+=writeBytesOut;
	FileDataBaseHeader* tempFileDataBaseHeader=new FileDataBaseHeader[count];

	memset( tempFileDataBaseHeader, 0, sizeof(FileDataBaseHeader)*count ); // GKy 091118 initial value

	DWORD _offset=8;
	map<string, FileDbIndex>::iterator iter = m_table.begin();
	for (int i=0 ; iter != m_table.end(); iter++ ,i++)
	{
	

		FileDbIndex &tab=iter->second;
		tempFileDataBaseHeader[i].m_fileTime=tab.m_fileTime;
		tempFileDataBaseHeader[i].m_fileType=tab.m_fileType;
		tab.m_event=NULL;

		WriteFile( hFile, &tempFileDataBaseHeader[i], sizeof(FileDataBaseHeader), &writeBytesOut, NULL );		// 寫入檔案表頭

		offset+=writeBytesOut;
	}

	int len=0;
	int StringBuffSize=0;
	iter = m_table.begin();
	for ( ; iter != m_table.end(); iter++ )
	{



		len=strlen((iter->first).c_str());


		WriteFile( hFile, &len, sizeof(int), &writeBytesOut, NULL );		//寫入 string len
		offset+=writeBytesOut;

		StringBuffSize+=len+1;

	}

	WriteFile( hFile, &StringBuffSize, sizeof(int), &writeBytesOut, NULL );		//寫入 string List size
	offset+=writeBytesOut;
	//char tempName[2048];

	iter = m_table.begin();
	for (  ; iter != m_table.end(); iter++ )
	{

	//	bool bRDT=iter->second.m_comFmt==COMPRESS_FMT_RDT;
	//	strcpy(tempName,);
		const	char *fileName=(iter->first).c_str();
 		len=strlen(fileName);
// 		if (bRDT)
// 		{
// 			for (int i=len-1;i>=0;i--)
// 			{
// 				if (tempName[i]=='.')
// 				{
// 					tempName[i]=0;
// 					break;
// 				}
// 			}
// 
// 		
// 		//	strcat(tempName,".rdt");
// 
// 		}

	//	tempName[len]=0;
		WriteFile( hFile, fileName, len+1, &writeBytesOut, NULL );		//寫入 string data
		offset+=writeBytesOut;

	}
	DWORD dwThreadId;
	HANDLE preHandle=CreateThread(NULL,
		0,
		&preCompressRDT,
		this,
		0,
		&dwThreadId);

Sleep(3000);
	std::vector<std::string> files;
	count = 0;	
	char  filex[2048];
	iter = m_table.begin();
	for (int i=0 ; iter != m_table.end(); iter++,i++ )
	{
		FileDbIndex &tab=iter->second;
		string filename=tab.m_Sourcename;

		strcpy(filex,filename.c_str());
		bool bRDT=iter->second.m_comFmt==COMPRESS_FMT_RDT;
		const char *name=(iter->first).c_str();
		if (m_cancel)
			break ;

		if (bRDT)
		{
			bool wait=true;
			do
			{
				if (tab.m_event)
				{
					WaitForSingleObject(tab.m_event,INFINITE);
					CloseHandle(tab.m_event);
					tab.m_event=NULL;
					wait=false;


				}
				else
					Sleep(333);
			}while(wait);
			

		}
		
	

		RdtFile rdtfile(filename.c_str(),bRDT,files); 
		
		cFileObj *pFileObj = GetFileObj(name,bRDT);
	


		if ( pFileObj )
		{ 
			tempFileDataBaseHeader[i].m_offset=offset;

			int size=pFileObj->GetDataSize();

			size=pFileObj->SaveData( hFile,ProcessDataProc,data);

			offset+=size;
			count++;
			delete pFileObj;
			pFileObj = NULL;
		}
	/*	else
		{
			if (other)
			{
				cFileObj *pFileObj =other-> GetFileObj((iter->first).c_str());

				int size=pFileObj->GetDataSize();

				size=pFileObj->SaveData( hFile,ProcessDataProc);

				offset+=size;
				count++;


				delete pFileObj;
			}
			
		}*/

		//--DPF(1,"%s",filex);

	}


	DeleteAllFile(files);
	const int zero = 0;
	WriteFile( hFile, &zero, sizeof(int), &writeBytesOut, NULL );			// 區塊結束
	WriteFile( hFile, &count, sizeof(int), &writeBytesOut, NULL );

	SetFilePointer(hFile,_offset,0,FILE_BEGIN);
	WriteFile( hFile, tempFileDataBaseHeader, sizeof(FileDataBaseHeader)*count	, &writeBytesOut, NULL );		// 寫入檔案表頭


	CloseHandle( hFile );

	delete []tempFileDataBaseHeader;
	return count;

	
}

/*
FileDbIndex *cFilePackage::GetFirstItem()
{
	if ( m_table.empty() ) return NULL;
	m_currentIter = m_table.begin();
	return *m_currentIter;
}

FileDbIndex *cFilePackage::GetNextItem()
{	
	if ( m_currentIter != m_table.end() ) m_currentIter++;
	if ( m_currentIter != m_table.end() ) {
		return *m_currentIter;
	}
	return NULL;
}
*/
void StrToLow(char *tt)
{
	int i;
	BYTE *pp;
	pp=(BYTE *)tt;
	for (i=0;;i++)
	{
		if (tt[i]==0)
			break;
		else if (pp[i]>=0x80)
		{
			i++;
		}
		else
		{
			if (tt[i]>='A'&&tt[i]<='Z')
				tt[i]|=0x20;
		}
	}

}

void cFilePackage::SetDirectory( const char * dirStr )
{
	if ( dirStr )
	{
		strcpy( m_dirStr, dirStr );
		int len = (int)strlen( m_dirStr );
		if ( len > 0 && m_dirStr[len-1] != '\\' )
			strcat( m_dirStr, "\\" );
	}
}
void StrToLow(char *tt);
bool cFilePackage::AddToTable(const char * filenameX, FileDbIndex &fileInfo)
{

	char filename[512];
	strcpy(filename,filenameX);
	int len=strlen(filename);

	if (len>MAX_PATH)
		len=MAX_PATH;

	StrToLow( (char *)filename );

	char buf[512];
	//char buf2[512];
	int i = 0;
	int j = 0;

	
	while ( filename[i] ) {
		buf[j] = ::tolower(filename[i]);

		switch (buf[j])
		{
			// "/"
		case 47:
			buf[j] = 92;
			if ( filename[i+1] == 47 )
				i++;
			break;

		case 92:			
			if ( filename[i+1] == 92 )
				i++;
			break;
			
		}
		i++;
		j++;
	}
	buf[j] = 0;

	

	try 
	{


		

		map<string, FileDbIndex>::iterator it=	m_table.find(buf);

	


		if (it!=m_table.end())
		{
			m_table[buf]=fileInfo;
		}
		else
			m_table[buf]= fileInfo ;

		

	}
	catch (...)
	{
		DPF(0,_T("%s"),filename);

	}

	
	return true;	
}

FileDbIndex *cFilePackage::Find(const char *  filename)
{
	char  buf[MAX_PATH];
	int i = 0;
	int j = 0;
	while ( filename[i] ) {
		buf[j] = ::tolower(filename[i]);

		switch (buf[j])
		{
		// "/"
		case 47:			
			buf[j] = 92;
			if ( filename[i+1] == 47 )
				i++;
			break;

		case 92:			
			if ( filename[i+1] == 92 )
				i++;
			break;
		}
		i++;
		j++;
	}
	buf[j] = 0;
	map<string, FileDbIndex>::iterator iter = m_table.find(buf);
	if ( iter != m_table.end() )
		return &(iter->second);
	return NULL;
}

const char* cFilePackage::GetFullNameStr( const char*  fileNameStr )
{
	static char fullNameStr[MAX_PATH];
	if ( fileNameStr ) {
		strcpy( fullNameStr, m_dirStr );
		strcat( fullNameStr, fileNameStr );
		return fullNameStr;
	}
	return NULL;
}