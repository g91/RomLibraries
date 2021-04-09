#pragma once

#pragma warning(disable : 4949)
#pragma warning(disable : 4786)
#pragma unmanaged

#include <map>
#include <vector>
#include <stdio.h>
#include <algorithm>

#include "BaseFunction/BaseFunction.h"
#include "SmallObj/SmallObj.h"
#include "ReaderClass/ReaderClass.h"
#include "Malloc/MyAlloc.h"
#include "recyclebin/recyclebin.h"

#include "filebase/FileBase.h"

#include "iomanip"

using namespace std;

namespace NSObjectDataFile
{
#define DEF_MAX_NEED_FREE_COUNT 5000
#define DEF_ITEMOBJ_FILE_ID 0x6396
typedef void (*pFuncMessageOut)(int, const char *, const char *, ...);
typedef void (*pFuncInitObj)(void *, void *);

struct ObjFileHead
{
	char FileInfoStr[128];
	long Idenitfy;
	long Count;
	long Size;

	ObjFileHead()
	{
		Idenitfy = DEF_ITEMOBJ_FILE_ID;
		strcpy(FileInfoStr, "Item Obj File");
	};
};

struct ObjFileInfo
{
	MyFileBase *File;
	ObjFileHead FileHead;
	ObjFileInfo()
	{
		memset(this, 0, sizeof(*this));
	}
	~ObjFileInfo()
	{
		if (File != NULL)
			delete File;
	}
};

template <class T>
struct ObjBaseStruct
{
	ObjFileInfo *FileInfo;
	int Seek;
	T *Obj;
	union {
		int Mode;
		struct
		{
			short AccessCount;
			bool IsLoad : 1;
			bool IsInit : 1;
			bool IsNeedFree : 1;
		};
	};

	ObjBaseStruct()
	{
		Init();
	}

	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};

enum ObjectDataLoadTypeENUM
{
	EM_ObjectDataLoadType_None,
	EM_ObjectDataLoadType_LoadAll,
	EM_ObjectDataLoadType_LoadAll_NoFree,
};

template <class T, class FileClass = MyFile>
class ObjectDataFileClass
{
  private:
	map<int, ObjBaseStruct<T> *> _ObjData;
	vector<char *> _SourceData;
	vector<ObjFileInfo *> _FileInfoList;
	typename map<int, ObjBaseStruct<T> *>::iterator _SearchIter;
	pFuncMessageOut _MsgOutFunc;
	pFuncInitObj _InitObjFunc;
	int _NeedFreeCount;
	RecycleBin<ObjBaseStruct<T>> _ObjBaseRecycleBin;
	void *_InitUserObj;

  public:
	static bool ObjCmp(T *A, T *B)
	{
		return A->GUID < B->GUID;
	};

	ObjectDataFileClass();
	~ObjectDataFileClass();

	void Release();
	static bool EditorPointFilter(const ReadInfoStruct &a, const ReadInfoStruct &b);
	bool OpenFile(const char *FileName, ObjectDataLoadTypeENUM LoadType, map<string, ReadInfoStruct> *ReadInfo = NULL);

	ObjBaseStruct<T> *GetObjBase(int id);
	T *GetObj(int id);

	int TotalInfoCount() { return (int)_ObjData.size(); };

	T *GetDataByOrder(bool IsResetData = false);
	void PrintEditorPoint(map<string, ReadInfoStruct> *ReadInfo);
	void SetData(T *);

	void Process(int MaxObjCount = DEF_MAX_NEED_FREE_COUNT);

	vector<ObjFileInfo *> *FileInfoList() { return &_FileInfoList; }

