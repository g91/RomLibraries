#include "stdafx.h"
#include "iniFile2.h"

#include <windows.h>
#include <shlwapi.h>

#pragma warning (disable:4949)
#pragma unmanaged

//**************************************************************************************
//ÀRºA¸ê®Æ
//**************************************************************************************
IniFile2::IniFile2()
{
	_ini.SetUnicode(true);
	lstrcpy(_Section,_T("BOOT"));
}

bool IniFile2::SetInt(LPCTSTR key,int value )

{
	TCHAR temp[256];
	_stprintf(temp,_T("%d"),value);

	if(	SetStr( key , temp ) == false )
		return false;
	return true; 
}
void IniFile2::Delete(LPCTSTR key)
{

	_ini.Delete(_Section,key);
}
int	IniFile2::Int(LPCTSTR key )

{
	LPCTSTR a=Str(key);
	if (lstrlen(a)==0)
		return 0;

	return _tstoi(a);
}

int		IniFile2::IntDef(LPCTSTR key,int def )

{
	LPCTSTR a=Str(key);
	if (lstrlen(a)==0)
		return def;

	return _tstoi(a);
}


void	IniFile2::SetSection(LPCTSTR section)
{
	lstrcpy(_Section,section);

}
LPCTSTR 	IniFile2::GetSection(void)
{

	return _Section;
}
float	IniFile2::Float( LPCTSTR key )
{
	LPCTSTR a=Str(key);
    if (lstrlen(a)==0)
		return 0;

	return (float)_tstof(a);
}

LPCTSTR IniFile2::StrDef(LPCTSTR key,LPCTSTR defValue)
{
	LPCTSTR a=Str(key);
	if (lstrlen(a)==0)
		return defValue;

	return a;
}
float	IniFile2::FloatDef( LPCTSTR key  ,float def  )
{
	LPCTSTR a=Str(key);
    if (lstrlen(a)==0)
		return def;

	return (float)_tstof(a);
}
bool	IniFile2::SetFloat( LPCTSTR key ,float f )
{
	TCHAR temp[256];
	_stprintf(temp,_T("%f"),f);

	if(	SetStr( key , temp ) == false )
		return false;

return true;
}

bool IniFile2::SetStr(LPCTSTR key, LPCTSTR value)
{
	TCHAR Buf[256];

	SI_Error 	Ret;

	
	Ret=_ini.SetValue(_Section,key ,value);
		 

		
	
	if( Ret != SI_OK )
	{
		_stprintf( Buf , _T("IniFile : Name = %s not find!") , key );
		OutputDebugString( Buf );
		return false;
	}

	return true;
}
LPCTSTR IniFile2::Str(LPCTSTR key)
{
	
	
	LPCTSTR pValue=_ini.GetValue(_Section, key  );

	if(!pValue)
	{
		TCHAR Buf[1024];
		_stprintf( Buf , _T("IniFile : Name = %s not find!"), key );
		OutputDebugString( Buf );
		return _T("");
	}

	return pValue;
}

bool	IniFile2::SetIniFile(LPCTSTR filename)
{
	TCHAR Path[1024];
	
	lstrcpy(Path,filename);
	// if file contain filepath
	if( PathFileExists( filename ) == false )
	{		
		TCHAR tCurrentDirectory[1024];
		GetCurrentDirectory(sizeof(tCurrentDirectory),tCurrentDirectory);
		wsprintf(Path,_T("%s\\%s") ,tCurrentDirectory,filename);	
	}
	return _ini.LoadFile(Path)==SI_OK;


	//return true;
}


void IniFile2::SetUnicode(bool s)
{
_ini.SetUnicode(s);

}

void IniFile2::Reset(void)
{
_ini.Reset();
}
bool IniFile2::SetIniData(LPVOID data,DWORD size)
{

	return _ini.Load((char *)data,size)==SI_OK;


	//return true;
}

void IniFile2::Save(LPCTSTR FileName)
{

	_ini.SaveFile(FileName);



}
void IniFile2::SaveString (std::string &buff)
{

	_ini.Save(buff);



}
#pragma managed