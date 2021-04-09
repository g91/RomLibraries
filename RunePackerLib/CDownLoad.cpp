#include "stdafx.h"
#include "CDownload.h"
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <mmsystem.h>
#include <fstream>
#include <curlpp/http_easy.hpp>
	
class HFILE_trait : public curlpp::output_trait
{

public:
	virtual ~HFILE_trait()
	{}

	HFILE_trait( HANDLE file ) : m_file( file )
	{}

	size_t write( void *buffer, size_t length );

protected:
	HANDLE m_file;
};
size_t HFILE_trait::write( void *buffer, size_t length )
{
	DWORD writeBytesOut=0;

	WriteFile( m_file, buffer, length, &writeBytesOut, NULL );
	
	
	return writeBytesOut;

}


class my_progress_trait : public curlpp::progress_trait
{
public:
	my_progress_trait():m_This(NULL)
	{}
	bool progress_function(double dltotal,
		double dlnow,
		double ultotal,
		double ulnow);

	CFileDownLoad* m_This;
};
 my_progress_trait progress_trait;
bool my_progress_trait::progress_function(double dltotal,
										  double dlnow,
										  double ultotal,
										  double ulnow)
{
	
	m_This->m_totalSize = dltotal;		//檔案大小
	m_This->m_dlSize	 = dlnow;		//已下載Size

	int nowtime=timeGetTime();
	float time=(nowtime-m_This->m_starttime);
	if (time<=0) 
		time=1;
	m_This->m_speed= (m_This-> m_dlSize/time);
	float par=0;
	if (dltotal>0)
		par=( dlnow/dltotal)*100;
	if (m_This-> m_progreessBar)
	m_This-> m_progreessBar->SetProgress(par);

	return true;
}





void CFileDownLoad::SetProgreessBar(Iprogress *ss)
{
	m_progreessBar=ss;
}


CFileDownLoad::CFileDownLoad():m_port(-1),m_hFile(INVALID_HANDLE_VALUE)
{
	curlpp::initialize();
}
CFileDownLoad::~CFileDownLoad()
{

 curlpp::terminate();
}

CFileDownLoad& CFileDownLoad::SetDownloadPath(LPCTSTR Directory)
{

	
	lstrcpy(m_destFolder,Directory);
	return *this;
}
CFileDownLoad& CFileDownLoad::SetURL2(LPCTSTR sAddress,int port)
{
	lstrcpy(m_URL2,sAddress);
	m_port2=port;

	return *this;
}
CFileDownLoad& CFileDownLoad::SetURL(LPCTSTR sAddress,int port)
{
	lstrcpy(m_URL,sAddress);
	m_port=port;

	return *this;
}
CFileDownLoad& CFileDownLoad::SetPassWord(LPCTSTR password)
{
	lstrcpy(m_PassWord,password);
	
	return *this;
}
CFileDownLoad& CFileDownLoad::SetFile(LPCTSTR file,LPCTSTR remotefile)
{
	lstrcpy(m_file,file);
	
	lstrcpy(m_remotefile,remotefile);

	return *this;
}
vector<CURLcode>& CFileDownLoad::GetCurlErrCode()
{

	

	return m_ErrCode;

}
float CFileDownLoad::GetDLSpeed()
{

	return m_speed;
}
int CFileDownLoad::TotalSize()
{
	return m_totalSize;
}
int CFileDownLoad::DownloadSize()
{
return m_dlSize;
}

BOOL CFileDownLoad::_DownLoad(HANDLE hFile,int offset,int frist)
{



	
	BOOL ret=FALSE;

	curlpp::http_easy request;



	
	char UrlFile[ 512 ];
	char localFile[512];



#ifdef _UNICODE

	TCHAR tt[512];

	if (!frist)
		wsprintf( tt , L"%s%s", (LPCTSTR)m_URL ,(LPCTSTR) m_remotefile );
	else
		wsprintf( tt , L"%s%s", (LPCTSTR)m_URL2 ,(LPCTSTR) m_remotefile );
	int len=lstrlen(tt);
	len = WideCharToMultiByte(CP_ACP, 0, tt, len, UrlFile, 511, NULL, NULL);

	UrlFile[len]=0;


#else
	if (!frist)
		wsprintfA( UrlFile , "%s%s", (LPCTSTR)m_URL ,(LPCTSTR) m_remotefile );
	else
		wsprintfA( UrlFile , "%s%s", (LPCTSTR)m_URL2 ,(LPCTSTR) m_remotefile );
#endif
	



	HFILE_trait body_trait( hFile );
	request.m_body_storage.trait( &body_trait ); 


	request.resume_from(offset);
	request.follow_location(true);
	// request.  m_data->follow_location( true );

	 request.url( UrlFile);
	 if (!frist)
	 {

		 if (m_port>0&&m_port!=80)	
			 request.port(m_port);
	 }
	 else
	 {
		 if (m_port2>0&&m_port!=80)	
			 request.port(m_port2);

	 }

	 try
	 {

		// request.ssl_cipher_list()
		 request.progress();

		 progress_trait.m_This=this;
	
		 request.m_progress_storage.trait( &progress_trait );



		 request.perform();
	 }
	 catch (curlpp:: exception* err)
	 {

		const char * strerr=err->what();

		char temp[512];
		


		sprintf( temp ,"%s %d %s",(LPCTSTR)UrlFile,m_port,strerr);


		MessageBoxA(NULL,temp,"Error",0);


	 }

	

	
#ifdef _DEBUG
	getchar();
#endif
	
	return TRUE;


}
void CFileDownLoad::stop(void)
{
 
if (m_hFile!=INVALID_HANDLE_VALUE)
CloseHandle(m_hFile);
m_hFile=0;
}

BOOL CFileDownLoad::DownLoad(int tryCount,int first)
{

	BOOL ret=TRUE;
	int count=tryCount;


	TCHAR wlocalFile[ 512 ];
	wsprintf( wlocalFile , _T("%s%s"),(LPCTSTR) m_destFolder , (LPCTSTR)m_file );
//	char localFile[512];

	//int len=lstrlen(wlocalFile);
	//len = WideCharToMultiByte(CP_ACP, 0, wlocalFile, len, localFile, 511, NULL, NULL);

//	localFile[len]=0;

	if ( (m_hFile = CreateFile( wlocalFile, GENERIC_EXECUTE|GENERIC_WRITE, FILE_APPEND_DATA, NULL, OPEN_EXISTING, 0, NULL ) ) == INVALID_HANDLE_VALUE ) 
	{
		m_hFile =CreateFile(wlocalFile,GENERIC_EXECUTE|GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
		
		
	}
	
//	std::ofstream file(wlocalFile,std::ios::app|std::ios::binary); 


	

	m_totalSize=0;

	m_speed=0;
	m_dlSize=0;
	do 
	{




		m_offset=GetFileSize(m_hFile,NULL);
		SetFilePointer(m_hFile,m_offset,0,FILE_BEGIN);		
		
		try 
		{
			count--;
			ret=_DownLoad(m_hFile,m_offset,first);
			if (ret)
				break;
		}
		catch (...)
		{


			if (count<=0)
			{

			
			char temp[512];
			char UrlFile[512];
			sprintf( UrlFile , "%s:%d %s", (LPCTSTR)m_URL ,m_port,(LPCTSTR) m_remotefile );


			sprintf( temp , "Download %s fail!!" ,UrlFile);


			MessageBoxA(NULL,temp,"Error",0);

			ret=FALSE;
			}
		}


	}while(count>0);

	

	CloseHandle(m_hFile);

	return ret;

	
}
