#pragma once
#pragma warning (disable:4786)
#pragma warning (disable:4949)
#include <map>
#include <queue>
#include <vector>
#include <string>
#include "Singleton/singleton.h"
#include <boost/function.hpp>
#include <functional>
#pragma unmanaged
using namespace	std;
class SchedularInfo;
class FunctionSchedularClass;
#define SchedularFunctionBase boost::function<int ( SchedularInfo*	Obj,void*	InputClass ) >
class SchedularInfo
{
	friend class FunctionSchedularClass;
protected:
	SchedularInfo* _Next;
	SchedularInfo* _Front;
	long _GUID;
	long _ProcessTime;
	SchedularFunctionBase _Func;
	map<string, string> _Arg;
	bool _Enable;
	void* _UserObjPoint;
public:
	SchedularInfo();
	int ProcessTime();
	bool Enable();
	int GetNumber(string Name);
	char* GetString(string Name);
	void PushValue(string Name, string Value);
	void Add(SchedularInfo* Next);
	long GUID() { return _GUID; };
	UINT64 GetUINT64(string Name);
};
struct	SICompGreat {
	bool operator() (SchedularInfo* A, SchedularInfo* B)
	{
		return A->ProcessTime() > B->ProcessTime();
	}
};
enum FuncSchedularValueTypeENUM
{
	EM_ValueType_double = 1,
	EM_ValueType_int = 2,
	EM_ValueType_string = 3,
	EM_ValueType_uint64 = 4,
	EM_FuncSchedularValueType_FORCE_DWORD = 0x7fffffff,
};
template<class T, class Container, class Compare>
class MyP_Queue1 : public priority_queue<T, Container, Compare>
{
public:
	void clear() { c.clear(); };
};
class FunctionSchedularClass
{
protected:
	MyP_Queue1< SchedularInfo*, vector<SchedularInfo*>, SICompGreat>_PQueue;
	vector<SchedularInfo*> _FunList;
	vector<int> _NousedGUIDList;
	int _GetGUID();
	bool _ReleaseGUID(int guid);
	SchedularInfo* _GetSchedularInfo(int guid);
	bool _SetSchedularInfo(int guid, SchedularInfo*);
	bool _EraseSchedularInfo(SchedularInfo*);
public:
	~FunctionSchedularClass();
	void Clear();
	int Push(SchedularFunctionBase Func, int  ProcTime, void* UserObjPoint, ...);
	bool AddPush(int GUID, SchedularFunctionBase Func, void* UserObjPoint, ...);
	bool Erase(int GUID);
	void Process();
};
extern FunctionSchedularClass g_Schedular;
//=====================================================================================
#pragma managed