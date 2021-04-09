#include "..\..\stdafx.h"
#include "iniFile.h"

#include <windows.h>
#include <shlwapi.h>

#pragma warning (disable:4949)
#pragma unmanaged

//**************************************************************************************
//靜態資料
//**************************************************************************************
IniFileClass::IniFileClass()
{
	_Section=_T("BOOT");
}

bool IniFileClass::SetInt(LPCTSTR key,int value )

{
	TCHAR temp[256];
	_stprintf(temp,_T("%d"),value);

	if(	SetStr( key , temp ) == false )
		return false;
	return true; 
}

int		IniFileClass::Int(LPCTSTR key )

{
	CTString a=Str(key);
	if (a.length()==0)
		return 0;

	return _tstoi(a.c_str());
}

int		IniFileClass::IntDef(LPCTSTR key,int def )

{
	CTString a=Str(key);
	if (a.length()==0)
		return def;

	return _tstoi(a.c_str());
}


void	IniFileClass::SetSection(LPCTSTR section)
{
	_Section=section;

}
CTString 	IniFileClass::GetSection(void)
{

	return _Section;
}
float	IniFileClass::Float( LPCTSTR key )
{
	CTString a=Str(key);
    if (a.length()==0)
		return 0;

	return (float)_tstof(a.c_str());
}

CTString IniFileClass::StrDef(LPCTSTR key,LPCTSTR defValue)
{
	CTString a=Str(key);
	if (a.length()==0)
		return defValue;

	return a;
}
float	IniFileClass::FloatDef( LPCTSTR key  ,float def  )
{
	CTString a=Str(key);
    if (a.length()==0)
		return def;

	return (float)_tstof(a.c_str());
}
bool	IniFileClass::SetFloat( LPCTSTR key ,float f )
{
	TCHAR temp[256];
	_stprintf(temp,_T("%f"),f);

	if(	SetStr( key , temp ) == false )
		return false;

return true;
}
bool	IniFileClass::Float( LPCTSTR key , float&	f )
{

	CTString tempStr;
	if( Str( key , tempStr ) == false )
		return false;

	f = (float)_tstof( tempStr.c_str() );
	return true;
}
bool IniFileClass::SetStr(LPCTSTR key, LPCTSTR value)
{
	TCHAR Buf[256];
	set<CTString>::iterator Iter;
	int 	Ret=0;

	for( Iter = _FullPath.begin() ; Iter != _FullPath.end() ; Iter++)
	{
		Ret = WritePrivateProfileString(_Section.c_str(), key,  value, Iter->c_str()); 

		//如果有找到資料則結束
		if( Ret > 0 )
			break;		
	}
	if( Iter == _FullPath.end() )
	{
		_stprintf( Buf , _T("IniFile : Name = %s not find!") , key );
		OutputDebugString( Buf );
		return false;
	}

	return true;
}
CTString	IniFileClass::Str(LPCTSTR key)
{
	TCHAR Buf[256];
	set<CTString>::iterator Iter;
	int		Ret;

	for( Iter = _FullPath.begin() ; Iter != _FullPath.end() ; Iter++)
	{
		Ret = GetPrivateProfileString(_Section.c_str(), key, NULL, Buf, sizeof(Buf),Iter->c_str()); 

		//如果有找到資料則結束
		if( Ret > 0 )
			break;		
	}
	if( Iter == _FullPath.end() )
	{
		_stprintf( Buf , _T("IniFile : Name = %s not find!"), key );
		OutputDebugString( Buf );
		return _T("");
	}

	return Buf;
}

bool	IniFileClass::SetIniFile(LPCTSTR filename)
{
	TCHAR	Path[1024];
	CTString	FileFullPath;

	GetCurrentDirectory(sizeof(Path),Path);
	FileFullPath = Path ;
	FileFullPath = FileFullPath +_T( "\\") + filename;

	// if file contain filepath
	if( PathFileExists( FileFullPath.c_str() ) == false )
	{
/*		//檢查檔案是否存在
		if( PathFileExists( FileFullPath.c_str()) == false )
		{
			return false;
		}
		*/
		if( PathFileExists( filename ) == false )
		{
			return false;
		}
		_FullPath.insert( filename );
	}
	else
	{
		
		_FullPath.insert( FileFullPath );
	}

	return true;
}

bool	IniFileClass::Int( LPCTSTR key , int& OutInt )
{
	CTString tempStr;
	if( Str( key , tempStr ) == false )
		return false;

	OutInt = _tstoi( tempStr.c_str() );
	return true;
}
bool	IniFileClass::Str( LPCTSTR key , CTString&	OutString )
{
	TCHAR Buf[256];
	set<CTString>::iterator Iter;
	int		Ret;

	for( Iter = _FullPath.begin() ; Iter != _FullPath.end() ; Iter++)
	{
		Ret = GetPrivateProfileString(_Section.c_str(), key, NULL, Buf, sizeof(Buf),Iter->c_str()); 

		//如果有找到資料則結束
		if( Ret > 0 )
			break;		
	}
	if( Iter == _FullPath.end() )
	{
		return false;
	}

	OutString = Buf;

	return true;
}
#pragma managed