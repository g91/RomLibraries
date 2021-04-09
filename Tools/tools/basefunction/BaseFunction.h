#ifndef _BASEFUNCTION_H_2003_09_22
#define _BASEFUNCTION_H_2003_09_22
#pragma warning (disable:4949)
#pragma unmanaged
#include <string>
#include <vector>
#include <excpt.h>
#include <stdio.h>
using namespace std;
#define SetBit(x,b) ((x) |= (1<<(b)))
#define TestBit(x,b) (((x) & (1<<(b))) != 0 ) 
unsigned HexStrToUInt(const char* valueStr);
char* CombineFilePath(const char* Path, const char* File);
void FilePathnormalize(char* FilePath, char* OutPath);
bool CheckFloatAvlid(float X);
string FloatToStr(float Value);
string IntToStr(int Value);
string UInt64ToStr(unsigned __int64 Value);
int StrToNum(const char* Str, int Size);
unsigned int MytimeGetTime();
vector<string>* AllPathList(string Path);
vector<string>* AllPathList_Pure(string Path);
vector<string>* AllFileList(string Path, string Sub);
vector<string>* AllFileList_Sub(string Path, string Sub);
bool CreateAllPath(string Path, vector<string>& PathList);
bool FloatEQU(float X, float Y);
string StringToSqlX(const char* Str, int Size, bool IsNullEnd = true);
string WStringToSqlX(const wchar_t* Str, int Size);
string StringToSqlStr(const char* Str, int Size, bool IsNullEnd = true);
bool CheckSqlStr(const char* Str);
string	SqlStr_AddSingleQuotationMark(const char* Str);
int Random(int);
int Random(int Min, int Max);
int RangeValue(int MinValue, int MaxValue, int Value);
char* EncodePassword(const char* Password, const char* Key);
char* DecodePassword(const char* Password, const char* Key);
int ExceptionFilter(unsigned int code, struct _EXCEPTION_POINTERS* ep);
int MathCombination(int TotalCount, int SelectCount);
bool CheckDoubleChar(const char* Str);
int WCharToUTF8(const wchar_t* ws, char* StrOut, int BufSize);
float TimeExchangeIntToFloat(int Time);
int TimeExchangeFloatToInt(float Time);
float ToRoundOff(float);
int MAtoi(char* Str);
int GetNum(char* Str, int index);
char* GetStr(char* Str, int index);
bool Com_CreatePath(const char* szPathName);
void MyStrcpy(char* strDest, const char* StrSrc, int DestBufSize);
template< class T >
T	GetVectorPos(vector<T>& v, int Pos, T ErrorRet)
{
	if (unsigned(Pos) >= v.size())
		return ErrorRet;
	return v[Pos];
};
template< class T >
T* GetVectorPosPoint(vector<T>& v, int Pos)
{
	if (unsigned(Pos) >= v.size())
		return NULL;
	return&v[Pos];
};
float RandFloat();
int	GetMemoryUsage(int ProcessID);
unsigned long GetHashCode(const void* vk, int length);
bool CheckTwoLineIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
#pragma managed
#endif