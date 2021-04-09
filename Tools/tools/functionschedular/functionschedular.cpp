#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include "FunctionSchedular.h"
#pragma warning (disable:4949)
#pragma unmanaged
FunctionSchedularClass g_Schedular;
SchedularInfo::SchedularInfo()
{
	_Next = NULL;							
	_Front = NULL;						
	_GUID = -1;						
	_ProcessTime = 0;				
	_Func = NULL;
	_Enable = true;					
}
FunctionSchedularClass::~FunctionSchedularClass()
{
	Clear();
}
int SchedularInfo::ProcessTime()
{
	return _ProcessTime;
}
bool SchedularInfo::Enable()
{
	return _Enable;
}
char* SchedularInfo::GetString(string Name)
{
	map<string, string>::iterator Iter;
	Iter = _Arg.find(Name);
	if (Iter == _Arg.end())
		return NULL;
	return (char*)Iter->second.c_str();
}
int SchedularInfo::GetNumber(string Name)
{
	map<string, string>::iterator Iter;
	Iter = _Arg.find(Name);
	if (Iter == _Arg.end())
		return 0;
	return atol(Iter->second.c_str());
}
void SchedularInfo::PushValue(string Name, string Value)
{
	_Arg[Name] = Value;
}
void SchedularInfo::Add(SchedularInfo* Next)
{
	SchedularInfo* Temp;
	for (Temp = this; Temp->_Next != NULL; Temp = Temp->_Next);
	Temp->_Next = Next;
	Next->_Front = Temp->_Next;
}
void FunctionSchedularClass::Clear()
{
	int i;
	for (i = 0; i < (int)_FunList.size(); i++)
	{
		if (_FunList[i] != NULL)
			delete _FunList[i];
	}
	_FunList.clear();
	_NousedGUIDList.clear();
	_PQueue.clear();
}
int FunctionSchedularClass::_GetGUID()
{
	int Ret;
	if (_NousedGUIDList.size() != 0)
	{
		Ret = _NousedGUIDList.back();
		_NousedGUIDList.pop_back();
		return Ret;
	}
	Ret = (int)_FunList.size();
	_FunList.push_back(NULL);
	return Ret;
}
bool FunctionSchedularClass::_ReleaseGUID(int id)
{
	if (_FunList.size() <= (unsigned)id)
		return false;
	_FunList[id] = NULL;
	_NousedGUIDList.push_back(id);
	return true;
}
SchedularInfo* FunctionSchedularClass::_GetSchedularInfo(int guid)
{
	if ((unsigned)guid >= _FunList.size())
		return NULL;
	return _FunList[guid];
}
bool FunctionSchedularClass::_SetSchedularInfo(int guid, SchedularInfo* Info)
{
	if ((unsigned)guid >= _FunList.size())
		return false;
	_FunList[guid] = Info;
	return true;
}
int		FunctionSchedularClass::Push(SchedularFunctionBase Func, int  ProcTime, void* _UserObjPoint, ...)
{
	FuncSchedularValueTypeENUM	type;
	char Buf[64];
	char* name;
	char* value;
	SchedularInfo* Info = NEW(SchedularInfo);
	va_list marker;
	va_start(marker, _UserObjPoint);
	while (1)
	{
		name = va_arg(marker, char*);
		if (name == NULL)
			break;
		type = (FuncSchedularValueTypeENUM)va_arg(marker, int);
		switch (type)
		{
		case EM_ValueType_double:
			sprintf(Buf, "%f", va_arg(marker, double));
			value = Buf;
			break;
		case EM_ValueType_int:
			sprintf(Buf, "%d", va_arg(marker, int));
			value = Buf;
			break;	
		case EM_ValueType_string:
			value = va_arg(marker, char*);
			break;
		case EM_ValueType_uint64:
			sprintf(Buf, "%I64u", va_arg(marker, UINT64));
			value = Buf;
			break;					
		}
		Info->PushValue(name, value);
	}
	va_end(marker);
	Info->_GUID = _GetGUID();
	Info->_ProcessTime = ProcTime + clock();
	Info->_Func = Func;
	Info->_UserObjPoint = _UserObjPoint;
	_SetSchedularInfo(Info->_GUID, Info);
	_PQueue.push(Info);
	return Info->_GUID;
}
bool FunctionSchedularClass::AddPush(int GUID, SchedularFunctionBase Func, void* _UserObjPoint, ...)
{
	FuncSchedularValueTypeENUM	type;
	SchedularInfo* PrvInfo = _GetSchedularInfo(GUID);
	if (PrvInfo == NULL)
		return false;
	char* name;
	char* value;
	SchedularInfo* Info = NEW(SchedularInfo);
	va_list marker;
	char Buf[64];
	va_start(marker, _UserObjPoint);
	while (1)
	{
		name = va_arg(marker, char*);
		if (name == NULL)
			break;
		type = (FuncSchedularValueTypeENUM)va_arg(marker, int);
		switch (type)
		{
		case EM_ValueType_double:
			sprintf(Buf, "%f", va_arg(marker, double));
			value = Buf;
			break;
		case EM_ValueType_int:
			sprintf(Buf, "%d", va_arg(marker, int));
			value = Buf;
			break;
		case EM_ValueType_string:
			value = va_arg(marker, char*);
			break;
		case EM_ValueType_uint64:
			sprintf(Buf, "%I64u", va_arg(marker, UINT64));
			value = Buf;
			break;			
		}
		Info->PushValue(name, value);
	}
	va_end(marker);
	Info->_UserObjPoint = _UserObjPoint;
	Info->_Func = Func;
	PrvInfo->Add(Info);
	return true;
}
bool FunctionSchedularClass::Erase(int GUID)
{
	SchedularInfo* PrvInfo = _GetSchedularInfo(GUID);
	if (PrvInfo == NULL)
		return false;
	PrvInfo->_Enable = false;
	return true;
}
bool FunctionSchedularClass::_EraseSchedularInfo(SchedularInfo* Info)
{
	for (; Info->_Front != NULL; Info = Info->_Front);
	_ReleaseGUID(Info->_GUID);
	for (; Info->_Next != NULL; Info = Info->_Next)
		delete Info->_Front;
	delete Info;
	return true;
}
void FunctionSchedularClass::Process()
{
	int IntervalTime;
	SchedularInfo* Info;
	while (1)
	{
		if (_PQueue.size() == 0)
			return;
		Info = _PQueue.top();
		if (Info->_ProcessTime - clock() > 0)
			return;
		_PQueue.pop();
		if (!Info->Enable())
		{
			_EraseSchedularInfo(Info);
			continue;
		}
		if (Info->_Func != NULL)
		{
			IntervalTime = Info->_Func(Info, Info->_UserObjPoint);
			if (IntervalTime == -1 || Info->_Next == NULL)
			{
				_EraseSchedularInfo(Info);
			}
			else
			{
				Info->_Next->_ProcessTime = clock() + IntervalTime;
				_PQueue.push(Info->_Next);
			}
		}
	}
}					  
UINT64 SchedularInfo::GetUINT64(string Name)
{
	map<string, string>::iterator Iter;
	Iter = _Arg.find(Name);
	if (Iter == _Arg.end())
		return 0;
	return _strtoui64(Iter->second.c_str(), NULL, 10);
}
#pragma managed