	void SetOutPutFunc(pFuncMessageOut pFunc) { _MsgOutFunc = pFunc; }
	void SetInitFunc(pFuncInitObj pFunc, void *UserObj)
	{
		_InitObjFunc = pFunc;
		_InitUserObj = UserObj;
	}
};

template <class T, class FileClass>
void ObjectDataFileClass<T, FileClass>::Release()
{
	int i;
	for (i = 0; i < (int)_SourceData.size(); i++)
	{
		delete _SourceData[i];
	}
	_SourceData.clear();

	{
		vector<ObjFileInfo *>::iterator Iter;
		for (Iter = _FileInfoList.begin(); Iter != _FileInfoList.end(); Iter++)
		{
			ObjFileInfo *Info = *Iter;
			if (Info->File)
				Info->File->Close();
			delete *Iter;
		}
		_FileInfoList.clear();
	}

	{
		map<int, ObjBaseStruct<T> *>::iterator Iter;
		for (Iter = _ObjData.begin(); Iter != _ObjData.end(); Iter++)
		{
			if (Iter->second->Obj != NULL && Iter->second->IsLoad != false && Iter->second->IsNeedFree != false)
			{
				delete Iter->second->Obj;
			}
			_ObjBaseRecycleBin.DeleteObj(Iter->second);
		}
		_ObjData.clear();
	}
	_NeedFreeCount = 0;
}
//-------------------------------------------------------------------------------------------------
template <class T, class FileClass>
ObjectDataFileClass<T, FileClass>::ObjectDataFileClass()
	: _ObjBaseRecycleBin(1000)
{
	_InitObjFunc = NULL;
	_MsgOutFunc = NULL;
	_NeedFreeCount = 0;
};

template <class T, class FileClass>
ObjectDataFileClass<T, FileClass>::~ObjectDataFileClass()
{
	Release();
};

template <class T, class FileClass>
bool ObjectDataFileClass<T, FileClass>::EditorPointFilter(const ReadInfoStruct &a, const ReadInfoStruct &b)
{
	return a.Point < b.Point;
}

template <class T, class FileClass>
bool ObjectDataFileClass<T, FileClass>::OpenFile(const char *FileName, ObjectDataLoadTypeENUM LoadType, map<string, ReadInfoStruct> *ReadInfo)
{
	ObjFileInfo *FileInfo = new ObjFileInfo;

	ObjFileHead &Head = FileInfo->FileHead;
	FileClass *File = NEW FileClass;
	char *Data;
	int i;
	FileInfo->File = File;

	if (File->Open(FileName) == false)
	{
		delete FileInfo;
		return false;
	}

	File->Read(&Head, sizeof(Head));

	if (Head.Idenitfy != DEF_ITEMOBJ_FILE_ID)
	{
		delete FileInfo;
		return false;
	}

	switch (LoadType)
	{
	case EM_ObjectDataLoadType_None:
	{
		for (i = 0; i < Head.Count; i++)
		{
			T TempObj;
			File->Read(&TempObj, Head.Size);
			ObjBaseStruct<T> *TempObjBase = _ObjBaseRecycleBin.NewObj();
			TempObjBase->Init();
			TempObjBase->FileInfo = FileInfo;
			TempObjBase->Seek = i * Head.Size + sizeof(Head);
			_ObjData[TempObj.GUID] = TempObjBase;
		}
	}
	break;
	case EM_ObjectDataLoadType_LoadAll:
	{
		for (i = 0; i < Head.Count; i++)
		{
			ObjBaseStruct<T> *TempObjBase = _ObjBaseRecycleBin.NewObj();
			TempObjBase->Init();
			TempObjBase->FileInfo = FileInfo;
			TempObjBase->Seek = i * Head.Size + sizeof(Head);
			TempObjBase->Obj = (T *)NEW char[Head.Size];
			TempObjBase->IsNeedFree = true;
			TempObjBase->IsLoad = true;
			File->Read(TempObjBase->Obj, Head.Size);
			_ObjData[TempObjBase->Obj->GUID] = TempObjBase;
			_NeedFreeCount++;
		}
	}
	break;
	case EM_ObjectDataLoadType_LoadAll_NoFree:
	{
		int DataSizeBase = (Head.Size + sizeof(int) - 1) / sizeof(int) * sizeof(int);
		Data = NEW(char[DataSizeBase * Head.Count + sizeof(T)]);
		_SourceData.push_back((char *)Data);

		for (i = 0; i < Head.Count; i++)
		{
			T *D1 = (T *)(Data + i * DataSizeBase);
			File->Read(D1, Head.Size);
			ObjBaseStruct<T> *TempObjBase = _ObjBaseRecycleBin.NewObj();
			TempObjBase->Init();
			TempObjBase->FileInfo = FileInfo;
			TempObjBase->IsLoad = true;
			TempObjBase->Seek = i * Head.Size + sizeof(Head);
			TempObjBase->Obj = D1;

			_ObjData[D1->GUID] = TempObjBase;
		}
	}
	break;
	}

	_FileInfoList.push_back(FileInfo);

	if( ReadInfo != NULL )
	{
		map< string , int > NameAddressMap;
		int		StrLen = 0;
		int		Point = 0;
		char	ValueName[512];
		while(1)
		{
			if( File->Read( (char*)&( StrLen ) , sizeof(int) ) == 0 )
				break;
			File->Read( (char*)&(Point ) , sizeof(int) );
			File->Read( ValueName , StrLen );
			NameAddressMap[ ValueName ] = Point;
		}

		bool CheckVersionError = false;
		map<string, ReadInfoStruct>::iterator Iter;
		int bugFilePoint = 999999999;
		int bugProgramPoint = 999999999;
		for (Iter = ReadInfo->begin(); Iter != ReadInfo->end(); Iter++)
		{
			ReadInfoStruct *IDData = &(Iter->second);
			int FilePoint = NameAddressMap[IDData->ValueName];
			if (IDData->Point != FilePoint)
			{
				if (FilePoint < bugFilePoint)
					bugFilePoint = FilePoint;
				if (IDData->Point < bugProgramPoint)
					bugProgramPoint = IDData->Point;
				//Print(LV5, "OpenFile", "FileName=%s , FieldName=%s FilePoint=%d ProgramPont=%d", FileName, IDData->ValueName.c_str(), FilePoint, IDData->Point);
				CheckVersionError = true;
			}
		}
		std::vector<ReadInfoStruct> tempVec;
		if (CheckVersionError != false)
		{
			for (Iter = ReadInfo->begin(); Iter != ReadInfo->end(); Iter++)
			{
				ReadInfoStruct *IDData = &(Iter->second);
				ReadInfoStruct NewData;
				NewData.ValueName = IDData->ValueName;
				NewData.Point = IDData->Point;
				tempVec.push_back(NewData);
			}
			sort(tempVec.begin(), tempVec.end(), EditorPointFilter);
			for (int i = 0; i < tempVec.size(); i++)
			{
				ReadInfoStruct IDData = tempVec[i];
				int FilePoint = NameAddressMap[IDData.ValueName];
				if (IDData.Point == FilePoint)
					Print(LV5, FileName, "Field:%s reader:%d file:%d", IDData.ValueName.c_str(), IDData.Point, FilePoint);
				else
					Print(LV5, FileName, "Field:%s reader:%d file:%d @", IDData.ValueName.c_str(), IDData.Point, FilePoint);
			}
		}
		if (false)
		{
			if (NameAddressMap.size() > 0)
			{
				tempVec.clear();
				for (map<string, int>::iterator mapIter = NameAddressMap.begin(); mapIter != NameAddressMap.end(); mapIter++)
				{
					ReadInfoStruct NewData;
					NewData.ValueName = mapIter->first;
					NewData.Point = mapIter->second;
					tempVec.push_back(NewData);
				}
				sort(tempVec.begin(), tempVec.end(), EditorPointFilter);
				Print(LV5, FileName, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
				for (int i = 0; i < tempVec.size(); i++)
				{
					ReadInfoStruct IDData = tempVec[i];
					Iter = ReadInfo->find(IDData.ValueName);
					if (Iter ==  ReadInfo->end())
						Print(LV5, FileName, "Field:%s file:%d #", IDData.ValueName.c_str(), IDData.Point);
					else
						Print(LV5, FileName, "Field:%s file:%d", IDData.ValueName.c_str(), IDData.Point);
				}
			}
		}
		Print(LV5, FileName, "-------------------------------------------");
	}

	return true;
};

template <class T, class FileClass>
void ObjectDataFileClass<T, FileClass>::PrintEditorPoint(map<string, ReadInfoStruct> *ReadInfo)
{
	std::vector<ReadInfoStruct> vec;
	for (Iter = ReadInfo->begin(); Iter != ReadInfo->end(); Iter++)
	{
		vec.push_back(Iter->second);
	}
	sort(vec.begin(), vec.end(), EditorPointFilter);
	std::vector<ReadInfoStruct> temp;
	for (int i = 0; i < (int)vec.size() - 1; i++)
	{
		ReadInfoStruct curr = vec[i];
		ReadInfoStruct next = vec[i + 1];

		int currStartPoint = curr.Point;
		int currEndPoint = currStartPoint + curr.RealSize;

		int nextStartPoint = next.Point;
		if (currEndPoint != nextStartPoint)
		{
			int size = nextStartPoint - currEndPoint;
			ReadInfoStruct NewData;
			NewData.Point = currEndPoint;
			NewData.Type = ENUM_Type_FORCE_DWORD;
			NewData.AddressFlag = ENUM_Indirect_Address;
			NewData.UserData = 0;
			NewData.Size = 0;
			NewData.ValueName = "unknown";
			NewData.ID = -1;
			NewData.RealSize = size;
			temp.push_back(NewData);
		}
	}

	for (int i = 0; i < (int)temp.size(); i++)
	{
		vec.push_back(temp[i]);
	}

	char *pch;
	pch = strtok(Head.FileInfoStr, " ,.-");
	char *res;
	while (pch != NULL)
	{
		res = pch;
		pch = strtok(NULL, " ,.-");
	}
	string resultFileName = res;

	std::transform(resultFileName.begin(), resultFileName.end(), resultFileName.begin(), ::tolower);
	size_t f = resultFileName.find("db");
	resultFileName.replace(f, std::string("db").length(), ".db");

	sort(vec.begin(), vec.end(), EditorPointFilter);
	for (int i = 0; i < (int)vec.size(); i++)
	{
		ReadInfoStruct *IDData = &vec[i];
		if (IDData->Point > 70000)
			continue;
		int type = -1;
		switch (IDData->Type)
		{
		case ENUM_Type_IntPoint:
		{
			type = 1;
			break;
		}
		case ENUM_Type_FloatPoint:
		{
			type = 3;
			break;
		}
		case ENUM_Type_CharString:
		{
			type = 5;
			break;
		}
		case ENUM_Type_BitPoint:
		{
			type = 4;
			break;
		}
		case ENUM_Type_CharStringPoint:
		{
			type = 4;
			break;
		}
		case ENUM_Type_FORCE_DWORD:
		{
			type = 4;
			break;
		}
		default:
		{
			type = 4;
			break;
		}
		}
		std::string colStr = IDData->ValueName;
		std::string fileStr = Head.FileInfoStr;
		std::transform(colStr.begin(), colStr.end(), colStr.begin(), ::tolower);
		std::transform(fileStr.begin(), fileStr.end(), fileStr.begin(), ::tolower);
		fileStr.replace(0, 5, "");
		std::cout << resultFileName.c_str() << ";";
		std::cout << colStr.c_str() << ";";
		std::cout << std::hex << std::setw(4) << std::setfill('0') << IDData->Point << ";" << std::dec;
		std::cout << std::hex << std::setw(2) << std::setfill('0') << IDData->RealSize << ";" << std::dec;
		std::cout << type << ";";
		std::cout << std::endl;
	}
}

template <class T, class FileClass>
void ObjectDataFileClass<T, FileClass>::SetData(T *Data)
{
	ObjBaseStruct<T> *TempObjBase = NEW ObjBaseStruct<T>;
	TempObjBase->FileInfo = NULL;
	TempObjBase->Obj = NEW(T);
	TempObjBase->IsLoad = true;
	_ObjData[D1->GUID] = TempObjBase;

	T *NewData = TempObjBase->Obj;
	memcpy(NewData, Data, sizeof(T));

	_SourceData.push_back((char *)NewData);
}

template <class T, class FileClass>
T *ObjectDataFileClass<T, FileClass>::GetObj(int id)
{
	ObjBaseStruct<T> *ObjBase = GetObjBase(id);
	if (ObjBase == NULL)
		return NULL;

	return ObjBase->Obj;
}

template <class T, class FileClass>
ObjBaseStruct<T> *ObjectDataFileClass<T, FileClass>::GetObjBase(int id)
{
	if (id == 0)
		return NULL;

	map<int, ObjBaseStruct<T> *>::iterator Iter;

	Iter = _ObjData.find(id);
	if (Iter == _ObjData.end())
		return NULL;

	ObjBaseStruct<T> *ObjInfo = Iter->second;

	if (ObjInfo->IsLoad == false)
	{
		_NeedFreeCount++;

		ObjInfo->FileInfo->File->Seek(ObjInfo->Seek, EM_FileSeek_Begin);

		int ObjSize = ObjInfo->FileInfo->FileHead.Size;
		ObjInfo->Obj = (T *)NEW char[ObjSize];
		ObjInfo->FileInfo->File->Read(ObjInfo->Obj, ObjSize);

		ObjInfo->Mode = 0;
		ObjInfo->IsNeedFree = true;
		ObjInfo->IsLoad = true;
		ObjInfo->AccessCount = 2;
	}
	if (ObjInfo->IsInit == false)
	{
		if (_InitObjFunc)
			_InitObjFunc(_InitUserObj, ObjInfo->Obj);
		ObjInfo->IsInit = true;
	}

	ObjInfo->AccessCount++;

	return ObjInfo;
}

template <class T, class FileClass>
void ObjectDataFileClass<T, FileClass>::Process(int MaxObjCount)
{
	if (MaxObjCount > _NeedFreeCount)
		return;

	map<int, ObjBaseStruct<T> *>::iterator Iter;

	int MaxDeleteCount = MaxObjCount / 3;

	for (Iter = _ObjData.begin(); Iter != _ObjData.end(); Iter++)
	{
		if (MaxDeleteCount <= 0)
			break;

		ObjBaseStruct<T> *ObjBase = Iter->second;

		if (ObjBase->IsLoad == false || ObjBase->IsNeedFree == false)
			continue;

		if (ObjBase->AccessCount < 2)
		{
			delete ObjBase->Obj;
			ObjBase->Obj = NULL;
			ObjBase->Mode = 0;
			MaxDeleteCount--;
			_NeedFreeCount--;
		}
		else
		{
			Iter->second->AccessCount = 0;
		}
	}
}

template <class T, class FileClass>
T *ObjectDataFileClass<T, FileClass>::GetDataByOrder(bool IsResetData)
{
	if (IsResetData != false)
	{
		_SearchIter = _ObjData.begin();
		return NULL;
	}

	std::map<int, ObjBaseStruct<T> *>::iterator TempIter;

	if (_SearchIter == _ObjData.end())
		return NULL;

	TempIter = _SearchIter++;

	if (TempIter == _ObjData.end())
		return NULL;

	if (TempIter->second->IsLoad == false)
		return NULL;

	return TempIter->second->Obj;
}
} // namespace NSObjectDataFile
#pragma managed
