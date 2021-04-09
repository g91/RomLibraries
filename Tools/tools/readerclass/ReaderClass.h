#ifndef	__READERCLASS_H___
#define __READERCLASS_H___
#include <algorithm>
#include "malloc/myalloc.h"
#include "iostream"
#pragma	warning (disable:4311)
#pragma	warning (disable:4312)
#pragma warning (disable:4949)
#pragma warning (disable:4996)
#pragma unmanaged
#define Def_Disabled_Update_Flag 0x01
#define Def_Disabled_Insert_Flag 0x02
#define Def_Disabled_Select_Flag 0x04
#include <map>
#include <vector>
#include <string>
enum ReadInfoTypeENUM
{
	ENUM_Type_FloatPoint = 0,
	ENUM_Type_IntPoint,
	ENUM_Type_ShortPoint,
	ENUM_Type_CharPoint,
	ENUM_Type_BitPoint,
	ENUM_Type_SmallDateTime,
	ENUM_Type_CharString,
	ENUM_Type_WCharString,	
	ENUM_Type_WCharStringBase,
	ENUM_Type_BinaryData,      
	ENUM_Type_TextData,
	ENUM_Type_TimeCharString,
	ENUM_Type_FloatPointPoint,
	ENUM_Type_IntPointPoint,
	ENUM_Type_ShortPointPoint,
	ENUM_Type_CharPointPoint,
	ENUM_Type_CharStringPoint,
	ENUM_Type_TextDataPoint,
	ENUM_Type_Float,
	ENUM_Type_Int,
	ENUM_Type_Short,
	ENUM_Type_Char,
	ENUM_Type_Max,
	ENUM_Type_FORCE_DWORD = 0x7fffffff,
};
enum ReadInfoAddrTypeENUM
{
	ENUM_Direct_Address = 0,
	ENUM_Indirect_Address,
	ENUM_Addr_FORCE_DWORD = 0x7fffffff,
};
using	namespace  std;
struct ReadInfoStruct
{
	int ID;
	string ValueName;
	int Point;
	ReadInfoTypeENUM Type;
	ReadInfoAddrTypeENUM AddressFlag;
	int UserData;
	int Size;
	int RealSize;
};
template<class Temp>
class ReaderClass
{
	map<string, ReadInfoStruct> _ReadInfo;
	vector<string> _ArgList;
	vector< ReadInfoStruct > _ListByType[ENUM_Type_Max];
public:
	ReaderClass() {};
	~ReaderClass();
	map<string, ReadInfoStruct>* ReadInfo() { return &_ReadInfo; };
	ReadInfoStruct* GetColumnInfo(string ValueName);
	bool CheckStringType(string ValueName);
	int Read(Temp* Obj, string ValueName);
	double Read_Double(Temp* Obj, string ValueName);
	bool Write(Temp* Obj, string ValueName, double Value);
	char* ReadStr(Temp* Obj, string ValueName);
	bool WriteStr(Temp* Obj, string ValueName, const char* Str);
	vector<ReadInfoStruct >& ListByType(ReadInfoTypeENUM Type) { return _ListByType[Type]; };
	bool SetData(string ValueName, const void* Point, ReadInfoTypeENUM Type, int Size = 0, ReadInfoAddrTypeENUM Flag = ENUM_Indirect_Address, int UserData = 0, int RealSize = 0);
	void TransferWChartoUTF8(Temp* Obj, vector< wchar_t*> Data);
	void PrintReader();
	static bool EditorPointFilter(const ReadInfoStruct& a, const ReadInfoStruct& b);
	void PrintPointerInfo();
	void PrintReaderInfo(string file, string dbName);
};
template<class Temp>
ReaderClass<Temp>::~ReaderClass()
{
}
template<class Temp>
bool ReaderClass<Temp>::SetData(string	ValueName, const void* Point, ReadInfoTypeENUM Type, int Size, ReadInfoAddrTypeENUM Flag, int UserData, int RealSize)
{
	std::transform(ValueName.begin(), ValueName.end(), ValueName.begin(), ::tolower);
	ReadInfoStruct NewData;
	NewData.Point = (int)Point;
	NewData.Type = Type;
	NewData.AddressFlag = Flag;
	NewData.UserData = UserData;
	NewData.Size = Size;
	NewData.ValueName = ValueName;
	NewData.ID = (int)_ReadInfo.size();
	NewData.RealSize = RealSize;
	_ReadInfo[ValueName] = NewData;
	_ArgList.push_back(ValueName);
	_ListByType[Type].push_back(_ReadInfo[ValueName]);
	return true;
}
template<class Temp>
bool ReaderClass<Temp>::EditorPointFilter(const ReadInfoStruct& a, const ReadInfoStruct& b)
{
	return a.Point < b.Point;
}
template<class Temp>
void ReaderClass<Temp>::PrintReader()
{
	std::vector<ReadInfoStruct> tempVec;
	map<string, ReadInfoStruct>::iterator Iter;
	for (Iter = _ReadInfo.begin(); Iter != _ReadInfo.end(); Iter++)
	{
		tempVec.push_back(Iter->second);
	}
	sort(tempVec.begin(), tempVec.end(), EditorPointFilter);
	for (int i = 0; i < (int)tempVec.size(); i++)
	{
		Print(LV5, "Reader ", "%s %d", tempVec[i].ValueName.c_str(), tempVec[i].Point);
	}
}
template<class Temp>
int ReaderClass<Temp>::Read(Temp* Obj, string	ValueName)
{
	std::transform(ValueName.begin(), ValueName.end(), ValueName.begin(), ::tolower);
	map<string, ReadInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find(ValueName);
	if (Iter == _ReadInfo.end())
		return 0;
	ReadInfoStruct* IDData;
	IDData = &(Iter->second);
	if (IDData->AddressFlag == ENUM_Direct_Address)
	{
		switch (IDData->Type)
		{
		case ENUM_Type_FloatPoint:
			return (int) * ((float*)IDData->Point);
		case ENUM_Type_IntPoint:
			return (int) * ((int*)IDData->Point);
		case ENUM_Type_ShortPoint:
			return (int) * ((short*)IDData->Point);
		case ENUM_Type_CharPoint:
			return (int) * ((char*)IDData->Point);
		case ENUM_Type_BitPoint:
			return (int) * ((bool*)IDData->Point);
		case ENUM_Type_SmallDateTime:
			return (int) * ((float*)IDData->Point);
		case ENUM_Type_FloatPointPoint:
			return (int) * *((float**)IDData->Point);
		case ENUM_Type_IntPointPoint:
			return (int) * *((int**)IDData->Point);
		case ENUM_Type_ShortPointPoint:
			return (int) * *((short**)IDData->Point);
		case ENUM_Type_CharPointPoint:
			return (int) * *((char**)IDData->Point);
		}
	}
	else if (IDData->AddressFlag == ENUM_Indirect_Address)
	{
		switch (IDData->Type)
		{
		case ENUM_Type_FloatPoint:
			return (int) * ((float*)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_IntPoint:
			return (int) * ((int*)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_SmallDateTime:
			return (int) * ((float*)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_ShortPoint:
			return (int) * ((short*)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_CharPoint:
			return (int) * ((char*)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_BitPoint:
			return (int) * ((bool*)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_FloatPointPoint:
			return (int) * *((float**)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_IntPointPoint:
			return (int) * *((int**)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_ShortPointPoint:
			return (int) * *((short**)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_CharPointPoint:
			return (int) * *((char**)(IDData->Point + UINT64(Obj)));
		}
	}
	return 0;
}
template<class Temp>
double ReaderClass<Temp>::Read_Double(Temp* Obj, string	ValueName)
{
	std::transform(ValueName.begin(), ValueName.end(), ValueName.begin(), ::tolower);
	map<string, ReadInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find(ValueName);
	if (Iter == _ReadInfo.end())
		return 0;
	ReadInfoStruct* IDData;
	IDData = &(Iter->second);
	if (IDData->AddressFlag == ENUM_Direct_Address)
	{
		switch (IDData->Type)
		{
		case ENUM_Type_FloatPoint:
			return (double) * ((float*)IDData->Point);
		case ENUM_Type_IntPoint:
			return (double) * ((int*)IDData->Point);
		case ENUM_Type_ShortPoint:
			return (double) * ((short*)IDData->Point);
		case ENUM_Type_CharPoint:
			return (double) * ((char*)IDData->Point);
		case ENUM_Type_BitPoint:
			return (double) * ((bool*)IDData->Point);
		case ENUM_Type_SmallDateTime:
			return (double) * ((float*)IDData->Point);
		case ENUM_Type_FloatPointPoint:
			return (double) * *((float**)IDData->Point);
		case ENUM_Type_IntPointPoint:
			return (double) * *((int**)IDData->Point);
		case ENUM_Type_ShortPointPoint:
			return (double) * *((short**)IDData->Point);
		case ENUM_Type_CharPointPoint:
			return (double) * *((char**)IDData->Point);
		}
	}
	else if (IDData->AddressFlag == ENUM_Indirect_Address)
	{
		switch (IDData->Type)
		{
		case ENUM_Type_FloatPoint:
			return (double) * ((float*)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_IntPoint:
			return (double) * ((int*)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_SmallDateTime:
			return (double) * ((float*)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_ShortPoint:
			return (double) * ((short*)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_CharPoint:
			return (double) * ((char*)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_BitPoint:
			return (double) * ((bool*)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_FloatPointPoint:
			return (double) * *((float**)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_IntPointPoint:
			return (double) * *((int**)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_ShortPointPoint:
			return (double) * *((short**)(IDData->Point + UINT64(Obj)));
		case ENUM_Type_CharPointPoint:
			return (double) * *((char**)(IDData->Point + UINT64(Obj)));
		}
	}
	return 0;
}
template<class Temp>
bool ReaderClass<Temp>::Write(Temp* Obj, string ValueName, double Value)
{
	std::transform(ValueName.begin(), ValueName.end(), ValueName.begin(), ::tolower);
	map<string, ReadInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find(ValueName);
	if (Iter == _ReadInfo.end())
		return false;
	ReadInfoStruct* IDData;
	IDData = &(Iter->second);
	if (IDData->AddressFlag == ENUM_Direct_Address)
	{
		switch (IDData->Type)
		{
		case ENUM_Type_FloatPoint:
			*((float*)IDData->Point) = Value;
			break;
		case ENUM_Type_IntPoint:
			*((int*)IDData->Point) = (int)Value;
			break;
		case ENUM_Type_ShortPoint:
			*((short*)IDData->Point) = (int)Value;
			break;
		case ENUM_Type_CharPoint:
			*((char*)IDData->Point) = (int)Value;
			break;
		case ENUM_Type_BitPoint:
			*((bool*)IDData->Point) = Value != 0;
			break;
		case ENUM_Type_SmallDateTime:
			*((float*)IDData->Point) = (float)Value;
			break;
		case ENUM_Type_FloatPointPoint:
			**((float**)IDData->Point) = (float)Value;
			break;
		case ENUM_Type_IntPointPoint:
			**((int**)IDData->Point) = (int)Value;
			break;
		case ENUM_Type_ShortPointPoint:
			**((short**)IDData->Point) = (int)Value;
			break;
		case ENUM_Type_CharPointPoint:
			**((char**)IDData->Point) = (int)Value;
			break;
		default:
			return false;
		}
	}
	else if (IDData->AddressFlag == ENUM_Indirect_Address)
	{
		switch (IDData->Type)
		{
		case ENUM_Type_FloatPoint:
			*((float*)(IDData->Point + UINT64(Obj))) = (float)Value;
			break;
		case ENUM_Type_IntPoint:
			*((int*)(IDData->Point + UINT64(Obj))) = (int)Value;
			break;
		case ENUM_Type_ShortPoint:
			*((short*)(IDData->Point + UINT64(Obj))) = (int)Value;
			break;
		case ENUM_Type_CharPoint:
			*((char*)(IDData->Point + UINT64(Obj))) = (int)Value;
			break;
		case ENUM_Type_BitPoint:
			*((bool*)(IDData->Point + UINT64(Obj))) = Value != 0;
			break;
		case ENUM_Type_SmallDateTime:
			*((float*)(IDData->Point + UINT64(Obj))) = (float)Value;
			break;
		case ENUM_Type_FloatPointPoint:
			**((float**)(IDData->Point + UINT64(Obj))) = (float)Value;
			break;
		case ENUM_Type_IntPointPoint:
			**((int**)(IDData->Point + UINT64(Obj))) = (int)Value;
			break;
		case ENUM_Type_ShortPointPoint:
			**((short**)(IDData->Point + UINT64(Obj))) = (int)Value;
			break;
		case ENUM_Type_CharPointPoint:
			**((char**)(IDData->Point + UINT64(Obj))) = (int)Value;
			break;
		default:
			return false;
		}
	}
	return true;
}
template<class Temp>
char* ReaderClass<Temp>::ReadStr(Temp* Obj, string ValueName)
{
	std::transform(ValueName.begin(), ValueName.end(), ValueName.begin(), ::tolower);
	map<string, ReadInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find(ValueName);
	if (Iter == _ReadInfo.end())
		return NULL;
	ReadInfoStruct* IDData;
	IDData = &(Iter->second);
	if (IDData->Type == ENUM_Type_CharString
		|| IDData->Type == ENUM_Type_WCharString
		|| IDData->Type == ENUM_Type_BinaryData)
	{
		if (IDData->AddressFlag == ENUM_Direct_Address)
		{
			return ((char*)IDData->Point);
		}
		else if (IDData->AddressFlag == ENUM_Indirect_Address)
		{
			return ((char*)(IDData->Point + UINT64(Obj)));
		}
	}
	else if (IDData->Type == ENUM_Type_CharStringPoint
		|| IDData->Type == ENUM_Type_TextDataPoint)
	{
		if (IDData->AddressFlag == ENUM_Direct_Address)
		{
			return *((char**)IDData->Point);
		}
		else if (IDData->AddressFlag == ENUM_Indirect_Address)
		{
			return *((char**)(IDData->Point + UINT64(Obj)));
		}
	}
	return NULL;
}
template<class Temp>
bool ReaderClass<Temp>::WriteStr(Temp* Obj, string ValueName, const char* Str)
{
	std::transform(ValueName.begin(), ValueName.end(), ValueName.begin(), ::tolower);
	map<string, ReadInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find(ValueName);
	if (Iter == _ReadInfo.end())
		return false;
	ReadInfoStruct* IDData;
	IDData = &(Iter->second);
	if (IDData->Type == ENUM_Type_CharString
		|| IDData->Type == ENUM_Type_WCharString
		|| IDData->Type == ENUM_Type_TextData)
	{
		if (IDData->AddressFlag == ENUM_Direct_Address)
		{
			strcpy(((char*)IDData->Point), Str);
		}
		else if (IDData->AddressFlag == ENUM_Indirect_Address)
		{
			strcpy(((char*)(IDData->Point + UINT64(Obj))), Str);
		}
		return true;
	}
	else if (IDData->Type == ENUM_Type_CharStringPoint
		|| IDData->Type == ENUM_Type_TextDataPoint)
	{
		if (IDData->AddressFlag == ENUM_Direct_Address)
		{
			strcpy(*((char**)IDData->Point), Str);
		}
		else if (IDData->AddressFlag == ENUM_Indirect_Address)
		{
			strcpy(*((char**)(IDData->Point + UINT64(Obj))), Str);
		}
		return true;
	}
	return false;
}
template<class Temp>
ReadInfoStruct* ReaderClass<Temp>::GetColumnInfo(string ValueName)
{
	std::transform(ValueName.begin(), ValueName.end(), ValueName.begin(), ::tolower);
	map<string, ReadInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find(ValueName);
	if (Iter == _ReadInfo.end())
		return NULL;
	return &(Iter->second);
}
template<class Temp>
bool ReaderClass<Temp>::CheckStringType(string ValueName)
{
	std::transform(ValueName.begin(), ValueName.end(), ValueName.begin(), ::tolower);
	map<string, ReadInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find(ValueName);
	if (Iter == _ReadInfo.end())
		return false;
	ReadInfoStruct* IDData;
	IDData = &(Iter->second);
	if (IDData->Type != ENUM_Type_CharString
		&& IDData->Type != ENUM_Type_TextData
		&& IDData->Type != ENUM_Type_TextDataPoint)
		return false;
	return true;
}
template<class Temp>
void ReaderClass<Temp>::TransferWChartoUTF8(Temp* Obj, vector<wchar_t*> Data)
{
	vector< ReadInfoStruct >& ReadInfoList = _ListByType[ENUM_Type_WCharString];
	char* utf8Str = NULL;
	for (unsigned int i = 0; i < ReadInfoList.size(); i++)
	{
		ReadInfoStruct* IDData = &ReadInfoList[i];
		if (IDData->AddressFlag == ENUM_Direct_Address)
		{
			utf8Str = ((char*)IDData->Point);
		}
		else if (IDData->AddressFlag == ENUM_Indirect_Address)
		{
			utf8Str = ((char*)(IDData->Point + UINT64(Obj)));
		}
		WCharToUTF8(Data[IDData->ID], utf8Str, IDData->Size);
	}
}
template<class Temp>
void ReaderClass<Temp>::PrintPointerInfo()
{
	std::vector<ReadInfoStruct> tempVec;
	map<string, ReadInfoStruct>::iterator Iter;
	for (Iter = _ReadInfo.begin(); Iter != _ReadInfo.end(); Iter++)
	{
		tempVec.push_back(Iter->second);
	}
	sort(tempVec.begin(), tempVec.end(), EditorPointFilter);
	for (int i = 0; i < (int)tempVec.size(); i++)
	{
		std::cout << tempVec[i].ValueName.c_str() << ";" << tempVec[i].Point << ";" << tempVec[i].Type << ";" << tempVec[i].RealSize << std::endl;
	}
}
template<class Temp>
void ReaderClass<Temp>::PrintReaderInfo(string file, string dbName)
{
	map<ReadInfoTypeENUM, int> typeMap;
	//typeMap[] = 0; // byte array
	typeMap[ENUM_Type_IntPoint] = 1; // int
	//typeMap[] = 2; // uint
	typeMap[ENUM_Type_FloatPoint] = 3; // float
	typeMap[ENUM_Type_BitPoint] = 4; // bool
	typeMap[ENUM_Type_CharString] = 5; // char array
	map<ReadInfoTypeENUM, int>::iterator typeIter;
	

	std::vector<ReadInfoStruct> sortInfo;
	map<string, ReadInfoStruct>::iterator Iter;
	for (Iter = _ReadInfo.begin(); Iter != _ReadInfo.end(); Iter++)
	{
		sortInfo.push_back(Iter->second);
	}
	sort(sortInfo.begin(), sortInfo.end(), EditorPointFilter);

	std::ofstream s;
	s.open(file.c_str(), std::ios_base::app );
	if (s.is_open())
	{
		for (int i = 0; i < (int)sortInfo.size(); i++)
		{
			ReadInfoStruct info = sortInfo[i];
			if (info.Point > 70000)
				continue;
			int fieldType = 0;
			typeIter = typeMap.find(info.Type);
			if (typeIter != typeMap.end())
			{
				fieldType = typeIter->second;
			}
			char Buf[1024];
			sprintf_s(Buf, sizeof(Buf), "%s;%s;%04x;%02x;%d;", dbName.c_str(), info.ValueName.c_str(), info.Point, info.RealSize, fieldType);
			s << Buf << std::endl;
		}
	}
	s.close();
}

#pragma managed
#endif 