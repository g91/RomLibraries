#ifndef _RECYCLEBIN_
#define _RECYCLEBIN_
#include <vector>
#pragma unmanaged
//---------------------------------------------------------------------------
/**
 * @file recyclebin.h 
 * 處理物件記憶體配置與回收\n
 * 加速固定結構記憶體配置與釋放\n
 * Last update [9/9/2004]
 * @author: hsiangren@runewaker.com.tw
 */
//---------------------------------------------------------------------------


//----------------------------------------------------------------------------------
//資源回收物件
//----------------------------------------------------------------------------------
#define _RecycleBin_MaxSize_		0xffff	///<最大空間配置大小
#define _RecycleBin_PackageBlock_	100		///<設定配置記憶體切割成多少個區塊
#define _RecycleBin_MemCheckData	0xccddeeff

using namespace std;

template<class Temp>
class RecycleBin
{
    struct BaseStruct
    {
        int     CheckBegin;
        Temp    Data;
        int     CheckEnd;
        BaseStruct()
        {
            CheckBegin  = NULL;
            CheckEnd    = NULL;
        }
    };
	//初始化最大Size;
	int				_MaxSize;
	//暫存區(尚可配置區)
	vector <Temp *> _UnUsed;
	vector <Temp *> _AllData;

	vector <BaseStruct *> _AllocList;
	
    int             _MemGrowUPSize;
	//向系統要記憶體
	void _Alloc();

public:
	///建構函式
	///設定資源最多會要幾個
	///@param MaxSize 設定資源最多可配置量
	RecycleBin(int MaxSize = _RecycleBin_MaxSize_);
	~RecycleBin();

	///把所有的資源釋放掉，成為初始狀態
	void	Clear();

    ///回傳剩下未用到的記憶體大小
	///@return 配置未使用的物件各數
    int     MemSize()const {return _UnUsed.size();};

	///配置新記憶體
	///@return 配置到的物件位址 
	Temp*	NewObj();

	///回收記憶體
	///@param DelObj 要釋放的物件指標 
	void	DeleteObj(Temp *DelObj);

	///檢查所有的記憶體
	//@return ture OK false 記憶體出問題
	bool	CheckAllMem();

    ///設定成長大小
    void    SetMemGrowUpSize( int Size ){ _MemGrowUPSize = _Size; };
};
//----------------------------------------------------------------------------------
//
//					Source Code
//
//----------------------------------------------------------------------------------
template<class Temp>
Temp* RecycleBin<Temp>::NewObj()
{
	if(_UnUsed.size()==0)
	{
		_Alloc();
		if(_UnUsed.size()==0)
			return NULL;
	}
	
	
	Temp *Ret=_UnUsed[_UnUsed.size()-1];
	_UnUsed.pop_back();

	return Ret;
}
//----------------------------------------------------------------------------------
template<class Temp>
void RecycleBin<Temp>::DeleteObj(Temp *Obj)
{
	if(Obj==NULL)
		return;

	_UnUsed.push_back(Obj);	
}
//----------------------------------------------------------------------------------
template<class Temp>
RecycleBin<Temp>::RecycleBin(int MaxSize)
{
	_MaxSize=MaxSize;
    _MemGrowUPSize = 100;
}
//----------------------------------------------------------------------------------
template<class Temp>
RecycleBin<Temp>::~RecycleBin()
{
	Clear();

}
//----------------------------------------------------------------------------------
template<class Temp>
void RecycleBin<Temp>::_Alloc()
{
	int i;
	BaseStruct*		_AllocMemory;
	Temp*			_Addr;

	if(_RecycleBin_PackageBlock_ <= _AllocList.size())
		return;
	
    //_AllocMemory = new BaseStruct[ ( sizeof(Temp) + 8 ) * ( _MaxSize/_RecycleBin_PackageBlock_+1 ) ];
    _AllocMemory = new BaseStruct[ _MemGrowUPSize ];

	_AllocList.push_back(_AllocMemory);

	//for(i=0;i<=_MaxSize/_RecycleBin_PackageBlock_;i++)
    for(i=0;i<_MemGrowUPSize;i++)
	{
		_Addr = &_AllocMemory[i].Data;
		_UnUsed.push_back( _Addr );
		_AllData.push_back( _Addr );
	}

}
//----------------------------------------------------------------------------------
template<class Temp>
void RecycleBin<Temp>::Clear()
{
	int i;
	for(i=0;i<(int)_AllocList.size();i++)
		delete[] (_AllocList[i]);

	_UnUsed.clear();
	_AllocList.clear();
	_AllData.clear();

}
//----------------------------------------------------------------------------------	
///檢查所有的記憶體
template<class Temp>
bool RecycleBin<Temp>::CheckAllMem()
{
	BaseStruct*		Data;
	int		i;
	for( i = 0 ; i < (int)_AllData.size(); i++ )
	{
		Data = (BaseStruct*)( ((char*)(_AllData[i])) - 4 );
        if(     Data->CheckBegin != _RecycleBin_MemCheckData 
            ||  Data->CheckEnd   != _RecycleBin_MemCheckData )
            return false;
	}

	return true;
}
#pragma managed
#endif