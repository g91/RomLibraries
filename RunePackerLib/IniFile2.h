#ifndef		__INIFILE2_H_2008_6__
#define		__INIFILE2_H_2008_6__

#pragma warning (disable:4786)
#pragma warning (disable:4949)
#include	<string>
#include	<set>
#include	<tchar.h>
#include	<windows.h>


#include "SimpleIni.h"
#pragma unmanaged

using namespace std;




class IniFile2 
{
protected:

	TCHAR          _Section[512];

	CSimpleIni        _ini;
public:
	IniFile2();
	//設定INI檔案 (可複數設定)
	void Reset(void);
	void Delete(LPCTSTR key);
	bool	SetIniFile(LPCTSTR FileName);
	bool	SetIniData(LPVOID  data,DWORD size);
	void	SetSection(LPCTSTR section);
	LPCTSTR GetSection(void);
	int		Int(LPCTSTR key);
	int		IntDef(LPCTSTR key,int defValue =0);
	LPCTSTR	Str(LPCTSTR key);
	LPCTSTR	StrDef(LPCTSTR key,LPCTSTR defValue =0);
	float	Float( LPCTSTR key );
	float	FloatDef(LPCTSTR key ,float defValue =0);

	void SetUnicode(bool s);
	void Save(LPCTSTR FileName);
	void SaveString (std::string  &buff);
	bool	SetInt(LPCTSTR key,int value );


	bool	SetStr(LPCTSTR key, LPCTSTR value);
	bool	SetFloat( LPCTSTR key ,float f );
	
};


#pragma managed
#endif