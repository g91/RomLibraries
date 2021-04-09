#pragma once


#include "Curlpp\curlpp.hpp"
#include <vector>
using namespace std;

struct Iprogress
{
	virtual void SetProgress(float par)=0;
};
class CFileDownLoad
{
public:
	CFileDownLoad();
	virtual ~CFileDownLoad();

	TCHAR m_URL[512];
	TCHAR m_URL2[512];
	TCHAR m_PassWord[512];
	TCHAR m_destFolder[512];
	TCHAR m_file[512];
	TCHAR m_remotefile[512];

	int m_port;
	int m_port2;
	int m_starttime;
	int m_totalSize;
	float m_speed;
	int m_offset;
	int m_dlSize   ;
	int TotalSize();
	int DownloadSize();
	int Offset(){return m_offset; }
	float GetDLSpeed();
	vector<CURLcode> m_ErrCode;
	vector<CURLcode>& GetCurlErrCode();
	HANDLE m_hFile;

	Iprogress* m_progreessBar;
	void stop(void);
	void SetProgreessBar(Iprogress *ss);
	CFileDownLoad& SetURL(LPCTSTR URLAddress,int port);
	CFileDownLoad& SetURL2(LPCTSTR sAddress,int port);
	CFileDownLoad& SetDownloadPath(LPCTSTR Directory);
	CFileDownLoad& SetPassWord(LPCTSTR password);
	CFileDownLoad& SetFile(LPCTSTR localfile,LPCTSTR remotefile);
	BOOL DownLoad(int tryCount=3,int first=0);
	BOOL _DownLoad(HANDLE hFile,int offset,int first=0);
};