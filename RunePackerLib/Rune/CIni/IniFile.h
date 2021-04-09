#ifndef		__INIFILE_H_2003_16__
#define		__INIFILE_H_2003_16__

#pragma warning (disable:4786)
#pragma warning (disable:4949)
#include	<string>
#include	<set>
#include	<tchar.h>
#include	<windows.h>
#pragma unmanaged

using namespace std;

#ifdef UNICODE 
#define CTString  std::wstring
#else
#define CTString  std::string
#endif



class IniFileClass 
{
protected:
	//檔案完整路徑
	set<CTString>		_FullPath;
	CTString          _Section;
public:
	IniFileClass();
	//設定INI檔案 (可負數設定)
	bool	SetIniFile(LPCTSTR FileName);
	void	SetSection(LPCTSTR section);
	CTString GetSection(void);
	int		Int(LPCTSTR key);
	int		IntDef(LPCTSTR key,int defValue =0);
	CTString	Str(LPCTSTR key);
	CTString	StrDef(LPCTSTR key,LPCTSTR defValue =0);
	float	Float( LPCTSTR key );
	float	FloatDef(LPCTSTR key ,float defValue =0);

	bool	Int( LPCTSTR key, int&		OutInt );
	bool	SetInt(LPCTSTR key,int value );
	bool	Str( LPCTSTR key , CTString&	OutString );
	bool	Float( LPCTSTR key , float&	f );
	bool	SetStr(LPCTSTR key, LPCTSTR value);
	bool	SetFloat( LPCTSTR key ,float f );
};


#pragma managed
#endif